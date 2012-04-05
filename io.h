#ifndef __IO_H__
#define __IO_H__

#include <list>

#include "headers.h"
#include "mesh.h"

#ifndef __DEFAULT_COLORS__
#define __DEFAULT_COLORS__
#define DEFAULT_FACE_COLOR  Vec3f(0.8,0.8,0)
#define SELECTED_FACE_COLOR Vec3f(1,0,0)
#endif

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
  enum {
    NONE       = 0,
    TRACKBALL  = 1,
    SELECTED   = 1<<1,
    SELECTABLE = 1<<2,
  };

  //next two functions call draw_mesh() with right parameters
  /* draw_scene(): draws the lit and properly colored objects */
  static void draw_scene();
  /* draw_selectable(): draws faces with unique colors (doesn't swap buffers) */
  static void draw_selectable(); 

  static MeshObj mesh;
  
  static int get_mode(void);
  static void set_mode(int mode_bits);
  static void toggle_mode(int mode_bits);  //mode_bits must be NORMALS_MODE

 private:
  static int _DRAW_MODE;
  static void draw_mesh(int also_draw=NONE);
};

//-----------------------------------------------------------------------------

class Input {
 public:
  static uint32_t selected_face_color;
  static Face* selected_face;

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
