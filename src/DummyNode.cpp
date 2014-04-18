#include "StdAfx.h"
#include "DummyNode.h"
#include "Utilities.h"

#include <cstdlib> // for rand()
#include <sstream> // for ostringstream used in FORMAT

DummyNode::DummyNode (std::string const &name, std::shared_ptr<DummyNode> const &parent)
  :
  m_parent(parent)
{
  // Name cannot contain slash -- FIXME
  set_metadata_as("name", name);
}

std::string DummyNode::path () const {
  if (!m_parent) 
    return "/"; // root path
  else
    return m_parent->path() + '/' + get_metadata_as<std::string>("name");
}

std::shared_ptr<DummyNode> create_dummy_hierarchy (std::string const &root_name, unsigned int depth) {
  std::shared_ptr<DummyNode> root(new DummyNode(root_name));

  if (depth == 0)
    return root;

  // non-leaf children
  unsigned int non_leaf_child_count = (rand() % 3) + 3;
  for (unsigned int i = 0; i < non_leaf_child_count; ++i) {
    root->appendChildNode(create_dummy_hierarchy(FORMAT("dir" << i), depth-1));
  }

  // leaf children
  unsigned int leaf_child_count = (rand() % 3) + 3;
  for (unsigned int i = 0; i < leaf_child_count; ++i) {
    root->appendChildNode(std::shared_ptr<DummyNode>(new DummyNode(FORMAT("file" << i), root)));
  }

  return root;
}
