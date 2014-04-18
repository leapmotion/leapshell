#include "StdAfx.h"
#include "FileSystemNode.h"
#if defined(CINDER_COCOA)
#include "NSImageExt.h"
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
}

HierarchyNodeVector FileSystemNode::child_nodes(FilterCriteria const& filter_criteria)
{
  HierarchyNodeVector childNodes;

  if (boost::filesystem::is_directory(m_path)) {
    boost::filesystem::directory_iterator endIter; // default construction yields past-the-end
    for (boost::filesystem::directory_iterator iter(m_path); iter != endIter; ++iter) {
      childNodes.push_back(std::shared_ptr<FileSystemNode>(new FileSystemNode(*iter, std::static_pointer_cast<FileSystemNode>(shared_from_this()))));
    }
  }
  return childNodes; // ignore filter criteria for now
}

std::string FileSystemNode::path() const
{
  return m_path.string();
}

ci::Surface8u FileSystemNode::icon()
{
#if defined(CINDER_COCOA)
  if (!m_surface) {
    @autoreleasepool {
      NSString* path = [NSString stringWithUTF8String:m_path.string().c_str()];
      NSSize size = NSMakeSize(128, 128);
      NSImage* nsImage = [[[NSWorkspace sharedWorkspace] iconForFile:path] imageByScalingProportionallyToSize:size];
      NSBitmapImageRep* nsBitmapImageRep = [NSBitmapImageRep imageRepWithData:[nsImage TIFFRepresentation]];
      NSBitmapFormat nsBitmapFormat = [nsBitmapImageRep bitmapFormat];
      unsigned char *srcBytes = [nsBitmapImageRep bitmapData];

      size = [nsImage size];
      const int32_t width = static_cast<int32_t>(size.width)*2;
      const int32_t height = static_cast<int32_t>(size.height)*2;
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
  }
#endif
  return m_surface;
}
