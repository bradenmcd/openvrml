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
# include <openvrml/node_impl_util.h>
# include <private.h>
# include "point_set.h"

namespace {

    class OPENVRML_LOCAL point_set_node :
        public openvrml::node_impl_util::abstract_node<point_set_node>,
        public openvrml::geometry_node {

        friend class openvrml_node_vrml97::point_set_metatype;

        exposedfield<openvrml::sfnode> color_;
        exposedfield<openvrml::sfnode> coord_;

        openvrml::bounding_sphere bsphere;

    public:
        point_set_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~point_set_node() OPENVRML_NOTHROW;

        virtual bool modified() const;

    private:
        virtual const openvrml::bounding_volume &
        do_bounding_volume() const;

        virtual openvrml::viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    openvrml::rendering_context context);
        virtual bool do_emissive() const OPENVRML_NOTHROW;

        void recalc_bsphere();
    };

    /**
     * @class point_set_node
     *
     * @brief Represents PointSet node instances.
     */

    /**
     * @var class point_set_node::point_set_metatype
     *
     * @brief Class object for PointSet instances.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<point_set_node>::exposedfield<openvrml::sfnode> point_set_node::color_
     *
     * @brief color exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<point_set_node>::exposedfield<openvrml::sfnode> point_set_node::coord_
     *
     * @brief coord exposedField.
     */

    /**
     * @var openvrml::bounding_sphere point_set_node::bsphere
     *
     * @brief Bounding volume.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope the scope to which the node belongs.
     */
    point_set_node::
    point_set_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<point_set_node>(type, scope),
        geometry_node(type, scope),
        color_(*this),
        coord_(*this)
    {
        this->bounding_volume_dirty(true);
    }

    /**
     * @brief Destroy.
     */
    point_set_node::~point_set_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool point_set_node::modified() const
    {
        return this->node::modified()
            || (this->color_.sfnode::value()
                && this->color_.sfnode::value()->modified())
            || (this->coord_.sfnode::value()
                && this->coord_.sfnode::value()->modified());
    }

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     */
    openvrml::viewer::object_t
    point_set_node::
    do_render_geometry(openvrml::viewer & viewer,
                       const openvrml::rendering_context context)
    {
        using std::vector;
        using openvrml::node_cast;
        using openvrml::vec3f;

        if (context.draw_bounding_spheres) {
            using boost::polymorphic_downcast;
            using openvrml::bounding_sphere;
            using openvrml::bounding_volume;
            const bounding_sphere & bs =
                *polymorphic_downcast<const bounding_sphere *>(
                    &this->bounding_volume());
            viewer.draw_bounding_sphere(bs, bounding_volume::intersection(4));
        }

        openvrml::coordinate_node * const coordinateNode =
            node_cast<openvrml::coordinate_node *>(
                this->coord_.sfnode::value().get());
        const vector<vec3f> & coord = coordinateNode
            ? coordinateNode->point()
            : vector<vec3f>();

        openvrml::color_node * const colorNode =
            node_cast<openvrml::color_node *>(
                this->color_.sfnode::value().get());
        const vector<openvrml::color> & color = colorNode
            ? colorNode->color()
            : vector<openvrml::color>();

        openvrml::viewer::object_t obj = viewer.insert_point_set(coord, color);

        if (colorNode) { colorNode->modified(false); }
        if (coordinateNode) { coordinateNode->modified(false); }

        return obj;
    }

    /**
     * @brief Indicate that point sets should be drawn with the emissive color.
     *
     * @return @c true.
     */
    bool point_set_node::do_emissive() const OPENVRML_NOTHROW
    {
        return true;
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void point_set_node::recalc_bsphere()
    {
        using openvrml::node_cast;
        using openvrml::vec3f;
        this->bsphere = openvrml::bounding_sphere();
        openvrml::coordinate_node * const coordinateNode =
            node_cast<openvrml::coordinate_node *>(
                this->coord_.sfnode::value().get());
        if (coordinateNode) {
            const std::vector<vec3f> & coord = coordinateNode->point();
            for(std::vector<vec3f>::const_iterator vec(coord.begin());
                vec != coord.end(); ++vec) {
                this->bsphere.extend(*vec);
            }
        }
        this->bounding_volume_dirty(false);
    }

    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    point_set_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<point_set_node *>(this)->recalc_bsphere();
        }
        return this->bsphere;
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::point_set_metatype::id =
    "urn:X-openvrml:node:PointSet";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::point_set_metatype::
point_set_metatype(openvrml::browser & browser):
    node_metatype(point_set_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::point_set_metatype::~point_set_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating PointSet nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by point_set_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::point_set_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 3> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "coord"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<point_set_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & pointSetNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            pointSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<point_set_node>::exposedfield<sfnode> >(
                        &point_set_node::color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<point_set_node>::exposedfield<sfnode> >(
                        &point_set_node::color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<point_set_node>::exposedfield<sfnode> >(
                        &point_set_node::color_)));
        } else if (*interface_ == *++supported_interface) {
            pointSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<point_set_node>::exposedfield<sfnode> >(
                        &point_set_node::coord_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<point_set_node>::exposedfield<sfnode> >(
                        &point_set_node::coord_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<point_set_node>::exposedfield<sfnode> >(
                        &point_set_node::coord_)));
        } else if (*interface_ == *++supported_interface) {
            pointSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<point_set_node>::exposedfield<sfnode> >(
                        &point_set_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<point_set_node>::exposedfield<sfnode> >(
                        &point_set_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<point_set_node>::exposedfield<sfnode> >(
                        &point_set_node::metadata)));
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
