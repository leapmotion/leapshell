#ifndef __HIERARCHYNODE_H__
#define __HIERARCHYNODE_H__

#include "Value.h"

class FilterCriteria // TEMP fill in
{
public:

  static FilterCriteria const NONE;

  FilterCriteria () { }
};

class HierarchyNode;

/// @brief Convenience typedef for vectors of pointers to HierarchyNode instances.
typedef std::vector<std::shared_ptr<HierarchyNode>> HierarchyNodeVector;

/// @brief Interface class for implementing hierarchies of data that will be browsed by this application.
/// @details The basic information an implementation of this interface must provide is, for each node
/// in the hierarchy:
///
/// - Parent node (or nullptr if this node is the root node)
/// - Vector of child nodes (with optional filtering criteria (NOTE: filtering is yet to be implemented))
/// - Indication of if this node is a leaf node
/// - A path string which uniquely identifies a node within the hierarchy.  The item's ancestry should
///   be derivable from the path.
///
/// Some optional features are the following implementation-dependent functionality:
/// - An "open" procedure (e.g. run an executable, open a directory)
/// - An "move" procedure, to effectively re-parent a node.
/// - An "remove" procedure, to remove a node from the hierarchy.
/// - A "recursive search" function, which is vaguely defined for the moment.
/// - A procedure to attach a "notify upon update" callback, which the application can use however it pleases.
///
/// Each HierarchyNode also contains metadata (a Value::Hash-typed Value instance) which provides
/// some abstract information about the node, and can be used for sorting and clustering nodes.
class HierarchyNode : public std::enable_shared_from_this<HierarchyNode>
{
public:

  HierarchyNode () : m_metadata(Value::Hash()) {}

  /// @brief Returns the parent node if it exists, otherwise nullptr.
  virtual std::shared_ptr<HierarchyNode> parent () = 0;
  /// @brief Returns a list of the child nodes of this item.
  /// @note The FilterCriteria stuff is currently unimplemented.
  virtual void child_nodes (std::function<bool(const std::shared_ptr<HierarchyNode>&)> callback, FilterCriteria const &filter_criteria = FilterCriteria::NONE) = 0;
  /// @brief Returns true if and only if this is a leaf node.
  virtual bool is_leaf () const = 0;

  virtual void all_ancestors(std::function<void(const std::shared_ptr<HierarchyNode>&)> callback);

  /// @brief Uniquely identifies this item in the hierearchy.
  /// @details This item's ancestry should be derivable from the path (e.g. the directory portion of a file path).
  virtual std::string path () const = 0;

  /// @brief Returns the hierarchy-specific metadata for this node.
  Value const &metadata () const { return m_metadata; }
  /// @brief Convenience method for accessing typed metadata.
  template<typename T>
  T get_metadata_as(std::string const &key) const {
    Value value = m_metadata.HashGet(key);
    if (value.IsNull()) {
      throw std::invalid_argument("metadata with " + key + " not found");
    }
    return value.To<T>();
  }

  /// @brief Convenience method for modifying typed metadata.
  template<typename T>
  void set_metadata_as(std::string const& key, T const& value) {
    m_metadata.HashSet(key, Value(value));
  }

  /// @brief Returns the icon associated with this node.
  /// @details Technically this is an application-specific feature, but for the Hackathon, it made sense to go here.
  virtual ci::Surface8u icon() { return ci::Surface8u(); }

  /// @brief Provides a way to open/activate/execute a node (e.g. run an executable or open a C++ class source file).
  virtual bool open (std::vector<std::string> const &parameters = std::vector<std::string>()) const = 0;
  /// @brief Move a node to a particular parent node.
  virtual bool move (HierarchyNode &to_parent) = 0;
  /// @brief Remove a node from its parent.  Should not remove the root node.
  virtual bool remove () = 0;

  /// @brief Provides a way to search the hierarchy given some particular search criteria.
  virtual HierarchyNodeVector recursive_search (FilterCriteria const &filter_criteria) = 0;

  /// @brief Provides a way for updates to a hierarchy node to update the state of something else.
  typedef void (*UpdateCallback)(std::shared_ptr<HierarchyNode> updated_node);
  virtual void set_update_callback (UpdateCallback update_callback) = 0;

protected:

  /// @brief The Value::Hash-typed Value which contains the metadata for this node.
  Value m_metadata;
};

#endif
