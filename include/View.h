#ifndef __VIEW_H__
#define __VIEW_H__

#include <memory>
#include <vector>

#include "DataTypes.h"
#include "Layout.h"
#include "Tile.h"
#include "Utilities.h"

class HierarchyNode;
class Layout;
class NavigationState;

class View {
public:

	View (std::shared_ptr<NavigationState> const &ownerNavigationState);
  void Update();
	~View ();

  // getters
  const TileVector& Tiles() const { return m_tiles; }
  const Vector3& Position() const { return m_position; }
  const Vector3& LookAt() const { return m_lookat; }
  const Vector3& Up() const { return m_up; }
  float FOV() const { return m_fov; }
  float Near() const { return m_near; }
  float Far() const { return m_far; }

  // setters
  void ApplyVelocity(const Vector3& velocity, double deltaTime);
  void SetPosition(const Vector3& position);
  void SetLookAt(const Vector3& lookat);

private:

  Vector3 clampCameraPosition(const Vector3& position) const;

  std::shared_ptr<NavigationState> m_ownerNavigationState;

  // TODO: sorting criteria
  std::vector<std::shared_ptr<HierarchyNode>> m_sortedChildren;
  TileVector m_sortedChildPositions;
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
