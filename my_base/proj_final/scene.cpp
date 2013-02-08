#include "scene.h"
#include "Matrix3x3.h"
#include "tiff.h"
#include "tiffconf.h"
#include "tiffio.h"
#include "tiffvers.h"
#include "ppc.h"
#include "Vector3D.h"
#include "aabb.h"


#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

Scene *scene;

Scene::Scene(){
	wireframe = false;
	initializedHW = false;
	initializedGPU = false;

	int u0 = 20;
	int v0 = 50;
	int sci = 2;
	//int w = sci * 320;
	//int h = sci * 240;
	int w = 1280;
	int h = 720;

	gui = new GUI();
	gui->show();
	gui->uiw->position(w + u0 + 30, v0);

	hwFB = new FrameBuffer(u0, v0, w, h);
	hwFB->label("HW Framebuffer");
	hwFB->isHW = true;
	hwFB->isRef = false;
	//hwFB->show();
	cgi = new CGInterface();
	soi = new ShaderOneInterface();
	bemsi = new BgEnvMapShaderInterface();
	dbsi = new diffuseBunnyShaderInterface();
	//hwFB = 0;

	float hfov = 45.0f;
	ppc = new PPC(hfov, w, h);
	
	InitializeHWObjects();

	//DI = 0;
	DI = new DepthImage(hwFB, ppc, diffuseObjectHandle, 513, 512);

	hwFB->isDI = true;
	hwFB->show();

	refPPC = 0;
	refFB = 0;

	

	Render();

	cout << "here" << endl;
}

void Scene::Render(){
	for(list<TMesh>::iterator i = TMList.begin(); i != TMList.end(); ++i){
		i->wireframe = wireframe || i->wireframe;
	}
	
	//cout << ppc->GetVD() * ppc->Getf() << endl;
	//cout << ppc->c << endl;
	//cout << (ppc->GetVD() * ppc->Getf()).length() << endl;

	if(hwFB){
		hwFB->redraw();
	}
	return;
}

void Scene::InitializeHW(){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//Load Textures
	FrameBuffer ** texs;
	int texsN;

	texsN = 9;

	texs = new FrameBuffer*[texsN];

	texs[0] = openTIFF_FB("textures/sub_front.tiff");
	texNameToIDMap["sub_front"] = 0;
	texs[1] = openTIFF_FB("textures/sub_right.tiff");
	texNameToIDMap["sub_right"] = 1;
	texs[2] = openTIFF_FB("textures/sub_back.tiff");
	texNameToIDMap["sub_back"] = 2;
	texs[3] = openTIFF_FB("textures/sub_left.tiff");
	texNameToIDMap["sub_left"] = 3;
	texs[4] = openTIFF_FB("textures/sub_top.tiff");
	texNameToIDMap["sub_top"] = 4;
	texs[5] = openTIFF_FB("textures/brick50.tiff");
	texNameToIDMap["brick50"] = 5;
	texs[6] = openTIFF_FB("textures/brick128.tiff");
	texNameToIDMap["brick128"] = 6;
	texs[7] = openTIFF_FB("textures/2x2checker.tiff");
	texNameToIDMap["2x2checker"] = 7;
	texs[8] = openTIFF_FB("textures/4x4checker.tiff");
	texNameToIDMap["4x4checker"] = 8;

	for(int i = 0; i < texsN; i++){
		glBindTexture(GL_TEXTURE_2D, i);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		unsigned int * tempPix = texs[i]->convertPixToGLFormat();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texs[i]->w, texs[i]->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tempPix);

		delete tempPix;
	}

	for(int i = 0; i < texsN; i++){
		delete texs[i];
	}

	delete texs;
}

void Scene::InitializeHWObjects(){
		Vector3D center = Vector3D(0.0f,0.0f,-170.0f);
		float sl = 256.0;

		currObject = new TMesh();
		currObject->Load("geometry/bunny.bin");

		AABB aabb = currObject->GetAABB();
		float size0 = (aabb.corners[1]-aabb.corners[0]).length();
		Vector3D tcenter = currObject->GetCenter();
		currObject->Translate(tcenter*-1.0f+center);
		float size1 = 90.0f;
		currObject->ScaleAboutCenter(size1/size0);
		currObject->kamb = 0.20f;
		currObject->gouraud = false;
		currObject->phong = false;
		currObject->phongExp = 40.0f;
		currObject->enableShader = true;
		currObject->shaderSelection = 1;
		currObject->sphereMorphRaidus = 20.0f;
		currObject->sphereMorphScaleFactor = 0.0f;
		currObject->sphereMorphDirection = 1.0f;
		currObject->center = center;
		//currObject->reflectiveSF = 0.5f;
		TMList.push_back(*currObject);
		//currGuiObject = currObject;
		reflectiveObjectHandle = currObject;

		center = Vector3D(20.0f,0.0f,-150.0f);

		currObject = new TMesh();
		currObject->Load("geometry/bunny.bin");

		aabb = currObject->GetAABB();
		size0 = (aabb.corners[1]-aabb.corners[0]).length();
		tcenter = currObject->GetCenter();
		currObject->Translate(tcenter*-1.0f+center);
		size1 = 90.0f;
		currObject->ScaleAboutCenter(size1/size0);
		currObject->kamb = 0.20f;
		currObject->gouraud = false;
		currObject->phong = false;
		currObject->phongExp = 40.0f;
		currObject->enableShader = true;
		currObject->shaderSelection = 2;
		currObject->sphereMorphRaidus = 20.0f;
		currObject->sphereMorphScaleFactor = 0.0f;
		currObject->sphereMorphDirection = 1.0f;
		currObject->center = center;
		//currObject->reflectiveSF = 0.5f;
		TMList.push_back(*currObject);
		currGuiObject = currObject;
		diffuseObjectHandle = currObject;

		center = Vector3D(0.0f,0.0f,-170.0f);

		currObject = new TMesh();
		currObject->setTexturedQuad(center);
		currObject->enableShader = false;
		TMList.push_back(*currObject);
		quadHandle = currObject;
		//delete currObject;

		//center = Vector3D(60.0f,0.0f,-170.0f);
		//SetGLDemoTexturedModel(center);
}

void Scene::RenderHW(){
	//OpenGL Setup
	if(!initializedHW){
		env = 0;
		InitializeHW();
		InitializeHWObjects();
		initializedHW = true;
	}
	
	//Frame Setup
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set View
	//Set Intrinsics
	ppc->SetIntrinsicsHW();
	//Set Extrinsics
	ppc->SetExtrinsicsHW();

	for(list<TMesh>::iterator i = TMList.begin(); i != TMList.end(); ++i){
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		i->RenderHW();
	}
}

void Scene::RenderDIHW(){
	//OpenGL Setup
	if(!initializedHW){
		//env = 0;
		env = new Envmap();
		env->texID = 500;
		env->LoadHW();

		InitializeHW();
		initializedHW = true;
	}

	if(!initializedGPU){
		cgi->PerSessionInit();
		soi->PerSessionInit(cgi);
		bemsi->PerSessionInit(cgi);
		dbsi->PerSessionInit(cgi);
		initializedGPU = true;
	}
	
	//Frame Setup
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Vector3D center = diffuseObjectHandle->GetCenter();

	if(!DI->cameraSet){
		//DI->camera->copy(ppc);
		DI->camera = new PPC(150.0f, ppc->w, ppc->h);
		DI->camera->zFar = 100.0f;
		DI->camera->zNear = 1.0f;
		/*DI->camera->Translate('f', 170);
		DI->camera->Translate('r', 40);
		DI->camera->Pan(90.0f);*/

		Vector3D center1 = Vector3D(0.0f,0.0f,-170.0f);
		Vector3D center2 = Vector3D(20.0f,0.0f,-150.0f);

		DI->camera->PositionAndOrient(center1 - (center2 - center1).normalize() * 15, (center2 - center1).normalize(), Vector3D(0.0f, 1.0f, 0.0f), DI->camera->Getf(), *(DI->camera));
		
		DI->camera->setNearAndFarPoints();

		TMesh * obj = new TMesh();
		obj->drawCameraFrustum(DI->camera);
		obj->wireframe = true;
		//obj->enabled = false;
		//TMList.push_back(*obj);

		for(int i = 0; i < 8; i++){
			cout << i << ":\t" << DI->camera->frustum[i] << endl;
		}

		DI->cameraSet = true;
	}
	
	//Set View
	//Set Intrinsics
	DI->camera->SetIntrinsicsHW();
	//Set Extrinsics
	DI->camera->SetExtrinsicsHW();

	dbsi->PerFrameInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	dbsi->BindPrograms();
	cgi->EnableProfiles();
	diffuseObjectHandle->RenderHW();
	cgi->DisableProfiles();
	dbsi->PerFrameDisable();
	
}

void Scene::RenderGPU(){
	if(!initializedHW){
		env = new Envmap();
		env->texID = 500;
		env->LoadHW();
		//env = 0;

		InitializeHW();
		initializedHW = true;
	}
	
	if(!initializedGPU){
		cgi->PerSessionInit();
		soi->PerSessionInit(cgi);
		bemsi->PerSessionInit(cgi);
		dbsi->PerSessionInit(cgi);
		initializedGPU = true;
	}

	//Frame Setup
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set View
	//Set Intrinsics
	ppc->SetIntrinsicsHW();
	//Set Extrinsics
	ppc->SetExtrinsicsHW();
	
	

	soi->PerFrameInit();
	bemsi->PerFrameInit();
	

	if(env){
		bemsi->BindPrograms();
		cgi->EnableProfiles();
		ppc->RenderImageFrameGL();
		cgi->DisableProfiles();
	}

	for(list<TMesh>::iterator i = TMList.begin(); i != TMList.end(); ++i){
		glEnable(GL_DEPTH_TEST);
		
		if(i->cullFace){
			glEnable(GL_CULL_FACE);
		}else{
			glDisable(GL_CULL_FACE);
		}

		if(i->enableShader){
			if(i->shaderSelection == 1){
				soi->BindPrograms();
				
			}else if(i->shaderSelection == 2){
				
				dbsi->PerFrameInit();
				dbsi->BindPrograms();
			}
			cgi->EnableProfiles();
			i->RenderHW();
			cgi->DisableProfiles();
		}else{
			i->RenderHW();
		}
	}

	soi->PerFrameDisable();
	bemsi->PerFrameDisable();
	dbsi->PerFrameDisable();
}

void Scene::RenderRefGPU(){
	
}

FrameBuffer * Scene::openTIFF_FB(string filename){
	TIFF *FILE;

	if((FILE = TIFFOpen(filename.c_str(), "r")) == 0){
		return NULL;
	}

	FrameBuffer * fb;

	int u0 = 50;
	int v0 = 50;
	int w = 0;
	int h = 0;

	TIFFGetField(FILE, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField(FILE, TIFFTAG_IMAGELENGTH, &h);

	unsigned int npix = w*h;
	unsigned int *raster =(unsigned int *) _TIFFmalloc(npix *sizeof(unsigned int));

	fb = new FrameBuffer(u0,v0,w,h);

	int retval = TIFFReadRGBAImage(FILE, w, h, raster);

	int i = 0;

	for(int u = 0; u < w; u++){
		for(int v = 0; v < h; v++){
			fb->Set(u,v,raster[w*(h-1-v) + u]);
			i++;
		}
	}

	TIFFClose(FILE);

	_TIFFfree(raster);

	return fb;
}

void Scene::writeTIFF(string filename, FrameBuffer *frame){
	TIFF *FILE;
	
	if((FILE = TIFFOpen(filename.c_str(), "w")) == 0){
		return;
	}

	int sampleperpixel = 4;

	unsigned char *image = (unsigned char*) frame->pix;

	TIFFSetField(FILE, TIFFTAG_IMAGEWIDTH, frame->w);
	TIFFSetField(FILE, TIFFTAG_IMAGELENGTH, frame->h);
	TIFFSetField(FILE, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);
	TIFFSetField(FILE, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(FILE, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(FILE, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(FILE, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	tsize_t line = sampleperpixel * frame->w; 

	unsigned char *buffer = NULL;

	if(TIFFScanlineSize(FILE) == line){
		buffer = (unsigned char*) _TIFFmalloc(line);
	}else{
		buffer = (unsigned char*) _TIFFmalloc(TIFFScanlineSize(FILE));
	}

	TIFFSetField(FILE, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(FILE, frame->w*sampleperpixel));

	for (unsigned int row = 0; row < (unsigned int)frame->h; row++)
	{
		memcpy(buffer, &image[(frame->h-row-1)*line], line);
		if (TIFFWriteScanline(FILE, buffer, row, 0) < 0){
			break;
		}
	}

	TIFFClose(FILE);

	if(buffer){
		_TIFFfree(buffer);
	}
}

void Scene::quit(){
	exit(0);
}

void Scene::SaveView0(){
	ppc->Save("mydbg/view0.txt");
}

void Scene::LoadView0(){
	ppc->Load("mydbg/view0.txt");
	Render();
}

void Scene::GoToView0(){
	PPC nppc;
	nppc.Load("mydbg/view0.txt");
	GoToView(&nppc);
}

void Scene::SaveView1(){
	ppc->Save("mydbg/view1.txt");
}

void Scene::LoadView1(){
	ppc->Load("mydbg/view1.txt");
	Render();
}

void Scene::GoToView1(){
	PPC nppc;
	nppc.Load("mydbg/view1.txt");
	GoToView(&nppc);
}


void Scene::GoToView(PPC *nppc){
	PPC oppc(*ppc);
	int n = 150;
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc, nppc, frac);
		Render();
		Fl::check();
	}
}

void Scene::setGouraud(){
	if(pl->enabled){
		currObject->gouraud = true;
		currObject->phong = false;

		Render();
	}
}

void Scene::setPhong(){
	if(pl->enabled){
		currObject->gouraud = false;
		currObject->phong = true;

		Render();
	}
}

void Scene::lightLeft(){
	pl->pos = pl->pos - Vector3D(2.0f, 0.0f, 0.0f);
	Render();
}

void Scene::lightRight(){
	pl->pos = pl->pos + Vector3D(2.0f, 0.0f, 0.0f);
	Render();
}

void Scene::lightUp(){
	pl->pos = pl->pos + Vector3D(0.0f, 2.0f, 0.0f);
	Render();
}

void Scene::lightDown(){
	pl->pos = pl->pos - Vector3D(0.0f, 2.0f, 0.0f);
	Render();
}

void Scene::lightForward(){
	pl->pos = pl->pos - Vector3D(0.0f, 0.0f, 2.0f);
	Render();
}

void Scene::lightBackward(){
	pl->pos = pl->pos + Vector3D(0.0f, 0.0f, 2.0f);
	Render();
}

void Scene::setAmbient(const char * input){
	if(strlen(input) == 0){
		cerr << "Error: Must specify value" << endl;
		return;
	}

	float value = atof(input);

	if (value < 0.0f || value > 1.0f){
		cerr << "Error: Value must be between 0.0 and 1.0" << endl;
		return;
	}

	currObject->kamb = value;

	Render();
}

void Scene::setExponent(const char * input){
	if(strlen(input) == 0){
		cerr << "Error: Must specify value" << endl;
		return;
	}

	currObject->phongExp = atoi(input);

	Render();
}

void Scene::RefGoToView0(){
	if(refPPC){
		PPC nppc;
		nppc.Load("mydbg/refview0.txt");
		RefGoToView(&nppc);
	}
}

void Scene::RefGoToView1(){
	if(refPPC){
		PPC nppc;
		nppc.Load("mydbg/refview1.txt");
		RefGoToView(&nppc);
	}
}

void Scene::RefSaveView0(){
	if(refPPC){
		refPPC->Save("mydbg/refview0.txt");
	}
}

void Scene::RefLoadView0(){
	if(refPPC){
		refPPC->Load("mydbg/refview0.txt");
		Render();
	}
}

void Scene::RefSaveView1(){
	if(refPPC){
		refPPC->Save("mydbg/refview1.txt");
	}
}

void Scene::RefLoadView1(){
	if(refPPC){
		refPPC->Load("mydbg/refview1.txt");
		Render();
	}
}

void Scene::RefGoToView(PPC *nppc){
	PPC oppc(*refPPC);
	int n = 100;
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*refPPC = refPPC->Interpolate(&oppc, nppc, frac);
		Render();
		Fl::check();
	}
}

void Scene::DBG(){
	int currFile = 1640;

	for(int i = 0; i < 200; i++){
		reflectiveObjectHandle->sphereMorphScaleFactor = reflectiveObjectHandle->sphereMorphScaleFactor + reflectiveObjectHandle->sphereMorphDirection * (1.0f/100.0f);

		if(reflectiveObjectHandle->sphereMorphScaleFactor >= 1.0f){
			reflectiveObjectHandle->sphereMorphScaleFactor = 1.0f;
			reflectiveObjectHandle->sphereMorphDirection = -1.0f;
		}else if(reflectiveObjectHandle->sphereMorphScaleFactor <= 0.0f){
			reflectiveObjectHandle->sphereMorphScaleFactor = 0.0f;
			reflectiveObjectHandle->sphereMorphDirection = 1.0f;
		}

		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	reflectiveObjectHandle->sphereMorphScaleFactor = 0.0f;

	Render();

	Fl::check();

	/*for(int i = 0; i < 200; i++){
		diffuseObjectHandle->sphereMorphScaleFactor = diffuseObjectHandle->sphereMorphScaleFactor + diffuseObjectHandle->sphereMorphDirection * (1.0f/100.0f);

		if(diffuseObjectHandle->sphereMorphScaleFactor >= 1.0f){
			diffuseObjectHandle->sphereMorphScaleFactor = 1.0f;
			diffuseObjectHandle->sphereMorphDirection = -1.0f;
		}else if(diffuseObjectHandle->sphereMorphScaleFactor <= 0.0f){
			diffuseObjectHandle->sphereMorphScaleFactor = 0.0f;
			diffuseObjectHandle->sphereMorphDirection = 1.0f;
		}

		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	diffuseObjectHandle->sphereMorphScaleFactor = 0.0f;

	Render();

	Fl::check();*/
	
	return;
	
	//int currFile = 720;

	//writeTIFF("REFERENCE.TIFF", refFB);

	PPC ppc2(*ppc);
	ppc2.Load("mydbg/view1.txt");
	PPC oppc(*ppc);

	int n = 360;
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc, &ppc2, frac);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	for(int i = 0; i < 360; i+=1){
		currGuiObject->Rotate(currGuiObject->GetCenter(), Vector3D(0.0f, 1.0f, 0.0f), 1.0f);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}
	/*
	PPC ppc2(*ppc);
	ppc2.Load("mydbg/view1.txt");
	PPC oppc(*ppc);

	int n = 360;
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc, &ppc2, frac);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	return;
	
	//int currFile = 300;
	
	PPC ppcNew(*ppc);
	ppcNew.Pan(90.0f);
	ppcNew.Translate('b', 170.0f);
	ppcNew.Translate('r', 170.0f);

	//PPC oppc(*ppc);
	//int n = 75;
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc, &ppcNew, frac);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	ppcNew.Pan(90.0f);
	ppcNew.Translate('b', 170.0f);
	ppcNew.Translate('r', 170.0f);

	PPC oppc2(*ppc);
	n = 75;
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc2, &ppcNew, frac);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	ppcNew.Pan(90.0f);
	ppcNew.Translate('b', 170.0f);
	ppcNew.Translate('r', 170.0f);

	PPC oppc3(*ppc);
	n = 75;
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc3, &ppcNew, frac);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	ppcNew.Pan(90.0f);
	ppcNew.Translate('b', 170.0f);
	ppcNew.Translate('r', 170.0f);

	PPC oppc4(*ppc);
	n = 75;
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc4, &ppcNew, frac);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}
	
	return;*/
}

void Scene::writeCurrFrame(int currFrame, FrameBuffer *frame){
	stringstream filename;
	if(currFrame < 10){
		filename << "frames/00" << currFrame << ".tiff";
	}else if(currFrame < 100){
		filename << "frames/0" << currFrame << ".tiff";
	}else{
		filename << "frames/" << currFrame << ".tiff";
	}
	writeTIFF(filename.str(), frame);
}