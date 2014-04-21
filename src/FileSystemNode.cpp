#include "StdAfx.h"
#include "FileSystemNode.h"
#if defined(CINDER_COCOA)
#include "NSImageExt.h"
#endif
#if !defined(CINDER_MSW)
#include <sys/stat.h>
#endif

FileSystemNode::FileSystemNode(std::string const& path, std::shared_ptr<FileSystemNode> const& parent) :
  m_path(boost::filesystem::canonical(path))
{
  init(parent);
}

FileSystemNode::FileSystemNode(char const* path, std::shared_ptr<FileSystemNode> const& parent) :
  m_path(boost::filesystem::canonical(std::string(path)))
{
  init(parent);
}

FileSystemNode::FileSystemNode(boost::filesystem::path const& path, std::shared_ptr<FileSystemNode> const& parent) :
  m_path(path)
{
  init(parent);
}

FileSystemNode::FileSystemNode(boost::filesystem::path&& path, std::shared_ptr<FileSystemNode> const& parent) :
  m_path(std::move(path))
{
  init(parent);
}

void FileSystemNode::init(std::shared_ptr<FileSystemNode> const& parent)
{
  m_parent = (!parent && m_path.has_parent_path()) ?
             std::shared_ptr<FileSystemNode>(new FileSystemNode(m_path.parent_path())) : parent;
  set_metadata_as("name", m_path.filename().string());
  uint64_t size = 0;
  boost::filesystem::file_status file_status = boost::filesystem::status(m_path);
  if (file_status.type() == boost::filesystem::directory_file) {
    m_isLeaf = false;
  } else {
    m_isLeaf = true;
    if (file_status.type() == boost::filesystem::regular_file) {
      size = static_cast<uint64_t>(boost::filesystem::file_size(m_path));
    }
  }
  set_metadata_as("size", size);
  set_metadata_as("time", static_cast<uint64_t>(boost::filesystem::last_write_time(m_path)));
  set_metadata_as("perms", static_cast<uint16_t>(file_status.permissions()));
#if !defined(CINDER_MSW)
  struct stat sb = {0};
  if (::stat(m_path.c_str(), &sb) == 0) {
    set_metadata_as("uid", sb.st_uid);
    set_metadata_as("gid", sb.st_gid);
  }
#endif
}

bool FileSystemNode::is_leaf() const
{
  return m_isLeaf;
}

HierarchyNodeVector FileSystemNode::child_nodes(FilterCriteria const& filter_criteria)
{
  HierarchyNodeVector childNodes;

  try {
    if (boost::filesystem::is_directory(m_path)) {
      boost::filesystem::directory_iterator endIter; // default construction yields past-the-end
      for (boost::filesystem::directory_iterator iter(m_path); iter != endIter; ++iter) {
        if (iter->path().stem().empty()) { // Ignore dot (hidden) files for now
          continue;
        }
        try {
          childNodes.push_back(std::shared_ptr<FileSystemNode>(new FileSystemNode(*iter, std::static_pointer_cast<FileSystemNode>(shared_from_this()))));
        } catch (...) {}
      }
    }
  } catch (...) {}
  return childNodes; // ignore filter criteria for now
}

std::string FileSystemNode::path() const
{
  return m_path.string();
}

ci::Surface8u FileSystemNode::icon()
{
  if (!m_surface) {
#if defined(CINDER_COCOA)
    @autoreleasepool {
      NSString* path = [NSString stringWithUTF8String:m_path.string().c_str()];
      NSSize size = NSMakeSize(128, 128);
      NSImage* nsImage = [[[NSWorkspace sharedWorkspace] iconForFile:path] imageByScalingProportionallyToSize:size fill:NO];
      NSBitmapImageRep* nsBitmapImageRep = [NSBitmapImageRep imageRepWithData:[nsImage TIFFRepresentation]];
      NSBitmapFormat nsBitmapFormat = [nsBitmapImageRep bitmapFormat];
      unsigned char *srcBytes = [nsBitmapImageRep bitmapData];

      size = [nsImage pixelSize];
      const int32_t width = static_cast<int32_t>(size.width);
      const int32_t height = static_cast<int32_t>(size.height);
      const int32_t srcRowBytes = [nsBitmapImageRep bytesPerRow];

      m_surface = ci::Surface8u(width, height, true,
          (nsBitmapFormat & NSAlphaFirstBitmapFormat) ? ci::SurfaceChannelOrder::ARGB :ci::SurfaceChannelOrder::RGBA);
      m_surface.setPremultiplied((nsBitmapFormat & NSAlphaNonpremultipliedBitmapFormat) == 0);
      unsigned char* dstBytes = m_surface.getData();
      int32_t dstRowBytes = width*4;

      for (int32_t i = 0; i < height; i++) {
        ::memcpy(dstBytes, srcBytes, dstRowBytes);
        dstBytes += dstRowBytes;
        srcBytes += srcRowBytes;
      }
    }
#else
    // Really, any folder will do--we use GetDesktopFolder because it's always available to serve requests.
    CComPtr<IShellFolder> pshf;
    HRESULT hr = SHGetDesktopFolder(&pshf);
    if (!FAILED(hr)) {
      // Need to construct a descriptor list which describes the path to the file we are trying to find:
      PIDLIST_RELATIVE pidl;
      wchar_t* filename = (wchar_t*)(m_path.wstring().c_str());
      hr = pshf->ParseDisplayName(nullptr, nullptr, filename, nullptr, &pidl, nullptr);

      // Now we need to make an extractor for the file we're interested in.  The extractor allows us to generate
      // an HICON from a given file, and does most of the work of format parsing for us.
      CComPtr<IExtractIcon> iconExtract;
      hr = pshf->GetUIObjectOf(nullptr, 1, (LPCITEMIDLIST*) &pidl, __uuidof(IExtractIcon), nullptr, (void**) &iconExtract);

      // Figure out the location of the icon file and the index into that file where our icon is stored
      wchar_t iconFile[MAX_PATH];
      int index = 0;
      UINT flags = 0;
      hr = iconExtract->GetIconLocation(0, iconFile, MAX_PATH, &index, &flags);

      // Pull the icon out of storage by its file name and index.
      HICON hLarge;
      hr = iconExtract->Extract(iconFile, index, &hLarge, nullptr, 0);

      int dimension = 32;

      // Create an HBITMAP which we can query from a backing DIB:
      HANDLE hSection = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, 4*dimension*dimension, nullptr); void* pBits;

      // We need to create a backing surface to render the HICON.  This surface will be backed by the section we
      // just created, and have a maximum dimension of 256x256 pixels.
      BITMAPINFO bmi = {};
      bmi.bmiHeader.biSize = sizeof(bmi);
      bmi.bmiHeader.biWidth = dimension;
      bmi.bmiHeader.biHeight = -dimension;
      bmi.bmiHeader.biPlanes = 1;
      bmi.bmiHeader.biBitCount = 32;
      bmi.bmiHeader.biCompression = BI_RGB;
      HBITMAP hBmp = CreateDIBSection(nullptr, &bmi, 0, &pBits, hSection, 0);
      CloseHandle(hSection);

      // Create a DC that we will use to render the HICON.  We select the backing surface into the DC so that draw
      // operations on the DC have a place to go.
      HDC hCompatDC = CreateCompatibleDC(nullptr);
      SelectObject(hCompatDC, hBmp);

      // This is where our icon is actually drawn.  Up to this point, we've only been preparing for this render.
      DrawIcon(hCompatDC, 0, 0, hLarge);

      // At this point, pBits contains a pointer to the image starting at address zero, with a width of 256 pixels,
      // and a pixel depth of 32 bits.

      m_surface = ci::Surface8u(dimension, dimension, true, ci::SurfaceChannelOrder::ARGB);
      m_surface.setPremultiplied(true);
      ::memcpy(m_surface.getData(), pBits, dimension*dimension*4);

      DestroyIcon(hLarge);
      DeleteDC(hCompatDC);
      DeleteObject(hBmp);
    }
#endif
  }
  return m_surface;
}

bool FileSystemNode::open(std::vector<std::string> const& parameters) const
{
  const std::string path = m_path.string();
  bool opened = false;

  if (!path.empty()) {
#if defined(CINDER_MSW)
    if (is_leaf()) {
      HINSTANCE instance = ShellExecuteA(nullptr, "open", path.c_str(), "", nullptr, SW_SHOWNORMAL);
      opened = true;
      int returnCode = reinterpret_cast<int>(instance);
      opened = (returnCode >= 32);
    }
#elif defined(CINDER_COCOA)
    CFURLRef urlPathRef;

    if (path[0] != '/' && path[0] != '~') { // Relative Path
      CFBundleRef mainBundle = CFBundleGetMainBundle();
      CFURLRef urlExecRef = CFBundleCopyExecutableURL(mainBundle);
      CFURLRef urlRef = CFURLCreateCopyDeletingLastPathComponent(kCFAllocatorDefault, urlExecRef);
      CFStringRef pathRef = CFStringCreateWithCString(kCFAllocatorDefault, path.c_str(), kCFStringEncodingUTF8);
      urlPathRef = CFURLCreateCopyAppendingPathComponent(kCFAllocatorDefault, urlRef, pathRef, false);
      CFRelease(pathRef);
      CFRelease(urlRef);
      CFRelease(urlExecRef);
    } else {
      urlPathRef = CFURLCreateFromFileSystemRepresentation(kCFAllocatorDefault, reinterpret_cast<const UInt8 *>(path.data()), path.size(), false);
    }
    CFStringRef stringRef = CFURLCopyPath(urlPathRef);

    if (CFStringHasSuffix(stringRef, CFSTR(".app"))) {
      FSRef fsRef;

      if (CFURLGetFSRef(urlPathRef, &fsRef)) {
        CFStringRef args[parameters.size()];
        CFArrayRef argv = nullptr;

        if (!parameters.empty()) {
          for (size_t i = 0; i < parameters.size(); i++) {
            args[i] = CFStringCreateWithCString(kCFAllocatorDefault, parameters[i].c_str(), kCFStringEncodingUTF8);
          }
          argv = CFArrayCreate(kCFAllocatorDefault, reinterpret_cast<const void**>(args), parameters.size(), nullptr);
        }
        LSApplicationParameters params = {0, kLSLaunchDefaults, &fsRef, nullptr, nullptr, argv, nullptr};
        LSOpenApplication(&params, 0);
        if (argv) {
          CFRelease(argv);
        }
        for (size_t i = 0; i < parameters.size(); i++) {
          CFRelease(args[i]);
        }
      }
      opened = true; // Whether or not we actually launched the app, don't go snooping into .app files
    } else if (!boost::filesystem::is_directory(m_path)) {
      opened = !LSOpenCFURLRef(urlPathRef, 0);
    }
    CFRelease(stringRef);
    CFRelease(urlPathRef);
#endif
  }
  return opened;
}

bool FileSystemNode::move(HierarchyNode& to_parent)
{
  return false;
}

bool FileSystemNode::remove()
{
  return false;
}
