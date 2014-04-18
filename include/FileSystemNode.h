#ifndef __FILESYSTEMNODE_H__
#define __FILESYSTEMNODE_H__

#include "HierarchyNode.h"

#include <cassert>
#include <cstdint>

class FileSystemNode : public HierarchyNode
{
public:

  // the name for the root node is not used in the path
  FileSystemNode(std::string const& path, std::shared_ptr<FileSystemNode> const& parent = std::shared_ptr<FileSystemNode>());
  FileSystemNode(char const* path, std::shared_ptr<FileSystemNode> const& parent = std::shared_ptr<FileSystemNode>());

  // Returns the parent node if it exists, otherwise nullptr.
  virtual std::shared_ptr<HierarchyNode> parent() override { return m_parent; }
  // Returns a list of the child nodes of this item.
  virtual HierarchyNodeVector child_nodes(FilterCriteria const& filter_criteria = FilterCriteria::NONE) override;

  // Uniquely identifies this item in the hierarchy.  This item's ancestry should be derivable from the path.
  virtual std::string path() const override;

  virtual ci::Surface8u icon() override;

  // Provides a way to open/activate/execute a node (e.g. run an executable or open a C++ class source file).
  virtual bool open(std::vector<std::string> const& parameters) const override { return false; }
  // Move a node to a particular parent node.
  virtual bool move(HierarchyNode& to_parent) override { return false; }
  // Remove a node from its parent.  Should not remove the root node.
  virtual bool remove() override { return false; }

  // Provides a way to search the hierarchy given some particular search criteria.
  virtual HierarchyNodeVector recursive_search(FilterCriteria const& filter_criteria) override {
    return HierarchyNodeVector(); // nothing for now
  }

  // Provides a way for updates to a hierarchy node to update the state of something else.
  typedef void (*UpdateCallback)(std::shared_ptr<HierarchyNode> updated_node);
  virtual void set_update_callback(UpdateCallback update_callback) override {
    // the dummy hierarchy never changes so the callback never needs to be called
  }

private:
  FileSystemNode(boost::filesystem::path const& path, std::shared_ptr<FileSystemNode> const& parent = std::shared_ptr<FileSystemNode>());
  FileSystemNode(boost::filesystem::path&& path, std::shared_ptr<FileSystemNode> const& parent = std::shared_ptr<FileSystemNode>());

  void init(std::shared_ptr<FileSystemNode> const& parent);

  boost::filesystem::path m_path;
  std::shared_ptr<FileSystemNode> m_parent;
  ci::Surface8u m_surface;
};

#endif
