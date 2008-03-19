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
# include "indexed_face_set.h"
# include "abstract_indexed_set.h"

namespace {

    class OPENVRML_LOCAL indexed_face_set_node :
        public openvrml_node_vrml97::abstract_indexed_set_node<indexed_face_set_node> {

        friend class openvrml_node_vrml97::indexed_face_set_metatype;

        class set_normal_index_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public mfint32_listener {
        public:
            explicit set_normal_index_listener(
                indexed_face_set_node & node);
            virtual ~set_normal_index_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::mfint32 & normal_index,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_tex_coord_index_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public mfint32_listener {
        public:
            explicit set_tex_coord_index_listener(
                indexed_face_set_node & node);
            virtual ~set_tex_coord_index_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::mfint32 & tex_coord_index,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_normal_index_listener set_normal_index_listener_;
        set_tex_coord_index_listener set_tex_coord_index_listener_;
        exposedfield<openvrml::sfnode> normal_;
        exposedfield<openvrml::sfnode> tex_coord_;
        openvrml::sfbool ccw_;
        openvrml::sfbool convex_;
        openvrml::sffloat crease_angle_;
        openvrml::mfint32 normal_index_;
        openvrml::sfbool normal_per_vertex_;
        openvrml::sfbool solid_;
        openvrml::mfint32 tex_coord_index_;

        openvrml::bounding_sphere bsphere;

    public:
        indexed_face_set_node(
            const openvrml::node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~indexed_face_set_node() OPENVRML_NOTHROW;

        virtual bool modified() const;

    private:
        virtual const openvrml::bounding_volume &
        do_bounding_volume() const;

        virtual openvrml::viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    openvrml::rendering_context context);

        void recalc_bsphere();
    };

    /**
     * @class indexed_face_set_node
     *
     * @brief Represents IndexedFaceSet node instances.
     */

    /**
     * @internal
     *
     * @class indexed_face_set_node::set_normal_index_listener
     *
     * @brief set_normalIndex event handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  indexed_face_set_node.
     */
    indexed_face_set_node::set_normal_index_listener::
    set_normal_index_listener(indexed_face_set_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<indexed_face_set_node>(node),
        mfint32_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    indexed_face_set_node::set_normal_index_listener::
    ~set_normal_index_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param normal_index normalIndex.
     * @param timestamp   the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    indexed_face_set_node::set_normal_index_listener::
    do_process_event(const openvrml::mfint32 & normal_index, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            indexed_face_set_node & indexed_face_set =
                dynamic_cast<indexed_face_set_node &>(this->node());

            indexed_face_set.normal_index_ = normal_index;
            indexed_face_set.node::modified(true);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class indexed_face_set_node::set_tex_coord_index_listener
     *
     * @brief set_coordIndex event handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  indexed_face_set_node.
     */
    indexed_face_set_node::set_tex_coord_index_listener::
    set_tex_coord_index_listener(indexed_face_set_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<indexed_face_set_node>(node),
        mfint32_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    indexed_face_set_node::set_tex_coord_index_listener::
    ~set_tex_coord_index_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param tex_coord_index   texCoordIndex.
     * @param timestamp         the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    indexed_face_set_node::set_tex_coord_index_listener::
    do_process_event(const openvrml::mfint32 & tex_coord_index, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            indexed_face_set_node & indexed_face_set =
                dynamic_cast<indexed_face_set_node &>(this->node());

            indexed_face_set.tex_coord_index_ = tex_coord_index;
            indexed_face_set.node::modified(true);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var class indexed_face_set_node::indexed_face_set_metatype
     *
     * @brief Class object for IndexedFaceSet nodes.
     */

    /**
     * @var indexed_face_set_node::set_normal_index_listener indexed_face_set_node::set_normal_index_listener_
     *
     * @brief set_normalIndex eventIn handler.
     */

    /**
     * @var indexed_face_set_node::set_tex_coord_index_listener indexed_face_set_node::set_tex_coord_index_listener_
     *
     * @brief set_texCoordIndex eventIn handler.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<indexed_face_set_node>::exposedfield<openvrml::sfnode> indexed_face_set_node::normal_
     *
     * @brief normal exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<indexed_face_set_node>::exposedfield<openvrml::sfnode> indexed_face_set_node::tex_coord_
     *
     * @brief texCoord exposedField.
     */

    /**
     * @var openvrml::sfbool indexed_face_set_node::ccw_
     *
     * @brief ccw field.
     */

    /**
     * @var openvrml::sfbool indexed_face_set_node::convex_
     *
     * @brief convex field.
     */

    /**
     * @var openvrml::sffloat indexed_face_set_node::crease_angle_
     *
     * @brief creaseAngle field.
     */

    /**
     * @var openvrml::mfint32 indexed_face_set_node::normal_index_
     *
     * @brief set_normalIndex eventIn.
     */

    /**
     * @var openvrml::sfbool indexed_face_set_node::normal_per_vertex_
     *
     * @brief normalPerVertex field.
     */

    /**
     * @var openvrml::sfbool indexed_face_set_node::solid_
     *
     * @brief solid field.
     */

    /**
     * @var openvrml::mfint32 indexed_face_set_node::tex_coord_index_
     *
     * @brief set_texCoordIndex eventIn.
     */

    /**
     * @var openvrml::bounding_sphere indexed_face_set_node::bsphere
     *
     * @brief Bounding volume.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope     the scope to which the node belongs.
     */
    indexed_face_set_node::
    indexed_face_set_node(const openvrml::node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml_node_vrml97::abstract_indexed_set_node<indexed_face_set_node>(
            type, scope),
        set_normal_index_listener_(*this),
        set_tex_coord_index_listener_(*this),
        normal_(*this),
        tex_coord_(*this),
        ccw_(true),
        convex_(true),
        crease_angle_(0.0),
        normal_per_vertex_(true),
        solid_(true)
    {
        this->bounding_volume_dirty(true);
    }

    /**
     * @brief Destroy.
     */
    indexed_face_set_node::~indexed_face_set_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool indexed_face_set_node::modified() const
    {
        return this->node::modified()
            || (this->color_.sfnode::value()
                && this->color_.sfnode::value()->modified())
            || (this->coord_.sfnode::value()
                && this->coord_.sfnode::value()->modified())
            || (this->normal_.sfnode::value()
                && this->normal_.sfnode::value()->modified())
            || (this->tex_coord_.sfnode::value()
                && this->tex_coord_.sfnode::value()->modified());
    }

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param v         a viewer.
     * @param context   the rendering context.
     *
     * @todo stripify, crease angle, generate normals ...
     */
    openvrml::viewer::object_t
    indexed_face_set_node::
    do_render_geometry(openvrml::viewer & v,
                       const openvrml::rendering_context context)
    {
        using std::vector;
        using namespace openvrml;

        if (context.draw_bounding_spheres) {
            using boost::polymorphic_downcast;
            using openvrml::bounding_sphere;
            const bounding_sphere & bs =
                *polymorphic_downcast<const bounding_sphere *>(
                    &this->bounding_volume());
            v.draw_bounding_sphere(
                bs,
                static_cast<bounding_volume::intersection>(4));
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

        openvrml::normal_node * const normalNode =
            node_cast<openvrml::normal_node *>(
                this->normal_.sfnode::value().get());
        const vector<vec3f> & normal = normalNode
            ? normalNode->vector()
            : vector<vec3f>();

        openvrml::texture_coordinate_node * const texCoordNode =
            node_cast<openvrml::texture_coordinate_node *>(
                this->tex_coord_.sfnode::value().get());
        const vector<vec2f> & texCoord = texCoordNode
            ? texCoordNode->point()
            : vector<vec2f>();

        unsigned int optMask = 0;
        if (this->ccw_.value()) {
            optMask |= viewer::mask_ccw;
        }
        if (this->convex_.value()) {
            optMask |= viewer::mask_convex;
        }
        if (this->solid_.value()) {
            optMask |= viewer::mask_solid;
        }
        if (this->color_per_vertex_.value()) {
            optMask |= viewer::mask_color_per_vertex;
        }
        if (this->normal_per_vertex_.value()) {
            optMask |= viewer::mask_normal_per_vertex;
        }

        const openvrml::viewer::object_t obj =
            v.insert_shell(optMask,
                           coord, this->coord_index_.value(),
                           color, this->color_index_.value(),
                           normal, this->normal_index_.value(),
                           texCoord, this->tex_coord_index_.value());

        if (colorNode) { colorNode->modified(false); }
        if (coordinateNode) { coordinateNode->modified(false); }
        if (normalNode) { normalNode->modified(false); }
        if (texCoordNode) { texCoordNode->modified(false); }

        return obj;
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void indexed_face_set_node::recalc_bsphere()
    {
        using openvrml::node_cast;
        using openvrml::vec3f;

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
            this->bsphere = openvrml::bounding_sphere();
            this->bsphere.enclose(coord);
        }
        this->bounding_volume_dirty(false);
    }

    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    indexed_face_set_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<indexed_face_set_node *>(this)->recalc_bsphere();
        }
        return this->bsphere;
    }
}

 
/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::indexed_face_set_metatype::id =
    "urn:X-openvrml:node:IndexedFaceSet";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype
 *                object.
 */
openvrml_node_vrml97::indexed_face_set_metatype::
indexed_face_set_metatype(openvrml::browser & browser):
    node_metatype(indexed_face_set_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::indexed_face_set_metatype::~indexed_face_set_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating IndexedFaceSet nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by
 *                                  indexed_face_set_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::indexed_face_set_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 19> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::eventin_id,
                       field_value::mfint32_id,
                       "set_colorIndex"),
        node_interface(node_interface::eventin_id,
                       field_value::mfint32_id,
                       "set_coordIndex"),
        node_interface(node_interface::eventin_id,
                       field_value::mfint32_id,
                       "set_normalIndex"),
        node_interface(node_interface::eventin_id,
                       field_value::mfint32_id,
                       "set_texCoordIndex"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "coord"),
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
                       field_value::mfint32_id,
                       "colorIndex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "colorPerVertex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "convex"),
        node_interface(node_interface::field_id,
                       field_value::mfint32_id,
                       "coordIndex"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "creaseAngle"),
        node_interface(node_interface::field_id,
                       field_value::mfint32_id,
                       "normalIndex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "normalPerVertex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid"),
        node_interface(node_interface::field_id,
                       field_value::mfint32_id,
                       "texCoordIndex"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<indexed_face_set_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & indexedFaceSetNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    indexed_face_set_node::set_color_index_listener>(
                        &indexed_face_set_node::set_color_index_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    indexed_face_set_node::set_coord_index_listener>(
                        &indexed_face_set_node::set_coord_index_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    indexed_face_set_node::set_normal_index_listener>(
                        &indexed_face_set_node::set_normal_index_listener_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    indexed_face_set_node::set_tex_coord_index_listener>(
                        &indexed_face_set_node::set_tex_coord_index_listener_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<indexed_face_set_node>::
                    exposedfield<sfnode> >(
                        &indexed_face_set_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_face_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_face_set_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_face_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_face_set_node::color_)));
    } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<indexed_face_set_node>::
                    exposedfield<sfnode> >(
                        &indexed_face_set_node::coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_face_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_face_set_node::coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_face_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_face_set_node::coord_)));
} else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<indexed_face_set_node>::
                    exposedfield<sfnode> >(
                        &indexed_face_set_node::normal_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_face_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_face_set_node::normal_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_face_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_face_set_node::normal_)));
} else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<indexed_face_set_node>::
                    exposedfield<sfnode> >(
                        &indexed_face_set_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_face_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_face_set_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_face_set_node>::
                        exposedfield<sfnode> >(
                            &indexed_face_set_node::tex_coord_)));
    } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_face_set_node::ccw_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_face_set_node::color_index_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_face_set_node::color_per_vertex_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_face_set_node::convex_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_face_set_node::coord_index_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &indexed_face_set_node::crease_angle_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_face_set_node::normal_index_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_face_set_node::normal_per_vertex_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_face_set_node::solid_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_face_set_node::tex_coord_index_)));
        } else if (*interface == *++supported_interface) {
            indexedFaceSetNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<indexed_face_set_node>::exposedfield<sfnode> >(
                        &indexed_face_set_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<indexed_face_set_node>::exposedfield<sfnode> >(
                        &indexed_face_set_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<indexed_face_set_node>::exposedfield<sfnode> >(
                        &indexed_face_set_node::metadata)));
        } else {
            throw unsupported_interface(*interface);
        }
}
    return type;
}
