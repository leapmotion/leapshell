#include "StdAfx.h"
#include "HierarchyNode.h"

FilterCriteria const FilterCriteria::NONE;

void HierarchyNode::all_ancestors(std::function<void(const std::shared_ptr<HierarchyNode>&)> callback) {
  std::shared_ptr<HierarchyNode> curParent = parent();
  if (curParent) {
    callback(shared_from_this());
    curParent->all_ancestors(callback);
  }
}
