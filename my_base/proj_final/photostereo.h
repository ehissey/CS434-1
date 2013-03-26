#pragma once

#include "framebuffer.h"
#include "Vector3D.h"
#include "Matrix3x3.h"

class Photostereo{
public:
	Photostereo(bool r);
	~Photostereo();
	void GenerateResult();
	void UpdateResult();

	FrameBuffer ** images;
	FrameBuffer * result;

	Vector3D * n;
	float * a;
	Vector3D c;
	Vector3D * vec;
	Matrix3x3 L;

	bool real;
	bool generated;
	bool enabled;
	int effect;
};