#include "io.h"
#include "mesh.h"
#include "mesh-loader.h"
#include "headers.h"
#include "params.h"



int main( int argc, char* argv[] ) {
  try 
    {
      Draw::set_mode(Draw::PER_FACE_NORMALS);
      
      MeshLoad::OBJMesh *m = MeshLoad::readOBJ("./obj/dodecahedron.obj");
      //MeshLoad::OBJMesh *m = MeshLoad::readOBJ("./obj/spaceship.obj");
      Draw::mesh = *m;
    }
  catch (const char* err_str) 
    {
      std::cerr << "ERROR: " << err_str << "\n" << "Terminating...(1)" << endl;
      exit(1);
    }

  // initialize glut and parse command-line aguments that glut understands
  glutInit(&argc, argv);

  // initialize dislay mode: 4 color components, double buffer and depth buffer
  glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);

  glutInitWindowSize(Params::WindowWidth, Params::WindowHeight);
  Params::MainWindow = glutCreateWindow("Trackball");

  glutDisplayFunc(Draw::draw_scene); 
  //glutDisplayFunc(Draw::draw_selectable); 
  glutReshapeFunc(Input::Reshape);
  glutMouseFunc(Input::MouseClick);
  glutMotionFunc(Input::MouseMotion);
  glutKeyboardFunc(Input::Keyboard);
 
  // this is an infinite loop get event - dispatch event which never returns
  try {
    glutMainLoop();
  }
  catch (const char* err_str) {
    std::cerr << "ERROR: " << err_str << "\n" << "Terminating...(2)" << endl;
    exit(2);
  }

  return 0;
}
