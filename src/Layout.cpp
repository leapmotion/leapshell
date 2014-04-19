#include "StdAfx.h"
#include "Layout.h"
#include "Utilities.h"
#include "Globals.h"

// SizeLayout

SizeLayout::SizeLayout() : m_creationTime(Globals::curTimeSeconds) {

}

void SizeLayout::animateTileSize(Tile& tile, int idx, const Vector3& newSize) const {
  static const float SMOOTH_VARIATION_RADIUS = (1.0f - Tile::SIZE_SMOOTH)/2.0f;
  const float mult = SmootherStep(static_cast<float>(std::min(1.0, 2 * (Globals::curTimeSeconds - m_creationTime))));
  const float smoothVariation = SMOOTH_VARIATION_RADIUS * static_cast<float>(std::cos(3*Globals::curTimeSeconds + idx));
  const float smooth = mult*(Tile::SIZE_SMOOTH + smoothVariation) + (1.0f - mult);
  tile.m_sizeSmoother.Update(newSize, Globals::curTimeSeconds, smooth);
  tile.m_size = tile.m_sizeSmoother.value;
}

// UniformSizeLayout

UniformSizeLayout::UniformSizeLayout() : m_size(Vector3::Constant(15.0)) { }

void UniformSizeLayout::UpdateTileSizes(TilePointerVector::iterator tile_start, TilePointerVector::iterator tile_end) {
  int idx = 0;
  for (auto it = tile_start; it != tile_end; ++it, ++idx) {
    animateTileSize(**it, idx, m_size);
  }
}

// PositionLayout

PositionLayout::PositionLayout() : m_creationTime(Globals::curTimeSeconds) {

}

void PositionLayout::animateTilePosition(Tile& tile, int idx, const Vector3& newPosition) const {
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

void GridLayout::UpdateTilePositions(TilePointerVector::iterator tile_start, TilePointerVector::iterator tile_end) {
  // compute number of rows and height of the layout
  static const int TILES_PER_ROW = 6;
  const double inc = m_width / (TILES_PER_ROW-1);
  const int NUM_ROWS = static_cast<int>(std::ceil(static_cast<double>(tile_end - tile_start) / TILES_PER_ROW));
  m_height = inc * NUM_ROWS;

  // start placing tiles
  const double halfWidth = m_width/2.0;
  const double halfHeight = m_height/2.0;
  double curWidth = -halfWidth;
  double curHeight = halfHeight - inc/2.0;
  int idx = 0;
  for (auto it = tile_start; it != tile_end; ++it, ++idx) {
    animateTilePosition(**it, idx, Vector3(curWidth, curHeight, 0.0));

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

void RingLayout::UpdateTilePositions(TilePointerVector::iterator tile_start, TilePointerVector::iterator tile_end) {
  double theta = 0;
  const double thetaInc = 2*M_PI / static_cast<double>(tile_end - tile_start);
  int idx = 0;
  for (auto it = tile_start; it != tile_end; ++it, ++idx) {
    const double x = m_radius * std::cos(theta);
    const double y = m_radius * std::sin(theta);
    animateTilePosition(**it, idx, Vector3(x, y, 0.0));
    theta += thetaInc;
  }
}

Vector2 RingLayout::GetCameraMinBounds() const {
  return Vector2(-m_radius/2.0, -m_radius/2.0);
}

Vector2 RingLayout::GetCameraMaxBounds() const {
  return Vector2(m_radius/2.0, m_radius/2.0);
}

// LinearSpiralLayout

LinearSpiralLayout::LinearSpiralLayout() : m_startingAngle(2.0*M_PI), m_slope(3.0), m_radius(0.0) {

}

void LinearSpiralLayout::UpdateTilePositions(TilePointerVector::iterator tile_start, TilePointerVector::iterator tile_end) {
  double theta = m_startingAngle;
  double radius;
  int idx = 0;
  for (auto it = tile_start; it != tile_end; ++it, ++idx) {
    Tile &tile = **it;
    radius = m_slope * theta;
    const double x = radius * std::cos(theta);
    const double y = radius * std::sin(theta);
    animateTilePosition(tile, idx, Vector3(x, y, 0.0));
    // theta += m_spacing;
    // Calculate the next angle based on the speed at which the spiral is being swept out.
    // we want to go along the arc a length of about the tile diameter.  This should make
    // it so that the tiles don't overlap. 
    // length = radius * change_in_theta, so change_in_theta = length / radius.
    Vector2 tile_size_in_2d(tile.m_size(0), tile.m_size(1));
    // divide by sqrt(2) to get a better approx of the shape of the icons we use
    double tile_diameter = tile_size_in_2d.norm() * 0.707;
    theta += tile_diameter / radius;
  }
  m_radius = radius;
}

Vector2 LinearSpiralLayout::GetCameraMinBounds() const {
  return Vector2(-m_radius/2.0, -m_radius/2.0);
}

Vector2 LinearSpiralLayout::GetCameraMaxBounds() const {
  return Vector2(m_radius/2.0, m_radius/2.0);
}
