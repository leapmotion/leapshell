#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include "HierarchyNode.h"
#include "Tile.h"

#include <memory>
#include <vector>

class Layout {
public:
  virtual void UpdateTiles(const std::vector<std::shared_ptr<HierarchyNode>> nodes, TileVector& tiles) = 0;
};

class GridLayout : public Layout {
public:
  GridLayout();
  virtual void UpdateTiles(const std::vector<std::shared_ptr<HierarchyNode>> nodes, TileVector& tiles) override;
  void SetWidth(double width) { m_width = width; }
private:
  double m_width;
};

#endif
