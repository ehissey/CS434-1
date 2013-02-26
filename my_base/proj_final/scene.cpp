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
	int sci = 1;
	//int w = sci * 320;
	//int h = sci * 240;
	int w = sci * 128;
	int h = sci * 128;

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

	DI = 0;
	//DI = new DepthImage(hwFB, ppc, diffuseObjectHandle, 513, 512);

	hwFB->isDI = false;
	hwFB->show();

	refPPC = 0;
	refFB = 0;

	

	//Render();
	
}

void Scene::captureLightTransportMatrix(){
	hwFB->isLightTransport = true;
	hwFB->redraw();
	
	return;
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

	//OpenGL Spotlight
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	
	//Vector3D LVD = (currObjectHandle->GetCenter() - light->lppc->C).normalize();
	//GLfloat spot_direction[] = { LVD.coords[0], LVD.coords[1], LVD.coords[2] };
	

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 0.8);
	

	//GLfloat mat_specular[] = {0.0, 0.0, 0.0, 1.0};
	//GLfloat mat_shininess[] = { 0.0 };
	//glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	//glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	GLfloat mat_ambient[] = { 0.00, 0.00, 0.00, 1.0 };
	GLfloat mat_diffuse[] = { 0.0, 0.0, 0.0, 1.0 };
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, mat_ambient);

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glShadeModel(GL_SMOOTH);
}

void Scene::InitializeHWObjects(){
	Vector3D center = Vector3D(0.0f,0.0f,-500.0f);
	float sl = 256.0;

	currObject = new TMesh();
	currObject->Load("geometry/bunny.bin");

	AABB aabb = currObject->GetAABB();
	float size0 = (aabb.corners[1]-aabb.corners[0]).length();
	Vector3D tcenter = currObject->GetCenter();
	currObject->Translate(tcenter*-1.0f+center);
	float size1 = 700.0f;
	currObject->ScaleAboutCenter(size1/size0);
	currObject->kamb = 0.20f;
	currObject->gouraud = false;
	currObject->phong = false;
	currObject->phongExp = 40.0f;
	currObject->enableShader = false;
	currObject->shaderSelection = 2;
	currObject->sphereMorphRaidus = 20.0f;
	currObject->sphereMorphScaleFactor = 0.0f;
	currObject->sphereMorphDirection = 1.0f;
	currObject->center = center;
	//currObject->Rotate(currObject->GetCenter(), Vector3D(0.0f, 1.0f, 0.0f), 90.0f);
	//currObject->reflectiveSF = 0.5f;
	TMList.push_back(*currObject);
	currGuiObject = currObject;
	currObjectHandle = currObject;
	//reflectiveObjectHandle = currObject;

	light = new Light(128, 128, 45.0f, Vector3D(-200.0f, 0.0f, -100.0f), center, ppc);
	ppc->PositionAndOrient(Vector3D(200.0f, 0.0f, -100.0f), (center - Vector3D(200.0f, 0.0f, -100.0f)).normalize(), Vector3D(0.0f, 1.0f, 0.0f), ppc->Getf(), *ppc);  //Look at the object
}

void Scene::RenderHW(){
	//OpenGL Setup
	if(!initializedHW){
		env = 0;
		InitializeHW();
		//InitializeHWObjects();
		initializedHW = true;
	}
	
	//Frame Setup
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set View
	//Set Intrinsics
	ppc->SetIntrinsicsHW();
	//Set Extrinsics
	ppc->SetExtrinsicsHW();

	GLfloat light_position[] = { light->lppc->C.coords[0], light->lppc->C.coords[1], light->lppc->C.coords[2], 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	GLfloat spot_direction[] = { light->currLightDirection.coords[0], light->currLightDirection.coords[1], light->currLightDirection.coords[2] };
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);

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
		/*env = new Envmap();
		env->texID = 500;
		env->LoadHW();*/
		env = 0;

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
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Set View
	//Set Intrinsics
	ppc->SetIntrinsicsHW();
	//Set Extrinsics
	ppc->SetExtrinsicsHW();

	

	

	soi->PerFrameInit();

	if(env){
		bemsi->PerFrameInit();
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
	cout << "Capturing Light Transport Matrix" << endl;
	captureLightTransportMatrix();
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