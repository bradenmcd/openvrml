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

using namespace OpenVRML_;

namespace OpenVRML {

double Vlength(const float V[3])
{
    const double vlen = sqrt(V[0]*V[0]+V[1]*V[1]+V[2]*V[2]);
    return (fpzero(vlen) ? 0.0 : vlen);
}

void Vdiff(float V[3], const float A[3], const float B[3])
{
    V[0] = A[0] - B[0];
    V[1] = A[1] - B[1];
    V[2] = A[2] - B[2];
}

void Vcross(float V[3], const float A[3], const float B[3])
{
    float x, y, z; // Use temps so V can be A or B
    x = A[1] * B[2] - A[2] * B[1];
    y = A[2] * B[0] - A[0] * B[2];
    z = A[0] * B[1] - A[1] * B[0];
    V[0] = x;
    V[1] = y;
    V[2] = z;
}

void Vnorm(float V[3])
{
    const float vlen = float(sqrt(V[0] * V[0] + V[1] * V[1] + V[2] * V[2]));
    if (!fpzero(vlen)) {
        V[0] /= vlen;
        V[1] /= vlen;
        V[2] /= vlen;
    }
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

} // namespace OpenVRML
