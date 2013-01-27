#include "scene.h"
#include "envmap.h"

#include <cmath>

Envmap::Envmap(){
	lastChecked = 0;

	frames = new FrameBuffer*[6];
	//cameras = new PPC*[6];
	cameras = 0;
	texID = -1;

	frames[0] = scene->openTIFF_FB("envmap/front.tiff");
	frames[0]->label("Front");
	frames[1] = scene->openTIFF_FB("envmap/left.tiff");
	frames[1]->label("Left");
	frames[2] = scene->openTIFF_FB("envmap/back.tiff");
	frames[2]->label("Back");
	frames[3] = scene->openTIFF_FB("envmap/right.tiff");
	frames[3]->label("Right");
	frames[4] = scene->openTIFF_FB("envmap/top.tiff");
	frames[4]->label("Top");
	frames[5] = scene->openTIFF_FB("envmap/bottom.tiff");
	frames[5]->label("Bottom");

	/*cameras[0] = new PPC();
	cameras[0]->Load("envmap/front.txt");
	cameras[1] = new PPC();
	cameras[1]->Load("envmap/left.txt");
	cameras[2] = new PPC();
	cameras[2]->Load("envmap/back.txt");
	cameras[3] = new PPC();
	cameras[3]->Load("envmap/right.txt");
	cameras[4] = new PPC();
	cameras[4]->Load("envmap/top.txt");
	cameras[5] = new PPC();
	cameras[5]->Load("envmap/bottom.txt");*/

	for(int i = 0; i < 6; i++){
		//frames[i]->show();
		//cout << cameras[i]->GetVD() << endl;
	}
}

Envmap::~Envmap(){
	for(int i = 0; i < 6; i++){
		delete frames[i];
		//delete cameras[i];
	}

	delete frames;
	//delete cameras;
}

unsigned int Envmap::getColor(Vector3D dir){
	unsigned int retval = 0;
	Vector3D projDir = Vector3D();

	int frame = -1;
	float sc = -1.0f;
	float tc = -1.0f;
	float ma = -1.0f;

	if(fabs(dir[0]) > fabs(dir[1]) && fabs(dir[0]) > fabs(dir[2])){
		//x is largest component
		if(dir[0] < 0){
			//cameras[3]->Project(cameras[3]->C + dir, projDir);
			//retval = frames[3]->Get(projDir[0], projDir[1]);
			//frame = 3;
			frame = 1;
			sc = +(dir[2]);
			tc = -dir[1];
			ma = dir[0];
		}else if(dir[0] > 0){
			//cameras[1]->Project(cameras[1]->C + dir, projDir);
			//retval = frames[1]->Get(projDir[0], projDir[1]);
			//frame = 1;
			frame = 3;
			sc = -dir[2];
			tc = -dir[1];
			ma = dir[0];
		}
	}else if(fabs(dir[1]) > fabs(dir[0]) && fabs(dir[1]) > fabs(dir[2])){
		//y is largest component
		if(dir[1] < 0){
			//cameras[0]->Project(cameras[0]->C + dir, projDir);
			//retval = frames[0]->Get(projDir[0], projDir[1]);
			//frame = 0;
			frame = 5;
			sc = +(dir[0]);
			tc = -dir[2];
			ma = dir[1];
		}else if(dir[1] > 0){
			//cameras[2]->Project(cameras[2]->C + dir, projDir);
			//retval = frames[2]->Get(projDir[0], projDir[1]);
			//frame = 2;
			frame = 4;
			sc = +(dir[0]);
			tc = +(dir[2]);
			ma = dir[1];
		}
	}else if(fabs(dir[2]) > fabs(dir[0]) && fabs(dir[2]) > fabs(dir[1])){
		//z is largest component
		if(dir[2] < 0){
			//cameras[5]->Project(cameras[5]->C + dir, projDir);
			//retval = frames[5]->Get(projDir[0], projDir[1]);
			//frame = 5;
			frame = 0;
			sc = -dir[0];
			tc = -dir[1];
			ma = dir[2];
		}else if(dir[2] > 0){
			//cameras[4]->Project(cameras[4]->C + dir, projDir);
			//retval = frames[4]->Get(projDir[0], projDir[1]);
			//frame = 4;
			frame = 2;
			sc = +(dir[0]);
			tc = -dir[1];
			ma = dir[2];
		}
	}

	if(frame == -1 || sc == -1.0f || tc == -1.0f || ma == -1.0f || ma == 0.0f){
		//cerr << "Something is wrong." << endl;
		//system("pause");
		return 0;
	}

	float s = (sc/fabs(ma) + 1.0f) / 2.0f;
	float t = (tc/fabs(ma) + 1.0f) / 2.0f;

	retval = frames[frame]->Get(s*frames[frame]->w, t*frames[frame]->h);

	return retval;
}

void Envmap::LoadHW(int texBinding){
	texID = texBinding;

	glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, texBinding);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP_EXT, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	int fi = 0;
	gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT, GL_RGBA8, frames[fi]->w, frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, frames[fi]->pix);
	fi++;
	gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT, GL_RGBA8, frames[fi]->w, frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, frames[fi]->pix);
	fi++;
	gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT, GL_RGBA8, frames[fi]->w, frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, frames[fi]->pix);
	fi++;
	gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT, GL_RGBA8, frames[fi]->w, frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, frames[fi]->pix);
	fi++;
	gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT, GL_RGBA8, frames[fi]->w, frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, frames[fi]->pix);
	fi++;
	gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT, GL_RGBA8, frames[fi]->w, frames[fi]->h, GL_RGBA, GL_UNSIGNED_BYTE, frames[fi]->pix);
	fi++;
}