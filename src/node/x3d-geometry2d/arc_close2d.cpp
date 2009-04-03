// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008  Braden McDaniel
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

# include "arc_close2d.h"
# include <openvrml/node_impl_util.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Represents ArcClose2D node instances.
     */
    class OPENVRML_LOCAL arc_close2d_node :
        public abstract_node<arc_close2d_node>,
        public geometry_node,
        public child_node {

        friend class openvrml_node_x3d_geometry2d::arc_close2d_metatype;

        sfstring closure_type_;
        sffloat end_angle_;
        sffloat radius_;
        sfbool solid_;
        sffloat start_angle_;

    public:
        arc_close2d_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~arc_close2d_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        rendering_context context);
    };


    /**
     * @var arc_close2d_node::arc_close2d_metatype
     *
     * @brief Class object for ArcClose2D nodes.
     */

    /**
     * @var arc_close2d_node::closure_type_
     *
     * @brief closure_type field
     */

    /**
     * @var arc_close2d_node::end_angle_
     *
     * @brief end_angle field
     */

    /**
     * @var arc_close2d_node::radius_
     *
     * @brief radius field
     */

    /**
     * @var arc_close2d_node::solid_
     *
     * @brief solid field
     */

    /**
     * @var arc_close2d_node::start_angle_
     *
     * @brief start_angle field
     */


    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a @c viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    void
    arc_close2d_node::
    do_render_geometry(openvrml::viewer & /* viewer */,
                       const rendering_context /* context */)
    {}


    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool arc_close2d_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    arc_close2d_node::
    arc_close2d_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope):
      node(type, scope),
      bounded_volume_node(type, scope),
      abstract_node<self_t>(type, scope),
      geometry_node(type, scope),
      child_node(type, scope),
      closure_type_("PIE"),
      end_angle_(1.57079633f),
      radius_(1)
    {}

    /**
     * @brief Destroy.
     */
    arc_close2d_node::~arc_close2d_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_geometry2d::arc_close2d_metatype::id =
    "urn:X-openvrml:node:ArcClose2D";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c arc_close2d_metatype.
 */
openvrml_node_x3d_geometry2d::arc_close2d_metatype::
arc_close2d_metatype(openvrml::browser & browser):
    node_metatype(arc_close2d_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_geometry2d::arc_close2d_metatype::~arc_close2d_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the @c new node_type.
 *
 * @return a @c boost::shared_ptr to a @c node_type capable of creating
 *         ArcClose2D nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by @c arc_close2d_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_geometry2d::arc_close2d_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 6> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::sfstring_id,
                       "closureType"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "endAngle"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "radius"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "startAngle")
    };
    typedef node_type_impl<arc_close2d_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & the_node_type = static_cast<node_type_t &>(*type);

    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &arc_close2d_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &arc_close2d_node::closure_type_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &arc_close2d_node::end_angle_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &arc_close2d_node::radius_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &arc_close2d_node::solid_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &arc_close2d_node::start_angle_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
