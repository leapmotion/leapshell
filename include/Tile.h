#ifndef __Tile_h__
#define __Tile_h__

#include <memory>
#include "HierarchyNode.h"
#include "Utilities.h"

class Tile {
public:
  static const float POSITION_SMOOTH;
  Tile();
  mutable ci::gl::TextureRef m_icon;
  ExponentialFilter<Vector3> m_positionSmoother;
  Vector3 m_position;
  Vector3 m_size;
  std::shared_ptr<HierarchyNode> m_node;

  // TODO: should this be eigen-aligned?
};

typedef std::vector<Tile, Eigen::aligned_allocator<Tile>> TileVector;
typedef std::vector<std::shared_ptr<Tile>> TilePointerVector;

void SortTiles (TilePointerVector &v, std::vector<std::string> const &prioritizedKeys);

#endif
