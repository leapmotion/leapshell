#include "StdAfx.h"
#include "Layout.h"

GridLayout::GridLayout() : m_width(100) {

}

void GridLayout::UpdateTiles(const std::vector<std::shared_ptr<HierarchyNode>> nodes, TileVector& tiles) {
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
