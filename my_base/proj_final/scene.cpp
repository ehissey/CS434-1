#include "scene.h"
#include "Matrix3x3.h"
#include "tiff.h"
#include "tiffconf.h"
#include "tiffio.h"
#include "tiffvers.h"
#include "ppc.h"
#include "Vector3D.h"
#include "aabb.h"
#include <fstream>
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
	currSnapshot = 0;

	int u0 = 20;
	int v0 = 50;
	int sci = 2;
	int w = sci * 320;
	int h = sci * 240;
	//int w = sci * 128;
	//int h = sci * 128;

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
	
	psReal = 0;
	psVirtual = 0;
	InitializeHWObjects();

	hwFB->show();

	refPPC = 0;
	refFB = 0;

	Render();

	//cout << Fl::version() << endl;
}

void Scene::Render(){
	for(list<TMesh>::iterator i = TMList.begin(); i != TMList.end(); ++i){
		i->wireframe = wireframe || i->wireframe;
	}

	if(hwFB){
		hwFB->redraw();
	}

	if(psReal){
		if(psReal->generated && psReal->enabled){
			psReal->UpdateResult();
		}
	}

	if(psVirtual){
		if(psVirtual->generated && psVirtual->enabled){
			psVirtual->UpdateResult();
		}
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
	Vector3D center = Vector3D(0.0f,0.0f,-200.0f);
	float sl = 256.0;

	currObject = new TMesh();
	currObject->Load("geometry/bunny.bin");

	AABB aabb = currObject->GetAABB();
	float size0 = (aabb.corners[1]-aabb.corners[0]).length();
	Vector3D tcenter = currObject->GetCenter();
	currObject->Translate(tcenter*-1.0f+center);
	float size1 = 180.0f;
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
	//currObject->Rotate(currObject->GetCenter(), Vector3D(0.0f, 1.0f, 0.0f), 90.0f);
	//currObject->reflectiveSF = 0.5f;
	TMList.push_back(*currObject);
	currGuiObject = currObject;
	currObjectHandle = currObject;
	//reflectiveObjectHandle = currObject;

	lightDir = Vector3D(-1.0f, 0.0f, 0.0f);

	psReal = new Photostereo(true);
	psVirtual = new Photostereo(false);
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
	//ps->GenerateResult();
	if(psReal->enabled){
		writeCurrFrame(currSnapshot, psReal->result);
	}else if(psVirtual->enabled){
		writeCurrFrame(currSnapshot, psVirtual->result);
	}

	currSnapshot++;


	return;

	ofstream ofs("photostereo/light_pos.txt");

	lightDir = Vector3D(-1.0f, 0.0f, 1.0f);
	Render();
	Fl::check();
	//writeCurrFrame(0, hwFB);
	writeTIFF("photostereo/testA-0v.tiff", hwFB);
	ofs << lightDir << endl;

	lightDir = Vector3D(1.0f, 0.0f, 1.0f);
	Render();
	Fl::check();
	//writeCurrFrame(1, hwFB);
	writeTIFF("photostereo/testA-1v.tiff", hwFB);
	ofs << lightDir << endl;

	lightDir = Vector3D(0.0f, 1.0f, 1.0f);
	Render();
	Fl::check();
	//writeCurrFrame(2, hwFB);
	writeTIFF("photostereo/testA-2v.tiff", hwFB);
	ofs << lightDir << endl;
	
	lightDir = Vector3D(-1.0f, 1.0f, 0.0f);
	Render();
	Fl::check();
	//writeCurrFrame(3, hwFB);
	writeTIFF("photostereo/testA-3v.tiff", hwFB);
	ofs << lightDir << endl;

	lightDir = Vector3D(1.0f, 1.0f, 0.0f);
	Render();
	Fl::check();
	//writeCurrFrame(4, hwFB);
	writeTIFF("photostereo/testA-4v.tiff", hwFB);
	ofs << lightDir << endl;

	lightDir = Vector3D(-1.0f, 0.0f, 0.0f);
	Render();
	Fl::check();
	
	ofs.close();

	/*ifstream ifs("photostereo/light_pos.txt");
	Vector3D v0;
	Vector3D v1;
	Vector3D v2;
	Vector3D v3;
	Vector3D v4;
	ifs >> v0 >> v1 >> v2 >> v3 >> v4;

	cout << v0 << endl << v1 << endl << v2 << endl << v3 << endl << v4 << endl;

	ifs.close();*/
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

void Scene::setPSReal(){
	if(!psReal->generated){
		psReal->GenerateResult();
		psReal->enabled = true;
		psVirtual->enabled = false;
		psVirtual->result->hide();
	}else{
		psReal->enabled = true;
		psVirtual->enabled = false;
		psReal->result->show();
		psVirtual->result->hide();
		psReal->UpdateResult();
	}

	hwFB->hide();
}
void Scene::setPSVirtual(){
	if(!psVirtual->generated){
		psVirtual->GenerateResult();
		psVirtual->enabled = true;
		psReal->enabled = false;
		psReal->result->hide();
	}else{
		psVirtual->enabled = true;
		psReal->enabled = false;
		psVirtual->result->show();
		psReal->result->hide();
		psVirtual->UpdateResult();
	}

	hwFB->hide();
}

void Scene::setPSEffect(int e){
	/*if(e == 2){
		lightDir = Vector3D(-0.5f, 0.0f, 1.0f);
		Render();
	}else{*/
		psReal->effect = e;
		psVirtual->effect = e;

		if(psReal->enabled){
			psReal->UpdateResult();
		}else if(psVirtual->enabled){
			psVirtual->UpdateResult();
		}
	//}
}