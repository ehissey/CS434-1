#pragma once

#include "Vector3D.h"

class AABB{
public:
	Vector3D corners[2];
	AABB(Vector3D P);
	void AddPoint(Vector3D P);
	bool Clip(int w, int h);
};