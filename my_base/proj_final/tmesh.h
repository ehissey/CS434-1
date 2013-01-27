#pragma once

#include "Vector3D.h"
#include "ppc.h"
#include "framebuffer.h"
#include "aabb.h"
#include "pointlight.h"
#include "envmap.h"

#include "tiff.h"
#include "tiffconf.h"
#include "tiffio.h"
#include "tiffvers.h"

class TMesh{
public:
	Vector3D * verts, * cols, *normals;
	int vertsN;
	unsigned int *tris;
	int trisN;
	bool enabled;
	bool wireframe;
	float kamb, kdiff, kspec;
	bool gouraud;
	bool phong;
	float phongExp;
	bool enableShader;
	float reflectiveSF;
	bool envmapReflection;

	FrameBuffer ** texs;
	int texsN;
	int * triToTexMap;
	bool textured;
	int texID;
	bool projTextured;
	bool texRepetition;
	bool texMirror;
	Vector3D * st;
	float * st_2D;

	TMesh() : envmapReflection(false), reflectiveSF(-1.0f), enableShader(false), wireframe(false), projTextured(false), projTM(0), projPPC(0), texRepetition(false), texMirror(false), 
		st(0), textured(false), triToTexMap(0), texs(0), texsN(0), enabled(true), 
		kamb(0.0f), kdiff(0.0f), kspec(0.0f), gouraud(false), phong(false), phongExp(0), 
		verts(0), vertsN(0), tris(0), trisN(0), cols(0), normals(0) {};
	void SetCube(Vector3D center, float sl);
	void Render(PPC *ppc, FrameBuffer *fb, PointLight *pl, bool wireframe, Envmap *env);
	void RenderPoints(PPC *ppc, FrameBuffer *fb, int pSize);
	void Rotate(Vector3D dir, float theta);
	void Rotate(Vector3D aO, Vector3D dir, float theta);
	Vector3D GetCenter();
	void Load(char *fname);
	void Translate(Vector3D transv);
	void Scale(float s);
	void ScaleAboutCenter(float s);
	AABB GetAABB();
	void SetFromFB(FrameBuffer *fb, PPC *ppc);

	void SetSimpleQuad(Vector3D center, float sl);

	FrameBuffer * openTIFF(string filename);

	void SetDemoTexturedModel(Vector3D center);

	void SetDemoProjTexturedModel(Vector3D center);

	void loadProj8Quad(Vector3D center);

	FrameBuffer *projTM;
	PPC *projPPC;
	
	void RenderHW();
};
