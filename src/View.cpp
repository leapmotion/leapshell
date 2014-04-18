#include "StdAfx.h"
#include "View.h"

View::View(NavigationState &ownerNavigationState) {
  m_position = 100 * Vector3::UnitZ();
  m_lookat = Vector3::Zero();
  m_up = Vector3::UnitY();
  m_fov = 80.0f;
  m_near = 1.0f;
  m_far = 10000.0f;
  m_layout = std::shared_ptr<Layout>(new GridLayout());
  m_tiles.resize(100);
}

void View::Update() {
  m_layout->UpdateTiles(m_sortedChildren, m_tiles);
}
