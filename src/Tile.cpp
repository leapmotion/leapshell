#include "StdAfx.h"
#include "Tile.h"
#include "Globals.h"

const float Tile::POSITION_SMOOTH = 0.85f;
const float Tile::SIZE_SMOOTH = 0.85f;
const float Tile::ACTIVATION_SMOOTH = 0.925f;
const float Tile::GRABDELTA_SMOOTH = 0.5f;

Tile::Tile() {
  m_positionSmoother.value = Vector3::Zero();
  m_sizeSmoother.value = Vector3::Constant(15);
  m_highlightSmoother.value = 0.0f;
  m_activationSmoother.value = 0.0f;
  m_grabDeltaSmoother.value = Vector3::Zero();
  m_targetGrabDelta = Vector3::Zero();
  m_visible = true;
}

Vector3 Tile::OrigPosition() const {
  return m_positionSmoother.value;
}

Vector3 Tile::Position() const {
  return m_positionSmoother.value + m_grabDeltaSmoother.value;
}

Vector3 Tile::Size() const {
  return m_sizeSmoother.value;
}

float Tile::Highlight() const {
  return m_highlightSmoother.value;
}

float Tile::Activation() const {
  return m_activationSmoother.value;
}

double Tile::LastActivationUpdateTime() const {
  return m_activationSmoother.lastTimeSeconds;
}

float Tile::SwitchWarmupFactor() const {
  return SmootherStep(static_cast<float>(std::min(1.0, (Globals::curTimeSeconds - Globals::lastTileSwitchTime)/Globals::SWITCH_TIME)));
}

float Tile::TransitionWarmupFactor() const {
  return SmootherStep(static_cast<float>(std::min(1.0, (Globals::curTimeSeconds - std::max(m_visibleTime, Globals::lastTileTransitionTime))/Globals::TRANSITION_TIME)));
}

Vector3 Tile::GrabDelta() const {
  return m_grabDeltaSmoother.value;
}

const Vector3& Tile::TargetGrabDelta() const {
  return m_targetGrabDelta;
}

bool Tile::IsVisible() const {
  return m_visible;
}

void Tile::UpdateSize(const Vector3& newSize, float smooth) {
  m_sizeSmoother.Update(newSize, Globals::curTimeSeconds, smooth);
}

void Tile::UpdatePosition(const Vector3& newPosition, float smooth) {
  float warmupFactor = SwitchWarmupFactor();
  m_positionSmoother.Update(newPosition, Globals::curTimeSeconds, smooth + (1.0f-smooth)*(1.0f-warmupFactor));
}

void Tile::UpdateHighlight(float newHighlight, float smooth) {
  m_highlightSmoother.Update(newHighlight, Globals::curTimeSeconds, smooth);
}

void Tile::UpdateActivation(float newActivation, float smooth) {
  m_activationSmoother.Update(newActivation, Globals::curTimeSeconds, smooth);
}

void Tile::UpdateTargetGrabDelta(const Vector3& newGrabDelta) {
  m_targetGrabDelta = newGrabDelta;
}

void Tile::UpdateGrabDelta(float smooth) {
  m_grabDeltaSmoother.Update(m_targetGrabDelta, Globals::curTimeSeconds, smooth);
}

void Tile::ResetActivation() {
  UpdateHighlight(0.0f, 0.01f);
  UpdateActivation(0.0f, 0.01f);
}

void Tile::SetVisible(bool visible) {
  if (visible && !m_visible) {
    m_visibleTime = Globals::curTimeSeconds;
  }
  m_visible = visible;
}

struct TileOrder {
  TileOrder (std::vector<std::string> const &prioritizedKeys) : m_prioritizedKeys(prioritizedKeys) { }
  bool operator () (Tile *&lhs, Tile *&rhs) {
    assert(lhs != nullptr);
    assert(rhs != nullptr);
    if (m_prioritizedKeys.empty()) {
      // we have to set a fallback ordering which is used if there are no prioritized keys.
      // use the paths of the hierarchy nodes for the tiles
      assert(rhs->Node() && "rhs Tile has invalid HierarchyNode member m_node (this should never happen)");
      assert(lhs->Node() && "lhs Tile has invalid HierarchyNode member m_node (this should never happen)");
      return lhs->Node()->path() < rhs->Node()->path();
    }
    // otherwise go through the keys in priority order and check ordering on those values
    int compareResult = 0;
    for (auto it = m_prioritizedKeys.begin(), it_end = m_prioritizedKeys.end(); it != it_end; ++it) {
      std::string const &key = *it;
      Value const &lhs_metadata = lhs->Node()->metadata()[key];
      Value const &rhs_metadata = rhs->Node()->metadata()[key];
      // the value for key in the hash might be IsNull, which is fine.  not all nodes
      // should be expected to have completely identical keys.  the compare function
      // also takes into account the Value type.
      compareResult = lhs_metadata.compare(rhs_metadata);
      if (compareResult != 0) {
        break;
      }
    }
    return compareResult < 0; // if compare returns a negative number, then lhs < rhs.
  }
private:
  std::vector<std::string> const m_prioritizedKeys;
};

void SortTiles (TilePointerVector &v, std::vector<std::string> const &prioritizedKeys) {
  std::sort(v.begin(), v.end(), TileOrder(prioritizedKeys));
}
