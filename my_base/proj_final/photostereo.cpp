#include "scene.h"
#include "photostereo.h"

#include <fstream>

Photostereo::Photostereo(bool r){
	images = new FrameBuffer*[5];
	real = r;
	if(!real){
		images[0] = scene->openTIFF_FB("photostereo/testA-0v.tiff");
		images[1] = scene->openTIFF_FB("photostereo/testA-1v.tiff");
		images[2] = scene->openTIFF_FB("photostereo/testA-2v.tiff");
		//images[3] = scene->openTIFF_FB("photostereo/testA-3v.tiff");
		//images[4] = scene->openTIFF_FB("photostereo/testA-4v.tiff");
	}else{
		images[0] = scene->openTIFF_FB("photostereo/testA-0r.tiff");
		images[1] = scene->openTIFF_FB("photostereo/testA-1r.tiff");
		images[2] = scene->openTIFF_FB("photostereo/testA-2r.tiff");
		//images[3] = scene->openTIFF_FB("photostereo/testA-3v.tiff");
		//images[4] = scene->openTIFF_FB("photostereo/testA-4v.tiff");
	}

	for(int i = 0; i < 3; i++){
		if(images[i] == NULL){
			cerr << "ERROR: Could not open image " << i << " for photostereo" << endl;
			system("pause");
			exit(1);
		}
		//images[i]->show();
	}

	result = new FrameBuffer(20, 50, images[0]->w, images[0]->h);
	result->isHW = false;
	if(real){
		result->label("Real Result");
	}else{
		result->label("Virtual Result");
	}

	//c = new Vector3D[images[0]->w*images[0]->h];
	c = Vector3D();
	n = new Vector3D[images[0]->w*images[0]->h];
	a = new float[images[0]->w*images[0]->h];

	for(int i = 0; i < images[0]->w*images[0]->h; i++){
		n[i] = Vector3D();
	}

	vec = new Vector3D[5];

	ifstream ifs("photostereo/light_pos.txt");
	ifs >> vec[0] >> vec[1] >> vec[2] >> vec[3] >> vec[4];
	ifs.close();

	L = Matrix3x3();

	L.rows[0] = vec[0];
	L.rows[1] = vec[1];
	L.rows[2] = vec[2];
	L = L.inverse();

	//cout << v[4] << endl << v[1]  << endl << v[2] << endl;

	int i = 0;
	float * intensity = new float[5];

	for(int v = 0; v < images[0]->h; v++){
		for(int u = 0; u < images[0]->w; u++){
			intensity[0] = (images[0]->GetV(u,v)[0] + images[0]->GetV(u,v)[1] + images[0]->GetV(u,v)[2]) / 3.0f;
			intensity[1] = (images[1]->GetV(u,v)[0] + images[1]->GetV(u,v)[1] + images[1]->GetV(u,v)[2]) / 3.0f;
			intensity[2] = (images[2]->GetV(u,v)[0] + images[2]->GetV(u,v)[1] + images[2]->GetV(u,v)[2]) / 3.0f;
			//intensity[3] = (images[3]->GetV(u,v)[0] + images[3]->GetV(u,v)[1] + images[3]->GetV(u,v)[2]) / 3.0f;
			//intensity[4] = (images[4]->GetV(u,v)[0] + images[4]->GetV(u,v)[1] + images[4]->GetV(u,v)[2]) / 3.0f;
			
			/*for(int j = 0; j < 5; j++){
				cout << intensity[j] << " ";
			}
			cout << endl;*/

			/*float min = 1000000.0f;
			int minIndex = -1;
			float max = -1000000.0f;
			int maxIndex = -1;

			for(int j = 0; j < 5; j++){
				if(intensity[j] > max){
					max = intensity[j];
					maxIndex = j;
				}

				if(intensity[j] < min){
					min = intensity[j];
					minIndex = j;
				}
			}*/

			//cout << min << endl << minIndex << endl << max << endl << maxIndex << endl;

			/*if(minIndex == maxIndex){
				n[i] = Vector3D(0.0f, 0.0f, 0.0f);
				i++;
				continue;
			}*/

			/*int k = 0;
			for(int j = 0; j < 5; j++){
				if(j == maxIndex || j == minIndex){
					continue;
				}else{
					c[k] = intensity[j];
					L.rows[k] = vec[j];
					k++;
				}
			}
			L = L.inverse();*/

			if(real && intensity[0] < 0.16f && intensity[1] < 0.16f && intensity[2] < 0.16f){
				n[i] = Vector3D(0.0f, 0.0f, 0.0f);
				i++;
			}else if(!real && intensity[0] < 0.10f && intensity[1] < 0.10f && intensity[2] < 0.10){
				n[i] = Vector3D(0.0f, 0.0f, 0.0f);
				i++;
			}else{
				c = Vector3D(intensity[0], intensity[1], intensity[2]);
				n[i] = L*c;
				a[i] = n[i].length();
				n[i] = n[i].normalize();
				i++;
			}
		}
	}

	/*int count1 = 0;
	int count2 = 0;
	for(int i = 0; i < images[0]->w*images[0]->h; i++){
		if(n[i].coords[0] < 0 || n[i].coords[1] < 0 || n[i].coords[2] < 0){
			count1++;
			continue;
		}

		if(n[i].coords[0] > 0 || n[i].coords[1] > 0 || n[i].coords[2] > 0){
			count2++;
			continue;
		}
	}

	cout << count1 << endl << count2 << endl;*/

	generated = false;
	effect = 0;
	delete intensity;
}

Photostereo::~Photostereo(){

	for(int i = 0; i < 5; i++){
		delete images[i];
	}

	delete images;
	delete result;
	delete n;
	delete a;
	delete vec;
}

void Photostereo::GenerateResult(){
	if(!generated){
		int i = 0;
		result->Set(0xFF000000);
		Vector3D color = Vector3D(1.0f, 1.0f, 1.0f);
		scene->lightDir = scene->lightDir.normalize();
		for(int v = 0; v < images[0]->h; v++){
			for(int u = 0; u < images[0]->w; u++){
				if(n[i].coords[0] == 0.0f && n[i].coords[1] == 0.0f && n[i].coords[2] == 0.0f){
					i++;
					continue;
				}
			
				if(effect == 0){
					float value = a[i] * (scene->lightDir * n[i]);
					result->Set(u,v,Vector3D(value, value, value).GetColor());
				}else if(effect == 1){
					//Vector3D tempN = Vector3D(n[i][0] * n[i][0], n[i][1] * n[i][1], n[i][2] * n[i][2]);
					Vector3D tempN = Vector3D(pow(n[i][0], 2), pow(n[i][1], 2), pow(n[i][2], 2));
					if(n[i][0] < 0.0f){
						tempN[0] = -1.0f*tempN[0];
					}
					if(n[i][1] < 0.0f){
						tempN[1] = -1.0f*tempN[1];
					}
					if(n[i][2] < 0.0f){
						tempN[2] = -1.0f*tempN[2];
					}
					float value = a[i] * (scene->lightDir * tempN.normalize());
					result->Set(u,v,Vector3D(value, value, value).GetColor());
				}else if(effect == 2){
					float value = a[i] * (scene->lightDir * n[i]);

					result->Set(u,v,Vector3D(value*n[i][0], value*n[i][1], value*n[i][2]).GetColor());
				}

				//result->Set(u,v,n[i].GetColor());

				i++;
			}
		}

		result->show();
		generated = true;
	}
}

void Photostereo::UpdateResult(){
	if(generated && enabled){
		int i = 0;
		result->Set(0xFF000000);
		Vector3D color = Vector3D(1.0f, 1.0f, 1.0f);
		scene->lightDir = scene->lightDir.normalize();
		for(int v = 0; v < images[0]->h; v++){
			for(int u = 0; u < images[0]->w; u++){
				if(n[i].coords[0] == 0.0f && n[i].coords[1] == 0.0f && n[i].coords[2] == 0.0f){
					i++;
					continue;
				}

				if(effect == 0){
					float value = a[i] * (scene->lightDir * n[i]);
					result->Set(u,v,Vector3D(value, value, value).GetColor());
				}else if(effect == 1){
					//Vector3D tempN = Vector3D(n[i][0] * n[i][0], n[i][1] * n[i][1], n[i][2] * n[i][2]);
					Vector3D tempN = Vector3D(pow(n[i][0], 2), pow(n[i][1], 2), pow(n[i][2], 2));
					if(n[i][0] < 0.0f){
						tempN[0] = -1.0f*tempN[0];
					}
					if(n[i][1] < 0.0f){
						tempN[1] = -1.0f*tempN[1];
					}
					if(n[i][2] < 0.0f){
						tempN[2] = -1.0f*tempN[2];
					}
					float value = a[i] * (scene->lightDir * tempN.normalize());
					result->Set(u,v,Vector3D(value, value, value).GetColor());
				}else if(effect == 2){
					float value = a[i] * (scene->lightDir * n[i]);

					result->Set(u,v,Vector3D(value*n[i][0], value*n[i][1], value*n[i][2]).GetColor());
				}

				i++;

			}
		}

		result->redraw();
		generated = true;
	}
}