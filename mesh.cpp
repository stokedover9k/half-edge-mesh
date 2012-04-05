#include "mesh.h"

///////////////////////////////////////////////////////////////////////////////
// class MeshObj

void print_vert(const Vert& v) { cout << v.loc() << endl; }

MeshObj::MeshObj()
{  }

MeshObj::MeshObj(const MeshLoad::OBJMesh& m)
{
  construct(m);
}

MeshObj::MeshObj(const char* filename) {
  MeshLoad::OBJMesh *m = MeshLoad::readOBJ(filename);
  construct(*m);
  delete m;
}

const std::list<Edge*>& MeshObj::edges(void) const  { return _edges; }
const std::list<Vert*>& MeshObj::verts(void) const  { return _verts; }
const std::list<Face*>& MeshObj::faces(void) const  { return _faces; }

uint32_t MeshObj::color_to_i(const ColorVec4& c) {
  return c.x() | c.y()<<8 | c.z()<<16 | c.w()<<24; 
}

ColorVec4 MeshObj::i_to_color(uint32_t c) {
  return ColorVec4(c & 0xFF, c>>8 & 0xFF, c>>16 & 0xFF, c>>24 & 0xFF); 
}

uint32_t MeshObj::face_to_color(Face *face_ptr) const {
  return _face_to_color.find(face_ptr)->second; 
}

bool MeshObj::face_is_color(Face* f, uint32_t c) const {
  return _face_to_color.find(f)->second == c; 
}

Vert* MeshObj::split_edge(Edge *e) {
  Edge* o = e->opp();
  Vert* v = new Vert((e->vert()->loc() + o->vert()->loc())/2);

  e->next() = new Edge(e->vert(), e->face(), e->next(), o);
  o->next() = new Edge(o->vert(), o->face(), o->next(), o);
  e->vert() = v;
  o->vert() = v;
  _edges.push_back(e->next());
  _edges.push_back(o->next());

  e->opp() = o->next();
  o->opp() = e->next();
  
  v->edge() = o->face() == NULL ? o->next() : e->next();
  _verts.push_back(v);
  return v;
}

void MeshObj::_register_face(Face *f) {
  cout << "1 ";
  uint32_t key = _color_to_face.size() == 0 ? 1 : _color_to_face.rbegin()->first + 1;
  cout << "2 ";
  _color_to_face[key] = f;
  cout << "3 ";
  _face_to_color[f] = key;
  cout << "4 ";
  _faces.push_back(f);
  cout << "5 " << endl;
}

void MeshObj::face_to_triangles(uint32_t c) {
  face_to_triangles(_color_to_face[c]);
}

void MeshObj::face_to_triangles(Face *F0) {
  Edge* e0 = F0->edge();
  Vert* o  = e0->vert();

  int i=0;
  while( e0->next()->next()->next() != e0 )
    {
      cout << "splitting: " << ++i << endl;
      Edge* e1 = e0->next();
      Edge* e2 = e1->next();

      Face* f  = new Face(e2);
      //_faces.push_back(f);
      _register_face(f);

      //---------New Edge------- vert - face ------ next - opp -
      Edge* e3 = new Edge(          o,     f,         e1, NULL  );
      Edge* e4 = new Edge( e2->vert(),    F0, e2->next(), e3    );
      e3->opp()  = e4;
      e0->next() = e4;
      _edges.push_back(e3);
      _edges.push_back(e4);
    }
}

void MeshObj::construct(const MeshLoad::OBJMesh& m) {
  std::vector<Vert*> verts;
  for( std::vector<Vec3f>::const_iterator i = m.pos.begin(); 
       i != m.pos.end(); i++ )
    verts.push_back( new Vert(*i) );
  
  typedef pair<int, int> IndPair;
  map<IndPair, Edge*> edge_map;
  map<IndPair, Edge*>::iterator edge_map_itr;

  _color_to_face.clear();
  //_color_to_face = vector<Face*>(m.face_startidx.size()+1);
  //_color_to_face[0] = NULL;
  _face_to_color.clear();
  //ColorVec4 face_key(0,0,0,0);


  for( int i=0; i<m.face_startidx.size(); i++ ) {
    cout << "i: " << i << endl;

    int endind = 
      ( i < m.face_startidx.size() - 1 ) 
      ? m.face_startidx[i+1] : m.faces.size();
    
    Face* face = new Face();
    Edge* first_edge = new Edge(verts[m.faces[m.face_startidx[i]].posIdx],face);
    Edge* current_edge = first_edge;
    face->edge() = first_edge;

    int j;
    for( j = m.face_startidx[i] + 1; j<=endind; j++ ) 
      {
	cout << "j: " << j << endl;

	int faces_ind = (j == endind) ? m.face_startidx[i] : j;
	current_edge->next() = ( j == endind ) ? 
	  first_edge : new Edge(verts[m.faces[j].posIdx], face);
	current_edge->vert()->edge() = current_edge->next();
	_edges.push_back(current_edge);
	
	// if opposite is already in map
	IndPair opp_key(m.faces[faces_ind].posIdx, m.faces[j-1].posIdx);
	if( (edge_map_itr = edge_map.find( opp_key )) != edge_map.end() ) 
	  {
	    current_edge->next()->opp() = edge_map_itr->second;
	    edge_map_itr->second->opp() = current_edge->next();
	    edge_map.erase(edge_map_itr);  // erase opposite
	  } 
	else  // add edge to map 
	  edge_map[IndPair(opp_key.second, opp_key.first)] = current_edge->next();

	current_edge = current_edge->next();
      }

    // account for big endian and little endian differences
    /*
    uint32_t x = face_key.x();
    uint32_t y = face_key.y();
    uint32_t z = face_key.z();
    uint32_t w = face_key.w();
    */
    /*
    uint32_t i_face_key = 0;
    i_face_key = x | (y<<8) | (z<<16) | (w<<24);
    i_face_key += 20;
    */

    /*
    uint32_t i_face_key = (face_key.x()     |
			   face_key.y()<<8  |
			   face_key.z()<<16 |
			   face_key.w()<<24  ) + 1; //4632165;
    //*/
    /*
    face_key = ColorVec4(i_face_key & 0xFF,
			 i_face_key >>  8 & 0xFF,
			 i_face_key >> 16 & 0xFF,
			 i_face_key >> 24 & 0xFF);
    //*/
    /*
    face_key.x() = i_face_key & 0xFF;
    face_key.y() = i_face_key & (0xFF<<8);
    cout << (i_face_key & (0xFF << 8)) << "... ";
    cout << x << ","
	 << y << ","
	 << z << ","
	 << w << "  --  "
	 << i_face_key << endl;
    */
    /*
    cout << (int)face_key.x() << ","
	 << (int)face_key.y() << ","
	 << (int)face_key.z() << ","
	 << (int)face_key.w() << "  --  "
	 << i_face_key << endl;
    //*/

    face->normal() = face->calculate_normal();
    cout << "registering..." << endl;
    _register_face(face);
    cout << "registered" << endl;
    /*
    _color_to_face[i_face_key] = face;
    _face_to_color[face] = i_face_key;
    _faces.push_back(face);
    */
  }

  cout << "out of face loop" << endl;

  // handle boundaries
  // 1. create boundary edges, link opposites
  for( edge_map_itr = edge_map.begin(); 
       edge_map_itr != edge_map.end(); edge_map_itr++ ) 
    {
      Edge *a = edge_map_itr->second;
      Edge *b = a->next();
      while( b->next() != a) b = b->next();
      a->opp() = new Edge(b->vert());
      a->opp()->opp() = a;
      a->vert()->edge() = a->opp();
    }
  // 2. link boundary edges to next
  for( edge_map_itr = edge_map.begin();
       edge_map_itr != edge_map.end(); edge_map_itr++ ) {
    edge_map_itr->second->opp()->next() = edge_map_itr->second->vert()->edge();
    _edges.push_back(edge_map_itr->second);
    edge_map.erase(edge_map_itr);
  }

  _verts.insert(_verts.begin(), verts.begin(), verts.end());

  // computer per-vertex normals
  for( list<Vert*>::iterator vert_itr = _verts.begin(); 
       vert_itr != _verts.end(); vert_itr++ ) 
    {

      Vec3f normal(0,0,0);
      list<Face*> faces = (*vert_itr)->list_faces();
      for(list<Face*>::const_iterator face_itr = faces.begin();
	  face_itr != faces.end(); face_itr++ )
	{
	  normal += (*face_itr)->normal();
	}
      (*vert_itr)->normal() = normal.dir();
    }

  /*
  for(std::map<ColorVec4, Face*>::const_iterator map_itr = _color_to_face.begin();
      map_itr != _color_to_face.end(); map_itr++ ) {
    ColorVec4 c(map_itr->first);
    cout << (int)c.x() << "," << (int)c.y() << "," << (int)c.z() << "," << (int)c.w();
    cout << " " << map_itr->second << endl;
  }
  for(std::map<Face*, ColorVec4>::const_iterator map_itr = _face_to_color.begin();
      map_itr != _face_to_color.end(); map_itr++ ) {
    ColorVec4 c(map_itr->second);
    cout << (int)c.x() << "," << (int)c.y() << "," << (int)c.z() << "," << (int)c.w();
    cout << " " << map_itr->first << endl;
  }
  */
}

///////////////////////////////////////////////////////////////////////////////
// class Edge

Edge::Edge() : _next(NULL), _opp(NULL), _face(NULL), _vert(NULL)
{  }

Edge::Edge( Vert* v, Face* f, Edge* n, Edge* o ) 
  : _vert(v), _next(n), _opp(o), _face(f)
{  }

Edge::~Edge() {  }

//getters
Edge*   Edge::next(void) const  { return _next;   }
Edge*   Edge::opp (void) const  { return _opp;    }
Face*   Edge::face(void) const  { return _face;   }
Vert*   Edge::vert(void) const  { return _vert;   }

//setter return "this" pointer
Edge *& Edge::next(void)  { return _next; }
Edge *& Edge::opp (void)  { return _opp;  }
Face *& Edge::face(void)  { return _face; }
Vert *& Edge::vert(void)  { return _vert; }

std::ostream& operator << (std::ostream& s, const Edge& e) {
  s << *e.vert();
  return s;
}

///////////////////////////////////////////////////////////////////////////////
// class Face

Face::Face() : _edge(NULL)
{  }

Face::Face( Edge* e ) : _edge(e)
{  }

Face::~Face() {  }

Edge*        Face::edge  (void) const  { return _edge;   }
const Vec3f& Face::normal(void) const  { return _normal; }

Edge*& Face::edge  (void) { return _edge;   }
Vec3f& Face::normal(void) { return _normal; }

Vec3f Face::calculate_normal(void) const {
  Edge* ne = _edge->next();         //next edge
  Edge* nne = ne->next();            //next next edge
  Vec3f v1 = nne->vert()->loc() - ne->vert()->loc();
  Vec3f v2 = edge()->vert()->loc() - ne->vert()->loc();
  return cross(v1, v2);
}

unsigned int Face::edge_count(void) const {
  Edge *e;  int n = 1;
  for( e = _edge->next(); e != _edge; e = e->next() ) n++;
  return n;
}
  

///////////////////////////////////////////////////////////////////////////////
// class Vert

Vert::Vert() : _edge(NULL)
{  }

Vert::Vert(const Vec3f& v) : _edge(NULL), _loc(v)
{  }

Vert::~Vert() {  }

const Vec3f& Vert::loc   (void) const  { return _loc;    }
const Vec3f& Vert::normal(void) const  { return _normal; }
Edge*        Vert::edge  (void) const  { return _edge;   }

Vec3f& Vert::loc   (void) { return _loc;    }
Vec3f& Vert::normal(void) { return _normal; }
Edge*& Vert::edge  (void) { return _edge;   }

list<Face*> Vert::list_faces(void) const {
  list<Face*> l;
  Edge *first, *e;
  first = e = _edge;
  do {
    if( e->face() != NULL )  l.push_back(e->face());
    e = e->opp()->next();
  } while (e != first);
  return l;
}

std::ostream& operator << (std::ostream& s, const Vert& v) {
  s << v._loc;
  return s;
}
