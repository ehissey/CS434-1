#include "Vector3D.h"
#include "Matrix3x3.h"
#include <cmath>

Vector3D::Vector3D(){
	coords[0] = 0.0f;
	coords[1] = 0.0f;
	coords[2] = 0.0f;
}

Vector3D::Vector3D(float x, float y, float z){
	coords[0] = x;
	coords[1] = y;
	coords[2] = z;
}

float& Vector3D::operator[](int i){
	return coords[i];
}

float Vector3D::operator*(Vector3D v){
	return coords[0]*v[0] + coords[1]*v[1] + coords[2]*v[2];
}

Vector3D Vector3D::operator*(float s){
	Vector3D retval = *this;

	retval[0] *= s;
	retval[1] *= s;
	retval[2] *= s;

	return retval;
}

Vector3D operator*(float s, Vector3D v){
	return v * s;
}

Vector3D Vector3D::operator/(float s){
	Vector3D retval = *this;

	retval[0] /= s;
	retval[1] /= s;
	retval[2] /= s;

	return retval;
}

Vector3D Vector3D::operator+(Vector3D v){
	Vector3D retval = Vector3D();

	retval[0] = coords[0] + v[0];
	retval[1] = coords[1] + v[1];
	retval[2] = coords[2] + v[2];

	return retval;
}

Vector3D Vector3D::operator-(Vector3D v){
	Vector3D retval = Vector3D();

	retval[0] = coords[0] - v[0];
	retval[1] = coords[1] - v[1];
	retval[2] = coords[2] - v[2];

	return retval;
}

ostream& operator<<(ostream &out, Vector3D v){
	out << v[0] << " " << v[1] << " " << v[2];

	return out;
}

istream& operator>>(istream &in, Vector3D & v){
	in >> v[0];
	in >> v[1];
	in >> v[2];

	return in;
}

Vector3D Vector3D::normalize(){
	Vector3D retval = *this / (*this).length();

	return retval;
}

float Vector3D::length(){
	return sqrt(pow(coords[0], 2) + pow(coords[1], 2) + pow(coords[2], 2));
}

Vector3D Vector3D::operator^(Vector3D v){
	Vector3D retval = Vector3D();

	retval[0] = coords[1]*v[2] - coords[2]*v[1];
	retval[1] = -1 * (coords[0]*v[2] - coords[2]*v[0]);
	retval[2] = coords[0]*v[1] - coords[1]*v[0];

	return retval;
}

Vector3D Vector3D::rotate(Vector3D p1, Vector3D p2, float angle){
	Vector3D retval = Vector3D();

	Vector3D Oa = p1;

	Vector3D a = p2.normalize();
	Vector3D b;

	if(abs(a*Vector3D(0,1,0)) <= abs(a*Vector3D(1,0,0))){
		b = (Vector3D(0,1,0)^a).normalize();
	}else{
		b = (Vector3D(1,0,0)^a).normalize();
	}

	Vector3D c = (a^b).normalize();

	Matrix3x3 transMatrix = Matrix3x3();
	transMatrix.rows[0] = a;
	transMatrix.rows[1] = b;
	transMatrix.rows[2] = c;

	Vector3D transVector = transMatrix * ((*this)-Oa);

	Matrix3x3 rotMatrix = Matrix3x3('x',angle);

	Vector3D rotVector = rotMatrix * transVector;

	retval = Oa + transMatrix.inverse() * rotVector;

	return retval;
}

Vector3D Vector3D::rotate(Vector3D p1, float angle){
	Vector3D retval = rotate(Vector3D(0,0,0), p1, angle);

	return retval;
}

unsigned int Vector3D::GetColor(){
	unsigned int rgb[3];
	float _xyz[3];

	for(int i = 0; i < 3; i++){
		if(coords[i] < 0.0){
			_xyz[i] = 0.0;
		}else if(coords[i] > 1.0){
			_xyz[i] = 1.0;
		}else{
			_xyz[i] = coords[i];
		}

		
	}

	rgb[0] = (int) (_xyz[0]*255.0);
	rgb[1] = (int) (_xyz[1]*255.0);
	rgb[2] = (int) (_xyz[2]*255.0);

	return 0xFF000000 + rgb[0] + rgb[1]*256 + rgb[2]*256*256;
}

void Vector3D::SetFromColor(unsigned int color){
	unsigned char *rgb = (unsigned char*)(&color);
	coords[0] = ((float)rgb[0])/255.0f;
	coords[1] = ((float)rgb[1])/255.0f;
	coords[2] = ((float)rgb[2])/255.0f;
}

void Vector3D::absolute(){
	for(int i = 0; i < 3; i++){
		coords[i] = abs(coords[i]);
	}
}