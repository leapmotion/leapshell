#include "StdAfx.h"
#include "Layout.h"
#include "Utilities.h"
#include "Globals.h"

// SizeLayout

SizeLayout::SizeLayout() : m_creationTime(Globals::curTimeSeconds) {

}

void SizeLayout::animateTileSize(Tile& tile, int idx, const Vector3& newSize) const {
  static const float SMOOTH_VARIATION_RADIUS = (1.0f - Tile::SIZE_SMOOTH)/2.0f;
  const float mult = SmootherStep(static_cast<float>(std::min(1.0, (Globals::curTimeSeconds - m_creationTime)/Globals::TRANSITION_TIME)));
  const float smoothVariation = SMOOTH_VARIATION_RADIUS * static_cast<float>(std::cos(3*Globals::curTimeSeconds + idx));
  const float smooth = mult*(Tile::SIZE_SMOOTH + smoothVariation) + (1.0f - mult);
  tile.UpdateSize(newSize, smooth);
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
  const float mult = SmootherStep(static_cast<float>(std::min(1.0, (Globals::curTimeSeconds - m_creationTime)/Globals::TRANSITION_TIME)));
  const float smoothVariation = SMOOTH_VARIATION_RADIUS * static_cast<float>(std::cos(3*Globals::curTimeSeconds + idx));
  const float smooth = mult*(Tile::POSITION_SMOOTH + smoothVariation) + (1.0f - mult);
  tile.UpdatePosition(newPosition, smooth);
}

// GridLayout

GridLayout::GridLayout() : m_width(100), m_height(m_width) {

}

void GridLayout::UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles, bool updatePhantomPosition) {
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
    if (updatePhantomPosition) {
      (**t).m_phantomPosition = Vector3(curWidth, curHeight, 0.0);
    } else {
      animateTilePosition(**t, idx, Vector3(curWidth, curHeight, 0.0));
    }

    curWidth += inc;
    if (curWidth > halfWidth) {
      curWidth = -halfWidth;
      curHeight -= inc;
    }
  }
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

void RingLayout::UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles, bool updatePhantomPosition) {
  double theta = 0;
  const double thetaInc = 2*M_PI / static_cast<double>(tiles.length());
  int idx = 0;
  for (auto t = tiles; t.is_not_at_end(); ++t, ++idx) {
    const double x = m_radius * std::cos(theta);
    const double y = m_radius * std::sin(theta);
    if (updatePhantomPosition) {
      (**t).m_phantomPosition = Vector3(x, y, 0.0);
    } else {
      animateTilePosition(**t, idx, Vector3(x, y, 0.0));
    }
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

void LinearSpiralLayout::UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles, bool updatePhantomPosition) {
  double theta = m_startingAngle;
  double radius;
  int idx = 0;
  for (auto t = tiles; t.is_not_at_end(); ++t, ++idx) {
    Tile &tile = **t;
    radius = m_slope * theta;
    const double x = radius * std::cos(theta);
    const double y = radius * std::sin(theta);
    if (updatePhantomPosition) {
      (**t).m_phantomPosition = Vector3(x, y, 0.0);
    } else {
      animateTilePosition(tile, idx, Vector3(x, y, 0.0));
    }
    // Calculate the next angle based on the speed at which the spiral is being swept out.
    // we want to go along the arc a length of about the tile diameter.  This should make
    // it so that the tiles don't overlap. 
    // length = radius * change_in_theta, so change_in_theta = length / radius.
    Vector2 tileSizeIn2d(tile.Size()(0), tile.Size()(1));
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

void ExponentialSpiralLayout::UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles, bool updatePhantomPosition) {
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
    if (updatePhantomPosition) {
      (**t).m_phantomPosition = Vector3(x, y, 0.0);
    } else {
      animateTilePosition(**t, idx, Vector3(x, y, 0.0));
    }
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

BlobClusterLayout::BlobClusterLayout ()
  :
  m_clusteringKey("ext"), // this should be file type
  // m_clusterOuterLayout(new GridLayout()),
  m_clusterInnerSizeLayout(new UniformSizeLayout()),
  m_clusterInnerPositionLayout(new LinearSpiralLayout())
{ }

// NOTE: this updates the sizes of the tiles too
void BlobClusterLayout::UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles, bool updatePhantomPosition) {
  // compute iterator ranges for each cluster, because we need to know how many there are.
  {
    m_clusterTileRanges.clear();
    TilePointerVector::iterator clusterRangeStart = tiles.start();
    Tile &clusterRangeStartTile = **clusterRangeStart;
    Value clusterRangeStartKey = clusterRangeStartTile.Node()->metadata()[m_clusteringKey];
    for (auto t = tiles; t.is_not_at_end(); ++t) {
      TilePointerVector::iterator current = t.start();
      Tile &currentTile = **current;
      Value currentKey = currentTile.Node()->metadata()[m_clusteringKey];
      // we use Value::compare here, because Value::operator== compares the Value JSON strings
      if (currentKey.compare(clusterRangeStartKey) != 0) { // compare returns 0 when equal
        // add the current range to the cluster tile ranges and initialize a new range
        m_clusterTileRanges.push_back(range(clusterRangeStart, current));
        clusterRangeStart = current;
        clusterRangeStartKey = currentKey;
      }
    }
    // add the last range
    m_clusterTileRanges.push_back(range(clusterRangeStart, tiles.end()));
  }
#if 0
  // TEMP: print out the ranges to make sure the cluster range computation worked.
  for (auto r_it = m_clusterTileRanges.begin(); r_it != m_clusterTileRanges.end(); ++r_it) {
    Range<TilePointerVector::iterator> &r = *r_it;
    assert(r.length() > 0);
    Tile &firstTile = **r;
    std::cout << "range corresponding to key " << firstTile.Node()->metadata()[m_clusteringKey] << " {\n";
    for (auto t = r; t.is_not_at_end(); ++t) {
      Tile &tile = **t;
      std::cout << "    " << tile.Node()->metadata() << '\n';
    }
  }
  std::cout << '\n';
#endif
  // first, lay out the Tiles in each cluster in their own scene spaces, and
  // compute the maximum cluster size.
  Vector2 clusterSize(0.0, 0.0);
  for (auto r_it = m_clusterTileRanges.begin(), r_it_end = m_clusterTileRanges.end(); r_it != r_it_end; ++r_it) {
    Range<TilePointerVector::iterator> &r = *r_it;
    assert(r.length() > 0);
    m_clusterInnerSizeLayout->UpdateTileSizes(r);
    m_clusterInnerPositionLayout->UpdateTilePositions(r, true);
    Vector2 cameraMinBounds(m_clusterInnerPositionLayout->GetCameraMinBounds());
    Vector2 cameraMaxBounds(m_clusterInnerPositionLayout->GetCameraMaxBounds());
    Vector2 boundSize(std::abs(cameraMaxBounds(0) - cameraMinBounds(0)),
                      std::abs(cameraMaxBounds(1) - cameraMinBounds(1)));
    // std::cout << "bound size = " << boundSize.transpose() << '\n';
    clusterSize(0) = std::max(clusterSize(0), boundSize(0));
    clusterSize(1) = std::max(clusterSize(1), boundSize(0));
  }
  // std::cout << "cluster size = " << clusterSize.transpose() << '\n';
  // std::cout << '\n';

  // next, because we now know how big the clusters are, lay the clusters out.
  // for now, just do a vertical linear layout.  also compute the camera bounds
  m_cameraMinBounds(0) = std::numeric_limits<double>::max();
  m_cameraMinBounds(1) = std::numeric_limits<double>::max();
  m_cameraMaxBounds(0) = std::numeric_limits<double>::min();
  m_cameraMaxBounds(1) = std::numeric_limits<double>::min();
  int idx = 0;
  Vector3 clusterCenter(0.0, -clusterSize(1)/2.0, 0.0);
  for (auto r_it = m_clusterTileRanges.begin(), r_it_end = m_clusterTileRanges.end();
       r_it != r_it_end;
       ++r_it, clusterCenter(1) -= clusterSize(1)) {
    Range<TilePointerVector::iterator> const &r = *r_it;
    for (auto t = r; t.is_not_at_end(); ++t, ++idx) {
      Tile &tile = **t;
      Vector3 position(tile.m_phantomPosition+clusterCenter);
      animateTilePosition(tile, idx, position);
      m_cameraMinBounds(0) = std::min(m_cameraMinBounds(0), position(0));
      m_cameraMinBounds(1) = std::min(m_cameraMinBounds(1), position(1));
      m_cameraMaxBounds(0) = std::max(m_cameraMaxBounds(0), position(0));
      m_cameraMaxBounds(1) = std::max(m_cameraMaxBounds(1), position(1));
    }
  }
}

Vector2 BlobClusterLayout::GetCameraMinBounds() const {
  return m_cameraMinBounds;
}

Vector2 BlobClusterLayout::GetCameraMaxBounds() const {
  return m_cameraMaxBounds;
}



