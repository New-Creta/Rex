#include "rex_engine/platform/win/win_com_library.h"

#include "rex_engine/diagnostics/log.h"

#include <coml2api.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <shobjidl_core.h>
#include <wtypes.h>

namespace rex
{
  namespace win
  {
    namespace com_lib
    {
      DEFINE_LOG_CATEGORY(WinComLibrary, rex::LogVerbosity::Log);

      namespace internal
      {
        class ComLibrary
        {
        public:
          // Initialize the library on first try
          ComLibrary()  = default;
          ~ComLibrary() = default;

          ComLibrary(const ComLibrary&) = delete;
          ComLibrary(ComLibrary&&)      = delete;

          ComLibrary& operator=(const ComLibrary&) = delete;
          ComLibrary& operator=(ComLibrary&&)      = delete;

          // Return if the library is initialized
          bool is_initialized() const
          {
            return s_ref_count == 0;
          }

          // Read a symbolic link's path and return the path it actually points to
          rsl::string read_link(rsl::string_view filepath)
          {
            IShellLinkW* psl = nullptr;
            HRESULT hres     = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, reinterpret_cast<LPVOID*>(&psl));
            rsl::wbig_stack_string res;

            if(SUCCEEDED(hres))
            {
              IPersistFile* ppf = nullptr;
              hres              = psl->QueryInterface(IID_IPersistFile, reinterpret_cast<LPVOID*>(&ppf));

              if(SUCCEEDED(hres))
              {
                const rsl::wstring wide_filepath(filepath.cbegin(), filepath.cend());
                hres = ppf->Load(wide_filepath.c_str(), STGM_READ);

                if(SUCCEEDED(hres))
                {
                  hres = psl->GetPath(res.data(), rsl::wbig_stack_string::max_size(), nullptr, SLGP_UNCPRIORITY);
                  res.reset_null_termination_offset();
                }

                ppf->Release();
              }

              psl->Release();
            }

            return rsl::to_string(res);
          }

          // Increase the amount of reference the com lib has on this thread
          void inc_ref()
          {
            if(!is_initialized())
            {
              init_lib();
            }
            s_ref_count++;
          }
          // Decrease the amount of reference the com lib has on this thread
          void dec_ref()
          {
            s_ref_count--;
            if(s_ref_count == 0)
            {
              shutdown();
            }
          }

        private:
          // Initialize the library
          bool init_lib()
          {
            return HR_SUCCESS(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));
          }

          // Destroy the com library
          void shutdown()
          {
            CoUninitialize();
          }

        private:
          thread_local static s32 s_ref_count;
        };

        thread_local s32 ComLibrary::s_ref_count = 0;

        // Return the global com library object
        ComLibrary& com_library()
        {
          static thread_local ComLibrary com_lib;
          return com_lib;
        }
      } // namespace internal

      // Creates a WinComHandle.
      // Makes sure the win com lib is initialized before creating the handle
      WinComLibHandle create_lib_handle()
      {
        // Just access it, that'll initialize it.
        internal::com_library().inc_ref();
        return WinComLibHandle();
      }
      // Return if the library is initialized already or not
      bool is_initialized()
      {
        return internal::com_library().is_initialized();
      }

      // Read a symbolic link's path and return the path it actually points to
      rsl::string read_link(rsl::string_view filepath)
      {
        return internal::com_library().read_link(filepath);
      }

      // Destroy the win com library
      WinComLibHandle::~WinComLibHandle()
      {
        internal::com_library().dec_ref();
      }
    } // namespace com_lib
  }   // namespace win
} // namespace rex