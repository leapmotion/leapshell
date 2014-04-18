#include "StdAfx.h"
#include "View.h"
#include <algorithm>

View::View(NavigationState &ownerNavigationState) {
  m_position = 50 * Vector3::UnitZ();
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

void View::SetPosition(const Vector3& position) {
  Vector3 clampedPosition(position);
  clampCameraPosition(clampedPosition);
  m_position = clampedPosition;
}

void View::SetLookAt(const Vector3& lookat) {
  Vector3 clampedLookat(lookat);
  clampCameraPosition(clampedLookat);
  m_lookat = clampedLookat;
}

void View::clampCameraPosition(Vector3& position) const {
  const Vector2 boundsX = m_layout->GetCameraBoundsX();
  const Vector2 boundsY = m_layout->GetCameraBoundsY();
  position.x() = std::max(std::min(position.x(), boundsX[1]), boundsX[0]);
  position.y() = std::max(std::min(position.y(), boundsY[1]), boundsY[0]);
}
