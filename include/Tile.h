#ifndef __Tile_h__
#define __Tile_h__

#include "HierarchyNode.h"
#include "Utilities.h"

class Tile {
public:
  static const float POSITION_SMOOTH;
  static const float SIZE_SMOOTH;
  static const float ACTIVATION_SMOOTH;
  Tile();
  mutable ci::gl::TextureRef m_icon;
  ExponentialFilter<Vector3> m_positionSmoother;
  ExponentialFilter<Vector3> m_sizeSmoother;
  ExponentialFilter<float> m_highlightSmoother;
  ExponentialFilter<float> m_activationSmoother;
  Vector3 m_position;
  Vector3 m_size;
  std::shared_ptr<HierarchyNode> m_node;

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

typedef std::vector<Tile, Eigen::aligned_allocator<Tile>> TileVector;
typedef std::vector<Tile *> TilePointerVector;

void SortTiles (TilePointerVector &v, std::vector<std::string> const &prioritizedKeys);

#endif
