#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include "HierarchyNode.h"
#include "Tile.h"
#include "Utilities.h"

#include <memory>
#include <vector>


class SizeLayout {
public:
  SizeLayout();
  virtual void UpdateTileSizes(TilePointerVector &tiles) = 0;
protected:
  void animateTileSize(Tile& tile, int idx, const Vector3& newSize) const;
  double m_creationTime;
};

class UniformSizeLayout : public SizeLayout {
public:
  UniformSizeLayout();
  virtual void UpdateTileSizes(TilePointerVector &tiles) override;
  void SetSize(const Vector3 &size) { m_size = size; }
protected:
  Vector3 m_size;
};

// the tiles' sizes will have been updated before their positions are updated.
class PositionLayout {
public:
  PositionLayout();
  virtual void UpdateTilePositions(TilePointerVector &tiles) = 0;
  virtual Vector2 GetCameraMinBounds() const = 0;
  virtual Vector2 GetCameraMaxBounds() const = 0;
protected:
  void animateTilePosition(Tile& tile, int idx, const Vector3& newPosition) const;
  double m_creationTime;
};

class GridLayout : public PositionLayout {
public:
  GridLayout();
  virtual void UpdateTilePositions(TilePointerVector &tiles) override;
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
  virtual void UpdateTilePositions(TilePointerVector &tiles) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetRadius(double radius) { m_radius = radius; }
private:
  double m_radius;
};

class LinearSpiralLayout : public PositionLayout {
public:
  LinearSpiralLayout();
  virtual void UpdateTilePositions(TilePointerVector &tiles) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetStartingAngle(double startingAngle) { m_startingAngle = startingAngle; }
  void SetSlope(double slope) { m_slope = slope; }
private:
  double m_startingAngle;
  double m_slope;
  double m_radius;
};

#endif
