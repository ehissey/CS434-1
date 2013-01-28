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
	sf = 0.6f;
	lightDirv = Vector3D(0.0f, 0.0f, 1.0f);

	int u0 = 20;
	int v0 = 50;
	int sci = 2;
	int w = sci * 320;
	int h = sci * 240;

	gui = new GUI();
	gui->show();
	gui->uiw->position(w+20+20, 50);

	

	hwFB = new FrameBuffer(u0, v0, w, h);
	hwFB->label("HW Framebuffer");
	hwFB->isHW = true;
	hwFB->isRef = false;
	hwFB->show();
	cgi = new CGInterface();
	soi = new ShaderOneInterface();
	//hwFB = 0;

	float hfov = 45.0f;
	ppc = new PPC(hfov, w, h);
	
	if(!hwFB){
		fb = new FrameBuffer(u0,v0,w,h);
		fb->label("FrameBuffer");
		fb->show();

		InitializeSWObjects();
	}

	/*refPPC = new PPC(hfov, w, h);
	refFB = new FrameBuffer(u0*15, v0*10, w, h);
	refFB->isRef = true;
	refFB->isHW = true;
	refFB->label("Ref Frembuffer");
	refFB->show();
	settingRefMatrix = false;
	RefLoadView0();*/
	refPPC = 0;
	refFB = 0;

	env = new Envmap();

	Render();
}

void Scene::InitializeSWObjects(){
	Vector3D center = Vector3D(0.0f,0.0f,-170.0f);
	float sl = 256.0;

	//pl = new PointLight(Vector3D(0.0f, 0.0f, -70.0f));
	//pl->enabled = false;
	pl = 0;

	currObject = new TMesh();

	currObject->Load("geometry/teapot57k.bin");

	AABB aabb = currObject->GetAABB();
	float size0 = (aabb.corners[1]-aabb.corners[0]).length();
	Vector3D tcenter = currObject->GetCenter();
	currObject->Translate(tcenter*-1.0f+center);
	float size1 = 170.0;
	currObject->ScaleAboutCenter(size1/size0);
	currObject->kamb = 0.20f;
	currObject->gouraud = false;
	currObject->phong = false;
	currObject->phongExp = 40.0f;
	currObject->enableShader = false;
	currObject->textured = false;
	currObject->envmapReflection = true;
	TMList.push_back(*currObject);
	currGuiObject = currObject;

	//env = new Envmap();
	env = 0;

	//ppc->Translate('f', 170.0f);
	//ppc->Roll(90.0f);
	//ppc->Translate('b', 2.5f);
	//ppc->Pan(180.0f);


	//currGuiObject->Rotate(currGuiObject->GetCenter(), ppc->a, -90.0f);
	//currGuiObject->Rotate(currGuiObject->GetCenter(), ppc->b*-1, 180.0f);

	//ppc->Pan(90.0f);
	//ppc->Translate('b', 170.0f);
	//ppc->Translate('r', 170.0f);

	//cout << ppc->GetVD() << endl;
}

void Scene::FrameSetup(){
	if(!env){
		fb->Set(0xFFFFFFFF);
	}else{
		for(int v = 0; v < fb->h; v++){
			for(int u = 0; u < fb->w; u++){
				Vector3D dir = ((ppc->c + ppc->a*((float)u+0.5f) + ppc->b*((float)v+0.5f))).normalize();
				unsigned int col = env->getColor(dir);
				fb->Set(u,v,col);
			}
		}
	}
	fb->SetZB(0.0f);
}

void Scene::Render(){
	for(list<TMesh>::iterator i = TMList.begin(); i != TMList.end(); ++i){
		i->wireframe = wireframe;
	}
	
	if(hwFB){
		if(refPPC && refFB){
			refFB->redraw();
		}
		hwFB->redraw();
	}
	return;
	//SW Rendering
	FrameSetup();
	if(refPPC && refFB){
		currObject->projTextured = false;
		refFB->Set(0xFFFFFFFF);
		refFB->SetZB(0.0f);
		currObject->Render(refPPC, refFB, pl, wireframe, env);
		currObject->projTextured = true;
	}
		
	for(list<TMesh>::iterator i = TMList.begin(); i != TMList.end(); ++i){
		i->Render(ppc, fb, pl, wireframe, env);
	}

	if(pl){
		if(pl->enabled == true){
			pl->Visualize(ppc, fb);
		}
	}

	fb->redraw();

	if(refPPC && refFB){
		refFB->redraw();
	}
}

void Scene::DBG(){
	int currFile = 0;

	//writeTIFF("REFERENCE.TIFF", refFB);

	for(int i = 0; i < 360; i++){
		currGuiObject->Rotate(currGuiObject->GetCenter(), ppc->b * -1.0f, 1.0f);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

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
	
	return;
	
	writeTIFF("envmap/front.tiff", hwFB);
	ppc->Save("envmap/front.txt");
	ppc->Pan(90.0f);
	Render();
	Fl::check();
	writeTIFF("envmap/left.tiff", hwFB);
	ppc->Save("envmap/left.txt");
	ppc->Pan(90.0f);
	Render();
	Fl::check();
	writeTIFF("envmap/back.tiff", hwFB);
	ppc->Save("envmap/back.txt");
	ppc->Pan(90.0f);
	Render();
	Fl::check();
	writeTIFF("envmap/right.tiff", hwFB);
	ppc->Save("envmap/right.txt");
	ppc->Pan(90.0f);
	ppc->Roll(90.0f);
	Render();
	Fl::check();
	writeTIFF("envmap/top.tiff", hwFB);
	ppc->Save("envmap/top.txt");
	ppc->Roll(180.0f);
	Render();
	Fl::check();
	writeTIFF("envmap/bottom.tiff", hwFB);
	ppc->Save("envmap/bottom.txt");

	return;
}

int Scene::runCameraPath(int currFile){
	PPC * ppcNew = new PPC();

	ppc->Load("mydbg/view0_demo.txt");
	ppcNew->Load("mydbg/view1_demo.txt");

	PPC oppc(*ppc);
	int n = 90;
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc, ppcNew, frac);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	ppc->Load("mydbg/view1_demo.txt");
	ppcNew->Load("mydbg/view2_demo.txt");

	PPC oppc2(*ppc);
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc2, ppcNew, frac);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	ppc->Load("mydbg/view2_demo.txt");
	ppcNew->Load("mydbg/view3_demo.txt");

	PPC oppc3(*ppc);
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc3, ppcNew, frac);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	ppc->Load("mydbg/view3_demo.txt");
	ppcNew->Load("mydbg/view4_demo.txt");

	PPC oppc4(*ppc);
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc4, ppcNew, frac);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	ppc->Load("mydbg/view4_demo.txt");
	ppcNew->Load("mydbg/view0_demo.txt");

	PPC oppc5(*ppc);
	for (int i = 0; i < n; i++) {
		float frac = (float) i / (float) (n-1);
		*ppc = ppc->Interpolate(&oppc5, ppcNew, frac);
		Render();

		writeCurrFrame(currFile, hwFB);
		currFile++;

		Fl::check();
	}

	

	delete ppcNew;

	return currFile;
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
	//writeTIFF(filename.str(), frame);
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

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		unsigned int * tempPix = texs[i]->convertPixToGLFormat();

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texs[i]->w, texs[i]->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tempPix);

		delete tempPix;
	}

	for(int i = 0; i < texsN; i++){
		delete texs[i];
	}

	delete texs;

	if(env){
		int fi = 0;
		glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, env->texID);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		unsigned int * tempPix = env->frames[fi]->convertPixToGLFormat();
		gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT, GL_RGBA8, env->frames[fi]->w, env->frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, tempPix);
		delete tempPix;
		fi++;
		tempPix = env->frames[fi]->convertPixToGLFormat();
		gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT, GL_RGBA8, env->frames[fi]->w, env->frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, tempPix);
		delete tempPix;
		fi++;
		tempPix = env->frames[fi]->convertPixToGLFormat();
		gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT, GL_RGBA8, env->frames[fi]->w, env->frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, tempPix);
		delete tempPix;
		fi++;
		tempPix = env->frames[fi]->convertPixToGLFormat();
		gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT, GL_RGBA8, env->frames[fi]->w, env->frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, tempPix);
		delete tempPix;
		fi++;
		tempPix = env->frames[fi]->convertPixToGLFormat();
		gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT, GL_RGBA8, env->frames[fi]->w, env->frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, tempPix);
		delete tempPix;
		fi++;
		tempPix = env->frames[fi]->convertPixToGLFormat();
		gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT, GL_RGBA8, env->frames[fi]->w, env->frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, tempPix);
		delete tempPix;
		fi++;
	}
}

void Scene::InitializeHWObjects(){
		Vector3D center = Vector3D(30.0f,0.0f,-170.0f);
		float sl = 256.0;

		currObject = new TMesh();
		currObject->Load("geometry/bunny.bin");

		AABB aabb = currObject->GetAABB();
		float size0 = (aabb.corners[1]-aabb.corners[0]).length();
		Vector3D tcenter = currObject->GetCenter();
		currObject->Translate(tcenter*-1.0f+center);
		float size1 = 100.0f;
		cout << size0 << endl;
		currObject->ScaleAboutCenter(size1/size0);
		currObject->kamb = 0.20f;
		currObject->gouraud = false;
		currObject->phong = false;
		currObject->phongExp = 40.0f;
		currObject->enableShader = false;
		//currObject->reflectiveSF = 0.5f;
		TMList.push_back(*currObject);
		currGuiObject = currObject;
		//delete currObject;


		center = Vector3D(-30.0f,0.0f,-170.0f);

		currObject = new TMesh();
		currObject->Load("geometry/bunny.bin");

		aabb = currObject->GetAABB();
		size0 = (aabb.corners[1]-aabb.corners[0]).length();
		tcenter = currObject->GetCenter();
		currObject->Translate(tcenter*-1.0f+center);
		size1 = 100.0f;
		cout << size0 << endl;
		currObject->ScaleAboutCenter(size1/size0);
		currObject->kamb = 0.20f;
		currObject->gouraud = false;
		currObject->phong = false;
		currObject->phongExp = 40.0f;
		currObject->enableShader = false;
		//currObject->reflectiveSF = 0.5f;
		TMList.push_back(*currObject);
		currGuiObject = currObject;

		/*currObject = new TMesh();
		currObject->loadProj8Quad(center);
		currObject->enableShader = true;
		TMList.push_back(*currObject);
		proj8QuadHandle = currObject;*/
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

void Scene::RenderRefHW(){
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
	refPPC->SetIntrinsicsHW();
	//Set Extrinsics
	refPPC->SetExtrinsicsHW();

	for(list<TMesh>::iterator i = TMList.begin(); i != TMList.end(); ++i){
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		i->RenderHW();
	}
}

void Scene::RenderGPU(){
	if(!initializedHW){
		//env = new Envmap();
		//env->texID = 500;
		env = 0;
		InitializeHW();
		InitializeHWObjects();
		initializedHW = true;
	}
	
	if(!initializedGPU){
		cgi->PerSessionInit();
		soi->PerSessionInit(cgi);
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
	settingRefMatrix = false;
	
	if(env){
		renderingBackground = 1.0f;
		soi->PerFrameInit();
		soi->BindPrograms();
		cgi->EnableProfiles();
		ppc->RenderImageFrameGL();
		cgi->DisableProfiles();
		renderingBackground = 0.0f;
	}

	soi->PerFrameInit();
	ppc->SetIntrinsicsHW();
	ppc->SetExtrinsicsHW();
	soi->BindPrograms();

	for(list<TMesh>::iterator i = TMList.begin(); i != TMList.end(); ++i){
		if(i->enableShader){
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			cgi->EnableProfiles();
			i->RenderHW();
			cgi->DisableProfiles();
		}else{
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			i->RenderHW();
		}
	}

	soi->PerFrameDisable();
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

void Scene::openTIFF(string filename){
	TIFF *FILE;

	if((FILE = TIFFOpen(filename.c_str(), "r")) == 0){
		return;
	}

	delete(fb);

	int u0 = 20;
	int v0 = 50;
	int w = 0;
	int h = 0;

	TIFFGetField(FILE, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField(FILE, TIFFTAG_IMAGELENGTH, &h);

	unsigned int npix = w*h;
	unsigned int *raster =(unsigned int *) _TIFFmalloc(npix *sizeof(unsigned int));

	fb = new FrameBuffer(u0,v0,w,h);
	fb->label(filename.c_str());
	fb->show();

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

	Render();

	return;
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

void Scene::writeTIFF(string filename){
	TIFF *FILE;
	
	if((FILE = TIFFOpen(filename.c_str(), "w")) == 0){
		return;
	}

	int sampleperpixel = 4;

	unsigned char *image = (unsigned char*) fb->pix;

	TIFFSetField(FILE, TIFFTAG_IMAGEWIDTH, fb->w);
	TIFFSetField(FILE, TIFFTAG_IMAGELENGTH, fb->h);
	TIFFSetField(FILE, TIFFTAG_SAMPLESPERPIXEL, sampleperpixel);
	TIFFSetField(FILE, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(FILE, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(FILE, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(FILE, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	tsize_t line = sampleperpixel * fb->w; 

	unsigned char *buffer = NULL;

	if(TIFFScanlineSize(FILE) == line){
		buffer = (unsigned char*) _TIFFmalloc(line);
	}else{
		buffer = (unsigned char*) _TIFFmalloc(TIFFScanlineSize(FILE));
	}

	TIFFSetField(FILE, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(FILE, fb->w*sampleperpixel));

	for (unsigned int row = 0; row < (unsigned int)fb->h; row++)
	{
		memcpy(buffer, &image[(fb->h-row-1)*line], line);
		if (TIFFWriteScanline(FILE, buffer, row, 0) < 0){
			break;
		}
	}

	TIFFClose(FILE);

	if(buffer){
		_TIFFfree(buffer);
	}
}

void Scene::FindEdges(){
	FrameBuffer *edgeMap;

	int u0 = 20;
	int v0 = 50;
	int w = fb->w;
	int h = fb->h;
	edgeMap = new FrameBuffer(u0,v0,w,h);
	edgeMap->label("Edge Map");

	fb->FindEdges(edgeMap);

	FrameBuffer *old = fb;

	fb = edgeMap;

	delete old;

	fb->show();
}

void Scene::loadObject(const char * objectName){

	//DISABLED
	return;

	if(strcmp(objectName, "Cube") == 0){
		Vector3D center = Vector3D(0.0f,0.0f,-175.0f);
		float sl = 30.0;

		currObject->SetCube(center,sl);
		pl->enabled = false;
		currObject->gouraud = false;
		currObject->phong = false;
		currObject->texMirror = false;
		currObject->texRepetition = false;
		currObject->textured = false;
	}else if(strcmp(objectName, "Tex Subwoofer") == 0){
		Vector3D center = Vector3D(0.0f,0.0f,-450.0f);

		currObject->SetDemoTexturedModel(center);
		currObject->gouraud = false;
		currObject->phong = false;
		pl->enabled = false;
	}else{
		Vector3D center = Vector3D(0.0f,0.0f,-175.0f);

		stringstream filename;
		char * objN = (char *) malloc((strlen(objectName)+1)*sizeof(char));
		strcpy(objN,objectName);
		filename << "geometry/" << objN << ".bin";
		char * finalName = (char *) malloc((strlen(objectName)+1)*sizeof(char));
		strcpy(finalName,filename.str().c_str());
		
		currObject->Load(finalName);

		AABB aabb = currObject->GetAABB();
		float size0 = (aabb.corners[1]-aabb.corners[0]).length();
		Vector3D tcenter = currObject->GetCenter();
		currObject->Translate(tcenter*-1.0f+center);
		float size1 = 170.0;
		currObject->ScaleAboutCenter(size1/size0);

		pl->enabled = false;

		if(pl->enabled == true){
			currObject->kamb = 0.20f;
			currObject->gouraud = true;
			currObject->phong = false;
			currObject->phongExp = 40.0f;
		}

		currObject->texMirror = false;
		currObject->texRepetition = false;
		currObject->textured = false;

	}

	cout << "Rendering..." << endl;
	Render();
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

void Scene::setTexRepetition(){
	if(currObject->textured){
		currObject->texMirror = false;
		currObject->texRepetition = true;

		Render();
	}
}


void Scene::setTexMirror(){
	if(currObject->textured){
		currObject->texMirror = true;
		currObject->texRepetition = false;

		Render();
	}
}

void Scene::ProjectiveTextureMappingSetup(){
	if(refPPC){
		delete refPPC;
		delete refFB;
	}

	refFB = new FrameBuffer(20, fb->h+50+30, 512, 512);

	refPPC = new PPC(*ppc);
	refPPC->w = refFB->w;
	refPPC->h = refFB->h;
	float hfovR = 45.0f / 180.0f * PI;
	refPPC->c = (ppc->GetVD().normalize()*(float)refPPC->w/(2.0f*tanf(hfovR/2.0f))) + ppc->a.normalize() * -refPPC->w/2.0f - ppc->b.normalize() * refPPC->h/2.0f;

	currObject->projTextured = false;
	refFB->Set(0xFFFFFFFF);
	refFB->SetZB(0.0f);
	currObject->Render(refPPC, refFB, pl, wireframe, env);
	//currObject->projTextured = true;

	refFB->label("Reference Image");
	refFB->show();

	RefSaveView0();

	currObject->projPPC = refPPC;
	currObject->projTM = refFB;
	currObject->projTextured = true;
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

void Scene::SetGLDemoTexturedModel(Vector3D center){
	int tN = 6;
	int dx = 32;
	int dy = 50;
	int dz = 50;
	TMesh **GLDemoTexturedModel = new TMesh*[tN];

	for(int i = 0; i < tN; i++){
		GLDemoTexturedModel[i] = new TMesh();

		//Initializations
		GLDemoTexturedModel[i]->textured = true;
		GLDemoTexturedModel[i]->vertsN = 4;
		GLDemoTexturedModel[i]->trisN = 2;
		GLDemoTexturedModel[i]->verts = new Vector3D[GLDemoTexturedModel[i]->vertsN];
		GLDemoTexturedModel[i]->tris = new unsigned int[GLDemoTexturedModel[i]->trisN];
		GLDemoTexturedModel[i]->st = new Vector3D[GLDemoTexturedModel[i]->vertsN];
		GLDemoTexturedModel[i]->st_2D = new float[GLDemoTexturedModel[i]->vertsN*2];
		GLDemoTexturedModel[i]->cols = new Vector3D[GLDemoTexturedModel[i]->vertsN];

		//Tri connections
		int tri = 0;
		GLDemoTexturedModel[i]->tris[3*tri+0] = 0;
		GLDemoTexturedModel[i]->tris[3*tri+1] = 1;
		GLDemoTexturedModel[i]->tris[3*tri+2] = 2;
		tri++;
		GLDemoTexturedModel[i]->tris[3*tri+0] = 2;
		GLDemoTexturedModel[i]->tris[3*tri+1] = 3;
		GLDemoTexturedModel[i]->tris[3*tri+2] = 0;
		tri++;

		//Vertex colors
		for(int j = 0; j < GLDemoTexturedModel[i]->vertsN; j++){
			if(j % 2 == 0){
				GLDemoTexturedModel[i]->cols[j] = Vector3D(0.0f, 0.0f, 1.0f);
			}else{
				GLDemoTexturedModel[i]->cols[j] = Vector3D(1.0f, 0.0f, 0.0f);
			}
		}

		//Tex coords
		GLDemoTexturedModel[i]->st[0] = Vector3D(0.0f, 0.0f, 0.0f);
		GLDemoTexturedModel[i]->st[1] = Vector3D(0.0f, 1.0f, 0.0f);
		GLDemoTexturedModel[i]->st[2] = Vector3D(1.0f, 1.0f, 0.0f);
		GLDemoTexturedModel[i]->st[3] = Vector3D(1.0f, 0.0f, 0.0f);
		for(int j = 0; j < GLDemoTexturedModel[i]->vertsN; j++){
			GLDemoTexturedModel[i]->st_2D[2*j] = GLDemoTexturedModel[i]->st[j][0];
			GLDemoTexturedModel[i]->st_2D[2*j+1] = GLDemoTexturedModel[i]->st[j][1];
		}
	}

	int currTMesh = 0;
	//Front
	GLDemoTexturedModel[currTMesh]->verts[0] = center + Vector3D(-dx, dy, dz);
	GLDemoTexturedModel[currTMesh]->verts[1] = center + Vector3D(-dx, -dy, dz);
	GLDemoTexturedModel[currTMesh]->verts[2] = center + Vector3D(dx, -dy, dz);
	GLDemoTexturedModel[currTMesh]->verts[3] = center + Vector3D(dx, dy, dz);
	GLDemoTexturedModel[currTMesh]->texID = texNameToIDMap["sub_front"];
	//cerr << "currTMesh: " << currTMesh << "\ttexID: " << GLDemoTexturedModel[currTMesh]->texID << "\t" << texNameToIDMap["sub_front"] << endl;
	currTMesh++;
	//Right
	GLDemoTexturedModel[currTMesh]->verts[0] = center + Vector3D(dx, dy, dz);
	GLDemoTexturedModel[currTMesh]->verts[1] = center + Vector3D(dx, -dy, dz);
	GLDemoTexturedModel[currTMesh]->verts[2] = center + Vector3D(dx, -dy, -dz);
	GLDemoTexturedModel[currTMesh]->verts[3] = center + Vector3D(dx, dy, -dz);
	GLDemoTexturedModel[currTMesh]->texID = texNameToIDMap["sub_right"];
	//cerr << "currTMesh: " << currTMesh << "\ttexID: " << GLDemoTexturedModel[currTMesh]->texID << "\t" << texNameToIDMap["sub_right"] << endl;
	currTMesh++;
	//Back
	GLDemoTexturedModel[currTMesh]->verts[0] = center + Vector3D(dx, dy, -dz);
	GLDemoTexturedModel[currTMesh]->verts[1] = center + Vector3D(dx, -dy, -dz);
	GLDemoTexturedModel[currTMesh]->verts[2] = center + Vector3D(-dx, -dy, -dz);
	GLDemoTexturedModel[currTMesh]->verts[3] = center + Vector3D(-dx, dy, -dz);
	GLDemoTexturedModel[currTMesh]->texID = texNameToIDMap["sub_back"];
	//cerr << "currTMesh: " << currTMesh << "\ttexID: " << GLDemoTexturedModel[currTMesh]->texID << endl;
	currTMesh++;
	//Left
	GLDemoTexturedModel[currTMesh]->verts[0] = center + Vector3D(-dx, dy, -dz);
	GLDemoTexturedModel[currTMesh]->verts[1] = center + Vector3D(-dx, -dy, -dz);
	GLDemoTexturedModel[currTMesh]->verts[2] = center + Vector3D(-dx, -dy, dz);
	GLDemoTexturedModel[currTMesh]->verts[3] = center + Vector3D(-dx, dy, dz);
	GLDemoTexturedModel[currTMesh]->texID = texNameToIDMap["sub_left"];
	//cerr << "currTMesh: " << currTMesh << "\ttexID: " << GLDemoTexturedModel[currTMesh]->texID << endl;
	currTMesh++;
	//Top
	GLDemoTexturedModel[currTMesh]->verts[0] = center + Vector3D(-dx, dy, -dz);
	GLDemoTexturedModel[currTMesh]->verts[1] = center + Vector3D(-dx, dy, dz);
	GLDemoTexturedModel[currTMesh]->verts[2] = center + Vector3D(dx, dy, dz);
	GLDemoTexturedModel[currTMesh]->verts[3] = center + Vector3D(dx, dy, -dz);
	GLDemoTexturedModel[currTMesh]->texID = texNameToIDMap["sub_top"];
	//cerr << "currTMesh: " << currTMesh << "\ttexID: " << GLDemoTexturedModel[currTMesh]->texID << endl;
	currTMesh++;
	//Bottom
	GLDemoTexturedModel[currTMesh]->verts[0] = center + Vector3D(-dx, -dy, dz);
	GLDemoTexturedModel[currTMesh]->verts[1] = center + Vector3D(-dx, -dy, -dz);
	GLDemoTexturedModel[currTMesh]->verts[2] = center + Vector3D(dx, -dy, -dz);
	GLDemoTexturedModel[currTMesh]->verts[3] = center + Vector3D(dx, -dy, dz);
	GLDemoTexturedModel[currTMesh]->st[0] = Vector3D(0.0f, 0.0f, 0.0f);
	GLDemoTexturedModel[currTMesh]->st[1] = Vector3D(0.0f, 4.0f, 0.0f);
	GLDemoTexturedModel[currTMesh]->st[2] = Vector3D(4.0f, 4.0f, 0.0f);
	GLDemoTexturedModel[currTMesh]->st[3] = Vector3D(4.0f, 0.0f, 0.0f);
	GLDemoTexturedModel[currTMesh]->texID = texNameToIDMap["brick50"];
	//cerr << "currTMesh: " << currTMesh << "\ttexID: " << GLDemoTexturedModel[currTMesh]->texID << endl;
	for(int j = 0; j < GLDemoTexturedModel[currTMesh]->vertsN; j++){
		GLDemoTexturedModel[currTMesh]->st_2D[2*j] = GLDemoTexturedModel[currTMesh]->st[j][0];
		GLDemoTexturedModel[currTMesh]->st_2D[2*j+1] = GLDemoTexturedModel[currTMesh]->st[j][1];
	}
	currTMesh++;
	

	for(int i = 0; i < tN; i++){
		TMList.push_back(*GLDemoTexturedModel[i]);

		delete GLDemoTexturedModel[i];
	}

	delete GLDemoTexturedModel;
}
