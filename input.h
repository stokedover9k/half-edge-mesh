#ifndef __INPUT_H__
#define __INPUT_H__

#include "params.h"

namespace Input {
  Vec3f CurrentPsphere;
  Vec3f NewPsphere;
  
  void Reshape(int width, int height);
  Vec3f ScreenToWorld(int windowid, int x, int y);
  bool SpherePoint(const Vec3f& center, float r,
		   const Vec3f& pscreen, Vec3f& psphere);
  void MouseClick (int button, int state, int x, int y);
  void MouseMotion(int x, int y);
};

void Input::Reshape(int width, int height) {
  Params::Window::WindowWidth = width;
  Params::Window::WindowHeight = height;

  glViewport(0,0,width,height);

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  gluPerspective(40, width/float(height), 1, 10);
}

Vec3f Input::ScreenToWorld(int windowid, int x, int y) { 
  glutSetWindow(windowid);
  GLdouble modelview[16];
  GLdouble projection[16];
  GLint viewport[4];
  double world_x, world_y, world_z; 
  // get current modelview, projection and viewport transforms
  glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
  glGetDoublev(GL_PROJECTION_MATRIX,projection);
  glGetIntegerv(GL_VIEWPORT,viewport);
  // this function computes inverse of VPM and applies it to (x,y,0) to convert from pixel to world coords
  // this computes the world coordinates of the point on the near plane of the frustum which corresponds to pixel (x,y)
  gluUnProject(x,y,0,modelview,projection,viewport, &world_x,&world_y,&world_z);
  return Vec3f(world_x,world_y,world_z);
}

bool Input::SpherePoint(const Vec3f& center, float r, 
			const Vec3f& pscreen, Vec3f& psphere) { 
  Vec3f v = (pscreen- Params::View::CameraPosition).dir(); 
  Vec3f d = Params::View::CameraPosition-center;
  float ddotv = d.dot(v);
  float D = ddotv*ddotv-d.dot() +r*r;
  if (D < 0) return false;
  float t = -ddotv-sqrt(D);
  psphere = Params::View::CameraPosition+v*t;
  return true;
}

void Input::MouseClick (int button, int state, int x, int y) {
  y = Params::Window::WindowHeight - y-1;
  if(state == GLUT_DOWN) {
    Vec3f psphere; 
    if(SpherePoint(Params::View::SphereCenter,Params::View::SphereRadius,ScreenToWorld(Params::Window::MainWindow,x,y),psphere)) {
      CurrentPsphere = psphere;
      NewPsphere = psphere;
    }
    glutPostRedisplay();
  } 
  if(state == GLUT_UP) { 
    CurrentPsphere = NewPsphere;
  }
}

void Input::MouseMotion(int x, int y) { 
  y = Params::Window::WindowHeight - y-1;
  Vec3f psphere;

  if(SpherePoint(Params::View::SphereCenter,Params::View::SphereRadius,ScreenToWorld(Params::Window::MainWindow,x,y),psphere)) {
    Vec3f tmpRotAxis = cross(CurrentPsphere-Params::View::SphereCenter, psphere-Params::View::SphereCenter);
    Params::View::ExaminerRotAxis = tmpRotAxis;
    Params::View::ExaminerRotAngle = acos((CurrentPsphere-Params::View::SphereCenter).dot(psphere-Params::View::SphereCenter)/Params::View::SphereRadius/Params::View::SphereRadius);
    if( tmpRotAxis.x() != 0 && tmpRotAxis.y() != 0 && tmpRotAxis.z() != 0 ) 
      Params::View::ExaminerRotation = HMatrix<float>::Rotation(Params::View::ExaminerRotAngle,Params::View::ExaminerRotAxis)*Params::View::ExaminerRotation;
    CurrentPsphere = psphere;
  }
  glutPostRedisplay();
}

#endif
