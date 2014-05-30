#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include "HierarchyNode.h"
#include "Range.h"
#include "Tile.h"
#include "Utilities.h"

/// @brief Interface class for setting the layout-specific sizes of tiles.
/// @details The sizes of a View's tiles are determined before their positions,
/// because their sizes may affect how the positions are computed.
/// @see UniformSizeLayout
class SizeLayout {
public:
  SizeLayout();
  virtual void UpdateTileSizes(const Range<TilePointerVector::iterator> &tiles) = 0;
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
protected:
  void animateTileSize(Tile& tile, int idx, const Vector3& newSize) const;
  double m_creationTime;
};

/// @brief Sets all tile sizes to the same, pre-specified size.  The default is Vector3::Constant(15).
class UniformSizeLayout : public SizeLayout {
public:
  UniformSizeLayout();
  virtual void UpdateTileSizes(const Range<TilePointerVector::iterator> &tiles) override;
  void SetSize(const Vector3 &size) { m_size = size; }
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
protected:
  Vector3 m_size;
};

/// @brief Interface class for setting the layout-specific positions of tiles.
/// @details The sizes of a View's tiles are determined before their positions,
/// because their sizes may affect how the positions are computed.
class PositionLayout {
public:
  PositionLayout();
  // TEMP HACK: updatePhantomPosition is an artifact of the first-pass, hacky version of clustering, and
  // will go away once "real" clustering is implemented via HierarchyNode.
  virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles, bool updatePhantomPosition = false) = 0;
  virtual Vector2 GetCameraMinBounds() const = 0;
  virtual Vector2 GetCameraMaxBounds() const = 0;
  void SetNumVisibleTiles(int numVisibleTiles) { m_numVisibleTiles = numVisibleTiles; }
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
protected:
  void animateTilePosition(Tile& tile, int idx, const Vector3& newPosition) const;
  double m_creationTime;
  int m_numVisibleTiles;
};

/// @brief Lays tiles out in a fixed-width grid, row-major, in the given order.
class GridLayout : public PositionLayout {
public:
  GridLayout();
  virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles, bool updatePhantomPosition = false) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetWidth(double width) { m_width = width; }
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  double m_width;
  double m_height;
};

/// @brief Lays tiles out in a (possibly densely-packed) ring, in the given order.
class RingLayout : public PositionLayout {
public:
  RingLayout();
  virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles, bool updatePhantomPosition = false) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetRadius(double radius) { m_radius = radius; }
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  double m_radius;
};

/// @brief Lays tiles out in a tightly-packed, but non-overlapping, linear spiral, in the given order.
class LinearSpiralLayout : public PositionLayout {
public:
  LinearSpiralLayout();
  virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles, bool updatePhantomPosition = false) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetStartingAngle(double startingAngle) { m_startingAngle = startingAngle; }
  void SetSlope(double slope) { m_slope = slope; }
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  double m_startingAngle;
  double m_slope;
  double m_boundingRadius;
};

/// @brief Lays tiles out in a tightly-packed exponential spiral.
/// @details This implements both SizeLayout and PositionLayout, because the tiles that
/// are closer to the center have to be proportionally smaller.
class ExponentialSpiralLayout : public SizeLayout, public PositionLayout {
public:
  ExponentialSpiralLayout();
  virtual void UpdateTileSizes(const Range<TilePointerVector::iterator> &tiles) override;
  virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles, bool updatePhantomPosition = false) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetBaseTileSize(const Vector3 &baseTileSize) { m_baseTileSize = baseTileSize; }
  void SetSpacing(double spacing);
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  void SetBaseRadius(double baseRadius);

  Vector3 m_baseTileSize;
  double m_spacing;
  double m_baseRadius;
  double m_boundingRadius;
  double m_exponentialRate;
  double m_thetaIncrement;
};

/// @brief Lays tiles out in clusters based on the first key in the prioritized sorting criteria.
/// @details The clusters are layed out in a vertical line.
/// @note This is a first-pass, hacky, and somewhat crappy version.  Real clustering will be
/// implemented by another implementation of HierarchyNode, so that recursive clusters can be
/// computed, and layouts can be used to position each cluster.
class BlobClusterLayout : public PositionLayout {
public:
  BlobClusterLayout();
  virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles, bool updatePhantomPosition = false) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetClusteringKey(const std::string &clusteringKey) { m_clusteringKey = clusteringKey; }
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  // for now make these private

  // the cluster outer layout decides how each cluster (as a whole) should be positioned.
  // the cluster inner layout decides how the elements of each cluster should be layed out.
  
  // TODO: cache clustering iterator ranges
  std::string m_clusteringKey;
  // std::shared_ptr<PositionLayout> m_clusterOuterLayout;
  std::shared_ptr<SizeLayout> m_clusterInnerSizeLayout;
  std::shared_ptr<PositionLayout> m_clusterInnerPositionLayout;

  std::vector<Range<TilePointerVector::iterator>> m_clusterTileRanges;
  double m_boundingRadius;
  Vector2 m_cameraMinBounds;
  Vector2 m_cameraMaxBounds;
};

#endif
