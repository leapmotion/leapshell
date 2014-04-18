#ifndef __HIERARCHYNODE_H__
#define __HIERARCHYNODE_H__

class FilterCriteria // TEMP fill in
{
public:

  static FilterCriteria const NONE;

  FilterCriteria () { }
};

class HierarchyNode
{
public:

  // Returns the parent node if it exists, otherwise nullptr.
  virtual std::shared_ptr<HierarchyNode> parent () = 0;
  // Returns a list of the child nodes of this item.
  virtual std::vector<HierarchyNode *> child_nodes (FilterCriteria const &filter_criteria = FilterCriteria::NONE) = 0;

  // Uniquely identifies this item in the hierearchy.  This item's ancestry should be derivable from the path.
  virtual std::string path () const = 0;
  // Returns the hierarchy-specific meta
  virtual void metadata () const = 0;

  // Provides a way to open/activate/execute a node (e.g. run an executable or open a C++ class source file).
  virtual bool open (std::vector<std::string> const &parameters) const = 0;
  // Move a node to a particular parent node.
  virtual bool move (HierarchyNode &to_parent) = 0;
  // Remove a node from its parent.  Should not remove the root node.
  virtual bool remove () = 0;

  // Provides a way to search the hierarchy given some particular search criteria.
  virtual std::vector<std::shared_ptr<HierarchyNode>> recursive_search (FilterCriteria const &filter_criteria) = 0;

  // Provides a way for updates to a hierarchy node to update the state of something else.
  typedef void (*UpdateCallback)(std::shared_ptr<HierarchyNode> updated_node);
  virtual void set_update_callback (UpdateCallback update_callback) = 0;
};

#endif
