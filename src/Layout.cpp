#include "StdAfx.h"
#include "Layout.h"
#include "Utilities.h"
#include "Globals.h"

// Layout

Layout::Layout() : m_creationTime(Globals::curTimeSeconds) {

}

void Layout::animateTilePosition(Tile& tile, int idx, const Vector3& newPosition) const {
  static const float SMOOTH_VARIATION_RADIUS = (1.0f - Tile::POSITION_SMOOTH)/2.0f;
  const float mult = SmootherStep(static_cast<float>(std::min(1.0, 2 * (Globals::curTimeSeconds - m_creationTime))));
  const float smoothVariation = SMOOTH_VARIATION_RADIUS * static_cast<float>(std::cos(3*Globals::curTimeSeconds + idx));
  const float smooth = mult*(Tile::POSITION_SMOOTH + smoothVariation) + (1.0f - mult);
  tile.m_positionSmoother.Update(newPosition, Globals::curTimeSeconds, smooth);
  tile.m_position = tile.m_positionSmoother.value;
}

// GridLayout

GridLayout::GridLayout() : m_width(100), m_height(m_width) {

}

void GridLayout::UpdateTiles(TilePointerVector &tiles) {
  // compute number of rows and height of the layout
  static const int TILES_PER_ROW = 6;
  const double inc = m_width / (TILES_PER_ROW-1);
  const int NUM_ROWS = static_cast<int>(std::ceil(static_cast<double>(tiles.size()) / TILES_PER_ROW));
  m_height = inc * NUM_ROWS;

  // start placing tiles
  const double halfWidth = m_width/2.0;
  const double halfHeight = m_height/2.0;
  double curWidth = -halfWidth;
  double curHeight = halfHeight - inc/2.0;
  for (size_t i=0; i<tiles.size(); i++) {
    animateTilePosition(*tiles[i], i, Vector3(curWidth, curHeight, 0.0));

    curWidth += inc;
    if (curWidth > halfWidth) {
      curWidth = -halfWidth;
      curHeight -= inc;
    }
  }

  m_height = (halfHeight - curHeight);
}

Vector2 GridLayout::GetCameraMinBounds() const {
  return Vector2(0, -m_height/2.0);
}

Vector2 GridLayout::GetCameraMaxBounds() const {
  return Vector2(0, m_height/2.0);
}

// RingLayout

RingLayout::RingLayout() : m_radius(50) {

}

void RingLayout::UpdateTiles(TilePointerVector &tiles) {
  double theta = 0;
  const double thetaInc = 2*M_PI / static_cast<double>(tiles.size());
  for (size_t i=0; i<tiles.size(); i++) {
    const double x = m_radius * std::cos(theta);
    const double y = m_radius * std::sin(theta);
    animateTilePosition(*tiles[i], i, Vector3(x, y, 0.0));
    theta += thetaInc;
  }
}

Vector2 RingLayout::GetCameraMinBounds() const {
  return Vector2(-m_radius/2.0, -m_radius/2.0);
}

Vector2 RingLayout::GetCameraMaxBounds() const {
  return Vector2(m_radius/2.0, m_radius/2.0);
}
