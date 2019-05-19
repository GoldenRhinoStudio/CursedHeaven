#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Pathfinding.h"
#include "j1Map.h"
#include "j1EntityManager.h"

#include <algorithm>

#include "Brofiler/Brofiler.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), width(0), height(0)
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

Movement j1PathFinding::CheckDirection(const std::vector<iPoint>* path,int * node) const
{
	if (path->size() > 1 && *node < path->size() - 1)
	{
		iPoint tile = { path->at(*node).x, path->at(*node).y };
		iPoint next_tile = { path->at(*node + 1).x, path->at(*node + 1).y };


		int x_difference = next_tile.x - tile.x;
		int y_difference = next_tile.y - tile.y;

		if (x_difference > 0 && y_difference > 0) return DOWN;
		else if (x_difference > 0 && y_difference < 0) return RIGHT;
		else if (x_difference < 0 && y_difference > 0) return LEFT;
		else if (x_difference < 0 && y_difference < 0) return UP;
		else if (x_difference == 0 && y_difference < 0) return UP_RIGHT;
		else if (x_difference < 0 && y_difference == 0) return UP_LEFT;
		else if (x_difference > 0 && y_difference == 0) return DOWN_RIGHT;
		else if (x_difference == 0 && y_difference > 0)	return DOWN_LEFT;
	}
	return NONE;
}

bool j1PathFinding::check_nextTile(const std::vector<iPoint>* path, int* cnode, iPoint* position, Movement direction) {

	if (path->size() > 1 && *cnode != path->size() - 1)
	{
		iPoint prevDest = path->at(*cnode);


		bool reachedX = (direction == RIGHT && position->x >= prevDest.x)
			|| (direction == LEFT && position->x <= prevDest.x || prevDest.x == position->x);
		bool reachedY = (direction == DOWN && position->y >= prevDest.y)
			|| (direction == UP && position->y <= prevDest.y || prevDest.y == position->y);
		return (reachedX & reachedY);
	}
	return false;
}

Movement j1PathFinding::CheckDirectionGround(const std::vector<iPoint>* path) const
{
	if (path->size() >= 2)
	{
		iPoint tile = path->at(0);
		iPoint next_tile = path->at(1);

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
std::vector<iPoint>* j1PathFinding::GetLastPath() const
{
	return last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
std::list<PathNode>::iterator PathList::Find(const iPoint& point)
{
	std::list<PathNode>::iterator item = list.begin();

	while (item != list.end())
	{

		if ((*item).pos == point)
			return item;

		item = next(item);
	}

	return list.end();
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
std::list<PathNode>::iterator PathList::GetNodeLowestScore()
{
	std::list<PathNode>::iterator ret;
	int min = 65535;

	std::list<PathNode>::iterator item = list.begin();

	while (item != list.end())
	{
		if ((*item).Score() < min)
		{
			min = (*item).Score();
			ret = item;
		}
		item = next(item);
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
	/*g = parent->g + 1;

	int x_distance = abs(pos.x - destination.x);
	int y_distance = abs(pos.y - destination.y);

	h = (x_distance + y_distance) * min(x_distance, y_distance);*/

	g = parent->g + 1;

	double D = 1;
	double D2 = 2;
	int dx = abs(pos.x - destination.x);
	int dy = abs(pos.y - destination.y);

	h = D * (dx + dy) + (D2 - 2 * D) * min(dx, dy);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(iPoint& origin, iPoint& destination)
{
	BROFILER_CATEGORY("CreatePath", Profiler::Color::Gray)

	int ret = -1;

	if (!IsWalkable(origin) || !IsWalkable(destination))
		return ret;

	PathList open;
	PathList close;
	last_path = new std::vector<iPoint>();

	open.list.push_back(PathNode(0, 0, origin, NULL));

	while (open.list.size() > 0) {

		std::list<PathNode>::iterator aux = open.GetNodeLowestScore();
		close.list.push_back(*aux);

		std::list<PathNode>::iterator lower = prev(close.list.end());
		open.list.erase(aux);

		if ((*lower).pos == destination) {
			const PathNode *new_node = &(*lower);

			while (new_node) {

				last_path->push_back(new_node->pos);
				new_node = new_node->parent;
			}

			std::reverse(last_path->begin(), last_path->end());
			ret = last_path->size();
			break;
		}

		PathList AdjacentNodes;
		AdjacentNodes.list.clear();

		(*lower).FindWalkableAdjacents(AdjacentNodes);
		std::list<PathNode>::iterator it = AdjacentNodes.list.begin();

		for (; it != AdjacentNodes.list.end(); it = next(it)) {

			if (close.Find((*it).pos) != close.list.end())
				continue;

			std::list<PathNode>::iterator adj_node = open.Find((*it).pos);

			if (adj_node == open.list.end()) {

				(*it).CalculateF(destination);
				open.list.push_back(*it);
			}
			else if ((*adj_node).g > (*it).g + 1) {

				(*adj_node).parent = (*it).parent;
				(*adj_node).CalculateF(destination);

			}
		}
	}

	return ret;
}


PathList PathNode::PruneNeighbours(const iPoint& destination, j1PathFinding* PF_Module) {

	PathList ret;

	//TODO 2: Here we do the step that A* does in its core and that we deleted in TODO1
	//Fill the neighbours list with the real or immediate neighbours
	//Then iterate it
	PathList neighbours;
	FindWalkableAdjacents(neighbours);

	std::list<PathNode>::iterator item = neighbours.list.begin();
	for (; item != neighbours.list.end(); item = next(item)) {

		//TODO 3: For each iteration, calculate the direction from current node
		//to its neighbour. You can use CLAMP (defined in p2Defs)
		iPoint direction(CLAMP((*item).pos.x - pos.x, 1, -1), CLAMP((*item).pos.y - pos.y, 1, -1));

		//TODO 4: Make a Jump towards the calculated direction to find the next Jump Point
		//and, if any Jump Point is found, add it to the list that we must return
		PathNode* JumpPoint = PF_Module->Jump(pos, direction, destination, this);

		if (JumpPoint != nullptr)
			ret.list.push_back(*JumpPoint);
	}

	return ret;
}


PathNode* j1PathFinding::Jump(iPoint current_position, iPoint direction, const iPoint& destination, PathNode* parent) {

	//Determine next possible Jump Point's Position
	iPoint JumpPoint_pos(current_position.x + direction.x, current_position.y + direction.y);

	//If next point isn't walkable, return nullptr
	if (IsWalkable(JumpPoint_pos) == false)
		return nullptr;

	PathNode *ret_JumpPoint = new PathNode(-1, -1, JumpPoint_pos, parent);

	//If next point is goal, return it
	if (ret_JumpPoint->pos == destination)
		return ret_JumpPoint;


	//TODO 5: Check if there is any possible Jump Point in Straight Directions (horizontal and vertical)
	//If found any, return it. Else, keep jumping in the same direction
	/// Checking Horizontals
	if (direction.x != 0 && direction.y == 0) {

		if (IsWalkable(current_position + iPoint(0, 1)) == false && IsWalkable(current_position + iPoint(direction.x, 1)) == true)
			return ret_JumpPoint;

		else if (IsWalkable(current_position + iPoint(0, -1)) == false && IsWalkable(current_position + iPoint(direction.x, -1)) == true)
			return ret_JumpPoint;

	}
	/// Checking Verticals
	else if (direction.x == 0 && direction.y != 0) {

		if (IsWalkable(current_position + iPoint(1, 0)) == false && IsWalkable(current_position + iPoint(1, direction.y)) == true)
			return ret_JumpPoint;

		else if (IsWalkable(current_position + iPoint(-1, 0)) == false && IsWalkable(current_position + iPoint(-1, direction.y)) == true)
			return ret_JumpPoint;
	}
	//TODO 6: Do the same check than for Straight Lines but now for Diagonals!
	//(Remember prunning rules for diagonals!)
	/// Checking Diagonals
	else if (direction.x != 0 && direction.y != 0) {

		if (IsWalkable(current_position + iPoint(direction.x, 0)) == false)
			return ret_JumpPoint;
		else if (IsWalkable(current_position + iPoint(0, direction.y)) == false)
			return ret_JumpPoint;

		if (Jump(JumpPoint_pos, iPoint(direction.x, 0), destination, parent) != nullptr
			|| Jump(JumpPoint_pos, iPoint(0, direction.y), destination, parent) != nullptr)
			return ret_JumpPoint;
	}

	return Jump(JumpPoint_pos, direction, destination, parent);
}

