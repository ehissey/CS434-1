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
	CGparameter geometrySF;

public:
	ShaderOneInterface() {};
	bool PerSessionInit(CGInterface *cgi);
	void BindPrograms();
	void PerFrameInit();
	void PerFrameDisable();
};
