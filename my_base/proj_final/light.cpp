#include "scene.h"
#include "light.h"


Light::Light(int width, int height, float hfov, Vector3D eye, Vector3D lookAt, PPC * cppc){
	enabled = true;

	lppc = new PPC(hfov, width, height);  //Set up directional light grid as a ppc

	lppc->PositionAndOrient(eye, lookAt - eye, Vector3D(0.0f, 1.0f, 0.0f), lppc->Getf(), *lppc);  //Look at the object

	lightVector = new float[width*height];

	//int v = cppc->w * cppc->h * width * height;
	//int v = 128 * 128 * width * height;


	//lightTransportMatrix = new float[v - 1];
	lightFrameBuffer = new FrameBuffer(0.0f, 0.0f, width, height);

	w = lppc->w;
	h = lppc->h;

	setLightVector(1.0f);
}

Light::~Light(){
	delete lppc;
	delete lightVector;
}

bool Light::setLightVector(float value){
	if(value > 1.0f){
		value = 1.0f;
	}else if(value < 0.0f){
		value = 0.0f;
	}

	for(int v = 0; v < h; v++){
		for(int u = 0; u < w; u++){
			if(v < h / 2.0f){
				lightVector[v*w + u] = 1.0f - value;  //temporary, trying to orient light source
			}else{
				lightVector[v*w + u] = value;  //vth row + u offset in that row
			}
		}
	}


	saveLightVectorAsImage();  //temporary
	return true;
}

bool Light::saveLightVectorAsImage(){
	for(int v = 0; v < h; v++){
		for(int u = 0; u < w; u++){
			lightFrameBuffer->pix[(h - 1 -v)*w + u] = Vector3D(lightVector[v*w + u], lightVector[v*w + u], lightVector[v*w + u]).GetColor();
		}
	}

	scene->writeTIFF("LIGHT.TIFF", lightFrameBuffer);

	return true;
}