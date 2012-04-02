#ifndef _HMATRIX_H_
#define _HMATRIX_H_

#include <string.h>
#include "cvec2t.h"
#include "cvec3t.h"
#include "cvec4t.h"


template <class F> 
class HMatrix {
 private:
  enum AxesNames { X = 0,Y = 1 ,Z = 2, W = 3};
 public:
  // make this an identity matrix
  HMatrix&  setIdentity() {     
    memset(_m,0, 16*sizeof(F));
    (*this)(0,0) = (*this)(1,1) = (*this)(2,2) = (*this)(3,3) = 1.0f;
    return *this;
  }
  // returns an identity matrix  
  static HMatrix Identity() {
    return HMatrix().setIdentity();
  }
  HMatrix() { 
    setIdentity();
  }
  // create from elements
  HMatrix(F m00, F m01, F m02, F m03,
	  F m10, F m11, F m12, F m13,
	  F m20, F m21, F m22, F m23,
	  F m30, F m31, F m32, F m33) { 
    HMatrix& M= *this;
    M(0,0) = m00; M(0,1) = m01; M(0,2) = m02; M(0,3) = m03;
    M(1,0) = m10; M(1,1) = m11; M(1,2) = m12; M(1,3) = m13;
    M(2,0) = m20; M(2,1) = m21; M(2,2) = m22; M(2,3) = m23;
    M(3,0) = m30; M(3,1) = m31; M(3,2) = m32; M(3,3) = m33;
  }
  HMatrix(const HMatrix& mat) { 
    memcpy(_m,mat._m,16*sizeof(F));
  }
  
  // cast to array
  // entries are in column-major order, i.e. first 4 elements are the first column
  operator F*() 
  { return _m; }
  // cast to const array
  operator const F*() const { 
    return _m;
  }
  // conversion from array to matrix: require to be explicit to avoid
  // unexpected implicit casts 
  explicit HMatrix(F* m ) { 
    memcpy(_m, m,16*sizeof(F));
  }
  // set to matrix for translation transform
  HMatrix& setTranslation( const CVec3T<F>& trans ) { 
    setIdentity(); 
    (*this)(0,3) = trans.x();
    (*this)(1,3) = trans.y();
    (*this)(2,3) = trans.z();
    return *this;
  }
  
  static HMatrix Translation( const CVec3T<F>& trans ) {
    return HMatrix().setTranslation(trans);
  }
  
  // set to matrix for nonuniform scale transform
  HMatrix& setScale( const CVec3T<F>& scale ) { 
    setIdentity(); 
    (*this)(0,0) = scale.x();
    (*this)(1,1) = scale.y();
    (*this)(2,2) = scale.z();
    return *this;
  }
  
  static HMatrix Scale( const CVec3T<F>& scale ) {
    return HMatrix().setScale(scale);
  }
  // set to a rotation matrix for axis v and angle a; unlike OpenGL the angle is in radians, not degrees!
  HMatrix& setRotation(F a,const CVec3T<F>& v) {
    //Vec3T<F> u = v.dir();
    CVec3T<F> u = v/v.l2();
    F u1 = u.x();
    F u2 = u.y();
    F u3 = u.z();
	 
    HMatrix U = HMatrix(        u1*u1, u1*u2, u1*u3, 0,
				u2*u1, u2*u2, u2*u3, 0,
				u3*u1, u3*u2, u3*u3, 0,
				0,     0,     0,     0);
    HMatrix S = HMatrix(        0,   -u3,  u2,  0,
				u3,    0, -u1,  0,
				-u2,  u1,   0,  0,
				0,     0,   0,  0);

    (*this) = U + (Identity() - U) * cos(a)  + S * sin(a);
    (*this)(3,3) = 1.0;
    return *this;
  }
  static HMatrix Rotation( F a,const CVec3T<F>& v) {
    return HMatrix().setRotation(a,v);
  }

  
  // (i,j) element access
  // this is the only function for which the internal storage order matters
  F operator()(int i, int j) const { 
    return _m[4*j + i ];
  }
  
  F& operator()(int i, int j)  { 
    return _m[4*j + i ];
  }
  
  HMatrix& operator=(const HMatrix& mat) { 
    memcpy(_m, mat._m, 16*sizeof(F));
    return *this;
  }

  // extract column
  CVec4T<F> col(int i) { 
    return CVec4T<F>( (*this)(0,i), (*this)(1,i), (*this)(2,i), (*this)(3,i));
  }
  // extract row
  CVec4T<F> row(int i) { 
    return CVec4T<F>( (*this)(i,0), (*this)(i,1), (*this)(i,2), (*this)(i,3));
  }
  
  
  // set the matrix to the inverse of M; return true if inverse exists  
  bool setInverse(const HMatrix& M) {
    HMatrix A;
    int i, j, k;
    F V;
    
    A = M;
    setIdentity();
	 
    
    for (i = 0; i < 4; i++) {
      V = A(i,i);				      /* Find the new pivot. */
      k = i;
      for (j = i + 1; j < 4; j++) 
	if (abs(A(j,i)) > abs(V)) {
	  /* Find maximum on col i, row i+1..n */
	  V = A(j,i);
	  k = j;
	}
      j = k;
		

      F tmp;
      if (i != j)
	for (k = 0; k < 4; k++) {
	  tmp = A(i,k); A(i,k) = A(j,k); A(j,k) = tmp;
	  tmp = (*this)(i,k); (*this)(i,k) = (*this)(j,k); (*this)(j,k) = tmp;
	}
		
		
      for (j = i + 1; j < 4; j++) {	 /* Eliminate col i from row i+1..n. */
	if(A(j,i) != 0) {
	  V = A(j,i) / A(i,i);
			 
	  for (k = 0; k < 4; k++) {
	    A(j,k)    -= V * A(i,k);
	    (*this)(j,k) -= V * (*this)(i,k);
	  }
	}
		  
      }
    }
	 
    for (i = 3; i >= 0; i--) {			       /* Back Substitution. */
      if (A(i,i) == 0)
	return false;					   /* Error. */
		
      for (j = 0; j < i; j++) {	 /* Eliminate col i from row 1..i-1. */
	V = A(j,i) / A(i,i);
		  
	for (k = 0; k < 4; k++) {
	  /* A[j][k] -= V * A[i][k]; */
	  (*this)(j,k) -= V * (*this)(i,k);
	}
      }
    }
    
    for (i = 0; i < 4; i++)		    /* Normalize the inverse Matrix. */
      for (j = 0; j < 4; j++)
	(*this)(i,j) /= A(i,i);
    
    return true;
  }  
  
  // return inverse; WARNING: returns garbage if the matrix is not invertible
  HMatrix inverse() const { 
    HMatrix M; M.setInverse(*this);
    return M;
  }
  // set the matrix to transpose of M
  HMatrix& setTranspose(const HMatrix& M )  { 
    for(int i = 0; i < 4; i++) 
      for(int j = 0; j < 4; j++) 
	(*this)(i,j)= M(j,i);
    return (*this);
  }

  HMatrix transpose() const { 
    HMatrix M; M.setTranspose(*this);
    return M;	
  }
  // matrix vector product Mv, vector regarded as a column  
  CVec4T<F> operator* (const CVec4T<F>& v) const { 
    const HMatrix& M = *this;
	 

    return CVec4T<F>( 
		     v.x() * M(0,0) + v.y() * M(0,1) + v.z() * M(0,2) + v.w()*M(0,3),
		     v.x() * M(1,0) + v.y() * M(1,1) + v.z() * M(1,2) + v.w()*M(1,3),
		     v.x() * M(2,0) + v.y() * M(2,1) + v.z() * M(2,2) + v.w()*M(2,3),
		     v.x() * M(3,0) + v.y() * M(3,1) + v.z() * M(3,2) + v.w()*M(3,3)
		      );
  }

  // scale a matrix
  HMatrix operator* (F s) const {
    const HMatrix& M = *this;
    return HMatrix(     s*M(0,0), s*M(0,1), s*M(0,2), s*M(0,3),
			s*M(1,0), s*M(1,1), s*M(1,2), s*M(1,3),
			s*M(2,0), s*M(2,1), s*M(2,2), s*M(2,3),
			s*M(3,0), s*M(3,1), s*M(3,2), s*M(3,3));
  }
  // multiply two matrices  
  HMatrix operator* (const HMatrix& M2) const {     
    const HMatrix& M1 = *this;
    return HMatrix(
		   M1(0,0)*M2(0,0) + M1(0,1)*M2(1,0) + M1(0,2)*M2(2,0) + M1(0,3)*M2(3,0),
		   M1(0,0)*M2(0,1) + M1(0,1)*M2(1,1) + M1(0,2)*M2(2,1) + M1(0,3)*M2(3,1),
		   M1(0,0)*M2(0,2) + M1(0,1)*M2(1,2) + M1(0,2)*M2(2,2) + M1(0,3)*M2(3,2),
		   M1(0,0)*M2(0,3) + M1(0,1)*M2(1,3) + M1(0,2)*M2(2,3) + M1(0,3)*M2(3,3),
						 
		   M1(1,0)*M2(0,0) + M1(1,1)*M2(1,0) + M1(1,2)*M2(2,0) + M1(1,3)*M2(3,0),
		   M1(1,0)*M2(0,1) + M1(1,1)*M2(1,1) + M1(1,2)*M2(2,1) + M1(1,3)*M2(3,1),
		   M1(1,0)*M2(0,2) + M1(1,1)*M2(1,2) + M1(1,2)*M2(2,2) + M1(1,3)*M2(3,2),
		   M1(1,0)*M2(0,3) + M1(1,1)*M2(1,3) + M1(1,2)*M2(2,3) + M1(1,3)*M2(3,3),
						 
		   M1(2,0)*M2(0,0) + M1(2,1)*M2(1,0) + M1(2,2)*M2(2,0) + M1(2,3)*M2(3,0),
		   M1(2,0)*M2(0,1) + M1(2,1)*M2(1,1) + M1(2,2)*M2(2,1) + M1(2,3)*M2(3,1),
		   M1(2,0)*M2(0,2) + M1(2,1)*M2(1,2) + M1(2,2)*M2(2,2) + M1(2,3)*M2(3,2),
		   M1(2,0)*M2(0,3) + M1(2,1)*M2(1,3) + M1(2,2)*M2(2,3) + M1(2,3)*M2(3,3),
						 
		   M1(3,0)*M2(0,0) + M1(3,1)*M2(1,0) + M1(3,2)*M2(2,0) + M1(3,3)*M2(3,0),
		   M1(3,0)*M2(0,1) + M1(3,1)*M2(1,1) + M1(3,2)*M2(2,1) + M1(3,3)*M2(3,1),
		   M1(3,0)*M2(0,2) + M1(3,1)*M2(1,2) + M1(3,2)*M2(2,2) + M1(3,3)*M2(3,2),
		   M1(3,0)*M2(0,3) + M1(3,1)*M2(1,3) + M1(3,2)*M2(2,3) + M1(3,3)*M2(3,3)
		   );
  }
  
  HMatrix operator+ (const HMatrix& M2) const {
    const HMatrix& M1 = *this;
    return HMatrix(
		   M1(0,0)+M2(0,0), M1(0,1)+M2(0,1), M1(0,2)+M2(0,2), M1(0,3)+M2(0,3),
		   M1(1,0)+M2(1,0), M1(1,1)+M2(1,1), M1(1,2)+M2(1,2), M1(1,3)+M2(1,3),
		   M1(2,0)+M2(2,0), M1(2,1)+M2(2,1), M1(2,2)+M2(2,2), M1(2,3)+M2(2,3),
		   M1(3,0)+M2(3,0), M1(3,1)+M2(3,1), M1(3,2)+M2(3,2), M1(3,3)+M2(3,3));
  }
  
  HMatrix operator- (const HMatrix& M2) const {
    const HMatrix& M1 = *this;
    return HMatrix(
		   M1(0,0)-M2(0,0), M1(0,1)-M2(0,1), M1(0,2)-M2(0,2), M1(0,3)-M2(0,3),
		   M1(1,0)-M2(1,0), M1(1,1)-M2(1,1), M1(1,2)-M2(1,2), M1(1,3)-M2(1,3),
		   M1(2,0)-M2(2,0), M1(2,1)-M2(2,1), M1(2,2)-M2(2,2), M1(2,3)-M2(2,3),
		   M1(3,0)-M2(3,0), M1(3,1)-M2(3,1), M1(3,2)-M2(3,2), M1(3,3)-M2(3,3));
  }
  
  // Frobenius norm, i.e. sqrt of the sum of the squares of the entries also defined as trace(M^2)
  F frobnorm() const { 
    F fn = 0; 
    for(int i = 0; i < 16; i++) fn += _m[i]*_m[i]; 
    return sqrt(fn);
  }

 private:
  // data stored in column major order for OpenGL compatibility, i.e. first for elements are the first column of the matrix
  F _m[16];
};

template <class F> inline HMatrix<F> operator*(F s, const HMatrix<F>& M) { 
  return M*s;
}
template <class F> inline std::ostream& operator<< ( std::ostream& os, const HMatrix<F>& M ) { 
  os << "[ " << M(0,0)  << " " << M(0,1)  << " " << M(0,2)  << " " <<  M(0,3)  << "; ";
  os         << M(1,0)  << " " << M(1,1)  << " " << M(1,2)  << " " <<  M(1,3)  << "; ";
  os         << M(2,0)  << " " << M(2,1)  << " " << M(2,2)  << " " <<  M(2,3)  << "; ";
  os         << M(3,0)  << " " <<  M(3,1) << " " << M(3,2)  << " " <<  M(3,3) 
	     << "] ";   
  return os;
}


#endif
