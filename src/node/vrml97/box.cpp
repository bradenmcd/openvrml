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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <openvrml/browser.h>
# include <openvrml/node_impl_util.h>
# include "box.h"

namespace {

    class OPENVRML_LOCAL box_node :
        public openvrml::node_impl_util::abstract_node<box_node>,
        public openvrml::geometry_node {

        friend class openvrml_node_vrml97::box_metatype;

        openvrml::sfvec3f size;
        openvrml::sfbool solid_;

        openvrml::bounding_sphere bsphere;

    public:
        box_node(const openvrml::node_type & type,
                 const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~box_node() OPENVRML_NOTHROW;

    private:
        virtual const openvrml::bounding_volume &
        do_bounding_volume() const;

        virtual
        openvrml::viewer::object_t
        do_render_geometry(openvrml::viewer & viewer,
                           openvrml::rendering_context context);
    };


    /**
     * @class box_node
     *
     * @brief Box node instances.
     */

    /**
     * @var class box_node::box_metatype
     *
     * @brief Class object for Box nodes.
     */

    /**
     * @var openvrml::sfvec3f box_node::size
     *
     * @brief size field.
     */

    /**
     * @var openvrml::bounding_sphere box_node::bsphere
     *
     * @brief Bounding volume.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    box_node::
    box_node(const openvrml::node_type & type,
             const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<box_node>(type, scope),
        geometry_node(type, scope),
        size(openvrml::make_vec3f(2.0, 2.0, 2.0)),
        solid_(true)
    {
        this->bounding_volume_dirty(true); // lazy calc of bvolume
    }

    /**
     * @brief Destroy.
     */
    box_node::~box_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a viewer.
     * @param context   the rendering context.
     *
     * @return display object identifier.
     */
    openvrml::viewer::object_t
    box_node::
    do_render_geometry(openvrml::viewer & viewer, openvrml::rendering_context)
    {
        const openvrml::viewer::object_t object_ref =
            viewer.insert_box(this->size.value());
        return object_ref;
    }

    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    box_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const openvrml::vec3f corner = this->size.value() / 2.0f;
            const float r = corner.length();
            const_cast<box_node *>(this)->bsphere.radius(r);
            const_cast<box_node *>(this)->bounding_volume_dirty(false); // logical const
        }
        return this->bsphere;
    }
}

/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::box_metatype::id =
    "urn:X-openvrml:node:Box";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::box_metatype::box_metatype(openvrml::browser & browser):
    node_metatype(box_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::box_metatype::~box_metatype() OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Box nodes.
 *
 * @exception openvrml::unsupported_interface   if @p interfaces includes an
 *                                              interface not supported by
 *                                              @c box_metatype.
 * @exception std::bad_alloc                    if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::box_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 3> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "size"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid")
    };

    typedef node_impl_util::node_type_impl<box_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & boxNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            boxNodeType.add_field(supported_interface->field_type,
                                  supported_interface->id,
                                  &box_node::size);
        } else if (*interface_ == *++supported_interface) {
            boxNodeType.add_exposedfield(supported_interface->field_type,
                                         supported_interface->id,
                                         &box_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            boxNodeType.add_field(supported_interface->field_type,
                                  supported_interface->id,
                                  &box_node::solid_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
