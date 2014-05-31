#include "StdAfx.h"
#include "NavigationState.h"
#include "View.h"

NavigationState::NavigationState() :
  m_running(false),
  m_dirty(false),
  m_first(false),
  m_updateNumber(0),
  m_updateThread(boost::bind(&NavigationState::updateThread, this))
{
  boost::unique_lock<boost::mutex> lock(m_mutex);
  m_cond.wait(lock, [this] { return m_running; });
}

NavigationState::~NavigationState()
{
  boost::unique_lock<boost::mutex> lock(m_mutex);
  m_running = false;
  m_cond.notify_all();
  lock.unlock();
  m_updateThread.join();
}

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
  v->UpdateFromChangedNavigationState();
}

void NavigationState::unregisterView (std::weak_ptr<View> const &view) {
  auto it = m_views.find(view);
  if (it != m_views.end()) {
    m_views.erase(it);
  }
}

void NavigationState::update () {
  boost::unique_lock<boost::mutex> lock(m_mutex);
  ++m_updateNumber;
  m_cond.notify_all();
}

void NavigationState::updateThread() {
  boost::unique_lock<boost::mutex> lock(m_mutex);
  m_running = true;
  m_cond.notify_all();
  uint32_t updateNumber = m_updateNumber;
  while (m_running) {
    m_cond.wait(lock, [this, updateNumber] { return !m_running || updateNumber != m_updateNumber; });
    if (m_running) {
      updateNumber = m_updateNumber;
      m_dirty = true;
      m_first = true;
      m_currentChildNodes.clear();
      m_currentOrderedPathNodes.clear();

      // if the current location is valid, query its child nodes
      if (m_currentLocation) {
        m_currentLocation->child_nodes([this, updateNumber] (const std::shared_ptr<HierarchyNode>& child) -> bool {
          boost::lock_guard<boost::mutex> lock(m_mutex);
          if (updateNumber != m_updateNumber) {
            return false;
          }
          m_currentChildNodes.push_back(child);
          m_dirty = true;
          return true;
        });

        m_currentLocation->all_ancestors([this] (const std::shared_ptr<HierarchyNode>& child) {
          boost::lock_guard<boost::mutex> lock(m_ancestorMutex);
          m_currentOrderedPathNodes.push_back(child);
        });
      }
    }
  }
}

bool NavigationState::updateChildren()
{
  boost::unique_lock<boost::mutex> lock(m_mutex);
  if (!m_dirty) {
    return false;
  }
  m_dirty = false;
  bool isFirst = m_first;
  if (!m_currentChildNodes.empty()) {
    m_first = false;
  }
  lock.unlock();

  // inform all views that there has been an update
  auto it = m_views.begin();
  while (it != m_views.end()) {
    auto v = it->lock();
    if (v) {
      v->UpdateFromChangedNavigationState(isFirst);
      ++it;
    } else {
      m_views.erase(it++);
    }
  }
  return true;
}
