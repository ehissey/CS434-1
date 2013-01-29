#pragma once

#include "framebuffer.h"
#include "ppc.h"
#include "Vector3D.h"

class DepthImage{
public:
	FrameBuffer* frame;
	PPC* camera;
	TMesh * object;
	int rgbTexID;
	int depthTexID;
	bool rendered;

	DepthImage(TMesh * diffuseObject);
	~DepthImage();
	void renderImage();
};