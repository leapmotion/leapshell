#include "StdAfx.h"
#include "DummyNode.h"
#include "Utilities.h"

#include <cstdlib> // for rand()
#include <sstream> // for ostringstream used in FORMAT

DummyNode::DummyNode (std::string const &name, std::shared_ptr<DummyNode> const &parent)
  :
  m_parent(parent)
{
  set_name(name);
}

void DummyNode::set_name (std::string const &name) {
  if (name.empty())
    throw std::invalid_argument("DummyNode name must not be empty");
  if (name.find('/') != std::string::npos)
    throw std::invalid_argument("DummyNode name can not contain '/' character");
  m_name = name;
}

std::string DummyNode::path () const {
  if (!m_parent) 
    return "/"; // root path
  else
    return m_parent->path() + '/' + m_name;
}

std::shared_ptr<DummyNode> create_dummy_hierarchy (std::string const &root_name, uint depth) {
  std::shared_ptr<DummyNode> root(new DummyNode(root_name));

  if (depth == 0)
    return root;

  // non-leaf children
  uint non_leaf_child_count = (rand() % 3) + 3;
  for (uint i = 0; i < non_leaf_child_count; ++i) {
    root->appendChildNode(create_dummy_hierarchy(FORMAT("dir" << i), depth-1));
  }

  // leaf children
  uint leaf_child_count = (rand() % 3) + 3;
  for (uint i = 0; i < leaf_child_count; ++i) {
    root->appendChildNode(std::shared_ptr<DummyNode>(new DummyNode(FORMAT("file" << i), root)));
  }

  return root;
}
