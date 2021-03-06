#ifndef _GRID_H_
#define _GRID_H_

#include "AccelerationStructure.h"
#include "Bbox.h"
#include "../Lights_Color/Color.h"
#include "../Lights_Color/Light.h"
#include <iostream>
#include "../Shapes_and_globals/Plane.h"

#define NUM_AXES 3 // the total number of axes

class Grid : public AccelerationStructure {
	struct Cell {
		Cell() {}

		// triangle descriptor struct
		// holds description for each triangle (a ptr and a offset)
		struct TriangleDes {
			Object* obj;
			int idx;
			// Should be a mesh pointer but we haven't included
			// triangle meshes yet
			TriangleDes(Object* objPtr, int index) : obj(objPtr), idx(index) {}

		};
		void insertTriangles(TriangleDes tri) {
			triangles.push_back(tri);
		}

		void insertObject(Object* obj) {
			cellObjects.push_back(obj);
		}
		// Cell's Intersect function: iterates thru all triangles
		// inserted in the cell, and stores a record abt closest one
		// returns true or false if a triangle in the mesh was intersected
		//bool intersect(glm::vec3 orig, glm::vec3 dir, Object* hitObj);

		// iterates thru triangles vector and finds the closest ray-triangle inter.
		// in the cell, stores that triangle/oject
		bool intersect(glm::vec3 orig, glm::vec3 dir, float& tNear, Object* hitObj) {
			glm::vec2 uv;
			int objectIdx = 0;
			float tCurrNearest = FLT_MAX;
			for (int i = 0; i < cellObjects.size(); ++i) {
				// findIntersectionwill save tCurrNearest, and pointer to nearest
				// object -- (we don't use objectIdx and uv at the moment..)
				if (cellObjects[i]->findIntersection(orig, dir, tCurrNearest, objectIdx, uv) 
					&& tCurrNearest < tNear) {
					 tNear = tCurrNearest;
					hitObj = cellObjects[i];
				}
			}
			return (hitObj != nullptr);
		}

		std::vector<TriangleDes> triangles;
		std::vector<Object*> cellObjects;
		Color color;
	};
private: 


public:

	Grid();

	// define grid constructor
	Grid(std::vector<Object*>& objs, std::vector<LightSources*>& lights);

	// frees the array of pointers to Cell objects, then finally frees
	// pointer to the array of Cell pointers
	~Grid();

	// clamps the integer value in the range [lo, hi]
	int clamp(const int& lo, const int& hi, const int& v);

	// x y z dimensions 
	int resolution[3];
	glm::vec3 cellDimensions;

	// resolution: X by Y by Z 
	int numCells;
	Cell** cells;
	Bbox gridBbox;
	
	// Initiates intersection routine of 
	// cam ray casted into the scene into the grid 
	// if the grid is intersected, traverse ray thru the cells 
	// using 3D-DDA algorithm. Returns pointer to the object 
	// intersected 
	bool intersect(glm::vec3 orig, glm::vec3 dir, 
		const std::vector<Object*>& objects, float& tNear, 
		int& objIndex, glm::vec2& uv, 
		Object* hitObj);
};
#endif