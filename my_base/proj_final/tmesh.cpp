#include "tmesh.h"
#include "ppc.h"
#include "Matrix3x3.h"
#include "aabb.h"
#include <fstream>
#include <cfloat>

void ComputeBBox(float** bbox, float x[], float y[]);
void ClipBBox(float** bbox,int w0, int w, int h0, int h);

void TMesh::SetDemoProjTexturedModel(Vector3D center){
	vertsN = 8;
	if(verts != NULL){
		free(verts);
	}

	verts = new Vector3D[vertsN];

	trisN = 4;
	if(tris != NULL){
		free(tris);
	}
	tris = new unsigned int[trisN*3];

	float dx = 75.0f;
	float dy = 75.0f;
	float dz = 75.0f;

	verts[0] = center + Vector3D(-dx, 0.0f, -dz);
	verts[1] = center + Vector3D(-dx, 0.0f, dz);
	verts[2] = center + Vector3D(dx, 0.0f, dz);
	verts[3] = center + Vector3D(dx, 0.0f, -dz);

	verts[4] = center + Vector3D(-dx/3.0f, dy/2.0f, 0.0f);
	verts[5] = center + Vector3D(-dx/3.0f, 0.0f, 0.0f);
	verts[6] = center + Vector3D(dx/3.0f, 0.0f, 0.0f);
	verts[7] = center + Vector3D(dx/3.0f, dy/2.0f, 0.0f);

	int tri = 0;

	tris[3*tri+0] = 0;
	tris[3*tri+1] = 1;
	tris[3*tri+2] = 2;
	tri++;
	tris[3*tri+0] = 2;
	tris[3*tri+1] = 3;
	tris[3*tri+2] = 0;
	tri++;
	tris[3*tri+0] = 4;
	tris[3*tri+1] = 5;
	tris[3*tri+2] = 6;
	tri++;
	tris[3*tri+0] = 6;
	tris[3*tri+1] = 7;
	tris[3*tri+2] = 4;
	tri++;

	cols = new Vector3D[vertsN];

	for(int i = 0; i < vertsN; i++){
		if(i < 4){
			cols[i] = Vector3D(1.0f, 0.0f, 0.0f);
		}else{
			cols[i] = Vector3D(0.0f, 1.0f, 0.0f);
		}
	}
}

void TMesh::setTexturedQuad(Vector3D center){
	vertsN = 4;
	if(verts){
		free(verts);
	}

	verts = new Vector3D[vertsN];

	trisN = 2;
	if(tris){
		free(tris);
	}
	tris = new unsigned int[trisN*3];

	float dx = 1000.0f;
	float dy = -40.0f;
	float dz = 1000.0f;

	verts[0] = center + Vector3D(-dx, dy, -dz);
	verts[1] = center + Vector3D(-dx, dy, dz);
	verts[2] = center + Vector3D(dx, dy, dz);
	verts[3] = center + Vector3D(dx, dy, -dz);

	int tri = 0;

	tris[3*tri+0] = 0;
	tris[3*tri+1] = 1;
	tris[3*tri+2] = 2;
	tri++;
	tris[3*tri+0] = 2;
	tris[3*tri+1] = 3;
	tris[3*tri+2] = 0;
	tri++;

	cols = new Vector3D[vertsN];

	for(int i = 0; i < vertsN; i++){
		cols[i] = Vector3D(1.0f, 0.0f, 0.0f);
	}

	st = new Vector3D[vertsN];

	int mult = 5;

	st[0] = mult*Vector3D(0.0f, 0.0f, 0.0f);
	st[1] = mult*Vector3D(0.0f, 1.0f, 0.0f);
	st[2] = mult*Vector3D(1.0f, 1.0f, 0.0f);
	st[3] = mult*Vector3D(1.0f, 0.0f, 0.0f);

	st_2D = new float[vertsN*2];

	for(int i = 0; i < vertsN; i++){
		st_2D[2*i] = st[i][0];
		st_2D[2*i+1] = st[i][1];
	}

	texID = 8;
	textured = true;
}

void TMesh::SetDemoTexturedModel(Vector3D center){
	vertsN = 24;
	if(verts != NULL){
		free(verts);
	}

	verts = new Vector3D[vertsN];

	trisN = 12;
	if(tris != NULL){
		free(tris);
	}
	tris = new unsigned int[trisN*3];

	int dx = 32;
	int dy = 50;
	int dz = 50;

	//top left front corner
	verts[0] = center + Vector3D(-dx, dy, dz);
	verts[1] = center + Vector3D(-dx, dy, dz);
	verts[2] = center + Vector3D(-dx, dy, dz);

	//bottom left front corner
	verts[3] = center + Vector3D(-dx, -dy, dz);
	verts[4] = center + Vector3D(-dx, -dy, dz);
	verts[5] = center + Vector3D(-dx, -dy, dz);

	//bottom right front corner
	verts[6] = center + Vector3D(dx, -dy, dz);
	verts[7] = center + Vector3D(dx, -dy, dz);
	verts[8] = center + Vector3D(dx, -dy, dz);

	//top right front corner
	verts[9] = center + Vector3D(dx, dy, dz);
	verts[10] = center + Vector3D(dx, dy, dz);
	verts[11] = center + Vector3D(dx, dy, dz);

	//top left back corner
	verts[12] = center + Vector3D(-dx, dy, -dz);
	verts[13] = center + Vector3D(-dx, dy, -dz);
	verts[14] = center + Vector3D(-dx, dy, -dz);

	//bottom left back corner
	verts[15] = center + Vector3D(-dx, -dy, -dz);
	verts[16] = center + Vector3D(-dx, -dy, -dz);
	verts[17] = center + Vector3D(-dx, -dy, -dz);

	//bottom right back corner
	verts[18] = center + Vector3D(dx, -dy, -dz);
	verts[19] = center + Vector3D(dx, -dy, -dz);
	verts[20] = center + Vector3D(dx, -dy, -dz);

	//top right back corner
	verts[21] = center + Vector3D(dx, dy, -dz);
	verts[22] = center + Vector3D(dx, dy, -dz);
	verts[23] = center + Vector3D(dx, dy, -dz);

	int tri = 0;

	tris[3*tri+0] = 0;
	tris[3*tri+1] = 3;
	tris[3*tri+2] = 6;
	tri++;
	tris[3*tri+0] = 6;
	tris[3*tri+1] = 9;
	tris[3*tri+2] = 0;
	tri++;
	tris[3*tri+0] = 10;
	tris[3*tri+1] = 7;
	tris[3*tri+2] = 18;
	tri++;
	tris[3*tri+0] = 18;
	tris[3*tri+1] = 21;
	tris[3*tri+2] = 10;
	tri++;
	tris[3*tri+0] = 22;
	tris[3*tri+1] = 19;
	tris[3*tri+2] = 15;
	tri++;
	tris[3*tri+0] = 15;
	tris[3*tri+1] = 12;
	tris[3*tri+2] = 22;
	tri++;
	tris[3*tri+0] = 13;
	tris[3*tri+1] = 16;
	tris[3*tri+2] = 4;
	tri++;
	tris[3*tri+0] = 4;
	tris[3*tri+1] = 1;
	tris[3*tri+2] = 13;
	tri++;
	tris[3*tri+0] = 14;
	tris[3*tri+1] = 2;
	tris[3*tri+2] = 11;
	tri++;
	tris[3*tri+0] = 11;
	tris[3*tri+1] = 23;
	tris[3*tri+2] = 14;
	tri++;
	tris[3*tri+0] = 5;
	tris[3*tri+1] = 17;
	tris[3*tri+2] = 20;
	tri++;
	tris[3*tri+0] = 20;
	tris[3*tri+1] = 8;
	tris[3*tri+2] = 5;
	tri++;

	cols = new Vector3D[vertsN];

	for(int i = 0; i < vertsN; i++){
		if(i % 2 == 0){
			cols[i] = Vector3D(0.0f, 0.0f, 1.0f);
		}else{
			cols[i] = Vector3D(1.0f, 0.0f, 0.0f);
		}
	}

	texsN = 6;

	texs = (FrameBuffer **) malloc(texsN*sizeof(FrameBuffer *));

	texs[0] = openTIFF("textures/front.tiff");
	texs[1] = openTIFF("textures/right.tiff");
	texs[2] = openTIFF("textures/back.tiff");
	texs[3] = openTIFF("textures/left.tiff");
	texs[4] = openTIFF("textures/top.tiff");
	texs[5] = openTIFF("textures/brick50.tiff");

	triToTexMap = new int[trisN];

	triToTexMap[0] = 0;
	triToTexMap[1] = 0;
	triToTexMap[2] = 1;
	triToTexMap[3] = 1;
	triToTexMap[4] = 2;
	triToTexMap[5] = 2;
	triToTexMap[6] = 3;
	triToTexMap[7] = 3;
	triToTexMap[8] = 4;
	triToTexMap[9] = 4;
	triToTexMap[10] = 5;
	triToTexMap[11] = 5;

	st = new Vector3D[vertsN];
	st_2D = new float[vertsN*2];

	//Front
	st[0] = Vector3D(0.0, 0.0, 0.0);
	st[3] = Vector3D(0.0, 1.0, 0.0);
	st[6] = Vector3D(1.0, 1.0, 0.0);
	st[9] = Vector3D(1.0, 0.0, 0.0);

	//Right
	st[10] = Vector3D(0.0, 0.0, 0.0);
	st[7] = Vector3D(0.0, 1.0, 0.0);
	st[18] = Vector3D(1.0, 1.0, 0.0);
	st[21] = Vector3D(1.0, 0.0, 0.0);

	//Back
	st[22] = Vector3D(0.0, 0.0, 0.0);
	st[19] = Vector3D(0.0, 1.0, 0.0);
	st[15] = Vector3D(1.0, 1.0, 0.0);
	st[12] = Vector3D(1.0, 0.0, 0.0);

	//Left
	st[13] = Vector3D(0.0, 0.0, 0.0);
	st[16] = Vector3D(0.0, 1.0, 0.0);
	st[4] = Vector3D(1.0, 1.0, 0.0);
	st[1] = Vector3D(1.0, 0.0, 0.0);

	//Top
	st[14] = Vector3D(0.0, 0.0, 0.0);
	st[2] = Vector3D(0.0, 1.0, 0.0);
	st[11] = Vector3D(1.0, 1.0, 0.0);
	st[23] = Vector3D(1.0, 0.0, 0.0);

	//Botom
	st[5] = Vector3D(0.0, 0.0, 0.0);
	st[17] = Vector3D(0.0, 4.0, 0.0);
	st[20] = Vector3D(4.0, 4.0, 0.0);
	st[8] = Vector3D(4.0, 0.0, 0.0);

	for(int i = 0; i < vertsN*2; i+=2){
		st_2D[i] = st[i][0];
		st_2D[i+1] = st[i][1];
	}
	
	textured = true;
	texRepetition = true;
}

void TMesh::SetSimpleQuad(Vector3D center, float sl){
	vertsN = 4;
	if(verts != NULL){
		free(verts);
	}
	verts = new Vector3D[vertsN];
	trisN = 2;
	if(tris != NULL){
		free(tris);
	}
	tris = new unsigned int[trisN*3];

	verts[0] = center + Vector3D(-sl/2.0, sl/2.0, 0);
	verts[1] = center + Vector3D(-sl/2.0, -sl/2.0, 0);
	verts[2] = center + Vector3D(sl/2.0, -sl/2.0, 0);
	verts[3] = center + Vector3D(sl/2.0, sl/2.0, 0);

	int tri = 0;
	tris[3*tri+0] = 0;
	tris[3*tri+1] = 1;
	tris[3*tri+2] = 2;
	tri++;
	tris[3*tri+0] = 2;
	tris[3*tri+1] = 3;
	tris[3*tri+2] = 0;

	cols = new Vector3D[vertsN];

	for(int i = 0; i < vertsN; i++){
		if(i % 2 == 0){
			cols[i] = Vector3D(0.0f, 0.0f, 1.0f);
		}else{
			cols[i] = Vector3D(1.0f, 0.0f, 0.0f);
		}
	}

	texsN = 1;

	texs = (FrameBuffer **) malloc(texsN*sizeof(FrameBuffer *));

	texs[0] = openTIFF("textures/brick50.tiff");

	//texs[0]->show();

	triToTexMap = new int[trisN];

	triToTexMap[0] = 0;
	triToTexMap[1] = 0;
	
	st = new Vector3D[vertsN];
	st[0] = Vector3D(0.0, 0.0, 0.0);
	st[1] = Vector3D(0.0, 2.0, 0.0);
	st[2] = Vector3D(2.0, 2.0, 0.0);
	st[3] = Vector3D(2.0, 0.0, 0.0);

	st_2D = new float[vertsN*2];

	for(int i = 0; i < vertsN; i++){
		st_2D[2*i] = st[i][0];
		st_2D[2*i+1] = st[i][1];

		//printf("%f %f\n", st_2D[i], st_2D[i+1]);
	}

	texID = 5;

	textured = true;
	texRepetition = true;
}

void TMesh::SetCube(Vector3D center, float sl){
	vertsN = 8;
	if(verts != NULL){
		free(verts);
	}
	verts = new Vector3D[vertsN];
	trisN = 12;
	if(tris != NULL){
		free(tris);
	}
	tris = new unsigned int[trisN*3];

	verts[0] = center + Vector3D(-sl/2.0, sl/2.0, sl/2.0);
	verts[1] = center + Vector3D(-sl/2.0, -sl/2.0, sl/2.0);
	verts[2] = center + Vector3D(sl/2.0, -sl/2.0, sl/2.0);
	verts[3] = center + Vector3D(sl/2.0, sl/2.0, sl/2.0);
	verts[4] = center + Vector3D(-sl/2.0, sl/2.0, -sl/2.0);
	verts[5] = center + Vector3D(-sl/2.0, -sl/2.0, -sl/2.0);
	verts[6] = center + Vector3D(sl/2.0, -sl/2.0, -sl/2.0);
	verts[7] = center + Vector3D(sl/2.0, sl/2.0, -sl/2.0);

	int tri = 0;

	tris[3*tri+0] = 0;
	tris[3*tri+1] = 1;
	tris[3*tri+2] = 2;
	tri++;
	tris[3*tri+0] = 2;
	tris[3*tri+1] = 3;
	tris[3*tri+2] = 0;
	tri++;
	tris[3*tri+0] = 3;
	tris[3*tri+1] = 2;
	tris[3*tri+2] = 6;
	tri++;
	tris[3*tri+0] = 6;
	tris[3*tri+1] = 7;
	tris[3*tri+2] = 3;
	tri++;
	tris[3*tri+0] = 7;
	tris[3*tri+1] = 6;
	tris[3*tri+2] = 5;
	tri++;
	tris[3*tri+0] = 5;
	tris[3*tri+1] = 4;
	tris[3*tri+2] = 7;
	tri++;
	tris[3*tri+0] = 4;
	tris[3*tri+1] = 5;
	tris[3*tri+2] = 1;
	tri++;
	tris[3*tri+0] = 1;
	tris[3*tri+1] = 0;
	tris[3*tri+2] = 4;
	tri++;
	tris[3*tri+0] = 4;
	tris[3*tri+1] = 0;
	tris[3*tri+2] = 3;
	tri++;
	tris[3*tri+0] = 3;
	tris[3*tri+1] = 7;
	tris[3*tri+2] = 4;
	tri++;
	tris[3*tri+0] = 1;
	tris[3*tri+1] = 5;
	tris[3*tri+2] = 6;
	tri++;
	tris[3*tri+0] = 6;
	tris[3*tri+1] = 2;
	tris[3*tri+2] = 1;
	tri++;

	cols = new Vector3D[vertsN];

	for(int i = 0; i < 8; i++){
		if(i % 2 == 0){
			cols[i] = Vector3D(0.0f, 0.0f, 0.0f);
		}else{
			cols[i] = Vector3D(1.0f, 0.0f, 0.0f);
		}
	}
}

void TMesh::Render(PPC * ppc, FrameBuffer *fb, PointLight *pl, bool wireframe, Envmap *env){
	if(trisN == 0 && vertsN != 0){
		RenderPoints(ppc,fb,3);
		return;
	}else if(trisN == 0 && vertsN == 0){
		return;
	}
	
	Vector3D *projVerts = new Vector3D[vertsN];

	for(int i = 0; i < vertsN; i++){
		projVerts[i] = Vector3D(FLT_MAX, FLT_MAX, FLT_MAX);
		ppc->Project(verts[i], projVerts[i]);
	}

	for(int i = 0; i < trisN; i++){
		int currTri = i;

		unsigned int vinds[3];
		vinds[0] = tris[3*i+0];
		vinds[1] = tris[3*i+1];
		vinds[2] = tris[3*i+2];

		if(wireframe){
			for(int j = 0; j < 3; j++){
				fb->DrawSegment(projVerts[vinds[j]], projVerts[vinds[(j+1)%3]], cols[vinds[j]], cols[vinds[(j+1)%3]]);
			}

			continue;
		}

		//Setup Rasterization

		//Create AABB
		AABB aabb = AABB(projVerts[vinds[0]]);
		aabb.AddPoint(projVerts[vinds[1]]);
		aabb.AddPoint(projVerts[vinds[2]]);

		if(!aabb.Clip(fb->w, fb->h)){
			continue;
		}

		int top = (int) (0.5f + aabb.corners[0][1]);
		int left = (int) (0.5f + aabb.corners[0][0]);
		int bottom = (int) (-0.5f + aabb.corners[1][1]);
		int right = (int) (-0.5f + aabb.corners[1][0]);

		//Edge equations
		Vector3D eeqs[3];

		for(int i = 0; i < 3; i++){
			int _i = (i+1)%3;

			eeqs[i][0] = projVerts[vinds[_i]][1] - projVerts[vinds[i]][1];
			eeqs[i][1] = projVerts[vinds[i]][0] - projVerts[vinds[_i]][0];
			eeqs[i][2] = projVerts[vinds[i]][1] * (-eeqs[i][1]) - projVerts[vinds[i]][0] * eeqs[i][0];

			int _i2 = (i+2)%3;

			Vector3D v3 = Vector3D(projVerts[vinds[_i2]][0], projVerts[vinds[_i2]][1], 1.0f);

			if(v3*eeqs[i] < 0){
				eeqs[i] = -1.0f * eeqs[i];
			}
		}

		//Screen Space Interpolation
		Matrix3x3 ssii = Matrix3x3();
		ssii[0] = projVerts[vinds[0]]; ssii[0][2] = 1.0f;
		ssii[1] = projVerts[vinds[1]]; ssii[1][2] = 1.0f;
		ssii[2] = projVerts[vinds[2]]; ssii[2][2] = 1.0f;
		ssii = ssii.inverse();

		//Color interpolation
		Matrix3x3 tcols = Matrix3x3();
		tcols[0] = cols[vinds[0]];
		tcols[1] = cols[vinds[1]];
		tcols[2] = cols[vinds[2]];

		if(gouraud){ //Apply lighting before creating interpolation matrix
			Vector3D toPL;
			Vector3D norm;

			Vector3D eyeV;
			Vector3D reflectedToPL;

			for(int i = 0; i < 3; i++){
				toPL = (pl->pos - verts[vinds[i]]).normalize();
				norm = (normals[vinds[i]]).normalize();

				eyeV = (ppc->C - verts[vinds[i]]).normalize();
				reflectedToPL = (2.0f * norm * (norm * toPL) - toPL).normalize();

				kdiff = toPL * norm;
				if(kdiff < 0.0f){
					kdiff = 0.0f;
				}

				float temp = reflectedToPL * eyeV;

				if(temp < 0){
					temp = 0;
				}

				kspec = pow(temp, phongExp);

				if(norm * toPL < 0){
					kspec = 0;
				}

				tcols[i] = tcols[i] * (kamb + (1.0f - kamb) * kdiff + kspec);
			}
		}

		Matrix3x3 colABCs = ssii*tcols;
		colABCs = colABCs.transpose();

		//Z buffer interpolation
		Vector3D tzs = Vector3D(projVerts[vinds[0]][2], projVerts[vinds[1]][2], projVerts[vinds[2]][2]);
		Vector3D zABC = ssii * tzs;

		//Normal interpolation
		
		Matrix3x3 tnormals = Matrix3x3();
		Matrix3x3 normalABCs = Matrix3x3();
		if(normals != NULL){
			tnormals[0] = normals[vinds[0]];
			tnormals[1] = normals[vinds[1]];
			tnormals[2] = normals[vinds[2]];
			normalABCs = ssii*tnormals;
			normalABCs = normalABCs.transpose();
		}

		//Model Space Interpolation
		Matrix3x3 camM;
		camM.setColumn(0,ppc->a);
		camM.setColumn(1,ppc->b);
		camM.setColumn(2,ppc->c);
		Matrix3x3 triM;
		triM.setColumn(0, verts[vinds[0]] - ppc->C);
		triM.setColumn(1, verts[vinds[1]] - ppc->C);
		triM.setColumn(2, verts[vinds[2]] - ppc->C);
		Matrix3x3 Q = triM.inverse() * camM;
		Vector3D DEF = Q[0] + Q[1] + Q[2];
		Vector3D ABCr = Vector3D(
			tcols.getColumn(0) * Q.getColumn(0), 
			tcols.getColumn(0) * Q.getColumn(1), 
			tcols.getColumn(0) * Q.getColumn(2));
		Vector3D ABCg = Vector3D(
			tcols.getColumn(1) * Q.getColumn(0), 
			tcols.getColumn(1) * Q.getColumn(1), 
			tcols.getColumn(1) * Q.getColumn(2)); 
		Vector3D ABCb = Vector3D(
			tcols.getColumn(2) * Q.getColumn(0), 
			tcols.getColumn(2) * Q.getColumn(1), 
			tcols.getColumn(2) * Q.getColumn(2));
		Vector3D ABCk = Vector3D(Q[1]);
		Vector3D ABCl = Vector3D(Q[2]);

		Matrix3x3 tst = Matrix3x3();
		Vector3D ABCs, ABCt;

		if(textured){
			tst[0] = st[vinds[0]];
			tst[1] = st[vinds[1]];
			tst[2] = st[vinds[2]];
			ABCs = Vector3D(
				tst.getColumn(0) * Q.getColumn(0), 
				tst.getColumn(0) * Q.getColumn(1), 
				tst.getColumn(0) * Q.getColumn(2));
			ABCt = Vector3D(
				tst.getColumn(1) * Q.getColumn(0), 
				tst.getColumn(1) * Q.getColumn(1), 
				tst.getColumn(1) * Q.getColumn(2));
		}


		//Rasterization
		for(int v = top; v <= bottom; v++){
			for(int u = left; u <= right; u++){
				Vector3D pixv = Vector3D(0.5f + (float)u, 0.5f + (float)v, 1.0f);
				if(pixv * eeqs[0] < 0.0f || pixv * eeqs[1] < 0.0f || pixv * eeqs[2] < 0.0f){
					continue;
				}
				if(((normalABCs*pixv).normalize() * ppc->GetVD())>=0){
					continue;
				}

				float currZ = zABC * pixv;
				Vector3D currP = Vector3D(pixv[0], pixv[1], currZ);
				if(!fb->CloserThenSet(currP)){
					continue;
				}

				Vector3D currC;
				
				//currC = colABCs * pixv; //SS Interpolation
				
				currC[0] = (ABCr*pixv) / (DEF*pixv); //MS Interpolation
				currC[1] = (ABCg*pixv) / (DEF*pixv);
				currC[2] = (ABCb*pixv) / (DEF*pixv);

				if(textured){
					Vector3D texel = Vector3D();

					texel[0] = (ABCs*pixv)/(DEF*pixv);
					texel[1] = (ABCt*pixv)/(DEF*pixv);


					FrameBuffer * currTexture = texs[triToTexMap[currTri]];

					if(texRepetition){
						texel[0] = (int)(texel[0]*currTexture->w) % currTexture->w;
						texel[1] = (int)(texel[1]*currTexture->h) % currTexture->h;
					}else if(texMirror){
						if((int)((texel[0]*currTexture->w) / currTexture->w) % 2 == 0){
							texel[0] = (int)(texel[0]*currTexture->w) % currTexture->w;
						}else{
							texel[0] = currTexture->w - 1 - ((int)(texel[0]*currTexture->w) % currTexture->w);
						}

						if((int)((texel[1]*currTexture->h) / currTexture->h) % 2 == 0){
							texel[1] = (int)(texel[1]*currTexture->h) % currTexture->h;
						}else{
							texel[1] = currTexture->h - 1 - ((int)(texel[1]*currTexture->h) % currTexture->h);
						}
					}

					currC = currTexture->GetV(texel[0],texel[1]);
				}else if(projTextured){
					int uv = (fb->h-1-v)*fb->w+u;
					Vector3D projP(0.5f + float(u), 0.5f + (float)v, currZ);
					Vector3D P = ppc->GetPoint(projP);  //Unprojecting point
					Vector3D refProjP;  //Projected point onto reference framebuffer

					if(!projPPC->Project(P,refProjP)){
						currC = Vector3D(1.0f, 0.0f, 1.0f);
					}else{
						int u0 = (int) refProjP[0];
						int v0 = (int) refProjP[1];

						if (u0 < 0 || u0 > projTM->w-1 || v0 < 0 || v0 > projTM->h-1){
							//currC = Vector3D(0.0f, 0.0f, 0.0f);
						}else{
							int uv0 = (projTM->h-1-v0)*projTM->w+u0;
							float eps = 0.01f;
							if(projTM->zb[uv0] > eps+refProjP[2]){  //If not in foreground on reference framebuffer
								currC = Vector3D(0.0f, 0.0f, 0.0f);
							}
							else{
								currC = projTM->GetV(u0, v0);
							}
						}
					}
				}

				if(phong){ //Interpolate normal, calculate lighting at individual point
					Vector3D currNorm = normalABCs * pixv;
					Vector3D toPL;
					Vector3D norm;

					Vector3D eyeV;
					Vector3D reflectedToPL;

					toPL = (pl->pos - ppc->GetPoint(currP)).normalize(); //Need to unproject currP
					norm = (currNorm).normalize(); //Using interpolated normal

					eyeV = (ppc->C - ppc->GetPoint(currP)).normalize(); //Need to unproject currP
					reflectedToPL = (2.0f * norm * (norm * toPL) - toPL).normalize();

					kdiff = toPL * norm;
					if(kdiff < 0.0f){
						kdiff = 0.0f;
					}

					float temp = reflectedToPL * eyeV;

					if(temp < 0){
						temp = 0;
					}

					kspec = pow(temp, phongExp);

					if(norm * toPL < 0){
						kspec = 0;
					}

					currC = currC * (kamb + (1.0f - kamb) * kdiff + kspec); //Applying lighting equation to the interpolated color
				}

				if(envmapReflection){
					/*Vector3D currNorm = normalABCs * pixv;
					Vector3D toPL;
					Vector3D norm;

					//Vector3D eyeV;
					Vector3D reflectedToPL;

					toPL = (ppc->C - ppc->GetPoint(currP)).normalize(); //Need to unproject currP
					norm = (currNorm).normalize(); //Using interpolated normal

					//eyeV = (ppc->C - ppc->GetPoint(currP)).normalize(); //Need to unproject currP
					reflectedToPL = (2.0f * norm * (norm * toPL) - toPL).normalize();

					unsigned int col = env->getColor(reflectedToPL);
					currC.SetFromColor(col);*/

					Vector3D norm = (normalABCs*pixv).normalize();
					Vector3D toEye = Vector3D();
					Vector3D reflected = Vector3D();

					toEye = (ppc->C - ppc->GetPoint(currP));

					reflected = (2.0f * (norm*toEye) * norm - toEye).normalize();

					unsigned int col = env->getColor(reflected);
					currC.SetFromColor(col);
				}
				
				fb->Set(u,v,currC.GetColor());
			}
		}
	}

	delete[] projVerts;
}

void TMesh::Rotate(Vector3D dir, float theta){
	if(theta == 0){
		return;
	}
	
	for(int i = 0; i < vertsN; i++){
		verts[i] = verts[i].rotate(Vector3D(), dir, theta);
	}
}

void TMesh::Rotate(Vector3D aO, Vector3D dir, float theta){
	if(theta == 0){
		return;
	}

	for(int i = 0; i < vertsN; i++){
		verts[i] = verts[i].rotate(aO, dir, theta);
	}
}

Vector3D TMesh::GetCenter(){
	Vector3D retval = Vector3D(0.0,0.0,0.0);

	for(int i = 0; i < vertsN; i++){
		retval = retval + verts[i];
	}

	return retval / (float) vertsN;
}

void TMesh::Load(char *fname) {

  ifstream ifs(fname, ios::binary);
  if (ifs.fail()) {
    cerr << "INFO: cannot open file: " << fname << endl;
    return;
  }

  ifs.read((char*)&vertsN, sizeof(int));
  char yn;
  ifs.read(&yn, 1); // always xyz
  if (yn != 'y') {
    cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
    return;
  }
  if (verts)
    delete verts;
  verts = new Vector3D[vertsN];

  ifs.read(&yn, 1); // cols 3 floats
  if (cols)
    delete cols;
  cols = 0;
  if (yn == 'y') {
    cols = new Vector3D[vertsN];
  }

  ifs.read(&yn, 1); // normals 3 floats
  //Vector3D *normals = 0; // don't have normals for now
  if (normals)
    delete normals;
  normals = 0;
  if (yn == 'y') {
    normals = new Vector3D[vertsN];
  }

  ifs.read(&yn, 1); // texture coordinates 2 floats
  float *tcs = 0; // don't have texture coordinates for now
  if (tcs)
    delete tcs;
  tcs = 0;
  if (yn == 'y') {
    tcs = new float[vertsN*2];
  }

  ifs.read((char*)verts, vertsN*3*sizeof(float)); // load verts

  if (cols) {
    ifs.read((char*)cols, vertsN*3*sizeof(float)); // load cols
  }

  if (normals)
    ifs.read((char*)normals, vertsN*3*sizeof(float)); // load normals

  if (tcs)
    ifs.read((char*)tcs, vertsN*2*sizeof(float)); // load texture coordinates

  ifs.read((char*)&trisN, sizeof(int));
  if (tris)
    delete tris;
  tris = new unsigned int[trisN*3];
  ifs.read((char*)tris, trisN*3*sizeof(unsigned int)); // read tiangles

  ifs.close();

  cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
  cerr << "      xyz " << ((cols) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;

  float max_y = FLT_MIN;
  float min_y = FLT_MAX;
  float max_z = FLT_MIN;
  float min_z = FLT_MAX;

  for(int i = 0; i < vertsN; i++){
	if(verts[i].coords[1] < min_y){
		min_y = verts[i].coords[1];
	}else if(verts[i].coords[1] > max_y){
		max_y = verts[i].coords[1];
	}

	if(verts[i].coords[2] < min_z){
		min_z = verts[i].coords[2];
	}else if(verts[i].coords[2] > max_z){
		max_z = verts[i].coords[2];
	}
  }

  float range_y = max_y - min_y;
  float range_z = max_z - min_z;

  //cout << min_y << endl;

  for(int i = 0; i < vertsN; i++){
	float scale_y = (verts[i].coords[1]-min_y)/range_y;
	float scale_z = (verts[i].coords[2]-min_z)/range_z;
	cols[i] = Vector3D(scale_y*scale_z, 0.0f, 0.0f);
  }
}

void TMesh::Translate(Vector3D transv){
	for (int vi = 0; vi < vertsN; vi++) {
		verts[vi] = verts[vi] + transv;
	}
}

void TMesh::ScaleAboutCenter(float s){
	Vector3D center = GetCenter();
	Translate(center*-1.0f);

	Scale(s);

	Translate(center);
}

void TMesh::Scale(float s){
	for(int i = 0; i < vertsN; i++){
		verts[i] = verts[i] * s;
	}
}

AABB TMesh::GetAABB(){
	AABB retval = AABB(verts[0]);

	for(int i = 0; i < vertsN; i++){
		retval.AddPoint(verts[i]);
	}

	return retval;
}

void TMesh::SetFromFB(FrameBuffer *fb, PPC *ppc){
	vertsN = fb->w*fb->h;

	if(verts != NULL){
		delete verts;
	}

	if(cols != NULL){
		delete cols;
	}
	
	verts = new Vector3D[vertsN];
	cols = new Vector3D[vertsN];

	int vi = 0;

	for(int v = 0; v < fb->h; v++){
		for(int u = 0; u < fb->w; u++){
			int uv = (fb->h-1-v)*fb->w+u;
			
			if(fb->zb[uv] == 0.0f){
				continue;
			}

			Vector3D projP(0.5f + (float)u, 0.5f + (float)v, fb->zb[uv]);
			verts[vi] = ppc->GetPoint(projP);
			cols[vi] = fb->GetV(u,v);
			vi++;
		}
	}

	vertsN = vi;
	trisN = 0;
}

void TMesh::RenderPoints(PPC *ppc, FrameBuffer *fb, int pSize){
	for(int vi = 0; vi < vertsN; vi++){
		Vector3D projV;

		if(!ppc->Project(verts[vi], projV)){
			continue;
		}

		for(int i = 0; i < pSize; i++){
			for(int j = 0; j < pSize; j++){
				Vector3D currP = projV - Vector3D((float)pSize/2, (float)pSize/2, 0.0f) + Vector3D((float)i, (float)j, 0.0f);
				
				if(!fb->CloserThenSet(currP)){
					continue;
				}

				fb->Set(currP, cols[vi].GetColor());
			}
		}
	}
}

FrameBuffer * TMesh::openTIFF(string filename){
	TIFF *FILE;

	if((FILE = TIFFOpen(filename.c_str(), "r")) == 0){
		return NULL;
	}

	//delete(fb);

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
	//fb->label(filename.c_str());
	//fb->show();

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

	//Render();

	return fb;
}

void TMesh::RenderHW(){
	if(wireframe){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	//Specify Vertices
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3,GL_FLOAT,0,verts);
	
	//Specify additional vertex attributes (eg color, normal, texture coord, etc)
	if(textured){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texID);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2,GL_FLOAT,0,st_2D);
	}else{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(3,GL_FLOAT,0,cols);
	}

	if(normals){
		glEnableClientState(GL_NORMAL_ARRAY);
		glNormalPointer(GL_FLOAT,0,normals);
	}

	//Draw mesh by providing connectivity data
	glDrawElements(GL_TRIANGLES, 3*trisN, GL_UNSIGNED_INT, tris);

	glDisableClientState(GL_VERTEX_ARRAY);

	if(wireframe){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if(normals){
		glDisableClientState(GL_NORMAL_ARRAY);
	}

	if(textured){
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_GEN_S);
		glDisable(GL_TEXTURE_2D);
	}else{
		glDisableClientState(GL_COLOR_ARRAY);
	}
}