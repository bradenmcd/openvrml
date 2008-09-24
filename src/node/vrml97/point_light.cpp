// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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
# include <openvrml/browser.h>
# include <openvrml/scene.h>
# include <private.h>
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

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating PointLight nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by point_light_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::point_light_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 8> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "ambientIntensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "attenuation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "intensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "location"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "on"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "radius"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<point_light_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & pointLightNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            pointLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &point_light_node::ambient_intensity_);
        } else if (*interface_ == *++supported_interface) {
            pointLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &point_light_node::attenuation_);
        } else if (*interface_ == *++supported_interface) {
            pointLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &point_light_node::color_);
        } else if (*interface_ == *++supported_interface) {
            pointLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &point_light_node::intensity_);
        } else if (*interface_ == *++supported_interface) {
            pointLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &point_light_node::location_);
        } else if (*interface_ == *++supported_interface) {
            pointLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &point_light_node::on_);
        } else if (*interface_ == *++supported_interface) {
            pointLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &point_light_node::radius_);
        } else if (*interface_ == *++supported_interface) {
            pointLightNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &point_light_node::metadata);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
