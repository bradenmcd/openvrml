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
# include <private.h>
# include "indexed_line_set.h"
# include "abstract_indexed_set.h"

namespace {

    class OPENVRML_LOCAL indexed_line_set_node :
        public openvrml_node_vrml97::abstract_indexed_set_node<indexed_line_set_node> {

        friend class openvrml_node_vrml97::indexed_line_set_metatype;

        exposedfield<openvrml::mfint32> vertex_count_;

    public:
        indexed_line_set_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~indexed_line_set_node() OPENVRML_NOTHROW;

    private:
        virtual openvrml::viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    openvrml::rendering_context context);
        virtual bool do_emissive() const OPENVRML_NOTHROW;
    };

    /**
     * @class indexed_line_set_node
     *
     * @brief Represents IndexedLineSet node instances.
     */

    /**
     * @var class indexed_line_set_node::indexed_line_set_metatype
     *
     * @brief Class object for IndexedLineSet nodes.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope the scope to which the node belongs.
     */
    indexed_line_set_node::
    indexed_line_set_node(const openvrml::node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml_node_vrml97::abstract_indexed_set_node<indexed_line_set_node>(
            type, scope),
        vertex_count_(*this)
    {}

    /**
     * @brief Destroy.
     */
    indexed_line_set_node::~indexed_line_set_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     *
     * @todo colors
     */
    openvrml::viewer::object_t
    indexed_line_set_node::
    do_render_geometry(openvrml::viewer & viewer, openvrml::rendering_context)
    {
        using std::vector;
        using openvrml::node_cast;
        using openvrml::vec3f;

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

        openvrml::viewer::object_t obj =
            viewer.insert_line_set(coord, this->coord_index_.value(),
                                   this->color_per_vertex_.value(),
                                   color, this->color_index_.value());

        if (colorNode) { colorNode->modified(false); }
        if (coordinateNode) { coordinateNode->modified(false); }

        return obj;
    }

    /**
     * @brief Indicate that line sets should be drawn with the emissive color.
     *
     * @return @c true.
     */
    bool indexed_line_set_node::do_emissive() const OPENVRML_NOTHROW
    {
        return true;
    }
}

 
/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::indexed_line_set_metatype::id =
    "urn:X-openvrml:node:IndexedLineSet";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype object.
 */
openvrml_node_vrml97::indexed_line_set_metatype::
indexed_line_set_metatype(openvrml::browser & browser):
    node_metatype(indexed_line_set_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::indexed_line_set_metatype::~indexed_line_set_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating IndexedLineSet nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  indexed_line_set_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::indexed_line_set_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 9> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::eventin_id,
                       field_value::mfint32_id,
                       "set_colorIndex"),
        node_interface(node_interface::eventin_id,
                       field_value::mfint32_id,
                       "set_coordIndex"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "coord"),
        node_interface(node_interface::field_id,
                       field_value::mfint32_id,
                       "colorIndex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "colorPerVertex"),
        node_interface(node_interface::field_id,
                       field_value::mfint32_id,
                       "coordIndex"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfint32_id,
                       "vertexCount")
    };

    typedef node_impl_util::node_type_impl<indexed_line_set_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & indexedLineSetNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            indexedLineSetNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    indexed_line_set_node::set_color_index_listener>(
                        &indexed_line_set_node::set_color_index_)));
        } else if (*interface_ == *++supported_interface) {
            indexedLineSetNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    indexed_line_set_node::set_coord_index_listener>(
                        &indexed_line_set_node::set_coord_index_)));
        } else if (*interface_ == *++supported_interface) {
            indexedLineSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<indexed_line_set_node>::
                    exposedfield<sfnode> >(
                        &indexed_line_set_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_line_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_line_set_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_line_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_line_set_node::color_)));
        } else if (*interface_ == *++supported_interface) {
            indexedLineSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<indexed_line_set_node>::
                    exposedfield<sfnode> >(
                        &indexed_line_set_node::coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_line_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_line_set_node::coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_line_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_line_set_node::coord_)));
        } else if (*interface_ == *++supported_interface) {
            indexedLineSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_line_set_node::color_index_)));
        } else if (*interface_ == *++supported_interface) {
            indexedLineSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_line_set_node::color_per_vertex_)));
        } else if (*interface_ == *++supported_interface) {
            indexedLineSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_line_set_node::coord_index_)));
        } else if (*interface_ == *++supported_interface) {
            indexedLineSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<indexed_line_set_node>::exposedfield<sfnode> >(
                        &indexed_line_set_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<indexed_line_set_node>::exposedfield<sfnode> >(
                        &indexed_line_set_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<indexed_line_set_node>::exposedfield<sfnode> >(
                        &indexed_line_set_node::metadata)));
        } else if (*interface_ == *++supported_interface) {
            indexedLineSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<indexed_line_set_node>::exposedfield<mfint32> >(
                        &indexed_line_set_node::vertex_count_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<indexed_line_set_node>::exposedfield<mfint32> >(
                        &indexed_line_set_node::vertex_count_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<indexed_line_set_node>::exposedfield<mfint32> >(
                        &indexed_line_set_node::vertex_count_)));
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
