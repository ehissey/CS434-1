#pragma once

#include "Vector3D.h"
#include "ppc.h"
#include "framebuffer.h"

#include <string>

using namespace std;

class Light{
public:
	PPC * lppc;
	bool enabled;
	bool grayscale;
	bool cameraVectorRendered;
	bool lightTransportMatrixCreated;
	bool lightTransportMatrixIsTransposed;
	float * lightVector;  //Values for the grid lights, ordered by rows.
	float * cameraVector;  //Values for the camera values, ordered by rows.
	float * cameraVectorR;  //Values for the camera values, ordered by rows.
	float * cameraVectorG;  //Values for the camera values, ordered by rows.
	float * cameraVectorB;  //Values for the camera values, ordered by rows.
	//float ** lightTransportMatrix;  //Array of column vectors
	unsigned char *** lightTransportMatrix;

	FrameBuffer * lightFrameBuffer;
	int w;
	int h;
	Vector3D currLightDirection;

	Light(int width, int height, float hfov, Vector3D eye, Vector3D lookAt, PPC * cppc);
	~Light();

	bool loadLightVector(string filename);  //Load the light vector from a file
	bool setLightVector(float value);  //Set all values of the light vector
	bool setLightVectorCheckered(int checkerSize);
	bool reverseLightVector();  //Reverse all values of the light vector
	bool saveLightVectorAsImage();
	//bool captureLightTransportMatrix(FrameBuffer * fb);
	bool insertColumnIntoLightTransportMatrix(int col, FrameBuffer * fb);
	bool saveSubsampledLightTransportMatrixAsImage(string filename);
	bool applyLightTransportMatrixToLightVector(FrameBuffer *fb);
	bool applyTransposeLightTransportMatrixToCameraVector(FrameBuffer *fb);
	bool transposeLightTransportMatrix();

	unsigned char accessLightTransportMatrix(int matrixID, int col, int row);
	void writeToLightTransportMatrix(int matrixID, int col, int row, unsigned char value);
};