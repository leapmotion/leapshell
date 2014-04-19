#include "StdAfx.h"
#include "Tile.h"
#include "Globals.h"

const float Tile::POSITION_SMOOTH = 0.85f;
const float Tile::ACTIVATION_SMOOTH = 0.95f;

Tile::Tile() {
  m_position = 100 * Vector3::Random();
  m_position.z() = 0.0;
  m_size = Vector3::Constant(15);
  m_highlightSmoother.Update(0.0f, Globals::curTimeSeconds, 0.5f);
  m_activationSmoother.Update(0.0f, Globals::curTimeSeconds, 0.5f);
}

struct TileOrder {
  TileOrder (std::vector<std::string> const &prioritizedKeys) : m_prioritizedKeys(prioritizedKeys) { }
  bool operator () (std::shared_ptr<Tile> const &lhs, std::shared_ptr<Tile> const &rhs) {
    int compareResult = 0;
    for (auto it = m_prioritizedKeys.begin(), it_end = m_prioritizedKeys.end(); it != it_end; ++it) {
      std::string const &key = *it;
      Value const &lhs_metadata = lhs->m_node->metadata()[key];
      Value const &rhs_metadata = rhs->m_node->metadata()[key];
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
