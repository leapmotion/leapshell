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

  // getters
  Vector3 OrigPosition() const;
  Vector3 Position() const;
  Vector3 Size() const;
  float Highlight() const;
  float Activation() const;
  double LastActivationUpdateTime() const;
  static float SwitchWarmupFactor();
  static float TransitionWarmupFactor();

  Vector3 m_phantomPosition;

  ci::gl::TextureRef& Icon() const { return m_icon; }
  std::shared_ptr<HierarchyNode>& Node() { return m_node; }
  const std::shared_ptr<HierarchyNode>& Node() const { return m_node; }

  // setters
  void UpdateSize(const Vector3& newSize, float smooth);
  void UpdatePosition(const Vector3& newPosition, float smooth);
  void UpdateHighlight(float newHighlight, float smooth);
  void UpdateActivation(float newActivation, float smooth);
  void UpdateGrabDelta(const Vector3& newGrabDelta, float smooth);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

private:
  ExponentialFilter<Vector3> m_positionSmoother;
  ExponentialFilter<Vector3> m_sizeSmoother;
  ExponentialFilter<float> m_highlightSmoother;
  ExponentialFilter<float> m_activationSmoother;
  ExponentialFilter<Vector3> m_grabDeltaSmoother;

  mutable ci::gl::TextureRef m_icon;
  std::shared_ptr<HierarchyNode> m_node;
};

typedef std::vector<Tile, Eigen::aligned_allocator<Tile>> TileVector;
typedef std::vector<Tile *> TilePointerVector;

void SortTiles (TilePointerVector &v, std::vector<std::string> const &prioritizedKeys);

#endif
