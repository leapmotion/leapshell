#include "StdAfx.h"
#include "Layout.h"

// GridLayout

GridLayout::GridLayout() : m_width(100) {

}

void GridLayout::UpdateTiles(const HierarchyNodeVector &nodes, TileVector& tiles) {
  static const int NUM_GRID = 10;
  int tile = 0;
  for (int i=0; i<NUM_GRID; i++) {
    const double ratioX = static_cast<double>(i) / static_cast<double>(NUM_GRID-1);
    const double x = -(m_width/2.0) + ratioX*m_width;
    for (int j=0; j<NUM_GRID; j++) {
      const double ratioY = static_cast<double>(j) / static_cast<double>(NUM_GRID-1);
      const double y = -(m_width/2.0) + ratioY*m_width;
      tiles[tile++].m_position << x, y, 0.0;
    }
  }
}

Vector2 GridLayout::GetCameraMinBounds() const {
  return Vector2(0, -m_width/4.0);
}

Vector2 GridLayout::GetCameraMaxBounds() const {
  return Vector2(0, m_width/4.0);
}

// RingLayout

RingLayout::RingLayout() : m_radius(50) {

}

void RingLayout::UpdateTiles(const HierarchyNodeVector &nodes, TileVector& tiles) {
  double theta = 0;
  const double thetaInc = 2*M_PI / static_cast<double>(tiles.size());
  for (size_t i=0; i<tiles.size(); i++) {
    const double x = m_radius * std::cos(theta);
    const double y = m_radius * std::sin(theta);
    tiles[i].m_position << x, y, 0.0;
    theta += thetaInc;
  }
}

Vector2 RingLayout::GetCameraMinBounds() const {
  return Vector2(-m_radius/2.0, -m_radius/2.0);
}

Vector2 RingLayout::GetCameraMaxBounds() const {
  return Vector2(m_radius/2.0, m_radius/2.0);
}
