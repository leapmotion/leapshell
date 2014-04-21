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

void UniformSizeLayout::UpdateTileSizes(const Range<TilePointerVector::iterator> &tiles) {
  int idx = 0;
  for (auto t = tiles; t.is_not_at_end(); ++t, ++idx) {
    animateTileSize(**t, idx, m_size);
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

void GridLayout::UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles) {
  // compute number of rows and height of the layout
  static const int TILES_PER_ROW = 6;
  const double inc = m_width / (TILES_PER_ROW-1);
  const int NUM_ROWS = static_cast<int>(std::ceil(static_cast<double>(tiles.length()) / TILES_PER_ROW));
  m_height = inc * NUM_ROWS;

  // start placing tiles
  const double halfWidth = m_width/2.0;
  const double halfHeight = m_height/2.0;
  double curWidth = -halfWidth;
  double curHeight = halfHeight - inc/2.0;
  int idx = 0;
  for (auto t = tiles; t.is_not_at_end(); ++t, ++idx) {
    animateTilePosition(**t, idx, Vector3(curWidth, curHeight, 0.0));

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

void RingLayout::UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles) {
  double theta = 0;
  const double thetaInc = 2*M_PI / static_cast<double>(tiles.length());
  int idx = 0;
  for (auto t = tiles; t.is_not_at_end(); ++t, ++idx) {
    const double x = m_radius * std::cos(theta);
    const double y = m_radius * std::sin(theta);
    animateTilePosition(**t, idx, Vector3(x, y, 0.0));
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

LinearSpiralLayout::LinearSpiralLayout() : m_startingAngle(2.0*M_PI), m_slope(3.0), m_boundingRadius(0.0) {

}

void LinearSpiralLayout::UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles) {
  double theta = m_startingAngle;
  double radius;
  int idx = 0;
  for (auto t = tiles; t.is_not_at_end(); ++t, ++idx) {
    Tile &tile = **t;
    radius = m_slope * theta;
    const double x = radius * std::cos(theta);
    const double y = radius * std::sin(theta);
    animateTilePosition(tile, idx, Vector3(x, y, 0.0));
    // Calculate the next angle based on the speed at which the spiral is being swept out.
    // we want to go along the arc a length of about the tile diameter.  This should make
    // it so that the tiles don't overlap. 
    // length = radius * change_in_theta, so change_in_theta = length / radius.
    Vector2 tileSizeIn2d(tile.m_size(0), tile.m_size(1));
    // divide by sqrt(2) to get a better approx of the shape of the icons we use
    double tile_diameter = tileSizeIn2d.norm() * 0.707;
    theta += tile_diameter / radius;
  }
  m_boundingRadius = radius;
}

Vector2 LinearSpiralLayout::GetCameraMinBounds() const {
  return Vector2(-m_boundingRadius/2.0, -m_boundingRadius/2.0);
}

Vector2 LinearSpiralLayout::GetCameraMaxBounds() const {
  return Vector2(m_boundingRadius/2.0, m_boundingRadius/2.0);
}

// ExponentialSpiralLayout

ExponentialSpiralLayout::ExponentialSpiralLayout()
  :
  m_baseTileSize(Vector3::Constant(8.0)),
  m_spacing(1.0)
{
  SetBaseRadius(5.0);
}

void ExponentialSpiralLayout::UpdateTileSizes(const Range<TilePointerVector::iterator> &tiles) {
  int tile_count = tiles.length();
  int idx = 0;
  for (auto t = tiles; t.is_not_at_end(); ++t, ++idx) {
    double centeredIndex = (idx - tile_count/2.0);
    double theta = m_thetaIncrement*centeredIndex;
    double ratio = std::exp(m_exponentialRate*theta);
    animateTileSize(**t, idx, ratio*m_baseTileSize);
  }
}

void ExponentialSpiralLayout::UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles) {
  Vector2 baseTileSizeIn2d(m_baseTileSize(0), m_baseTileSize(1));
  // divide by sqrt(2) to get a better approx of the shape of the icons we use
  double baseTileDiameter = baseTileSizeIn2d.norm() * 0.707;
  double baseTileRadius = baseTileDiameter / 2.0;
  int tile_count = tiles.length();
  int idx = 0;
  double radius = 0.0;
  for (auto t = tiles; t.is_not_at_end(); ++t, ++idx) {
    double centeredIndex = (idx - tile_count/2.0);
    double theta = m_thetaIncrement*centeredIndex;
    double ratio = m_spacing * std::exp(m_exponentialRate*theta);
    radius = m_baseRadius * baseTileRadius * ratio;
    const double x = radius * std::cos(theta);
    const double y = radius * std::sin(theta);
    animateTilePosition(**t, idx, Vector3(x, y, 0.0));
  }
  m_boundingRadius = radius;
}

Vector2 ExponentialSpiralLayout::GetCameraMinBounds() const {
  return Vector2(-m_boundingRadius/2.0, -m_boundingRadius/2.0);
}

Vector2 ExponentialSpiralLayout::GetCameraMaxBounds() const {
  return Vector2(m_boundingRadius/2.0, m_boundingRadius/2.0);
}

void ExponentialSpiralLayout::SetSpacing(double spacing) {
  m_spacing = std::max(1.0, spacing);
  SetBaseRadius(m_baseRadius);
}

void ExponentialSpiralLayout::SetBaseRadius(double baseRadius) {
  assert(baseRadius > 1.0);
  m_baseRadius = baseRadius;
  m_thetaIncrement = 2.0 / (m_baseRadius * m_spacing);
  double c = (m_baseRadius + 1.0) / (m_baseRadius - 1.0);
  m_exponentialRate = std::log(c) / (2.0*M_PI);
}

// BlobClusterLayout

// BlobClusterLayout::BlobClusterLayout ()
//   :
//   m_clusteringKey("name"),
//   m_clusterOuterLayout(new GridLayout()),
//   m_clusterInnerLayout(new LinearSpiralLayout())
// { }

// void BlobClusterLayout::UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles) {

//   // compute iterator ranges for each cluster, because we need to know how many there are.


//   // calculate the maximum bounding box size of each cluster


//   Vector2 baseTileSizeIn2d(m_baseTileSize(0), m_baseTileSize(1));
//   // divide by sqrt(2) to get a better approx of the shape of the icons we use
//   double baseTileDiameter = baseTileSizeIn2d.norm() * 0.707;
//   double baseTileRadius = baseTileDiameter / 2.0;
//   int tile_count = tiles.length();
//   int idx = 0;
//   double radius = 0.0;
//   for (auto t = tiles; t.is_not_at_end(); ++t, ++idx) {
//     double centeredIndex = (idx - tile_count/2.0);
//     double theta = m_thetaIncrement*centeredIndex;
//     double ratio = m_spacing * std::exp(m_exponentialRate*theta);
//     radius = m_baseRadius * baseTileRadius * ratio;
//     const double x = radius * std::cos(theta);
//     const double y = radius * std::sin(theta);
//     animateTilePosition(**t, idx, Vector3(x, y, 0.0));
//   }
//   m_boundingRadius = radius;
// }

// Vector2 BlobClusterLayout::GetCameraMinBounds() const {
//   return Vector2(-m_boundingRadius/2.0, -m_boundingRadius/2.0);
// }

// Vector2 BlobClusterLayout::GetCameraMaxBounds() const {
//   return Vector2(m_boundingRadius/2.0, m_boundingRadius/2.0);
// }



