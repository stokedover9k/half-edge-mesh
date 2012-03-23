// -*- Mode: c++ -*-

#ifndef	__CVEC3T_H__
#define	__CVEC3T_H__

#include <float.h>
#include <iostream>
#include <assert.h>
#include <math.h>
//#include "cvec2t.h"

using namespace std;

// typically F is float or double

template <class F> class CVec3T;

// read a vector from a stream
template <class F>
istream& operator>> ( istream& is, CVec3T<F>& v) {
  is >> v.v[0];
  is >> v.v[1];
  is >> v.v[2];
  return is;
}


template <class F>
class CVec3T {
public:
  enum{ X = 0, Y = 1, Z = 2 };

  CVec3T( void ) {}

  // copy constructor
  CVec3T( const CVec3T& c )
  { v[X] = c.v[X], v[Y] = c.v[Y],v[Z] = c.v[Z]; }
  // vector from 3 numbers
  CVec3T( const F& a, const F& b, const F& c)
  { v[X] = a, v[Y] = b; v[Z] = c; }
  // vector from an array of 3 numbers	
  CVec3T( const F* a )
    { v[X] = a[X], v[Y] = a[Y], v[Z] = a[Z]; }
  // cast a vector to an array
  operator const F*( void ) const
    { return &v[X]; }


  ~CVec3T( void ) {}
  
  // arithmetic operations +,-, multiplication by constant, division by constant
  CVec3T& operator=( const CVec3T& c )
    { v[X] = c.v[X]; v[Y] = c.v[Y];v[Z] = c.v[Z]; return *this; }
  CVec3T operator+( const CVec3T& c ) const
    { return CVec3T( v[X] + c.v[X], v[Y] + c.v[Y], v[Z] + c.v[Z]); }
  CVec3T operator-( const CVec3T& c ) const
    { return CVec3T( v[X] - c.v[X], v[Y] - c.v[Y], v[Z] - c.v[Z] ); }
  CVec3T operator*( const F& s ) const
    { return CVec3T( s * v[X], s * v[Y], s*v[Z] ); }
  friend CVec3T operator*( const F& s, const CVec3T& c )
    { return CVec3T( s * c.v[X], s * c.v[Y], s * c.v[Z] ); }
  CVec3T operator/( const F& s ) const
    { return CVec3T( v[X] / s, v[Y] / s , v[Z] / s ); }
  
  CVec3T& operator+=( const CVec3T& c )
    { v[X] += c.v[X], v[Y] += c.v[Y]; v[Z] += c.v[Z]; return *this; }
  CVec3T& operator-=( const CVec3T& c )
    { v[X] -= c.v[X], v[Y] -= c.v[Y]; v[Z] -= c.v[Z]; return *this; }
  CVec3T& operator*=( const F& s )
    { v[X] *= s, v[Y] *= s,v[Z] *= s; return *this; }
  CVec3T& operator/=( const F& s )
    { v[X] /= s, v[Y] /= s, v[Z] /= s; return *this; }

  // unary minus
  CVec3T operator-( void ) const
    { return CVec3T( -v[X], -v[Y], -v[Z]); }
  
  // access components  
  F& x( void ) { return v[X]; }
  const F& x( void ) const { return v[X]; }
  F& y( void ) { return v[Y]; }
  const F& y( void ) const { return v[Y]; }
  F& z( void ) { return v[Z]; }
  const F& z( void ) const { return v[Z]; }
  F& operator() (const int i)      { return v[i]; }
  const F& operator() (const int i) const { return v[i]; }
  
  // *this + t(v1- *this )
  CVec3T lerp( const CVec3T& v1, const F& t ) const
  { return CVec3T( v[X] + t * ( v1.v[X] - v[X] ),
		   v[Y] + t * ( v1.v[Y] - v[Y] ), 
		   v[Z] + t * ( v1.v[Z] - v[Z])); }

  // componentwise min and max
  CVec3T min( const CVec3T& o ) const
    { F a = ::min( v[X], o.v[X] );
      F b = ::min( v[Y], o.v[Y] );
      F c = ::min( v[Z], o.v[Z] );
      return CVec3T( a, b,c); }
  CVec3T max( const CVec3T& o ) const
    { F a = ::max( v[X], o.v[X] );
      F b = ::max( v[Y], o.v[Y] );
      F c = ::max( v[Z], o.v[Z] );
      return CVec3T( a, b,c); }

  F dot( const CVec3T& c ) const
    { return v[X] * c.v[X] + v[Y] * c.v[Y]+ v[Z] * c.v[Z]; }
  F dot( void ) const
    { return v[X] * v[X] + v[Y] * v[Y] + v[Z] * v[Z]; }

  // norms: l1 norm |x|+|y|+|z|, l infinity norm max(|x|,|y|,|z|), 
  // l2 norm = length = sqrt(|x|^2 + |y|^2+|z|^2)
  F l1( void ) const
    { F a = fabs( v[X] ); a += fabs( v[Y] );a += fabs( v[Z] ); return a; }
  F linfty( void ) const
    { F a = fabs( v[X] ); a = ::max( a, F(abs( v[Y] )) );
    a = ::max( a, F(abs( v[Z] )) );
      return  a; }
  F l2( void ) const { return sqrt( dot() ); }
  
  // unit vector with the same direction as this
  CVec3T dir( void ) const {
    F a = l2();
    if( zero( a ) ) return *this;
    else return *this / a;
  }

  // change length to unit, returns 0 if the vector is zero length
  int normalize( void )
    { F mag = l2(); return zero( mag ) ? 0 : ( *this *= 1 / mag, 1 ); }

  // distance from *this to c
  F dist( const CVec3T& c ) const { return ( *this - c ).l2(); }

  
 protected:
 private:
  F v[3];
};



template <class F>
ostream& operator<<( ostream& os, const CVec3T<F>& v) {
  return os << v(0) << " " << v(1) << " " << v(2);
}


template <class F> 
inline CVec3T<F> min( const CVec3T<F>& a, const CVec3T<F>& b) {
  return CVec3T<F>( min( a(0), b(0)),  min( a(1), b(1)), min( a(2), b(2)) );
}

template <class F> 
inline CVec3T<F> max( const CVec3T<F>& a, const CVec3T<F>& b) {
  return CVec3T<F>( max( a(0), b(0)),  max( a(1), b(1)), max( a(2), b(2)));
}

template <class F> 
inline CVec3T<F> abs( const CVec3T<F>& a ) {
  return CVec3T<F>( abs( a(0)),  abs( a(1)), abs( a(2)));
}

// cross product of a and b
template <class F> 
inline CVec3T<F> cross( const CVec3T<F>& a, const CVec3T<F>& b )
{ return CVec3T<F>( a.y() * b.z() - a.z() * b.y(),
	       a.z() * b.x() - a.x() * b.z(),
	       a.x() * b.y() - a.y() * b.x() ); }



#endif	/* __CVEC3T_H__ */








