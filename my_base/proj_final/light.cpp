#include "scene.h"
#include "light.h"

#include <sstream>


Light::Light(int width, int height, float hfov, Vector3D eye, Vector3D lookAt, PPC * cppc){
	enabled = true;
	grayscale = false;
	cameraVectorRendered = false;
	lightTransportMatrixCreated = false;
	lightTransportMatrixIsTransposed = false;

	lppc = new PPC(hfov, width, height);  //Set up directional light grid as a ppc

	lppc->PositionAndOrient(eye, lookAt - eye, Vector3D(0.0f, 1.0f, 0.0f), lppc->Getf(), *lppc);  //Look at the object

	lightVector = new float[width*height];
	cameraVector = new float[width*height];
	cameraVectorR = new float[width*height];
	cameraVectorG = new float[width*height];
	cameraVectorB = new float[width*height];

	int cSize = cppc->w*cppc->h;

	int TransportMatrixParameters = 3;  //1 matrix for greyscale, 3 matricies for RGB

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
	delete cameraVector;
	delete cameraVectorR;
	delete cameraVectorG;
	delete cameraVectorB;
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

	scene->writeTIFF("light_transport/INPUT_light_image.TIFF", lightFrameBuffer);

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

	cerr << "INFO: Loaded light vector: " << filename << endl;

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

			//lightTransportMatrix[0][col][i] = valToInsert;
			writeToLightTransportMatrix(0, col, i, valToInsert);
		}
	
	}else{
		//Extra credit stuff
		for(int i = 0; i < w*h; i++){
			Vector3D tempCol;
			tempCol.SetFromColor(fb->pix[i]);

			unsigned char valToInsertR = 0;
			unsigned char valToInsertG = 0;
			unsigned char valToInsertB = 0;

			if(tempCol.coords[0] < 0.001f && tempCol.coords[1] < 0.001f && tempCol.coords[2] < 0.001f){
				valToInsertR = 0;
				valToInsertG = 0;
				valToInsertB = 0;
			}else{
				valToInsertR = tempCol.coords[0] * 255.0f;
				valToInsertG = tempCol.coords[1] * 255.0f;
				valToInsertB = tempCol.coords[2] * 255.0f;
			}

			//lightTransportMatrix[0][col][i] = valToInsert;
			writeToLightTransportMatrix(0, col, i, valToInsertR);
			writeToLightTransportMatrix(1, col, i, valToInsertG);
			writeToLightTransportMatrix(2, col, i, valToInsertB);
		}
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
						//unsigned char temp = lightTransportMatrix[0][uprime][vprime];
						unsigned char temp = accessLightTransportMatrix(0, uprime, vprime);
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
		int LTFB_h = w*h / 4.0f;
		int LTFB_w = w*h / 4.0f;

		FrameBuffer * LTGrayScale = new FrameBuffer(0.0f, 0.0f, LTFB_w, LTFB_h);
		int pixv = 0;
		int pixu = 0;
		for(int v = 0; v < w*h; v+=4){
			pixu = 0;
			for(int u = 0; u < w*h; u+=4){
				Vector3D valToAssign = Vector3D(0.0f, 0.0f, 0.0f);
				unsigned char maxR = 0;
				unsigned char maxG = 0;
				unsigned char maxB = 0;

				//Find max value out of 4x4 block of values in LT matrix
				for(int vprime = v; vprime < v+4; vprime++){
					for(int uprime = u; uprime < u+4; uprime++){
						//unsigned char temp = lightTransportMatrix[0][uprime][vprime];
						unsigned char tempR = accessLightTransportMatrix(0, uprime, vprime);
						unsigned char tempG = accessLightTransportMatrix(1, uprime, vprime);
						unsigned char tempB = accessLightTransportMatrix(2, uprime, vprime);
						if(tempR > maxR){
							maxR = tempR;
						}
						if(tempG > maxG){
							maxG = tempG;
						}
						if(tempB > maxB){
							maxB = tempB;
						}
					}
				}

				valToAssign = Vector3D(maxR/255.0f, maxG/255.0f, maxB/255.0f);
				LTGrayScale->pix[(LTFB_h - 1 - pixv)*LTFB_w + pixu] = valToAssign.GetColor();
				//LTGrayScale->pix[pixv*LTFB_w + pixu] = valToAssign.GetColor();
				pixu++;
			}
			pixv++;
		}

		scene->writeTIFF(filename, LTGrayScale);

		delete LTGrayScale;
	}
	
	cerr << "INFO: Saved light transport: " << filename << endl;

	return true;
}

bool Light::applyLightTransportMatrixToLightVector(FrameBuffer *fb){
	if(lightTransportMatrixIsTransposed){
		transposeLightTransportMatrix();
	}
	
	if(grayscale){
		unsigned char * rowBuffer = new unsigned char[w*h];

		for(int c_i = 0; c_i < w*h; c_i++){
			for(int col = 0; col < w*h; col++){
				//rowBuffer[col] = lightTransportMatrix[0][col][c_i];
				rowBuffer[col] = accessLightTransportMatrix(0, col, c_i);
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
		unsigned char * rowBufferR = new unsigned char[w*h];
		unsigned char * rowBufferG = new unsigned char[w*h];
		unsigned char * rowBufferB = new unsigned char[w*h];

		for(int c_i = 0; c_i < w*h; c_i++){
			for(int col = 0; col < w*h; col++){
				//rowBuffer[col] = lightTransportMatrix[0][col][c_i];
				rowBufferR[col] = accessLightTransportMatrix(0, col, c_i);
				rowBufferG[col] = accessLightTransportMatrix(1, col, c_i);
				rowBufferB[col] = accessLightTransportMatrix(2, col, c_i);
			}

			unsigned int sumR = 0;
			unsigned int countR = 0;
			unsigned int sumG = 0;
			unsigned int countG = 0;
			unsigned int sumB = 0;
			unsigned int countB = 0;
			for(int l_i = 0; l_i < w*h; l_i++){
				if(rowBufferR[l_i] > 0.01f){
					sumR = sumR + rowBufferR[l_i] * lightVector[l_i];
					countR++;
				}
				if(rowBufferG[l_i] > 0.01f){
					sumG = sumG + rowBufferG[l_i] * lightVector[l_i];
					countG++;
				}
				if(rowBufferB[l_i] > 0.01f){
					sumB = sumB + rowBufferB[l_i] * lightVector[l_i];
					countB++;
				}
			}

			if(countR != 0){
				sumR = sumR / countR;
			}

			if(countG != 0){
				sumG = sumG / countG;
			}

			if(countB != 0){
				sumB = sumB / countB;
			}

			Vector3D valToAssign = Vector3D(sumR / 255.0f, sumG / 255.0f, sumB / 255.0f);

			//fb->pix[c_i] = valToAssign.GetColor();
			cameraVectorR[c_i] = sumR / 255.0f;
			cameraVectorG[c_i] = sumG / 255.0f;
			cameraVectorB[c_i] = sumB / 255.0f;
			fb->pix[c_i] = valToAssign.GetColor();
		}

		delete rowBufferR;
		delete rowBufferG;
		delete rowBufferB;
		cameraVectorRendered = true;
	}

	return true;
}

bool Light::applyTransposeLightTransportMatrixToCameraVector(FrameBuffer *fb){
	if(!cameraVectorRendered){
		cerr << "ERROR: Camera vector not rendered" << endl;
		
		return false;
	}
	
	if(!lightTransportMatrixIsTransposed){
		transposeLightTransportMatrix();
		//saveSubsampledLightTransportMatrixAsImage("light_transport/light_transport_transpose.tiff");
	}
	

	if(grayscale){
		unsigned char * rowBuffer = new unsigned char[w*h];

		for(int v = 0; v < h; v++){
			for(int u = 0; u < w; u++){
				int c_i = v*w + u;

				for(int col = 0; col < w*h; col++){
					//rowBuffer[col] = lightTransportMatrix[0][col][c_i];
					rowBuffer[col] = accessLightTransportMatrix(0, col, c_i);
				}

				unsigned int sum = 0;
				unsigned int count = 0;
				for(int l_i = 0; l_i < w*h; l_i++){
					if(rowBuffer[l_i] > 0.01f){
						sum = sum + rowBuffer[l_i] * cameraVector[l_i];
						count++;
					}
				}

				if(count != 0){
					sum = sum / count;
				}

				Vector3D valToAssign = Vector3D(sum / 255.0f, sum / 255.0f, sum / 255.0f);

				fb->Set(u,v,valToAssign.GetColor());
				//fb->pix[c_i] = valToAssign.GetColor();
			}
		}

		delete rowBuffer;
	}else{
		//Extra credit stuff
		unsigned char * rowBufferR = new unsigned char[w*h];
		unsigned char * rowBufferG = new unsigned char[w*h];
		unsigned char * rowBufferB = new unsigned char[w*h];

		for(int v = 0; v < h; v++){
			for(int u = 0; u < w; u++){
				int c_i = v*w + u;

				for(int col = 0; col < w*h; col++){
					//rowBuffer[col] = lightTransportMatrix[0][col][c_i];
					rowBufferR[col] = accessLightTransportMatrix(0, col, c_i);
					rowBufferG[col] = accessLightTransportMatrix(1, col, c_i);
					rowBufferB[col] = accessLightTransportMatrix(2, col, c_i);
				}

				unsigned int sumR = 0;
				unsigned int countR = 0;
				unsigned int sumG = 0;
				unsigned int countG = 0;
				unsigned int sumB = 0;
				unsigned int countB = 0;
				for(int l_i = 0; l_i < w*h; l_i++){
					if(rowBufferR[l_i] > 0.01f){
						sumR = sumR + rowBufferR[l_i] * cameraVectorR[l_i];
						countR++;
					}
					if(rowBufferG[l_i] > 0.01f){
						sumG = sumG + rowBufferG[l_i] * cameraVectorG[l_i];
						countG++;
					}
					if(rowBufferB[l_i] > 0.01f){
						sumB = sumB + rowBufferB[l_i] * cameraVectorB[l_i];
						countB++;
					}
				}

				if(countR != 0){
					sumR = sumR / countR;
				}

				if(countG != 0){
					sumG = sumG / countG;
				}

				if(countB != 0){
					sumB = sumB / countB;
				}

				Vector3D valToAssign = Vector3D(sumR / 255.0f, sumG / 255.0f, sumB / 255.0f);

				fb->Set(u,v,valToAssign.GetColor());
				//fb->pix[c_i] = valToAssign.GetColor();
			}
		}

		delete rowBufferR;
		delete rowBufferG;
		delete rowBufferB;
	}

	return true;
}

bool Light::transposeLightTransportMatrix(){
	cerr << "INFO: Begin transposing light transport matrix" << endl;
	/*
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
	*/
	lightTransportMatrixIsTransposed = !lightTransportMatrixIsTransposed;

	cerr << "INFO: Transposing light transport matrix complete" << endl;

	return true;
}

unsigned char Light::accessLightTransportMatrix(int matrixID, int col, int row){
	unsigned char retval = 0;

	if(lightTransportMatrixIsTransposed){
		retval = lightTransportMatrix[matrixID][row][col];
	}else{
		retval = lightTransportMatrix[matrixID][col][row];
	}

	return retval;
}

void Light::writeToLightTransportMatrix(int matrixID, int col, int row, unsigned char value){
	if(lightTransportMatrixIsTransposed){
		lightTransportMatrix[matrixID][row][col] = value;
	}else{
		lightTransportMatrix[matrixID][col][row] = value;
	}

}