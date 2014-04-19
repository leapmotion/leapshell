#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include "HierarchyNode.h"
#include "Tile.h"

#include <memory>
#include <vector>

class Layout {
public:
  Layout();
  virtual void UpdateTiles(const HierarchyNodeVector& nodes, TileVector& tiles) = 0;
  virtual Vector2 GetCameraMinBounds() const = 0;
  virtual Vector2 GetCameraMaxBounds() const = 0;
protected:
  void animateTilePosition(Tile& tile, int idx, const Vector3& newPosition) const;
  double m_creationTime;
};

class GridLayout : public Layout {
public:
  GridLayout();
  virtual void UpdateTiles(const HierarchyNodeVector& nodes, TileVector& tiles) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetWidth(double width) { m_width = width; }
private:
  double m_width;
  double m_height;
};

class RingLayout : public Layout {
public:
  RingLayout();
  virtual void UpdateTiles(const HierarchyNodeVector& nodes, TileVector& tiles) override;
  virtual Vector2 GetCameraMinBounds() const override;
  virtual Vector2 GetCameraMaxBounds() const override;
  void SetRadius(double radius) { m_radius = radius; }
private:
  double m_radius;
};

#endif
