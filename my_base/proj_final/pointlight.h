#include "Vector3D.h"
#include "ppc.h"
#include "framebuffer.h"
#pragma once
#include "Vector3D.h"
#include "ppc.h"
#include "framebuffer.h"


class PointLight{
public:
	Vector3D pos;
	bool enabled;


	PointLight(Vector3D pos);
	void Visualize(PPC *ppc, FrameBuffer *fb);
};