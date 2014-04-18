#ifndef __NAVIGATIONSTATE_H__
#define __NAVIGATIONSTATE_H__

#include <memory>
#include <set>
#include <vector>

class HierarchyNode;
class View;

class NavigationState {
public:

  typedef std::vector<std::shared_ptr<HierarchyNode>> NodeVector;

  NavigationState() { }
  std::shared_ptr<HierarchyNode> currentLocation() const { return m_currentLocation; }
  NodeVector const &currentChildNodes() { return m_currentChildNodes; }

  bool navigateUp();
  bool navigateDown(std::shared_ptr<HierarchyNode> const &targetNode);

  void registerView(View &view);
  void unregisterView(View &view);

  void update();

private:

  std::shared_ptr<HierarchyNode> m_currentLocation;
  NodeVector m_currentChildNodes;

  typedef std::set<std::weak_ptr<View>> ViewSet;
  ViewSet m_views;
};

#endif
