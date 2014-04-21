#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include "HierarchyNode.h"
#include "Range.h"
#include "Tile.h"
#include "Utilities.h"

class SizeLayout {
public:
  SizeLayout();
  virtual void UpdateTileSizes(const Range<TilePointerVector::iterator> &tiles) = 0;
protected:
  void animateTileSize(Tile& tile, int idx, const Vector3& newSize) const;
  double m_creationTime;
};

class UniformSizeLayout : public SizeLayout {
public:
  UniformSizeLayout();
  virtual void UpdateTileSizes(const Range<TilePointerVector::iterator> &tiles) override;
  void SetSize(const Vector3 &size) { m_size = size; }
protected:
  Vector3 m_size;
};

// the tiles' sizes will have been updated before their positions are updated.
class PositionLayout {
public:
  PositionLayout();
  virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles) = 0;
  virtual Vector2 GetCameraMinBounds() const = 0;
  virtual Vector2 GetCameraMaxBounds() const = 0;
protected:
  void animateTilePosition(Tile& tile, int idx, const Vector3& newPosition) const;
  double m_creationTime;
};

class GridLayout : public PositionLayout {
public:
  GridLayout();
  virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetWidth(double width) { m_width = width; }
private:
  double m_width;
  double m_height;
};

class RingLayout : public PositionLayout {
public:
  RingLayout();
  virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetRadius(double radius) { m_radius = radius; }
private:
  double m_radius;
};

class LinearSpiralLayout : public PositionLayout {
public:
  LinearSpiralLayout();
  virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetStartingAngle(double startingAngle) { m_startingAngle = startingAngle; }
  void SetSlope(double slope) { m_slope = slope; }
private:
  double m_startingAngle;
  double m_slope;
  double m_boundingRadius;
};

class ExponentialSpiralLayout : public SizeLayout, public PositionLayout {
public:
  ExponentialSpiralLayout();
  virtual void UpdateTileSizes(const Range<TilePointerVector::iterator> &tiles) override;
  virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetBaseTileSize(const Vector3 &baseTileSize) { m_baseTileSize = baseTileSize; }
  void SetSpacing(double spacing);
private:
  void SetBaseRadius(double baseRadius);

  Vector3 m_baseTileSize;
  double m_spacing;
  double m_baseRadius;
  double m_boundingRadius;
  double m_exponentialRate;
  double m_thetaIncrement;
};

// class BlobClusterLayout : public PositionLayout {
// public:
//   BlobClusterLayout();
  // virtual void UpdateTilePositions(const Range<TilePointerVector::iterator> &tiles) override;
//   virtual Vector2 GetCameraMinBounds() const override;
//   virtual Vector2 GetCameraMaxBounds() const override;
//   void SetClusteringKey(const std::string &clusteringKey) { m_clusteringKey = clusteringKey; }
// private:
//   // for now make these private

//   // the cluster outer layout decides how each cluster (as a whole) should be positioned.
//   void SetClusterOuterLayout(const std::shared_ptr<Layout> &clusterOuterLayout) { m_clusterOuterLayout = clusterOuterLayout; }
//   // the cluster inner layout decides how the elements of each cluster should be layed out.
//   void SetClusterInnerLayout(const std::shared_ptr<Layout> &clusterInnerLayout) { m_clusterInnerLayout = clusterInnerLayout; }
  
//   // TODO: cache clustering iterator ranges
//   std::string m_clusteringKey;
//   std::shared_ptr<Layout> m_clusterOuterLayout;
//   std::shared_ptr<Layout> m_clusterInnerLayout;

//   std::vector<Range<
// };

#endif
