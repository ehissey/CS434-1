#pragma once

#include "framebuffer.h"
#include "ppc.h"
#include "Vector3D.h"

class DepthImage{
public:
	unsigned int * rgb;
	float * depths;
	PPC * camera;
	TMesh * diffuseObject;
	int rgbTexID;
	int depthTexID;
	bool rendered;
	bool cameraSet;
	int u0;
	int v0;
	int w;
	int h;

	DepthImage(FrameBuffer * frame, PPC * ppc, TMesh * diffuseObject, int rID, int dID);
	~DepthImage();
	void renderImage();
};