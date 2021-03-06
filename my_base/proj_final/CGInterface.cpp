#include "Vector3D.h"
#include "scene.h"
#include "CGInterface.h"

#include <iostream>

//#define GEOMETRY_SUPPORT

using namespace std;

CGInterface::CGInterface() : needInit(true) {};

void CGInterface::PerSessionInit(){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	#ifdef GEOMETRY_SUPPORT
		CGprofile latestGeometryProfile = cgGLGetLatestProfile(CG_GL_GEOMETRY);
		if(latestGeometryProfile == CG_PROFILE_UNKNOWN){
			cerr << "ERROR: geometry profile is not available" << endl;
			system("pause");
			exit(0);
		}
		cgGLSetOptimalOptions(latestGeometryProfile);
		CGerror Error = cgGetError();
		if(Error){
			cerr << "CG ERROR: " << cgGetErrorString(Error) << endl;
		}

		cout << "Info: Latest GP Profile Supported: " << cgGetProfileString(latestGeometryProfile) << endl;

		geometryCGprofile = latestGeometryProfile;
	#endif

	CGprofile latestVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
	CGprofile latestPixelProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);

	cout << "Info: Latest VP Profile Supported: " << cgGetProfileString(latestPixelProfile) << endl;
	cout << "Info: Latest FP Profile Supported: " << cgGetProfileString(latestPixelProfile) << endl;

	vertexCGprofile = latestVertexProfile;
	pixelCGprofile = latestPixelProfile;
	cgContext = cgCreateContext();

	needInit = false;

}

void CGInterface::DisableProfiles(){	
	cgGLDisableProfile(vertexCGprofile);
	#ifdef GEOMETRY_SUPPORT
		cgGLDisableProfile(geometryCGprofile);
	#endif
	cgGLDisableProfile(pixelCGprofile);
}

void CGInterface::EnableProfiles(){
	cgGLEnableProfile(vertexCGprofile);
	#ifdef GEOMETRY_SUPPORT
		cgGLEnableProfile(geometryCGprofile);
	#endif
	cgGLEnableProfile(pixelCGprofile);
}

bool ShaderOneInterface::PerSessionInit(CGInterface *cgi){
	
	#ifdef GEOMETRY_SUPPORT
		geometryProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, "CG/shaderOne.cg", cgi->geometryCGprofile, "GeometryMain", NULL);
		if(geometryProgram == NULL) {
			CGerror Error = cgGetError();
			cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
			cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
			return false;
		}
	#endif

	vertexProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, "CG/shaderOne.cg", cgi->vertexCGprofile, "VertexMain", NULL);
	if(vertexProgram == NULL){
		CGerror Error = cgGetError();
		cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
		cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
		return false;
	}

	pixelProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, "CG/shaderOne.cg", cgi->pixelCGprofile, "FragmentMain", NULL);
	if(pixelProgram == NULL) {
		CGerror Error = cgGetError();
		cerr << "Shader One Fragment Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
		cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
		return false;
	}

	// load programs
	#ifdef GEOMETRY_SUPPORT
		cgGLLoadProgram(geometryProgram);
	#endif
	cgGLLoadProgram(vertexProgram);
	cgGLLoadProgram(pixelProgram);

	// build some parameters by name such that we can set them later...
	vertexModelViewProj = cgGetNamedParameter(vertexProgram, "modelViewProj");
	#ifdef GEOMETRY_SUPPORT
		geometryModelViewProj = cgGetNamedParameter(geometryProgram, "modelViewProj");
	#endif

	return true;
}

void ShaderOneInterface::PerFrameInit(){
	//set parameters
	cgGLSetStateMatrixParameter(vertexModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	#ifdef GEOMETRY_SUPPORT
		cgGLSetStateMatrixParameter(geometryModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	#endif
}

void ShaderOneInterface::PerFrameDisable(){
}

void ShaderOneInterface::BindPrograms(){
	#ifdef GEOMETRY_SUPPORT
		cgGLBindProgram(geometryProgram);
	#endif
	cgGLBindProgram(vertexProgram);
	cgGLBindProgram(pixelProgram);
}




bool BgEnvMapShaderInterface::PerSessionInit(CGInterface *cgi){
	
	#ifdef GEOMETRY_SUPPORT
		geometryProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, "CG/BgEnvMapShader.cg", cgi->geometryCGprofile, "GeometryMain", NULL);
		if(geometryProgram == NULL) {
			CGerror Error = cgGetError();
			cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
			cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
			return false;
		}
	#endif

	vertexProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, "CG/BgEnvMapShader.cg", cgi->vertexCGprofile, "VertexMain", NULL);
	if(vertexProgram == NULL){
		CGerror Error = cgGetError();
		cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
		cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
		return false;
	}

	pixelProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, "CG/BgEnvMapShader.cg", cgi->pixelCGprofile, "FragmentMain", NULL);
	if(pixelProgram == NULL) {
		CGerror Error = cgGetError();
		cerr << "Shader One Fragment Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
		cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
		return false;
	}

	// load programs
	#ifdef GEOMETRY_SUPPORT
		cgGLLoadProgram(geometryProgram);
	#endif
	cgGLLoadProgram(vertexProgram);
	cgGLLoadProgram(pixelProgram);

	// build some parameters by name such that we can set them later...
	vertexModelViewProj = cgGetNamedParameter(vertexProgram, "modelViewProj");
	#ifdef GEOMETRY_SUPPORT
		geometryModelViewProj = cgGetNamedParameter(geometryProgram, "modelViewProj");
	#endif

	pixelCameraEye = cgGetNamedParameter(pixelProgram, "cameraEye");
	pixelCubeMap = cgGetNamedParameter(pixelProgram, "envMap" );

	return true;
}

void BgEnvMapShaderInterface::PerFrameInit(){
	//set parameters
	cgGLSetStateMatrixParameter(vertexModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	#ifdef GEOMETRY_SUPPORT
		cgGLSetStateMatrixParameter(geometryModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	#endif
	
	cgGLSetParameter3fv(pixelCameraEye, (float*)&(scene->ppc->C));
	cgGLSetTextureParameter(pixelCubeMap, scene->env->texID);
    cgGLEnableTextureParameter(pixelCubeMap);
}

void BgEnvMapShaderInterface::PerFrameDisable(){
}

void BgEnvMapShaderInterface::BindPrograms(){
	#ifdef GEOMETRY_SUPPORT
		cgGLBindProgram(geometryProgram);
	#endif
	cgGLBindProgram(vertexProgram);
	cgGLBindProgram(pixelProgram);
}


bool diffuseBunnyShaderInterface::PerSessionInit(CGInterface *cgi){
	
	#ifdef GEOMETRY_SUPPORT
		geometryProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, "CG/diffuseBunnyShader.cg", cgi->geometryCGprofile, "GeometryMain", NULL);
		if(geometryProgram == NULL) {
			CGerror Error = cgGetError();
			cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
			cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
			return false;
		}
	#endif

	vertexProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, "CG/diffuseBunnyShader.cg", cgi->vertexCGprofile, "VertexMain", NULL);
	if(vertexProgram == NULL){
		CGerror Error = cgGetError();
		cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
		cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
		return false;
	}

	pixelProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE, "CG/diffuseBunnyShader.cg", cgi->pixelCGprofile, "FragmentMain", NULL);
	if(pixelProgram == NULL) {
		CGerror Error = cgGetError();
		cerr << "Shader One Fragment Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
		cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
		return false;
	}

	// load programs
	#ifdef GEOMETRY_SUPPORT
		cgGLLoadProgram(geometryProgram);
	#endif
	cgGLLoadProgram(vertexProgram);
	cgGLLoadProgram(pixelProgram);

	// build some parameters by name such that we can set them later...
	vertexModelViewProj = cgGetNamedParameter(vertexProgram, "modelViewProj");
	#ifdef GEOMETRY_SUPPORT
		geometryModelViewProj = cgGetNamedParameter(geometryProgram, "modelViewProj");
	#endif

	pixelLightDir = cgGetNamedParameter(pixelProgram, "lightDir" );

	return true;
}

void diffuseBunnyShaderInterface::PerFrameInit(){
	//set parameters
	cgGLSetStateMatrixParameter(vertexModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	#ifdef GEOMETRY_SUPPORT
		cgGLSetStateMatrixParameter(geometryModelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	#endif

	cgGLSetParameter3fv(pixelLightDir, (float*)&(scene->lightDir));
}

void diffuseBunnyShaderInterface::PerFrameDisable(){
}

void diffuseBunnyShaderInterface::BindPrograms(){
	#ifdef GEOMETRY_SUPPORT
		cgGLBindProgram(geometryProgram);
	#endif
	cgGLBindProgram(vertexProgram);
	cgGLBindProgram(pixelProgram);
}
