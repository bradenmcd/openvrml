// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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

# ifndef OPENVRML_VIEWER_H
#   define OPENVRML_VIEWER_H

#   include <stddef.h>
#   include <vector>
#   include <OpenVRML/bvolume.h>
#   include <OpenVRML/VrmlFrustum.h>

namespace OpenVRML {

    class node;
    class BSphere;
    class browser;

    class OPENVRML_SCOPE Viewer {
    protected:
        VrmlFrustum d_frust;

    public:
        enum {
            MASK_NONE                 = 0,
            MASK_CCW                  = 1,
            MASK_CONVEX               = 2,
            MASK_SOLID                = 4,
            MASK_BOTTOM               = 8,
            MASK_TOP                  = 16,
            MASK_SIDE                 = 32,
            MASK_COLOR_PER_VERTEX     = 64,
            MASK_NORMAL_PER_VERTEX    = 128
        };

        enum RenderMode {
            RENDER_MODE_DRAW,
            RENDER_MODE_PICK
        };

        typedef long Object;
        typedef long TextureObject;

        OpenVRML::browser & browser;

        virtual ~Viewer() = 0;

        virtual RenderMode getRenderMode() = 0;
        virtual double getFrameRate() = 0;
        virtual void resetUserNavigation() = 0;
        virtual void getUserNavigation(mat4f & M) = 0;

        virtual Object beginObject(const char *, bool = false) = 0;
        virtual void endObject() = 0;

        virtual Object insertBackground(const std::vector<float> & groundAngle,
                                        const std::vector<color> & groundColor,
                                        const std::vector<float> & skyAngle,
                                        const std::vector<color> & skyColor,
                                        int* whc = 0,
                                        unsigned char ** pixels = 0) = 0;

        virtual Object insertBox(const vec3f & size) = 0;
        virtual Object insertCone(float height, float radius, bool bottom,
                                  bool side) = 0;
        virtual Object
        insertCylinder(float height, float radius, bool bottom, bool side,
                       bool top) = 0;
        virtual Object
        insertElevationGrid(unsigned int mask,
                            const std::vector<float> & height,
                            int32 xDimension, int32 zDimension,
                            float xSpacing, float zSpacing,
                            const std::vector<color> & color,
                            const std::vector<vec3f> & normal,
                            const std::vector<vec2f> & texCoord) = 0;
        virtual Object
        insertExtrusion(unsigned int,
                        const std::vector<vec3f> & spine,
                        const std::vector<vec2f> & crossSection,
                        const std::vector<rotation> & orientation,
                        const std::vector<vec2f> & scale) = 0;
        virtual Object
        insertLineSet(const std::vector<vec3f> & coord,
                      const std::vector<int32> & coordIndex,
                      bool colorPerVertex,
                      const std::vector<color> & color,
                      const std::vector<int32> & colorIndex) = 0;
        virtual Object
        insertPointSet(const std::vector<vec3f> & coord,
                       const std::vector<color> & color) = 0;
        virtual Object
        insertShell(unsigned int mask,
                    const std::vector<vec3f> & coord,
                    const std::vector<int32> & coordIndex,
                    const std::vector<color> & color,
                    const std::vector<int32> & colorIndex,
                    const std::vector<vec3f> & normal,
                    const std::vector<int32> & normalIndex,
                    const std::vector<vec2f> & texCoord,
                    const std::vector<int32> & texCoordIndex) = 0;
        virtual Object insertSphere(float radius) = 0;
        virtual Object insertDirLight(float ambientIntensity, float intensity,
                                      const color & color,
                                      const vec3f & direction) = 0;
        virtual Object insertPointLight(float ambientIntensity,
                                        const vec3f & attenuation,
                                        const color & color,
                                        float intensity,
                                        const vec3f & location,
                                        float radius) = 0;
        virtual Object insertSpotLight(float ambientIntensity,
                                       const vec3f & attenuation,
                                       float beamWidth,
                                       const color & color,
                                       float cutOffAngle,
                                       const vec3f & direction,
                                       float intensity,
                                       const vec3f & location,
                                       float radius) = 0;
        virtual Object insertReference(Object existingObject) = 0;

        virtual void removeObject(Object) = 0;

        virtual void enableLighting(bool) = 0;

        virtual void setFog(const color & color,
                            float visibilityRange,
                            const char * fogType) = 0;

        virtual void setColor(const color & rgb, float a = 1.0) = 0;

        virtual void setMaterial(float ambientIntensity,
                                 const color & diffuseColor,
                                 const color & emissiveColor,
                                 float shininess,
                                 const color & specularColor,
                                 float transparency) = 0;

        virtual void setMaterialMode(int nTexComponents, bool geometryColor) = 0;

        virtual void setSensitive(node * object) = 0;

        virtual void scaleTexture(size_t w, size_t h,
                                  size_t newW, size_t newH,
                                  size_t nc,
                                  unsigned char * pixels) = 0;

        // Create a texture object
        virtual TextureObject insertTexture(size_t w, size_t h, size_t nc,
                                            bool repeat_s,
                                            bool repeat_t,
                                            const unsigned char * pixels,
                                            bool retainHint = false) = 0;

        // Reference/remove a texture object
        virtual void insertTextureReference(TextureObject, int) = 0;
        virtual void removeTextureObject(TextureObject) = 0;

        virtual void setTextureTransform(const vec2f & center,
                                         float rotation,
                                         const vec2f & scale,
                                         const vec2f & translation) = 0;

        virtual void setViewpoint(const vec3f & position,
                                  const rotation & orientation,
                                  float fieldOfView,
                                  float avatarSize,
                                  float visibilityLimit) = 0;

        virtual void transform(const mat4f & mat) = 0;

        // The viewer knows the current viewpoint
        virtual void transformPoints(int nPoints, float *points) = 0;

        // still working on some navigation api issues, so don't depend on
        // thses yet. there's a default implementation in any case, so you
        // shouldn't have to worry about it.
        //
        virtual const VrmlFrustum& getFrustum() const;
        virtual void setFrustum(const VrmlFrustum& afrust);

        virtual BVolume::Intersection
        intersectViewVolume(const BVolume & bvolume) const;

        virtual void drawBSphere(const BSphere & bs,
                                 BVolume::Intersection intersection) = 0;

    protected:
        explicit Viewer(OpenVRML::browser & browser);

    private:
        // non-copyable
        Viewer(const Viewer &);
        Viewer & operator=(const Viewer &);
    };
}

# endif // OPENVRML_VIEWER_H
