#include "StdAfx.h"
#include "Tile.h"

Tile::Tile() {
  m_position = 100 * Vector3::Random();
  m_position.z() = 0.0;
  m_size = Vector3::Constant(10);
  m_name = "Node";
}
