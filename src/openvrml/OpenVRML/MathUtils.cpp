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

#include "MathUtils.h"
#include "private.h"
#include <string.h>		// memcpy

using namespace OpenVRML_;

double Vlength( float V[3] )
{
  double vlen = sqrt(V[0]*V[0]+V[1]*V[1]+V[2]*V[2]);
  return (fpzero(vlen) ? 0.0 : vlen);
}

void Vdiff( float V[3], const float A[3], const float B[3] )
{
  V[0] = A[0] - B[0];
  V[1] = A[1] - B[1];
  V[2] = A[2] - B[2];
}

void Vcross( float V[3], const float A[3], const float B[3] )
{
  float x,y,z;			// Use temps so V can be A or B
  x = A[1]*B[2] - A[2]*B[1];
  y = A[2]*B[0] - A[0]*B[2];
  z = A[0]*B[1] - A[1]*B[0];
  V[0] = x;
  V[1] = y;
  V[2] = z;
}

void Vnorm( float V[3] )
{
  float vlen = (float) sqrt(V[0]*V[0]+V[1]*V[1]+V[2]*V[2]);
  if (! fpzero(vlen))
    {
      V[0] /= vlen;
      V[1] /= vlen;
      V[2] /= vlen;
    }
}


// Note that these matrices are stored in natural (C) order (the transpose
// of the OpenGL matrix). Could change this someday...

void Midentity( double M[4][4] )
{
  for (int i=0; i<4; ++i)
    for (int j=0; j<4; ++j)
      M[i][j] = (i == j) ? 1.0 : 0.0;
}

// Convert from axis/angle to transformation matrix GG p466

void Mrotation( double M[4][4], const float axisAngle[4] )
{
  float aa_norm[4];
  aa_norm[0] = axisAngle[0];
  aa_norm[1] = axisAngle[1];
  aa_norm[2] = axisAngle[2];
  aa_norm[3] = axisAngle[3];
  Vnorm( aa_norm );

  double s = sin(aa_norm[3]);
  double c = cos(aa_norm[3]);
  double t = 1.0 - c;
  double x = aa_norm[0];
  double y = aa_norm[1];
  double z = aa_norm[2];

  M[0][0] = t*x*x + c;
  M[0][1] = t*x*y - s*z;
  M[0][2] = t*x*z + s*y;
  M[0][3] = 0.0;
  M[1][0] = t*x*y + s*z;
  M[1][1] = t*y*y + c;
  M[1][2] = t*y*z - s*x;
  M[1][3] = 0.0;
  M[2][0] = t*x*z - s*y;
  M[2][1] = t*y*z + s*x;
  M[2][2] = t*z*z + c;
  M[2][3] = 0.0;
  M[3][0] = M[3][1] = M[3][2] = 0.0;
  M[3][3] = 1.0;
}

void Mscale( double M[4][4], const float scale[3] )
{
  Midentity(M);
  for (int i=0; i<3; ++i)
    M[i][i] = scale[i];
}


// M <= AB
//
// this version does not allow aliasing!. that means that M,A,B must
// all be different arrays. call with care. justification is that the
// caller generally know whether or not aliasing is possible, so if
// the caller wants to take the risk in order to gain a few cycles,
// that's the caller's reponsibility
//
//
void MM(double M[4][4], const double A[4][4], const double B[4][4])
{
  // someday we should unroll this, or at least use a couple of
  // registers for the loop indicies.
  for (int i=0; i<4; ++i)
    for (int j=0; j<4; ++j)
      M[i][j] = A[i][0]*B[0][j] + A[i][1]*B[1][j] +
	A[i][2]*B[2][j] + A[i][3]*B[3][j];
}


void MM( double M[4][4], const double N[4][4] )
{
  double m[4][4];

  memcpy(m, M, sizeof(m));
  for (int i=0; i<4; ++i)
    for (int j=0; j<4; ++j)
      M[i][j] = m[i][0]*N[0][j] + m[i][1]*N[1][j] +
	m[i][2]*N[2][j] + m[i][3]*N[3][j];
}


void VM( float V[3], const double M[4][4], const float A[3] )
{
  float v[3] = { A[0], A[1], A[2] }; // Allow for V/A aliasing
  for (int i=0; i<3; ++i)
    V[i] = (float)(M[i][0] * v[0] + M[i][1] * v[1] + M[i][2] * v[2] + M[i][3]);
}


void Mtranslation(double M[4][4], const float t[3])
{
  Midentity(M);
  M[0][3] = t[0];
  M[1][3] = t[1];
  M[2][3] = t[2];
}


ostream& Mdump(ostream& o, const double M[4][4])
{
  for(int i=0; i<4; i++) {
    for(int j=0; j<4; j++) {
      o << M[i][j];
      if (j!=3) cout << ",";
    }
    o << endl;
  }
  return o;
}


void Mcopy(double M_new[4][4], const double M_orig[4][4])
{
  memcpy(M_new, M_orig, sizeof(double[4][4]));
}


void axis_aligned_bbox(float M[4][4], float *min, float *max)
{
// Algorithm is taken from "Transforming Axis aligned Bounding Boxes"
// by Jim Arvo "Graphics Gems Academic Press 1990"

 float box[8],newbox[8],a,b;
 int i,j;

 for(i=0;i<8;i++)newbox[i]=0.0,
 box[0] = min[0]; box[1] = min[1]; box[2] = min[2]; box[3] = 1.0;
 box[4] = max[0]; box[5] = max[1]; box[6] = max[2]; box[7] = 1.0;
 for(i=0; i<3; i++)
	 for(j=0; j<4; j++)
	 {
		 a = box[j] * M[j][i];
		 b = box[j+4] * M[j][i];
		 if(a < b)
		 {
			 newbox[i] += a;
			 newbox[i+4] += b;
		 }
		 else
		 {
			 newbox[i] += b;
			 newbox[i+4] += a;
		 }
	 }
	 min[0] = newbox[0]; min[1] = newbox[1]; min[2] = newbox[2];
	 max[0] = newbox[4]; max[1] = newbox[5]; max[2] = newbox[6];
}

bool InvertMatrix3x3of4x4(float In[16],float Out[9])
{
	float *Inp = &In[0];
	float mat[] = {Inp[0], Inp[1], Inp[2],
                       Inp[4], Inp[5], Inp[6],
		       Inp[8], Inp[9], Inp[10]};
	float a = mat[4] * mat[8];
	float b = mat[5] * mat[7];
	float c = mat[3] * mat[8];
	float d = mat[5] * mat[6];
	float e = mat[3] * mat[7];
	float f = mat[4] * mat[6];
	float det = mat[0] * (a - b) - mat[1] * (c - d) + mat[2] * (e - f);
	if(det == 0) return false;
	Out[0] = (a - b)/det;
	Out[1] = (mat[2] * mat[7] - mat[1] * mat[8])/det;
	Out[2] = (mat[1] * mat[5] - mat[2] * mat[4])/det;
	Out[3] = (d - c)/det;
	Out[4] = (mat[0] * mat[8] - mat[2] * mat[6])/det;
	Out[5] = (mat[2] * mat[3] - mat[0] * mat[5])/det;
	Out[6] = (e - f)/det;
        Out[7] = (mat[1] * mat[6] - mat[0] * mat[7])/det;
	Out[8] = (mat[0] * mat[4] - mat[1] * mat[3])/det;
	return true;
}

/*
 * Build a quaternion rotation , given a rotation matrix.
 */
 
void build_quaternion(float m[4][4], float quat[4])
{
  float  diagonal, s, q[4];
  int    i, j, k;
  int next[3] = {1, 2, 0};
  diagonal = m[0][0] + m[1][1] + m[2][2];
  // check the diagonal
  if (diagonal > 0.0) {
    s = sqrt (diagonal + 1.0);
    quat[3] = s / 2.0;
    s = 0.5 / s;
    quat[0] = (m[1][2] - m[2][1]) * s;
    quat[1] = (m[2][0] - m[0][2]) * s;
    quat[2] = (m[0][1] - m[1][0]) * s;
} else {
         // diagonal is negative
          i = 0;
          if (m[1][1] > m[0][0]) i = 1;
             if (m[2][2] > m[i][i]) i = 2;
            j = next[i];
            k = next[j];
            s = sqrt ((m[i][i] - (m[j][j] + m[k][k])) + 1.0);
            q[i] = s * 0.5;
 
            if (s != 0.0) s = 0.5 / s;
            q[3] = (m[j][k] - m[k][j]) * s;
            q[j] = (m[i][j] + m[j][i]) * s;
            q[k] = (m[i][k] + m[k][i]) * s;                  
 
 
          quat[0] = q[0];
          quat[1] = q[1];
          quat[2] = q[2];
          quat[3] = q[3];
  }
}         

/*
 *  Given quaternion, compute axis and angle.
 */

void quat_to_axis(float q[4], float axisAngle[4])
{
  float val = acos(q[3]);
  if (fpzero(val))
  {
    axisAngle[0] = 0.0;
    axisAngle[1] = 1.0;
    axisAngle[2] = 0.0;
    axisAngle[3] = 0.0;
  }
  else
  {
    axisAngle[0] = q[0]/sin(val);
    axisAngle[1] = q[1]/sin(val);
    axisAngle[2] = q[2]/sin(val);
    axisAngle[3] = 2*val;
  }
}                 




