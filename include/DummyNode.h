#ifndef __DUMMYNODE_H__
#define __DUMMYNODE_H__

#include "HierarchyNode.h"

#include <cassert>
#include <cstdint>

class DummyNode : public HierarchyNode
{
public:

  // the name for the root node is not used in the path
  DummyNode (std::string const &name, std::shared_ptr<DummyNode> const &parent = std::shared_ptr<DummyNode>());

  // Returns the parent node if it exists, otherwise nullptr.
  virtual std::shared_ptr<HierarchyNode> parent () override { return m_parent; }
  // Returns a list of the child nodes of this item.
  virtual std::vector<std::shared_ptr<HierarchyNode>> child_nodes (FilterCriteria const &filter_criteria = FilterCriteria::NONE) override {
    return m_childNodes; // ignore filter criteria for now
  }

  // Uniquely identifies this item in the hierearchy.  This item's ancestry should be derivable from the path.
  virtual std::string path () const override;

  // Provides a way to open/activate/execute a node (e.g. run an executable or open a C++ class source file).
  virtual bool open (std::vector<std::string> const &parameters) const override { return false; }
  // Move a node to a particular parent node.
  virtual bool move (HierarchyNode &to_parent) override { return false; }
  // Remove a node from its parent.  Should not remove the root node.
  virtual bool remove () override { return false; }

  // Provides a way to search the hierarchy given some particular search criteria.
  virtual std::vector<std::shared_ptr<HierarchyNode>> recursive_search (FilterCriteria const &filter_criteria)override {
    return std::vector<std::shared_ptr<HierarchyNode>>(); // nothing for now
  }

  // Provides a way for updates to a hierarchy node to update the state of something else.
  typedef void (*UpdateCallback)(std::shared_ptr<HierarchyNode> updated_node);
  virtual void set_update_callback (UpdateCallback update_callback) override {
    // the dummy hierarchy never changes so the callback never needs to be called
  }

  void appendChildNode (std::shared_ptr<DummyNode> const &child) { m_childNodes.push_back(child); }

private:

  std::shared_ptr<DummyNode> m_parent;
  std::vector<std::shared_ptr<HierarchyNode>> m_childNodes;
};

std::shared_ptr<DummyNode> create_dummy_hierarchy (std::string const &root_name, unsigned int depth);

#endif
