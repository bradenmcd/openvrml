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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# if HAVE_WINDOWS_H && defined(_WIN32)
#   include <windows.h>
# endif
# ifdef interface
#   undef interface
# endif

# if HAVE_APPLE_OPENGL_FRAMEWORK
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
# else
#   include <GL/gl.h>
#   include <GL/glu.h>
# endif

# include <cmath>
# include <limits>
# ifndef NDEBUG
#   include <iostream>
# endif
# include <openvrml/browser.h>
# include <openvrml/vrml97node.h>
# include "viewer.h"

// Textures are now done using OGL1.1 bindTexture API rather than
// display lists when this flag is set. Don't define this if you
// are still at OpenGL 1.0 (or get a newer OpenGL).

# define USE_TEXTURE_DISPLAY_LISTS 1

#   ifdef NDEBUG
#     define OPENVRML_GL_PRINT_MESSAGE_(message_)
#     define OPENVRML_GL_PRINT_ERRORS_
#   else
#     define OPENVRML_GL_PRINT_MESSAGE_(message_) \
            std::cerr << __FILE__ << ":" << __LINE__ << ": " \
                      << message_ << std::endl
#     define OPENVRML_GL_PRINT_ERRORS_ \
            do { \
                GLenum glerr; \
                while ((glerr = glGetError()) != GL_NO_ERROR) { \
                    const GLubyte * const error_str = gluErrorString(glerr); \
                    const GLubyte * end; \
                    for (end = error_str; *end; ++end) {} \
                    OPENVRML_GL_PRINT_MESSAGE_(std::string(error_str, end)); \
                } \
            } while (false)
#   endif

# if (defined(__CYGWIN__) && defined(X_DISPLAY_MISSING)) || defined(__MINGW32__)
#   define OPENVRML_GL_CALLBACK_ __attribute__ ((__stdcall__))
# elif defined (_WIN32)
#   define OPENVRML_GL_CALLBACK_ APIENTRY
# else
#   define OPENVRML_GL_CALLBACK_
# endif

namespace {

    const double pi     = 3.14159265358979323846;
    const double pi_2   = 1.57079632679489661923;
    const double pi_4   = 0.78539816339744830962;
    const double inv_pi = 0.31830988618379067154;

    template <typename Float>
    OPENVRML_GL_LOCAL inline Float fabs(const Float f)
    {
        return f < 0.0 ? -f : f;
    }

    template <typename Float>
    struct OPENVRML_GL_LOCAL fequal :
        std::binary_function<Float, Float, bool> {
        bool operator()(Float a, Float b) const
        {
            const Float diff = fabs(a - b);
            if (diff == 0.0) { return true; }
            const Float e = std::numeric_limits<Float>::epsilon();
            return diff / fabs(a) <= e && diff / fabs(b) <= e;
        }
    };

    class OPENVRML_GL_LOCAL gl_capabilities {
    public:
        GLint max_modelview_stack_depth;
        GLint max_texture_size;

        static const gl_capabilities & instance()
            OPENVRML_THROW1(std::bad_alloc);

    private:
        static boost::scoped_ptr<const gl_capabilities> instance_;

        gl_capabilities();
    };

    boost::scoped_ptr<const gl_capabilities> gl_capabilities::instance_;

    const gl_capabilities & gl_capabilities::instance()
        OPENVRML_THROW1(std::bad_alloc)
    {
        if (!gl_capabilities::instance_) {
            gl_capabilities::instance_.reset(new gl_capabilities);
        }
        return *gl_capabilities::instance_;
    }

    gl_capabilities::gl_capabilities()
    {
        glGetIntegerv(GL_MAX_MODELVIEW_STACK_DEPTH,
                      &this->max_modelview_stack_depth);
        glGetIntegerv(GL_MAX_TEXTURE_SIZE,
                      &this->max_texture_size);
    }


    struct OPENVRML_GL_LOCAL vertex_data {
        GLdouble coord[3];
        const openvrml::color * color;
        const openvrml::vec3f * normal;
        const openvrml::vec2f * tex_coord;

        vertex_data();
    };

    vertex_data::vertex_data():
        color(0),
        normal(0),
        tex_coord(0)
    {}


    struct OPENVRML_GL_LOCAL combined_vertex_data_ {
        openvrml::color color;
        openvrml::vec3f normal;
        openvrml::vec2f tex_coord;
    };


    struct OPENVRML_GL_LOCAL shell_tess_user_data {
        size_t face_index;
        const std::vector<openvrml::color> * face_color;
        const std::vector<openvrml::int32> * face_color_index;
        const std::vector<openvrml::vec3f> * face_normal;
        const std::vector<openvrml::int32> * face_normal_index;
        std::list<vertex_data> combined_vertices;
        std::list<combined_vertex_data_> combined_vertex_data;

        shell_tess_user_data();
    };

    shell_tess_user_data::shell_tess_user_data():
        face_index(0),
        face_color(0),
        face_color_index(0),
        face_normal(0),
        face_normal_index(0)
    {}


    /**
     * @internal
     */
    struct OPENVRML_GL_LOCAL ShellData {
        unsigned int mask;
        const std::vector<openvrml::vec3f> & coord;
        const std::vector<openvrml::int32> & coordIndex;
        const std::vector<openvrml::color> & color;
        const std::vector<openvrml::int32> & colorIndex;
        const std::vector<openvrml::vec3f> & normal;
        const std::vector<openvrml::int32> & normalIndex;
        const std::vector<openvrml::vec2f> & texCoord;
        const std::vector<openvrml::int32> & texCoordIndex;
        int *texAxes;
        float *texParams;
        size_t nf, i;

        ShellData(unsigned int mask,
                  const std::vector<openvrml::vec3f> & coord,
                  const std::vector<openvrml::int32> & coordIndex,
                  const std::vector<openvrml::color> & color,
                  const std::vector<openvrml::int32> & colorIndex,
                  const std::vector<openvrml::vec3f> & normal,
                  const std::vector<openvrml::int32> & normalIndex,
                  const std::vector<openvrml::vec2f> & texCoord,
                  const std::vector<openvrml::int32> & texCoordIndex,
                  int * texAxes, float * texParams, size_t nf, size_t i);
    };

    ShellData::ShellData(unsigned int mask,
                         const std::vector<openvrml::vec3f> & coord,
                         const std::vector<openvrml::int32> & coordIndex,
                         const std::vector<openvrml::color> & color,
                         const std::vector<openvrml::int32> & colorIndex,
                         const std::vector<openvrml::vec3f> & normal,
                         const std::vector<openvrml::int32> & normalIndex,
                         const std::vector<openvrml::vec2f> & texCoord,
                         const std::vector<openvrml::int32> & texCoordIndex,
                         int * texAxes,
                         float * texParams,
                         size_t nf,
                         size_t i):
        mask(mask),
        coord(coord),
        coordIndex(coordIndex),
        color(color),
        colorIndex(colorIndex),
        normal(normal),
        normalIndex(normalIndex),
        texCoord(texCoord),
        texCoordIndex(texCoordIndex),
        texAxes(texAxes),
        texParams(texParams),
        nf(nf),
        i(i)
    {}

    // Generate a normal from 3 indexed points.
    OPENVRML_GL_LOCAL const openvrml::vec3f
    indexFaceNormal(const size_t i1,
                    const size_t i2,
                    const size_t i3,
                    const std::vector<openvrml::vec3f> & points)
    {
        using openvrml::vec3f;
        const vec3f v1 = points[i2] - points[i3];
        const vec3f v2 = points[i2] - points[i1];
        return (v1 * v2).normalize();
    }
}

extern "C" void OPENVRML_GL_CALLBACK_ shell_tess_begin(const GLenum type,
                                                       void * const user_data)
{
    shell_tess_user_data & user_data_ =
        *static_cast<shell_tess_user_data *>(user_data);

    glBegin(type);

    if (user_data_.face_color) {
        const size_t color_index =
            (user_data_.face_color_index
             && user_data_.face_index < user_data_.face_color_index->size())
            ? (*user_data_.face_color_index)[user_data_.face_index]
            : user_data_.face_index;
        if (color_index < user_data_.face_color->size()) {
            glColor3fv(&(*user_data_.face_color)[color_index][0]);
        }
    }

    if (user_data_.face_normal) {
        const size_t normal_index =
            (user_data_.face_normal_index
             && user_data_.face_index < user_data_.face_normal_index->size())
            ? (*user_data_.face_normal_index)[user_data_.face_index]
            : user_data_.face_index;
        if (normal_index < user_data_.face_normal->size()) {
            glNormal3fv(&(*user_data_.face_normal)[normal_index][0]);
        }
    }
}

extern "C" void OPENVRML_GL_CALLBACK_ shell_tess_vertex(void * vertex_data)
{
    ::vertex_data & vertex_data_ = *static_cast< ::vertex_data *>(vertex_data);

    if (vertex_data_.color) {
        glColor3fv(&(*vertex_data_.color)[0]);
    }
    if (vertex_data_.normal) {
        glNormal3fv(&(*vertex_data_.normal)[0]);
    }
    if (vertex_data_.tex_coord) {
        glTexCoord2fv(&(*vertex_data_.tex_coord)[0]);
    }

    glVertex3dv(vertex_data_.coord);
}

extern "C" void OPENVRML_GL_CALLBACK_
shell_tess_combine(GLdouble coords[3],
                   void *[4],
                   GLfloat[4],
                   void ** outData,
                   void * user_data)
{
    try {
        using std::list;
        assert(user_data);
        shell_tess_user_data & user_data_ =
            *static_cast<shell_tess_user_data *>(user_data);
        user_data_.combined_vertices.push_back(::vertex_data());
        ::vertex_data & new_vertex = user_data_.combined_vertices.back();
        new_vertex.coord[0] = coords[0];
        new_vertex.coord[1] = coords[1];
        new_vertex.coord[2] = coords[2];
        *outData = &new_vertex;
    } catch (std::bad_alloc &) {
        *outData = 0;
    }
}

extern "C" void OPENVRML_GL_CALLBACK_ shell_tess_end(void * user_data)
{
    shell_tess_user_data & user_data_ =
        *static_cast<shell_tess_user_data *>(user_data);
    ++user_data_.face_index;
    glEnd();
}

extern "C" void OPENVRML_GL_CALLBACK_ tess_error(const GLenum error_code)
{
    const GLubyte * const error_str = gluErrorString(error_code);
    const GLubyte * end;
    for (end = error_str; *end; ++end) {}
    OPENVRML_GL_PRINT_MESSAGE_(std::string(error_str, end));
}


/**
 * @namespace openvrml::gl
 *
 * @brief OpenGL geometry renderer.
 */

/**
 * @class openvrml::gl::viewer
 *
 * @brief Abstract class for display of VRML models using OpenGL/Mesa.
 *
 * A window-system specific subclass needs to redefine the pure
 * virtual methods.
 */

/**
 * @var openvrml::gl::viewer::max_lights
 *
 * @brief Maximum number of lights in a scene.
 */

/**
 * @enum openvrml::gl::viewer::light_type
 *
 * @brief Light type.
 */

/**
 * @var openvrml::gl::viewer::light_type openvrml::gl::viewer::light_unused
 *
 * @brief Unused.
 */

/**
 * @var openvrml::gl::viewer::light_type openvrml::gl::viewer::light_directional
 *
 * @brief Directional.
 */

/**
 * @var openvrml::gl::viewer::light_type openvrml::gl::viewer::light_positional
 *
 * @brief Positional.
 */

/**
 * @enum openvrml::gl::viewer::event_type
 *
 * @brief Event type.
 */

/**
 * @var viewer::event_type openvrml::gl::viewer::event_key_down
 *
 * @brief Key down.
 */

/**
 * @var openvrml::gl::viewer::event_type openvrml::gl::viewer::event_mouse_move
 *
 * @brief Mouse movement.
 */

/**
 * @var openvrml::gl::viewer::event_type openvrml::gl::viewer::event_mouse_click
 *
 * @brief Mouse button click.
 */

/**
 * @var openvrml::gl::viewer::event_type openvrml::gl::viewer::event_mouse_drag
 *
 * @brief Mouse drag.
 */

/**
 * @var openvrml::gl::viewer::event_type openvrml::gl::viewer::event_mouse_release
 *
 * @brief Mouse button release.
 */

/**
 * @struct openvrml::gl::viewer::event_info
 *
 * @brief Input event.
 */

/**
 * @var openvrml::gl::viewer::event_type openvrml::gl::viewer::event_info::event
 *
 * @brief Event type.
 */

/**
 * @var int openvrml::gl::viewer::event_info::what
 *
 * @brief Key or button number.
 */

/**
 * @var int openvrml::gl::viewer::event_info::x
 *
 * @brief x-coordinate (for mouse events).
 */

/**
 * @var int openvrml::gl::viewer::event_info::y
 *
 * @brief y-coordinate (for mouse events).
 */

/**
 * @enum openvrml::gl::viewer::cursor_style
 *
 * @brief Cursor style.
 */

/**
 * @var openvrml::gl::viewer::cursor_style openvrml::gl::viewer::cursor_inherit
 *
 * @brief Inherit the parent's cursor style.
 */

/**
 * @var openvrml::gl::viewer::cursor_style openvrml::gl::viewer::cursor_info
 *
 * @brief Pointing hand.
 */

/**
 * @var openvrml::gl::viewer::cursor_style openvrml::gl::viewer::cursor_cycle
 *
 * @brief Arrows rotating in a circle.
 */

/**
 * @var openvrml::gl::viewer::cursor_style openvrml::gl::viewer::cursor_up_down
 *
 * @brief Bidirectional pointing up and down.
 */

/**
 * @var openvrml::gl::viewer::cursor_style openvrml::gl::viewer::cursor_crosshair
 *
 * @brief Crosshair.
 */

/**
 * @class openvrml::gl::viewer::modelview_matrix_stack
 *
 * @brief Encapsulates an extended modelview matrix stack.
 *
 * OpenGL requires that implementations have a modelview matrix stack with a
 * maximum depth of only 32. Regardless of that, the maximum depth can be
 * expected to vary between implementations, and we don't want nesting of
 * Transform nodes in VRML worlds to be constrained by this limit.
 *
 * modelview_matrix_stack uses the OpenGL modelview matrix stack until it fills
 * up, at which point any additional matrices that spill over are pushed onto
 * a conventional stack of mat4f.
 */

/**
 * @var size_t openvrml::gl::viewer::modelview_matrix_stack::size
 *
 * @brief The current stack depth.
 */

/**
 * @var std::stack<openvrml::mat4f> openvrml::gl::viewer::modelview_matrix_stack::spillover
 *
 * @brief Any matrices that won't fit on the OpenGL modelview matrix stack get
 *      pushed onto this stack.
 */

/**
 * @brief Construct.
 */
openvrml::gl::viewer::modelview_matrix_stack::modelview_matrix_stack():
    size(0)
{}

/**
 * @brief Push the current matrix onto the stack.
 *
 * @pre The current matrix is the modelview matrix.
 */
void openvrml::gl::viewer::modelview_matrix_stack::push()
{
# ifndef NDEBUG
    GLint matrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
# endif
    assert(matrixMode == GL_MODELVIEW);
    if (this->size
        == size_t(gl_capabilities::instance().max_modelview_stack_depth - 1))
    {
        mat4f mat;
        glGetFloatv(GL_MODELVIEW_MATRIX, &mat[0][0]);
        this->spillover.push(mat);
        glPopMatrix();
    }
    glPushMatrix();
    ++this->size;
}

/**
 * @brief Pop the current matrix off of the stack.
 *
 * @pre The current matrix is the modelview matrix.
 */
void openvrml::gl::viewer::modelview_matrix_stack::pop()
{
# ifndef NDEBUG
    GLint matrixMode;
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);
# endif
    assert(matrixMode == GL_MODELVIEW);
    glPopMatrix();
    if (!this->spillover.empty()) {
        glLoadMatrixf(&this->spillover.top()[0][0]);
        this->spillover.pop();
        glPushMatrix();
    }
    --this->size;
}

/**
 * @struct openvrml::gl::viewer::light_info
 *
 * @brief Light information.
 */

/**
 * @var openvrml::gl::viewer::light_type openvrml::gl::viewer::light_info::type
 *
 * @brief Light type.
 */

/**
 * @var int openvrml::gl::viewer::light_info::nesting_level
 *
 * @brief Nesting level.
 */

/**
 * @var float openvrml::gl::viewer::light_info::radius_squared
 *
 * @brief The light's radius squared.
 */

/**
 * @var openvrml::vec3f openvrml::gl::viewer::light_info::location
 *
 * @brief The light's location.
 */

/**
 * @var openvrml::gl::viewer::modelview_matrix_stack openvrml::gl::viewer::modelview_matrix_stack_
 *
 * @brief Modelview matrix stack.
 */

/**
 * @var bool openvrml::gl::viewer::gl_initialized
 *
 * @brief Indicate whether OpenGL has been initialized for rendering.
 */

/**
 * @var bool openvrml::gl::viewer::blend
 *
 * @brief Indicate whether alpha blending is enabled.
 */

/**
 * @var bool openvrml::gl::viewer::lit
 *
 * @brief Indicate whether lighting is enabled.
 */

/**
 * @var bool openvrml::gl::viewer::texture
 *
 * @brief Indicate whether texture mapping is enabled.
 */

/**
 * @var bool openvrml::gl::viewer::wireframe
 *
 * @brief Indicate whether wireframe mode is enabled.
 */

/**
 * @var size_t openvrml::gl::viewer::win_width
 *
 * @brief Window width.
 */

/**
 * @var size_t openvrml::gl::viewer::win_height
 *
 * @brief Window height.
 */

/**
 * @var color openvrml::gl::viewer::background
 *
 * @brief Background color.
 */

/**
 * @var size_t openvrml::gl::viewer::objects
 *
 * @brief Number of objects.
 */

/**
 * @var size_t openvrml::gl::viewer::nested_objects
 *
 * @brief Number of nested objects.
 */

/**
 * @var GLUtesselator * openvrml::gl::viewer::tesselator
 *
 * @brief GLU tesselation object.
 */

/**
 * @var size_t openvrml::gl::viewer::sensitive
 *
 * @brief Count of sensitive @link openvrml::node nodes@endlink.
 */

/**
 * @var size_t openvrml::gl::viewer::active_sensitive
 *
 * @brief Count of active sensitive @link openvrml::node nodes@endlink.
 */

/**
 * @var size_t openvrml::gl::viewer::over_sensitive
 *
 * @brief Count of sensitive @link openvrml::node nodes@endlink the mouse is
 *        over.
 */

/**
 * @var openvrml::gl::viewer::maxsensitive
 *
 * @brief Maximum number of sensitive @link openvrml::node nodes@endlink.
 */

/**
 * @var openvrml::node * openvrml::gl::viewer::sensitive_object[openvrml::gl::viewer::maxsensitive]
 *
 * @brief Sensitive @link openvrml::node nodes@endlink.
 */

/**
 * @var bool openvrml::gl::viewer::select_mode
 *
 * @brief Indicate whether the renderer is in select mode.
 */

/**
 * @var double openvrml::gl::viewer::select_z
 *
 * @brief Window z-coordinate of last selection.
 */

/**
 * @var openvrml::gl::viewer::light_info openvrml::gl::viewer::light_info_[viewer::max_lights]
 *
 * @brief light_info for the lights in the scene.
 */

/**
 * @var int openvrml::gl::viewer::beginx
 *
 * @brief Mouse drag start x-coordinate.
 */

/**
 * @var int openvrml::gl::viewer::beginy
 *
 * @brief Mouse drag start y-coordinate.
 */

/**
 * @var bool openvrml::gl::viewer::rotating
 *
 * @brief Whether the viewer is in the process of rotating.
 */

/**
 * @var bool openvrml::gl::viewer::scaling
 *
 * @brief Whether the viewer is in the process of scaling.
 */

/**
 * @var bool openvrml::gl::viewer::translating
 *
 * @brief Whether the viewer is in the process of translating.
 */

/**
 * @var bool openvrml::gl::viewer::draw_bounding_spheres
 *
 * @brief Whether to draw bounding spheres.
 */

/**
 * @var double openvrml::gl::viewer::render_time
 *
 * @brief Updated at the end of redraw with the time redraw took to execute.
 */

/**
 * @var double openvrml::gl::viewer::render_time1
 *
 * @brief Rendering time for the previous cycle.
 */

namespace {
    //
    // The functions trackball and tb_project_to_sphere are derived from code
    // by Silicon Graphics, Inc.
    //
    // (c) Copyright 1993, 1994, Silicon Graphics, Inc.
    // ALL RIGHTS RESERVED
    // Permission to use, copy, modify, and distribute this software for
    // any purpose and without fee is hereby granted, provided that the above
    // copyright notice appear in all copies and that both the copyright notice
    // and this permission notice appear in supporting documentation, and that
    // the name of Silicon Graphics, Inc. not be used in advertising
    // or publicity pertaining to distribution of the software without specific,
    // written prior permission.
    //
    // THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
    // AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
    // INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
    // FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
    // GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
    // SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
    // KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
    // LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
    // THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
    // ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
    // ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
    // POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
    //
    // US Government Users Restricted Rights
    // Use, duplication, or disclosure by the Government is subject to
    // restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
    // (c)(1)(ii) of the Rights in Technical Data and Computer Software
    // clause at DFARS 252.227-7013 and/or in similar or successor
    // clauses in the FAR or the DOD or NASA FAR Supplement.
    // Unpublished-- rights reserved under the copyright laws of the
    // United States.  Contractor/manufacturer is Silicon Graphics,
    // Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
    //
    // OpenGL(TM) is a trademark of Silicon Graphics, Inc.
    //
    //
    // Trackball code:
    //
    // Implementation of a virtual trackball.
    // Implemented by Gavin Bell, lots of ideas from Thant Tessman and
    //   the August '88 issue of Siggraph's "Computer Graphics," pp. 121-129.
    //

    //
    // Project an x, y pair onto a sphere of radius r OR a hyperbolic sheet
    // if we are away from the center of the sphere.
    //
    OPENVRML_GL_LOCAL float tb_project_to_sphere(float r, float x, float y)
    {
        static const double sqrt2 = sqrt(2.0);
        static const double sqrt2_2 = sqrt2 / 2.0;

        float z;
        const double d = sqrt(x * x + y * y);
        if (d < r * sqrt2_2) { /* Inside sphere */
            z = float(sqrt(r * r - d * d));
        } else { /* On hyperbola */
            const double t = r / sqrt2;
            z = float(t * t / d);
        }
        return z;
    }

    //
    // Ok, simulate a track-ball.  Project the points onto the virtual
    // trackball, then figure out the axis of rotation, which is the cross
    // product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
    // Note:  This is a deformed trackball-- is a trackball in the center,
    // but is deformed into a hyperbolic sheet of rotation away from the
    // center.  This particular function was chosen after trying out
    // several variations.
    //
    // It is assumed that the arguments to this routine are in the range
    // (-1.0 ... 1.0)
    //
    OPENVRML_GL_LOCAL const openvrml::rotation trackball(float p1x,
                                                         float p1y,
                                                         float p2x,
                                                         float p2y)
    {
        using openvrml::rotation;
        using openvrml::vec3f;

        //
        // This size should really be based on the distance from the center of
        // rotation to the point on the object underneath the mouse.  That
        // point would then track the mouse as closely as possible.  This is a
        // simple example, though, so that is left as an Exercise for the
        // Programmer.
        //
        static const float trackballSize = 0.8f;

        rotation result;

        if (p1x == p2x && p1y == p2y) {
            /* Zero rotation */
            return result;
        }

        //
        // First, figure out z-coordinates for projection of P1 and P2 to
        // deformed sphere
        //
        vec3f p1(p1x, p1y, tb_project_to_sphere(trackballSize, p1x, p1y));
        vec3f p2(p2x, p2y, tb_project_to_sphere(trackballSize, p2x, p2y));

        result.axis((p2 * p1).normalize());

        //
        // Figure out how much to rotate around that axis.
        //
        vec3f d = p1 - p2;
        float t = float(d.length() / (2.0 * trackballSize));

        //
        // Avoid problems with out-of-control values...
        //
        if (t > 1.0) { t = 1.0; }
        if (t < -1.0) { t = -1.0; }

        result.angle(float(2.0 * asin(t)));

        return result;
    }
}

/**
 * @brief Construct a viewer for the specified browser.
 */
openvrml::gl::viewer::viewer():
    gl_initialized(false),
    blend(true),
    lit(true),
    texture(true),
    wireframe(false),
    win_width(1),
    win_height(1),
    objects(0),
    nested_objects(0),
    tesselator(gluNewTess()),
    sensitive(0),
    active_sensitive(0),
    over_sensitive(0),
    select_mode(false),
    select_z(0.0),
    rotating(false),
    scaling(false),
    translating(false),
    draw_bounding_spheres(false),
    render_time(1.0),
    render_time1(1.0)
{
    // Don't make any GL calls here since the window probably doesn't exist.

    for (size_t i = 0; i < max_lights; ++i) {
        this->light_info_[i].type = viewer::light_unused;
    }
}

/**
 * @brief Destroy.
 */
openvrml::gl::viewer::~viewer() OPENVRML_NOTHROW
{
    gluDeleteTess(this->tesselator);
}

/**
 * @brief Initialize.
 */
void openvrml::gl::viewer::initialize()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

#if USE_STENCIL_SHAPE
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
#endif

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    // should only trigger this after a non-uniform scale is seen...
    // on my system this is <5% speedup though.
    glEnable(GL_NORMALIZE);

    // blending is only enabled if this->blend is true and a non-zero
    // transparency value is passed in.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    this->gl_initialized = true;
}

namespace {

    // Call this after each frame for debugging...
    OPENVRML_GL_LOCAL void checkErrors(const std::string & s)
    {
        GLenum glerr;
        while ((glerr = glGetError()) != GL_NO_ERROR) {
            const GLubyte * const error_str = gluErrorString(glerr);
            const GLubyte * end;
            for (end = error_str; *end; ++end) {}
            OPENVRML_GL_PRINT_MESSAGE_("GL ERROR: " + s + " "
                                       + std::string(error_str, end));
        }
    }
}


//
//  beginObject/endObject should correspond to grouping nodes.
//  Group-level scoping for directional lights, anchors, sensors
//  are handled here. Display lists can optionally be created
//  (but the retain flag is just a hint, not guaranteed). Retained
//  objects can be referred to later to avoid duplicating geometry.
//  OpenGL doesn't allow nested objects. The top-down approach of
//  putting entire groups in display lists is faster for static
//  scenes but uses more memory and means that if anything is changed,
//  the whole object must be tossed.
//  The bottom-up model wraps each piece of geometry in a dlist but
//  requires traversal of the entire scene graph to reference each dlist.
//  The decision about what groups to stuff in an object is punted to
//  the object itself, as it can decide whether it is mutable.
//
//  The OpenGL viewer never puts objects in display lists, so the
//  retain hint is ignored.

/**
 * @brief Begin a display list.
 *
 * begin_object/end_object should correspond to
 * @link grouping_node grouping_nodes@endlink.
 * Group-level scoping for directional lights, anchors, sensors
 * are handled here. Display lists can optionally be created
 * (but the retain flag is just a hint, not guaranteed). Retained
 * objects can be referred to later to avoid duplicating geometry.
 * OpenGL doesn't allow nested objects. The top-down approach of
 * putting entire groups in display lists is faster for static
 * scenes but uses more memory and means that if anything is changed,
 * the whole object must be tossed.
 *
 * The bottom-up model wraps each piece of geometry in a dlist but
 * requires traversal of the entire scene graph to reference each dlist.
 * The decision about what groups to stuff in an object is punted to
 * the object itself, as it can decide whether it is mutable.
 *
 * The OpenGL viewer never puts objects in display lists, so the
 * retain hint is ignored.
 *
 * @param[in] id        not used.
 * @param[in] retain    not used.
 *
 * @return 0.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::begin_object(const char *, bool)
{
    // Finish setup stuff before first object
    if (1 == ++this->objects) {
        // Finish specifying the view (postponed to make Background easier)
        this->modelview_matrix_stack_.push();
        if (!this->lit) { glDisable(GL_LIGHTING); }
    }

    ++this->nested_objects;

    // Increment nesting level for group-scoped lights
    for (size_t i = 0; i < viewer::max_lights; ++i) {
        if (this->light_info_[i].type == viewer::light_directional) {
            ++this->light_info_[i].nesting_level;
        }
    }
    this->modelview_matrix_stack_.push();
    return 0;
}

/**
 * @brief End of group scope.
 */
void openvrml::gl::viewer::end_object()
{
    // Decrement nesting level for group-scoped lights and get rid
    // of any defined at this level
    for (size_t i = 0; i < max_lights; ++i) {
        if (this->light_info_[i].type == viewer::light_directional) {
            if (--this->light_info_[i].nesting_level < 0) {
                glDisable(GLenum(GL_LIGHT0 + i));
                this->light_info_[i].type = viewer::light_unused;
            }
        }
    }

    this->modelview_matrix_stack_.pop();

    if (--this->nested_objects == 0) { this->modelview_matrix_stack_.pop(); }
}

/**
 * @brief Reset per-geometry attributes.
 *
 * Called after the appearance/material has been set.
 */
void openvrml::gl::viewer::begin_geometry()
{
    //
    // Any attribute modified in a geometry function should be reset here.
    // Any attribute that can be modified by an Appearance node should not be
    // modified here since these settings will be put in dlists with the
    // geometry.
    //
    glPushAttrib(GL_ENABLE_BIT);
}

/**
 * @brief Reset attributes changed during geometry insertion.
 */
void openvrml::gl::viewer::end_geometry()
{
    //
    // This should be called BEFORE ending any dlists containing geometry,
    // otherwise, attributes changed in the geometry insertion will impact
    // the geometries rendered after the dlist is executed.
    //
    glPopAttrib();
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);
    glFrontFace(GL_CCW);

    // if needed...
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Rendering mode.
 *
 * @return the rendering_mode.
 */
openvrml::gl::viewer::rendering_mode openvrml::gl::viewer::mode()
{
    return this->select_mode
        ? viewer::pick_mode
        : viewer::draw_mode;
}

/**
 * @brief Frame rate.
 *
 * @return the frame rate.
 */
double openvrml::gl::viewer::frame_rate()
{
    return 1.0 / this->render_time;
}

/**
 * @brief Reset the user view to the position and orientation of the currently
 *        bound Viewpoint node.
 */
void openvrml::gl::viewer::reset_user_navigation()
{
    assert(this->browser());
    this->browser()->active_viewpoint().user_view_transform(mat4f());
    this->post_redraw();
}

/**
 * @brief Insert a background into a display list.
 *
 * @param[in] groundAngle   ground angles.
 * @param[in] groundColor   ground colors.
 * @param[in] skyAngle      sky angles.
 * @param[in] skyColor      sky colors.
 * @param[in] front         front texture.
 * @param[in] back          back texture.
 * @param[in] left          left texture.
 * @param[in] right         right texture.
 * @param[in] top           top texture.
 * @param[in] bottom        bottom texture.
 *
 * @return display object identifier.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_background(const std::vector<float> & groundAngle,
                                        const std::vector<color> & groundColor,
                                        const std::vector<float> & skyAngle,
                                        const std::vector<color> & skyColor,
                                        const image & front,
                                        const image & back,
                                        const image & left,
                                        const image & right,
                                        const image & top,
                                        const image & bottom)
{
    using std::vector;

    float r = 0.0, g = 0.0, b = 0.0, a = 1.0;

    // Clear to last sky color
    if (!skyColor.empty()) {
        r = skyColor.back().r();
        g = skyColor.back().g();
        b = skyColor.back().b();
    }

    GLuint glid = 0;

    // Need to separate the geometry from the transformation so the
    // dlist doesn't have to get rebuilt for every mouse movement...
# if 0
    // Don't bother with a dlist if we aren't drawing anything
    if (!this->select_mode
            && (!skyAngle.empty() || !groundAngle.empty() || pixels)) {
        glid = glGenLists(1);
        glNewList(glid, GL_COMPILE_AND_EXECUTE);
    }
# endif

    glClearColor(r, g, b, a);
    GLuint mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
# if USE_STENCIL_SHAPE
    mask |= GL_STENCIL_BUFFER_BIT;
    glClear(mask);
# else
    glClear(mask);

    // Draw the background as big spheres centered at the view position
    if (!this->select_mode
            && (!skyAngle.empty()
                || !groundAngle.empty()
                || !front.array().empty()
                || !back.array().empty()
                || !left.array().empty()
                || !right.array().empty()
                || !top.array().empty()
                || !bottom.array().empty())) {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_LIGHTING);

        this->modelview_matrix_stack_.push();

        glScalef(1000.0, 1000.0, 1000.0);

        // Sphere constants
        static const size_t nCirc = 8; // number of circumferential slices
        static const double cd = 2.0 * pi / nCirc;

        double heightAngle0, heightAngle1 = 0.0;
        vector<color>::const_iterator c0, c1 = skyColor.begin();

        for (vector<float>::const_iterator angle = skyAngle.begin();
             angle != skyAngle.end();
             ++angle) {
            heightAngle0 = heightAngle1;
            heightAngle1 = *angle;
            c0 = c1;
            ++c1;

            double circAngle0, circAngle1 = 0.0;
            double sha0 = sin(heightAngle0), cha0 = cos(heightAngle0);
            double sha1 = sin(heightAngle1), cha1 = cos(heightAngle1);
            double sca0, cca0;
            double sca1 = sin(circAngle1), cca1 = cos(circAngle1);

            glBegin(GL_QUADS);
            for (size_t nc = 0; nc < nCirc; ++nc) {
                circAngle0 = circAngle1;
                circAngle1 = (nc + 1) * cd;
                sca0 = sca1;
                sca1 = sin(circAngle1);
                cca0 = cca1;
                cca1 = cos(circAngle1);

                glColor3fv(&(*c1)[0]);
                glVertex3f(GLfloat(sha1 * cca0),
                           GLfloat(cha1),
                           GLfloat(sha1 * sca0));
                glVertex3f(GLfloat(sha1 * cca1),
                           GLfloat(cha1),
                           GLfloat(sha1 * sca1));
                glColor3fv(&(*c0)[0]);
                glVertex3f(GLfloat(sha0 * cca1),
                           GLfloat(cha0),
                           GLfloat(sha0 * sca1));
                glVertex3f(GLfloat(sha0 * cca0),
                           GLfloat(cha0),
                           GLfloat(sha0 * sca0));
            }
            glEnd(); // GL_QUADS
        }

        // Ground
        heightAngle1 = pi;
        c1 = groundColor.begin();

        for (vector<float>::const_iterator angle = groundAngle.begin();
             angle != groundAngle.end();
             ++angle) {
            heightAngle0 = heightAngle1;
            heightAngle1 = pi - *angle;
            c0 = c1;
            ++c1;

            double circAngle0, circAngle1 = 0.0;
            double sha0 = sin(heightAngle0), cha0 = cos(heightAngle0);
            double sha1 = sin(heightAngle1), cha1 = cos(heightAngle1);
            double sca0, cca0;
            double sca1 = sin(circAngle1), cca1 = cos(circAngle1);

            glBegin(GL_QUADS);
            for (size_t nc = 0; nc < nCirc; ++nc) {
                circAngle0 = circAngle1;
                circAngle1 = (nc + 1) * cd;
                sca0 = sca1;
                sca1 = sin(circAngle1);
                cca0 = cca1;
                cca1 = cos(circAngle1);

                glColor3fv(&(*c1)[0]);
                glVertex3f(GLfloat(sha1 * cca1),
                           GLfloat(cha1),
                           GLfloat(sha1 * sca1));
                glVertex3f(GLfloat(sha1 * cca0),
                           GLfloat(cha1),
                           GLfloat(sha1 * sca0));
                glColor3fv(&(*c0)[0]);
                glVertex3f(GLfloat(sha0 * cca0),
                           GLfloat(cha0),
                           GLfloat(sha0 * sca0));
                glVertex3f(GLfloat(sha0 * cca1),
                           GLfloat(cha0),
                           GLfloat(sha0 * sca1));
            }
            glEnd(); // GL_QUADS
        }

        if (this->texture && !this->wireframe) {
            glScalef(0.5, 0.5, 0.5);

            glEnable(GL_TEXTURE_2D);
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

            static const bool repeat_s = false;
            static const bool repeat_t = false;
            static const bool retain = true;

            if (!front.array().empty()) {
                this->insert_texture(front, repeat_s, repeat_t, retain);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(-1, -1, -1);
                glTexCoord2f(1, 0);
                glVertex3f(1, -1, -1);
                glTexCoord2f(1, 1);
                glVertex3f(1, 1, -1);
                glTexCoord2f(0, 1);
                glVertex3f(-1, 1, -1);
                glEnd(); // GL_QUADS
            }
            if (!back.array().empty()) {
                this->insert_texture(back, repeat_s, repeat_t, retain);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(1, -1, 1);
                glTexCoord2f(1, 0);
                glVertex3f(-1, -1, 1);
                glTexCoord2f(1, 1);
                glVertex3f(-1, 1, 1);
                glTexCoord2f(0, 1);
                glVertex3f(1, 1, 1);
                glEnd(); // GL_QUADS
            }
            if (!left.array().empty()) {
                this->insert_texture(left, repeat_s, repeat_t, retain);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(-1, -1, 1);
                glTexCoord2f(1, 0);
                glVertex3f(-1, -1, -1);
                glTexCoord2f(1, 1);
                glVertex3f(-1, 1, -1);
                glTexCoord2f(0, 1);
                glVertex3f(-1, 1, 1);
                glEnd(); // GL_QUADS
            }
            if (!right.array().empty()) {
                this->insert_texture(right, repeat_s, repeat_t, retain);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(1, -1, -1);
                glTexCoord2f(1, 0);
                glVertex3f(1, -1, 1);
                glTexCoord2f(1, 1);
                glVertex3f(1, 1, 1);
                glTexCoord2f(0, 1);
                glVertex3f(1, 1, -1);
                glEnd(); // GL_QUADS
            }
            if (!top.array().empty()) {
                this->insert_texture(top, repeat_s, repeat_t, retain);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(-1, 1, -1);
                glTexCoord2f(1, 0);
                glVertex3f(1, 1, -1);
                glTexCoord2f(1, 1);
                glVertex3f(1, 1, 1);
                glTexCoord2f(0, 1);
                glVertex3f(-1, 1, 1);
                glEnd(); // GL_QUADS
            }
            if (!bottom.array().empty()) {
                this->insert_texture(bottom, repeat_s, repeat_t, retain);
                glBegin(GL_QUADS);
                glTexCoord2f(0, 0);
                glVertex3f(-1, -1, 1);
                glTexCoord2f(1, 0);
                glVertex3f(1, -1, 1);
                glTexCoord2f(1, 1);
                glVertex3f(1, -1, -1);
                glTexCoord2f(0, 1);
                glVertex3f(-1, -1, -1);
                glEnd(); // GL_QUADS
            }
            glDisable(GL_TEXTURE_2D);
        }

        // Put everything back the way it was
        this->modelview_matrix_stack_.pop();

        if (this->lit) { glEnable(GL_LIGHTING); }
        glEnable(GL_DEPTH_TEST);
    }

# endif // USE_STENCIL_SHAPE

    if (glid) { glEndList(); }

    // Save bg color so we can choose a fg color (doesn't help bg textures...)
    this->background.r(r);
    this->background.g(g);
    this->background.b(b);

    return object_t(glid);
}

/**
 * @brief Insert a box into a display list.
 *
 * @param[in] size  box dimensions.
 *
 * @return display object identifier.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_box(const vec3f & size)
{
    GLuint glid = 0;

    if (!this->select_mode) {
        glid = glGenLists(1);
        glNewList(glid, GL_COMPILE_AND_EXECUTE);
    }

    static const GLint faces[6][4] =
    {
        {0, 1, 2, 3},
        {1, 5, 6, 2},
        {5, 4, 7, 6},
        {4, 0, 3, 7},
        {2, 6, 7, 3},
        {0, 4, 5, 1}
    };

    static const GLfloat n[6][3] =        // normals
    {
        {-1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0},
        {1.0, 0.0, 0.0},
        {0.0, 0.0, -1.0},
        {0.0, 1.0, 0.0},
        {0.0, -1.0, 0.0}
    };

    GLfloat v[8][3];
    GLint i;

    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -size.x() / 2;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = size.x() / 2;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -size.y() / 2;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = size.y() / 2;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = -size.z() / 2;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = size.z() / 2;

    begin_geometry();
    glShadeModel(GL_FLAT);

    glBegin(GL_QUADS);
    for (i = 0; i < 6; ++i) {
        glNormal3fv(&n[i][0]);

        //glTexCoord2f(0.0, 1.0);
        glTexCoord2f(0.0, 0.0);
        glVertex3fv(&v[faces[i][0]][0]);

        //glTexCoord2f(1.0, 1.0);
        glTexCoord2f(1.0, 0.0);
        glVertex3fv(&v[faces[i][1]][0]);

        //glTexCoord2f(1.0, 0.0);
        glTexCoord2f(1.0, 1.0);
        glVertex3fv(&v[faces[i][2]][0]);

        //glTexCoord2f(0.0, 0.0);
        glTexCoord2f(0.0, 1.0);
        glVertex3fv(&v[faces[i][3]][0]);
    }
    glEnd();

    end_geometry();
    if (glid) { glEndList(); }

    return object_t(glid);
}

namespace {

    /**
     * @brief Build a cylinder object.
     *
     * @param[in] height    the height for the cylinder.
     * @param[in] radius    the radius for the cylinder.
     * @param[in] numFacets the number of facets for the sides of the cylinder.
     * @retval c        the coordinates.
     * @retval tc       the texture coordinates.
     * @retval faces    the faces.
     *
     * It might be smarter to do just one, and reference it with scaling (but the
     * world creator could just as easily do that with DEF/USE ...).
     */
    OPENVRML_GL_LOCAL void computeCylinder(const double height,
                                           const double radius,
                                           const int numFacets,
                                           float (*c)[3],
                                           float (*tc)[3],
                                           int * faces)
    {
        double angle, x, y;
        int i, polyIndex;

        // Compute coordinates, texture coordinates:
        for (i = 0; i < numFacets; ++i) {
            angle = i * 2 * pi / numFacets;
            x = cos(angle);
            y = sin(angle);
            c[i][0] = float(radius * x);
            c[i][1] = float(0.5 * height);
            c[i][2] = float(radius * y);
            c[numFacets+i][0] = float(radius * x);
            c[numFacets+i][1] = float(-0.5 * height);
            c[numFacets+i][2] = float(radius * y);

            if (tc) {
                float u = float(0.75 - float(i) / numFacets);
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
}

/**
 * @brief Insert a cone into a display list.
 *
 * @param[in] height    height.
 * @param[in] radius    radius at base.
 * @param[in] bottom    show the bottom.
 * @param[in] side      show the side.
 *
 * @return display object identifier.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_cone(const float height,
                                  const float radius,
                                  const bool bottom,
                                  const bool side)
{
    GLuint glid = 0;

    if (!this->select_mode) {
        glid = glGenLists(1);
        glNewList( glid, GL_COMPILE_AND_EXECUTE );
    }

    begin_geometry();
    if (!bottom || !side) { glDisable(GL_CULL_FACE); }

    if (bottom || side) {
        static const size_t nfacets = 11; // Number of polygons for sides
        static const size_t npts = 2 * nfacets;
        static const size_t nfaces = nfacets * 5;

        float c[npts][3];                // coordinates
        float tc[npts][3];                // texture coordinates
        int faces[nfaces];                // face lists

        // should only compute tc if a texture is present...
        computeCylinder(height, radius, nfacets, c, tc, faces);

        for (size_t i = 0; i < nfacets; ++i) { c[i][0] = c[i][2] = 0.0; }

        if (side) {
            float Ny = radius * radius / height;
            glBegin(GL_QUAD_STRIP);
            for (size_t i = 0; i < nfacets; ++i) {
                glNormal3f(c[i + nfacets][0], Ny, c[i + nfacets][2]);
                glTexCoord2fv(&tc[i + nfacets][0]);
                glVertex3fv(&c[i + nfacets][0]);
                glTexCoord2fv(&tc[i][0]);
                glVertex3fv(&c[i][0]);
            }

            glNormal3f(c[nfacets][0], Ny, c[nfacets][2]);
            glTexCoord2f(tc[nfacets][0] - 1.0f, tc[nfacets][1]);
            glVertex3fv(&c[nfacets][0]);
            glTexCoord2f(tc[0][0] - 1.0f, tc[0][1]);
            glVertex3fv(&c[0][0]);
            glEnd();
        }

        if (bottom) {
            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0.0, -1.0, 0.0);
            glTexCoord2f(0.5, 0.5);
            glVertex3f(0.0, GLfloat(-0.5 * height), 0.0);

            float angle = float(0.5 * pi); // First v is at max x
            float aincr = float(2.0 * pi / nfacets);
            for (size_t i = 0; i < nfacets; ++i, angle += aincr) {
                glTexCoord2f(GLfloat(0.5 * (1.0 + sin(angle))),
                             GLfloat(1.0 - 0.5 * (1.0 + cos(angle))));
                glVertex3fv(&c[i + nfacets][0]);
            }
            glTexCoord2f(GLfloat(0.5 * (1.0 + sin(angle))),
                         GLfloat(1.0 - 0.5 * (1.0 + cos(angle))));
            glVertex3fv(&c[nfacets][0]);
            glEnd();
        }
    }

    end_geometry();
    if (glid) { glEndList(); }

    return object_t(glid);
}

/**
 * @brief Insert a cylinder into a display list.
 *
 * @param[in] height    height.
 * @param[in] radius    radius.
 * @param[in] bottom    show the bottom.
 * @param[in] side      show the side.
 * @param[in] top       show the top.
 *
 * @return display object identifier.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_cylinder(const float height,
                                      const float radius,
                                      const bool bottom,
                                      const bool side,
                                      const bool top)
{
    GLuint glid = 0;

    if (!this->select_mode) {
        glid = glGenLists(1);
        glNewList(glid, GL_COMPILE_AND_EXECUTE);
    }

    begin_geometry();
    if (!bottom || !side || !top) { glDisable(GL_CULL_FACE); }

    if (bottom || side || top) {
        static const size_t nfacets = 8; // Number of polygons for sides
        static const size_t npts = 2 * nfacets;
        static const size_t nfaces = nfacets * 5;

        float c[npts][3];                // coordinates
        float tc[npts][3];                // texture coordinates
        int faces[nfaces];                // face lists

        // should only compute tc if a texture is present...
        computeCylinder(height, radius, nfacets, c, tc, faces);

        if (side) {
            glBegin(GL_QUAD_STRIP);
            for (size_t i = 0; i < nfacets; ++i) {
                glNormal3f(c[i + nfacets][0], 0.0, c[i + nfacets][2]);
                glTexCoord2fv(&tc[i + nfacets][0]);
                glVertex3fv(&c[i + nfacets][0]);
                glTexCoord2fv(&tc[i][0]);
                glVertex3fv(&c[i][0]);
            }

            glNormal3f(c[nfacets][0], 0.0, c[nfacets][2]);
            glTexCoord2f(tc[nfacets][0] - 1.0f, tc[nfacets][1]);
            glVertex3fv(&c[nfacets][0]);
            glTexCoord2f(tc[0][0] - 1.0f, tc[0][1]);
            glVertex3fv(&c[0][0]);
            glEnd();
        }

        if (bottom) {                // tex coords...
            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0.0, -1.0, 0.0);
            glTexCoord2f(0.5, 0.5);
            glVertex3f(0.0, GLfloat(-0.5 * height), 0.0);

            float angle = float(0.5 * pi); // First v is at max x
            const float aincr = float(2.0 * pi / nfacets);
            for (size_t i = 0; i < nfacets; ++i, angle += aincr) {
                glTexCoord2f(GLfloat(0.5 * (1.0 + sin(angle))),
                             GLfloat(1.0 - 0.5 * (1.0 + cos(angle))));
                glVertex3fv(&c[i + nfacets][0]);
            }
            glTexCoord2f(GLfloat(0.5 * (1.0 + sin(angle))),
                         GLfloat(1.0 - 0.5 * (1.0 + cos(angle))));
            glVertex3fv(&c[nfacets][0]);
            glEnd();
        }

        if (top) {                // tex coords...
            glBegin(GL_TRIANGLE_FAN);
            glNormal3f(0.0, 1.0, 0.0);
            glTexCoord2f(0.5, 0.5);
            glVertex3f(0.0, GLfloat(0.5 * height), 0.0);

            float angle = float(0.75 * pi);
            const float aincr = float(2.0 * pi / nfacets);
            for (size_t i = nfacets; i > 0; --i, angle += aincr) {
                glTexCoord2f(GLfloat(0.5 * (1.0 + sin(angle))),
                             GLfloat(1.0 - 0.5 * (1.0 + cos(angle))));
                glVertex3fv(&c[i - 1][0]);
            }
            glTexCoord2f(GLfloat(0.5 * (1.0 + sin(angle))),
                         GLfloat(1.0 - 0.5 * (1.0 + cos(angle))));
            glVertex3fv(&c[nfacets - 1][0]);
            glEnd();
        }
    }

    end_geometry();
    if (glid) { glEndList(); }

    return object_t(glid);
}

namespace {

    // Compute a normal at vert i,j of an ElevationGrid.

    OPENVRML_GL_LOCAL const openvrml::vec3f elevationVertexNormal(
            const int i, const int j,
            const int nx, const int nz,
            const float dx, const float dz,
            const std::vector<float>::const_iterator height)
    {
        openvrml::vec3f Vx, Vz;

        if (i > 0 && i < nx - 1) {
            Vx.x(float(2.0 * dx));
            Vx.y(*(height + 1) - *(height - 1));
        } else if (i == 0) {
            Vx.x(dx);
            Vx.y(*(height + 1) - *(height));
        } else {
            Vx.x(dx);
            Vx.y(*(height) - *(height - 1));
        }
        Vx.z(0.0);

        Vz.x(0.0);
        if (j > 0 && j < nz - 1) {
            Vz.y(*(height + nx) - *(height - nx));
            Vz.z(float(2.0 * dz));
        } else if (j == 0) {
            Vz.y(*(height+nx) - *(height));
            Vz.z(dz);
        } else {
            Vz.y(*(height) - *(height - nx));
            Vz.z(dz);
        }

        return Vz * Vx;
    }
}


/**
 * @brief Insert an elevation grid into a display list.
 *
 * @param[in] mask
 * @param[in] height        height field.
 * @param[in] xDimension    vertices in the x direction.
 * @param[in] zDimension    vertices in the z direction.
 * @param[in] xSpacing      distance between vertices in the x direction.
 * @param[in] zSpacing      distance between vertices in the z direction.
 * @param[in] color         colors.
 * @param[in] normal        normals.
 * @param[in] texCoord      texture coordinates.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_elevation_grid(const unsigned int mask,
                                            const std::vector<float> & height,
                                            const int32 xDimension,
                                            const int32 zDimension,
                                            const float xSpacing,
                                            const float zSpacing,
                                            const std::vector<color> & color,
                                            const std::vector<vec3f> & normal,
                                            const std::vector<vec2f> & texCoord)
{
    int32 i, j;
    float x, z;

    GLuint glid = 0;

    if (!this->select_mode) {
        glid = glGenLists(1);
        glNewList( glid, GL_COMPILE_AND_EXECUTE );
    }

    this->begin_geometry();

    // Face orientation & culling
    glFrontFace((mask & mask_ccw) ? GL_CCW : GL_CW);
    if (!(mask & mask_solid)) { glDisable(GL_CULL_FACE); }

    std::vector<float>::const_iterator h = height.begin();
    std::vector<openvrml::color>::const_iterator c = color.begin();
    std::vector<vec3f>::const_iterator n = normal.begin();
    std::vector<vec2f>::const_iterator tc = texCoord.begin();

    // x varies fastest
    for (j = 0; j < zDimension - 1; ++j) {
        float s0, s1, t0 = 0.0, t1 = 0.0;

        z = zSpacing * j;
        if (texCoord.empty()) {
            t0 = float(j)     / (zDimension - 1);
            t1 = float(j + 1) / (zDimension - 1);
        }

        glBegin(GL_QUAD_STRIP);

        for (i = 0; i < xDimension; ++i) {
            x = xSpacing * i;

            if (!color.empty() && ((mask & mask_color_per_vertex)
                                    || (i < xDimension - 1))) {
                glColor3fv(&(*c)[0]);
            }

            if (!normal.empty() && ((mask & mask_normal_per_vertex)
                                    || (i < xDimension - 1))) {
                glNormal3fv(&(*n)[0]);
            } else if (normal.empty()) {
                if (mask & mask_normal_per_vertex) {
                    const vec3f N =
                            elevationVertexNormal(i, j,
                                                  xDimension, zDimension,
                                                  xSpacing, zSpacing,
                                                  h);
                    glNormal3fv(&N[0]);
                } else if (i < xDimension - 1) {
                    //
                    // Normal per face.
                    //
                    const vec3f vx(xSpacing, *(h + 1) - *h, 0.0);
                    const vec3f vz(0.0, *(h + xDimension) - *h, zSpacing);
                    glNormal3fv(&(vx * vz)[0]);
                }
            }

            if (!texCoord.empty()) {
                s0 = tc->x();
                t0 = tc->y();
                s1 = (tc + xDimension * 2)->x();
                t1 = (tc + xDimension * 2)->y();
                ++tc;
            } else {
                s0 = s1 = float(i) / (xDimension - 1);
            }

            glTexCoord2f(s0, t0);
            glVertex3f(x, *h, z);

            // Vertex from next row
            if (!color.empty() && (mask & mask_color_per_vertex)) {
                glColor3fv(&(*(c + xDimension))[0]);
            }

            if (mask & mask_normal_per_vertex) {
                if (!normal.empty()) {
                    glNormal3fv(&(*(n + xDimension))[0]);
                } else {
                    const vec3f N =
                            elevationVertexNormal(i, j + 1,
                                                  xDimension, zDimension,
                                                  xSpacing, zSpacing,
                                                  h + xDimension);
                    glNormal3fv(&N[0]);
                }
            }

            glTexCoord2f(s1, t1);
            glVertex3f(x, *(h + xDimension), z + zSpacing);

            ++h;
            if (!color.empty()) { ++c; }
            if (!normal.empty()) { ++n; }
        }

        glEnd();
    }

    this->end_geometry();
    if (glid) { glEndList(); }
    return object_t(glid);
}


namespace {

    // Extrusion cap tessellation for non-convex shapes

    struct OPENVRML_GL_LOCAL TessExtrusion {
        const float * c; // coordinates array [nVerts * 3]
        const float * crossSection; // crossSection coordinates [nCrossSection * 2]
        float tcDeltaU, tcDeltaV;
        float tcScaleU, tcScaleV;
        size_t vOffset;
        openvrml::vec3f N; // Normal

        TessExtrusion(const float * c, const float * crossSection,
                      float tcDeltaU, float tcDeltaV,
                      float tcScaleU, float tcScaleV,
                      size_t vOffset, const openvrml::vec3f & N);
    };

    TessExtrusion::TessExtrusion(const float * const c,
                                 const float * const crossSection,
                                 const float tcDeltaU,
                                 const float tcDeltaV,
                                 const float tcScaleU,
                                 const float tcScaleV,
                                 const size_t vOffset,
                                 const openvrml::vec3f & N):
        c(c),
        crossSection(crossSection),
        tcDeltaU(tcDeltaU),
        tcDeltaV(tcDeltaV),
        tcScaleU(tcScaleU),
        tcScaleV(tcScaleV),
        vOffset(vOffset),
        N(N)
    {}
}

extern "C" {
    /**
     * @internal
     *
     * @brief Tesselator callback.
     *
     * As the type of callback functions given to OpenGL, this type must have
     * C linkage.
     */
    typedef GLvoid (OPENVRML_GL_CALLBACK_* TessCB)();

    /**
     * @internal
     */
    void OPENVRML_GL_CALLBACK_ tessExtrusionBegin(const GLenum type,
                                                  void * const pdata)
    {
        const TessExtrusion & p = *static_cast<TessExtrusion *>(pdata);
        glBegin(type);
        glNormal3fv(&p.N[0]);
    }

    /**
     * @internal
     */
    void OPENVRML_GL_CALLBACK_ tessExtrusionVertex(void * const vdata,
                                                   void * const pdata)
    {
        const size_t j = *static_cast<size_t *>(vdata);
        const TessExtrusion & p = *static_cast<TessExtrusion *>(pdata);

        glTexCoord2f((p.crossSection[2 * j] - p.tcDeltaU) * p.tcScaleU,
                     (p.crossSection[2 * j + 1] - p.tcDeltaV) * p.tcScaleV);
        glVertex3fv(&(p.c[3 * (j + p.vOffset)]));
    }
}

namespace {

    OPENVRML_GL_LOCAL void
    insertExtrusionCaps(GLUtesselator & tesselator,
                        const unsigned int mask,
                        const size_t nSpine,
                        const std::vector<openvrml::vec3f> & c,
                        const std::vector<openvrml::vec2f> & cs)
    {
        using std::vector;
        using openvrml::vec2f;
        using openvrml::vec3f;
        using openvrml::gl::viewer;

        // Determine x,z ranges for top & bottom tex coords
        float xz[4] = { cs[0].x(), cs[0].x(), cs[0].y(), cs[0].y() };
        vector<vec2f>::const_iterator csp = cs.begin();
        for (vector<vec2f>::size_type nn = 1; nn < cs.size(); ++nn, ++csp) {
            if (csp->x() < xz[0]) {
                xz[0] = csp->x();
            } else if (csp->x() > xz[1]) {
                xz[1] = csp->x();
            }
            if (csp->y() < xz[2]) {
                xz[2] = csp->y();
            } else if (csp->y() > xz[3]) {
                xz[3] = csp->y();
            }
        }

        float dx = xz[1] - xz[0];
        float dz = xz[3] - xz[2];
        if (!fequal<float>()(dx, 0.0f)) { dx = float(1.0 / dx); }
        if (!fequal<float>()(dz, 0.0f)) { dz = float(1.0 / dz); }

        // If geometry is in dlists, should just always use the tesselator...

        const bool equalEndpts = cs.front() == cs.back();

        if (!(mask & viewer::mask_convex)) {
            gluTessCallback(&tesselator, GLU_TESS_BEGIN_DATA,
                            reinterpret_cast<TessCB>(tessExtrusionBegin));
            gluTessCallback(&tesselator, GLU_TESS_VERTEX_DATA,
                            reinterpret_cast<TessCB>(tessExtrusionVertex));
            gluTessCallback(&tesselator, GLU_TESS_END, glEnd);

            if (mask & viewer::mask_bottom) {
                TessExtrusion bottom(&c[0][0],
                                     &cs[0][0],
                                     xz[0], xz[2],
                                     dx, dz,
                                     0,
                                     indexFaceNormal(0, 1, 2, c));

                gluTessBeginPolygon(&tesselator, &bottom);
                gluTessBeginContour(&tesselator);
                GLdouble v[3];
                // Mesa tesselator doesn;t like closed polys
                size_t j = equalEndpts ? cs.size() - 2 : cs.size() - 1;
                for (; j > 0; --j) {
                    const size_t index = j - 1;
                    v[0] = c[index].x();
                    v[1] = c[index].y();
                    v[2] = c[index].z();
                    gluTessVertex(&tesselator,
                                  v,
                                  const_cast<GLvoid *>(
                                      static_cast<const GLvoid *>(&index)));
                }
                gluTessEndContour(&tesselator);
                gluTessEndPolygon(&tesselator);
            }

            if (mask & viewer::mask_top) {
                size_t n = (nSpine - 1) * cs.size();
                TessExtrusion top(&c[0][0],
                                  &cs[0][0],
                                  xz[0], xz[2],
                                  dx, dz,
                                  n,
                                  indexFaceNormal(n + 2, n + 1, n, c));

                gluTessBeginPolygon(&tesselator, &top);
                gluTessBeginContour(&tesselator);

                GLdouble v[3];
                // Mesa tesselator doesn;t like closed polys
                size_t j = equalEndpts ? 1 : 0;
                for (; j < cs.size(); ++j) {
                    v[0] = c[j + n].x();
                    v[1] = c[j + n].y();
                    v[2] = c[j + n].z();
                    gluTessVertex(&tesselator, v, &j);
                }
                gluTessEndContour(&tesselator);
                gluTessEndPolygon(&tesselator);
            }
        } else {
            //
            // Convex (or not GLU1.2 ...)
            //
            vec3f N; // Normal

            if (mask & viewer::mask_bottom) {
                glBegin(GL_POLYGON);
                N = indexFaceNormal(0, 1, 2, c);
                glNormal3fv(&N[0]);

                for (size_t j = cs.size(); j > 0; --j) {
                    const size_t index = j - 1;
                    glTexCoord2f((cs[index].x() - xz[0]) * dx,
                                 (cs[index].y() - xz[2]) * dz);
                    glVertex3fv(&c[index][0]);
                }
                glEnd();
            }

            if (mask & viewer::mask_top) {
                size_t n = (nSpine - 1) * cs.size();
                glBegin(GL_POLYGON);
                N = indexFaceNormal(n + 2, n + 1, n, c);
                glNormal3fv(&N[0]);

                for (size_t j = 0; j < cs.size(); ++j) {
                    glTexCoord2f((cs[j].x() - xz[0]) * dx,
                                 (cs[j].y() - xz[2]) * dz);
                    glVertex3fv(&c[j + n][0]);
                }
                glEnd();
            }
        }
    }

    /**
     * @brief Build an extrusion.
     *
     * @param[in] orientation
     * @param[in] scale
     * @param[in] crossSection
     * @param[in] spine
     * @param[out] c
     * @param[out] tc
     */
    OPENVRML_GL_LOCAL void
    computeExtrusion_(const std::vector<openvrml::rotation> & orientation,
                      const std::vector<openvrml::vec2f> & scale,
                      const std::vector<openvrml::vec2f> & crossSection,
                      const std::vector<openvrml::vec3f> & spine,
                      std::vector<openvrml::vec3f> & c,
                      std::vector<openvrml::vec2f> & tc)
    {
        using openvrml::rotation;
        using openvrml::vec2f;
        using openvrml::vec3f;
        using openvrml::mat4f;

        size_t i, j, ci;

        // Xscp, Yscp, Zscp- columns of xform matrix to align cross section
        // with spine segments.
        vec3f Xscp(1.0, 0.0, 0.0);
        vec3f Yscp(0.0, 1.0, 0.0);
        vec3f Zscp(0.0, 0.0, 1.0);
        vec3f lastZ;

        // Is the spine a closed curve (last pt == first pt)?
        bool spineClosed = spine.back() == spine.front();

        // Is the spine a straight line?
        bool spineStraight = true;
        for (i = 1; i < spine.size() - 1; ++i) {
            const vec3f v = (spine[i - 1] - spine[i])
                            * (spine[i + 1] - spine[i]);
            if (!fequal<float>()(v.length(), 0.0f)) {
                spineStraight = false;
                lastZ = v.normalize();
                break;
            }
        }

        // If the spine is a straight line, compute a constant SCP xform
        if (spineStraight) {
            const vec3f v1(0.0, 1.0, 0.0);
            const vec3f v2 = spine.back() - spine.front();
            vec3f v3 = v2 * v1;
            float len = v3.length();
            if (!fequal<float>()(len, 0.0f)) {
                //
                // Not aligned with Y axis.
                //
                v3 *= float(1.0 / len);

                const rotation orient(v3, float(acos(v1.dot(v2)))); // Axis/angle
                const mat4f scp = mat4f::rotation(orient); // xform matrix
                Xscp = vec3f(scp[0][0], scp[0][1], scp[0][2]);
                Yscp = vec3f(scp[1][0], scp[1][1], scp[1][2]);
                Zscp = vec3f(scp[2][0], scp[2][1], scp[2][2]);
            }
        }

        // Orientation matrix
        mat4f om;
        if (orientation.size() == 1
                && !fequal<float>()(orientation.front().angle(), 0.0f)) {
            om = mat4f::rotation(orientation.front());
        }

        using std::vector;
        vector<vec2f>::const_iterator s = scale.begin();
        vector<rotation>::const_iterator r = orientation.begin();
        // Compute coordinates, texture coordinates:
        for (i = 0, ci = 0; i < spine.size(); ++i, ci += crossSection.size()) {

            // Scale cross section
            for (j = 0; j < crossSection.size(); ++j) {
                c[ci + j].x(s->x() * crossSection[j].x());
                c[ci + j].y(0.0);
                c[ci + j].z(s->y() * crossSection[j].y());
            }

            // Compute Spine-aligned Cross-section Plane (SCP)
            if (!spineStraight) {
                size_t yi1, yi2, si1, s1i2, s2i2;

                if (spineClosed && (i == 0 || i == spine.size() - 1)) {
                    yi1 = spine.size() - 2;
                    yi2 = 1;
                    si1 = 0;
                    s1i2 = spine.size() - 2;
                    s2i2 = 1;
                } else if (i == 0) {
                    yi1 = 0;
                    yi2 = 1;
                    si1 = 1;
                    s1i2 = 0;
                    s2i2 = 2;
                } else if (i == spine.size() - 1) {
                    yi1 = spine.size() - 2;
                    yi2 = spine.size() - 1;
                    si1 = spine.size() - 2;
                    s1i2 = spine.size() - 3;
                    s2i2 = spine.size() - 1;
                } else {
                    yi1 = i - 1;
                    yi2 = i + 1;
                    si1 = i;
                    s1i2 = i - 1;
                    s2i2 = i + 1;
                }

                Yscp = (spine[yi2] - spine[yi1]).normalize();

                lastZ = Zscp; // Save last Zscp.
                Zscp = (spine[s1i2] - spine[si1]) * (spine[s2i2] - spine[si1]);

                float VlenZ = Zscp.length();
                if (fequal<float>()(VlenZ, 0.0f)) {
                    Zscp = lastZ;
                } else {
                    Zscp *= float(1.0 / VlenZ);
                }

                if (i > 0 && Zscp.dot(lastZ) < 0.0) { Zscp *= -1.0; }

                Xscp = Yscp * Zscp;
            }

            // Rotate cross section into SCP
            for (j = 0; j < crossSection.size(); ++j) {
                float cx, cy, cz;
                cx = c[ci + j].x() * Xscp.x()
                   + c[ci + j].y() * Yscp.x()
                   + c[ci + j].z() * Zscp.x();
                cy = c[ci + j].x() * Xscp.y()
                   + c[ci + j].y() * Yscp.y()
                   + c[ci + j].z() * Zscp.y();
                cz = c[ci + j].x() * Xscp.z()
                   + c[ci + j].y() * Yscp.z()
                   + c[ci + j].z() * Zscp.z();
                c[ci + j].x(cx);
                c[ci + j].y(cy);
                c[ci + j].z(cz);
            }

            //
            // Apply orientation.
            //
            if (!fequal<float>()(r->angle(), 0.0f)) {
                if (orientation.size() > 1) { om = mat4f::rotation(*r); }

                for (j = 0; j < crossSection.size(); ++j) {
                    float cx, cy, cz;
                    cx = c[ci + j].x() * om[0][0]
                       + c[ci + j].y() * om[1][0]
                       + c[ci + j].z() * om[2][0];
                    cy = c[ci + j].x() * om[0][1]
                       + c[ci + j].y() * om[1][1]
                       + c[ci + j].z() * om[2][1];
                    cz = c[ci + j].x() * om[0][2]
                       + c[ci + j].y() * om[1][2]
                       + c[ci + j].z() * om[2][2];
                    c[ci + j].x(cx);
                    c[ci + j].y(cy);
                    c[ci + j].z(cz);
                }
            }

            //
            // Translate cross section.
            //
            for (j = 0; j < crossSection.size(); ++j) {
                c[ci + j] += spine[i];

                // Texture coords
                tc[ci + j].x(float(j) / (crossSection.size() - 1));
                tc[ci + j].y(float(i) / (spine.size() - 1));
            }

            if (scale.size() > 1) { ++s; }
            if (orientation.size() > 1) { ++r; }
        }
    }

    /**
     * @brief Build an extrusion.
     *
     * @param[in] orientation
     * @param[in] scale
     * @param[in] crossSection
     * @param[in] spine
     * @param[out] c
     * @param[out] tc
     * @param[out] faces
     */
    OPENVRML_GL_LOCAL void
    computeExtrusion_(const std::vector<openvrml::rotation> & orientation,
                      const std::vector<openvrml::vec2f> & scale,
                      const std::vector<openvrml::vec2f> & crossSection,
                      const std::vector<openvrml::vec3f> & spine,
                      std::vector<openvrml::vec3f> & c,
                      std::vector<openvrml::vec2f> & tc,
                      std::vector<openvrml::int32> & faces)
    {
        computeExtrusion_(orientation, scale, crossSection, spine, c, tc);

        //
        // Compute face indices.
        //
        size_t polyIndex = 0;
        for (size_t i = 0, ci = 0; i < spine.size() - 1;
                ++i, ci += crossSection.size()) {
            for (size_t j = 0; j < crossSection.size() - 1; ++j) {
                using openvrml::int32;
                faces[polyIndex + 0] = int32(ci + j);
                faces[polyIndex + 1] = int32(ci + j + 1);
                faces[polyIndex + 2] = int32(ci + j + 1 + crossSection.size());
                faces[polyIndex + 3] = int32(ci + j + crossSection.size());
                faces[polyIndex + 4] = -1;
                polyIndex += 5;
            }
        }
    }
}

/**
 * @brief Insert an extrusion into a display list.
 *
 * @param[in] mask
 * @param[in] spine         spine points.
 * @param[in] crossSection  cross-sections.
 * @param[in] orientation   cross-section orientations.
 * @param[in] scale         cross-section scales.
 *
 * @return display object identifier.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_extrusion(
    unsigned int mask,
    const std::vector<vec3f> & spine,
    const std::vector<vec2f> & crossSection,
    const std::vector<openvrml::rotation> & orientation,
    const std::vector<vec2f> & scale)
{
    using std::vector;
    vector<vec3f> c(crossSection.size() * spine.size());
    vector<vec2f> tc(crossSection.size() * spine.size());

    computeExtrusion_(orientation, scale, crossSection, spine, c, tc);

    GLuint glid = 0;

    if (!this->select_mode) {
        glid = glGenLists(1);
        glNewList(glid, GL_COMPILE_AND_EXECUTE);
    }

    this->begin_geometry();

    // Face orientation & culling
    glFrontFace((mask & mask_ccw) ? GL_CCW : GL_CW);
    if (!(mask & mask_solid)) { glDisable(GL_CULL_FACE); }

    // Handle creaseAngle, correct normals, ...
    size_t n = 0;
    for (vector<vec3f>::size_type i = 0;
         i < spine.size() - 1;
         ++i, n += crossSection.size()) {
        glBegin(GL_QUAD_STRIP);
        for (size_t j = 0; j < crossSection.size(); ++j) {
            // Compute normals
            vec3f v1 = j < crossSection.size() - 1
                     ? c[n + j + 1] - c[n + j]
                     : c[n + j] - c[n + j - 1];
            vec3f v2 = c[n + j + crossSection.size()] - c[n + j];
            v1 *= v2;
            glNormal3fv(&v1[0]);

            glTexCoord2fv(&tc[n + j + crossSection.size()][0]);
            glVertex3fv(&c[n + j + crossSection.size()][0]);
            glTexCoord2fv(&tc[n + j][0]);
            glVertex3fv(&c[n + j][0]);
        }
        glEnd();
    }

    // Draw caps. Convex can only impact the caps of an extrusion.
    if (mask & (mask_bottom | mask_top)) {
        insertExtrusionCaps(*this->tesselator,
                            mask,
                            spine.size(),
                            c,
                            crossSection);
    }

    this->end_geometry();
    if (glid) { glEndList(); }
    return object_t(glid);
}

/**
 * @brief Insert a line set into a display list.
 *
 * @param[in] coord             coordinates.
 * @param[in] coordIndex        coordinate indices.
 * @param[in] colorPerVertex    whether colors are applied per-vertex or per-face.
 * @param[in] color             colors.
 * @param[in] colorIndex        color indices.
 *
 * @return display object identifier.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_line_set(const std::vector<vec3f> & coord,
                                      const std::vector<int32> & coordIndex,
                                      bool colorPerVertex,
                                      const std::vector<color> & color,
                                      const std::vector<int32> & colorIndex)
{
    GLuint glid = 0;

    if (coord.size() < 2) { return 0; }

    if (!this->select_mode) {
        glid = glGenLists(1);
        glNewList(glid, GL_COMPILE_AND_EXECUTE);
    }

    this->begin_geometry();

    // Lighting, texturing don't apply to line sets
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    const bool color_per_face = (!color.empty() && !colorPerVertex);
    if (color_per_face) { glShadeModel(GL_FLAT); }

    glBegin(GL_LINE_STRIP);
    if (!color.empty() && color_per_face) {
        const size_t color_index = !colorIndex.empty()
                                 ? colorIndex.front()
                                 : 0;
        glColor3fv(&color[color_index][0]);
    }

    size_t nl = 0;
    for (size_t i = 0; i < coordIndex.size(); ++i) {
        if (coordIndex[i] == -1) {
            glEnd();
            glBegin(GL_LINE_STRIP);
            ++nl;
            if (i < coordIndex.size() - 1 && color_per_face) {
                const int32 index = !colorIndex.empty()
                                  ? int32(colorIndex[nl])
                                  : int32(nl);
                if (size_t(index) < color.size()) {
                    glColor3fv(&color[index][0]);
                }
            }
        } else {
            if (!color.empty() && colorPerVertex) {
                const int32 index = !colorIndex.empty()
                                  ? colorIndex[i]
                                  : coordIndex[i];
                if (size_t(index) < color.size()) {
                    glColor3fv(&color[index][0]);
                }
            }
            if (size_t(coordIndex[i]) < coord.size()) {
                glVertex3fv(&coord[coordIndex[i]][0]);
            }
        }
    }

    glEnd();

    this->end_geometry();

    if (glid) { glEndList(); }

    return object_t(glid);
}

/**
 * @brief Insert a point set into a display list.
 *
 * @param[in] coord     points.
 * @param[in] color     colors.
 *
 * @return display object identifier.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_point_set(const std::vector<vec3f> & coord,
                                       const std::vector<color> & color)
{
    GLuint glid = 0;

    if (!this->select_mode) {
        glid = glGenLists(1);
        glNewList(glid, GL_COMPILE_AND_EXECUTE);
    }

    this->begin_geometry();

    // Lighting, texturing don't apply to points
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_POINTS);

    for (size_t i = 0; i < coord.size(); ++i) {
        if (i < color.size()) { glColor3fv(&color[i][0]); }
        glVertex3fv(&coord[i][0]);
    }

    glEnd();
    this->end_geometry();
    if (glid) { glEndList(); }

    return object_t(glid);
}

namespace {

    OPENVRML_GL_LOCAL void computeBounds(size_t npoints,
                                         const float * points,
                                         float (&bounds)[6])
    {
        if (npoints == 0) {
            std::fill(bounds, bounds + 6, 0.0f);
        } else {
            bounds[0] = bounds[1] = points[0]; // xmin, xmax
            bounds[2] = bounds[3] = points[1]; // ymin, ymax
            bounds[4] = bounds[5] = points[2]; // zmin, zmax

            for (size_t i = 1; i < npoints; ++i) {
                points += 3;
                if (points[0] < bounds[0])      bounds[0] = points[0];
                else if (points[0] > bounds[1]) bounds[1] = points[0];
                if (points[1] < bounds[2])      bounds[2] = points[1];
                else if (points[1] > bounds[3]) bounds[3] = points[1];
                if (points[2] < bounds[4])      bounds[4] = points[2];
                else if (points[2] > bounds[5]) bounds[5] = points[2];
            }
        }
    }

    OPENVRML_GL_LOCAL void texGenParams(float (&bounds)[6],  // xmin,xmax, ymin,ymax, zmin,zmax
                                        int (&axes)[2],     // s, t
                                        float (&params)[4]) // s0, 1/sSize, t0, 1/tSize
    {
        axes[0] = 0;
        axes[1] = 1;
        params[0] = params[1] = params[2] = params[3] = 0.0;

        for (int nb = 0; nb < 3; ++nb) {
            float db = bounds[2 * nb + 1] - bounds[2 * nb];
            if (db > params[1]) {
                axes[1] = axes[0];
                axes[0] = nb;
                params[2] = params[0];
                params[3] = params[1];
                params[0] = bounds[2 * nb];
                params[1] = db;
            } else if (db > params[3]) {
                axes[1] = nb;
                params[2] = bounds[2 * nb];
                params[3] = db;
            }
        }

        // If two of the dimensions are zero, give up.
        if (fequal<float>()(params[1], 0.0f)
                || fequal<float>()(params[3], 0.0f)) {
            return;
        }

        params[1] = float(1.0 / params[1]);
        params[3] = float(1.0 / params[3]);
    }

    OPENVRML_GL_LOCAL void insertShellConvex(ShellData * const s)
    {
        using openvrml::vec3f;
        using openvrml::gl::viewer;

        vec3f N;
        size_t i, nf = 0;                        // Number of faces

        for (i = 0; i < s->coordIndex.size(); ++i) {
            if (i == 0 || s->coordIndex[i] == -1) {
                if (i > 0) { glEnd(); }
                if (i == s->coordIndex.size() - 1) { break; }

                glBegin(GL_POLYGON);

                // Per-face attributes
                if (!s->color.empty()
                        && !(s->mask & viewer::mask_color_per_vertex)) {
                    const size_t index = !s->colorIndex.empty()
                                       ? s->colorIndex[nf]
                                       : nf;
                    glColor3fv(&s->color[index][0]);
                }

                if (! (s->mask & viewer::mask_normal_per_vertex)) {
                    size_t i1 = (i == 0)
                           ? 0
                           : i + 1;
                    if (!s->normal.empty()) {
                        const size_t index = !s->normalIndex.empty()
                                           ? s->normalIndex[nf]
                                           : nf;
                        glNormal3fv(&s->normal[index][0]);
                    } else if (i < s->coordIndex.size() - 4
                        && s->coordIndex[i1] >= 0
                        && s->coordIndex[i1 + 1] >= 0
                        && s->coordIndex[i1 + 2] >= 0) {
                        N = indexFaceNormal(s->coordIndex[i1],
                                            s->coordIndex[i1 + 1],
                                            s->coordIndex[i1 + 2],
                                            s->coord);

                        // Flip normal if primitiv-orientation is clockwise
                        if (!(s->mask & viewer::mask_ccw)) { N = -N; }
                        glNormal3fv(&N[0]);
                    }
                }

                ++nf;
            }

            if (s->coordIndex[i] >= 0) {
                // Per-vertex attributes
                if (!s->color.empty()
                        && (s->mask & viewer::mask_color_per_vertex)) {
                    const size_t index = !s->colorIndex.empty()
                                       ? s->colorIndex[i]
                                       : s->coordIndex[i];
                    glColor3fv(&s->color[index][0]);
                }

                if (s->mask & viewer::mask_normal_per_vertex) {
                    if (!s->normal.empty()) {
                        const size_t index = !s->normalIndex.empty()
                                           ? s->normalIndex[i]
                                           : s->coordIndex[i];
                        glNormal3fv(&s->normal[index][0]);
                    } else {
                        ; // Generate per-vertex normal here...
                    }
                }

                const vec3f & v = s->coord[s->coordIndex[i]];
                if (!s->texCoord.empty()) {
                    const size_t index = !s->texCoordIndex.empty()
                                       ? s->texCoordIndex[i]
                                       : s->coordIndex[i];
                    glTexCoord2fv(&s->texCoord[index][0]);
                } else {
                    float c0, c1;
                    c0 = (v[s->texAxes[0]] - s->texParams[0])
                        * s->texParams[1];
                    c1 = (v[s->texAxes[1]] - s->texParams[2])
                        * s->texParams[3];
                    glTexCoord2f(c0, c1);
                }

                glVertex3fv(&v[0]);
            }
        }

        // Watch out for no terminating -1 in face list
        // two ways to break out:
        //   i > 0 && i == coordIndex.size() - 1 && coordIndex[i] == -1
        //   i == coordIndex.size()
        //
        if (i >= s->coordIndex.size()) {
            if (s->coordIndex[i - 1] >= 0) { glEnd(); }
        } else {
            if (s->coordIndex[i] >= 0) { glEnd(); }
        }
    }

    OPENVRML_GL_LOCAL void
    insertShellTess(GLUtesselator & tessobj,
                    const std::vector<vertex_data> & vertices,
                    const std::vector<openvrml::int32> & coord_index,
                    const bool color_per_face,
                    const std::vector<openvrml::color> & color,
                    const std::vector<openvrml::int32> & color_index,
                    const bool normal_per_face,
                    const std::vector<openvrml::vec3f> & normal,
                    const std::vector<openvrml::int32> & normal_index)
    {
        using std::vector;
        gluTessCallback(&tessobj,
                        GLU_TESS_BEGIN_DATA,
                        reinterpret_cast<TessCB>(shell_tess_begin));
        gluTessCallback(&tessobj,
                        GLU_TESS_VERTEX,
                        reinterpret_cast<TessCB>(shell_tess_vertex));
        gluTessCallback(&tessobj,
                        GLU_TESS_COMBINE_DATA,
                        reinterpret_cast<TessCB>(shell_tess_combine));
        gluTessCallback(&tessobj,
                        GLU_TESS_END_DATA,
                        reinterpret_cast<TessCB>(shell_tess_end));
        gluTessCallback(&tessobj, GLU_TESS_ERROR,
                        reinterpret_cast<TessCB>(tess_error));

        shell_tess_user_data user_data;
        if (color_per_face) {
            if (!color.empty()) {
                user_data.face_color = &color;
            }
            if (!color_index.empty()) {
                user_data.face_color_index = &color_index;
            }
        }
        if (normal_per_face) {
            if (!normal.empty()) {
                user_data.face_normal = &normal;
            }
            if (!normal_index.empty()) {
                user_data.face_normal_index = &normal_index;
            }
        }

        gluTessBeginPolygon(&tessobj, &user_data);
        gluTessBeginContour(&tessobj);
        for (vector<openvrml::int32>::size_type i = 0;
             i < coord_index.size();
             ++i) {
            if (coord_index[i] != -1) {
                gluTessVertex(&tessobj,
                              const_cast<GLdouble *>(
                                  vertices[coord_index[i]].coord),
                              &const_cast<vertex_data &>(
                                  vertices[coord_index[i]]));
            } else {
                gluTessEndContour(&tessobj);
                gluTessEndPolygon(&tessobj);
                if (i != coord_index.size() - 1) {
                    gluTessBeginPolygon(&tessobj, &user_data);
                    gluTessBeginContour(&tessobj);
                }
            }
        }

        // Watch out for no terminating -1 in face list
        if (coord_index.back() != -1) {
            gluTessEndContour(&tessobj);
            gluTessEndPolygon(&tessobj);
        }
    }
}


/**
 * @brief Insert a shell into a display list.
 *
 * @param[in] mask
 * @param[in] coord             coordinates.
 * @param[in] coord_index       coordinate indices.
 * @param[in] color             colors.
 * @param[in] color_index       color indices.
 * @param[in] normal            normals.
 * @param[in] normal_index      normal indices.
 * @param[in] tex_coord         texture coordinates.
 * @param[in] tex_coord_index   texture coordinate indices.
 *
 * @return display object identifier.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_shell(unsigned int mask,
                                   const std::vector<vec3f> & coord,
                                   const std::vector<int32> & coord_index,
                                   const std::vector<color> & color,
                                   const std::vector<int32> & color_index,
                                   const std::vector<vec3f> & normal,
                                   const std::vector<int32> & normal_index,
                                   const std::vector<vec2f> & tex_coord,
                                   const std::vector<int32> & tex_coord_index)
{
    using std::vector;
    if (coord_index.size() < 4) { return 0; } // 3 pts and a trailing -1

    // Texture coordinate generation parameters.
    int texAxes[2];                        // Map s,t to x,y,z
    float texParams[4];                // s0, 1/sSize, t0, 1/tSize

    // Compute bounding box for texture coord generation and lighting.
    if (tex_coord.empty()) { // || any positional lights are active...
        float bounds[6]; // xmin,xmax, ymin,ymax, zmin,zmax
        computeBounds(coord.size(), &coord[0][0], bounds);

        // do the bounds intersect the radius of any active positional lights.
        texGenParams(bounds, texAxes, texParams);
        if (fequal<float>()(texParams[1], 0.0f)
                || fequal<float>()(texParams[3], 0.0f)) {
            return 0;
        }
    }

    GLuint glid = 0;

    if (!this->select_mode) {
        glid = glGenLists(1);
        glNewList(glid, GL_COMPILE_AND_EXECUTE);
    }

    this->begin_geometry();

    // Face orientation & culling
    glFrontFace((mask & mask_ccw) ? GL_CCW : GL_CW);
    if (!(mask & mask_solid)) { glDisable(GL_CULL_FACE); }

    // Color per face
    if (!color.empty() && ! (mask & mask_color_per_vertex)) {
        glShadeModel(GL_FLAT);
    }

    // -------------------------------------------------------

    // Generation of per vertex normals isn't implemented yet...
    if (normal.empty() && (mask & mask_normal_per_vertex)) {
        mask &= ~mask_normal_per_vertex;
    }

    // Handle non-convex polys
    if (mask & mask_convex) {
        ShellData s(mask,
                    coord, coord_index,
                    color, color_index,
                    normal, normal_index,
                    tex_coord, tex_coord_index,
                    texAxes, texParams, 0, 0);

        insertShellConvex(&s);
    } else {
        vector<vertex_data> vertices(coord.size()); // Throws std::bad_alloc.
        for (vector<vertex_data>::size_type i = 0; i < vertices.size(); ++i) {
            vertices[i].coord[0] = coord[i].x();
            vertices[i].coord[1] = coord[i].y();
            vertices[i].coord[2] = coord[i].z();
        }

        if (mask & mask_color_per_vertex) {
            if (color_index.empty()) {
                for (vector<openvrml::color>::size_type i = 0;
                     i < color.size();
                     ++i) {
                    vertices[i].color = &color[i];
                }
            } else {
                for (vector<int32>::size_type i = 0;
                     i < color_index.size();
                     ++i) {
                    vertices[i].color = &color[color_index[i]];
                }
            }
        }
        if (mask & mask_normal_per_vertex) {
            if (normal_index.empty()) {
                for (vector<openvrml::vec3f>::size_type i = 0;
                     i < normal.size();
                     ++i) {
                    vertices[i].normal = &normal[i];
                }
            } else {
                for (vector<int32>::size_type i = 0;
                     i < normal_index.size();
                     ++i) {
                    vertices[i].normal = &normal[normal_index[i]];
                }
            }
        }

        if (tex_coord_index.empty()) {
            for (vector<openvrml::vec2f>::size_type i = 0;
                 i < tex_coord.size();
                 ++i) {
                vertices[i].tex_coord = &tex_coord[i];
            }
        } else {
            for (vector<int32>::size_type i = 0;
                 i < tex_coord_index.size();
                 ++i) {
                vertices[tex_coord_index[i]].tex_coord =
                    &tex_coord[tex_coord_index[i]];
            }
        }

        const bool color_per_face = !(mask & mask_color_per_vertex);
        const bool normal_per_face = !(mask & mask_color_per_vertex);
        insertShellTess(*this->tesselator,
                        vertices,
                        coord_index,
                        color_per_face,
                        color,
                        color_index,
                        normal_per_face,
                        normal,
                        normal_index);
    }

    this->end_geometry();

    if (glid) { glEndList(); }

    return object_t(glid);
}

namespace {

    OPENVRML_GL_LOCAL void computeSphere(const double radius,
                                         const int numLatLong,
                                         float (*c)[3],
                                         float (*tc)[3],
                                         int *faces)
    {
        double r, angle, x, y, z;
        int i, j, polyIndex;

        // Compute coordinates, texture coordinates:
        for (i = 0; i < numLatLong; ++i) {
            /*y = 2.0 * ( ((double)i) / (numLatLong-1) ) - 1.0;*/
            angle = ( i * pi / (numLatLong-1) ) - pi_2;
            y = sin( angle );
            r = sqrt( 1.0 - y*y );
            for (j = 0; j < numLatLong; ++j) {
                angle = 2 * pi * ((double)j) / numLatLong;
                x = - sin(angle)*r;
                z = - cos(angle)*r;
                c[i * numLatLong + j][0] = float(radius * x);
                c[i * numLatLong + j][1] = float(radius * y);
                c[i * numLatLong + j][2] = float(radius * z);
                if (tc) {
                    tc[i * numLatLong + j][0] = float(j) / numLatLong;
                    tc[i * numLatLong + j][1] = float(i) / numLatLong;
                    tc[i * numLatLong + j][2] = 0.0;
                }
            }
        }

        // And compute indices:
        if (faces) {
            for (i = 0; i < numLatLong-1; ++i) {
                for (j = 0; j < numLatLong; ++j) {
                    polyIndex = 5 * (i * numLatLong + j);
                    faces[polyIndex + 0] = i * numLatLong + j;
                    faces[polyIndex + 1] = i * numLatLong
                                           + (j + 1) % numLatLong;
                    faces[polyIndex + 2] = (i + 1) * numLatLong
                                           + (j + 1) % numLatLong;
                    faces[polyIndex + 3] = (i + 1) * numLatLong + j;
                    faces[polyIndex + 4] = -1;  // quad
                }
            }
        }
    }
}

/**
 * @brief Insert a sphere into a display list.
 *
 * @param[in] radius    sphere radius.
 *
 * @return display object identifier.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_sphere(const float radius)
{
    GLuint glid = 0;

    if (!this->select_mode) {
        glid = glGenLists(1);
        glNewList(glid, GL_COMPILE_AND_EXECUTE);
    }

    static const size_t numLatLong = 10;
    static const size_t npts = numLatLong * numLatLong;

    float c[npts][3];
    float tc[npts][3];

    // should only compute tc if a texture is present...
    computeSphere(radius, numLatLong, c, tc, 0);

    this->begin_geometry();

    for (size_t i = 0; i < numLatLong - 1; ++i) {
        size_t n = i * numLatLong;

        glBegin(GL_QUAD_STRIP);

        for (size_t j = 0; j < numLatLong; ++j) {
            glTexCoord2f(tc[n + j + numLatLong][0], tc[n + j + numLatLong][1]);
            glNormal3fv(&c[n + j + numLatLong][0]);
            glVertex3fv(&c[n + j + numLatLong][0]);

            glTexCoord2f(tc[n+j][0], tc[n+j][1]);
            glNormal3fv(&c[n+j][0]);
            glVertex3fv(&c[n+j][0]);
        }

        glTexCoord2f(1.0, tc[n + numLatLong][1]);
        glNormal3fv(&c[n + numLatLong][0]);
        glVertex3fv(&c[n + numLatLong][0]);

        glTexCoord2f(1.0, tc[n][1]);
        glNormal3fv(&c[n][0]);
        glVertex3fv(&c[n][0]);

        glEnd();
    }

    this->end_geometry();
    if (glid) { glEndList(); }

    return object_t(glid);
}

/**
 * @brief Insert a directional light into a display list.
 *
 * @param[in] ambientIntensity  ambient intensity.
 * @param[in] intensity         intensity.
 * @param[in] color             color.
 * @param[in] direction         direction.
 *
 * @return display object identifier.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_dir_light(const float ambientIntensity,
                                       const float intensity,
                                       const color & color,
                                       const vec3f & direction)
{
    float amb[4] = { ambientIntensity * color.r(),
                     ambientIntensity * color.g(),
                     ambientIntensity * color.b(),
                     1.0 };
    float dif[4] = { intensity * color.r(),
                     intensity * color.g(),
                     intensity * color.b(),
                     1.0 };
    float pos[4] = { direction.x(), direction.y(), -direction.z(), 0.0 };

    // Find an unused light, give up if none left.
    int i;
    for (i = 0; i < max_lights; ++i) {
        if (this->light_info_[i].type == viewer::light_unused) { break; }
    }
    if (i == max_lights) { return 0; }

    this->light_info_[i].type = viewer::light_directional;
    this->light_info_[i].nesting_level = 0;
    GLenum light = GLenum(GL_LIGHT0 + i);

    glEnable(light);
    glLightfv(light, GL_AMBIENT, amb);
    glLightfv(light, GL_DIFFUSE, dif);
    glLightfv(light, GL_POSITION, pos);

    // Disable any old point/spot settings
    glLightf(light, GL_CONSTANT_ATTENUATION, 1.0);
    glLightf(light, GL_LINEAR_ATTENUATION, 0.0);
    glLightf(light, GL_QUADRATIC_ATTENUATION, 0.0);

    glLightf(light, GL_SPOT_CUTOFF, 180.0);
    glLightf(light, GL_SPOT_EXPONENT, 0.0);

    return 0;
}

/**
 * @brief Insert a point light into a display list.
 *
 * @param[in] ambientIntensity  ambient intensity.
 * @param[in] attenuation       attenuation.
 * @param[in] color             color.
 * @param[in] intensity         intensity.
 * @param[in] location          location.
 * @param[in] radius            radius.
 *
 * @return display object identifier.
 *
 * @todo Only objects within radius should be lit by each PointLight.
 *      Test each object drawn against each point light and enable
 *      the lights accordingly? Get light and geometry into consistent
 *      coordinates first.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_point_light(const float ambientIntensity,
                                         const vec3f & attenuation,
                                         const color & color,
                                         const float intensity,
                                         const vec3f & location,
                                         const float radius)
{
    float amb[4] = { ambientIntensity * color.r(),
                     ambientIntensity * color.g(),
                     ambientIntensity * color.b(),
                     1.0 };
    float dif[4] = { intensity * color.r(),
                     intensity * color.g(),
                     intensity * color.b(),
                     1.0 };
    float pos[4] = { location.x(), location.y(), location.z(), 1.0 };

    // Find an unused light, give up if none left.
    int i;
    for (i = 0; i < max_lights; ++i) {
        if (this->light_info_[i].type == viewer::light_unused) { break; }
    }
    if (i == max_lights) { return 0; }

    this->light_info_[i].type = viewer::light_positional;
    this->light_info_[i].location = location;
    this->light_info_[i].radius_squared = radius * radius;

    GLenum light(GL_LIGHT0 + i);

    // should be enabled/disabled per geometry based on distance & radius...
    glEnable(light);
    glLightfv(light, GL_AMBIENT, amb);
    glLightfv(light, GL_DIFFUSE, dif);
    glLightfv(light, GL_POSITION, pos);

    glLightf(light, GL_CONSTANT_ATTENUATION, attenuation[0]);
    glLightf(light, GL_LINEAR_ATTENUATION, attenuation[1]);
    glLightf(light, GL_QUADRATIC_ATTENUATION, attenuation[2]);

    // Disable old spot settings
    glLightf(light, GL_SPOT_CUTOFF, 180.0);
    glLightf(light, GL_SPOT_EXPONENT, 0.0);

    return 0;
}

/**
 * @brief Insert a point light into a display list.
 *
 * @param[in] ambientIntensity  ambient intensity.
 * @param[in] attenuation       attenuation.
 * @param[in] beamWidth         beam width.
 * @param[in] color             color.
 * @param[in] cutOffAngle       cut-off angle.
 * @param[in] direction         direction.
 * @param[in] intensity         intensity.
 * @param[in] location          location.
 * @param[in] radius            radius.
 *
 * @return 0.
 *
 * @todo Same comments as for PointLight apply here.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_spot_light(const float ambientIntensity,
                                        const vec3f & attenuation,
                                        const float beamWidth,
                                        const color & color,
                                        const float cutOffAngle,
                                        const vec3f & direction,
                                        const float intensity,
                                        const vec3f & location,
                                        const float radius)
{
    float amb[4] = { ambientIntensity * color.r(),
                     ambientIntensity * color.g(),
                     ambientIntensity * color.b(),
                     1.0 };
    float dif[4] = { intensity * color.r(),
                     intensity * color.g(),
                     intensity * color.b(),
                     1.0 };
    float pos[4] = { location.x(), location.y(), location.z(), 1.0 };


    // Find an unused light, give up if none left.
    int i;
    for (i = 0; i < max_lights; ++i) {
        if (this->light_info_[i].type == viewer::light_unused) { break; }
    }
    if (i == max_lights) { return 0; }

    this->light_info_[i].type = viewer::light_positional;
    this->light_info_[i].location = location;
    this->light_info_[i].radius_squared = radius * radius;

    GLenum light(GL_LIGHT0 + i);

    // should be enabled/disabled per geometry based on distance & radius...
    glEnable(light);
    glLightfv(light, GL_AMBIENT, amb);
    glLightfv(light, GL_DIFFUSE, dif);
    glLightfv(light, GL_POSITION, pos);

    glLightf(light, GL_CONSTANT_ATTENUATION, attenuation[0]);
    glLightf(light, GL_LINEAR_ATTENUATION, attenuation[1]);
    glLightf(light, GL_QUADRATIC_ATTENUATION, attenuation[2]);

    glLightfv(light, GL_SPOT_DIRECTION, &direction[0]);
    glLightf(light, GL_SPOT_CUTOFF, GLfloat(cutOffAngle * 180.0 / pi));
    // The exponential dropoff is not right/spec compliant...
    glLightf(light, GL_SPOT_EXPONENT, beamWidth < cutOffAngle ? 1.0f : 0.0f);

    return 0;
}


/**
 * @brief Insert a reference to an existing object into a display list.
 *
 * @return 0.
 */
openvrml::gl::viewer::object_t
openvrml::gl::viewer::insert_reference(const object_t existing_object)
{
    glCallList(GLuint(existing_object));
    return 0;
}

/**
 * @brief Remove an object from the display list.
 *
 * @param[in] ref   object handle.
 */
void openvrml::gl::viewer::remove_object(const object_t ref)
{
    glDeleteLists(GLuint(ref), 1);
}

/**
 * @brief Enable/disable lighting.
 *
 * @param[in] val whether lighting should be enabled.
 */
void openvrml::gl::viewer::enable_lighting(const bool val)
{
    if (val) {
        if (this->lit) {
            glEnable(GL_LIGHTING);
        }
    } else {
        glDisable(GL_LIGHTING);
    }
}

/**
 * @brief Set the color.
 *
 * @param[in] rgb   red, green, and blue components.
 * @param[in] a     alpha (transparency) component.
 */
void openvrml::gl::viewer::set_color(const color & rgb, const float a)
{
    glColor4f(rgb.r(), rgb.g(), rgb.b(), a);
}

/**
 * @brief Set the fog.
 *
 * @param[in] color             fog color.
 * @param[in] visibilityRange   the distance at which objects are fully obscured by
 *                              fog.
 * @param[in] type              fog type.
 */
void openvrml::gl::viewer::set_fog(const color & color,
                                   const float visibilityRange,
                                   const char * const type)
{
    static const std::string exponential("EXPONENTIAL");
    const GLfloat fogColor[4] = { color.r(), color.g(), color.b(), 1.0 };
    const GLint fogMode = (type == exponential)
                        ? GL_EXP
                        : GL_LINEAR;

    glEnable(GL_FOG);
    glFogf(GL_FOG_START, 1.5); // XXX What should this be?
    glFogf(GL_FOG_END, visibilityRange);
    glFogi(GL_FOG_MODE, fogMode);
    glFogfv(GL_FOG_COLOR, fogColor);
}

/**
 * @brief Set the material.
 *
 * @param[in] ambientIntensity  ambient intensity.
 * @param[in] diffuseColor      diffuse color.
 * @param[in] emissiveColor     emissive color.
 * @param[in] shininess         shininess.
 * @param[in] specularColor     specular color.
 * @param[in] transparency      transparency.
 */
void openvrml::gl::viewer::set_material(const float ambientIntensity,
                                        const color & diffuseColor,
                                        const color & emissiveColor,
                                        const float shininess,
                                        const color & specularColor,
                                        const float transparency)
{
    const float alpha = 1.0f - transparency;

    const float ambient[4] = { ambientIntensity * diffuseColor.r(),
                               ambientIntensity * diffuseColor.g(),
                               ambientIntensity * diffuseColor.b(),
                               alpha };
    const float diffuse[4] = { diffuseColor.r(),
                               diffuseColor.g(),
                               diffuseColor.b(),
                               alpha };
    const float emission[4] = { emissiveColor.r(),
                                emissiveColor.g(),
                                emissiveColor.b(),
                                alpha };
    const float specular[4] = { specularColor.r(),
                                specularColor.g(),
                                specularColor.b(),
                                alpha };

    // XXX doesn't work right yet (need alpha render pass...)
    if (this->blend && !fequal<float>()(transparency, 0.0f)) {
        glEnable(GL_BLEND);
    }

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
    //
    // In OGL standard range of shininess is [0.0,128.0]
    // In VRML97 the range is [0.0,1.0]
    //
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * 128);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
}

/**
 * @brief Set the material mode.
 *
 * @note This hack is necessary because setting the color mode needs to know
 *       about the appearance (presence & components of texture) and the
 *       geometry (presence of colors). Putting this stuff in either
 *       insert_texture or insert_<geometry> causes problems when the texture
 *       or geometry node is USE'd with a different context.
 *
 * @param[in] tex_components    texture components.
 * @param[in] geometry_color    geometry color.
 */
void openvrml::gl::viewer::set_material_mode(const size_t tex_components,
                                             const bool geometry_color)
{
    if (tex_components && this->texture && !this->wireframe) {
        glEnable(GL_TEXTURE_2D);

        // This is a hack: if per-{face,vertex} colors are specified,
        // they take precedence over textures with GL_MODULATE. The
        // textures won't be lit this way but at least they show up...
        if (tex_components > 2 && geometry_color) {
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        } else {
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        }
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    if (geometry_color && tex_components < 3 /* && lighting enabled... */ ) {
        glEnable(GL_COLOR_MATERIAL);
    } else {
        glDisable(GL_COLOR_MATERIAL);
    }
}

/**
 * @brief Indicate that a node should be sensitive to the pointing device.
 *
 * @param[in] object    a node.
 */
void openvrml::gl::viewer::set_sensitive(node * object)
{
    if (object) {
        // should make this dynamic...
        if (this->sensitive == viewer::maxsensitive) {
            OPENVRML_GL_PRINT_MESSAGE_("Internal Error: too many sensitive "
                                       "objects.");
            return;
        }

        // push name, increment object count
        this->sensitive_object[this->sensitive] = object;
        glPushName(GLuint(++this->sensitive)); // array index + 1
    } else {
        glPopName();
    }
}

namespace {

    OPENVRML_GL_LOCAL inline bool power_of_2(unsigned long n)
    {
        return !(n & (n - 1));
    }
}

/**
 * @brief Create a texture object.
 *
 * @param[in] img           image.
 * @param[in] repeat_s      repeat in the S direction.
 * @param[in] repeat_t      repeat in the T direction.
 * @param[in] retainHint    whether the texture is likely to be reused.
 *
 * @return a handle to the inserted texture.
 */
openvrml::gl::viewer::texture_object_t
openvrml::gl::viewer::insert_texture(const image & img,
                                     bool repeat_s,
                                     bool repeat_t,
                                     bool retainHint)
{
    using std::vector;

    static const GLenum fmt[] = {
        GL_LUMINANCE,       // single component
        GL_LUMINANCE_ALPHA, // 2 components
        GL_RGB,             // 3 components
        GL_RGBA             // 4 components
    };

    if (img.array().empty()) { return 0; }

    GLsizei width = GLsizei(img.x());
    GLsizei height = GLsizei(img.y());
    const GLubyte * pixels = &img.array()[0];

    //
    // Rescale the texture if necessary.
    //
    const GLint max_texture_size =
        gl_capabilities::instance().max_texture_size;
    if (width > max_texture_size) { width = max_texture_size; }
    if (height > max_texture_size) { height = max_texture_size; }

    while (!power_of_2(width)) { --width; }
    while (!power_of_2(height)) { --height; }

    vector<GLubyte> rescaled_pixels;

    if (size_t(width) != img.x() || size_t(height) != img.y()) {
        //
        // Throws std::bad_alloc.
        //
        rescaled_pixels.resize(img.comp() * width * height);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        const GLint result = gluScaleImage(fmt[img.comp() - 1],
                                           GLsizei(img.x()),
                                           GLsizei(img.y()),
                                           GL_UNSIGNED_BYTE,
                                           &img.array()[0],
                                           width,
                                           height,
                                           GL_UNSIGNED_BYTE,
                                           &rescaled_pixels[0]);
        pixels = (result == GL_NO_ERROR)
               ? &rescaled_pixels[0]
               : 0;
    }

    //
    // Pixels are lower left to upper right by row.
    //
    GLuint glid = 0;

    if (pixels) {
        if (this->select_mode) { return 0; }

        // Enable blending if needed
        if (this->blend && (img.comp() == 2 || img.comp() == 4)) {
            glEnable(GL_BLEND);
        }

#if USE_TEXTURE_DISPLAY_LISTS
        if (retainHint) {
            glGenTextures(1, &glid);
            glBindTexture(GL_TEXTURE_2D, glid);
        }
#endif

        // Texturing is enabled in setMaterialMode
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_S,
                        repeat_s ? GL_REPEAT : GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,
                        GL_TEXTURE_WRAP_T,
                        repeat_t ? GL_REPEAT : GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        static const GLint level = 0;
        static const GLint border = 0;
        glTexImage2D(GL_TEXTURE_2D,
                     level,
                     GLint(img.comp()),
                     width,
                     height,
                     border,
                     fmt[img.comp() - 1],
                     GL_UNSIGNED_BYTE,
                     pixels);
        OPENVRML_GL_PRINT_ERRORS_;
    }

    return texture_object_t(glid);
}

/**
 * @brief Insert a texture into the display list from an existing handle.
 *
 * @param[in] ref           texture handle.
 * @param[in] components    number of components.
 */
void openvrml::gl::viewer::insert_texture_reference(const texture_object_t ref,
                                                    const size_t components)
{
#if USE_TEXTURE_DISPLAY_LISTS
    // Enable blending if needed
    if (this->blend && (components == 2 || components == 4)) {
        glEnable(GL_BLEND);
    }
    glBindTexture(GL_TEXTURE_2D, GLuint(ref));
#endif
}


/**
 * @brief Remove a texture from the display list.
 *
 * @param[in] ref   texture handle.
 */
void openvrml::gl::viewer::remove_texture_object(const texture_object_t ref)
{
#if USE_TEXTURE_DISPLAY_LISTS
    const GLuint glid = GLuint(ref);
    glDeleteTextures(1, &glid);
#endif
}

/**
 * @brief Set the texture transform.
 *
 * Texture coordinate transform
 * Tc' = -C x S x R x C x T x Tc
 *
 * @param[in] center        center.
 * @param[in] rotation      rotation.
 * @param[in] scale         scale.
 * @param[in] translation   translation.
 */
void openvrml::gl::viewer::set_texture_transform(const vec2f & center,
                                                 float rotation,
                                                 const vec2f & scale,
                                                 const vec2f & translation)
{
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glTranslatef(-center.x(), -center.y(), 0.0);
    glScalef(scale.x(), scale.y(), 1.0);
    if (!fequal<float>()(rotation, 0.0f)) {
        glRotatef(GLfloat(rotation * 180.0 / pi), 0.0, 0.0, 1.0);
    }

    glTranslatef(center.x(), center.y(), 0.0);
    glTranslatef(translation.x(), translation.y(), 0.0);

    glMatrixMode(GL_MODELVIEW);
}

namespace {

    /**
     * Compute a target and up vector from position/orientation/distance.
     */
    OPENVRML_GL_LOCAL void computeView(const openvrml::vec3f & position,
                                       const openvrml::rotation & orientation,
                                       const float distance,
                                       openvrml::vec3f & target,
                                       openvrml::vec3f & up)
    {
        // Graphics Gems, p 466. Convert between axis/angle and rotation matrix

        const double s = sin(orientation.angle());
        const double c = cos(orientation.angle());
        const double t = 1.0 - c;

        // Transform [0,0,1] by the orientation to determine sight line
        target.x(float(t * orientation.x() * orientation.z()
                       + s * orientation.y()));
        target.y(float(t * orientation.y() * orientation.z()
                       - s * orientation.x()));
        target.z(float(t * orientation.z() * orientation.z() + c));

        // Move along that vector the specified distance away from position[]
        target = target * -distance + position;

        // Transform [0,1,0] by the orientation to determine up vector
        up.x(float(t * orientation.x() * orientation.y()
                   - s * orientation.z()));
        up.y(float(t * orientation.y() * orientation.y() + c));
        up.z(float(t * orientation.y() * orientation.z()
                   + s * orientation.x()));
    }
}

/**
 * @brief Set the viewpoint.
 *
 * @param[in] position          position.
 * @param[in] orientation       orientation.
 * @param[in] fieldOfView       field of view.
 * @param[in] avatarSize        avatar size.
 * @param[in] visibilityLimit   visiblity limit.
 */
void openvrml::gl::viewer::set_viewpoint(const vec3f & position,
                                         const openvrml::rotation & orientation,
                                         const float fieldOfView,
                                         const float avatarSize,
                                         const float visibilityLimit)
{
    glMatrixMode( GL_PROJECTION );
    if (!this->select_mode) { glLoadIdentity(); }

    float field_of_view = float(fieldOfView * 180.0 / pi);
    float aspect = float(this->win_width) / this->win_height;
    float znear = (avatarSize > 0.0)
                ? float(0.5 * avatarSize)
                : 0.01f;
    float zfar = (visibilityLimit > 0.0)
               ? visibilityLimit
               : 30000.0f;
    gluPerspective(field_of_view, aspect, znear, zfar);

    this->frustum(openvrml::frustum(field_of_view, aspect, znear, zfar));

    glMatrixMode(GL_MODELVIEW);

    // Guess some distance along the sight line to use as a target...
    float d = float(10.0 * avatarSize);
    if (d < znear || d > zfar) { d = float(0.2 * (avatarSize + zfar)); }

    vec3f target, up;
    computeView(position, orientation, d, target, up);

    gluLookAt(position.x(), position.y(), position.z(),
              target.x(), target.y(), target.z(),
              up.x(), up.y(), up.z());
}

/**
 * @brief Transform @p points by the current modelview matrix.
 *
 * @param[in] nPoints   number of points.
 * @param[in] point     pointer to the first point in an array.
 */
void openvrml::gl::viewer::transform_points(const size_t nPoints,
                                            vec3f * point) const
{
    mat4f m;
    glGetFloatv(GL_MODELVIEW_MATRIX, &m[0][0]);
    vec3f * const end = point + nPoints;
    for (; point != end; ++point) { *point *= m; }
}

/**
 * @brief Multiply the current modelview matrix by @p mat.
 *
 * @param[in] mat   a matrix.
 */
void openvrml::gl::viewer::transform(const mat4f & mat)
{
    glMultMatrixf(&mat[0][0]);
}

/**
 * @brief Update the scene.
 *
 * update is called from a timer callback and from checkSensitive.
 *
 * @param[in] time  current time.
 */
void openvrml::gl::viewer::update(const double time)
{
    assert(this->browser());
    if (this->browser()->update(time)) {
        checkErrors("update");
        this->post_redraw();
    }

    // Set an alarm clock for the next update time.
    this->set_timer(this->browser()->delta());
}

/**
 * @brief Redraw the scene.
 */
void openvrml::gl::viewer::redraw()
{
    assert(this->browser());

    if (!this->gl_initialized) { initialize(); }

    double start = browser::current_time();

    glDisable(GL_FOG);                // this is a global attribute
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    if (this->lit) { glEnable(GL_LIGHTING); }
    glDisable(GL_COLOR_MATERIAL);

    glDisable(GL_BLEND);

    glShadeModel(GL_SMOOTH);

    this->objects = 0;
    this->nested_objects = 0;

    this->sensitive = 0;

    // Clean out any defined lights
    for (size_t i = 0; i < viewer::max_lights; ++i) {
        light_info_[i].type = viewer::light_unused;
        GLenum light = GLenum(GL_LIGHT0 + i);
        glDisable(light);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    this->browser()->render();

    this->swap_buffers();

    this->render_time1 = this->render_time;
    this->render_time = browser::current_time() - start;
}

/**
 * @brief Resize the viewport.
 *
 * @param[in] width     new width.
 * @param[in] height    new height.
 */
void openvrml::gl::viewer::resize(size_t width, size_t height)
{
    if (width < 2) { width = 2; }
    if (height < 2) { height = 2; }
    glViewport(0, 0, GLsizei(width), GLsizei(height));
    this->win_width = width;
    this->win_height = height;
}

/**
 * @brief Handle an event generated from user input.
 *
 * @param[in] e event data.
 */
void openvrml::gl::viewer::input(event_info * e)
{
    switch (e->event) {
    case event_key_down:
        handleKey(e->what);
        break;
    case event_mouse_move:
        this->checkSensitive(e->x, e->y, event_mouse_move);
        break;
    case event_mouse_click:
    case event_mouse_release:
        this->handleButton(e);
        break;
    case event_mouse_drag:
        handleMouseDrag(e->x, e->y);
        break;
    }
}

/**
 * @brief Rotate the user view.
 *
 * @param[in] rot   rotation.
 */
void openvrml::gl::viewer::rotate(const openvrml::rotation & rot) OPENVRML_NOTHROW
{
    assert(this->browser());

    if (fequal<float>()(rot.angle(), 0.0f)) { return; }

    viewpoint_node & activeViewpoint = this->browser()->active_viewpoint();
    const mat4f & viewpointTransformation = activeViewpoint.transformation();
    const mat4f & currentUserViewTransform =
            activeViewpoint.user_view_transform();

    mat4f oldCameraTransform =
            currentUserViewTransform * viewpointTransformation;

    vec3f currentTranslation, currentScale;
    openvrml::rotation currentRotation;
    oldCameraTransform.transformation(currentTranslation,
                                      currentRotation,
                                      currentScale);

    const mat4f r = mat4f::rotation(rot);

    const mat4f prevOrientation = mat4f::rotation(currentRotation);

    const mat4f t = mat4f::translation(currentTranslation);

    const mat4f newCameraTransform =
            prevOrientation * (t * (r * viewpointTransformation.inverse()));

    activeViewpoint.user_view_transform(newCameraTransform);

    post_redraw();
}

/**
 * @brief Translate the user view.
 *
 * @param[in] x translation vector x component.
 * @param[in] y translation vector y component.
 * @param[in] z translation vector z component.
 */
void openvrml::gl::viewer::step(const float x, const float y, const float z)
{
    assert(this->browser());

    mat4f t = mat4f::translation(vec3f(x, y, z));
    viewpoint_node & activeViewpoint = this->browser()->active_viewpoint();
    activeViewpoint
        .user_view_transform(t * activeViewpoint.user_view_transform());
    post_redraw();
}

/**
 * @brief Zoom.
 *
 * @param[in] z amount to zoom.
 */
void openvrml::gl::viewer::zoom(const float z)
{
    assert(this->browser());

    GLint viewport[4];
    GLdouble modelview[16], projection[16];
    glGetIntegerv (GL_VIEWPORT, viewport);
    glGetDoublev (GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev (GL_PROJECTION_MATRIX, projection);
    navigation_info_node & nav_info =
        this->browser()->active_navigation_info();
    GLdouble x_c = this->win_width / 2;
    GLdouble y_c = this->win_height / 2;
    GLdouble z_c = 0.5;
    float visibilityLimit = nav_info.visibility_limit();
    if (fequal<float>()(visibilityLimit, 0.0f)) { visibilityLimit = 30000.0; }
    GLdouble ox, oy, oz;
    gluUnProject(x_c, y_c, z_c,
                 modelview,
                 projection,
                 viewport,
                 &ox, &oy, &oz);
    z_c = z_c - 100.0 * z / visibilityLimit;
    GLdouble dx, dy, dz;
    gluUnProject(x_c, y_c, z_c,
                 modelview,
                 projection,
                 viewport,
                 &dx, &dy, &dz);
    dx -= ox;
    dy -= oy;
    dz -= oz;
    double dist = dx * dx + dy * dy + dz * dz;
    if (dist < 1.0e-25) { return; }
    dist = sqrt(dist);
    float speed = nav_info.speed();
    dist = speed / dist;
    if (fequal<float>()(float(dist), 0.0f)) { return; }
    dx *= dist;
    dy *= dist;
    dz *= dist;
    const vec3f translation(static_cast<float>(dx),
                            static_cast<float>(dy),
                            static_cast<float>(dz));
    mat4f t = mat4f::translation(translation);
    viewpoint_node & activeViewpoint = this->browser()->active_viewpoint();
    const mat4f & userViewTransform = activeViewpoint.user_view_transform();
    activeViewpoint.user_view_transform(t * userViewTransform);
    post_redraw();
}

/**
 * @brief Handle keypresses.
 *
 * @param[in] key   key identifier.
 */
void openvrml::gl::viewer::handleKey(int key)
{
    using std::find;
    using std::list;

    assert(this->browser());

    switch (key) {
    case key_left:
        step(-1, 0, 0);
        break;

    case key_right:
        step(1, 0, 0);
        break;

    case key_up:  // move forward along line of sight
        zoom(1);
        break;

    case key_down: // move backwards along line of sight
        zoom(-1);
        break;

    case 'a':  // look up
        this->rotate(trackball(0.0, 0.45f, 0.0, 0.55f));
        break;

    case 'z':  // look down
        this->rotate(trackball(0.0, 0.55f, 0.0, 0.45f));
        break;

    case 'A':  // translate up
        step(0,1,0);
        break;

    case 'Z':  // translate down
        step(0,-1,0);
        break;

    case ',':                   // Look left
        this->rotate(trackball(0.55f, 0.0, 0.45f, 0.0));
        break;

    case '.':                   // Look right
        this->rotate(trackball(0.45f, 0.0, 0.55f, 0.0));
        break;

    // XXX
    // XXX This can't work well. The list of Viewpoints in the world could
    // XXX change, and the resulting ordering would give unexpected results.
    // XXX The ordering of Viewpoints in the list should be the same as the
    // XXX order in which the nodes are encountered in a normal traversal of
    // XXX the scene graph.
    // XXX
    case key_page_down:
        {
            viewpoint_node & currentViewpoint =
                this->browser()->active_viewpoint();
            const list<viewpoint_node *> & viewpoints =
                this->browser()->viewpoints();
            list<viewpoint_node *>::const_iterator pos =
                    find(viewpoints.begin(), viewpoints.end(),
                         &currentViewpoint);
            if (pos != viewpoints.end()) {
                ++pos;
                if (pos == viewpoints.end()) { pos = viewpoints.begin(); }
                event_listener & listener = (*pos)->event_listener("set_bind");
                sfbool_listener * sfb_listener =
                    dynamic_cast<sfbool_listener *>(&listener);
                assert(sfb_listener);
                sfb_listener->process_event(sfbool(true),
                                            browser::current_time());
            }
        }
        this->post_redraw();
        break;

    case key_page_up:
        {
            viewpoint_node & currentViewpoint =
                this->browser()->active_viewpoint();
            const list<viewpoint_node *> & viewpoints =
                this->browser()->viewpoints();
            list<viewpoint_node *>::const_iterator pos =
                    find(viewpoints.begin(), viewpoints.end(),
                         &currentViewpoint);
            if (pos != viewpoints.end()) {
                if (pos == viewpoints.begin()) {
                    pos = viewpoints.end();
                }
                --pos;
                event_listener & listener = (*pos)->event_listener("set_bind");
                sfbool_listener * sfb_listener =
                    dynamic_cast<sfbool_listener *>(&listener);
                assert(sfb_listener);
                sfb_listener->process_event(sfbool(true),
                                            browser::current_time());
            }
        }
        this->post_redraw();
        break;

    case 'b':
        this->blend = !this->blend;
        this->post_redraw();
        OPENVRML_GL_PRINT_MESSAGE_(" Alpha blending "
                                   + std::string(this->blend ? "en" : "dis")
                                   + "sabled.");
        break;

    case 'd':
        this->draw_bounding_spheres = ! this->draw_bounding_spheres;
        OPENVRML_GL_PRINT_MESSAGE_(" bounding spheres "
                                   + std::string(this->draw_bounding_spheres
                                                 ? "en" : "dis")
                                   + "sabled.");
        this->post_redraw();
        break;

    case 'l':
        this->lit = !this->lit;
        OPENVRML_GL_PRINT_MESSAGE_(" Lighting "
                                   + std::string(this->lit ? "en" : "dis")
                                   + "sabled.");
        this->post_redraw();
        break;

    case key_home:
    case 'r':                        // Reset view
        this->reset_user_navigation();
        break;

    case 't':
        this->texture = !this->texture;
        this->post_redraw();
        OPENVRML_GL_PRINT_MESSAGE_(" Texture mapping "
                                   + std::string(this->texture ? "en" : "dis")
                                   + "sabled.");
        break;

    case 'w':                        // Wireframe (should disable texturing)
        this->wireframe = !this->wireframe;
        glPolygonMode(GL_FRONT_AND_BACK, this->wireframe ? GL_LINE : GL_FILL);
        this->post_redraw();
        OPENVRML_GL_PRINT_MESSAGE_(" Drawing polygins in "
                                   + std::string(this->wireframe
                                                 ?"wireframe"
                                                 : "filled")
                                   + " mode.");
        break;

    default:
        break;
    }
}

/**
 * @brief Handle mouse button up/down.
 *
 * @param[in] e event data.
 */
void openvrml::gl::viewer::handleButton(event_info * e)
{
    this->rotating = false;
    this->scaling = false;
    this->translating = false;

    // Check for a sensitive object first
    if (e->what == 0 && checkSensitive(e->x, e->y, e->event)) { return; }

    this->active_sensitive = 0;

    // Nothing under the mouse
    if (e->event == event_mouse_release) {
        this->set_cursor(cursor_inherit);
    } else {
        switch (e->what) {                // button
        case 0:
            this->set_cursor(cursor_cycle);
            this->rotating = true;
            this->beginx = e->x;
            this->beginy = e->y;
            break;

        case 1:
            this->set_cursor(cursor_up_down);
            this->scaling = true;
            this->beginx = e->x;
            this->beginy = e->y;
            break;

        case 2:
            this->set_cursor(cursor_crosshair);
            this->translating = true;
            this->beginx = e->x;
            this->beginy = e->y;
            break;
        }
    }
}


/**
 * @brief Handle mouse dragging.
 *
 * @param[in] x pointer x-coordinate.
 * @param[in] y pointer y-coordinate.
 */
void openvrml::gl::viewer::handleMouseDrag(int x, int y)
{
    if (this->active_sensitive) {
        this->checkSensitive(x, y, event_mouse_drag);
    } else if (this->rotating) {
        const float x1 = float((2.0 * this->beginx - this->win_width)
                               / this->win_width);
        const float y1 = float((this->win_height - 2.0 * this->beginy)
                               / this->win_height);
        const float x2 = float((2.0 * x - this->win_width) / this->win_width);
        const float y2 = float((this->win_height - 2.0 * y)
                               / this->win_height);
        this->rotate(trackball(x1, y1, x2, y2));
        this->beginx = x;
        this->beginy = y;
    } else if (this->scaling) {
        // This is not scaling, it is now moving in screen Z coords
        this->zoom(float(this->beginy - y) / this->win_height );
        this->beginx = x;
        this->beginy = y;
    } else if (this->translating) {
        this->step(float(x - this->beginx) / this->win_width,
                   float(this->beginy - y) / this->win_height,
                   0.0);
        this->beginx = x;
        this->beginy = y;
    }
}


/**
 * Check for pickable objects.
 */
bool openvrml::gl::viewer::checkSensitive(const int x,
                                          const int y,
                                          const event_type mouseEvent)
{
    assert(this->browser());

    double timeNow = browser::current_time();
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    GLuint selectBuffer[4 * viewer::maxsensitive];
    glSelectBuffer(4 * viewer::maxsensitive, selectBuffer);

    glRenderMode(GL_SELECT);
    this->select_mode = true;

    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(GLdouble(x), GLdouble(viewport[3] - y),
                  2.0, 2.0, viewport);

    // Set up the global attributes
    glDisable(GL_FOG);
    glDisable(GL_TEXTURE_2D);

    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);

    glShadeModel(GL_FLAT);

    this->objects = 0;
    this->nested_objects = 0;

    this->sensitive = 0;

    // Clean out any defined lights
    for (size_t i = 0; i < max_lights; ++i) {
        this->light_info_[i].type = viewer::light_unused;
        GLenum light = GLenum(GL_LIGHT0 + i);
        glDisable(light);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    this->browser()->render();

    this->select_mode = false;

    // select closest hit
    GLuint closest = 0, minz = 0xffffffff;

    size_t selected = 0;   // nothing selected
    GLint hits = glRenderMode(GL_RENDER);
    if (hits > 0) {
        // selectBuffer[] = { N1, z1, z2, name1, ..., nameN1, N2, ... }
        GLuint * sel = selectBuffer;

        for (GLint nh = 0; nh < hits; ++nh, sel += (3 + sel[0])) {
            if (sel[1] <= minz) {
                minz = sel[1];  // z1
                closest = sel[2 + sel[0]];  // nameN (most deeply nested)
            }
        }

        if (closest > 0 && closest <= GLuint(this->sensitive)) {
            selected = closest;
        }
    }

    this->set_cursor(selected ? cursor_info : cursor_inherit);

    //
    // Compute & store the world coords of the pick if something
    // was already active or is about to become active. The window
    // Z coord is retained when a drag is started on a sensitive
    // so the drag stays in the same plane even if the mouse moves
    // off the original sensitive object.
    //
    double selectCoord[3] = { 0.0, 0.0, 0.0 };

    if (this->active_sensitive || selected) {
        if (!this->active_sensitive) {
            this->select_z = minz / double(0xffffffff);
        }

        GLint viewport[4];
        GLdouble modelview[16], projection[16];
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
  //      glGetDoublev (GL_MODELVIEW_MATRIX, modelview);

        //
        // make modelview as a unit matrix as this is taken care in the core
        // side during render traversal.
        //
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                modelview[4 * i + j] = (i == j) ? 1.0 : 0.0;
            }
        }

        GLdouble dx, dy, dz;
        gluUnProject(GLdouble(x), GLdouble(viewport[3] - y), this->select_z,
                     modelview, projection, viewport,
                     &dx, &dy, &dz);

        selectCoord[0] = dx;
        selectCoord[1] = dy;
        selectCoord[2] = dz;
    }

    bool wasActive = false;
  // To unset PickMatrix...
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    // Sanity check. This can happen when the world gets replaced
    // by clicking on an anchor - the current sensitive object goes
    // away, but these variables are not reset.
    if (this->active_sensitive > this->sensitive) {
        this->active_sensitive = 0;
    }
    if (this->over_sensitive > this->sensitive) {
        this->over_sensitive = 0;
    }

    // An active sensitive object "grabs" the mouse until button released
    if (this->active_sensitive) {
        if (mouseEvent == event_mouse_release
                || mouseEvent == event_mouse_move) {
            this->browser()->sensitive_event(
                this->sensitive_object[this->active_sensitive - 1],
                timeNow,
                selected == this->active_sensitive,
                false,
                selectCoord);
            this->active_sensitive = 0;
        } else {
            // _DRAG
            this->browser()->sensitive_event(
                this->sensitive_object[this->active_sensitive - 1],
                timeNow,
                selected == this->active_sensitive,
                true,
                selectCoord);
        }
        wasActive = true;
    } else if (mouseEvent == event_mouse_click && selected) {
        //
        // A click down over a sensitive object initiates an active grab and
        // mouse over events are no longer relevant.
        //
        if (over_sensitive && over_sensitive != selected) {
            this->browser()->sensitive_event(
                this->sensitive_object[this->over_sensitive - 1],
                timeNow,
                false, false, // isOver, isActive
                selectCoord);
            this->over_sensitive = 0;
        }
        this->active_sensitive = selected;
        this->browser()->sensitive_event(
            this->sensitive_object[this->active_sensitive - 1],
            timeNow,
            true, true, // isOver, isActive
            selectCoord);
    } else if (mouseEvent == event_mouse_move) {
        // Handle isOver events (coords are bogus)
        if (this->over_sensitive && this->over_sensitive != selected) {
            this->browser()->sensitive_event(
                this->sensitive_object[this->over_sensitive - 1],
                timeNow,
                false, false, // isOver, isActive
                selectCoord);
        }
        this->over_sensitive = selected;
        if (this->over_sensitive) {
            this->browser()->sensitive_event(
                this->sensitive_object[this->over_sensitive - 1],
                timeNow,
                true, false,  // isOver, isActive
                selectCoord);
        }
    }

    // Was event handled here?
    if (this->active_sensitive || wasActive) { this->update(timeNow); }

    // Everything is handled except down clicks where nothing was selected
    // and up clicks where nothing was active.
    return this->active_sensitive || wasActive;
}

/**
 * @brief Draw a bounding sphere.
 *
 * Used for debugging view culling. Probably should be draw_bounding_volume and
 * handle axis_aligned_bounding_boxes as well.
 *
 * @param[in] bs            a bounding sphere; if max, will not be drawn
 * @param[in] intersection  one of the bvolume intersection test constants, or 4
 *                      to draw in unique way. (useful for debugging)
 */
void
openvrml::gl::viewer::draw_bounding_sphere(
    const bounding_sphere & bs,
    const bounding_volume::intersection intersection)
{
    static const GLfloat green[] = { 0.25f, 1.0f, 0.25f, 1.0f };
    static const GLfloat red[] = { 1.0f, 0.5f, 0.5f, 1.0f };
    static const GLfloat grey[] = { 0.5f, 0.5f, 0.5f, 1.0f };

    if (!this->draw_bounding_spheres
        || bs.maximized()
        || bs.radius() == -1.0) {
        return;
    }
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glEnable(GL_LIGHTING);
    glShadeModel(GL_FLAT);
    glMatrixMode(GL_MODELVIEW);
    this->modelview_matrix_stack_.push();
    const vec3f & c = bs.center();
    glTranslatef(c.x(), c.y(), c.z());
    GLUquadricObj * const sph = gluNewQuadric();
    switch (intersection) {
    case bounding_volume::outside:
        //glDisable(GL_LIGHTING);
        //glEnable(GL_LIGHTING);
        gluQuadricDrawStyle(sph, GLU_LINE);
        //gluQuadricDrawStyle(sph, GLU_POINT);
        glColor3f(0.5, 0.5, 0.5);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, grey);
        gluSphere(sph, bs.radius(), 5, 5);
        break;

    case bounding_volume::partial:
        //glEnable(GL_LIGHTING);
        gluQuadricNormals(sph, GLU_SMOOTH);
        //gluQuadricDrawStyle(sph, GLU_FILL);
        gluQuadricDrawStyle(sph, GLU_LINE);
        glColor3f(0.25, 1.0, 0.25);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
        gluSphere(sph, bs.radius(), 8, 8);
        break;

    case bounding_volume::inside:
        //glEnable(GL_LIGHTING);
        gluQuadricNormals(sph, GLU_SMOOTH);
        //gluQuadricDrawStyle(sph, GLU_FILL);
        gluQuadricDrawStyle(sph, GLU_LINE);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
        glColor3f(1.0, 0.5, 0.5);
        gluSphere(sph, bs.radius(), 8, 8);
        break;

    default:
        //glEnable(GL_LIGHTING);
        gluQuadricNormals(sph, GLU_SMOOTH);
        gluQuadricDrawStyle(sph, GLU_LINE);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, grey);
        glColor3f(0.5, 0.5, 0.5);
        gluSphere(sph, bs.radius(), 8, 8);
    }
    gluDeleteQuadric(sph);
    this->modelview_matrix_stack_.pop();
}

/**
 * @fn void openvrml::gl::viewer::post_redraw()
 *
 * @brief Called to indicate to the windowing system that a redraw is
 *        necessary.
 */

/**
 * @fn void openvrml::gl::viewer::set_cursor(cursor_style c)
 *
 * @brief Called to set the cursor style.
 *
 * @param[in] c cursor style identifier.
 */

/**
 * @fn void openvrml::gl::viewer::swap_buffers()
 *
 * @brief Called to indicate to the windowing system that the front and back
 *        buffers should be swapped.
 */

/**
 * @fn void openvrml::gl::viewer::set_timer(double interval)
 *
 * @brief Set a delay.
 *
 * @param[in] interval  milliseconds to delay.
 */
