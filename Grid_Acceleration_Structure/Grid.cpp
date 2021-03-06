#include "Grid.h"

Grid::Grid(std::vector<Object*>& objs, std::vector<LightSources*>& lights) :
	AccelerationStructure(objs) {
	// determine the bounds of the grid by iterating
	// thru all scene objects and expanding its bounds accordingly
	// USE extendBy for each object iterated thru
	for (int i = 0; i < objects.size(); ++i) {
		// keeps enlarging the grid until it perfectly 
		// encapsulates every geometry
		if (typeid(*objects[i]) == typeid(Plane)) continue;
		gridBbox.extendBy(objects[i]->bbox.minBounds);
		gridBbox.extendBy(objects[i]->bbox.maxBounds);
		
		// print out the values
		//std::cout << "current Object Bbox: " << std::endl;
		//std::cout << objects[i]->bbox.minBounds.x << " " << objects[i]->bbox.minBounds.y << " " << objects[i]->bbox.minBounds.z << " " << std::endl;
		//std::cout << objects[i]->bbox.maxBounds.x << " " << objects[i]->bbox.maxBounds.y << " " << objects[i]->bbox.maxBounds.z << " " << std::endl;
		//std::cout << "grid Bbox Object: " << std::endl;
		std::cout << gridBbox.minBounds.x << " " << gridBbox.minBounds.y << " " << gridBbox.minBounds.z << " " << std::endl;
		std::cout << gridBbox.maxBounds.x << " " << gridBbox.maxBounds.y << " " << gridBbox.maxBounds.z << " " << std::endl;
	}

	// 1. Determining grid size
	// 2. Determine the sub-division
	// dimensions by finding cellDimensions, resolution, size, 
	glm::vec3 diff = gridBbox.maxBounds - gridBbox.minBounds;
	glm::vec3 size;
	//int resolution[3];
	int lambda = 5;
	for (int j = 0; j < 3; ++j) {
		size[j] = fabsf(diff[j]); 
		resolution[j] = 5;
	}
	cellDimensions = glm::vec3(size.x / resolution[0], size.y / resolution[1], size.z / resolution[2]);
	// We won't use the formula for resolution now since we haven't included polygonal meshes yet
	// since we need N, the # of tris in each cell
	//resolution[0] =  floor(size.x * pow(lambda * objects.size() * (1.0f / (size.x * size.y * size.z)), 1.0f / 3.0f));
	//resolution[1] = floor(size.y * pow(lambda * objects.size() * (1.0f / (size.x * size.y * size.z)), 1.0f / 3.0f));
	//resolution[2] = floor(size.z * pow(lambda * objects.size() * (1.0f / (size.x * size.y * size.z)), 1.0f / 3.0f));

	// create a 1-D cell array with resx * resy * resz elements
	numCells = resolution[0] * resolution[1] * resolution[2];
	cells = new Cell*[numCells];
	
	// memset them all to nullptr for now
	memset(cells, 0x0, sizeof(Grid::Cell*) * numCells);

	// populate the cells with triangles/objects 
	// the ones with no objects in theirs cells remain nullptr
	
	// find the cell coords for xmin and xmax (y & z also)
	// loop thru all the objects and insert each one into the grid
	// based on their max and min bounding points
	int totalItems = 0;
	for (int k = 0; k < objects.size(); ++k) {
		//totalItems++;
		// find the x/y/z indices 
		// subtract minimum of object Bbox with min of grid's Bbox
		// and divide by cellDimensions x/y/z to get the "normalized" value,
		// and floor to convert to the corresp cell index where the min/max
		// point resides in
		glm::vec3 minDiff = (objects[k]->bbox.minBounds - gridBbox.minBounds);
		glm::vec3 maxDiff = (objects[k]->bbox.maxBounds - gridBbox.minBounds);
		
		// since z is always negative and we want a +ve 
		// index, we negate for z. 
		// Obtain cell coordinates for each object
		int zmin = clamp(0, resolution[2] - 1, floor(-minDiff.z / cellDimensions.z));
		int zmax = clamp(0, resolution[2] - 1, floor(-maxDiff.z / cellDimensions.z));
		int ymin = clamp(0, resolution[1] - 1, floor(minDiff.y / cellDimensions.y));
		int ymax = clamp(0, resolution[1] - 1, floor(maxDiff.y / cellDimensions.y));
		int xmin = clamp(0, resolution[0] - 1, floor(minDiff.x / cellDimensions.x));
		int xmax = clamp(0, resolution[0] - 1, floor(maxDiff.x / cellDimensions.x));
		// loop thru cells to insert object inside them
		for (int z = zmin; z <= zmax; ++z) {
			for (int y = ymin; y <= ymax; ++y) {
				for (int x = xmin; x <= xmax; ++x) {
					// get the current grid idx -- its 1-dimensional
					// each z index has traversed x*y elems, same idea for y etc...
					int curIdx = z * resolution[0] * resolution[1] + y * resolution[0] + x;
					if (cells[curIdx] == nullptr) { // cells with objects are not null
						cells[curIdx] = new Cell;
					}
					// insert object into Cell object array
					// cells[curIdx]
					cells[curIdx]->insertObject(objects[k]);
					std::cout << curIdx << std::endl;
				}
			}
		}

	}
	//std::vector<Object*> objPtrs;
	//std::cout << numCells << std::endl;
	////int totalItems = 0;
	//for (int m = 0; m < numCells; ++m) {
	//	if (cells[m] != nullptr) {
	//		for (int a = 0; a < cells[m]->cellObjects.size(); ++a) {
	//			std::cout << cells[m]->cellObjects.size() << std::endl;
	//			totalItems += cells[m]->cellObjects.size();
	//		}
	//	}
	//}
	std::cout << "total object count is " << totalItems << std::endl;
}

// frees the array of pointers to Cell objects, then finally frees
// pointer to the array of Cell pointers
Grid::~Grid() {
	for (int i = 0; i < resolution[0] * resolution[1] * resolution[2]; ++i) {
		delete cells[i];
	}
	delete cells;
}

int Grid::clamp(const int& lo, const int& hi, const int& v) {
	return std::max(lo, std::min(hi, v));
}

bool Grid::intersect(glm::vec3 orig, glm::vec3 dir, const std::vector<Object*>& objects, float& tNear, int& objIndex, glm::vec2& uv, Object* hitObj) {
	// prep the deltaT, nextCrossingT, initial x/y/z, 
	// find the starting cell coordinates using 	
	// rayOriginGrid 

	// Check if ray intersects Grid's Bounding Box
	// If ray orig is inside, box intersection routine still returns
	// true 
	if (!gridBbox.findIntersection(orig, dir, tNear)) return false;

	// ray intersects the grid bounding box (either from the outside
	// or inside) 
	// set up the neccessary index for bounds, exit, and cell, 
	// and set up parametric distance variables
	glm::vec3 rayOriginGrid;
	// the next closest parametric intersect dist
	glm::vec3 nextCrossingT; 
	// the parametric dist traversed due to i-th axis
	glm::vec3 deltaT; 
	int cell[3], step[3], exit[3];
	// loop thru all 3 axes to initialize each respective component
	for (int i = 0; i < NUM_AXES; ++i) {
		// tNear is the distance from ray origin (outside) to intersct pt
		// in the i-th dimension to grid
		rayOriginGrid[i] = (orig[i] + tNear * dir[i]) - gridBbox.minBounds[i];
		// find starting cell index in the i-th axis
		cell[i] = clamp(floor(rayOriginGrid[i] / 
			cellDimensions[i]), 0, resolution[i] - 1);


		// check if i-th direction is + or -ve
		if (dir[i] < .0f) {
			//if (i == 0 || i == 1) {
				// the parametric dist traversed due to i-th axis
				deltaT[i] = -cellDimensions[i] / dir[i];
				
				if (i == 0 || i == 1) {
					nextCrossingT[i] = tNear + ((cell[i] * cellDimensions[i] - rayOriginGrid[i]) / dir[i]);
					// decide cell idx step direction
					step[i] = -1;
					// set exit bounds 
					exit[i] = -1;
				}
				else {
					nextCrossingT[i] = tNear + (( (cell[i] + 1) * cellDimensions[i] - rayOriginGrid[i]) / dir[i]);
					// decide cell idx step direction
					step[i] = 1;
					// set exit bounds 
					exit[i] = resolution[i];
				}
			//}
			//else {
			//	deltaT[i] = -cellDimensions[i] / dir[i];
			//	nextCrossingT[i] = tNear + (((cell[i] + 1) * cellDimensions[i] - rayOriginGrid[i]) / -dir[i]);
			//	// decide cell idx step direction
			//	step[i] = 1;
			//	// set exit bounds 
			//	exit[i] = resolution[i];
			//}
		}
		else {
			//if (i == 0 || i == 1) {
				deltaT[i] = cellDimensions[i] / dir[i];
				nextCrossingT[i] = tNear + (((cell[i] + 1) * cellDimensions[i] - rayOriginGrid[i]) / dir[i]);
				// decide cell idx step direction
				step[i] = 1;
				// set exit bounds 
				exit[i] = resolution[i];
			//}
			//else {
			//	// the parametric dist traversed due to i-th axis
			//	deltaT[i] = cellDimensions[i] / dir[i];
			//	nextCrossingT[i] = tNear + ((cell[i] * cellDimensions[i] - rayOriginGrid[i]) / dir[i]);
			//	// decide cell idx step direction
			//	step[i] = -1;
			//	// set exit bounds 
			//	exit[i] = -1;
			//}
		}
	}

	// We want to retrieve a pointer to the object

	// Begin traversing the cells of the Grid and do intersection
	// test at each cell, return reference to object hit
	while (1) {
		// carry out intersection test at current cell
		int index = cell[2] * resolution[1] * resolution[0] + 
			cell[1] * resolution[0] + cell[0];
		// NOTE: cell != cells -- if cells contains object
		// intersection check it: returns pointer to object if obj is hit
		if (cells[index] != nullptr) { 
			cells[index]->intersect(orig, dir, tNear, hitObj);
		}

		// To check if object is hit we need find 
		// the axis for nextCrossingT (which dimension is closest)
		
		// use a map to map index to closest axis
		int map[8] = { 2, 1, 2, 1, 2, 2, 0, 0 };
		// Right shift by 2 i.e. 1 << 2 == 4 (decimal)
		int axis = map[((nextCrossingT[0] < nextCrossingT[1]) << 2) +
			((nextCrossingT[0] < nextCrossingT[2]) << 1) +
			(nextCrossingT[1] < nextCrossingT[2])];
		// now we have the parametric dist to the closest
		// intersecting axis (x/y/z) 
		
		// check if object is hit, and if the intersection 
		// dist is within cell confines -- we break out of loop
		// if we did. We now have object's surf. color
		if (hitObj != nullptr && tNear < nextCrossingT[axis]) {
			break;
		}
		// else no hit, traverse to next cell
		nextCrossingT[axis] += deltaT[axis];
		cell[axis] += step[axis];
		// check if traversal bounds exceeded 
		if (cell[axis] == exit[axis]) break;
	}
	// if hitObj references an object, then 
	// intersection found is true, else nullptr means false
	return (hitObj != nullptr);
}