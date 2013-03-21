#pragma once


#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "Vector3D.h"
#include "ppc.h"

class FrameBuffer : public Fl_Gl_Window{
public:
		unsigned int *pix;
		float *zb;
		int w, h;
		bool isHW;
		bool isRef;

		bool mouseCameraControl;
		bool keyboardCameraControl;

		int mouseX;
		int mouseY;

		FrameBuffer();
		FrameBuffer(int u0, int v0, int _w, int _h);
		void Set(unsigned int color);
		void SetZB(float z0);
		unsigned int Get(int u, int v);
		void Set(int u, int v, unsigned int color);
		void Set(Vector3D P, unsigned int color);
		bool CloserThenSet(Vector3D P);
		void SetGuarded(Vector3D P, unsigned int color);
		void draw();
		void DrawSegment3D(Vector3D p0, Vector3D p1, Vector3D c0, Vector3D c1, PPC *ppc);
		void DrawSegment(Vector3D p0, Vector3D p1, Vector3D c0, Vector3D c1);
		void DrawBoldedSegment(Vector3D p0, Vector3D p1, Vector3D c0, Vector3D c1);
		void Brighten(float s);
		void FindEdges();
		void FindEdges(FrameBuffer *edgeMap);
		Vector3D GetV(int u, int v);

		void openTIFF(string filename);
		void writeTIFF(string filename);

		int handle(int event);
		void KeyboardHandle();
		void MouseWheelHandle();
		void MouseDragHandle();

		void CopyFrom(FrameBuffer *fb);

		unsigned int * convertPixToGLFormat();

		~FrameBuffer();
};