#ifndef __VIEW_H__
#define __VIEW_H__

#include <memory>
#include <vector>

#include "DataTypes.h"
#include "Layout.h"
#include "Tile.h"

class HierarchyNode;
class Layout;
class NavigationState;

class View {
public:

	View (NavigationState &ownerNavigationState);
  void Update();
	~View ();

  const TileVector& Tiles() const { return m_tiles; }
  const Vector3& Position() const { return m_position; }
  const Vector3& LookAt() const { return m_lookat; }
  const Vector3& Up() const { return m_up; }
  float FOV() const { return m_fov; }
  float Near() const { return m_near; }
  float Far() const { return m_far; }

private:

	// TODO: sorting criteria
	std::vector<std::shared_ptr<HierarchyNode>> m_sortedChildren;
	std::shared_ptr<Layout> m_layout;
  TileVector m_tiles;

  // render parameters
  Vector3 m_position;
  Vector3 m_lookat;
  Vector3 m_up;
  float m_fov;
  float m_near;
  float m_far;

};

#endif
