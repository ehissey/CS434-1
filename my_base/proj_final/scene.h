#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <GL/glew.h>

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "tmesh.h"
#include "pointlight.h"
#include "envmap.h"
#include "depthimage.h"
#include "CGInterface.h"
#include "light.h"

#include <string>
#include <list>
#include <map>

class Scene{
public:
	// gpu programming
	CGInterface *cgi;
	ShaderOneInterface *soi;
	BgEnvMapShaderInterface *bemsi;
	diffuseBunnyShaderInterface *dbsi;

	GUI *gui;
	PPC *ppc, *refPPC;
	list<TMesh> TMList;
	map<string,int> texNameToIDMap;
	TMesh *currObject;
	TMesh *currGuiObject;
	TMesh *quadHandle;
	TMesh *diffuseObjectHandle;
	TMesh *reflectiveObjectHandle;
	TMesh *currObjectHandle;
	FrameBuffer *fb, *refFB, *hwFB;
	PointLight *pl;
	Envmap *env;
	DepthImage *DI;
	Light * light;
	bool wireframe;
	bool initializedHW;
	bool initializedGPU;

	Scene();
	void DBG();
	void Render(); //SW & HW
	void RenderHW(); //Fixed pipeline
	void RenderGPU(); //Programmable pipeline
	void RenderDIHW(); //Fixed pipeline
	void RenderDIGPU(); //Programmable pipeline
	void RenderRefHW(); //Fixed pipeline
	void RenderRefGPU(); //Programmable pipeline
	void FrameSetup();
	void InitializeHW();
	void InitializeHWObjects();
	void InitializeSWObjects();

	void writeCurrFrame(int currFrame, FrameBuffer *frame);
	FrameBuffer * openTIFF_FB(string filename);
	void writeTIFF(string filename, FrameBuffer *frame);

	void quit();

	void SaveView0();
	void LoadView0();
	void GoToView0();
	void SaveView1();
	void LoadView1();
	void GoToView1();
	void GoToView(PPC *nppc);

	void setGouraud();
	void setPhong();
	void lightLeft();
	void lightRight();
	void lightUp();
	void lightDown();
	void lightForward();
	void lightBackward();
	void setAmbient(const char * input);
	void setExponent(const char * input);

	void RefGoToView0();
	void RefGoToView1();
	void RefSaveView0();
	void RefLoadView0();
	void RefSaveView1();
	void RefLoadView1();
	void RefGoToView(PPC *nppc);

	void captureLightTransportMatrix();

	GLuint fboId;
	GLuint rboId;
};

extern Scene *scene;