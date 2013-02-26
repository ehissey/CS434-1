#include "scene.h"
#include "light.h"

#include <sstream>


Light::Light(int width, int height, float hfov, Vector3D eye, Vector3D lookAt, PPC * cppc){
	enabled = true;
	grayscale = true;
	cameraVectorRendered = false;

	lppc = new PPC(179.5f, width, height);  //Set up directional light grid as a ppc

	lppc->PositionAndOrient(eye, lookAt - eye, Vector3D(0.0f, 1.0f, 0.0f), lppc->Getf(), *lppc);  //Look at the object

	lightVector = new float[width*height];
	cameraVector = new float[width*height];

	int cSize = cppc->w*cppc->h;

	/*lightTransportMatrix = new float*[cSize];
	for(int i = 0; i < cSize; i++){
		lightTransportMatrix[i] = new float[width*height];
	}*/

	int TransportMatrixParameters = 1;  //1 matrix for greyscale, 3 matricies for RGB

	lightTransportMatrix = new unsigned char**[TransportMatrixParameters];  //Initialize pointers to matricies
	for(int i = 0; i < TransportMatrixParameters; i++){
		lightTransportMatrix[i] = new unsigned char*[cSize];  //Initialize pointers to columns of matrix of parameter i

		for(int j = 0; j < cSize; j++){
			lightTransportMatrix[i][j] = new unsigned char[width*height];  //Initialize columns of matrix i
			for(int k = 0; k < width*height; k++){
				lightTransportMatrix[i][j][k] = 0;
			}
		}
	}

	lightFrameBuffer = new FrameBuffer(0.0f, 0.0f, width, height);

	w = lppc->w;
	h = lppc->h;
	currLightDirection = Vector3D(0.0f, 0.0f, 0.0f);

	setLightVector(1.0f);
	//setLightVectorCheckered(8);
	//loadLightVector("light_transport/load_light.TIFF");
	saveLightVectorAsImage();
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
			lightVector[v*w + u] = value;  //vth row + u offset in that row
		}
	}


	saveLightVectorAsImage();  //temporary
	return true;
}

bool Light::setLightVectorCheckered(int checkerSize){
	if(checkerSize % 2 != 0 || checkerSize > 64){
		return false;
	}

	int firstRowValue = 0;
	int currValue = firstRowValue;
	for(int v = 0; v < h; v+=checkerSize){
		firstRowValue = 1 - firstRowValue;
		currValue = firstRowValue;
		for(int u = 0; u < w; u+=checkerSize){


			for(int vprime = v; vprime < v + checkerSize; vprime++){
				for(int uprime = u; uprime < u + checkerSize; uprime++){
					lightVector[vprime*w + uprime] = currValue;
				}
			}

			currValue = 1 - currValue;
		}
	}

	saveLightVectorAsImage();
	return true;
}

bool Light::saveLightVectorAsImage(){
	for(int v = 0; v < h; v++){
		for(int u = 0; u < w; u++){
			lightFrameBuffer->pix[(h - 1 -v)*w + u] = Vector3D(lightVector[v*w + u], lightVector[v*w + u], lightVector[v*w + u]).GetColor();
		}
	}

	scene->writeTIFF("light_transport/LIGHT.TIFF", lightFrameBuffer);

	return true;
}

bool Light::loadLightVector(string filename){
	if(lightFrameBuffer){
		delete lightFrameBuffer;
	}
	
	lightFrameBuffer = scene->openTIFF_FB(filename);

	for(int i = 0; i < w*h; i++){
		
	}

	for(int v = 0; v < h; v++){
		for(int u = 0; u < w; u++){
			Vector3D temp = Vector3D(0.0f, 0.0f, 0.0f);
			temp.SetFromColor(lightFrameBuffer->pix[(h - 1 -v)*w + u]);
			lightVector[v*w + u] = temp.coords[0];
		}
	}

	return true;
}

bool Light::insertColumnIntoLightTransportMatrix(int col, FrameBuffer * fb){
	if(grayscale){
		for(int i = 0; i < w*h; i++){
			Vector3D tempCol;
			tempCol.SetFromColor(fb->pix[i]);

			unsigned char valToInsert = 0;

			if(tempCol.coords[0] < 0.001f && tempCol.coords[1] < 0.001f && tempCol.coords[2] < 0.001f){
				valToInsert = 0;
			}else{
				valToInsert = ((tempCol.coords[0] + tempCol.coords[1] + tempCol.coords[2]) / 3.0f) * 255.0f;
			}

			lightTransportMatrix[0][col][i] = valToInsert;
		}
	
	}else{
		//Extra credit stuff
	}
	
	return true;
}

bool Light::saveSubsampledLightTransportMatrixAsImage(string filename){
	if(grayscale){
		int LTFB_h = w*h / 4.0f;
		int LTFB_w = w*h / 4.0f;

		FrameBuffer * LTGrayScale = new FrameBuffer(0.0f, 0.0f, LTFB_w, LTFB_h);
		int pixv = 0;
		int pixu = 0;
		for(int v = 0; v < w*h; v+=4){
			pixu = 0;
			for(int u = 0; u < w*h; u+=4){
				Vector3D valToAssign = Vector3D(0.0f, 0.0f, 0.0f);
				unsigned char max = 0;

				//Find max value out of 4x4 block of values in LT matrix
				for(int vprime = v; vprime < v+4; vprime++){
					for(int uprime = u; uprime < u+4; uprime++){
						unsigned char temp = lightTransportMatrix[0][uprime][vprime];
						if(temp > max){
							max = temp;
						}
					}
				}

				valToAssign = Vector3D(max/255.0f, max/255.0f, max/255.0f);
				LTGrayScale->pix[(LTFB_h - 1 - pixv)*LTFB_w + pixu] = valToAssign.GetColor();
				//LTGrayScale->pix[pixv*LTFB_w + pixu] = valToAssign.GetColor();
				pixu++;
			}
			pixv++;
		}

		scene->writeTIFF(filename, LTGrayScale);

		delete LTGrayScale;
	}else{
		//Extra credit stuff
	}
	
	return true;
}

bool Light::applyLightTransportMatrixToLightVector(FrameBuffer *fb){
	if(grayscale){
		unsigned char * rowBuffer = new unsigned char[w*h];

		for(int c_i = 0; c_i < w*h; c_i++){
			for(int col = 0; col < w*h; col++){
				rowBuffer[col] = lightTransportMatrix[0][col][c_i];
			}

			unsigned int sum = 0;
			unsigned int count = 0;
			for(int l_i = 0; l_i < w*h; l_i++){
				if(rowBuffer[l_i] > 0.01f){
					sum = sum + rowBuffer[l_i] * lightVector[l_i];
					count++;
				}
			}

			if(count != 0){
				sum = sum / count;
			}

			Vector3D valToAssign = Vector3D(sum / 255.0f, sum / 255.0f, sum / 255.0f);

			//fb->pix[c_i] = valToAssign.GetColor();
			cameraVector[c_i] = sum / 255.0f;
			fb->pix[c_i] = valToAssign.GetColor();
		}

		delete rowBuffer;
		cameraVectorRendered = true;
	}else{
		//Extra credit stuff
	}

	return true;
}

bool Light::applyTransposeLightTransportMatrixToCameraVector(FrameBuffer *fb){
	if(!cameraVectorRendered){
		return false;
	}
	
	cerr << "Transposing light transport matrix..." << endl;
	transposeLightTransportMatrix();
	saveSubsampledLightTransportMatrixAsImage("light_transport/LT_transpose.tiff");
	cerr << "Transposing Complete" << endl;

	if(grayscale){
		unsigned char * rowBuffer = new unsigned char[w*h];

		for(int v = 0; v < h; v++){
			for(int u = 0; u < w; u++){
				int c_i = v*w + u;

				for(int col = 0; col < w*h; col++){
					rowBuffer[col] = lightTransportMatrix[0][col][c_i];
				}

				unsigned int sum = 0;
				unsigned int count = 0;
				for(int l_i = 0; l_i < w*h; l_i++){
					if(rowBuffer[l_i] != 0){
						sum = sum + rowBuffer[l_i] * cameraVector[l_i];
						count++;
					}
				}

				if(count != 0){
					sum = sum / count;
				}

				Vector3D valToAssign = Vector3D(sum / 255.0f, sum / 255.0f, sum / 255.0f);

				//fb->pix[c_i] = valToAssign.GetColor();
				fb->Set(u,v,valToAssign.GetColor());
			}
		}

		delete rowBuffer;
	}else{
		//Extra credit stuff
	}

	return true;
}

bool Light::transposeLightTransportMatrix(){
	if(grayscale){
		int currRow = 0;
		int currCol = 0;

		unsigned char * rowBuffer = new unsigned char[w*h];
		unsigned char * colBuffer = new unsigned char[w*h];


		while(currRow < w*h && currCol < w*h){
			for(int col = currCol; col < w*h; col++){
				rowBuffer[col] = lightTransportMatrix[0][col][currRow];
			}
			for(int row = currRow; row < w*h; row++){
				colBuffer[row] = lightTransportMatrix[0][currCol][row];
			}

			for(int col = currCol; col < w*h; col++){
				lightTransportMatrix[0][col][currRow] = colBuffer[col];
			}

			for(int row = currRow; row < w*h; row++){
				lightTransportMatrix[0][currCol][row] = rowBuffer[row];
			}

			currRow++;
			currCol++;
		}

		delete rowBuffer;
		delete colBuffer;
	}else{
		//Extra credit stuff
	}

	return true;
}