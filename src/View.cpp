#include "StdAfx.h"
#include "NavigationState.h"
#include "Value.h"
#include "View.h"

const double View::CAM_DISTANCE_FROM_PLANE = 50.0;

View::View(std::shared_ptr<NavigationState> const &ownerNavigationState)
  :
  m_ownerNavigationState(ownerNavigationState)
{
  m_position = CAM_DISTANCE_FROM_PLANE * Vector3::UnitZ();
  m_lookat = Vector3::Zero();
  m_up = Vector3::UnitY();
  m_fov = 80.0f;
  m_near = 1.0f;
  m_far = 500.0f;
  m_sizeLayout = std::shared_ptr<SizeLayout>(new UniformSizeLayout());
  m_positionLayout = std::shared_ptr<PositionLayout>(new GridLayout());
  m_lookatSmoother.Update(m_lookat, 0.0, 0.5f);

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
      if (node) {
        ExtractPrioritizedKeysFrom(*node, m_sortingCriteria);
      }
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
  for (TileVector::iterator it = m_tiles.begin(); it != m_tiles.end(); ++it) {
    Tile& tile = *it;
    if (tile.Activation() > 0.99f && tile.Position().z() > 10.0) {
      selectedNode = tile.Node();
      break;
    }
  }

  if (selectedNode &&
     !selectedNode->open() &&
     !selectedNode->is_leaf()) {
    m_tiles.clear();
    m_ownerNavigationState->navigateDown(selectedNode);
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
