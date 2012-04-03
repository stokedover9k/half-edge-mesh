#ifndef __MESH_H__
#define __MESH_H__

#include <cstddef>         //for NULL
#include <list>
#include <vector>
#include <utility>
#include <map>
#include "mesh-loader.h"
#include "headers.h"

using std::vector;
using std::list;
using std::map;
using std::pair;
using std::cout;
using std::endl;

class Edge;
class Face;
class Vert;

//-----------------------------------------------------------------------------

class MeshObj {
 public:
  MeshObj();
  MeshObj(const MeshLoad::OBJMesh& m);
  MeshObj(const char* filename);

  // getters for constant iterators to mesh elements
  const std::list<Edge*>& edges(void) const;
  const std::list<Vert*>& verts(void) const;
  const std::list<Face*>& faces(void) const;
  
 private:
  std::list<Vert*> _verts;
  std::list<Edge*> _edges;
  std::list<Face*> _faces;
  // faces are ID'd by a unique RGBA value stored as a CVec4T
  std::map<CVec4T<unsigned char>, Face*> _color_to_face;

  void construct(const MeshLoad::OBJMesh &);
};

//-----------------------------------------------------------------------------

class Edge {
 public:
  Edge();
  Edge( Vert* v, Face* f=NULL, Edge* n=NULL, Edge* opp=NULL );
  ~Edge();

  //getters
  Edge* next(void) const;
  Edge*  opp(void) const;
  Face* face(void) const;
  Vert* vert(void) const;

  //setter return "this" pointer
  Edge *& next(void);
  Edge *&  opp(void);
  Face *& face(void);
  Vert *& vert(void);

  friend std::ostream& operator << (std::ostream& s, const Edge& e);

 private:
  Edge *_next;
  Edge *_opp;
  Face *_face;
  Vert *_vert;
};

//-----------------------------------------------------------------------------

class Face {
 public:
  Face();
  Face( Edge* e );
  ~Face();

  //getters
  Edge*          edge(void) const;
  const Vec3f& normal(void) const;

  Vec3f calculate_normal(void) const;

  //setter 
  Edge*& edge(void);
  Vec3f& normal(void);

 private:
  Edge *_edge;
  Vec3f _normal;
};

//-----------------------------------------------------------------------------

class Vert {
 public:
  Vert();
  Vert(const Vec3f& v);
  ~Vert();

  //getter;
  const Vec3f& loc   (void) const;
  const Vec3f& normal(void) const;
  Edge*        edge  (void) const;

  //setter retruning "this" pointer
  Vec3f& loc   (void);
  Vec3f& normal(void);
  Edge*& edge  (void);

  //lists
  list<Face*> list_faces(void) const;
  
  friend std::ostream& operator << (std::ostream& s, const Vert& v);

 private:
  Vec3f _loc;
  Vec3f _normal;
  Edge *_edge;
};


#endif
