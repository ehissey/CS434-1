#pragma once

#include <iostream>
#include <ostream>
#include <istream>

using namespace std;

class Vector3D{
public:
	float coords[3];

	Vector3D();
	Vector3D(float x, float y, float z);

	float& operator[](int i);
	
	Vector3D normalize();
	float length();

	Vector3D operator*(float s);
	friend Vector3D operator*(float s, Vector3D v);
	Vector3D operator/(float s);
	
	Vector3D operator+(Vector3D v);
	Vector3D operator-(Vector3D v);
	Vector3D operator^(Vector3D v); //cross product
	float operator*(Vector3D v); //dot product

	friend ostream& operator<<(ostream &out, Vector3D v);
	friend istream& operator>>(istream &in, Vector3D & v);
	
	Vector3D rotate(Vector3D p1, Vector3D p2, float angle);
	Vector3D rotate(Vector3D p1, float angle);

	void SetFromColor(unsigned int color);
	unsigned int GetColor();

	void absolute();

	void copy(Vector3D a);
};