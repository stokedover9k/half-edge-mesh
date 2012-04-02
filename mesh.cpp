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

const std::vector<Edge*>& MeshObj::edges(void) const  { return _edges; }
const std::vector<Vert*>& MeshObj::verts(void) const  { return _verts; }
const std::vector<Face*>& MeshObj::faces(void) const  { return _faces; }

void MeshObj::construct(const MeshLoad::OBJMesh& m) {
  for( std::vector<Vec3f>::const_iterator i = m.pos.begin(); 
       i != m.pos.end(); i++ )
    _verts.push_back( new Vert(*i) );
  
  typedef pair<int, int> IndPair;
  map<IndPair, Edge*> edge_map;
  map<IndPair, Edge*>::iterator edge_map_itr;
  

  
  for( int i=0; i<m.face_startidx.size(); i++ ) {
    int endind = 
      ( i < m.face_startidx.size() - 1 ) 
      ? m.face_startidx[i+1] : m.faces.size();
    
    Face* face = new Face();
    Edge* first_edge = new Edge(_verts[m.faces[m.face_startidx[i]].posIdx],face);
    Edge* current_edge = first_edge;
    face->edge() = first_edge;

    int j;
    for( j = m.face_startidx[i] + 1; j<=endind; j++ ) 
      {
	int faces_ind = (j == endind) ? m.face_startidx[i] : j;
	current_edge->next() = ( j == endind ) ? 
	  first_edge : new Edge(_verts[m.faces[j].posIdx], face);
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

    face->normal() = face->calculate_normal();
    _faces.push_back(face);
  }

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

  // computer per-vertex normals
  for( vector<Vert*>::iterator vert_itr = _verts.begin(); 
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
