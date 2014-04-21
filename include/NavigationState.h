#ifndef __NAVIGATIONSTATE_H__
#define __NAVIGATIONSTATE_H__

#include "HierarchyNode.h" // this is really just for the HierarchyNodeVector typedef, which could be moved to a smaller header file

class View;

class NavigationState {
public:

  // starts in a not-pointing-at-anything state.
  NavigationState();
  ~NavigationState();

  std::shared_ptr<HierarchyNode> currentLocation() const { return m_currentLocation; }
  HierarchyNodeVector const &currentChildNodes() {
    boost::lock_guard<boost::mutex> lock(m_mutex);
    return m_currentChildNodes;
  }

  void setCurrentLocation (std::shared_ptr<HierarchyNode> const &newLocation);

  bool navigateUp();
  bool navigateDown(std::shared_ptr<HierarchyNode> const &targetNode);

  void registerView(std::weak_ptr<View> const &view);
  void unregisterView(std::weak_ptr<View> const &view);

  void update();
  bool updateChildren();

private:
  void updateThread();

  std::shared_ptr<HierarchyNode> m_currentLocation;
  HierarchyNodeVector m_currentChildNodes;

  bool m_running;
  bool m_dirty;
  bool m_first;
  uint32_t m_updateNumber;
  boost::thread m_updateThread;
  boost::thread m_childThread;
  boost::condition_variable m_cond;
  boost::mutex m_mutex;

  typedef std::set<std::weak_ptr<View>, std::owner_less<std::weak_ptr<View>>> ViewSet;
  ViewSet m_views;
};

#endif
