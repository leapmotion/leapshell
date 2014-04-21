#ifndef __VIEW_H__
#define __VIEW_H__

#include "Layout.h"
#include "Tile.h"
#include "Utilities.h"
#include "MeshHand.h"

class HierarchyNode;
class PositionLayout;
class NavigationState;

class SortingCriteria {
public:

  SortingCriteria (std::vector<std::string> const &prioritizedKeys = std::vector<std::string>())
    :
    m_prioritizedKeys(prioritizedKeys)
  { }

  std::vector<std::string> const &PrioritizedKeys () const { return m_prioritizedKeys; }
  void SetPrioritizedKeys (std::vector<std::string> const &prioritizedKeys) { m_prioritizedKeys = prioritizedKeys; }

  // TEMP for 2014.04.21 demo
  void PrioritizeKey (const std::string &key);

private:

  std::vector<std::string> m_prioritizedKeys;
};

struct Force {
  Force(const Vector3& position, float strength) : m_position(position), m_strength(strength) { }
  Vector3 ForceAt(const Vector3& position) const {
    static const float FORCE_DISTANCE = 75.0f;
    const Vector3 diff = position - m_position;
    const double normSq = diff.squaredNorm();
    if (normSq > 0.001) {
      return FORCE_DISTANCE * m_strength * diff / normSq;
    } else {
      return Vector3::Zero();
    }
  }
  Vector3 m_position;
  float m_strength;
};

typedef std::vector<Force, Eigen::aligned_allocator<Force> > ForceVector;

class View {
public:

  View (std::shared_ptr<NavigationState> const &ownerNavigationState);
  ~View ();

  // this is a "heavy" update that happens only when the NavigationState changes
  void UpdateFromChangedNavigationState();
  // this is a "light" update that happens every frame
  void PerFrameUpdate();

  // getters
  const TileVector& Tiles() const { return m_tiles; }
  TileVector& Tiles() { return m_tiles; }
  const Vector3 Position() const { return m_position + m_additionalZ.value*Vector3::UnitZ(); }
  const Vector3& LookAt() const { return m_lookatSmoother.value; }
  const Vector3& Up() const { return m_up; }
  float FOV() const { return m_fov; }
  float Near() const { return m_near; }
  float Far() const { return m_far; }
  MeshHand& LeftHand() const { return *m_handL; }
  MeshHand& RightHand() const { return *m_handR; }
  const ForceVector& Forces() const { return m_forces; }
  ForceVector& Forces() { return m_forces; }

  // setters
  void SetSizeLayout(const std::shared_ptr<SizeLayout>& sizeLayout);
  void SetPositionLayout(const std::shared_ptr<PositionLayout>& positionLayout);
  void ApplyVelocity(const Vector3& velocity, double timeSeconds, double deltaTime);
  void SetPosition(const Vector3& position);
  void SetLookAt(const Vector3& lookat);

  // TEMPORARY HACK for 2014.04.21 demo
  void PrioritizeKey (const std::string &key) {
    m_sortingCriteria.PrioritizeKey(key);
    SortTiles(m_sortedTiles, m_sortingCriteria.PrioritizedKeys());
  }

  static const double CAM_DISTANCE_FROM_PLANE;

private:

  void resetView();
  Vector3 clampCameraPosition(const Vector3& position) const;
  static void ExtractPrioritizedKeysFrom(const HierarchyNode &node, SortingCriteria &sortingCriteria);
  static void RegenerateTilesAndTilePointers(const HierarchyNodeVector &nodes, TileVector &tiles, TilePointerVector &tilePointers);

  std::shared_ptr<NavigationState> m_ownerNavigationState;

  SortingCriteria m_sortingCriteria;
  std::shared_ptr<SizeLayout> m_sizeLayout;
  std::shared_ptr<PositionLayout> m_positionLayout;
  TileVector m_tiles;
  TilePointerVector m_sortedTiles;
  ForceVector m_forces;

  // render parameters
  ExponentialFilter<Vector3> m_lookatSmoother;
  Vector3 m_position;
  Vector3 m_lookat;
  Vector3 m_up;
  float m_fov;
  float m_near;
  float m_far;
  ExponentialFilter<double> m_additionalZ;
  double m_lastSwitchTime;

  // hands
  mutable MeshHand* m_handL;
  mutable MeshHand* m_handR;

};

#endif
