#include "Matrix3x3.h"
#include <cmath>
#include <iostream>

Matrix3x3::Matrix3x3(){
	rows[0] = Vector3D(1.0f,0.0f,0.0f);
	rows[1] = Vector3D(0.0f,1.0f,0.0f);
	rows[2] = Vector3D(0.0f,0.0f,1.0f);
}

Matrix3x3::Matrix3x3(char axis, float angle){

	float c = cos((angle*PI)/180.0f);
	float s = sin((angle*PI)/180.0f);

	if(axis == 'x'){
		rows[0] = Vector3D(1.0f,0.0f,0.0f);
		rows[1] = Vector3D(0.0f,c,-s);
		rows[2] = Vector3D(0.0f,s,c);
	}else if(axis == 'y'){
		rows[0] = Vector3D(c,0.0f,s);
		rows[1] = Vector3D(0.0f,1.0f,0.0f);
		rows[2] = Vector3D(-s,0.0f,c);
	}else if(axis == 'z'){
		rows[0] = Vector3D(c,-s,0.0f);
		rows[1] = Vector3D(s,c,0.0f);
		rows[2] = Vector3D(0.0f,0.0f,0.0f);
	}
}

Vector3D& Matrix3x3::operator[](int i){
	return rows[i];
}

Vector3D Matrix3x3::operator*(Vector3D v){
	Vector3D retval = Vector3D(rows[0]*v, rows[1]*v, rows[2]*v);
	return retval;
}

Matrix3x3 Matrix3x3::operator*(Matrix3x3 m){
	Matrix3x3 retval = Matrix3x3();
	
	for(int i = 0; i < 3; i++){
		retval.setColumn(i, (*this)*m.getColumn(i));
	}

	return retval;
}

Matrix3x3 Matrix3x3::operator*(float s){
	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			rows[i][j] *= s;
		}
	}

	return *this;
}

Matrix3x3 operator*(float s, Matrix3x3 m){
	return m * s;
}

ostream& operator<<(ostream &out, Matrix3x3 m){
	cout << m[0][0] << " " << m[0][1] << " " << m[0][2] << endl;
	cout << m[1][0] << " " << m[1][1] << " " << m[1][2] << endl;
	cout << m[2][0] << " " << m[2][1] << " " << m[2][2];

	return out;
}

istream& operator>>(istream &in, Matrix3x3 &m){
	in >> m[0][0]; in >> m[0][1]; in >> m[0][2];
	in >> m[1][0]; in >> m[1][1]; in >> m[1][2];
	in >> m[2][0]; in >> m[2][1]; in >> m[2][2];

	return in;
}

Vector3D Matrix3x3::getColumn(int i){
	Vector3D retval = Vector3D(rows[0][i], rows[1][i], rows[2][i]);
	
	return retval;
}

void Matrix3x3::setColumn(int i, Vector3D v){
	rows[0][i] = v[0];
	rows[1][i] = v[1];
	rows[2][i] = v[2];
}

float Matrix3x3::determinant(){
	float retval = 0;
	Matrix3x3 m = *this;

	retval = m.rows[0][0] * (rows[1][1]*rows[2][2] - rows[1][2]*rows[2][1]) 
		- m.rows[0][1] * (rows[1][0]*rows[2][2] - rows[1][2]*rows[2][0]) 
		+ m.rows[0][2] * (rows[1][0]*rows[2][1] - rows[1][1]*rows[2][0]);
	
	return retval;
}

Matrix3x3 Matrix3x3::inverse(){
	Matrix3x3 retval = Matrix3x3();

	//code provided by professor
	Vector3D a = getColumn(0), b = getColumn(1), c = getColumn(2);
	Vector3D _a = b ^ c;
	float dena = (a * _a);
	_a = _a / dena;
	Vector3D _b = c ^ a;
	float  denb = (b * _b);
	_b = _b / denb;
	Vector3D _c = a ^ b;
	float  denc = (c * _c);
	_c = _c / denc;
	retval[0] = _a;
	retval[1] = _b;
	retval[2] = _c;
	
	//code I created, is less efficient/elegant
	//retval = (1.0/determinant()) * adjoint();

	return retval;
}

Matrix3x3 Matrix3x3::transpose(){
	Matrix3x3 retval = copy();

	for(int i = 0; i < 3; i++){
		for(int j = i; j < 3; j++){
				retval[i][j] = rows[j][i];
				retval[j][i] = rows[i][j];
		}
	}

	return retval;
}

Matrix3x3 Matrix3x3::copy(){
	Matrix3x3 retval = Matrix3x3();

	for(int i = 0; i < 3; i++){
		for(int j = 0; j < 3; j++){
			retval[i][j] = rows[i][j];
		}
	}

	return retval;
}

Matrix3x3 Matrix3x3::adjoint(){
	Matrix3x3 retval = Matrix3x3();
	Matrix3x3 trans = transpose();

	retval[0][0] = trans[1][1]*trans[2][2] - trans[1][2]*trans[2][1];
	retval[0][1] = -1*(trans[1][0]*trans[2][2] - trans[1][2]*trans[2][0]);
	retval[0][2] = trans[1][0]*trans[2][1] - trans[1][1]*trans[2][0];

	retval[1][0] = -1*(trans[0][1]*trans[2][2] - trans[0][2]*trans[2][1]);
	retval[1][1] = trans[0][0]*trans[2][2] - trans[0][2]*trans[2][0];
	retval[1][2] = -1*(trans[0][0]*trans[2][1] - trans[0][1]*trans[2][0]);

	retval[2][0] = trans[0][1]*trans[1][2] - trans[0][2]*trans[1][1];
	retval[2][1] = -1*(trans[0][0]*trans[1][2] - trans[0][2]*trans[1][0]);
	retval[2][2] = trans[0][0]*trans[1][1] - trans[0][1]*trans[1][0];

	return retval;
}

void Matrix3x3::copy(Matrix3x3 a){
	rows[0].copy(a.rows[0]);
	rows[1].copy(a.rows[1]);
	rows[2].copy(a.rows[2]);
}