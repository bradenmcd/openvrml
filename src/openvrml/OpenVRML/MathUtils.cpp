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

using namespace OpenVRML;
using namespace OpenVRML_;

double OpenVRML::Vlength(const float V[3])
{
  double vlen = sqrt(V[0]*V[0]+V[1]*V[1]+V[2]*V[2]);
  return (fpzero(vlen) ? 0.0 : vlen);
}

void OpenVRML::Vdiff(float V[3], const float A[3], const float B[3])
{
  V[0] = A[0] - B[0];
  V[1] = A[1] - B[1];
  V[2] = A[2] - B[2];
}

void OpenVRML::Vcross(float V[3], const float A[3], const float B[3])
{
  float x,y,z;			// Use temps so V can be A or B
  x = A[1]*B[2] - A[2]*B[1];
  y = A[2]*B[0] - A[0]*B[2];
  z = A[0]*B[1] - A[1]*B[0];
  V[0] = x;
  V[1] = y;
  V[2] = z;
}

void OpenVRML::Vnorm(float V[3])
{
  float vlen = (float) sqrt(V[0]*V[0]+V[1]*V[1]+V[2]*V[2]);
  if (! fpzero(vlen))
    {
      V[0] /= vlen;
      V[1] /= vlen;
      V[2] /= vlen;
    }
}

void OpenVRML::axis_aligned_bbox(float M[4][4], float *min, float *max)
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

bool OpenVRML::InvertMatrix3x3of4x4(const float In[16], float Out[9]) {
    const float * Inp = &In[0];
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
 *  Given quaternion, compute axis and angle.
 */

void OpenVRML::quat_to_axis(float q[4], float axisAngle[4])
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




