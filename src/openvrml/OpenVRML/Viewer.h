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
//  Viewer.h
//  Abstract base class for display of VRML models
//

#ifndef VIEWER_H
#define VIEWER_H

#include <stddef.h>
#include "common.h"
#include "VrmlFrustum.h"

namespace OpenVRML {

    class VrmlScene;
    class BVolume;
    class BSphere;
    class AABox;
    class SFVec3f;
    class VrmlMatrix;

    class OPENVRML_SCOPE Viewer {
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

        VrmlScene & getScene();


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
         * VrmlMatrix-format matrix. The navigation transformation should be
         * orthonormal.
         *
         * @param M returns navigation matrix in VrmlMatrix format
         */
        virtual void getUserNavigation(VrmlMatrix & M) = 0;

        // Open/close display lists
        virtual Object beginObject(const char *, bool = false) = 0;
        virtual void endObject() = 0;

        // Insert objects into the display list
        virtual Object insertBackground(size_t nGroundAngles = 0,
				        const float * groundAngle = 0,
				        const float * groundColor = 0,
				        size_t nSkyAngles = 0,
				        const float * skyAngle = 0,
				        const float * skyColor = 0,
				        int* whc = 0,
                                        unsigned char ** pixels = 0) = 0;

        virtual Object insertBox(float, float, float ) = 0;
        virtual Object insertCone(float, float, bool, bool) = 0;
        virtual Object insertCylinder(float, float, bool, bool, bool) = 0;

        virtual Object insertElevationGrid(unsigned int mask,
				           size_t nx,
				           size_t nz,
				           const float * height,
				           float dx,
				           float dz,
				           const float * tc,
				           const float * normals,
				           const float * colors) = 0;

        virtual Object insertExtrusion(unsigned int,
				       size_t nOrientation,
				       const float * orientation,
				       size_t nScale,
				       const float * scale,
				       size_t nCrossSection,
				       const float * crossSection,
				       size_t nSpine,
				       const float * spine) = 0;

        virtual Object insertLineSet(size_t nCoords, const float * coord,
			             size_t nCoordIndex, const long * coordIndex,
			             bool colorPerVertex,
			             const float * color,
			             size_t nColorIndex,
			             const long * colorIndex) = 0;

        virtual Object insertPointSet(size_t nv, const float * v, const float * c) = 0;

        virtual Object insertShell(unsigned int mask,
			           size_t npoints, const float * points,
			           size_t nfaces, const long * faces,
			           const float * tc,
			           size_t ntci, const long * tci,
			           const float * normal,
			           size_t nni, const long * ni,
			           const float * color,
			           size_t nci, const long * ci) = 0;

        virtual Object insertSphere(float /*radius*/) = 0;

        virtual Object insertText(int*, float size, int n, char const * const *s) = 0;

        // Lights
        virtual Object insertDirLight(float, float, const float [], const float []) = 0;

        virtual Object insertPointLight(float, const float [], const float [],
				        float, const float [], float ) = 0;

        virtual Object insertSpotLight(float ambientIntensity,
				       const float attenuation[],
				       float beamWidth,
				       const float color[],
				       float cutOffAngle,
				       const float direction[],
				       float intensity,
				       const float location[],
				       float radius) = 0;

        // Lightweight copy
        virtual Object insertReference(Object /*existingObject*/) = 0;

        // Remove an object from the display list
        virtual void removeObject(Object) = 0;

        virtual void enableLighting(bool) = 0;

        // Set attributes
        virtual void setFog(const float * /*color*/,
		            float   /*visibilityRange*/,
		            const char * /*fogType*/) = 0;

        virtual void setColor(float r, float g, float b, float a = 1.0) = 0;

        virtual void setMaterial(float ambientIntensity,
			         const float diffuseColor[3],
			         const float emissiveColor[3],
			         float shininess,
			         const float specularColor[3],
			         float transparency) = 0;

        virtual void setMaterialMode( int nTexComponents, bool geometryColor ) = 0;

        virtual void setSensitive(void *object) = 0;

        virtual void scaleTexture(size_t /*w*/, size_t /*h*/,
			          size_t /*newW*/, size_t /*newH*/,
			          size_t /*nc*/,
			          unsigned char* /*pixels*/) = 0;

        // Create a texture object
        virtual TextureObject insertTexture(size_t /*w*/, size_t /*h*/, size_t /*nc*/,
				            bool /*repeat_s*/,
				            bool /*repeat_t*/,
				            const unsigned char * /*pixels*/,
				            bool retainHint = false) = 0;

        // Reference/remove a texture object
        virtual void insertTextureReference(TextureObject, int) = 0;
        virtual void removeTextureObject(TextureObject) = 0;

        virtual void setTextureTransform(const float center[2],
				         float rotation,
				         const float scale[2],
				         const float translation[2]) = 0;

        virtual void setTransform(const float center[3],
			          const float rotation[4],
			          const float scale[3],
			          const float scaleOrientation[4],
			          const float translation[3]) = 0;

        // This is a hack to work around the glPushMatrix() limit (32 deep on Mesa).
        // It has some ugly disadvantages: it is slower and the resulting transform
        // after a setTransform/unsetTransform may not be identical to the original.
        // It might be better to just build our own matrix stack...
        virtual void unsetTransform(const float center[3],
			            const float rotation[4],
			            const float scale[3],
			            const float scaleOrientation[4],
			            const float translation[3]) = 0;

        virtual void setBillboardTransform(const float axisOfRotation[3]) = 0;

        virtual void unsetBillboardTransform(const float axisOfRotation[3]) = 0;

        virtual void setViewpoint(const float * /*position*/,
			          float * /*orientation*/,
			          float /*fieldOfView*/,
			          float /*avatarSize*/,
			          float /*visLimit*/) = 0;

        // The viewer knows the current viewpoint
        virtual void transformPoints(int nPoints, float *points) = 0;

      /**
       * Multiply current ModelView Matrix with Given Matrix M
       *
       * @param M matrix in VrmlMatrix format (Same as OGL)
       */
      // 
      virtual void MatrixMultiply(const float M[4][4]) = 0;

        // still working on some navigation api issues, so don't depend on
        // thses yet. there's a default implementation in any case, so you
        // shouldn't have to worry about it.
        //
        virtual const VrmlFrustum& getFrustum() const;
        virtual void setFrustum(const VrmlFrustum& afrust);

        virtual int isectViewVolume(const BVolume & bv) const;

        virtual void drawBSphere(const BSphere & bs, int flag) = 0;

    protected:
        VrmlScene & scene;
        VrmlFrustum d_frust;

        Viewer(VrmlScene & scene);
        static void computeCylinder(double height, double radius, int numFacets,
		                    float c[][3], float tc[][3], int faces[]);

        static void computeExtrusion(int nOrientation, const float * orientation,
                                     int nScale, const float * scale,
                                     int nCrossSection, const float * crossSection,
                                     int nSpine, const float * spine,
                                     float * c,   // OUT: coordinates
                                     float * tc,  // OUT: texture coords
                                     int * faces);    // OUT: face list

        static void computeSphere(double radius, int numLatLong,
                                  float c[][3], float tc[][3], int faces[]);

        static void computeView(const float position[3], float orientation[3],
                                float distance, float target[3], float up[3]);

    private:
        // non-copyable
        Viewer(const Viewer &);
        Viewer & operator=(const Viewer &);
    };
}

#endif // VIEWER_H
