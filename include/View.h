#ifndef __VIEW_H__
#define __VIEW_H__

#include "Layout.h"
#include "Tile.h"
#include "Utilities.h"

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
    static const float FORCE_DISTANCE = 50.0f;
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

  // this is a "heavy" update that happens only when the NavigationState changes
  void UpdateFromChangedNavigationState(bool fadeIn = true);
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
  const ForceVector& Forces() const { return m_forces; }
  ForceVector& Forces() { return m_forces; }
  float TransitionOpacity() const { return m_transitionOpacity; }
  const std::string& SearchFilter() const { return m_searchFilter; }
  const std::shared_ptr<SizeLayout>& GetSizeLayout() const { return m_sizeLayout; }
  const std::shared_ptr<PositionLayout>& GetPositionLayout() const { return m_positionLayout; }
  Vector2 ViewSizeAtPlane() const;
  double LastUpdateTime() const { return m_lastUpdateTime; }
  float Activation() const { return m_activationSmoother.value; }
  float InactiveOpacity() const { return m_inactiveOpacity; }

  // setters
  void SetSizeLayout(const std::shared_ptr<SizeLayout>& sizeLayout);
  void SetPositionLayout(const std::shared_ptr<PositionLayout>& positionLayout);
  void ApplyVelocity(const Vector3& velocity);
  void SetPosition(const Vector3& position);
  void SetLookAt(const Vector3& lookat);
  void SetSearchFilter(const std::string& searchFilter);
  void UpdateActivation(float activation, float smooth);
  void SetWorldView(const std::shared_ptr<View>& worldViewPtr) { m_worldView = worldViewPtr; }
  void SetIsNavView(bool isNav) { m_isNavView = isNav; }
  void SetInactiveOpacity(float opacity) { m_inactiveOpacity = opacity; }

  // TEMPORARY HACK for 2014.04.21 demo
  void SetPrioritizedKeys (const std::vector<std::string> &prioritizedKeys) {
    m_sortingCriteria.SetPrioritizedKeys(prioritizedKeys);
    SortTiles(m_sortedTiles, m_sortingCriteria.PrioritizedKeys());
  }
  // TEMPORARY HACK for 2014.04.21 demo
  void PrioritizeKey (const std::string &key) {
    m_sortingCriteria.PrioritizeKey(key);
    SortTiles(m_sortedTiles, m_sortingCriteria.PrioritizedKeys());
  }

  static const double CAM_DISTANCE_FROM_PLANE;
  static const double TILE_PULL_THRESHOLD;
  static const double PUSH_THRESHOLD;
  static const double MIN_TIME_BETWEEN_SWITCH;

private:

  void resetView();
  Vector3 clampCameraPosition(const Vector3& position) const;
  float calcPullOpacity(double maxTileZ, float maxActivation) const;
  float calcPushOpacity() const;
  static void ExtractPrioritizedKeysFrom(const HierarchyNode &node, SortingCriteria &sortingCriteria);
  static void RegenerateTilesAndTilePointers(const HierarchyNodeVector &nodes, TileVector &tiles, TilePointerVector &tilePointers, bool fadeIn);

  std::shared_ptr<NavigationState> m_ownerNavigationState;

  SortingCriteria m_sortingCriteria;
  std::shared_ptr<SizeLayout> m_sizeLayout;
  std::shared_ptr<PositionLayout> m_positionLayout;
  TileVector m_tiles;
  TilePointerVector m_sortedTiles;
  ForceVector m_forces;
  std::string m_searchFilter;
  int m_prevSearchVisibleTiles;
  double m_lastUpdateTime;

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
  float m_transitionOpacity;

  std::shared_ptr<View> m_worldView;
  bool m_isNavView;
  float m_inactiveOpacity;

  ExponentialFilter<float> m_activationSmoother;

};

#endif
