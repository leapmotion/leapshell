#ifndef __Tile_h__
#define __Tile_h__

#include "DataTypes.h"
#include <memory>
#include "HierarchyNode.h"

class Tile {
public:
  Tile();
  ci::gl::TextureRef m_icon;
  Vector3 m_position;
  Vector3 m_size;
  std::shared_ptr<HierarchyNode> m_node;
};

typedef std::vector<Tile, Eigen::aligned_allocator<Tile>> TileVector;

#endif
