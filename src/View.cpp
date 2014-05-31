#include "StdAfx.h"
#include "NavigationState.h"
#include "Value.h"
#include "View.h"
#include "Globals.h"

const double View::CAM_DISTANCE_FROM_PLANE = 70.0; // mm
const double View::TILE_PULL_THRESHOLD = 0.5 * CAM_DISTANCE_FROM_PLANE; // mm
const double View::PUSH_THRESHOLD = 1.1 * CAM_DISTANCE_FROM_PLANE; // mm
const double View::MIN_TIME_BETWEEN_SWITCH = 1.0; // in seconds, how much time must elapse between changes in navigation

// TEMP for the 2014.04.21 demo
void SortingCriteria::PrioritizeKey (const std::string &key) {
  // std::cout << "SortingCriteria::PrioritizeKey(\"" << key << "\");\nbefore: m_prioritizedKeys = { ";
  // for (auto it = m_prioritizedKeys.begin(); it != m_prioritizedKeys.end(); ++it) {
  //   std::cout << *it << ", ";
  // }
  // std::cout << "}\n";

  std::vector<std::string>::iterator match_it = std::find(m_prioritizedKeys.begin(), m_prioritizedKeys.end(), key);
  assert(match_it != m_prioritizedKeys.end() && "this should never happen");
  // if we found the key, shift it to the front
  if (match_it != m_prioritizedKeys.end()) {
    for (auto it = match_it; it != m_prioritizedKeys.begin(); --it) {
      auto prev_it = it-1;
      *it = *prev_it;
    }
    *m_prioritizedKeys.begin() = key;
  }

  Globals::lastTileSwitchTime = Globals::curTimeSeconds;

  // std::cout << "after: m_prioritizedKeys = { ";
  // for (auto it = m_prioritizedKeys.begin(); it != m_prioritizedKeys.end(); ++it) {
  //   std::cout << *it << ", ";
  // }
  // std::cout << "}\n\n";
}

View::View(std::shared_ptr<NavigationState> const &ownerNavigationState)
  :
  m_ownerNavigationState(ownerNavigationState)
{
  resetView();
  m_up = Vector3::UnitY();
  m_fov = 60.0f;
  m_near = 1.0f;
  m_far = 500.0f;
  m_transitionOpacity = 1.0f;
  m_sizeLayout = std::shared_ptr<SizeLayout>(new UniformSizeLayout());
  m_positionLayout = std::shared_ptr<PositionLayout>(new GridLayout());
  m_lookatSmoother.Update(m_lookat, 0.0, 0.5f);
  m_lastSwitchTime = 0.0;
  m_lastUpdateTime = 0.0;
}

void View::UpdateFromChangedNavigationState(bool fadeIn) {
  m_sortedTiles.clear();
  if (m_ownerNavigationState) {
    if (m_sortingCriteria.PrioritizedKeys().empty()) {
      std::shared_ptr<HierarchyNode> node = m_ownerNavigationState->currentLocation();
      // if (node) {
      //   ExtractPrioritizedKeysFrom(*node, m_sortingCriteria);
      // }
    }
    RegenerateTilesAndTilePointers(m_ownerNavigationState->currentChildNodes(), m_tiles, m_sortedTiles, false);
    // NOTE: now the elements of m_sortedTiles point to elements of m_tiles, so m_sortedTiles
    // should be cleared and regenerated if m_tiles is changed.
    SetSearchFilter(m_searchFilter);
  }
  SortTiles(m_sortedTiles, m_sortingCriteria.PrioritizedKeys());

  PerFrameUpdate();
}

void View::PerFrameUpdate () {
  // update the sizes first (a PositionLayout implementation often depends on the sizes)
  m_sizeLayout->UpdateTileSizes(range(m_sortedTiles.begin(), m_sortedTiles.end()));
  // then update the positions
  m_positionLayout->SetNumVisibleTiles(m_prevSearchVisibleTiles);
  m_positionLayout->UpdateTilePositions(range(m_sortedTiles.begin(), m_sortedTiles.end()));

  std::shared_ptr<HierarchyNode> selectedNode(nullptr);
  Tile* selectedTile = nullptr;
  double maxTileZ = 0;
  float maxActivation = 0;
  for (TileVector::iterator it = m_tiles.begin(); it != m_tiles.end(); ++it) {
    Tile& tile = *it;
    maxTileZ = std::max(maxTileZ, tile.Position().z());
    maxActivation = std::max(maxActivation, tile.Activation());
    if (tile.Activation() > 0.5f && tile.Position().z() > TILE_PULL_THRESHOLD) {
      selectedNode = tile.Node();
      selectedTile = &tile;
      break;
    }
  }

  const bool haveParent = !!(m_ownerNavigationState->currentLocation()->parent());

  // calculate a fade when about to change navigation state
  float pullOpacity = 1.0f;
  float pushOpacity = 1.0f;
  if ((Globals::curTimeSeconds - m_lastSwitchTime) > MIN_TIME_BETWEEN_SWITCH) {
    pullOpacity = calcPullOpacity(maxTileZ, maxActivation);
    pushOpacity = haveParent ? calcPushOpacity() : 1.0f;
  }
  m_transitionOpacity = std::min(pullOpacity, pushOpacity);

  if (selectedNode) {
    if (selectedNode->open()) {
      selectedTile->ResetActivation();
      Globals::haveSeenOpenHand = false;
    } else if (!selectedNode->is_leaf() && (Globals::curTimeSeconds - m_lastSwitchTime) > MIN_TIME_BETWEEN_SWITCH) {
      resetView();
      Globals::lastTileSwitchTime = Globals::curTimeSeconds;
      Globals::lastTileTransitionTime = Globals::curTimeSeconds;
      m_tiles.clear();
      m_additionalZ.Update(CAM_DISTANCE_FROM_PLANE, Globals::curTimeSeconds, 0.1f);
      m_lastSwitchTime = Globals::curTimeSeconds;
      Globals::haveSeenOpenHand = false;
      m_ownerNavigationState->navigateDown(selectedNode);
    }
  } else if (haveParent && (m_position.z() > PUSH_THRESHOLD) && (Globals::curTimeSeconds - m_lastSwitchTime) > MIN_TIME_BETWEEN_SWITCH) {
    Globals::lastTileSwitchTime = Globals::curTimeSeconds;
    Globals::lastTileTransitionTime = Globals::curTimeSeconds;
    m_additionalZ.Update(-CAM_DISTANCE_FROM_PLANE, Globals::curTimeSeconds, 0.1f);
    m_lastSwitchTime = Globals::curTimeSeconds;
    m_ownerNavigationState->navigateUp();
    resetView();
  } else {
    m_additionalZ.Update(0.0, Globals::curTimeSeconds, 0.965f);
  }
}

Vector2 View::ViewSizeAtPlane() const {
  /*
  Note: Cinder has a bug in calculating Projection frustum (CameraPersp::calcProjection)
  "horizFovDegrees" is actually vertical FOV, so the below math will need to change when using horizontal FOV
  */
  const float aspect = static_cast<float>(Globals::windowWidth / Globals::windowHeight);
  const float hFov = aspect * m_fov;
  const double width = 2.0 * m_position.z() * std::tan(DEGREES_TO_RADIANS * 0.5 * hFov);
  const double height = 2.0 * m_position.z() * std::tan(DEGREES_TO_RADIANS * 0.5 * m_fov);
  return Vector2(width, height);
}

void View::SetSizeLayout(const std::shared_ptr<SizeLayout>& sizeLayout) {
  m_sizeLayout = sizeLayout;
}

void View::SetPositionLayout(const std::shared_ptr<PositionLayout>& positionLayout) {
  m_positionLayout = positionLayout;
}

void View::ApplyVelocity(const Vector3& velocity) {
  const double deltaTime = Globals::curTimeSeconds - m_lastUpdateTime;
  m_lastUpdateTime = Globals::curTimeSeconds;

  const Vector3 deltaPosition = velocity * deltaTime;

  m_position += deltaPosition;
  m_lookat += deltaPosition;

  static const double RUBBER_BAND_SPEED = 0.3333;
  Vector3 clampedPosition = clampCameraPosition(m_position);
  clampedPosition.z() = CAM_DISTANCE_FROM_PLANE;
  const Vector3 positionRubberBandForce = RUBBER_BAND_SPEED * (clampedPosition - m_position);
  Vector3 clampedLookat = clampCameraPosition(m_lookat);
  clampedLookat.z() = 0.0;
  const Vector3 lookatRubberBandForce = RUBBER_BAND_SPEED * (clampedLookat - m_lookat);

  m_position += positionRubberBandForce;
  m_lookat += lookatRubberBandForce;

  m_lookatSmoother.Update(m_lookat, Globals::curTimeSeconds, 0.5f);
}

void View::SetPosition(const Vector3& position) {
  m_position = clampCameraPosition(position);
}

void View::SetLookAt(const Vector3& lookat) {
  m_lookat = clampCameraPosition(lookat);
}

void View::SetSearchFilter(const std::string& searchFilter) {
  m_searchFilter = searchFilter;
  int numVisibleTiles = 0;
  for (TileVector::iterator it = m_tiles.begin(); it != m_tiles.end(); ++it) {
    Tile& tile = *it;
    if (tile.Node()) {
      const std::string name = tile.Node()->get_metadata_as<std::string>("name");
      const bool visible = StringContains(name, searchFilter, true);
      tile.SetVisible(visible);
      if (visible) {
        numVisibleTiles++;
      }
    }
  }
  if (m_prevSearchVisibleTiles != numVisibleTiles) {
    Globals::lastTileSwitchTime = Globals::curTimeSeconds;
    m_prevSearchVisibleTiles = numVisibleTiles;
  }
}

void View::resetView() {
  m_position = CAM_DISTANCE_FROM_PLANE * Vector3::UnitZ();
  m_lookat = Vector3::Zero();
}

Vector3 View::clampCameraPosition(const Vector3& position) const {
  Vector3 result(position);
  static const float EXTRA_HEIGHT_SCALE = 0.95f; // scale factor to leave some room at the edge
  const float extraHeight = EXTRA_HEIGHT_SCALE * static_cast<float>(ViewSizeAtPlane().y()/2.0);
  Vector2 min = m_positionLayout->GetCameraMinBounds();
  Vector2 max = m_positionLayout->GetCameraMaxBounds();
  min.y() += extraHeight;
  max.y() -= extraHeight;

  // make sure min and max are valid after adding height from camera bounds
  for (int i=0; i<2; i++) {
    if (min[i] > max[i]) {
      double middle = 0.5 * (min[i] + max[i]);
      min[i] = middle;
      max[i] = middle;
    }
  }

  result.head<2>() = result.head<2>().cwiseMax(min).cwiseMin(max);
  return result;
}

float View::calcPullOpacity(double maxTileZ, float maxActivation) const {
  static const float FADE_DISTANCE = 0.25f * static_cast<float>(TILE_PULL_THRESHOLD);
  const float distFromPull = static_cast<float>(TILE_PULL_THRESHOLD - maxTileZ);
  float pullOpacity = 0.0f;
  if (maxTileZ < TILE_PULL_THRESHOLD) {
    pullOpacity = Clamp(distFromPull/FADE_DISTANCE);
  }
  const float activationOpacity = 1.0f - maxActivation;
  return SmootherStep(std::max(activationOpacity, pullOpacity));
}

float View::calcPushOpacity() const {
  if (m_position.z() >= PUSH_THRESHOLD) {
    return 0.0f;
  }
  static const float FADE_DISTANCE = 0.25f * static_cast<float>(PUSH_THRESHOLD - CAM_DISTANCE_FROM_PLANE);
  const float distFromPush = static_cast<float>(PUSH_THRESHOLD - m_position.z());
  return SmootherStep(Clamp(distFromPush/FADE_DISTANCE));
}

void View::ExtractPrioritizedKeysFrom (const HierarchyNode &node, SortingCriteria &sortingCriteria) {
  // for now, use the keys of the given HierarchyNode as the SearchCriteria
  // prioritized keys.  Note that SearchCriteria is constructed with an empty
  // prioritizedKeys vector, which indicates that no sorting should be done.
  Value const &metadata = node.metadata();
  assert(metadata.IsHash() && "HierarchyNode metadata should always be a Hash type Value");
  std::map<std::string,Value> const &hash = metadata.Cast<Value::Hash>();
  std::vector<std::string> prioritizedKeys;
  std::cout << "prioritized keys:\n";
  for (auto it = hash.begin(), it_end = hash.end(); it != it_end; ++it) {
    prioritizedKeys.push_back(it->first);
    std::cout << "    key = \"" << it->first << "\"\n";
  }
  std::cout << '\n';
  sortingCriteria.SetPrioritizedKeys(prioritizedKeys);
}

void View::RegenerateTilesAndTilePointers(const HierarchyNodeVector &nodes, TileVector &tiles, TilePointerVector &tilePointers, bool fadeIn) {
  // clear the vector of Tile pointers BEFORE tiles is changed, because they point to elements of tiles.
  tilePointers.clear();
  // create a Tile for each node
  tiles.clear();
  tiles.resize(nodes.size());
  if (fadeIn) {
    Globals::lastTileSwitchTime = Globals::curTimeSeconds;
    Globals::lastTileTransitionTime = Globals::curTimeSeconds;
  }
  // iterate through the tile and node vectors in parallel
  auto tile_it = tiles.begin();
  auto tile_it_end = tiles.end();
  auto node_it = nodes.begin();
  // assign the node to the Tile's m_node member, and store a pointer to the Tile
  for ( ; tile_it != tile_it_end; ++tile_it, ++node_it) {
    assert(node_it != nodes.end() && "tiles and nodes should have the same length, so this should be impossible");
    Tile &tile = *tile_it;
    tile.Node() = *node_it;
    tilePointers.push_back(&tile);
  }
}
