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

  // Really, any folder will do--we use GetDesktopFolder because it's always available to serve requests.
  CComPtr<IShellFolder> pshf;
  hr = SHGetDesktopFolder(&pshf);
  if(FAILED(hr))
    return hr;

  // Need to construct a descriptor list which describes the path to the file we are trying to find:
  PIDLIST_RELATIVE pidl;
  hr = pshf->ParseDisplayName(nullptr, nullptr, L"C:\\Windows\\", nullptr, &pidl, nullptr);

  // Now we need to make an extractor for the file we're interested in.  The extractor allows us to generate
  // an HICON from a given file, and does most of the work of format parsing for us.
  CComPtr<IExtractIcon> iconExtract;
  hr = pshf->GetUIObjectOf(nullptr, 1, (LPCITEMIDLIST*) &pidl, __uuidof(IExtractIcon), nullptr, (void**) &iconExtract);

  // Figure out the location of the icon file and the index into that file where our icon is stored
  wchar_t iconFile[MAX_PATH];
  int index;
  UINT flags;
  hr = iconExtract->GetIconLocation(0, iconFile, MAX_PATH, &index, &flags);

  // Pull the icon out of storage by its file name and index.
  HICON hLarge;
  hr = iconExtract->Extract(iconFile, index, &hLarge, nullptr, 0);

  // Create an HBITMAP which we can query from a backing DIB:
  HANDLE hSection = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 0x1000 * 0x1000, nullptr);
  void* pBits;

  // We need to create a backing surface to render the HICON.  This surface will be backed by the section we
  // just created, and have a maximum dimension of 256x256 pixels.
  BITMAPINFO bmi = {};
  bmi.bmiHeader.biSize = sizeof(bmi);
  bmi.bmiHeader.biWidth = 0x100;
  bmi.bmiHeader.biHeight = -0x100;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  HBITMAP hBmp = CreateDIBSection(nullptr, &bmi, 0, &pBits, hSection, 0);

  // Create a DC that we will use to render the HICON.  We select the backing surface into the DC so that draw
  // operations on the DC have a place to go.
  HDC hCompatDC = CreateCompatibleDC(nullptr);
  SelectObject(hCompatDC, hBmp);

  // This is where our icon is actually drawn.  Up to this point, we've only been preparing for this render.
  DrawIcon(hCompatDC, 0, 0, hLarge);


  // At this point, pBits contains a pointer to the image starting at address zero, with a stride of 100 pixels,
  // and a pixel depth of 32 bits.

  DestroyIcon(hLarge);
  DeleteDC(hCompatDC);
	return 0;
}

