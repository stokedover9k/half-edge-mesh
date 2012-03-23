#include "params.h"
#include "input.h"
#include "draw.h"

int main(int argc, char* argv[]) {

  // initialize glut and parse command-line aguments that glut understands
  glutInit(&argc, argv);

  // initialize dislay mode: 4 color components, double buffer and depth buffer
  glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);

  glutInitWindowSize(Params::Window::WindowWidth,Params::Window::WindowHeight);
  Params::Window::MainWindow = glutCreateWindow("Trackball");

  glutDisplayFunc(Draw::draw_scene); 
  glutReshapeFunc(Input::Reshape);
  glutMouseFunc(Input::MouseClick);
  glutMotionFunc(Input::MouseMotion);
 
  // this is an infinite loop get event - dispatch event which never returns
  glutMainLoop();
  return 0;
}
