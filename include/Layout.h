#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include "DataTypes.h"
#include "HierarchyNode.h"
#include "Tile.h"
#include "DataTypes.h"

#include <memory>
#include <vector>

class Layout {
public:
  virtual void UpdateTiles(const std::vector<std::shared_ptr<HierarchyNode>> nodes, TileVector& tiles) = 0;
  virtual Vector2 GetCameraBoundsX() const = 0;
  virtual Vector2 GetCameraBoundsY() const = 0;
};

class GridLayout : public Layout {
public:
  GridLayout();
  virtual void UpdateTiles(const std::vector<std::shared_ptr<HierarchyNode>> nodes, TileVector& tiles) override;
  virtual Vector2 GetCameraBoundsX() const override;
  virtual Vector2 GetCameraBoundsY() const override;
  void SetWidth(double width) { m_width = width; }
private:
  double m_width;
};

#endif
