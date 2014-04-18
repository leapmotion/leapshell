#include "StdAfx.h"
#include "FileSystemNode.h"

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
