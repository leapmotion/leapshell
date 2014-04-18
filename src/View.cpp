#include "StdAfx.h"
#include "View.h"

Tile::Tile() {
  m_position = 100 * Vector3::Random();
  m_position.z() = 0.0;
  m_size = Vector3::Constant(10);
  m_name = "Node";
}

View::View(NavigationState &ownerNavigationState) {
  m_position = 100 * Vector3::UnitZ();
  m_lookat = Vector3::Zero();
  m_up = Vector3::UnitY();
  m_fov = 80.0f;
  m_near = 1.0f;
  m_far = 10000.0f;

  static const int NUM_GRID = 10;
  const double size = 100;
  for (int i=0; i<NUM_GRID; i++) {
    const double ratioX = static_cast<double>(i) / static_cast<double>(NUM_GRID-1);
    const double x = -(size/2.0) + ratioX*size;
    for (int j=0; j<NUM_GRID; j++) {
      const double ratioY = static_cast<double>(j) / static_cast<double>(NUM_GRID-1);
      const double y = -(size/2.0) + ratioY*size;
      m_tiles.push_back(Tile());
      m_tiles.back().m_position << x, y, 0.0;
    }
  }
}
