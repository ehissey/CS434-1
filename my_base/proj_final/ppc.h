#pragma once

#include "Vector3D.h"
#include "Matrix3x3.h"
#include <string>

class FrameBuffer;

class PPC{
public:
	Vector3D a, b, c, C;
	int w, h;
	float hfov;
	float hfovR;
	float zNear, zFar;
	Matrix3x3 pMat;
	//Vector3D n0, n1, n2, n3, f0, f1, f2, f3;
	Vector3D * frustum; //n0, n1, n2, n3, f0, f1, f2, f3
	float * frustumf; //n0, n1, n2, n3, f0, f1, f2, f3


	void SetPMat();

	PPC() : frustum(0), frustumf(0) {};
	PPC(float hfov, int _w, int _h);
	bool Project (Vector3D P, Vector3D &projP);
	void Translate(char dir, float ts);
	void Pan(float rs);
	void Roll(float rs);
	void Tilt(float rs);

	Vector3D GetVD();
	float Getf();
	void zoom(float s, char S);
	void PositionAndOrient(Vector3D newC, Vector3D newVD, Vector3D vinUP, float newf, PPC &ppc);
	PPC Interpolate(PPC *ppc0, PPC *ppc1, float frac);
	void Visualize(PPC * ppc0, FrameBuffer *fb, float visF, int vLN);

	void Save(char *fname);
	void Load(char *fname);
	void Print();
	Vector3D GetRay(float uf, float vf);
	Vector3D GetPoint(Vector3D projP);
	Vector3D GetPoint(float uf, float vf, float z);

	void SetIntrinsicsHW();
	void SetExtrinsicsHW();
	void RenderImageFrameGL();

	void copy(PPC *p);
	void setNearAndFarPoints();
};