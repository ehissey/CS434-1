#include "ppc.h"
#include "framebuffer.h"

#include <cmath>
#include <fstream>

PPC::PPC(float hfov, int _w, int _h) : w(_w), h(_h){
	C = Vector3D(0.0f, 0.0f, 0.0f);
	a = Vector3D(1.0f, 0.0f, 0.0f);
	b = Vector3D(0.0f, -1.0f, 0.0f);
	this->hfov = hfov;

	hfovR = hfov / 180.0f * PI;
	c = Vector3D(-((float)w)/2.0f, ((float)h)/2.0f, -(float)w/(2.0f*tanf(hfovR/2.0f)));
	
	zNear = 1.0f;
	zFar = 10000.0f;

	frustum = 0;

	//setNearAndFarPoints();
	SetPMat();
}

bool PPC::Project(Vector3D P, Vector3D &projP){
	Vector3D Q = pMat*(P-C);

	if(Q[2] <= 0.0f){
		return false;
	}

	projP[0] = Q[0] / Q[2];
	projP[1] = Q[1] / Q[2];
	projP[2] = 1.0f / Q[2];

	return true;
}

void PPC::Translate(char dir, float ts){
	if(ts == 0){
		return;
	}

	switch(dir){
		case 'l':{
			C = C - a.normalize() * ts;
			break;
		}
		case 'r':{
			C = C + a.normalize() * ts;
			break;
		}
		case 'u':{
			C = C - b.normalize() * ts;
			break;
		}
		case 'd':{
			C = C + b.normalize() * ts;
			break;
		}
		case 'f':{
			C = C + GetVD().normalize() * ts;
			break;
		}
		case 'b':{
			C = C - GetVD().normalize() * ts;
			break;
		}
		default:{
			cerr << "Invalid key" << endl;
		}
	}

	//setNearAndFarPoints();
}

void PPC::Pan(float rs){
	if(rs == 0){
		return;
	}

	a = a.rotate(b.normalize()*-1.0f, rs);
	//b = b.rotate(b.normalize()*-1.0f, rs);
	c = c.rotate(b.normalize()*-1.0f, rs);

	//setNearAndFarPoints();
	SetPMat();
}
void PPC::Roll(float rs){
	if(rs == 0){
		return;
	}

	//a = a.rotate(a.normalize(), rs);
	b = b.rotate(a.normalize(), rs);
	c = c.rotate(a.normalize(), rs);

	//setNearAndFarPoints();
	SetPMat();
}
void PPC::Tilt(float rs){
	if(rs == 0){
		return;
	}

	a = a.rotate(GetVD().normalize(), rs);
	b = b.rotate(GetVD().normalize(), rs);
	c = c.rotate(GetVD().normalize(), rs);

	//setNearAndFarPoints();
	SetPMat();
}

Vector3D PPC::GetVD(){
	return (a^b).normalize();
}

float PPC::Getf(){
	return GetVD() * c;
}

void PPC::zoom(float s, char S){
	if(s == 0.0f){
		return;
	}

	if(S == 'o'){
		s = 1.0f/s;
	}

	float f = Getf();
	float newf = f*s;

	Vector3D newc = newf * GetVD() + (float) w / 2.0f * (-1.0f * a) + (float)h / 2.0f * (-1.0f * b);

	c = newc;

	//setNearAndFarPoints();
	SetPMat();
}

void PPC::PositionAndOrient(Vector3D newC, Vector3D newVD, Vector3D vinUP, float newf, PPC &ppc){
	ppc.w = w;
	ppc.h = h;
	ppc.zFar = zFar;
	ppc.zNear = zNear;
	ppc.C = newC;
	ppc.a = (newVD ^ vinUP).normalize() * a.length();
	Vector3D newUp = (ppc.a ^ newVD).normalize();
	ppc.b = newUp * -1.0F * b.length();
	ppc.c = newVD*newf - ppc.b*(float)h/2.0f - ppc.a*(float)w/2.0f;

	//setNearAndFarPoints();
	ppc.SetPMat();
}

PPC PPC::Interpolate(PPC * ppc0, PPC * ppc1, float frac){
	Vector3D newC = ppc0->C + (ppc1->C - ppc0->C)*frac;
	Vector3D anewb = ppc0->b + (ppc1->b - ppc0->b)*frac;
	float newf = ppc0->Getf() + (ppc1->Getf() - ppc0->Getf()) * frac;
	Vector3D vinUP = anewb *-1.0f;
	Vector3D newVD = (ppc0->GetVD() + (ppc1->GetVD() - ppc0->GetVD())*frac).normalize();

	PPC ret;
	PositionAndOrient(newC, newVD, vinUP, newf, ret);

	return ret;
}

void PPC::Visualize(PPC *ppc0, FrameBuffer *fb, float visF, int vLN){
	Vector3D P0, P1, c0(0.0f, 0.0f, 0.0f), c1(0.0f, 0.0f, 0.0f);

	float s = visF/Getf();

	P0 = C + c * s;
	P1 = P0 + a * (float)w*s;
	fb->DrawSegment3D(P0, P1, c0, c1, ppc0);

	P0 = P1;
	P1 = P0 + b * (float)h*s;
	fb->DrawSegment3D(P0, P1, c0, c1, ppc0);

	P0 = P1;
	P1 = P0 - a * (float)w*s;
	fb->DrawSegment3D(P0, P1, c0, c1, ppc0);

	P0 = P1;
	P1 = C + c * s;
	fb->DrawSegment3D(P0, P1, c0, c1, ppc0);

	P0 = C;
	P1 = C + c * s;
	fb->DrawSegment3D(P0, P1, c0 + Vector3D(1.0f, 0.0f, 0.0f), c1, ppc0);


}

Vector3D PPC::GetRay(float uf, float vf){
	return c + a*uf + b*vf;
}

Vector3D PPC::GetPoint(Vector3D projP){
	// projP[0] is "u"
	// projP[1] is "v"
	// projP[2] is "1/w"

	return C + GetRay(projP[0], projP[1])/projP[2];
}

Vector3D PPC::GetPoint(float uf, float vf, float z) {

  float f = Getf();
  float scf = z/f;
  Vector3D pointIP = C + a*uf + b*vf + c;
  Vector3D ret = C + (pointIP-C)*scf;
  return ret;

}

void PPC::Save(char *fname){

	ofstream ofs(fname);
	ofs << a << endl;
	ofs << b << endl;
	ofs << c << endl;
	ofs << C << endl;
	ofs << "// a b c C" << endl;
	ofs.close();
}


void PPC::Load(char *fname){

	ifstream ifs(fname);
	ifs >> a ;
	ifs >> b ;
	ifs >> c ;
	ifs >> C ;
	ifs.close();

	//setNearAndFarPoints();
	SetPMat();
}

void PPC::Print(){
	cout << "a: " << a << "\tb: " << b << "\tc: " << c << "\tC: " << C << "\tVD: " << GetVD() << endl;
}

void PPC::SetPMat(){
	Matrix3x3 cam;
	cam.setColumn(0, a);
	cam.setColumn(1, b);
	cam.setColumn(2, c);
	pMat = cam.inverse();
}

void PPC::SetIntrinsicsHW(){
	glViewport(0,0,w,h);
	float wf = a.length() * (float)w;
	float hf = b.length() * (float)h;
	float scf = zNear/Getf();
	float left = -wf/2.0f*scf;
	float right = +wf/2.0f*scf;
	float top = hf/2.0f*scf;
	float bottom = -hf/2.0f*scf;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(left, right, bottom, top, zNear, zFar);
	glMatrixMode(GL_MODELVIEW);
}

void PPC::SetExtrinsicsHW(){
	Vector3D LAP = C + GetVD();
	Vector3D up = (b*-1.0f).normalize();
	glLoadIdentity();
	gluLookAt(C[0], C[1], C[2], LAP[0], LAP[1], LAP[2], up[0], up[1], up[2]);
}

void PPC::RenderImageFrameGL() {

	float bf = 0.0f;
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	float zMed = (zNear + zFar) / 2.0f;
	Vector3D V;
	glBegin(GL_QUADS);
	V = GetPoint(bf+0.0f, bf+0.0f, zMed);
	glVertex3fv((float*)&V);
	V = GetPoint(bf+0.0f, (float)h-bf, zMed);
	glVertex3fv((float*)&V);
	V = GetPoint((float)w-bf, (float)h-bf, zMed);
	glVertex3fv((float*)&V);
	V = GetPoint((float)w-bf, bf+0.0f, zMed);
	glVertex3fv((float*)&V);
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

}

void PPC::copy(PPC *p){
	a.copy(p->a);
	b.copy(p->b);
	c.copy(p->c);
	C.copy(p->C);
	w = p->w;
	h = p->h;
	zNear = p->zNear;
	zFar = p->zFar;
	pMat.copy(p->pMat);
	hfov = p->hfov;
	hfovR = p->hfovR;
}

void PPC::setNearAndFarPoints(){
	//Vector3D cNear = (c.normalize()*zNear*GetVD().length())/(c.normalize()*GetVD());
	//Vector3D cFar = (c.normalize()*zFar*GetVD().length())/(c.normalize()*GetVD());

	if(!frustum){
		frustum = new Vector3D[8];
	}

	/*for(int i = 0; i < 3; i++){
		if(abs(a[i]) < 0.000001){
			a[i] = 0;
		}
		if(abs(b[i]) < 0.000001){
			b[i] = 0;
		}
		if(abs(c[i]) < 0.000001){
			c[i] = 0;
		}
		if(abs(C[i]) < 0.000001){
			C[i] = 0;
		}
	}*/

	Vector3D VDNear, VDFar;
	float wNear, wFar, hNear, hFar;
	Vector3D n0, n1, n2, n3, f0, f1, f2, f3;

	VDNear = GetVD() * zNear; //Vector from eye to center of near plane
	VDFar = GetVD() * zFar; //Vector from eye to center of far plane

	wNear = 2.0f*zNear*tan(hfovR/2.0f); //width of near plane
	wFar = 2.0f*zFar*tan(hfovR/2.0f); //width of far plane
	hNear = wNear*((float)h)/((float)w); //height of near plane
	hFar = wFar*((float)h)/((float)w); //height of far plane

	n0 = C + VDNear - (wNear/2.0f) * a.normalize() - (hNear/2.0f) * b.normalize(); //top left near
	n1 = n0 + hNear * b.normalize(); //bottom left near
	n2 = n1 + wNear * a.normalize(); //bottom right near
	n3 = n0 + wNear * a.normalize(); //top right near

	f0 = C + VDFar - (wFar/2.0f) * a.normalize() - (hFar/2.0f) * b.normalize(); //top left far
	f1 = f0 + hFar * b.normalize(); //bottom left far
	f2 = f1 + wFar * a.normalize(); //bottom right far
	f3 = f0 + wFar * a.normalize(); //top right far

	frustum[0] = n0;
	frustum[1] = n1;
	frustum[2] = n2;
	frustum[3] = n3;
	frustum[4] = f0;
	frustum[5] = f1;
	frustum[6] = f2;
	frustum[7] = f3;
}