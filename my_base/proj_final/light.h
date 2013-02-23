#pragma once

#include "Vector3D.h"
#include "ppc.h"
#include "framebuffer.h"

#include <string>

using namespace std;

class Light{
public:
	PPC * lppc;
	bool enabled;
	float * lightVector;  //Values for the grid lights, ordered by rows.
	float * lightTransportMatrix;
	FrameBuffer * lightFrameBuffer;
	int w;
	int h;

	Light(int width, int height, float hfov, Vector3D eye, Vector3D lookAt, PPC * cppc);
	~Light();

	bool loadLightVector(string filename);  //Load the light vector from a file
	bool setLightVector(float value);  //Set all values of the light vector
	bool reverseLightVector();  //Reverse all values of the light vector
	bool saveLightVectorAsImage();
};