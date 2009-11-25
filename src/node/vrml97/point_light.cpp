// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2009  Braden McDaniel
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# include "point_light.h"
# include "abstract_light.h"
# include <private.h>
# include <openvrml/browser.h>
# include <openvrml/scene.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL point_light_node :
        public openvrml_node_vrml97::abstract_light_node<point_light_node>,
        public openvrml::scoped_light_node {

        friend class openvrml_node_vrml97::point_light_metatype;

        exposedfield<openvrml::sfvec3f> attenuation_;
        exposedfield<openvrml::sfvec3f> location_;
        exposedfield<openvrml::sffloat> radius_;

    public:
        point_light_node(const openvrml::node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~point_light_node() OPENVRML_NOTHROW;

    private:
        virtual void do_initialize(double timestamp) OPENVRML_THROW1(std::bad_alloc);
        virtual void do_shutdown(double timestamp) OPENVRML_NOTHROW;
        virtual void do_render_scoped_light(openvrml::viewer & v);
    };

    /**
     * @class point_light_node
     *
     * @brief PointLight node instances.
     */

    /**
     * @var class point_light_node::point_light_metatype
     *
     * @brief Class object for PointLight instances.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<point_light_node>::exposedfield<openvrml::sfvec3f> point_light_node::attenuation_
     *
     * @brief attenuation exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<point_light_node>::exposedfield<openvrml::sfvec3f> point_light_node::location_
     *
     * @brief location exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<point_light_node>::exposedfield<openvrml::sffloat> point_light_node::radius_
     *
     * @brief radius exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    point_light_node::
    point_light_node(const openvrml::node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        light_node(type, scope),
        openvrml_node_vrml97::abstract_light_node<point_light_node>(type, scope),
        scoped_light_node(type, scope),
        attenuation_(*this, openvrml::make_vec3f(1.0, 0.0, 0.0)),
        location_(*this, openvrml::make_vec3f(0.0, 0.0, 0.0)),
        radius_(*this, 100)
    {}

    /**
     * @brief Destroy.
     */
    point_light_node::~point_light_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Render the scoped light.
     *
     * This should be called before rendering any geometry nodes in the scene.
     * Since this is called from Scene::render() before traversing the
     * scene graph, the proper transformation matrix hasn't been set up.
     * Somehow it needs to figure out the accumulated xforms of its
     * parents and apply them before rendering. This is not easy with
     * DEF/USEd nodes...
     *
     * @param viewer    a viewer.
     */
    void
    point_light_node::do_render_scoped_light(openvrml::viewer & v)
    {
        if (this->on_.sfbool::value() && this->radius_.sffloat::value() > 0.0)
        {
            v.insert_point_light(this->ambient_intensity_.sffloat::value(),
                                 this->attenuation_.sfvec3f::value(),
                                 this->color_.sfcolor::value(),
                                 this->intensity_.sffloat::value(),
                                 this->location_.sfvec3f::value(),
                                 this->radius_.sffloat::value());
        }
        this->node::modified(false);
    }

    /**
     * @brief Initialize.
     *
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    point_light_node::do_initialize(double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        assert(this->scene());
        this->scene()->browser().add_scoped_light(*this);
    }

    /**
     * @brief Shut down.
     *
     * @param timestamp the current time.
     */
    void
    point_light_node::do_shutdown(double)
        OPENVRML_NOTHROW
    {
        assert(this->scene());
        this->scene()->browser().remove_scoped_light(*this);
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::point_light_metatype::id =
    "urn:X-openvrml:node:PointLight";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::point_light_metatype::
point_light_metatype(openvrml::browser & browser):
    node_metatype(point_light_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::point_light_metatype::~point_light_metatype()
    OPENVRML_NOTHROW
{}

# define POINT_LIGHT_INTERFACE_SEQ                                    \
    ((exposedfield, sffloat, "ambientIntensity", ambient_intensity_)) \
    ((exposedfield, sfvec3f, "attenuation",      attenuation_))       \
    ((exposedfield, sfcolor, "color",            color_))             \
    ((exposedfield, sffloat, "intensity",        intensity_))         \
    ((exposedfield, sfvec3f, "location",         location_))          \
    ((exposedfield, sfbool,  "on",               on_))                \
    ((exposedfield, sffloat, "radius",           radius_))            \
    ((exposedfield, sfnode,  "metadata",         metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              point_light_metatype,
                                              point_light_node,
                                              POINT_LIGHT_INTERFACE_SEQ)
