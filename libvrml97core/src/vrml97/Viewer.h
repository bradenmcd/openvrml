//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//
//  Viewer.h
//  Abstract base class for display of VRML models
//

#ifndef VIEWER_H
#define VIEWER_H

class VrmlScene;
class VrmlBVolume;
class VrmlBSphere;
class VrmlAABox;

#include "VrmlFrustum.h"


/**
 * Map the scene graph to the underlying graphics library.
 *
 */
class Viewer {

protected:
  // Explicitly instantiate a subclass object
  Viewer(VrmlScene *scene) : d_scene(scene) {}

public:

  virtual ~Viewer() = 0;

  // Options flags
  enum {
    MASK_NONE	= 0,
    MASK_CCW	= 1,
    MASK_CONVEX	= 2,
    MASK_SOLID	= 4,
    MASK_BOTTOM = 8,
    MASK_TOP	= 16,
    MASK_SIDE	= 32,
    MASK_COLOR_PER_VERTEX  = 64,
    MASK_NORMAL_PER_VERTEX = 128
  };

  // Object and texture keys. Don't mix them up.
  typedef long Object;
  typedef long TextureObject;

  // 
  VrmlScene *scene() { return d_scene; }

  // Query
  virtual void getPosition( float *x, float *y, float *z ) = 0;
  virtual void getOrientation( float *orientation ) = 0;

  enum RenderMode {
    RENDER_MODE_DRAW,
    RENDER_MODE_PICK
  };

  // Return renderMode
  virtual RenderMode getRenderMode() = 0;

  virtual double getFrameRate() = 0;

  /**
   * Return view to last bound viewpoint.
   */
  virtual void resetUserNavigation() = 0;

  /**
   * Return user's navigation from the last bound viewpoint as a
   * MathUtils-format matrix. The navigation transformation should be
   * orthonormal.
   *
   * @param M returns navigation matrix in MathUtils format
   */
  virtual void getUserNavigation(double M[4][4]) = 0;

  // Open/close display lists
  virtual Object beginObject(const char *, bool = false) = 0;
  virtual void endObject() = 0;

  // Insert objects into the display list
  virtual Object insertBackground(int = 0    /*nGroundAngles*/,
				  float* = 0 /*groundAngle*/,
				  float* = 0 /*groundColor*/,
				  int = 0    /*nSkyAngles*/,
				  float* = 0 /*skyAngle*/,
				  float* = 0 /*skyColor*/,
				  int* = 0, unsigned char ** = 0) = 0;
			     
  virtual Object insertBox(float, float, float ) = 0;
  virtual Object insertCone(float, float, bool, bool) = 0;
  virtual Object insertCylinder(float, float, bool, bool, bool) = 0;

  virtual Object insertElevationGrid(unsigned int /*mask*/,
				     int  /*nx*/,
				     int  /*nz*/,
				     float * /*height*/,
				     float /*dx*/,
				     float /*dz*/,
				     float * /*tc*/,
				     float * /*normals*/,
				     float *colors ) = 0;

  virtual Object insertExtrusion(unsigned int,
				 int  /*nOrientation*/,
				 float * /*orientation*/,
				 int  /*nScale*/,
				 float * /*scale*/,
				 int  /*nCrossSection*/,
				 float * /*crossSection*/,
				 int   /*nSpine*/,
				 float * /*spine*/) = 0;

  virtual Object insertLineSet(int nCoords, float *coord,
			       int nCoordIndex, int *coordIndex,
			       bool colorPerVertex,
			       float *color,
			       int nColorIndex,
			       int *colorIndex) = 0;

  virtual Object insertPointSet(int nv, float *v, float *c) = 0;

  virtual Object insertShell(unsigned int mask,
			     int npoints, float *points,
			     int nfaces, int *faces,
			     float *tc,
			     int ntci, int *tci,
			     float *normal,
			     int nni, int *ni,
			     float *color,
			     int nci, int *ci) = 0;

  virtual Object insertSphere(float /*radius*/) = 0;

  virtual Object insertText(int*, float size, int n, char const * const *s) = 0;

  // Lights
  virtual Object insertDirLight(float, float, float [], float []) = 0;

  virtual Object insertPointLight(float, float [], float [],
				  float, float [], float ) = 0;

  virtual Object insertSpotLight( float /*ambientIntensity*/ ,
				  float /*attenuation*/ [],
				  float /*beamWidth*/ ,
				  float /*color*/ [],
				  float /*cutOffAngle*/ ,
				  float /*direction*/ [],
				  float /*intensity*/ ,
				  float /*location*/ [],
				  float /*radius*/ ) = 0;

  // Lightweight copy
  virtual Object insertReference(Object /*existingObject*/) = 0;

  // Remove an object from the display list
  virtual void removeObject(Object) = 0;

  virtual void enableLighting(bool) = 0;

  // Set attributes
  virtual void setFog(float * /*color*/,
		      float   /*visibilityRange*/,
		      const char * /*fogType*/) = 0;

  virtual void setColor(float r, float g, float b, float a = 1.0) = 0;

  virtual void setMaterial(float, float[], float[], float, float[], float) = 0;

  virtual void setMaterialMode( int nTexComponents, bool geometryColor ) = 0;

  virtual void setSensitive(void *object) = 0;

  virtual void scaleTexture(int /*w*/, int /*h*/,
			    int /*newW*/, int /*newH*/,
			    int /*nc*/,
			    unsigned char* /*pixels*/) = 0;

  // Create a texture object
  virtual TextureObject insertTexture(int /*w*/, int /*h*/, int /*nc*/,
				      bool /*repeat_s*/,
				      bool /*repeat_t*/,
				      unsigned char* /*pixels*/,
				      bool retainHint = false) = 0;

  // Reference/remove a texture object
  virtual void insertTextureReference(TextureObject, int) = 0;
  virtual void removeTextureObject(TextureObject) = 0;

  virtual void setTextureTransform( float * /*center*/,
				    float /*rotation*/,
				    float * /*scale*/,
				    float * /*translation*/ ) = 0;

  virtual void setTransform(float * /*center*/,
			    float * /*rotation*/,
			    float * /*scale*/,
			    float * /*scaleOrientation*/,
			    float * /*translation*/) = 0;

  // This is a hack to work around the glPushMatrix() limit (32 deep on Mesa).
  // It has some ugly disadvantages: it is slower and the resulting transform
  // after a setTransform/unsetTransform may not be identical to the original.
  // It might be better to just build our own matrix stack...
  virtual void unsetTransform(float * /*center*/,
			      float * /*rotation*/,
			      float * /*scale*/,
			      float * /*scaleOrientation*/,
			      float * /*translation*/) = 0;

  virtual void setBillboardTransform(float * /*axisOfRotation*/) = 0;

  virtual void unsetBillboardTransform(float * /*axisOfRotation*/) = 0;

  virtual void setViewpoint(float * /*position*/,
			    float * /*orientation*/,
			    float /*fieldOfView*/,
			    float /*avatarSize*/,
			    float /*visLimit*/) = 0;

  // The viewer knows the current viewpoint
  virtual void transformPoints(int nPoints, float *points) = 0;


  // still working on some navigation api issues, so don't depend on
  // thses yet. there's a default implementation in any case, so you
  // shouldn't have to worry about it.
  //
  virtual const VrmlFrustum& getFrustum() const;
  virtual void setFrustum(const VrmlFrustum& afrust);


  /**
   * Intersect the given bounding volume with the view volume. This
   * goes into the viewer to provide a hook for systems that use
   * non-standard view volumes. Most subclasses should be able to use
   * the default implementation provided here. If your view volume is
   * so strange that there's no way to cull to is, then reimplement to
   * always return BV_INSIDE
   *
   * @param bv the bounding volume to intersect with the view volume
   * @return VrmlBVolume::INSIDE, OUTSIDE, or PARTIAL
   */
  virtual int isectViewVolume(const VrmlBVolume& bv) const;

  /**
   * Draw a bounding sphere. Used for debugging view culling. Probably
   * should be drawBVolume and handle aaboxes as well.
   *
   * @param bs a bounding sphere, if max, will not be drawn
   * @param flag one of the bvolume intersection test constants, or 4
   *             to draw in unique way. (useful for debugging)
   */
  virtual void drawBSphere(const VrmlBSphere& bs, int flag) = 0;


protected:

  static void computeCylinder(double height,
		       double radius,
		       int numFacets,
		       float c[][3],
		       float tc[][3],
		       int faces[]);

  static void computeExtrusion( int  /*nOrientation*/,
			 float * /*orientation*/,
			 int  /*nScale*/,
			 float * /*scale*/,
			 int  /*nCrossSection*/,
			 float * /*crossSection*/,
			 int   /*nSpine*/,
			 float * /*spine*/,
			 float *c,   // OUT: coordinates
			 float *tc,  // OUT: texture coords
			 int *faces);    // OUT: face list

  static void computeSphere(double radius,
		     int numLatLong,
		     float c[][3],
		     float tc[][3],
		     int faces[]);

  static void computeView(float position[3],
		   float orientation[3],
		   float distance,
		   float target[3],
		   float up[3]);

  VrmlScene *d_scene;

  VrmlFrustum d_frust;



private:

  Viewer() {}			// Don't allow default constructors

};

#endif // VIEWER_H
