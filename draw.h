#ifndef __DRAW_H__
#define __DRAW_H__

#include "params.h"

namespace Draw {
  void draw_scene();
};

void Draw::draw_scene() {
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity();
  gluLookAt(Params::View::CameraPosition.x(),
	    Params::View::CameraPosition.y(),
	    Params::View::CameraPosition.z(),
	    0,0,0,0,1,0);

  glClearColor( 0.6, 0.6,0.6,0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);
  // enable automatic rescaling of normals to unit length
  glEnable(GL_NORMALIZE);
  // enable two lights
  glEnable(GL_LIGHT0);
  //  glEnable(GL_LIGHT1);
  // directional lights (w=0) along z axis
  glLightfv(GL_LIGHT0,GL_DIFFUSE, Vec4f(1,  1, 1,1));
  glLightfv(GL_LIGHT0,GL_POSITION, Vec4f(0,  0, 1,0));
  glLightfv(GL_LIGHT1,GL_DIFFUSE,Vec4f(1,  1, 1,1));
  glLightfv(GL_LIGHT1,GL_POSITION, Vec4f(0,  0, -1,0));

  glPushMatrix();
  //cout << ExaminerRotation << endl;
  glMultMatrixf(Params::View::ExaminerRotation);
  glutWireSphere(Params::View::SphereRadius,10,10);
  glEnable(GL_LIGHTING);
  glutSolidTeapot(1.0);
  glDisable(GL_LIGHTING);
  glPopMatrix();

  glutSwapBuffers();
}

#endif
