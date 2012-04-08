#ifndef __MESH_H__
#define __MESH_H__

#include <cstddef>         //for NULL
#include <algorithm>
#include <list>
#include <vector>
#include <utility>
#include <map>
#include <set>
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

  /* FACE-COLOR encoding interface */
  static uint32_t color_to_i(const ColorVec4& c);  //convert a color vec to an int
  static ColorVec4 i_to_color(uint32_t c);         //convert an int to a color vec
  uint32_t face_to_color(Face *) const;
  bool face_is_color(Face*, uint32_t) const;

  /* ALTERATION INTERFACE */
  void convert_to_triangles(void);
  bool delete_face(uint32_t color);   //returns true on success, false on failure
  void subdivide_faces(void);
  
  /* returns the new vector which splits the edge 
   * (automatically adds that vector to the mesh) 
   */
  Vert* split_edge(Edge *);

  /* splits all edges and puts the newly created vertices into the list (arg 1)
   */
  void split_all_edges(list<Vert*>&);

  /* Bisects the triangle on the Edge side of the Vertex.
   * Expects 6 or 4 sides to the figure.
   */
  void bisect_subdiv_triangle(Vert*, Edge*);

  void face_to_triangles(Face *);   //use the version with uint32_t arg instead
  void face_to_triangles(uint32_t);
  
  bool validate(void);

 private:
  typedef std::list<Vert*> VertContainer;
  typedef std::list<Edge*> EdgeContainer;
  typedef std::list<Face*> FaceContainer;
  typedef VertContainer::iterator         VertItr;
  typedef VertContainer::reverse_iterator VertRevItr;
  typedef EdgeContainer::iterator         EdgeItr;
  typedef EdgeContainer::reverse_iterator EdgeRevItr;
  typedef FaceContainer::iterator         FaceItr;
  typedef FaceContainer::reverse_iterator FaceRevItr;
  VertContainer _verts;
  EdgeContainer _edges;
  FaceContainer _faces;
  
  // faces are ID'd by a unique RGBA value stored as a CVec4T
  std::map<uint32_t, Face*> _color_to_face;
  std::map<Face*, uint32_t> _face_to_color;

  void _register_face(Face*);
  void _remove_edge(Edge*);
  void _remove_vert(Vert*);
  void _remove_face(Face*);

  void construct(const MeshLoad::OBJMesh &);

};

//-----------------------------------------------------------------------------

class Edge {
  enum EdgeType { HALF_EDGE, DOUBLE_EDGE, OPPOSITE_EDGE };
    
 public:
  Edge();
  Edge( Vert* v, Face* f=NULL, Edge* n=NULL, Edge* opp=NULL );
  ~Edge();

  //getters
  Edge* next(void) const;
  Edge*  opp(void) const;
  Face* face(void) const;
  Vert* vert(void) const;

  Edge* prev(void) const;

  bool external(EdgeType = DOUBLE_EDGE);

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
  unsigned int edge_count(void) const;

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

  Vec3f calculate_normal() const;

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
