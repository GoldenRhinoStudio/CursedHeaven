#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Pathfinding.h"

#include <algorithm>

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.assign("pathfinding");
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

Movement j1PathFinding::CheckDirection(std::vector<iPoint>& path) const
{
	if (path.size() >= 2)
	{
		iPoint tile = path[0];
		iPoint next_tile = path[1];

		int x_difference = next_tile.x - tile.x;
		int y_difference = next_tile.y - tile.y;

		if (x_difference == 1 && y_difference == 1) return DOWN_RIGHT;
		else if (x_difference == 1 && y_difference == -1) return UP_RIGHT;
		else if (x_difference == -1 && y_difference == 1) return DOWN_LEFT;
		else if (x_difference == -1 && y_difference == -1) return UP_LEFT;
		else if (x_difference == 1) return RIGHT;			
		else if (x_difference == -1) return LEFT;
		else if (y_difference == 1)	return DOWN;			
		else if (y_difference == -1) return UP;
	}

	else return NONE;
}

Movement j1PathFinding::CheckDirectionGround(std::vector<iPoint>& path) const
{
	if (path.size() >= 2)
	{
		iPoint tile = path[0];
		iPoint next_tile = path[1];

		int x_difference = next_tile.x - tile.x;
		int y_difference = next_tile.y - tile.y;

		 if (x_difference == 1) return RIGHT;
		else if (x_difference == -1) return LEFT;
		else if (y_difference == 1)	return DOWN;
		else if (y_difference == -1) return UP;
	}

	else return NONE;
}

// To request all tiles involved in the last generated path
const std::vector<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
PathNode* PathList::Find(const iPoint& point)
{
	for (std::list<PathNode>::iterator item = list.begin(); item != list.end(); ++item)
	{
		if ((*item).pos == point)
			return &(*item);
	}

	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
const PathNode* PathList::GetNodeLowestScore() const
{
	const PathNode* ret = NULL;
	int min = 65535;

	for (std::list<PathNode>::const_iterator item = list.end(); item != list.begin(); --item)
	{
		if ((*item).Score() < min)
		{
			min = (*item).Score();
			ret = &(*item);
		}
	}

	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.size();


	//north-east
	cell.create(pos.x + 1, pos.y + 1);
	if (App->path->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//north-west
	cell.create(pos.x - 1, pos.y + 1);
	if (App->path->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south-east
	cell.create(pos.x + 1, pos.y - 1);
	if (App->path->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south-west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->path->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->path->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->path->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->path->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->path->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));



	return list_to_fill.list.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;

	int x_distance = abs(pos.x - destination.x);
	int y_distance = abs(pos.y - destination.y);

	h = (x_distance + y_distance) * min(x_distance, y_distance);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
std::vector<iPoint>* j1PathFinding::CreatePath(iPoint& origin, iPoint& destination)
{
	BROFILER_CATEGORY("CreatePath", Profiler::Color::SlateGray)

	//last_path.clear();
	//// If origin or destination are not walkable, return -1
	//if (IsWalkable(origin) && IsWalkable(destination)) {

	//	// We create two lists: open, close, and we add the origin tile to open, and iterate while we have tile in the open list
	//	PathList open, close;
	//	PathNode origin(0, origin.DistanceNoSqrt(destination), origin, nullptr);
	//	open.list.push_back(origin);

	//	while (open.list.size() > 0)
	//	{
	//		// We move the lowest score cell from open list to the closed list
	//		close.list.push_back(*open.GetNodeLowestScore());
	//		open.list.remove(*open.GetNodeLowestScore());

	//		if (close.list.end->data.pos != destination)
	//		{
	//			// We fill a list of all adjancent nodes
	//			PathList adjancent;

	//			// We iterate adjancent nodes:
	//			close.list.end->data.FindWalkableAdjacents(adjancent);

	//			for (std::list<PathNode>::iterator item = adjancent.list.begin(); item != adjancent.list.end(); ++item)
	//			{
	//				// Ignore nodes in the closed list
	//				if (close.Find((*item).pos))
	//					continue;

	//				// If it is already in the open list, check if it is a better path (compare G)
	//				else if (open.Find((*item).pos))
	//				{
	//					PathNode tmp = *open.Find((*item).pos);
	//					(*item).CalculateF(destination);
	//					if (tmp.g > (*item).g)
	//					{
	//						// If it is a better path, Update the parent
	//						tmp.parent = (*item).parent;
	//					}
	//				}
	//				// If it is NOT found, calculate its F and add it to the open list
	//				else
	//				{
	//					(*item).CalculateF(destination);
	//					open.list.push_back(*item);
	//				}
	//			}
	//			adjancent.list.clear();
	//		}
	//		else
	//		{
	//			// If we just added the destination, we are done!
	//			//for (p2List_item<PathNode>* iterator = close.list.end; iterator->data.parent != nullptr; iterator = close.Find(iterator->data.parent->pos))
	//			for (std::list<PathNode>::iterator item = close.list.end(); (*item).parent != nullptr; item = close.Find((*item).parent->pos))
	//			{
	//				// Backtrack to create the final path
	//				last_path.push_back((*item).pos);
	//				if ((*item).parent == nullptr)
	//					last_path.push_back((*close.list.begin()).pos);
	//			}

	//			// We use the Pathnode::parent and Flip() the path when you are finish
	//			reverse(last_path.begin(), last_path.end());

	//			return &last_path;
	//		}
	//	}
	//}

	return nullptr;
}

