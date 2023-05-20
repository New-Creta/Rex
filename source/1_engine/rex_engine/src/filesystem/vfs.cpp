#include "rex_engine/filesystem/vfs.h"

#include "rex_engine/diagnostics/assert.h"
#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_engine/diagnostics/win/win_call.h"
#include "rex_engine/memory/untracked_allocator.h"
#include "rex_engine/string/stringid.h"
#include "rex_std/atomic.h"
#include "rex_std/bonus/platform/windows/handle.h"
#include "rex_std/bonus/string.h"
#include "rex_std/chrono.h"
#include "rex_std/memory.h"
#include "rex_std/mutex.h"
#include "rex_std/semaphore.h"
#include "rex_std/thread.h"
#include "rex_std_extra/memory.h"
#include "rex_std_extra/utility.h"

#include <Windows.h>

DEFINE_LOG_CATEGORY(RexFileSystem, rex::LogVerbosity::Log);

// NOLINTBEGIN(modernize-use-nullptr)

namespace rex
{
  namespace vfs
  {

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

        if (it != m_requests.cend())
        {
          m_requests.erase(it);
        }
      }
      void swap_request_to_signal(ReadRequest* original, ReadRequest* newRequest)
      {
        const rsl::unique_lock lock(m_requests_access_mtx);
        auto it = rsl::find(m_requests.begin(), m_requests.end(), original);

        REX_ASSERT_X(it != m_requests.end(), "Request at {} not found as a request to signal", reinterpret_cast<void*>(original)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

        *it = newRequest;
      }

      void signal_requests(const rsl::byte* buffer, rsl::memory_size size)
      {
        const rsl::unique_lock lock(m_requests_access_mtx);

        for (ReadRequest* read_request : m_requests)
        {
          read_request->signal(buffer, size);
        }
      }
      void wait_async_until_requests_are_finished(memory::Blob&& buffer)
      {
        // this thread becomes the owner of the buffer
        // when the thread finished, the buffer will be destroyed
        rsl::thread t(
          [this, buffer = rsl::move(buffer)]()
        {
          // don't think this needs to be behind a lock
          // it's value will always decrement
          while (!m_requests.empty())
          {
            using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
            rsl::this_thread::sleep_for(1ms);
          }
          m_is_done = true;
        });
        t.detach();
      }

      bool all_requests_finished() const
      {
        return m_is_done.load();
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
      if (m_queued_request)
      {
        m_queued_request->remove_request_to_signal(this);
      }
    }

    ReadRequest& ReadRequest::operator=(const ReadRequest& other) // NOLINT(bugprone-unhandled-self-assignment)
    {
      REX_ASSERT_X(this != &other, "assigning a read request to itself");

      m_queued_request->remove_request_to_signal(this);

      m_filepath = other.m_filepath;
      m_queued_request = other.m_queued_request;
      m_is_done = other.m_is_done;
      m_buffer = other.m_buffer;
      m_size = other.m_size;

      m_queued_request->add_request_to_signal(this);

      return *this;
    }
    ReadRequest& ReadRequest::operator=(ReadRequest&& other)
    {
      REX_ASSERT_X(this != &other, "moving a read request to itself");

      m_filepath = rsl::exchange(other.m_filepath, "");
      m_queued_request = rsl::exchange(other.m_queued_request, nullptr);
      m_is_done = rsl::exchange(other.m_is_done, false);
      m_buffer = rsl::exchange(other.m_buffer, nullptr);
      m_size = rsl::exchange(other.m_size, 0_bytes);

      m_queued_request->swap_request_to_signal(&other, this);

      return *this;
    }

    void ReadRequest::signal(const rsl::byte* buffer, rsl::memory_size size)
    {
      m_is_done = true;
      m_buffer = buffer;
      m_size = size;
    }

    void ReadRequest::wait() const
    {
      // this should ideally be solved with fibers..
      while (!m_is_done)
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

    // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables, fuchsia-statically-constructed-objects)
    rsl::medium_stack_string g_root;
    bool g_is_initialized = false;
    rsl::mutex g_read_request_mutex;
    rsl::mutex g_closed_request_mutex;
    rsl::vector<rsl::unique_ptr<QueuedRequest>> g_read_requests;
    rsl::vector<rsl::unique_ptr<QueuedRequest>> g_closed_requests;
    rsl::unordered_map<MountRoot, rsl::string> g_roots;
    rsl::thread g_reading_thread;
    rsl::thread g_closing_thread;
    rsl::atomic<bool> g_keep_processing = false;
    // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables, fuchsia-statically-constructed-objects)

    void init(rsl::string_view root)
    {
      if (root.empty())
      {
        WIN_CALL(GetCurrentDirectoryA(g_root.max_size(), g_root.data()));
        g_root.reset_null_termination_offset();
      }
      else if (is_abs(root))
      {
        g_root = root;
      }
      else
      {
        rsl::medium_stack_string cwd;
        WIN_CALL(GetCurrentDirectoryA(cwd.max_size(), cwd.data()));
        cwd.reset_null_termination_offset();

        g_root = cwd;
        g_root += "/";
        g_root += root;
      }

      g_is_initialized = true;
      g_keep_processing = true;

      REX_ASSERT_X(is_dir(g_root), "root of vfs is not a directory");
      REX_LOG(RexFileSystem, "FileSystem initialised with root '{}'", g_root);

      g_reading_thread = rsl::thread(
        []()
        {
          while (g_keep_processing)
          {
            rsl::unique_lock lock(g_read_request_mutex);
            if (!g_read_requests.empty())
            {
              // get the queued request and remvoe it from the queue
              rsl::unique_ptr<QueuedRequest> request = rsl::move(g_read_requests.front());
              g_read_requests.erase(g_read_requests.cbegin());

              // we don't need access to the queue anymore, we can unlock its access mutex
              lock.unlock();

              // read the actual file we requested
              memory::Blob buffer = open_read(request->filepath());

              // signal all read requests that this file has now been read
              // it's possible multiple read requests want to access the same file
              // if such requests come in while there's already a request for this file
              // they get added to the original queued request and they now all get notified
              request->signal_requests(buffer.data(), buffer.size());

              // wait for all read requests to finish processing the data read
              // pass ownership of the data to the queued task, it'll be deallocated when the task is destroyed
              request->wait_async_until_requests_are_finished(rsl::move(buffer));

              // add the closed task to the queue, making sure it stays alive until all requests have finished processing the data
              const rsl::unique_lock closed_req_lock(g_closed_request_mutex);
              g_closed_requests.push_back(rsl::move(request));
            }

            using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
            rsl::this_thread::sleep_for(1ms);
          }
        });
      g_closing_thread = rsl::thread(
        []()
        {
          while (g_keep_processing)
          {
            const rsl::unique_lock lock(g_closed_request_mutex);
            if (!g_closed_requests.empty())
            {
              for (rsl::unique_ptr<QueuedRequest>& request : g_closed_requests)
              {
                if (request->all_requests_finished())
                {
                  request.reset();
                }
              }

              auto it = rsl::remove(g_closed_requests.begin(), g_closed_requests.end(), nullptr);
              g_closed_requests.erase(it, g_closed_requests.end());
            }

            using namespace rsl::chrono_literals; // NOLINT(google-build-using-namespace)
            rsl::this_thread::sleep_for(1ms);
          }
        });
    }

    void mount(MountRoot root, rsl::string_view path)
    {
      REX_ASSERT_X(!g_roots.contains(root), "root {} is already mapped. currently mapped to '{}'", rsl::enum_refl::enum_name(root), g_roots.at(root));
      g_roots[root] = rsl::string(path);

      // make sure the mount exists
      create_dir(path);
    }

    void shutdown()
    {
      g_keep_processing = false;
    }

    memory::Blob open_read(MountRoot root, rsl::string_view filepath)
    {
      rsl::medium_stack_string path(g_roots.at(root));
      path += "/";
      path += filepath;
      return open_read(path);
    }

    memory::Blob open_read(rsl::string_view filepath)
    {
      rsl::medium_stack_string path = create_full_path(filepath);

      rsl::win::handle handle(WIN_CALL_IGNORE(CreateFile(path.data(),               // Path to file
        GENERIC_READ,              // General read and write access
        FILE_SHARE_READ,           // Other processes can also read the file
        NULL,                      // No SECURITY_ATTRIBUTES
        OPEN_EXISTING,             // Open the file, only if it exists
        FILE_FLAG_SEQUENTIAL_SCAN, // Files will be read from beginning to end
        NULL                       // No template file
      ),
        ERROR_ALREADY_EXISTS));

      // prepare a buffer to receive the file content
      const card32 file_size = static_cast<card32>(GetFileSize(handle.get(), nullptr));
      rsl::unique_array<rsl::byte> buffer = rsl::make_unique<rsl::byte[]>(file_size + 1); // NOLINT(modernize-avoid-c-arrays)
      buffer[file_size] = rsl::byte(0); // make sure we end with a null char

      // actually read the file
      DWORD bytes_read = 0;
      WIN_CALL(ReadFile(handle.get(), buffer.get(), static_cast<DWORD>(buffer.count()), &bytes_read, NULL));

      // return the buffer
      return memory::Blob(rsl::move(buffer));
    }

    ReadRequest open_read_async(MountRoot root, rsl::string_view filepath)
    {
      rsl::medium_stack_string path(g_roots.at(root));
      path += "/";
      path += filepath;
      return open_read_async(path);
    }

    ReadRequest open_read_async(rsl::string_view filepath)
    {
      const rsl::unique_lock lock(g_read_request_mutex);

      // create the queued request, at this point it doesn't hold any signals it should fire on finish
      rsl::unique_ptr<QueuedRequest> queued_request = rsl::make_unique<QueuedRequest>(filepath);
      ReadRequest request(filepath, queued_request.get());
      queued_request->add_request_to_signal(&request);

      g_read_requests.emplace_back(rsl::move(queued_request));

      return request;
    }

    void save_to_file(MountRoot root, rsl::string_view filepath, const void* data, card64 size, AppendToFile shouldAppend)
    {
      rsl::medium_stack_string path(g_roots.at(root));
      path += "/";
      path += filepath;
      save_to_file(path, data, size, shouldAppend);
    }

    void save_to_file(rsl::string_view filepath, const void* data, card64 size, AppendToFile shouldAppend)
    {
      rsl::medium_stack_string fullpath = create_full_path(filepath);

      rsl::win::handle handle(WIN_CALL_IGNORE(CreateFile(fullpath.data(),           // Path to file
        GENERIC_WRITE,             // General read and write access
        FILE_SHARE_READ,           // Other processes can also read the file
        NULL,                      // No SECURITY_ATTRIBUTES
        OPEN_ALWAYS,               // Create a new file, error when it already exists
        FILE_FLAG_SEQUENTIAL_SCAN, // Files will be read from beginning to end
        NULL                       // No template file
      ),
        ERROR_ALREADY_EXISTS));

      if (shouldAppend)
      {
        WIN_CALL(SetFilePointer(handle.get(), 0, NULL, FILE_END));
        WIN_CALL(SetEndOfFile(handle.get()));
      }

      DWORD bytes_written = 0;
      WIN_CALL(WriteFile(handle.get(), data, static_cast<DWORD>(size), &bytes_written, NULL));
    }

    void save_to_file(MountRoot root, rsl::string_view filepath, const memory::Blob& blob, AppendToFile shouldAppend)
    {
      rsl::medium_stack_string path(g_roots.at(root));
      path += "/";
      path += filepath;
      save_to_file(path, blob.data(), blob.size(), shouldAppend);
    }

    void save_to_file(rsl::string_view filepath, const memory::Blob& blob, AppendToFile shouldAppend)
    {
      save_to_file(filepath, blob.data(), blob.size(), shouldAppend);
    }

    void create_dir(MountRoot root, rsl::string_view path)
    {
      rsl::medium_stack_string filepath(g_roots.at(root));
      filepath += "/";
      filepath += path;
      create_dir(filepath);
    }
    void create_dir(rsl::string_view path)
    {
      rsl::medium_stack_string fullpath = create_full_path(path);
      WIN_CALL_IGNORE(CreateDirectory(fullpath.data(), NULL), ERROR_ALREADY_EXISTS);
    }

    bool exists(MountRoot root, rsl::string_view path)
    {
      rsl::medium_stack_string filepath(g_roots.at(root));
      filepath += "/";
      filepath += path;
      return exists(filepath);
    }

    bool exists(rsl::string_view path)
    {
      rsl::medium_stack_string fullpath = create_full_path(path);

      const DWORD ftyp = GetFileAttributesA(fullpath.data());
      if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false; // NOLINT(readability-simplify-boolean-expr)

      return true;
    }

    bool is_dir(MountRoot root, rsl::string_view path)
    {
      rsl::medium_stack_string filepath(g_roots.at(root));
      filepath += "/";
      filepath += path;
      return is_dir(filepath);
    }
    bool is_dir(rsl::string_view path)
    {
      rsl::medium_stack_string fullpath = create_full_path(path);

      const DWORD ftyp = GetFileAttributesA(fullpath.data());
      if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;

      if (ftyp & FILE_ATTRIBUTE_DIRECTORY) // NOLINT(readability-implicit-bool-conversion)
        return true;

      return false;
    }
    bool is_file(MountRoot root, rsl::string_view path)
    {
      rsl::medium_stack_string filepath(g_roots.at(root));
      filepath += "/";
      filepath += path;
      return is_file(filepath);
    }
    bool is_file(rsl::string_view path)
    {
      rsl::medium_stack_string fullpath = create_full_path(path);

      const DWORD ftyp = GetFileAttributesA(fullpath.data());
      if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;

      if (ftyp & FILE_ATTRIBUTE_DIRECTORY) // NOLINT(readability-implicit-bool-conversion)
        return false;

      return true;
    }
    bool is_abs(rsl::string_view path)
    {
      if (path.length() < 2)
        return false;

      if (!rsl::is_alpha(path[0]))
        return false;

      if (path[1] != ':')
        return false;

      if (path[2] != '/' && path[2] != '\\')
        return false;

      return true;
    }
    bool is_rel(rsl::string_view path)
    {
      return !is_abs(path);
    }

    rsl::medium_stack_string create_full_path(MountRoot root, rsl::string_view path)
    {
      REX_ASSERT_X(g_is_initialized, "Trying to use vfs before it's initialized");
      REX_ASSERT_X(!is_abs(path), "Passed an absolute path into a function that doesn't allow absolute paths");

      rsl::medium_stack_string full_path(g_roots.at(root));
      full_path += "/";
      full_path += path;
      return full_path;
    }
    rsl::medium_stack_string create_full_path(rsl::string_view path)
    {
      REX_ASSERT_X(g_is_initialized, "Trying to use vfs before it's initialized");

      if (is_abs(path))
      {
        return path;
      }

      rsl::medium_stack_string full_path(g_root);
      full_path += "/";
      full_path += path;

      return full_path;
    }
  } // namespace vfs
} // namespace rex

// NOLINTEND(modernize-use-nullptr)
