#ifndef __VIEW_H__
#define __VIEW_H__

#include <memory>
#include <vector>

#include "Layout.h"

class HierarchyNode;
class Layout;

class View {
public:

	View (NavigationState &ownerNavigationState);
	~View ();



private:

	// TODO: sorting criteria
	std::vector<std::shared_ptr<HierarchyNode>> m_sortedChildren;
	std::vector<SceneSpaceVector> m_sortedChildPositions;
	std::shared_ptr<Layout> m_layout;
};

#endif
