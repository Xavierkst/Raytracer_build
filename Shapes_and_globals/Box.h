#ifndef _BOX_H_
#define _BOX_H_

#include "../Lights_Color/Color.h"
#include "Object.h"

class Box : public Object {
private: 
	Color color;
	glm::vec3 normal;
	glm::vec3 centroid;
public:
	// Min and max bound planes
	glm::vec3 bounds[2];

	Box();
	//Box(glm::vec3 min, glm::vec3 max, Color c, float refractIdx, materialType mat);
	// we assume the box to have all side same length for now
	// so no rectangular cuboids here
	Box(glm::vec3 centroid, float sideLength, Color c, float refractIdx, materialType mat);

	bool findIntersection(glm::vec3 orig,
		glm::vec3 dir, float& tNear,
		int& index, glm::vec2& uv) const;

	Color getColor();
	void setColor(float r, float g, float b);
	glm::vec3 getNormal(glm::vec3 point);

	void getSurfaceProperties(const glm::vec3& P, const glm::vec3 orig,
		const glm::vec3& I, const int& index,
		const glm::vec2& uv, glm::vec3& N,
		glm::vec2& st);

	materialType getMaterialType();
};
#endif