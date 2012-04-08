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

void MeshObj::convert_to_triangles(void) {
  FaceItr i = _faces.begin();
  FaceItr e = --_faces.end();
  while(true) {
    if( i != e )
      face_to_triangles(*(i++));
    else {
      face_to_triangles(*i);
      break;
    }
  }
}

bool MeshObj::delete_face(uint32_t color) {
  Face* f = _color_to_face[color];
  if( f == NULL ) throw "MeshObj::delete_face(uint32_t): no face matched color.";
  
  // check deletability
  Edge* first = f->edge();  
  Edge* e = first;
  Edge* anchor = NULL;
  do {
    Edge* c = e;
    e = e->next();
    if( c->opp()->face() == NULL ) //edge on the boundary
      continue;
    else anchor = c;
    if( c->vert()->edge()->face() != NULL ) //vertex not on the boundary
      continue;
    if( c->next()->opp()->face() == NULL ) //next edge on the boundary
      continue;

    return false;
  } while( e != first );

  // point edges into the dark abyss
  Edge* n;
  if( anchor == NULL ) { // no adjacent faces exist
    do {
      n = e->next();
      _remove_vert(e->vert());
      _remove_edge(e->opp());
      _remove_edge(e);
      e = n;
    } while( e != first );
  }
  else {
    first = anchor;
    e = anchor;
    
    do {
      n = e->next();
      if( !e->external() ) {
	if( n != first && n->external() ) {
	  e->next() = e->vert()->edge();
	}
	else {
	  e->vert()->edge() = n;
	}
      }
      else {
	if( n != first && n->external() ) {
	  _remove_vert(e->vert());
	}
	else {
	  e->opp()->prev()->next() = n;
	  e->vert()->edge() = n;
	}
	_remove_edge(e->opp());
	_remove_edge(e);
      }
      e->face() = NULL;
      e = n;
    } while( e != first );
  }

  _color_to_face.erase(color);
  _face_to_color.erase(f);
  _remove_face(f);

  return true;
}

void MeshObj::subdivide_faces(void) {
  std::list<Vert*> new_verts;
  split_all_edges(new_verts);

  for( std::list<Vert*>::iterator v = new_verts.begin(); 
       v != new_verts.end(); v++ )
    {
      bisect_subdiv_triangle(*v, (*v)->edge()->opp()->next());
      if( (*v)->edge()->face() != NULL ) {
	bisect_subdiv_triangle(*v, (*v)->edge());
      }
    }
}

Vert* MeshObj::split_edge(Edge *e) {
  Edge* o = e->opp();
  Vert* v = new Vert((e->vert()->loc() + o->vert()->loc())/2);

  e->next() = new Edge(e->vert(), e->face(), e->next(), o);
  o->next() = new Edge(o->vert(), o->face(), o->next(), e);
  e->vert() = v;
  o->vert() = v;
  _edges.push_back(e->next());
  _edges.push_back(o->next());

  e->opp() = o->next();
  o->opp() = e->next();
  
  v->edge() = (o->face() == NULL) ? o->next() : e->next();
  v->normal() = v->calculate_normal();
  _verts.push_back(v);

  if( e->next()->opp()->next()->opp() != e )
    throw "MeshObj::split_edge(Edge*): invalid cycle.";

  return v;
}

void MeshObj::split_all_edges(list<Vert*>& v) {
  std::set<Edge*> edges(_edges.begin(), _edges.end());
  std::set<Edge*>::iterator itr;
  
  while( edges.size() > 0 ) {
    itr = edges.begin();
    edges.erase((*itr)->opp());
    v.push_back( split_edge(*itr) );
    edges.erase(itr);
  }
}

void MeshObj::bisect_subdiv_triangle(Vert* v, Edge* e0) {  
  Edge* e1 = e0->next();
  Edge* e2 = e1->next()->next();

  if( e2->next()->next()->next() == e0            //6 edges case
      && e2->next()->next()->vert() == v ) {
    e1 = e1->next();
    e2 = e2->next()->next();
  }
  else if( e2->next() != e0 || e2->vert() != v )  //not 4 edges case
    throw "MeshObj::bisect_subdiv_triangle(): unexpected surface.";

  e1->face()->edge() = e1;
  Face* f2 = new Face(e2);
  Edge* e3 = new Edge(e2->vert(), e1->face(), e2->next(), NULL);
  Edge* e4 = new Edge(e1->vert(),         f2, e1->next(),   e3);
  e3->opp() = e4;

  _edges.push_back(e3);
  _edges.push_back(e4);

  e1->next() = e3;
  e2->next() = e4;

  for( e2 = e4->next(); e2 != e4; e2 = e2->next() )
    e2->face() = f2;

  f2->normal() = f2->calculate_normal();
  _register_face(f2);
}

void MeshObj::_register_face(Face *f) {
  uint32_t key = (_color_to_face.size() == 0) ?
    1 : _color_to_face.rbegin()->first + 1;
  _color_to_face[key] = f;
  _face_to_color[f] = key;
  _faces.push_back(f);
}

void MeshObj::_remove_edge(Edge* e) {
  EdgeItr i = std::find(_edges.begin(), _edges.end(), e);
  _edges.erase(i);
}

void MeshObj::_remove_vert(Vert* v) {
  VertItr i = std::find(_verts.begin(), _verts.end(), v);
  _verts.erase(i);
}

void MeshObj::_remove_face(Face* f) {
  FaceItr i = std::find(_faces.begin(), _faces.end(), f);
  _faces.erase(i);
}

void MeshObj::face_to_triangles(uint32_t c) {
  face_to_triangles(_color_to_face[c]);
}

void MeshObj::face_to_triangles(Face *F0) {
  Edge* e0 = F0->edge();
  Vert* o  = e0->vert();

  while( e0->next()->next()->next() != e0 )
    {
      Edge* e1 = e0->next();
      Edge* e2 = e1->next();

      Face* f  = new Face(e1);

      //---------New Edge------- vert - face ------ next - opp -
      Edge* e3 = new Edge(          o,     f,         e1, NULL  );
      Edge* e4 = new Edge( e2->vert(),    F0, e2->next(), e3    );
      e0->next() = e4;
      e2->next() = e3;
      e3->opp()  = e4;
      e1->face() = f;
      e2->face() = f;

      f->normal() = f->calculate_normal();
      _register_face(f);
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
  _face_to_color.clear();

  for( int i=0; i<m.face_startidx.size(); i++ ) {

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

    face->normal() = face->calculate_normal();
    _register_face(face);
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
    edge_map_itr->second->opp()->next() = 
      edge_map_itr->second->opp()->vert()->edge();
    _edges.push_back(edge_map_itr->second->opp());
    //edge_map.erase(edge_map_itr);
  }
  edge_map.clear();

  _verts.insert(_verts.begin(), verts.begin(), verts.end());

  // computer per-vertex normals
  for( list<Vert*>::iterator vert_itr = _verts.begin(); 
       vert_itr != _verts.end(); vert_itr++ ) 
    {
      (*vert_itr)->normal() = (*vert_itr)->calculate_normal();
    }
}

///////////////////////////////////////////////////////////////////////////////
// class Edge

Edge::Edge() : _next(NULL), _opp(NULL), _face(NULL), _vert(NULL)
{  }

Edge::Edge( Vert* v, Face* f, Edge* n, Edge* o ) 
  : _vert(v), _next(n), _opp(o), _face(f)
{ }

Edge::~Edge() {  }

//getters
Edge* Edge::next(void) const  { return _next; }
Edge* Edge::opp (void) const  { return _opp;  }
Face* Edge::face(void) const  { return _face; }
Vert* Edge::vert(void) const  { return _vert; }

//setter return "this" pointer
Edge *& Edge::next(void)  { return _next; }
Edge *& Edge::opp (void)  { return _opp;  }
Face *& Edge::face(void)  { return _face; }
Vert *& Edge::vert(void)  { return _vert; }

Edge* Edge::prev(void) const {
  Edge* e;
  Vert* v = _opp->vert();
  for( e = _opp; e->_next != this; e = e->_next->_opp ) {
    if( e->vert() != v ) throw "Edge:prev(): unexpected vertex found.";
  }
  return e;
}

bool Edge::external(EdgeType t) {
  switch( t ) 
    {
    case HALF_EDGE:     return _face == NULL;
    case DOUBLE_EDGE:   return _face == NULL || _opp->_face == NULL;
    case OPPOSITE_EDGE: return _opp->_face == NULL;
    default: throw "Edge::on_border(Edge::EdgeType): invalid type provided.";
    }
}

std::ostream& operator << (std::ostream& s, const Edge& e) {
  s << *e.vert();    return s;
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
  Edge* nne = ne->next();           //next next edge
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

Vec3f Vert::calculate_normal(void) const {
  Vec3f n(0,0,0);
  list<Face*> faces = list_faces();
  for(list<Face*>::const_iterator face_itr = faces.begin();
      face_itr != faces.end(); face_itr++ )
    {
      n += (*face_itr)->normal();
    }
  return n;
}

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
  s << v._loc;  return s;
}

///////////////////////////////////////////////////////////////////////////////

bool MeshObj::validate(void) {
  bool ok = true;
  for( VertItr i = _verts.begin(); i != _verts.end(); i++ ) {
    Vert* v = *i;
    cout << "\nVert: " << v << " || ";

    // check vert's edge.opp points back to vert
    if( v->edge()->opp()->vert() != v ) {
      ok = false; cout << "x ";
    } else cout << ". ";
  }

  for( EdgeItr i = _edges.begin(); i != _edges.end(); i++ ) {
    Edge* e = *i;
    cout << "\nEdge: " << e << " || ";
    
    // check pointing to self
    if( e->next() == e ) {
      ok = false;  cout << "x ";
    } else cout << ". ";

    // check border's next
    if( e->face() == NULL && e->next()->face() != NULL ) {
      ok = false;  cout << "x ";
    } else cout << ". ";
    
    // check border's vert
    if( e->face() == NULL && e->vert()->edge()->face() != NULL ) {
      ok = false;  cout << "x ";
    } else cout << ". ";

    // check border's next = vert.edge
    if( e->face() == NULL && e->next() != e->vert()->edge() ) {
      ok = false;  cout << "x ";
    } else cout << ". ";

    // check opposites
    if( e->opp()->opp() != e ) {
      ok = false;  cout << "x ";
    } else cout << ". ";

    // check face == .next.face
    if( e->face() != e->next()->face() ) {
      ok = false;  cout << "x ";
    } else cout << ". ";

    if( e->face() == NULL ) { //|| e->opp()->face() == NULL ) {
      cout << ":";  std::flush(cout);
      for( Edge* t = e->next(); t->next() != e; t = t->next() ) 
      cout << ".";
    } else cout << "NA";
  }

  for( FaceItr i = _faces.begin(); i != _faces.end(); i++ ) {
    Face* f = *i;
    cout << "\nFace: " << f << " ";

    Edge* e = f->edge(); 
    do {
      if( e->face() != f ) { cout << "x"; ok = false; }
      else cout << ".";  
      std::flush(cout);
      e = e->next();
    } while(e != f->edge());
  }  
  cout << "\nface count: " << _faces.size();
  cout << "\nedge count: " << _edges.size();
  cout << "\nvert count: " << _verts.size();

  cout << endl;
  return ok;
}
