#include "io.h"



namespace View {
  static Vec3f CameraPosition(5,5,5);
  static Vec3f SphereCenter(0,0,0);
  static float SphereRadius = 2;
  static float ExaminerRotAngle = 0;
  static Vec3f ExaminerRotAxis(0,1,0);
  static HMatrix<float> ExaminerRotation;
};

//-----------------------------------------------------------------------------

Vec3f Input::CurrentPsphere;
Vec3f Input::NewPsphere;

void Input::Reshape(int width, int height) {
  Params::WindowWidth = width;
  Params::WindowHeight = height;

  glViewport(0,0,width,height);

  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity();
  gluPerspective(40, width/float(height), 1, 10);
}

Vec3f Input::ScreenToWorld(const int wind_id, int x, int y) { 
  glutSetWindow(wind_id);
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

/*
  center: center of the sphere (trackball)
  pscreen: point on screen
  psphere: destination: point on sphere?
 */
bool Input::SpherePoint(const Vec3f& center, float r, 
			const Vec3f& pscreen, Vec3f& psphere) { 
  Vec3f v = (pscreen- View::CameraPosition).dir(); 
  Vec3f d = View::CameraPosition-center;
  float ddotv = d.dot(v);
  float D = ddotv*ddotv-d.dot() +r*r;
  if (D < 0) return false;
  float t = -ddotv-sqrt(D);
  psphere = View::CameraPosition+v*t;
  return true;
}

void Input::MouseClick (int button, int state, int x, int y) {

  y = Params::WindowHeight - y-1;
  if(state == GLUT_DOWN) {
    Vec3f psphere; 
    if(SpherePoint(View::SphereCenter, View::SphereRadius,
		   ScreenToWorld(Params::MainWindow, x, y), psphere)) {
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

  y = Params::WindowHeight - y-1;
  Vec3f psphere;

  if(SpherePoint(View::SphereCenter, View::SphereRadius, 
		 ScreenToWorld(Params::MainWindow, x, y), psphere)) {
    Vec3f tmpRotAxis = cross(CurrentPsphere-View::SphereCenter, 
			     psphere-View::SphereCenter);
    View::ExaminerRotAxis = tmpRotAxis;
    View::ExaminerRotAngle = 
      acos((CurrentPsphere-View::SphereCenter)
	   .dot(psphere-View::SphereCenter)
	   / View::SphereRadius / View::SphereRadius);
    if( !(tmpRotAxis.x() == 0 && tmpRotAxis.y() == 0 && tmpRotAxis.z() == 0) ) 
      View::ExaminerRotation = 
	HMatrix<float>::Rotation(View::ExaminerRotAngle,
				 View::ExaminerRotAxis) 
	* View::ExaminerRotation;
    CurrentPsphere = psphere;
  }
  glutPostRedisplay();
}

void Input::Keyboard(unsigned char key, int x, int y) {
  if( key == 'n' )
    {
      Draw::toggle_mode(Draw::NORMALS_MODE);
    }
  
  glutPostRedisplay();
}

///////////////////////////////////////////////////////////////////////////////

void Draw::draw_scene() {
  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity();
  gluLookAt(View::CameraPosition.x(),
	    View::CameraPosition.y(),
	    View::CameraPosition.z(),
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
  glLightfv(GL_LIGHT0,GL_DIFFUSE,  Vec4f(1,  1,  1, 1));
  glLightfv(GL_LIGHT0,GL_POSITION, Vec4f(0,  0,  1, 0));
  glLightfv(GL_LIGHT1,GL_DIFFUSE,  Vec4f(1,  1,  1, 1));
  glLightfv(GL_LIGHT1,GL_POSITION, Vec4f(0,  0, -1, 0));

  glPushMatrix();
    glMultMatrixf(View::ExaminerRotation);
    glutWireSphere(View::SphereRadius,10,10);

    glEnable(GL_LIGHTING);
    
    for( list<Face*>::const_iterator f_itr = mesh_itr->faces().begin();
	 f_itr != mesh_itr->faces().end(); f_itr++ ) {

      Edge *first_e = (*f_itr)->edge();
      Edge *e_ptr = first_e;
      
      glBegin(GL_POLYGON);
        if( _DRAW_MODE & PER_FACE_NORMALS ) glNormal3fv((*f_itr)->normal());
	do {
	  if( _DRAW_MODE & PER_VERTEX_NORMALS ) 
	    glNormal3fv(e_ptr->vert()->normal());

	  if( e_ptr->next() == NULL ) throw "next null";
	  glVertex3fv(e_ptr->vert()->loc());
	  e_ptr = e_ptr->next();

	} while ( e_ptr != first_e );
      glEnd();
    }

    glDisable(GL_LIGHTING);
  glPopMatrix();

  glutSwapBuffers();
}

int Draw::get_mode(void) { return _DRAW_MODE; }

void Draw::set_mode(int bits) {
  if( bits & PER_FACE_NORMALS && bits & PER_VERTEX_NORMALS )
    throw "Draw::set_mode(int): Conflicting modes requested.";
  
  _DRAW_MODE = _DRAW_MODE | bits;

  if( bits & PER_FACE_NORMALS   ) _DRAW_MODE &= ~PER_VERTEX_NORMALS;
  if( bits & PER_VERTEX_NORMALS ) _DRAW_MODE &= ~PER_FACE_NORMALS;
}

void Draw::toggle_mode(int bits) {
  if( bits != NORMALS_MODE )
    throw "Draw::toggle_mode(int): Invalid mode requested.";
  
  _DRAW_MODE ^= bits;  //bitwise XOR assignment
}

std::list<MeshObj> Draw::meshes;
std::list<MeshObj>::iterator Draw::mesh_itr = Draw::meshes.begin();
int Draw::_DRAW_MODE = Draw::PER_FACE_NORMALS | Draw::PER_VERTEX_NORMALS;
