#ifndef	__CVEC4T_H__
#define	__CVEC4T_H__

#include "cvec2t.h"
#include "cvec3t.h"

template <class T> class CVec4T;

// another VC 6.0 problem: would not accept this definition 
// after the class

template <class T>
std::istream& operator>>(std::istream& is, CVec4T<T>& v) {
  is >> v.x() >> v.y() >> v.z() >> v.w();
  return is;
}

template <class T>
std::ostream& operator<<(std::ostream& os, const CVec4T<T>& v) {
  return os << v(0) << " " << v(1) << " " << v(2) << " " << v(3);
}


//: template for vectors of length 4
template <class T>
class CVec4T {
public:
  enum{ X = 0, Y = 1, Z = 2, W = 3 };

  CVec4T(int a = 0) { v[X] = a, v[Y] = a, v[Z] = a, v[W] = a; }
  CVec4T(const CVec4T& c) 
  { v[X] = c.v[X], v[Y] = c.v[Y], v[Z] = c.v[Z], v[W] = c.v[W]; }
  CVec4T(const T& a) { v[X] = a, v[Y] = a, v[Z] = a, v[W] = a; }
  CVec4T(const T& a, const T& b, const T& c, const T& d) 
  { v[X] = a, v[Y] = b, v[Z] = c, v[W] = d; }
  CVec4T(const T* a) { v[X] = a[X], v[Y] = a[Y], v[Z] = a[Z], v[W] = a[W]; }

  // from vector of length 2
  CVec4T(const CVec2T<T>& c, T z, T w)
  { v[X] = c.x(), v[Y] = c.y(), v[Z] = z, v[W] = w; }
  // from vector of length 3
  CVec4T(const CVec3T<T>& c, T w)
  { v[X] = c.x(), v[Y] = c.y(), v[Z] = c.z(), v[W] = w; }

  // type convertion to array of T
  operator T*( void ) { return &v[X]; }
  operator const T*( void ) const { return &v[X]; }
  operator CVec3T<T>( void ) const { return CVec3T<T>(&v[X]); }

  CVec4T& operator=( const CVec4T& c )
  { v[X] = c.v[X]; v[Y] = c.v[Y]; v[Z] = c.v[Z]; v[W] = c.v[W]; return *this; }
  CVec4T operator+( const CVec4T& c ) const
  { return CVec4T( v[X]+c.v[X], v[Y]+c.v[Y], v[Z]+c.v[Z], v[W]+c.v[W] ); }
  CVec4T operator-( const CVec4T& c ) const
  { return CVec4T( v[X]-c.v[X], v[Y]-c.v[Y], v[Z]-c.v[Z], v[W]-c.v[W] ); }
  CVec4T operator*( const T& s ) const
  { return CVec4T( s * v[X], s * v[Y], s * v[Z], s * v[W] ); }
  CVec4T operator*( const CVec4T& c ) const
  { return CVec4T( v[X]*c.v[X], v[Y]*c.v[Y], v[Z]*c.v[Z], v[W]*c.v[W] ); }
  friend CVec4T operator*( const T& s, const CVec4T& c )
  { return CVec4T( s * c.v[X], s * c.v[Y], s * c.v[Z], s * c.v[W] ); }
  CVec4T operator/( const T& s ) const
  { return CVec4T( v[X] / s, v[Y] / s , v[Z] / s, v[W] / s ); }  
  CVec4T& operator+=( const CVec4T& c )
  { v[X]+=c.v[X], v[Y]+=c.v[Y], v[Z]+=c.v[Z], v[W]+=c.v[W]; return *this; }
  CVec4T& operator-=( const CVec4T& c )
  { v[X]-=c.v[X], v[Y]-=c.v[Y], v[Z]-=c.v[Z], v[W]-=c.v[W]; return *this; }
  CVec4T& operator*=( const T& s )
  { v[X] *= s, v[Y] *= s, v[Z] *= s, v[W] *= s; return *this; }
  CVec4T& operator/=( const T& s )
  { v[X] /= s, v[Y] /= s, v[Z] /= s, v[W] /= s; return *this; }
  CVec4T operator-( void ) const
    { return CVec4T( -v[X], -v[Y], -v[Z], -v[W]); }

  void plus_aX(const T& a, const CVec4T& u) 
    { v[X] += a*u[X]; v[Y] += a*u[X]; v[Z] += a*u[Z]; v[W] += a*u[W]; }

  T& x( void ) { return v[X]; }
  const T& x( void ) const { return v[X]; }
  T& y( void ) { return v[Y]; }
  const T& y( void ) const { return v[Y]; }
  T& z( void ) { return v[Z]; }
  const T& z( void ) const { return v[Z]; }
  T& w( void ) { return v[W]; }
  const T& w( void ) const { return v[W]; }
  T& operator() (const int i)      { return v[i]; }
  const T& operator() (const int i) const { return v[i]; }
  //: returns v + t(v1 - v), where v is *this 
  CVec4T lerp( const CVec4T& v1, const T& t ) const
    { return CVec4T( v[X] + t * ( v1.v[X] - v[X] ),
		     v[Y] + t * ( v1.v[Y] - v[Y] ), 
		     v[Z] + t * ( v1.v[Z] - v[Z] ), 
		     v[W] + t * ( v1.v[W] - v[W] )); }
  //: returns the componentwise minimum
  CVec4T min( const CVec4T& o ) const {
    T a = ::min( v[X], o.v[X] );
    T b = ::min( v[Y], o.v[Y] );
    T c = ::min( v[Z], o.v[Z] );
    T d = ::min( v[W], o.v[W] );
    return CVec4T( a, b, c, d ); }
  //: returns the componentwise maximum
  CVec4T max( const CVec4T& o ) const {
    T a = ::max( v[X], o.v[X] );
    T b = ::max( v[Y], o.v[Y] );
    T c = ::max( v[Z], o.v[Z] );
    T d = ::max( v[W], o.v[W] );
    return CVec4T( a, b, c, d ); }

  T dot( const CVec4T& c ) const
  { return v[X] * c.v[X] + v[Y] * c.v[Y] + v[Z] * c.v[Z] + v[W] * c.v[W]; }
  T dot( void ) const
  { return v[X] * v[X] + v[Y] * v[Y] + v[Z] * v[Z] + v[W] * v[W]; }
  //: l1 norm (sum of abs values of the components)
  T l1( void ) const
  { return T(fabs( v[X] ) + fabs( v[Y] ) + fabs( v[Z] ) + fabs( v[W] )); }
  //: l infinity norm (max of abs values of the components)
  T linfty( void ) const {
    T a = fabs( v[X] ); a = ::max( a, T(fabs( v[Y] )) );
    a = ::max( a, T(fabs( v[Z] )) );
    a = ::max( a, T(fabs( v[W] )) );
    return  a; }
  //: l infinity norm (sqrt of the sum of squares )
  T l2( void ) const { return sqrt( dot() ); }
  T length( void ) const { return sqrt( dot() ); }
  //: for nonzero vectors, returns the normalized vector
  CVec4T dir( void ) const {
    T a = l2();
    if ( a  == T(0)) return *this;
    else return *this / a;
  }

  int normalize( void )
    { T mag = l2(); return (mag == T(0)) ? 0 : ( *this /= mag, 1 ); }
  //:  length |*this -c|
  T dist( const CVec4T& c ) const { return ( *this - c ).l2(); }

  CVec4T abs() {
    return CVec4T((T)fabs(v[X]), (T)fabs(v[Y]), (T)fabs(v[Z]), (T)fabs(v[W]));
  }

  //: conversion of different vectors 
  // note that it is a template    memeber function (parameterzed by G) of
  // the template  class parameterized by T; not all compilers can handle this
    template <class G> CVec4T( const CVec4T<G>& u ) { v[0] = u.x(); v[1] = u.y();  v[2] = u.z();  }

private:
  T v[4];
};


#endif	/* __CVEC4T_H__ */

