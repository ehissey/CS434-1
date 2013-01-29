#include "scene.h"
#include "depthimage.h"

DepthImage::DepthImage(TMesh * diffuseObject){
	object = diffuseObject;
	
	int u0 = 400;
	int v0 = 400;
	int w = 640;
	int h = 480;

	frame = new FrameBuffer(u0, v0, w, h);
	frame->label("Depth Image");
	frame->isHW = false;
	frame->isRef = false;
	frame->isDI = true;
	//frame->show();

	float hfov = 65.0f;
	camera = new PPC(hfov, w, h);

	Vector3D center = object->GetCenter();

	camera->Translate('f', -center.coords[2]);
	//camera->Translate('u', center.coords[1]);
	camera->Translate('r', -center.coords[0]);
	camera->Pan(90.0f);

	rendered = false;
	depthTexID = 512;
	rgbTexID = 513;
//	frame->redraw();
}

DepthImage::~DepthImage(){
	delete frame;
	delete camera;
}

void DepthImage::renderImage(){
	if(!rendered){
		//object->Render(camera, frame, NULL, false, NULL);

		//frame->redraw();

		//Frame Setup
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set View
		//Set Intrinsics
		camera->SetIntrinsicsHW();
		//Set Extrinsics
		camera->SetExtrinsicsHW();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		object->RenderHW();

		//rendered = true;
	}
}