#pragma once

#include "Vector3D.h"

#define PI 3.141592653589793f

using namespace std;

class Matrix3x3{
public:
	Vector3D rows[3];

	Matrix3x3();
	Matrix3x3(char axis, float angle);
	Vector3D& operator[](int i);
	Vector3D operator*(Vector3D v);
	Matrix3x3 operator*(Matrix3x3 m);
	Matrix3x3 operator*(float s);
	friend Matrix3x3 operator*(float s, Matrix3x3 m);
	friend ostream& operator<<(ostream &out, Matrix3x3 m);
	friend istream& operator>>(istream &in, Matrix3x3 &m);

	Vector3D getColumn(int i);
	void setColumn(int i, Vector3D v);

	float determinant();
	Matrix3x3 inverse();
	Matrix3x3 transpose();
	Matrix3x3 copy();
	Matrix3x3 adjoint();
};
