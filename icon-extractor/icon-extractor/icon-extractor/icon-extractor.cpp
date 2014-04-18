#include "stdafx.h"

class ComInitializer {
public:
  ComInitializer(void) {
    CoInitializeEx(nullptr, COINIT_MULTITHREADED);
  }
  ~ComInitializer(void) {
    CoUninitialize();
  }
};

int main(int argc, _TCHAR* argv[])
{
  ComInitializer initializer;
  HRESULT hr;

  CComPtr<IShellFolder> pshf;
  hr = SHGetDesktopFolder(&pshf);
  if(FAILED(hr))
    return hr;

  PIDLIST_RELATIVE pidl;
  hr = pshf->ParseDisplayName(nullptr, nullptr, L"C:\\windows\\system32\\user32.dll", nullptr, &pidl, nullptr);


  CComPtr<IExtractIcon> iconExtract;
  hr = pshf->GetUIObjectOf(nullptr, 0, (LPCITEMIDLIST*) &pidl, __uuidof(IExtractIcon), nullptr, (void**) &iconExtract);
  CoTaskMemFree(pidl);

  wchar_t iconFile[MAX_PATH];
  int index;
  UINT flags;
  hr = iconExtract->GetIconLocation(0, iconFile, MAX_PATH, &index, &flags);

  HICON hLarge;
  HICON hSmall;
  hr = iconExtract->Extract(iconFile, index, &hLarge, &hSmall, 0);

	return 0;
}

