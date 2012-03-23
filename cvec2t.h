// -*- Mode: c++ -*-

#ifndef	__CVEC2T_H__
#define	__CVEC2T_H__

#include <float.h>
#include <iostream>
#include <assert.h>
#include <math.h>
using namespace std;

// typically F is float or double

template <class F> class CVec2T;

// read a vector from a stream
template <class F>
istream& operator>> ( istream& is, CVec2T<F>& v) {
  is >> v.v[0];
  is >> v.v[1];
  return is;
}

// used to determine if a vector is zero
template <class F> 
inline bool zero( F f ) { 
  return( f < F(DBL_EPSILON) );
}


template <class F>
class CVec2T {
public:
  enum{ X = 0, Y = 1 };

  CVec2T( void ) {}
  ~CVec2T( void ) {}

  // copy constructor	
  CVec2T( const CVec2T& c ) { v[X] = c.v[X], v[Y] = c.v[Y]; }
  // vector from 2 numbers
  CVec2T( const F& a, const F& b) { v[X] = a, v[Y] = b; }
  // vector from an array of 2 numbers	
  CVec2T( const F* a )  { v[X] = a[X], v[Y] = a[Y]; }
  // cast a vector to an array
  operator const F*( void ) const { return &v[X]; }

  
  CVec2T& operator=( const CVec2T& c )
    { v[X] = c.v[X], v[Y] = c.v[Y]; return *this; }

  // arithmetic operations +,-, multiplication by constant, division by constant
  CVec2T operator+( const CVec2T& c ) const
    { return CVec2T( v[X] + c.v[X], v[Y] + c.v[Y] ); }
  CVec2T operator-( const CVec2T& c ) const
    { return CVec2T( v[X] - c.v[X], v[Y] - c.v[Y]); }
  CVec2T operator*( const F& s ) const
    { return CVec2T( s * v[X], s * v[Y] ); }
  friend CVec2T operator*( const F& s, const CVec2T& c )
    { return CVec2T( s*c.v[X], s*c.v[Y] ); }
  CVec2T operator/( const F& s ) const
    { return CVec2T( v[X] / s, v[Y] / s ); }
  
  CVec2T& operator+=( const CVec2T& c )
    { v[X] += c.v[X], v[Y] += c.v[Y]; return *this; }
  CVec2T& operator-=( const CVec2T& c )
    { v[X] -= c.v[X], v[Y] -= c.v[Y]; return *this; }
  CVec2T& operator*=( const F& s )
    { v[X] *= s, v[Y] *= s; return *this; }
  CVec2T& operator/=( const F& s )
    { v[X] /= s, v[Y] /= s; return *this; }

  // unary minus
  CVec2T operator-( void ) const
    { return CVec2T( -v[X], -v[Y]); }

  // access components  
  F& x( void ) { return v[X]; }
  const F& x( void ) const { return v[X]; }
  F& y( void ) { return v[Y]; }
  const F& y( void ) const { return v[Y]; }
  F& operator() (const int i)      { return v[i]; }
  const F& operator() (const int i) const { return v[i]; }

  // *this + t(v1- *this )
  CVec2T lerp( const CVec2T& v1, const F& t ) const
    { return CVec2T( v[X] + t * ( v1.v[X] - v[X] ),
		   v[Y] + t * ( v1.v[Y] - v[Y] ) ); }

  // componentwise min and max
  CVec2T min( const CVec2T& o ) const
    { F a = ::min( v[X], o.v[X] );
      F b = ::min( v[Y], o.v[Y] );
      return CVec2T( a, b); }
  CVec2T max( const CVec2T& o ) const
    { F a = ::max( v[X], o.v[X] );
      F b = ::max( v[Y], o.v[Y] );
      return CVec2T( a, b); }

  F dot( const CVec2T& c ) const
    { return v[X] * c.v[X] + v[Y] * c.v[Y]; }
  F dot( void ) const
    { return v[X] * v[X] + v[Y] * v[Y]; }

  // norms: l1 norm |x|+|y|, l infinity norm max(|x|,|y|), 
  // l2 norm = length = sqrt(|x|^2 + |y|^2)
  F l1( void ) const
    { F a = fabs( v[X] ); a += fabs( v[Y] ); return a; }
  F linfty( void ) const
    { F a = fabs( v[X] ); a = ::max( a, F(abs( v[Y] )) );
      return  a; }
  F l2( void ) const { return sqrt( dot() ); }

  // unit vector with the same direction as this
  CVec2T dir( void ) const {
    F a = l2();
    if( zero( a ) ) return *this;
    else return *this / a;
  }

  // change length to unit, returns 0 if the vector is zero length
  int normalize( void )
    { F mag = l2(); return zero( mag ) ? 0 : ( *this *= F(1) / mag, 1 ); }

  void rotate(F angle) { 
    F tmp = cos(angle)*v[X] - sin(angle)*v[Y];
    v[Y] = cos(angle)*v[Y] + sin(angle)*v[X];
    v[X] = tmp;
  }

  // distance from *this to c
  F dist( const CVec2T& c ) const { return ( *this - c ).l2(); }
  
 protected:
 private:
  F v[2];
};



template <class F>
ostream& operator<<( ostream& os, const CVec2T<F>& v) {
  return os << v(0) << " " << v(1);
}


template <class F> 
inline CVec2T<F> min( const CVec2T<F>& a, const CVec2T<F>& b) {
  return CVec2T<F>( min( a(0), b(0)),  min( a(1), b(1)));
}

template <class F> 
inline CVec2T<F> max( const CVec2T<F>& a, const CVec2T<F>& b) {
  return CVec2T<F>( max( a(0), b(0)),  max( a(1), b(1)));
}

template <class F> 
inline CVec2T<F> abs( const CVec2T<F>& a ) {
  return CVec2T<F>( abs( a(0)),  abs( a(1)));
}




#endif	/* __CVEC2T_H__ */








