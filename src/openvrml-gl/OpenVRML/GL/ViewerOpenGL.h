//
// OpenVRML
//
// Copyright (C) 1998-2000  Chris Morley
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

#ifndef VIEWEROPENGL_H
#define VIEWEROPENGL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <OpenVRML/Viewer.h>


// Use the stencil buffer to set the SHAPE mask.
#define USE_STENCIL_SHAPE 0


// Win32 needs to know that GL stuff lives in a DLL (OPENGL32.DLL)
#if defined(WIN32)
#include <wtypes.h>
#include <winbase.h>
#include <windef.h>
#include <wingdi.h>
#endif

extern "C" struct GLUtesselator;

/**
 * Abstract class for display of VRML models using OpenGL/Mesa.
 * A window-system specific subclass needs to redefine the pure
 * virtual methods.
 */
class ViewerOpenGL : public Viewer {

public:

  enum { MAX_LIGHTS = 8 };

  ViewerOpenGL(VrmlScene *);
  ViewerOpenGL(ViewerOpenGL const &);
  virtual ~ViewerOpenGL();
  
  ViewerOpenGL & operator=(ViewerOpenGL const &);
  
  // Queries
  virtual void getPosition( float *x, float *y, float *z );
  virtual void getOrientation( float *orientation );


  // get rid of this asap.
  virtual void getBillboardTransformMatrix(float [4][4],
                                           const float axisOfRotation[3]);

  virtual RenderMode getRenderMode();
  virtual double getFrameRate();

  //
  virtual void resetUserNavigation();
  virtual void getUserNavigation(double M[4][4]);

  // Scope dirlights, open/close display lists
  virtual Object beginObject(const char *name, bool retain);
  virtual void endObject();

  // Insert objects into the display list
  virtual Object insertBackground(size_t nGroundAngles = 0,
				  const float * groundAngle = 0,
				  const float * groundColor = 0,
				  size_t nSkyAngles = 0,
				  const float * skyAngle = 0,
				  const float * skyColor = 0,
				  int* whc = 0,
                                  unsigned char ** pixels = 0);


  virtual Object insertBox(float x, float y, float z);
  virtual Object insertCone(float h, float r, bool bottom, bool side);
  virtual Object insertCylinder(float h, float r, bool, bool, bool);

  virtual Object insertElevationGrid(unsigned int mask,
				     size_t nx,
				     size_t nz,
				     const float * height,
				     float dx,
				     float dz,
				     const float * tc,
				     const float * normals,
				     const float * colors);

  virtual Object insertExtrusion(unsigned int,
				 size_t nOrientation,
				 const float * orientation,
				 size_t nScale,
				 const float * scale,
				 size_t nCrossSection,
				 const float * crossSection,
				 size_t nSpine,
				 const float * spine);

  virtual Object insertLineSet(size_t, const float *, size_t, const long *,
			       bool colorPerVertex,
                               const float * color,
			       size_t nColorIndex,
			       const long * colorIndex);

  virtual Object insertPointSet(size_t npts, const float * points, const float * colors);
  virtual Object insertShell(unsigned int mask,
			     size_t npoints, const float * points,
			     size_t nfaces, const long * faces,
			     const float * tc,
			     size_t ntci, const long * tci,
			     const float * normal,
			     size_t nni, const long * ni,
			     const float * color,
			     size_t nci, const long * ci);

  virtual Object insertSphere(float radius);

  virtual Object insertText(int *, float, int n, char const * const * s);

  // Lights
  virtual Object insertDirLight(float a, float i, const float rgb[], const float xyz[]);

  virtual Object insertPointLight( float ambientIntensity,
				   const float attenuation[],
				   const float color[],
				   float intensity,
				   const float location[],
				   float radius );

  virtual Object insertSpotLight( float /*ambientIntensity*/ ,
				  const float /*attenuation*/ [],
				  float /*beamWidth*/ ,
				  const float /*color*/ [],
				  float /*cutOffAngle*/ ,
				  const float /*direction*/ [],
				  float /*intensity*/ ,
				  const float /*location*/ [],
				  float /*radius*/ );


  // Lightweight copy
  virtual Object insertReference(Object existingObject);

  // Remove an object from the display list
  virtual void removeObject(Object key);

  virtual void enableLighting(bool);

  // Set attributes
  virtual void setColor(float r, float g, float b, float a = 1.0);

  virtual void setFog(const float * /*color*/,
		      float   /*visibilityRange*/,
		      const char * /*fogType*/);

  virtual void setMaterial(float, const float[], const float[], float, const float[], float);

  virtual void setMaterialMode( int nTexComponents, bool geometryColor );

  virtual void setSensitive(void *object);

  virtual void scaleTexture(size_t /*w*/, size_t /*h*/,
			    size_t /*newW*/, size_t /*newH*/,
			    size_t /*nc*/,
			    unsigned char* /*pixels*/);

  virtual TextureObject insertTexture(size_t w, size_t h, size_t nc,
				      bool repeat_s,
				      bool repeat_t,
				      const unsigned char *pixels,
				      bool retainHint = false);

  // Reference/remove a texture object
  virtual void insertTextureReference(TextureObject, int);
  virtual void removeTextureObject(TextureObject);

  virtual void setTextureTransform(const float center[2],
				   float rotation,
				   const float scale[2],
				   const float translation[2]);

  virtual void setTransform(const float center[3],
			    const float rotation[4],
			    const float scale[3],
			    const float scaleOrientation[4],
			    const float translation[3]);

  virtual void unsetTransform(const float center[3],
			      const float rotation[4],
			      const float scale[3],
			      const float scaleOrientation[4],
			      const float translation[3]);

  virtual void setBillboardTransform(const float axisOfRotation[3]);
  virtual void unsetBillboardTransform(const float axisOfRotation[3]);


  virtual void setViewpoint(const float *position,
			    float *orientation,
			    float fieldOfView,
			    float avatarSize,
			    float visLimit);

  // The viewer knows the current viewpoint
  virtual void transformPoints(int nPoints, float *points);

  virtual void drawBSphere(const VrmlBSphere& bs, int flag);



  //
  // Viewer callbacks (not for public consumption)

  // Update the model.
  void update( double time = 0.0 );

  // Redraw the screen.
  virtual void redraw();
  void resize(int w, int h);

  // user interaction
#ifdef macintosh /* in order to be able to send 'keys' from menus */
  void handleKey(int);
#endif

  // Event types
  typedef enum {
    EVENT_KEY_DOWN,
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_CLICK,
    EVENT_MOUSE_DRAG,
    EVENT_MOUSE_RELEASE
  } EventType;

  enum {
    KEY_HOME,
    KEY_LEFT,
    KEY_UP,
    KEY_RIGHT,
    KEY_DOWN,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN
  };

  typedef struct {
    EventType event;
    int what;			// key or button number
    int x, y;
  } EventInfo;
    
  void input( EventInfo *);

  // Structs used in tesselator callbacks
  typedef struct IndexData {
    const float * v;			// data values
    size_t ni; const long * i;			// number of indices, index pointer
  } IndexData;

  struct ShellData {
    unsigned int mask;
    const float * points;
    size_t nfaces; const long * faces;		// face list
    IndexData texCoord;		// texture coordinates and indices
    IndexData normal;		// normals and indices
    IndexData color;		// colors and indices
    int *texAxes;
    float *texParams;
    size_t nf, i;
  };


protected:
  typedef enum {
    CURSOR_INHERIT,
    CURSOR_INFO,
    CURSOR_CYCLE,
    CURSOR_UP_DOWN,
    CURSOR_CROSSHAIR
  } CursorStyle;

  // Window system specific methods

  virtual void wsPostRedraw() = 0;
  virtual void wsSetCursor( CursorStyle c) = 0;
  virtual void wsSwapBuffers() = 0;
  virtual void wsSetTimer( double ) = 0;

//private:

  // Initialize OpenGL state
  void initialize();

  // Geometry insertion setup & cleanup methods
  void beginGeometry();
  void endGeometry();

  // Text rendering
  void text2(int x, int y, float scale, char *text);
  void text3(int *justify, float size, int n, const char * const * s);

  // IndexedFaceSet helpers
  void insertShellConvex(ShellData *);
  void insertShellTess(ShellData *);

  // User interaction
  void step(float, float, float);
  void rot(float x , float y, float z, float a);
#ifndef macintosh
  void handleKey(int);
#endif
  void handleButton(EventInfo*);
  void handleMouseDrag(int, int);

  // GL status
  bool d_GLinitialized;
  bool d_blend;
  bool d_lit;
  bool d_texture;
  bool d_wireframe;

  int d_window;
  int d_winWidth, d_winHeight;
  float d_background[3];

  // Groups used to be put in display lists, now these counters
  // trigger various initializations and clean ups
  int d_nObjects, d_nestedObjects;

  // Tessellation
  GLUtesselator * d_tess;
  void insertExtrusionCaps(unsigned int mask, size_t nSpine, const float * c,
			   size_t nCrossSection, const float * cs);

  // Check for pickable entity selection
  bool checkSensitive(int x, int y, EventType );

  // Pickable entities
  int d_nSensitive;
  int d_activeSensitive;
  int d_overSensitive;

  enum { MAXSENSITIVE = 1000 };	// make dynamic?...
  void *d_sensitiveObject[ MAXSENSITIVE ];

  bool d_selectMode;
  double d_selectZ;		// window Z coord of last selection

  // Lights
  typedef enum { LIGHT_UNUSED, LIGHT_DIRECTIONAL, LIGHT_POSITIONAL } LightType;
  typedef struct lightinfo {
    LightType lightType;
    int nestingLevel;
    float radiusSquared;
    float location[3];
  } LightInfo;

  LightInfo d_lightInfo[MAX_LIGHTS];

  // View manipulation
  float d_position[3];
  float d_target[3];
  float d_orientation[4];

  int d_beginx, d_beginy;
  float d_scale;
  float d_translatex, d_translatey, d_translatez;

  // quaternion representations of last, current rotation
  float d_lastquat[4], d_curquat[4];
  bool d_rotationChanged;
  float d_rotationMatrix[4][4];

  bool d_rotating, d_scaling, d_translating;
  bool d_drawBSpheres;
  bool d_cull;

  bool d_reportFPS;
  double d_renderTime;
  double d_renderTime1;

};

#endif // VIEWEROPENGL_H


