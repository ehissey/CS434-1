#include <iostream>
#include <cmath>


#include "scene.h"
#include "framebuffer.h"
#include "Vector3D.h"

int pani = 0;

FrameBuffer::FrameBuffer() : Fl_Gl_Window(0,0,0,0,0){
}

FrameBuffer::FrameBuffer(int u0, int v0, int _w, int _h) : Fl_Gl_Window(u0, v0, _w, _h, 0){
	w = _w;
	h = _h;
	pix = new unsigned int[w*h];
	zb = new float[w*h];
	Set(0xFFFFFFFF);
	isHW = false;
	isDI = false;
	isRef = false;

	isLightTransport = false;
	generateLightTransport = false;
	generateCameraImage = false;
	generateLightImage = false;

	mouseCameraControl = false;
	keyboardCameraControl = false;
}

FrameBuffer::~FrameBuffer(){
	delete []pix;
}

void FrameBuffer::SetZB(float z0){
	for(int i = 0; i < w*h; i++){
		zb[i] = z0;
	}
}

void FrameBuffer::draw(){
	if(isHW && !isRef && !isDI && !isLightTransport){
		scene->RenderHW(); //fixed pipeline
		//scene->RenderGPU(); //programmable pipeline
		//glFlush();
		//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, scene->fboId);
		glReadPixels(0,0,w,h,GL_RGBA, GL_UNSIGNED_BYTE, pix);
		//glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glFlush();
		glFinish();
	}else if(isRef && isHW && !isDI){
		//nothing
	}else if(isDI && !isLightTransport){
		scene->RenderDIHW(); //Rendering only the diffuse object of the depth image
		glReadPixels(0,0,w,h,GL_RGBA, GL_UNSIGNED_BYTE, scene->DI->rgb);
		glReadPixels(0,0,w,h,GL_DEPTH_COMPONENT, GL_FLOAT, scene->DI->depths);
		scene->DI->renderImage();
		scene->RenderGPU(); //Done with depth image, render the whole scene now
		glReadPixels(0,0,w,h,GL_RGBA, GL_UNSIGNED_BYTE, pix);
		//isDI = false; //Never render depth image again
	}else if(isLightTransport){
		//nothing for now, will use transport matrix and light vector to derive camera vector (rendered image) instead of calling a regular render function
		int currLight = 0;
		
		if(generateLightTransport){
			for(int v = 0; v < scene->light->h; v++){
				for(int u = 0; u < scene->light->w; u++){
					scene->light->currLightDirection = (scene->light->lppc->c + scene->light->lppc->a * u + scene->light->lppc->a * 0.5f + scene->light->lppc->b * v + scene->light->lppc->b * 0.5f).normalize();  //Light direction relative to lppc "eye"
			
					scene->RenderHW();
					glReadPixels(0,0,w,h,GL_RGBA, GL_UNSIGNED_BYTE, pix);
					glFinish();

					scene->light->insertColumnIntoLightTransportMatrix(currLight, scene->hwFB);

			
					if(u == 0){
						cerr << "INFO: Current light row: " << v << " / " << scene->light->h - 1 << endl;
						//cerr << "currLightDirection: " << scene->light->currLightDirection << endl;
					}

					currLight++;
				}
			}

			cerr << "INFO: Generating subsambled matrix visualizations" << endl;
			scene->light->saveSubsampledLightTransportMatrixAsImage("light_transport/light_transport.tiff");
			scene->light->transposeLightTransportMatrix();
			scene->light->saveSubsampledLightTransportMatrixAsImage("light_transport/light_transport_transpose.tiff");
			scene->light->transposeLightTransportMatrix();

			generateLightTransport = false;
			scene->light->lightTransportMatrixCreated = true;
			generateCameraImage = true;
			generateLightImage = false;
		}

		if(generateCameraImage){
			cerr << "INFO: Begin generation of view from camera" << endl;
			scene->light->applyLightTransportMatrixToLightVector(this);
			//glFinish();
			scene->writeTIFF("light_transport/view_from_camera.tiff", this);
			glDrawPixels(w,h,GL_RGBA, GL_UNSIGNED_BYTE, pix);
			//generateCameraImage = false;
		}else if(generateLightImage){
			cerr << "INFO: Begin generation of view from light" << endl;
			scene->light->applyTransposeLightTransportMatrixToCameraVector(this);
			//glFinish();
			scene->writeTIFF("light_transport/view_from_light.tiff", this);
			glDrawPixels(w,h,GL_RGBA, GL_UNSIGNED_BYTE, pix);
			//generateLightImage = false;
		}
		
	}else{
		glDrawPixels(w,h,GL_RGBA, GL_UNSIGNED_BYTE, pix);
	}
}

void FrameBuffer::Set(unsigned int color){
	for(int i = 0; i < w*h; i++){
		pix[i] = color;
	}
}

void FrameBuffer::Set(int u, int v, unsigned int color){
	pix[w*(h-1-v) + u] = color;
}

unsigned int FrameBuffer::Get(int u, int v){
	return pix[w*(h-1-v) + u];
}

void FrameBuffer::DrawSegment3D(Vector3D p0, Vector3D p1, Vector3D c0, Vector3D c1, PPC *ppc){
	Vector3D pp0, pp1;

	if(!ppc->Project(p0, pp0)){
		return;
	}

	if(!ppc->Project(p1, pp1)){
		return;
	}

	DrawSegment(pp0,pp1,c0,c1);
}

void FrameBuffer::DrawSegment(Vector3D p0, Vector3D p1, Vector3D c0, Vector3D c1){
	int pixN;

	float du = abs(p0[0]-p1[0]);
	float dv = abs(p0[1]-p1[1]);

	if(du < dv){
		pixN = 1 + (int)dv;
	}else{
		pixN = 1 + (int)du;
	}

	Vector3D currP = p0;
	Vector3D dP = (p1-p0)/(float)pixN;
	Vector3D currC = c0;
	Vector3D dC = (c1-c0)/(float)pixN;

	for (int i = 0; i < pixN + 1; i++){
		if(CloserThenSet(currP)){
			SetGuarded(currP, currC.GetColor());
		}
		currP = currP + dP;
		currC = currC + dC;
	}
}

void FrameBuffer::DrawBoldedSegment(Vector3D p0, Vector3D p1, Vector3D c0, Vector3D c1){
	int pixN;

	float du = abs(p0[0]-p1[0]);
	float dv = abs(p0[1]-p1[1]);

	if(du < dv){
		pixN = 1 + (int)dv;
	}else{
		pixN = 1 + (int)du;
	}

	Vector3D currP = p0;
	Vector3D dP = (p1-p0)/(float)pixN;
	Vector3D currC = c0;
	Vector3D dC = (c1-c0)/(float)pixN;

	for (int i = 0; i < pixN + 1; i++){
		if(CloserThenSet(currP)){
			SetGuarded(currP, currC.GetColor());

			SetGuarded(currP + Vector3D(1.0f,0.0f,0.0f), currC.GetColor());
			SetGuarded(currP + Vector3D(-1.0f,0.0f,0.0f), currC.GetColor());
			SetGuarded(currP + Vector3D(0.0f,1.0f,0.0f), currC.GetColor());
			SetGuarded(currP + Vector3D(0.0f,0.0f,0.0f), currC.GetColor());
		}
		currP = currP + dP;
		currC = currC + dC;
	}
}

void FrameBuffer::Set(Vector3D P, unsigned int color){
	int u = (int) P[0];
	int v = (int) P[1];
	Set(u,v,color);
}

void FrameBuffer::SetGuarded(Vector3D P, unsigned int color){
	int u = (int) P[0];
	int v = (int) P[1];

	if (u < 0 || u > w-1 || v < 0 || v > h-1){
		return;
	}
	Set(u,v,color);
}

void FrameBuffer::Brighten(float s){
	for(int v = 0; v < h; v++){
		for(int u = 0; u < w; u++){
			Vector3D col;
			col.SetFromColor(Get(u,v));
			col = col * s;
			Set(u,v, col.GetColor());
		}
	}
}

void FrameBuffer::FindEdges(FrameBuffer *edgeMap){
	Vector3D conv = Vector3D(0.0,0.0,0.0);
	int i = 0;
	for(int v = 1; v < h-1; v++){
		for(int u = 1; u < w-1; u++){
			conv = GetV(u,v) * 4.0 - GetV(u-1, v) - GetV(u+1, v) - GetV(u, v-1) - GetV(u, v+1);
			conv.absolute();

			edgeMap->Set(u,v,conv.GetColor());
		}
	}

	return;
}

Vector3D FrameBuffer::GetV(int u, int v){
	Vector3D retval = Vector3D();

	retval.SetFromColor(Get(u,v));

	return retval;
}

int FrameBuffer::handle(int event){
	switch(event){
	case FL_KEYBOARD:
		KeyboardHandle();
		return 0;
	case FL_MOUSEWHEEL:
		MouseWheelHandle();
		return 0;
	case FL_PUSH:
		mouseX = Fl::event_x();
		mouseY = Fl::event_y();
		return 1;
	case FL_DRAG:
		MouseDragHandle();
		return 0;
	default:
		break;
	}

	return 0;
}

void DebugBreak(){
	return;
}

void FrameBuffer::MouseDragHandle(){
	if(mouseCameraControl){
		float rs = 0.1f;
		int mouse_dx = Fl::event_x() - mouseX;
		int mouse_dy = Fl::event_y() - mouseY;
		mouseX = Fl::event_x();
		mouseY = Fl::event_y();
		//cout << "dx: " << mouse_dx << "\tdy: " << mouse_dy << endl;
		if(mouse_dx != 0){
			scene->ppc->Pan(rs*mouse_dx);
		}
		if(mouse_dy != 0){
			scene->ppc->Roll(rs*mouse_dy);
		}
		scene->Render();
	}
}

void FrameBuffer::MouseWheelHandle(){
	if(mouseCameraControl){
		int dy = Fl::event_dy();
		float ts = 6.0f;

		if(dy < 0){
			scene->ppc->Translate('f', ts);
		}else{
			scene->ppc->Translate('b', ts);
		}

		scene->Render();
	}
}

void FrameBuffer::KeyboardHandle(){
	if(keyboardCameraControl){
		float ts = 6.0;
		float rs = 1.0;
		Vector3D p0,p1;
		int key = Fl::event_key();

		switch(key){
		case 'w':
			scene->ppc->Translate('u', ts);
			break;
		case 'a':
			scene->ppc->Translate('l', ts);
			break;
		case 's':
			scene->ppc->Translate('d', ts);
			break;
		case 'd':
			scene->ppc->Translate('r', ts);
			break;
		case 'r':
			scene->ppc->zoom(2.0f,'i');
			break;
		case 'f':
			scene->ppc->zoom(2.0f, 'o');
			break;
		case FL_Left:
			scene->ppc->Pan(rs);
			break;
		case FL_Right:
			scene->ppc->Pan(-rs);
			break;
		case FL_Up:
			scene->ppc->Roll(rs);
			break;
		case FL_Down:
			scene->ppc->Roll(-rs);
			break;
		case 'e':
			scene->ppc->Tilt(rs);
			break;
		case 'q':
			scene->ppc->Tilt(-rs);
			break;

		default:
			cerr << "Invalid key" << endl;
			break;
		}

		scene->Render();
	}
}

bool FrameBuffer::CloserThenSet(Vector3D P){
	int u = (int) P[0];
	int v = (int) P[1];

	int uv = (h-1-v)*w + u;

	if (u < 0 || u > w-1 || v < 0 || v > h-1){
		return false;
	}

	if(zb[uv] > P[2]){
		return false;
	}

	zb[uv] = P[2];
	return true;
}

void FrameBuffer::CopyFrom(FrameBuffer *fb) {
	for (int uv = 0; uv < w*h; uv++) {
		zb[uv] = fb->zb[uv];
		pix[uv] = fb->pix[uv];
	}
}

unsigned int * FrameBuffer::convertPixToGLFormat(){
	unsigned int * retval = new unsigned int[w*h];

	int i = 0;

	for(int v = 0; v < h; v++){
		for(int u = 0; u < w; u++){
			retval[i] = Get(u,v);
			i++;
		}
	}

	return retval;
}