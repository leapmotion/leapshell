#include "StdAfx.h"
#include "NavigationState.h"
#include "Value.h"
#include "View.h"
#include "Globals.h"

const double View::CAM_DISTANCE_FROM_PLANE = 50.0; // mm
const double View::TILE_PULL_THRESHOLD = 30.0; // mm
const double View::PUSH_THRESHOLD = 1.1 * CAM_DISTANCE_FROM_PLANE; // mm

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
  m_fov = 80.0f;
  m_near = 1.0f;
  m_far = 500.0f;
  m_transitionOpacity = 1.0f;
  m_sizeLayout = std::shared_ptr<SizeLayout>(new UniformSizeLayout());
  m_positionLayout = std::shared_ptr<PositionLayout>(new GridLayout());
  m_lookatSmoother.Update(m_lookat, 0.0, 0.5f);
  m_lastSwitchTime = 0.0;

  m_handL = new MeshHand("Left Hand", MeshHand::LEFT);
  m_handR = new MeshHand("Right Hand", MeshHand::RIGHT);

  m_handL->SetScale(0.75f);
  m_handR->SetScale(0.75f);
}

View::~View () {
  delete m_handL;
  delete m_handR;
}

void View::UpdateFromChangedNavigationState() {
  m_sortedTiles.clear();
  if (m_ownerNavigationState) {
    if (m_sortingCriteria.PrioritizedKeys().empty()) {
      std::shared_ptr<HierarchyNode> node = m_ownerNavigationState->currentLocation();
      // if (node) {
      //   ExtractPrioritizedKeysFrom(*node, m_sortingCriteria);
      // }
    }
    RegenerateTilesAndTilePointers(m_ownerNavigationState->currentChildNodes(), m_tiles, m_sortedTiles);
    // NOTE: now the elements of m_sortedTiles point to elements of m_tiles, so m_sortedTiles
    // should be cleared and regenerated if m_tiles is changed.
  }
  SortTiles(m_sortedTiles, m_sortingCriteria.PrioritizedKeys());

  PerFrameUpdate();
}

void View::PerFrameUpdate () {
  // update the sizes first (a PositionLayout implementation often depends on the sizes)
  m_sizeLayout->UpdateTileSizes(range(m_sortedTiles.begin(), m_sortedTiles.end()));
  // then update the positions
  m_positionLayout->UpdateTilePositions(range(m_sortedTiles.begin(), m_sortedTiles.end()));

  std::shared_ptr<HierarchyNode> selectedNode(nullptr);
  double maxTileZ = 0;
  float maxActivation = 0;
  for (TileVector::iterator it = m_tiles.begin(); it != m_tiles.end(); ++it) {
    Tile& tile = *it;
    maxTileZ = std::max(maxTileZ, tile.Position().z());
    maxActivation = std::max(maxActivation, tile.Activation());
    if (tile.Activation() > 0.95f && tile.Position().z() > TILE_PULL_THRESHOLD) {
      selectedNode = tile.Node();
      break;
    }
  }

  // calculate a fade when about to change navigation state
  const float pullOpacity = SmootherStep(std::max(1.0f - maxActivation, (maxTileZ > TILE_PULL_THRESHOLD ? 0.0f : std::max(0.0f, static_cast<float>(TILE_PULL_THRESHOLD - maxTileZ)/10.0f))));
  const float pushOpacity = SmootherStep(m_position.z() > PUSH_THRESHOLD ? 0.0f : std::max(0.0f, static_cast<float>(PUSH_THRESHOLD - m_position.z())/3.0f));
  m_transitionOpacity = std::min(pullOpacity, pushOpacity);

  static const double MIN_TIME_BETWEEN_SWITCH = 0.5; // in seconds, how much time must elapse between changes in navigation
  if (selectedNode &&
     !selectedNode->open() &&
     !selectedNode->is_leaf() &&
     (Globals::curTimeSeconds - m_lastSwitchTime) > MIN_TIME_BETWEEN_SWITCH) {
    resetView();
    m_tiles.clear();
    m_additionalZ.Update(CAM_DISTANCE_FROM_PLANE, Globals::curTimeSeconds, 0.1f);
    m_lastSwitchTime = Globals::curTimeSeconds;
    m_ownerNavigationState->navigateDown(selectedNode);
    Globals::haveSeenOpenHand = false;
  } else if (m_position.z() > PUSH_THRESHOLD && (Globals::curTimeSeconds - m_lastSwitchTime) > MIN_TIME_BETWEEN_SWITCH) {
    resetView();
    m_additionalZ.Update(-CAM_DISTANCE_FROM_PLANE, Globals::curTimeSeconds, 0.1f);
    m_lastSwitchTime = Globals::curTimeSeconds;
    m_ownerNavigationState->navigateUp();
  } else {
    m_additionalZ.Update(0.0, Globals::curTimeSeconds, 0.965f);
  }
}

void View::SetSizeLayout(const std::shared_ptr<SizeLayout>& sizeLayout) {
  m_sizeLayout = sizeLayout;
}

void View::SetPositionLayout(const std::shared_ptr<PositionLayout>& positionLayout) {
  m_positionLayout = positionLayout;
}

void View::ApplyVelocity(const Vector3& velocity, double timeSeconds, double deltaTime) {
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

  m_lookatSmoother.Update(m_lookat, timeSeconds, 0.5f);
}

void View::SetPosition(const Vector3& position) {
  m_position = clampCameraPosition(position);
}

void View::SetLookAt(const Vector3& lookat) {
  m_lookat = clampCameraPosition(lookat);
}

void View::resetView() {
  m_position = CAM_DISTANCE_FROM_PLANE * Vector3::UnitZ();
  m_lookat = Vector3::Zero();
}

Vector3 View::clampCameraPosition(const Vector3& position) const {
  Vector3 result(position);
  result.head<2>() = result.head<2>().cwiseMax(m_positionLayout->GetCameraMinBounds()).cwiseMin(m_positionLayout->GetCameraMaxBounds());
  return result;
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

void View::RegenerateTilesAndTilePointers(const HierarchyNodeVector &nodes, TileVector &tiles, TilePointerVector &tilePointers) {
  // clear the vector of Tile pointers BEFORE tiles is changed, because they point to elements of tiles.
  tilePointers.clear();
  // create a Tile for each node
  tiles.clear();
  tiles.resize(nodes.size());
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
