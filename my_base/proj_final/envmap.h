#pragma once

#include "framebuffer.h"
#include "ppc.h"
#include "Vector3D.h"

class Envmap{
public:
	FrameBuffer** frames;
	PPC** cameras;
	int lastChecked;
	int texID;

	Envmap();
	~Envmap();
	unsigned int getColor(Vector3D dir);
	void LoadHW(int texBinding);
};