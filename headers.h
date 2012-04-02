#ifndef __HEADERS_H__
#define __HEADERS_H__

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

#include "params.h"

#endif
