#include "scene.h"
#include "depthimage.h"

DepthImage::DepthImage(FrameBuffer * frame, PPC * ppc, TMesh * diffuseObject, int rID, int dID){
	this->diffuseObject = diffuseObject;
	camera = new PPC();
	camera->copy(ppc);
	depthTexID = dID;
	rgbTexID = rID;

	u0 = 1000;
	v0 = 500;
	w = 640;
	h = 480;

	rgb = new unsigned int[w*h];
	depths = new float[w*h];
	
	rendered = false;
}

DepthImage::~DepthImage(){
	delete rgb;
	delete depths;
	delete camera;
}

void DepthImage::renderImage(){
	if(!rendered){
		glReadPixels(0,0,w,h,GL_RGBA, GL_UNSIGNED_BYTE, rgb);
		glReadPixels(0,0,w,h,GL_DEPTH_COMPONENT, GL_FLOAT, depths);

		glBindTexture(GL_TEXTURE_2D, depthTexID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameterf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		//unsigned int * tempPix = convertPixToGLFormat();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depths);

		glBindTexture(GL_TEXTURE_2D, rgbTexID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		glTexParameterf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgb);

		rendered = true;
	}
}