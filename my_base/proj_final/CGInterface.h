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
	CGparameter pixelCameraEye;
	CGparameter pixelCubeMap;
	CGparameter pixelQuadV0;
	CGparameter pixelQuadV1;
	CGparameter pixelQuadV2;
	CGparameter pixelQuadV3;
	CGparameter pixelQuadTex;
	CGparameter pixelQuadTexCoordsMultiplier;
	CGparameter pixelDepthImageZ;
	CGparameter pixelDepthImageRGB;
	//CGparameter pixelDepthImageFrustumPoints;
	CGparameter pixelDIn0;
	CGparameter pixelDIn1;
	CGparameter pixelDIn2;
	CGparameter pixelDIn3;
	CGparameter pixelDIf0;
	CGparameter pixelDIf1;
	CGparameter pixelDIf2;
	CGparameter pixelDIf3;
	
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
