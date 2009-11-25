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

# include "elevation_grid.h"
# include <private.h>
# include <openvrml/node_impl_util.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL elevation_grid_node :
        public openvrml::node_impl_util::abstract_node<elevation_grid_node>,
        public openvrml::geometry_node {

        friend class openvrml_node_vrml97::elevation_grid_metatype;

        class set_height_listener : public openvrml::node_impl_util::event_listener_base<self_t>,
                                    public mffloat_listener {
        public:
            explicit set_height_listener(elevation_grid_node & node);
            virtual ~set_height_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::mffloat & height,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_height_listener set_height_listener_;
        exposedfield<openvrml::sfnode> color_;
        exposedfield<openvrml::sfnode> normal_;
        exposedfield<openvrml::sfnode> tex_coord_;
        openvrml::mffloat height_;
        openvrml::sfbool ccw_;
        openvrml::sfbool color_per_vertex_;
        openvrml::sffloat crease_angle_;
        openvrml::sfbool normal_per_vertex_;
        openvrml::sfbool solid_;
        openvrml::sfint32 x_dimension_;
        openvrml::sffloat x_spacing_;
        openvrml::sfint32 z_dimension_;
        openvrml::sffloat z_spacing_;

    public:
        elevation_grid_node(const openvrml::node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~elevation_grid_node() OPENVRML_NOTHROW;

    private:
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);

        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        openvrml::rendering_context context);
    };

    /**
     * @class elevation_grid_node
     *
     * @brief ElevationGrid node instances.
     */

    /**
     * @var class elevation_grid_node::elevation_grid_metatype
     *
     * @brief Class object for ElevationGrid nodes.
     */

    /**
     * @internal
     *
     * @class elevation_grid_node::set_height_listener
     *
     * @brief set_height event handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  elevation_grid_node.
     */
    elevation_grid_node::set_height_listener::
    set_height_listener(elevation_grid_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<elevation_grid_node>(node),
        mffloat_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    elevation_grid_node::set_height_listener::
    ~set_height_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param height    height value.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    elevation_grid_node::set_height_listener::
    do_process_event(const openvrml::mffloat & height, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            elevation_grid_node & elevation_grid =
                dynamic_cast<elevation_grid_node &>(this->node());

            elevation_grid.height_ = height;
            elevation_grid.node::modified(true);

        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var elevation_grid_node::set_height_listener elevation_grid_node::set_height_listener_
     *
     * @brief set_height eventIn handler.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<elevation_grid_node>::exposedfield<openvrml::sfnode> elevation_grid_node::color_
     *
     * @brief color exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<elevation_grid_node>::exposedfield<openvrml::sfnode> elevation_grid_node::normal_
     *
     * @brief normal exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<elevation_grid_node>::exposedfield<openvrml::sfnode> elevation_grid_node::tex_coord_
     *
     * @brief texCoord exposedField.
     */

    /**
     * @var openvrml::sfbool elevation_grid_node::ccw_
     *
     * @brief ccw field.
     */

    /**
     * @var openvrml::sfbool elevation_grid_node::color_per_vertex_
     *
     * @brief colorPerVertex field.
     */

    /**
     * @var openvrml::sffloat elevation_grid_node::crease_angle_
     *
     * @brief creaseAngle field.
     */

    /**
     * @var openvrml::mffloat elevation_grid_node::height_
     *
     * @brief height field.
     */

    /**
     * @var openvrml::sfbool elevation_grid_node::normal_per_vertex_
     *
     * @brief normalPerVertex field.
     */

    /**
     * @var openvrml::sfbool elevation_grid_node::solid_
     *
     * @brief solid field.
     */

    /**
     * @var openvrml::sfint32 elevation_grid_node::x_dimension_
     *
     * @brief xDimension field.
     */

    /**
     * @var openvrml::sffloat elevation_grid_node::x_spacing_
     *
     * @brief xSpacing field.
     */

    /**
     * @var openvrml::sfint32 elevation_grid_node::z_dimension_
     *
     * @brief zDimension field.
     */

    /**
     * @var openvrml::sffloat ,elevation_grid_node::z_spacing_
     *
     * @brief zSpacing field.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    elevation_grid_node::
    elevation_grid_node(const openvrml::node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<elevation_grid_node>(type, scope),
        geometry_node(type, scope),
        set_height_listener_(*this),
        color_(*this),
        normal_(*this),
        tex_coord_(*this),
        ccw_(true),
        color_per_vertex_(true),
        normal_per_vertex_(true),
        solid_(true),
        x_dimension_(0),
        x_spacing_(1.0f),
        z_dimension_(0),
        z_spacing_(1.0f)
    {}

    /**
     * @brief Destroy.
     */
    elevation_grid_node::~elevation_grid_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool elevation_grid_node::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        return (this->color_.value() && this->color_.value()->modified())
            || (this->normal_.value() && this->normal_.value()->modified())
            || (this->tex_coord_.value()
                && this->tex_coord_.value()->modified());
    }

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param v         a viewer.
     * @param context   the rendering context.
     */
    void elevation_grid_node::do_render_geometry(openvrml::viewer & v,
                                                 openvrml::rendering_context)
    {
        using openvrml::node_cast;

        openvrml::color_node * const colorNode =
            node_cast<openvrml::color_node *>(
                this->color_.sfnode::value().get());
        openvrml::normal_node * const normalNode =
            node_cast<openvrml::normal_node *>(
                this->normal_.sfnode::value().get());
        openvrml::texture_coordinate_node * const texCoordNode =
            node_cast<openvrml::texture_coordinate_node *>(
                this->tex_coord_.sfnode::value().get());

        if (!this->height_.mffloat::value().empty()) {
            using std::vector;
            using openvrml::vec2f;
            using openvrml::vec3f;
            using openvrml::viewer;

            const vector<openvrml::color> & color =
                colorNode
                ? colorNode->color()
                : vector<openvrml::color>();

            const vector<vec3f> & normal =
                normalNode
                ? normalNode->vector()
                : vector<vec3f>();

            const vector<vec2f> & texCoord =
                texCoordNode
                ? texCoordNode->point()
                : vector<vec2f>();
            // insert geometry
            unsigned int optMask = 0;
            if (this->ccw_.value()) {
                optMask |= viewer::mask_ccw;
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

            v.insert_elevation_grid(*this,
                                    optMask,
                                    this->height_.mffloat::value(),
                                    this->x_dimension_.sfint32::value(),
                                    this->z_dimension_.sfint32::value(),
                                    this->x_spacing_.sffloat::value(),
                                    this->z_spacing_.sffloat::value(),
                                    color,
                                    normal,
                                    texCoord);
        }

        if (colorNode) { colorNode->modified(false); }
        if (normalNode) { normalNode->modified(false); }
        if (texCoordNode) { texCoordNode->modified(false); }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::elevation_grid_metatype::id =
    "urn:X-openvrml:node:ElevationGrid";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::elevation_grid_metatype::
elevation_grid_metatype(openvrml::browser & browser):
    node_metatype(elevation_grid_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::elevation_grid_metatype::~elevation_grid_metatype()
    OPENVRML_NOTHROW
{}

# define ELEVATION_GRID_INTERFACE_SEQ                                  \
    ((eventin,      mffloat, "set_height",      set_height_listener_)) \
    ((exposedfield, sfnode,  "color",           color_))               \
    ((exposedfield, sfnode,  "normal",          normal_))              \
    ((exposedfield, sfnode,  "texCoord",        tex_coord_))           \
    ((field,        mffloat, "height",          height_))              \
    ((field,        sfbool,  "ccw",             ccw_))                 \
    ((field,        sfbool,  "colorPerVertex",  color_per_vertex_))    \
    ((field,        sffloat, "creaseAngle",     crease_angle_))        \
    ((field,        sfbool,  "normalPerVertex", normal_per_vertex_))   \
    ((field,        sfbool,  "solid",           solid_))               \
    ((field,        sfint32, "xDimension",      x_dimension_))         \
    ((field,        sffloat, "xSpacing",        x_spacing_))           \
    ((field,        sfint32, "zDimension",      z_dimension_))         \
    ((field,        sffloat, "zSpacing",        z_spacing_))           \
    ((exposedfield, sfnode,  "metadata",        metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              elevation_grid_metatype,
                                              elevation_grid_node,
                                              ELEVATION_GRID_INTERFACE_SEQ)
