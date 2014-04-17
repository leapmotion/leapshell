#ifndef __LAYOUT_H__
#define __LAYOUT_H__

#include "HierarchyNode.h"

#include <memory>
#include <vector>

class SceneSpaceVector { }; // TEMP

class Layout {
	virtual void calculate_desired_positions_of (std::vector<std::shared_ptr<HierarchyNode>> const &nodes,
		                                         std::vector<std::shared_ptr<SceneSpaceVector>> &desired_positions);
};

#endif
