#ifndef __PARAMS_H__
#define __PARAMS_H__

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "cvec2t.h"
#include "cvec3t.h"
#include "cvec4t.h"
#include "hmatrix.h"

typedef CVec3T<float> Vec3f;
typedef CVec4T<float> Vec4f;

namespace Params {

  namespace Window {
    int WindowWidth = 800;
    int WindowHeight = 600;
    int MainWindow; 
  };

  namespace View {
    Vec3f CameraPosition(5,5,5);
    Vec3f SphereCenter(0,0,0);
    float SphereRadius = 2;
    float ExaminerRotAngle = 0; 
    Vec3f ExaminerRotAxis(0,1,0); 
    HMatrix<float> ExaminerRotation;
  };

};

#endif
