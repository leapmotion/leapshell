#ifndef __NAVIGATIONSTATE_H__
#define __NAVIGATIONSTATE_H__

#include <memory>
#include <set>
#include <vector>

#include "HierarchyNode.h" // this is really just for the HierarchyNodeVector typedef, which could be moved to a smaller header file

class View;

class NavigationState {
public:

	// starts in a not-pointing-at-anything state.
  NavigationState() { }

  std::shared_ptr<HierarchyNode> currentLocation() const { return m_currentLocation; }
  HierarchyNodeVector const &currentChildNodes() { return m_currentChildNodes; }

  void setCurrentLocation (std::shared_ptr<HierarchyNode> const &newLocation);

  bool navigateUp();
  bool navigateDown(std::shared_ptr<HierarchyNode> const &targetNode);

  void registerView(std::weak_ptr<View> const &view);
  void unregisterView(std::weak_ptr<View> const &view);

  void update();

private:

  std::shared_ptr<HierarchyNode> m_currentLocation;
  HierarchyNodeVector m_currentChildNodes;

  typedef std::set<std::weak_ptr<View>, std::owner_less<std::weak_ptr<View>>> ViewSet;
  ViewSet m_views;
};

#endif
