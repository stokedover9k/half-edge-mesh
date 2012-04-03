#ifndef __IO_H__
#define __IO_H__

#include <list>

#include "headers.h"
#include "mesh.h"

using std::cout;
using std::cin;
using std::endl;

//-----------------------------------------------------------------------------

class Draw {
 public:
  enum {
    PER_FACE_NORMALS   = 1<<0,
    PER_VERTEX_NORMALS = 1<<1,

    NORMALS_MODE = PER_FACE_NORMALS|PER_VERTEX_NORMALS,
  };

  static void draw_scene();

  static std::list<MeshObj> meshes;
  static std::list<MeshObj>::iterator mesh_itr;

  static int get_mode(void);
  static void set_mode(int mode_bits);
  static void toggle_mode(int mode_bits);  //mode_bits must be NORMALS_MODE

 private:
  static int _DRAW_MODE;
};

//-----------------------------------------------------------------------------

class Input {
 public:
  static Vec3f CurrentPsphere;
  static Vec3f NewPsphere;

  static void Reshape(int width, int height);
  static Vec3f ScreenToWorld(int windowid, int x, int y);
  static bool SpherePoint(const Vec3f& center, float r,
			  const Vec3f& pscreen, Vec3f& psphere);
  static void MouseClick (int button, int state, int x, int y);
  static void MouseMotion(int x, int y);
  static void Keyboard(unsigned char key, int x, int y);
};

#endif
