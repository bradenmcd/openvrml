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

# include "indexed_face_set.h"
# include "abstract_indexed_set.h"
# include <private.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

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

    private:
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);

        virtual const openvrml::bounding_volume & do_bounding_volume() const;
        virtual void do_render_geometry(openvrml::viewer & viewer,
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
    bool indexed_face_set_node::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        return (this->color_.value() && this->color_.value()->modified())
            || (this->coord_.value() && this->coord_.value()->modified())
            || (this->normal_.value() && this->normal_.value()->modified())
            || (this->tex_coord_.value()
                && this->tex_coord_.value()->modified());
    }

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param v         a viewer.
     * @param context   the rendering context.
     *
     * @todo stripify, crease angle, generate normals ...
     */
    void
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

        v.insert_shell(*this,
                       optMask,
                       coord, this->coord_index_.value(),
                       color, this->color_index_.value(),
                       normal, this->normal_index_.value(),
                       texCoord, this->tex_coord_index_.value());

        if (colorNode) { colorNode->modified(false); }
        if (coordinateNode) { coordinateNode->modified(false); }
        if (normalNode) { normalNode->modified(false); }
        if (texCoordNode) { texCoordNode->modified(false); }
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

# define INDEXED_FACE_SET_INTERFACE_SEQ                                 \
    ((eventin,      mfint32, "set_colorIndex",    set_color_index_))    \
    ((eventin,      mfint32, "set_coordIndex",    set_coord_index_))    \
    ((eventin,      mfint32, "set_normalIndex",   set_normal_index_listener_)) \
    ((eventin,      mfint32, "set_texCoordIndex", set_tex_coord_index_listener_)) \
    ((exposedfield, sfnode,  "color",             color_))              \
    ((exposedfield, sfnode,  "coord",             coord_))              \
    ((exposedfield, sfnode,  "normal",            normal_))             \
    ((exposedfield, sfnode,  "texCoord",          tex_coord_))          \
    ((field,        sfbool,  "ccw",               ccw_))                \
    ((field,        mfint32, "colorIndex",        color_index_))        \
    ((field,        sfbool,  "colorPerVertex",    color_per_vertex_))   \
    ((field,        sfbool,  "convex",            convex_))             \
    ((field,        mfint32, "coordIndex",        coord_index_))        \
    ((field,        sffloat, "creaseAngle",       crease_angle_))       \
    ((field,        mfint32, "normalIndex",       normal_index_))       \
    ((field,        sfbool,  "normalPerVertex",   normal_per_vertex_))  \
    ((field,        sfbool,  "solid",             solid_))              \
    ((field,        mfint32, "texCoordIndex",     tex_coord_index_))    \
    ((exposedfield, sfnode,  "metadata",          metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              indexed_face_set_metatype,
                                              indexed_face_set_node,
                                              INDEXED_FACE_SET_INTERFACE_SEQ)
