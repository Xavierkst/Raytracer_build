#ifndef _OPTIONS_H_
#define _OPTIONS_H_

#include <glm/glm.hpp>
#include "Lights_Color/Color.h"

#define MAX_RECURSION_DEPTH 8
#define STARTING_DEPTH 0

class Options {
private:

public:
	int width;
	int height;
	float fov;
	float aspectRatio;
	float ambientLight;
	Color backgroundColor;
	float bias;
	int selectScene;
	float sampleNum;

	// Camera variables: 
	glm::vec3 cameraPos;
	glm::vec3 cameraForward;
	glm::vec3 cameraReferUp;
	glm::vec3 cameraRight;

	bool softShadows;
	// default constructor
	Options() {
		softShadows = true;
		selectScene = 1;
		sampleNum = 12;
		width = 1080;
		height = 720;
		aspectRatio = (float)width / (float)height;
		fov = M_PI * (90.0f / 180.0f);
		ambientLight = 0.4f;
		bias = 0.01f;
		backgroundColor = Color(
			201.0f / 255.0f, 
			226.0f / 255.0f, 
			255.0f / 255.0f, .0f);

		// Camera
		cameraPos = glm::vec3(.0f, -.2f, 0.0f);
		cameraForward = glm::vec3(.0f, .0f, -1.0f);
		cameraReferUp = glm::vec3(.0f, 1.0f, .0f);
		cameraRight = glm::vec3(1.0f, .0f, .0f);

		//cameraPos = glm::vec3(.0f, 0.f, 0.0f);
		//cameraForward = glm::vec3(.0f, -.10f, -1.0f); 
	}
};
#endif 