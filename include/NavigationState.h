#ifndef __NAVIGATIONSTATE_H__
#define __NAVIGATIONSTATE_H__

#include <memory>
#include <set>
#include <vector>

class Hierarchy;
class HierarchyNode;
class View;

enum class NavigateResult { SUCCESS, FAILURE };

class NavigationState {
public:

	typedef std::vector<std::shared_ptr<HierarchyNode>> NodeVector;

	std::shared_ptr<HierarchyNode> currentLocation () { return m_currentLocation; }
	NodeVector const &currentChildNodes () { return m_currentLocation; }

	NavigateResult navigateUp ();
	NavigateResult navigateDown (std::shared_ptr<HierarchyNode> const &targetNode);

	void registerView (View &view);
	void unregisterView (View &view);

	void update ();

private:

	std::shared_ptr<Hierarchy> m_currentHierarchy;
	std::shared_ptr<HierarchyNode> m_currentLocation;
	NodeVector m_currentChildNodes;

	typedef std::set<std::weak_ptr<View>> ViewSet;
	ViewSet m_views;
};

#endif
