#ifndef __NAVIGATIONSTATE_H__
#define __NAVIGATIONSTATE_H__

#include "HierarchyNode.h" // this is really just for the HierarchyNodeVector typedef, which could be moved to a smaller header file

class View;

/// @brief Stores the "view location" in a node hierarchy, and provides navigation methods.
/// @details Instances of View can be attached to an instance of NavigationState.  Each View
/// instance corresponds literally to a view window.  There can be multiple View instances per
/// NavigationState.  Each View instance is responsible for sorting and visual node layout.
/// When this class is issued a navigation command (@see setCurrentLocation, @see navigateUp,
/// @see navigateDown), it calls @see View::UpdateFromChangedNavigationState on all attached
/// views.
///
/// The NavigationState is also responsible for caching node data at the current location.
/// This is why @see NavigationState::currentChildNodes exists -- so that the hierarchy can
/// be queried infrequently and some caching can be done for the views.  This is likely where
/// node pre-caching would be done, if that is decided to be a worthwhile feature.
class NavigationState {
public:

  /// @brief Constructor initializes to a not-pointing-at-anything state.
  NavigationState();
  ~NavigationState();

  /// @brief Returns the current node.
  std::shared_ptr<HierarchyNode> currentLocation() const { return m_currentLocation; }
  /// @brief Returns the child nodes of the current node.
  HierarchyNodeVector const &currentChildNodes() {
    boost::lock_guard<boost::mutex> lock(m_mutex);
    return m_currentChildNodes;
  }

  /// @brief Sets the current node to anything, without regard to an "up"/"down" traversal.
  void setCurrentLocation (std::shared_ptr<HierarchyNode> const &newLocation);

  /// @brief Navigate to the current node's parent, or do nothing if this is the root node.
  /// @returns true if the navigation succeeded (basically if and only if this wasn't the
  /// root node).
  bool navigateUp();
  /// @brief Navigate to the specified child node.
  /// @details This is sort of redundant (@see setCurrentLocation), but provides a check
  /// that the navigation is actually to a child node.
  bool navigateDown(std::shared_ptr<HierarchyNode> const &targetNode);

  /// @brief Attaches a view to this NavigationState.
  /// @details All attached views are notified (via @see View::UpdateFromChangedNavigationState)
  /// of updates to this NavigationState.
  void registerView(std::weak_ptr<View> const &view);
  /// @brief Detaches a view from this NavigationState.
  /// @details The view must have been attached to begin with.
  void unregisterView(std::weak_ptr<View> const &view);

  /// @brief Causes this NavigationState instance to re-query the current location in the hierarchy.
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
