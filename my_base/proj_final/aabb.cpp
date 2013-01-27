#include "aabb.h"

AABB::AABB(Vector3D P){
	corners[0] = corners[1] = P;
}

void AABB::AddPoint(Vector3D P){
	for(int i = 0; i < 3; i++){
		if(P[i] < corners[0][i]){
			corners[0][i] = P[i];
		}

		if(P[i] > corners[1][i]){
			corners[1][i] = P[i];
		}
	}
}

bool AABB::Clip(int w, int h){
	if(corners[0][0] >= (float)w || corners[1][0] <= 0.0f || corners[0][1] >= (float)h || corners[1][1] <= 0.0f){
		return false;
	}

	if(corners[0][0] < 0.0f){
		corners[0][0] = 0.0f;
	}
	if(corners[1][0] > (float)w){
		corners[1][0] = (float)w;
	}
	if(corners[0][1] < 0.0f){
		corners[0][1] = 0.0f;
	}
	if(corners[1][1] > (float)h){
		corners[1][1] = (float)h;
	}

	return true;
}