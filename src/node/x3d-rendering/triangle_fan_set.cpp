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

# include "triangle_fan_set.h"
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
     * @brief Represents TriangleFanSet node instances.
     */
    class OPENVRML_LOCAL triangle_fan_set_node :
        public abstract_node<triangle_fan_set_node>,
        public geometry_node,
        public child_node {

        friend class openvrml_node_x3d_rendering::triangle_fan_set_metatype;

        exposedfield<sfnode> color_;
        exposedfield<sfnode> coord_;
        exposedfield<mfint32> fan_count_;
        exposedfield<sfnode> normal_;
        exposedfield<sfnode> tex_coord_;
        sfbool ccw_;
        sfbool color_per_vertex_;
        sfbool normal_per_vertex_;
        sfbool solid_;
        bounding_sphere bsphere;

    public:
        triangle_fan_set_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~triangle_fan_set_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
        virtual const color_node * color() const OPENVRML_NOTHROW;
    private:
        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        rendering_context context);
        virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

        void recalc_bsphere();
    };


    /**
     * @var triangle_fan_set_node::triangle_fan_set_metatype
     *
     * @brief Class object for TriangleFanSet nodes.
     */

    /**
     * @var triangle_fan_set_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var triangle_fan_set_node::coord_
     *
     * @brief coord exposedField
     */

    /**
     * @var triangle_fan_set_node::fan_count_
     *
     * @brief fan_count exposedField
     */

    /**
     * @var triangle_fan_set_node::normal_
     *
     * @brief normal exposedField
     */

    /**
     * @var triangle_fan_set_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var triangle_fan_set_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var triangle_fan_set_node::color_per_vertex_
     *
     * @brief color_per_vertex field
     */

    /**
     * @var triangle_fan_set_node::normal_per_vertex_
     *
     * @brief normal_per_vertex field
     */

    /**
     * @var triangle_fan_set_node::solid_
     *
     * @brief solid field
     */


    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    triangle_fan_set_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<self_t *>(this)->recalc_bsphere();
        }
        return this->bsphere;
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void triangle_fan_set_node::recalc_bsphere()
    {
        // take the bvolume of all the points. technically, we should figure
        // out just which points are used by the index and just use those,
        // but for a first pass this is fine (also: if we do it this way
        // then we don't have to update the bvolume when the index
        // changes). motto: always do it the simple way first...
        //
        openvrml::coordinate_node * const coordinateNode =
            node_cast<openvrml::coordinate_node *>(
                this->coord_.sfnode::value().get());
        if (coordinateNode) {
            const std::vector<vec3f> & coord = coordinateNode->point();
            this->bsphere = bounding_sphere();
            this->bsphere.enclose(coord);
        }
        this->bounding_volume_dirty(false);
    }

    const color_node * triangle_fan_set_node::color() const OPENVRML_NOTHROW
    {
        return node_cast<color_node *>(color_.sfnode::value().get());
    }

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    void
    triangle_fan_set_node::
    do_render_geometry(openvrml::viewer & /* viewer */,
                       const rendering_context /* context */)
    {}


    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool triangle_fan_set_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    triangle_fan_set_node::
    triangle_fan_set_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        color_(*this),
        coord_(*this),
        fan_count_(*this),
        normal_(*this),
        tex_coord_(*this),
        ccw_(true),
        color_per_vertex_(true),
        normal_per_vertex_(true),
        solid_(true)
    {}

    /**
     * @brief Destroy.
     */
    triangle_fan_set_node::~triangle_fan_set_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_rendering::triangle_fan_set_metatype::id =
    "urn:X-openvrml:node:TriangleFanSet";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c triangle_fan_set_metatype.
 */
openvrml_node_x3d_rendering::triangle_fan_set_metatype::
triangle_fan_set_metatype(openvrml::browser & browser):
    node_metatype(triangle_fan_set_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_rendering::triangle_fan_set_metatype::
~triangle_fan_set_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new @c node_type.
 * @param interfaces    the interfaces for the new @c node_type.
 *
 * @return a @c boost::shared_ptr to a @c node_type capable of creating
 *         TriangleFanSet nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  @c triangle_fan_set_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_x3d_rendering::triangle_fan_set_metatype::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
{
    typedef boost::array<node_interface, 10> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "coord"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "fanCount"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "normal"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "texCoord"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "ccw"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "colorPerVertex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "normalPerVertex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid")
    };
    typedef node_type_impl<triangle_fan_set_node> node_type_t;

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
                &triangle_fan_set_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &triangle_fan_set_node::color_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &triangle_fan_set_node::coord_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &triangle_fan_set_node::fan_count_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &triangle_fan_set_node::normal_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &triangle_fan_set_node::tex_coord_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &triangle_fan_set_node::ccw_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &triangle_fan_set_node::color_per_vertex_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &triangle_fan_set_node::normal_per_vertex_);
        } else if (*interface_ == *++supported_interface) {
            the_node_type.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &triangle_fan_set_node::solid_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
