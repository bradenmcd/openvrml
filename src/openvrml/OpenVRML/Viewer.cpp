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
//  Viewer.cpp
//  Abstract base class for display of VRML models
//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef _WIN32
#include <winconfig.h>
#endif

#include "Viewer.h"
#include "private.h"
#include "MathUtils.h"
#include "VrmlScene.h"

using namespace OpenVRML;
using namespace OpenVRML_;

/**
 * @class OpenVRML::Viewer
 *
 * Map the scene graph to the underlying graphics library.
 */

Viewer::Viewer(VrmlScene & scene): scene(scene) {}

Viewer::~Viewer() {}

VrmlScene & Viewer::getScene() {
    return this->scene;
}

//  Build a cylinder object. It might be smarter to do just one, and reference
//  it with scaling (but the world creator could just as easily do that with 
//  DEF/USE ...).

void Viewer::computeCylinder(double height, double radius, int numFacets,
			     float c[][3], float tc[][3], int faces[]) {
  double angle, x, y;
  int i, polyIndex;

  // Compute coordinates, texture coordinates:
  for (i = 0; i < numFacets; ++i) {
    angle = i * 2 * PI / numFacets;
    x = cos(angle);
    y = sin(angle);
    c[i][0] = radius * x;
    c[i][1] = 0.5 * height;
    c[i][2] = radius * y;
    c[numFacets+i][0] = radius * x;
    c[numFacets+i][1] = -0.5 * height;
    c[numFacets+i][2] = radius * y;

    if (tc)
      {
	double u = 0.75 - ((float) i) / numFacets;
	//double u = ((float) i) / numFacets + 0.25;
	//if ( u > 1.0 ) u -= 1.0;
	tc[i][0] = u; // ((float) i) / numFacets;
	tc[i][1] = 1.0;
	tc[i][2] = 0.0;
	tc[numFacets+i][0] = u; //((float) i) / numFacets;
	tc[numFacets+i][1] = 0.0;
	tc[numFacets+i][2] = 0.0;
      }
  }

  // And compute indices:
  for (i = 0; i < numFacets; ++i) {
    polyIndex = 5*i;
    faces[polyIndex + 0] = i;
    faces[polyIndex + 1] = (i+1) % numFacets;
    faces[polyIndex + 2] = (i+1) % numFacets + numFacets;
    faces[polyIndex + 3] = i + numFacets;
    faces[polyIndex + 4] = -1;
  }
}

//  Build an extrusion.

void Viewer::computeExtrusion(int nOrientation,
			      const float *orientation,
			      int nScale,
			      const float *scale,
			      int nCrossSection,
			      const float *crossSection,
			      int nSpine,
			      const float *spine,
			      float *c,   // OUT: coordinates
			      float *tc,  // OUT: texture coords
			      int *faces)     // OUT: face list
{
  int i, j, ci;

  // Xscp, Yscp, Zscp- columns of xform matrix to align cross section
  // with spine segments.
  float Xscp[3] = { 1.0, 0.0, 0.0};
  float Yscp[3] = { 0.0, 1.0, 0.0};
  float Zscp[3] = { 0.0, 0.0, 1.0};
  float lastZ[3];

  // Is the spine a closed curve (last pt == first pt)?
  bool spineClosed = (fpzero(spine[ 3*(nSpine-1)+0 ] - spine[0]) &&
		      fpzero(spine[ 3*(nSpine-1)+1 ] - spine[1]) &&
		      fpzero(spine[ 3*(nSpine-1)+2 ] - spine[2]));
  
  // Is the spine a straight line?
  bool spineStraight = true;
  for (i = 1; i < nSpine-1; ++i)
    {
      float v1[3], v2[3];
      v1[0] = spine[3*(i-1)+0] - spine[3*(i)+0];
      v1[1] = spine[3*(i-1)+1] - spine[3*(i)+1];
      v1[2] = spine[3*(i-1)+2] - spine[3*(i)+2];
      v2[0] = spine[3*(i+1)+0] - spine[3*(i)+0];
      v2[1] = spine[3*(i+1)+1] - spine[3*(i)+1];
      v2[2] = spine[3*(i+1)+2] - spine[3*(i)+2];
      Vcross(v1, v2, v1);
      if (Vlength(v1) != 0.0)
	{
	  spineStraight = false;
	  Vnorm( v1 );
	  Vset( lastZ, v1 );
	  break;
	}
    }

  // If the spine is a straight line, compute a constant SCP xform
  if (spineStraight)
    {
      float V1[3] = { 0.0, 1.0, 0.0}, V2[3], V3[3];
      V2[0] = spine[3*(nSpine-1)+0] - spine[0];
      V2[1] = spine[3*(nSpine-1)+1] - spine[1];
      V2[2] = spine[3*(nSpine-1)+2] - spine[2];
      Vcross( V3, V2, V1 );
      double len = Vlength(V3);
      if (len != 0.0)		// Not aligned with Y axis
	{
	  Vscale(V3, 1.0/len);

	  float orient[4];		// Axis/angle
	  Vset(orient, V3);
	  orient[3] = acos(Vdot(V1,V2));
	  double scp[4][4];	        // xform matrix
	  Mrotation( scp, orient );
	  for (int k=0; k<3; ++k) {
	    Xscp[k] = scp[0][k];
	    Yscp[k] = scp[1][k];
	    Zscp[k] = scp[2][k];
	  }
	}
    }

  // Orientation matrix
  double om[4][4];
  if (nOrientation == 1 && ! fpzero(orientation[3]) )
    Mrotation( om, orientation );

  // Compute coordinates, texture coordinates:
  for (i = 0, ci = 0; i < nSpine; ++i, ci+=nCrossSection) {

    // Scale cross section
    for (j = 0; j < nCrossSection; ++j) {
      c[3*(ci+j)+0] = scale[0] * crossSection[ 2*j ];
      c[3*(ci+j)+1] = 0.0;
      c[3*(ci+j)+2] = scale[1] * crossSection[ 2*j+1 ];
    }

    // Compute Spine-aligned Cross-section Plane (SCP)
    if (! spineStraight)
      {
	float S1[3], S2[3];	// Spine vectors [i,i-1] and [i,i+1]
	int yi1, yi2, si1, s1i2, s2i2;

	if (spineClosed && (i == 0 || i == nSpine-1))
	  {
	    yi1 = 3*(nSpine-2);
	    yi2 = 3;
	    si1 = 0;
	    s1i2 = 3*(nSpine-2);
	    s2i2 = 3;
	  }
	else if (i == 0)
	  {
	    yi1 = 0;
	    yi2 = 3;
	    si1 = 3;
	    s1i2 = 0;
	    s2i2 = 6;
	  }
	else if (i == nSpine-1)
	  {
	    yi1 = 3*(nSpine-2);
	    yi2 = 3*(nSpine-1);
	    si1 = 3*(nSpine-2);
	    s1i2 = 3*(nSpine-3);
	    s2i2 = 3*(nSpine-1);
	  }
	else
	  {
	    yi1 = 3*(i-1);
	    yi2 = 3*(i+1);
	    si1 = 3*i;
	    s1i2 = 3*(i-1);
	    s2i2 = 3*(i+1);
	  }

	Vdiff( Yscp, &spine[yi2], &spine[yi1] );
	Vdiff( S1, &spine[s1i2], &spine[si1] );
	Vdiff( S2, &spine[s2i2], &spine[si1] );

	Vnorm( Yscp );
	Vset(lastZ, Zscp);	// Save last Zscp
	Vcross( Zscp, S2, S1 );

	float VlenZ = Vlength(Zscp);
	if ( VlenZ == 0.0 )
	  Vset(Zscp, lastZ);
	else
	  Vscale( Zscp, 1.0/VlenZ );

	if ((i > 0) && (Vdot( Zscp, lastZ ) < 0.0))
	  Vscale( Zscp, -1.0 );

	Vcross( Xscp, Yscp, Zscp );
      }

    // Rotate cross section into SCP
    for (j = 0; j < nCrossSection; ++j) {
      float cx, cy, cz;
      cx = c[3*(ci+j)+0]*Xscp[0]+c[3*(ci+j)+1]*Yscp[0]+c[3*(ci+j)+2]*Zscp[0];
      cy = c[3*(ci+j)+0]*Xscp[1]+c[3*(ci+j)+1]*Yscp[1]+c[3*(ci+j)+2]*Zscp[1];
      cz = c[3*(ci+j)+0]*Xscp[2]+c[3*(ci+j)+1]*Yscp[2]+c[3*(ci+j)+2]*Zscp[2];
      c[3*(ci+j)+0] = cx;
      c[3*(ci+j)+1] = cy;
      c[3*(ci+j)+2] = cz;
    }

    // Apply orientation
    if (! fpzero(orientation[3]) )
      {
	if (nOrientation > 1)
	  Mrotation( om, orientation );

	for (j = 0; j < nCrossSection; ++j) {
	  float cx, cy, cz;
	  cx = c[3*(ci+j)+0]*om[0][0]+c[3*(ci+j)+1]*om[1][0]+c[3*(ci+j)+2]*om[2][0];
	  cy = c[3*(ci+j)+0]*om[0][1]+c[3*(ci+j)+1]*om[1][1]+c[3*(ci+j)+2]*om[2][1];
	  cz = c[3*(ci+j)+0]*om[0][2]+c[3*(ci+j)+1]*om[1][2]+c[3*(ci+j)+2]*om[2][2];
	  c[3*(ci+j)+0] = cx;
	  c[3*(ci+j)+1] = cy;
	  c[3*(ci+j)+2] = cz;
	}
      }

    // Translate cross section
    for (j = 0; j < nCrossSection; ++j) {
      c[3*(ci+j)+0] += spine[3*i+0];
      c[3*(ci+j)+1] += spine[3*i+1];
      c[3*(ci+j)+2] += spine[3*i+2];

      // Texture coords
      tc[3*(ci+j)+0] = ((float) j) / (nCrossSection-1);
      tc[3*(ci+j)+1] = ((float) i) / (nSpine-1);
      tc[3*(ci+j)+2] = 0.0;
    }

    if (nScale > 1) scale += 2;
    if (nOrientation > 1) orientation += 4;
  }

  // And compute face indices:
  if (faces)
    {
      int polyIndex = 0;
      for (i = 0, ci = 0; i < nSpine-1; ++i, ci+=nCrossSection) {
	for (j = 0; j < nCrossSection-1; ++j) {
	  faces[polyIndex + 0] = ci+j;
	  faces[polyIndex + 1] = ci+j+1;
	  faces[polyIndex + 2] = ci+j+1 + nCrossSection;
	  faces[polyIndex + 3] = ci+j + nCrossSection;
	  faces[polyIndex + 4] = -1;
	  polyIndex += 5;
	}
      }
    }
}


void Viewer::computeSphere(double radius,
			   int numLatLong,
			   float c[][3],
			   float tc[][3],
			   int *faces)
{
  double r, angle, x, y, z;
  int i, j, polyIndex;

  // Compute coordinates, texture coordinates:
  for (i = 0; i < numLatLong; ++i) {
    /*y = 2.0 * ( ((double)i) / (numLatLong-1) ) - 1.0;*/
    angle = ( i * PI / (numLatLong-1) ) - PI_2;
    y = sin( angle );
    r = sqrt( 1.0 - y*y );
    for (j = 0; j < numLatLong; ++j) {
      angle = 2 * PI * ((double)j) / numLatLong;
      x = - sin(angle)*r;
      z = - cos(angle)*r;
      c[i*numLatLong+j][0] = radius * x;
      c[i*numLatLong+j][1] = radius * y;
      c[i*numLatLong+j][2] = radius * z;
      if (tc)
	{
	  tc[i*numLatLong+j][0] = ((float) j)/(numLatLong);
	  tc[i*numLatLong+j][1] = ((float) i)/(numLatLong);
	  tc[i*numLatLong+j][2] = 0.0;
	}
    }
  }

  // And compute indices:
  if (faces)
    for (i = 0; i < numLatLong-1; ++i) {
      for (j = 0; j < numLatLong; ++j) {
	polyIndex = 5*(i*numLatLong+j);
	faces[polyIndex + 0] = i*numLatLong+j;
	faces[polyIndex + 1] = i*numLatLong+(j+1)%numLatLong;
	faces[polyIndex + 2] = (i+1)*numLatLong+(j+1)%numLatLong;
	faces[polyIndex + 3] = (i+1)*numLatLong+j;
	faces[polyIndex + 4] = -1;  // quad
      }
    }
}


//
// Compute a target and up vector from position/orientation/distance.
//

void Viewer::computeView(const float position[3],
			 float orientation[3],
			 float distance,
			 float target[3],
			 float up[3])
{
  // Graphics Gems, p 466. Convert between axis/angle and rotation matrix
  double len = sqrt( orientation[0]*orientation[0] +
		     orientation[1]*orientation[1] +
		     orientation[2]*orientation[2] );
  if (len > 0.0)
    {
      orientation[0] /= len;
      orientation[1] /= len;
      orientation[2] /= len;
    }

  double s = sin(orientation[3]);
  double c = cos(orientation[3]);
  double t = 1.0 - c;

  // Transform [0,0,1] by the orientation to determine sight line
  target[0] = t * orientation[0] * orientation[2] + s * orientation[1];
  target[1] = t * orientation[1] * orientation[2] - s * orientation[0];
  target[2] = t * orientation[2] * orientation[2] + c;

  // Move along that vector the specified distance away from position[]
  target[0] = -distance*target[0] + position[0];
  target[1] = -distance*target[1] + position[1];
  target[2] = -distance*target[2] + position[2];

  // Transform [0,1,0] by the orientation to determine up vector
  up[0] = t * orientation[0] * orientation[1] - s * orientation[2];
  up[1] = t * orientation[1] * orientation[1] + c;
  up[2] = t * orientation[1] * orientation[2] + s * orientation[0];
}


void Viewer::setColor(float , float , float , float ) {}



/**
 * Intersect the given bounding volume with the view volume. This
 * goes into the viewer to provide a hook for systems that use
 * non-standard view volumes. Most subclasses should be able to use
 * the default implementation provided here. If your view volume is
 * so strange that there's no way to cull to is, then reimplement to
 * always return BV_INSIDE
 *
 * @param bv the bounding volume to intersect with the view volume
 * @return BVolume::INSIDE, OUTSIDE, or PARTIAL
 */
int Viewer::isectViewVolume(const BVolume & bv) const {
    //
    // For normal VRML97 use, this won't need to be overridden, but for
    // systems with non-standard view volumes, this can be changed to
    // cull as appropriate. Note that culling can be disabled by setting
    // a flag in VrmlRenderContext. Since I don't have access to the
    // appropriate cave/boom/whichever api's, I can't be sure that this
    // is enough. If it isn't, please express any concerns to the
    // OpenVRML developer's list, and it can be fixed...
    //
    int r = BVolume::BV_PARTIAL;
    const BSphere * bs = 0;
    const AABox * ab = 0;
    if (bs = dynamic_cast<const BSphere *>(&bv)) {
        r = bs->isectFrustum(this->d_frust);
    } else if (ab = dynamic_cast<const AABox *>(&bv)) {
        r = ab->isectFrustum(this->d_frust);
    }
    return r;
}

/**
 * @fn void Viewer::drawBSphere(const BSphere & bs, int flag)
 *
 * Draw a bounding sphere. Used for debugging view culling. Probably
 * should be drawBVolume and handle aaboxes as well.
 *
 * @param bs a bounding sphere; if max, will not be drawn
 * @param flag one of the bvolume intersection test constants, or 4
 *             to draw in unique way. (useful for debugging)
 */



// comment here on how we're forcing everybody to carry around a frust
// whether they want it or not, and how it shouldn't be used except
// for debugging and stuff since it might not be valid in some
// implementations
//
const VrmlFrustum& Viewer::getFrustum() const
{
  return d_frust;
}

void Viewer::setFrustum(const VrmlFrustum& afrust) {
  d_frust = afrust; // copy
}
