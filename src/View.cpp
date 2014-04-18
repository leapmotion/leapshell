#include "StdAfx.h"
#include "NavigationState.h"
#include "View.h"
#include <algorithm>

View::View(std::shared_ptr<NavigationState> const &ownerNavigationState)
  :
  m_ownerNavigationState(ownerNavigationState)
{
  m_position = 50 * Vector3::UnitZ();
  m_lookat = Vector3::Zero();
  m_up = Vector3::UnitY();
  m_fov = 80.0f;
  m_near = 1.0f;
  m_far = 10000.0f;
  m_layout = std::shared_ptr<Layout>(new GridLayout());
  m_tiles.resize(100);
}

View::~View () {
}

void View::Update() {
  m_layout->UpdateTiles(m_sortedChildren, m_tiles);
}

void View::ApplyVelocity(const Vector3& velocity, double deltaTime) {
  const Vector3 deltaPosition = velocity * deltaTime;

  m_position += deltaPosition;
  m_lookat += deltaPosition;

  static const double RUBBER_BAND_SPEED = 0.3333;
  const Vector3 clampedPosition = clampCameraPosition(m_position);
  const Vector3 rubberBandForce = 0.3333 * (clampedPosition - m_position);
 
  m_position += rubberBandForce;
  m_lookat += rubberBandForce;
}

void View::SetPosition(const Vector3& position) {
  m_position = clampCameraPosition(position);
}

void View::SetLookAt(const Vector3& lookat) {
  m_lookat = clampCameraPosition(lookat);
}

Vector3 View::clampCameraPosition(const Vector3& position) const {
  Vector3 result(position);
  result.head<2>() = result.head<2>().cwiseMax(m_layout->GetCameraMinBounds()).cwiseMin(m_layout->GetCameraMaxBounds());
  return result;
}
