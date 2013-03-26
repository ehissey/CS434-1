#pragma once
#pragma comment(lib, "cg.lib")
#pragma comment(lib, "cggl.lib")

#include <Cg/cgGL.h>
#include <Cg/cg.h>

class CGInterface{
public:
	CGprofile vertexCGprofile;
	CGprofile geometryCGprofile;
	CGprofile pixelCGprofile;
	CGcontext cgContext;

	bool needInit;
	void PerSessionInit();
	CGInterface();
	void EnableProfiles();
	void DisableProfiles();
};

class ShaderOneInterface{
	CGprogram geometryProgram;
	CGprogram vertexProgram;
	CGprogram pixelProgram;

	CGparameter vertexModelViewProj;
	CGparameter geometryModelViewProj;
	
public:
	ShaderOneInterface() {};
	bool PerSessionInit(CGInterface *cgi);
	void BindPrograms();
	void PerFrameInit();
	void PerFrameDisable();
};

class BgEnvMapShaderInterface{
	CGprogram geometryProgram;
	CGprogram vertexProgram;
	CGprogram pixelProgram;

	CGparameter vertexModelViewProj;
	CGparameter geometryModelViewProj;
	CGparameter pixelCameraEye;
	CGparameter pixelCubeMap;

public:
	BgEnvMapShaderInterface() {};
	bool PerSessionInit(CGInterface *cgi);
	void BindPrograms();
	void PerFrameInit();
	void PerFrameDisable();
};

class diffuseBunnyShaderInterface{
	CGprogram geometryProgram;
	CGprogram vertexProgram;
	CGprogram pixelProgram;

	CGparameter vertexModelViewProj;
	CGparameter geometryModelViewProj;
	CGparameter pixelLightDir;

public:
	diffuseBunnyShaderInterface() {};
	bool PerSessionInit(CGInterface *cgi);
	void BindPrograms();
	void PerFrameInit();
	void PerFrameDisable();
};