//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
// 

#ifndef MATHUTILS_H
#define MATHUTILS_H

//
// Some useful math stuff that should someday be made into C++...
//
#include <math.h>
#include <iostream.h>
#include "common.h"

// Define single precision floating point "zero".


// Vector ops

#define Vset( V, A ) \
 ((V[0]=A[0]),(V[1]=A[1]),(V[2]=A[2]))             // V <= A
#define Vscale( V, s) \
 ((V[0]*=(s)),(V[1]*=(s)),(V[2]*=s)) 		   // V *= s
#define Vdot( A, B ) \
 (A[0]*B[0]+A[1]*B[1]+A[2]*B[2])		   // A.B

double Vlength( float V[3] );	                   // |V|

void Vdiff( float V[3], const float A[3], const float B[3] );  // V <= A - B
void Vcross( float V[3], const float A[3], const float B[3] ); // V <= A x B
void Vnorm( float V[3] );	                   // V <= V / |V|

void VM(float V[3], const double M[4][4], const float A[3]);   // V <= MA

// Matrix ops

void Midentity(double M[4][4]);	                           		// M <= I
void Mrotation(double M[4][4], const float r[4]);          		// M <= rotation about axis/angle
void Mscale(double M[4][4], const float s[3]);             		// M <= scale by s
void Mtranslation(double M[4][4], const float t[3]);       		// M <= translate by t
void MM(double M[4][4], const double N[4][4]);             		// M <= MN
void MM(double M[4][4], const double A[4][4], const double B[4][4]);    // M <= AB (no aliasing!)
void Mcopy(double M_new[4][4], const double M_orig[4][4]); 		// M_new <= M_orig


// debugging utility stuff.

ostream& Mdump(ostream& o, const double M[4][4]);


void axis_aligned_bbox(float M[4][4], float *min, float *max);
bool InvertMatrix3x3of4x4(float In[],float Out[]);


#endif
