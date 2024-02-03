#include "rex_engine/platform/win/win_com_library.h"

#include <coml2api.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <shobjidl_core.h>
#include <wtypes.h>

thread_local card32 rex::win::ComLibrary::s_init_succeeded_count = 0;

rex::win::ComLibrary::ComLibrary()
{
  if(s_init_succeeded_count == 0)
  {
    if(init_lib())
    {
      inc_ref();
    }
  }
  else
  {
    inc_ref();
  }
}

rex::win::ComLibrary::~ComLibrary()
{
  if(s_init_succeeded_count == 1)
  {
    uninit_lib();
  }

  dec_ref();
}

bool rex::win::ComLibrary::is_initialized() const
{
  return s_init_succeeded_count > 0;
}

rsl::string rex::win::ComLibrary::read_link(rsl::string_view filepath)
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
      // Replace "path_to_your_shortcut.lnk" with the actual path to your shortcut file
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

bool rex::win::ComLibrary::init_lib()
{
  return HR_SUCCESS(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));
}
void rex::win::ComLibrary::uninit_lib()
{
  CoUninitialize();
}

void rex::win::ComLibrary::inc_ref()
{
  s_init_succeeded_count++;
}
void rex::win::ComLibrary::dec_ref()
{
  s_init_succeeded_count--;
}

bool rex::win::init_com_library()
{
  // Simply touch the object, the ctor takes care of the rest
  return com_library().is_initialized();
}

rex::win::ComLibrary& rex::win::com_library()
{
  static thread_local ComLibrary com_lib;
  return com_lib;
}