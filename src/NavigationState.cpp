#include "StdAfx.h"
#include "NavigationState.h"
#include "View.h"

void NavigationState::setCurrentLocation (std::shared_ptr<HierarchyNode> const &newLocation) {
  // do nothing if this is a no-op.
  if (m_currentLocation != newLocation) {
    m_currentLocation = newLocation;
    update();
  }
}

bool NavigationState::navigateUp () {
  std::shared_ptr<HierarchyNode> parent(m_currentLocation->parent());
  // if there's a parent, go to it and update.  otherwise we're at the root, so do nothing.
  if (parent) {
    m_currentLocation = parent;
    update();
  }
  return !!parent; // navigation succeeded if there was a parent
}

bool NavigationState::navigateDown (std::shared_ptr<HierarchyNode> const &targetNode) {
  assert(targetNode->parent() == m_currentLocation && "targetNode is not a child of current location");
  m_currentLocation = targetNode;
  update();
  return true; // can't fail except by programmer error
}

void NavigationState::registerView (std::weak_ptr<View> const &view) {
  assert(!view.expired() && "view must be a valid weak_ptr");
  m_views.insert(view);
  auto v = view.lock();
  v->Update();
}

void NavigationState::unregisterView (std::weak_ptr<View> const &view) {
  auto it = m_views.find(view);
  if (it != m_views.end()) {
    m_views.erase(it);
  }
}

void NavigationState::update () {
  m_currentChildNodes.clear();
  // if the current location is valid, re-query its child nodes
  if (m_currentLocation) {
    m_currentChildNodes = m_currentLocation->child_nodes();
  } 

  // inform all views that there has been an update
  auto it = m_views.begin();
  while (it != m_views.end()) {
    auto v = it->lock();
    if (v) {
      v->Update();
      ++it;
    } else {
      m_views.erase(it++);
    }
  }
}
