// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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

# ifndef VIEWEROPENGL_H
#   define VIEWEROPENGL_H

#   include <stack>
#   include <OpenVRML/Viewer.h>
#   include <OpenVRML/GL/common.h>

// Use the stencil buffer to set the SHAPE mask.
#   define USE_STENCIL_SHAPE 0

extern "C" struct GLUtesselator;

namespace OpenVRML {

    namespace GL {

        class OPENVRML_GL_SCOPE ViewerOpenGL : public Viewer {
        public:
            enum { MAX_LIGHTS = 8 };

            enum LightType {
                LIGHT_UNUSED,
                LIGHT_DIRECTIONAL,
                LIGHT_POSITIONAL
            };

            // Event types
            enum EventType {
                EVENT_KEY_DOWN,
                EVENT_MOUSE_MOVE,
                EVENT_MOUSE_CLICK,
                EVENT_MOUSE_DRAG,
                EVENT_MOUSE_RELEASE
            };

            enum {
                KEY_HOME,
                KEY_LEFT,
                KEY_UP,
                KEY_RIGHT,
                KEY_DOWN,
                KEY_PAGE_UP,
                KEY_PAGE_DOWN
            };

            struct EventInfo {
                EventType event;
                int what; // key or button number
                int x, y;
            };

        protected:
            enum CursorStyle {
                CURSOR_INHERIT,
                CURSOR_INFO,
                CURSOR_CYCLE,
                CURSOR_UP_DOWN,
                CURSOR_CROSSHAIR
            };

            class ModelviewMatrixStack {
                size_t size;
                std::stack<mat4f> spillover;

            public:
                ModelviewMatrixStack();

                void push();
                void pop();
            };

            struct LightInfo {
                LightType lightType;
                int nestingLevel;
                float radiusSquared;
                vec3f location;
            };

            ModelviewMatrixStack modelviewMatrixStack;

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
            GLUtesselator * tesselator;

            // Pickable entities
            int d_nSensitive;
            int d_activeSensitive;
            int d_overSensitive;

            enum { MAXSENSITIVE = 1000 }; // make dynamic?...
            node * d_sensitiveObject[MAXSENSITIVE];

            bool d_selectMode;
            double d_selectZ; // window Z coord of last selection

            // Lights
            LightInfo d_lightInfo[MAX_LIGHTS];

            int d_beginx, d_beginy;

            // quaternion representations of last, current rotation
            quatf lastquat, curquat;
            bool d_rotationChanged;

            bool d_rotating, d_scaling, d_translating;
            bool d_drawBSpheres;
            bool d_cull;

            double d_renderTime;
            double d_renderTime1;

            // Window system specific methods

            virtual void wsPostRedraw() = 0;
            virtual void wsSetCursor( CursorStyle c) = 0;
            virtual void wsSwapBuffers() = 0;
            virtual void wsSetTimer( double ) = 0;

            // Initialize OpenGL state
            void initialize();

            // Geometry insertion setup & cleanup methods
            void beginGeometry();
            void endGeometry();

            // User interaction
            void step(float, float, float);
            void zoom(float);
            void rot_trackball(float x1, float y1, float x2, float y2);
            void rotate(const rotation & rotation) throw ();

#ifndef macintosh
            void handleKey(int);
#endif
            void handleButton(EventInfo*);
            void handleMouseDrag(int, int);

            void insertExtrusionCaps(unsigned int mask, size_t nSpine,
                                     const std::vector<vec3f> & c,
                                     const std::vector<vec2f> & cs);

            // Check for pickable entity selection
            bool checkSensitive(int x, int y, EventType );

        public:
            explicit ViewerOpenGL(OpenVRML::browser & browser);
            virtual ~ViewerOpenGL();

            virtual RenderMode getRenderMode();
            virtual double getFrameRate();

            //
            virtual void resetUserNavigation();
            virtual void getUserNavigation(mat4f & M);

            // Scope dirlights, open/close display lists
            virtual Object beginObject(const char *name, bool retain);
            virtual void endObject();

            // Insert objects into the display list
            virtual Object
            insertBackground(const std::vector<float> & groundAngle,
                             const std::vector<color> & groundColor,
                             const std::vector<float> & skyAngle,
                             const std::vector<color> & skyColor,
                             int* whc = 0,
                             unsigned char ** pixels = 0);


            virtual Object insertBox(const vec3f & size);

            virtual Object insertCone(float h, float r, bool bottom, bool side);
            virtual Object insertCylinder(float h, float r, bool, bool, bool);

            virtual Object
            insertElevationGrid(unsigned int mask,
                                const std::vector<float> & height,
                                int32 xDimension,
                                int32 zDimension,
                                float xSpacing,
                                float zSpacing,
                                const std::vector<color> & color,
                                const std::vector<vec3f> & normal,
                                const std::vector<vec2f> & texCoord);
            virtual Object
            insertExtrusion(unsigned int,
                            const std::vector<vec3f> & spine,
                            const std::vector<vec2f> & crossSection,
                            const std::vector<rotation> & orientation,
                            const std::vector<vec2f> & scale);
            virtual Object
            insertLineSet(const std::vector<vec3f> & coord,
                          const std::vector<int32> & coordIndex,
                          bool colorPerVertex,
                          const std::vector<color> & color,
                          const std::vector<int32> & colorIndex);
            virtual Object
            insertPointSet(const std::vector<vec3f> & coord,
                           const std::vector<color> & color);
            virtual Object
            insertShell(unsigned int mask,
                        const std::vector<vec3f> & coord,
                        const std::vector<int32> & coordIndex,
                        const std::vector<color> & color,
                        const std::vector<int32> & colorIndex,
                        const std::vector<vec3f> & normal,
                        const std::vector<int32> & normalIndex,
                        const std::vector<vec2f> & texCoord,
                        const std::vector<int32> & texCoordIndex);
            virtual Object insertSphere(float radius);

            // Lights
            virtual Object insertDirLight(float ambientItensity,
                                          float intensity,
                                          const color & color,
                                          const vec3f & direction);
            virtual Object insertPointLight(float ambientIntensity,
                                            const vec3f & attenuation,
                                            const color & color,
                                            float intensity,
                                            const vec3f & location,
                                            float radius);
            virtual Object insertSpotLight(float ambientIntensity,
                                           const vec3f & attenuation,
                                           float beamWidth,
                                           const color & color,
                                           float cutOffAngle,
                                           const vec3f & direction,
                                           float intensity,
                                           const vec3f & location,
                                           float radius);


            // Lightweight copy
            virtual Object insertReference(Object existingObject);

            // Remove an object from the display list
            virtual void removeObject(Object key);

            virtual void enableLighting(bool);

            // Set attributes
            virtual void setColor(const color & rgb, float a = 1.0);

            virtual void setFog(const color & color,
                                float visibilityRange,
                                const char * fogType);

            virtual void setMaterial(float ambientIntensity,
                                     const color & diffuseColor,
                                     const color & emissiveColor,
                                     float shininess,
                                     const color & specularColor,
                                     float transparency);

            virtual void setMaterialMode(int nTexComponents,
                                         bool geometryColor);

            virtual void setSensitive(node * object);

            virtual void scaleTexture(size_t w, size_t h,
                                      size_t newW, size_t newH,
                                      size_t nc,
                                      unsigned char * pixels);

            virtual TextureObject insertTexture(size_t w, size_t h, size_t nc,
                                                bool repeat_s,
                                                bool repeat_t,
                                                const unsigned char *pixels,
                                                bool retainHint = false);

            virtual TextureObject
            insertSubTexture(size_t xoffset, size_t yoffset,
                             size_t w, size_t h,
                             size_t whole_w, size_t whole_h,
                             size_t nc, bool repeat_s,
                             bool repeat_t,
                             const unsigned char *pixels,
                             bool retainHint = false);

            // Reference/remove a texture object
            virtual void insertTextureReference(TextureObject, int);
            virtual void removeTextureObject(TextureObject);

            virtual void setTextureTransform(const vec2f & center,
                                             float rotation,
                                             const vec2f & scale,
                                             const vec2f & translation);

            virtual void setViewpoint(const vec3f & position,
                                      const rotation & orientation,
                                      float fieldOfView,
                                      float avatarSize,
                                      float visibilityLimit);

            virtual void transform(const mat4f & mat);

            // The viewer knows the current viewpoint
            virtual void transformPoints(int nPoints, float *points);

            virtual void drawBSphere(const BSphere & bs,
                                     BVolume::Intersection intersection);



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

            void input( EventInfo *);
        };
    }
}

# endif // VIEWEROPENGL_H
