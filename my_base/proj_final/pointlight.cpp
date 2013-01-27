#include "pointlight.h"

PointLight::PointLight(Vector3D _pos){
	pos = _pos;
	enabled = false;
}

void PointLight::Visualize(PPC * ppc, FrameBuffer *fb){
	Vector3D projP = Vector3D();

	ppc->Project(pos, projP);

	Vector3D col = Vector3D();

	for(int v = projP[1] - 3; v <= projP[1] + 3; v++){
		for(int u = projP[0] - 3; u <= projP[0] + 3; u++){
			Vector3D loc = Vector3D(u,v,projP[2]);
			
			if(!fb->CloserThenSet(loc)){
				continue;
			}

			fb->Set(loc, col.GetColor());
		}
	}
}