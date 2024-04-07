#include "rex_engine/filesystem/vfs.h"

#include "rex_engine/cmdline/cmdline.h"
#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/logging/log_verbosity.h"
#include "rex_engine/engine/state_controller.h"
#include "rex_engine/engine/project.h"
#include "rex_engine/filesystem/directory.h"
#include "rex_engine/filesystem/file.h"
#include "rex_engine/filesystem/path.h"
#include "rex_engine/platform/win/diagnostics/win_call.h"
#include "rex_engine/timing/timepoint.h"
#include "rex_std/bonus/atomic/atomic.h"
#include "rex_std/bonus/hashtable.h"
#include "rex_std/bonus/memory.h"
#include "rex_std/bonus/platform.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/time/timepoint.h"
#include "rex_std/bonus/utility/enum_reflection.h"
#include "rex_std/ctype.h"
#include "rex_std/mutex.h"
#include "rex_std/thread.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

// Rex Engine - Virtual File System
// Users can make simple request to read or write to a file
// that run on the same thread
// eg: vfs::read_file and vfs::write_to_file
//
// the vfs also supports async read requests
// this is done by calling vfs::read_file_async
// This will return a read request immediately
// the user is meant to keep this read request
// alive as it'll be signaled when reading has finished
// After which the user can access its buffer
// to process the data it just read
// It works as follows:
//
// rex::vfs::ReadRequest request = rex::vfs::read_file_async("path/to/file");
//
// Do some other code here
// ..
//
//
// Wait for the file to be processed
// request.wait();
//
// const rsl::byte* buffer = request.buffer();
// rsl::memory_size size = request.size();
//
// Do something with the data..
//
//
//
// ASYNC VFS PIPELINE
//
//
// =====================================+=====================================================================================================================
//                                      |
//           USER CODE                  |                                                 INTERNAL CODE
//                                      |
// =====================================+=====================================================================================================================
//                                      |
// The following happens on the thread  |
// the request comes in from            |
//                                      |
// +-------------------------+          |            +----------------------+      +------------------------+      +-------------------------------+
// | User Requests File Read | ---------+----------> | Read Request Created | ---> | Queued Request Created | ---> | Queued Request Added To Queue |
// +-------------------------+          |            +----------------------+      +------------------------+      +-------------------------------+
//                                      |                                                                                         |
//                                      |                                                                                         |
// +----------------+                   |                                +-----------------------------+                          |
// | User Continues | <-----------------+------------------------------- | Return Read Request To User |   <----------------------+
// +----------------+                   |                                +-----------------------------+
//         |                            |
//         |                            |                                               The following happens on the vfs thread
//         |                            |              +----------------------------------------------------------------------------------------------------+
//         |                            |              |                                                                                                    |
//         |                            |              |       +----------------------------------+      +--------------------------+      +-----------+    |
//         |                            |              |       | Queued Request Popped From Queue | ---> | Queued Request Processed | ---> | File Read |    |
//         v                            |              |       +----------------------------------+      +--------------------------+      +-----------+    |
// +-----------------+                  |              |                                                                                        |           |
// | Other User Code |                  |              |                                                                                        |           |
// +-----------------+                  |              |                                                                                        |           |
//        |                             |              |                                                                                        |           |
//        |                             |              |                                                                                        |           |
//        |                             |              |                                                                                        |           |
//        v                             |              |                                                                                        |           |
// +-----------------------------+      |              |                                                                                        |           |
// | Waits for request to finish |      |              |                                                                                        |           |
// +-----------------------------+      |              |                                                                                        |           |
//        |                             |              |                                                                                        |           |
//        |                             |              |                                                                                        |           |
//        |                             |              |                                                                                        |           |
//        v                             |              |                                                                                        |           |
// +---------------------+              |              |                               +-----------------------+                                |           |
// | User Processes Data | <------------+--------------+-------------------------------| Read Request Signaled | <------------------------------+           |
// +---------------------+              |              |                               +-----------------------+                                            |
//                                      |              |                                                                                                    |
//                                      |              +----------------------------------------------------------------------------------------------------+
//

namespace rex
{
  namespace vfs
  {
    DEFINE_LOG_CATEGORY(LogFileSystem);

    enum class VfsState
    {
      NotInitialized = BIT(0),
      Initializing   = BIT(1),
      Running        = BIT(2),
      ShuttingDown   = BIT(3),
      ShutDown       = BIT(4)
    };

    // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables, fuchsia-statically-constructed-objects)

    // This the root where all relative paths will start from
    rsl::medium_stack_string g_root;

    // This controls the state of the vfs
    StateController<VfsState> g_vfs_state_controller(VfsState::NotInitialized);

    // mutices for the asyncronous operation of the vfs
    rsl::mutex g_read_request_mutex;
    rsl::mutex g_closed_request_mutex;

    // queues the vfs uses for its async operations
    rsl::vector<rsl::string_view> g_read_requests_in_order;
    rsl::unordered_map<rsl::string, rsl::unique_ptr<QueuedRequest>> g_read_requests;
    rsl::vector<rsl::unique_ptr<QueuedRequest>> g_closed_requests;

    // mounted roots
    rsl::unordered_map<MountingPoint, rsl::string> g_mounted_roots;

    // threads used by the vfs to perform the async operations
    rsl::thread g_reading_thread;
    rsl::thread g_closing_thread;

    // the project

    // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables, fuchsia-statically-constructed-objects)

    rsl::string_view root()
    {
      return g_root;
    }

    // Returns the root directory of the engine files
    rsl::string_view engine_root()
    {
      return mount_path(MountingPoint::EngineRoot);
    }

    // Returns the root directory of the current project
    rsl::string_view project_root()
    {
      return mount_path(MountingPoint::ProjectRoot);
    }

    // Returns the root for all sessions data
    rsl::string_view sessions_root()
    {
      return mount_path(MountingPoint::SessionsRoot);
    }

    // Returns the root for all sessions data of this project
    rsl::string_view project_sessions_root()
    {
      return mount_path(MountingPoint::ProjectSessionsRoot);
    }

    // Returns the root for all files outputed during this session run (eg. logs)
    rsl::string_view session_data_root()
    {
      return mount_path(MountingPoint::Session);
    }

    class QueuedRequest
    {
    public:
      explicit QueuedRequest(rsl::string_view filepath)
          : m_filepath(filepath)
          , m_requests()
          , m_requests_access_mtx()
          , m_is_done(false)
      {
      }

      void add_request_to_signal(ReadRequest* request)
      {
        const rsl::unique_lock lock(m_requests_access_mtx);
        m_requests.push_back(request);
      }
      void remove_request_to_signal(ReadRequest* request)
      {
        const rsl::unique_lock lock(m_requests_access_mtx);
        auto it = rsl::find(m_requests.cbegin(), m_requests.cend(), request);

        if(it != m_requests.cend())
        {
          m_requests.erase(it);
        }
      }
      void swap_request_to_signal(ReadRequest* original, ReadRequest* newRequest)
      {
        const rsl::unique_lock lock(m_requests_access_mtx);
        auto it = rsl::find(m_requests.begin(), m_requests.end(), original);

        REX_ASSERT_X(it != m_requests.end(), "Request at {} not found as a request to signa", reinterpret_cast<void*>(original)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

        *it = newRequest;
      }

      void signal_requests(const rsl::byte* buffer, rsl::memory_size size)
      {
        const rsl::unique_lock lock(m_requests_access_mtx);

        for(ReadRequest* read_request: m_requests)
        {
          read_request->signal(buffer, size);
        }
      }

      bool all_requests_finished() const
      {
        return m_requests.empty();
      }

      rsl::string_view filepath() const
      {
        return m_filepath;
      }

    private:
      rsl::medium_stack_string m_filepath;
      rsl::vector<ReadRequest*> m_requests;
      rsl::mutex m_requests_access_mtx;
      rsl::atomic<bool> m_is_done;
    };

    ReadRequest::ReadRequest(rsl::string_view filepath, QueuedRequest* queuedRequest)
        : m_filepath(filepath)
        , m_queued_request(queuedRequest)
        , m_is_done(false)
        , m_buffer(nullptr)
        , m_size(0_bytes)
    {
    }

    ReadRequest::ReadRequest(const ReadRequest& other)
        : m_filepath(other.m_filepath)
        , m_queued_request(other.m_queued_request)
        , m_is_done(other.m_is_done)
        , m_buffer(other.m_buffer)
        , m_size(other.m_size)
    {
      m_queued_request->add_request_to_signal(this);
    }

    ReadRequest::ReadRequest(ReadRequest&& other)
        : m_filepath(rsl::exchange(other.m_filepath, ""))
        , m_queued_request(rsl::exchange(other.m_queued_request, nullptr))
        , m_is_done(rsl::exchange(other.m_is_done, false))
        , m_buffer(rsl::exchange(other.m_buffer, nullptr))
        , m_size(rsl::exchange(other.m_size, 0_bytes))
    {
      m_queued_request->swap_request_to_signal(&other, this);
    }

    ReadRequest::~ReadRequest()
    {
      if(m_queued_request)
      {
        m_queued_request->remove_request_to_signal(this);
      }
    }

    ReadRequest& ReadRequest::operator=(const ReadRequest& other) // NOLINT(bugprone-unhandled-self-assignment)
    {
      REX_ASSERT_X(this != &other, "assigning a read request to itself");

      m_queued_request->remove_request_to_signal(this);

      m_filepath       = other.m_filepath;
      m_queued_request = other.m_queued_request;
      m_is_done        = other.m_is_done;
      m_buffer         = other.m_buffer;
      m_size           = other.m_size;

      m_queued_request->add_request_to_signal(this);

      return *this;
    }
    ReadRequest& ReadRequest::operator=(ReadRequest&& other)
    {
      REX_ASSERT_X(this != &other, "moving a read request to itself");

      m_filepath       = rsl::exchange(other.m_filepath, "");
      m_queued_request = rsl::exchange(other.m_queued_request, nullptr);
      m_is_done        = rsl::exchange(other.m_is_done, false);
      m_buffer         = rsl::exchange(other.m_buffer, nullptr);
      m_size           = rsl::exchange(other.m_size, 0_bytes);

      m_queued_request->swap_request_to_signal(&other, this);

      return *this;
    }

    void ReadRequest::signal(const rsl::byte* buffer, rsl::memory_size size)
    {
      m_is_done = true;
      m_buffer  = buffer;
      m_size    = size;
    }

    void ReadRequest::wait() const
    {
      // this should ideally be solved with fibers..
      while(!m_is_done)
      {
        using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
        rsl::this_thread::sleep_for(1ms);
      }
    }

    const rsl::byte* ReadRequest::data() const
    {
      return m_buffer;
    }
    rsl::memory_size ReadRequest::count() const
    {
      return m_size;
    }

    rsl::string_view ReadRequest::filepath() const
    {
      return m_filepath;
    }

    void process_read_requests()
    {
      while(g_vfs_state_controller.has_state(VfsState::Running))
      {
        rsl::unique_lock lock(g_read_request_mutex);
        if(!g_read_requests.empty())
        {
          // get the queued request and remvoe it from the queue,
          // but remove in the reverse order that they got added
          rsl::string_view filepath = g_read_requests_in_order.front();
          g_read_requests_in_order.erase(g_read_requests_in_order.cbegin());
          rsl::unique_ptr<QueuedRequest> request = rsl::move(g_read_requests[filepath]);
          g_read_requests.erase(filepath);

          // we don't need access to the queue anymore, we can unlock its access mutex
          lock.unlock();

          // read the actual file we requested
          memory::Blob buffer = read_file(request->filepath());

          // signal all read requests that this file has now been read
          // it's possible multiple read requests want to access the same file
          // if such requests come in while there's already a request for this file
          // they get added to the original queued request and they now all get notified
          request->signal_requests(buffer.data(), buffer.size());

          // add the closed task to the queue, making sure it stays alive until all requests have finished processing the data
          // This moves ownership of the buffer to the new request in the new queue
          // We do this so that we don't have lock any thread while waiting for the requests to complete
          const rsl::unique_lock closed_req_lock(g_closed_request_mutex);
          g_closed_requests.push_back(rsl::move(request));
        }

        using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
        rsl::this_thread::sleep_for(1ms);
      }
    }

    void wait_for_read_requests()
    {
      while(g_vfs_state_controller.has_state(VfsState::Running))
      {
        const rsl::unique_lock lock(g_closed_request_mutex);
        if(!g_closed_requests.empty())
        {
          // Go over all the queued request and check if their read requests are all done
          for(rsl::unique_ptr<QueuedRequest>& request: g_closed_requests)
          {
            if(request->all_requests_finished())
            {
              request.reset();
            }
          }

          // Remove all requests that are nullptr, aka all requests that have finished
          auto it = rsl::remove(g_closed_requests.begin(), g_closed_requests.end(), nullptr);
          g_closed_requests.erase(it, g_closed_requests.end());
        }

        using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
        rsl::this_thread::sleep_for(1ms);
      }
    }

    void start_threads()
    {
      // This thread processes the read requests
      // A queued request is popped from the queue
      // reads the file specified as the requested file to read
      // signals all the read requests that this file is read
      // and passed in the buffer with its size
      // After which it waits async for all read requests to have finished processing
      // the data passed in to them
      // Finally it passes itself over to the closing thread queue
      // to make sure the data stays alive until all requests have finished
      g_reading_thread = rsl::thread(process_read_requests);

      // this thread goes over all the requests that are currently in flight
      // If they've closed, it'll close these requests and remove them from the queue
      g_closing_thread = rsl::thread(wait_for_read_requests);
    }

    void init(rsl::string_view root)
    {
      g_vfs_state_controller.change_state(VfsState::Initializing);

      // Setting the root directory has the effect that all data will be read relative from this directory
      // This has the same effect as if you would put the working directory to this path
      set_root(cmdline::get_argument("Root").value_or(root));

      // Create the mount points
      mount(MountingPoint::EngineRoot, path::join(vfs::root(), "rex"));
      mount(MountingPoint::ProjectRoot, path::join(vfs::root(), project_name()));
      mount(MountingPoint::SessionsRoot, path::join(vfs::root(), "_sessions"));
      mount(MountingPoint::ProjectSessionsRoot, path::join(mount_path(MountingPoint::SessionsRoot), project_name()));
      mount(MountingPoint::Session, path::join(mount_path(MountingPoint::ProjectSessionsRoot), current_timepoint_str()));

      // Start the worker threads, listening to file IO requests and processing them
      start_threads();

      g_vfs_state_controller.change_state(VfsState::Running);

      // Create the current session root so data generated during this session
      // has somewhere to put itself
      create_dirs(mount_path(MountingPoint::Session));

      REX_INFO(LogFileSystem, "LogFileSystem initialized");
    }

    void set_root(rsl::string_view root)
    {
      if(root.empty())
      {
        g_root = rex::path::cwd();
      }
      else if(path::is_absolute(root))
      {
        g_root = root;
      }
      else
      {
        g_root = path::join(path::cwd(), root);
      }

      REX_ASSERT_X(directory::exists(g_root), "root of vfs is not a directory");
      REX_INFO(LogFileSystem, "LogFileSystem root changed to: {}", g_root);
    }

    void mount(MountingPoint root, rsl::string_view path)
    {
      REX_ASSERT_X(!g_mounted_roots.contains(root), "root {} is already mapped. currently mapped to '{}'", rsl::enum_refl::enum_name(root), g_mounted_roots.at(root));
      g_mounted_roots[root] = rsl::string(path);

      // make sure the mount exists
      create_dir(path);
    }

    void mount_for_session(MountingPoint root, rsl::string_view path)
    {
      REX_ASSERT_X(!g_mounted_roots.contains(root), "root {} is already mapped. currently mapped to '{}'", rsl::enum_refl::enum_name(root), g_mounted_roots.at(root));

      // make sure the mount exists
      rsl::string full_path = path::join(mount_path(MountingPoint::Session), path);
      create_dir(full_path);

      g_mounted_roots[root] = rsl::move(full_path);
    }

    void shutdown()
    {
      g_vfs_state_controller.change_state(VfsState::ShuttingDown);

      g_reading_thread.join();
      g_closing_thread.join();

      g_vfs_state_controller.change_state(VfsState::ShutDown);
    }

    memory::Blob read_file(MountingPoint root, rsl::string_view filepath)
    {
      filepath               = path::remove_quotes(filepath);
      const rsl::string path = path::join(g_mounted_roots.at(root), filepath);
      return read_file(path);
    }

    ReadRequest read_file_async(MountingPoint root, rsl::string_view filepath)
    {
      filepath                            = path::remove_quotes(filepath);
      const rsl::string path = path::join(g_mounted_roots.at(root), filepath);
      return read_file_async(path);
    }

    ReadRequest read_file_async(rsl::string_view filepath)
    {
      const rsl::unique_lock lock(g_read_request_mutex);

      // create the queued request, at this point it doesn't hold any signals it should fire on finish
      auto it = g_read_requests.find(filepath);

      // If we already have a request for this file, add a new request to signal to the queued request
      if(it != g_read_requests.end())
      {
        ReadRequest request(filepath, it->value.get());
        it->value->add_request_to_signal(&request);
        return request;
      }

      // If we don't have a request for this file yet, create a new one and add it to the hash table
      rsl::unique_ptr<QueuedRequest> queued_request = rsl::make_unique<QueuedRequest>(filepath);
      ReadRequest request(filepath, queued_request.get());

      // create the read request, which holds a link to the queued request
      queued_request->add_request_to_signal(&request);

      auto emplace_res = g_read_requests.emplace(rsl::string(filepath), rsl::move(queued_request));
      g_read_requests_in_order.push_back(emplace_res.inserted_element->key);

      return request;
    }

    bool save_to_file(MountingPoint root, rsl::string_view filepath, const void* data, card64 size, AppendToFile shouldAppend)
    {
      filepath = path::remove_quotes(filepath);

      const rsl::string path = path::join(g_mounted_roots.at(root), filepath);
      return save_to_file(path, data, size, shouldAppend);
    }

    bool save_to_file(MountingPoint root, rsl::string_view filepath, const memory::Blob& blob, AppendToFile shouldAppend)
    {
      filepath = path::remove_quotes(filepath);

      const rsl::string path = path::join(g_mounted_roots.at(root), filepath);
      return save_to_file(path, blob.data(), blob.size(), shouldAppend);
    }

    bool save_to_file(rsl::string_view filepath, const memory::Blob& blob, AppendToFile shouldAppend)
    {
      return save_to_file(filepath, blob.data(), blob.size(), shouldAppend);
    }

    bool create_dir(MountingPoint root, rsl::string_view path)
    {
      path = path::remove_quotes(path);

      const rsl::string filepath = path::join(g_mounted_roots.at(root), path);
      return create_dir(filepath);
    }

    bool exists(MountingPoint root, rsl::string_view path)
    {
      path = path::remove_quotes(path);

      const rsl::string filepath = path::join(g_mounted_roots.at(root), path);
      return directory::exists(filepath);
    }

    bool is_dir(MountingPoint root, rsl::string_view path)
    {
      path = path::remove_quotes(path);

      const rsl::string filepath = path::join(g_mounted_roots.at(root), path);
      return directory::exists(filepath);
    }
    bool is_file(MountingPoint root, rsl::string_view path)
    {
      path = path::remove_quotes(path);

      const rsl::string filepath = path::join(g_mounted_roots.at(root), path);
      return file::exists(filepath);
    }

    rsl::string create_full_path(MountingPoint root, rsl::string_view path)
    {
      REX_ASSERT_X(g_vfs_state_controller.has_state(VfsState::Running), "Trying to use vfs before it's initialized");
      REX_ASSERT_X(!path::is_absolute(path), "Passed an absolute path into a function that doesn't allow absolute paths");

      path = path::remove_quotes(path);

      return path::join(g_mounted_roots.at(root), path);
    }
    rsl::string create_full_path(rsl::string_view path)
    {
      path = path::remove_quotes(path);

      if(path::is_absolute(path))
      {
        return rsl::string(path);
      }

      REX_ASSERT_X(g_vfs_state_controller.has_state(VfsState::Running), "Trying to use vfs before it's initialized");
      return path::join(g_root, path);
    }

    rsl::string_view mount_path(MountingPoint mount)
    {
      if(g_mounted_roots.contains(mount))
      {
        return g_mounted_roots.at(mount);
      }

      return no_mount_path();
    }
    rsl::string_view no_mount_path()
    {
      // explicitely stating "no mount found" here
      // as an empty path would indicate it's at the root
      // instead of it not existing

      return rsl::string_view("no mount found");
    }

    rsl::vector<rsl::string> files_in_dir(rsl::string_view path)
    {
      const rsl::string full_path = vfs::create_full_path(path);

      return directory::list_files(full_path);
    }
  } // namespace vfs
} // namespace rex