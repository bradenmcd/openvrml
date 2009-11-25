// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
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

# include "geo_elevation_grid.h"
# include "geospatial-common.h"
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
     * @brief Represents GeoElevationGrid node instances.
     */
    class OPENVRML_LOCAL geo_elevation_grid_node :
        public abstract_node<geo_elevation_grid_node>,
        public geometry_node,
        public child_node {

        friend class openvrml_node_x3d_geospatial::geo_elevation_grid_metatype;

        class set_height_listener : public event_listener_base<self_t>,
                                    public mfdouble_listener {
        public:
            explicit set_height_listener(self_t & node);
            virtual ~set_height_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfdouble & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_height_listener set_height_listener_;
        exposedfield<sfnode> color_;
        exposedfield<sfnode> normal_;
        exposedfield<sfnode> tex_coord_;
        exposedfield<sffloat> y_scale_;
        sfbool ccw_;
        sfbool color_per_vertex_;
        sfdouble crease_angle_;
        sfvec3d geo_grid_origin_;
        sfnode geo_origin_;
        mfstring geo_system_;
        mfdouble height_;
        sfbool normal_per_vertex_;
        sfbool solid_;
        sfint32 x_dimension_;
        sfdouble x_spacing_;
        sfint32 z_dimension_;
        sfdouble z_spacing_;

    public:
        geo_elevation_grid_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_elevation_grid_node() OPENVRML_NOTHROW;

        virtual const color_node * color() const OPENVRML_NOTHROW;

    private:
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);

        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        rendering_context context);
    };


    /**
     * @var geo_elevation_grid_node::geo_elevation_grid_metatype
     *
     * @brief Class object for GeoElevationGrid nodes.
     */

    /**
     * @var geo_elevation_grid_node::set_height_listener geo_elevation_grid_node::set_height_listener_
     *
     * @brief set_height eventIn
     */

    /**
     * @var geo_elevation_grid_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var geo_elevation_grid_node::normal_
     *
     * @brief normal exposedField
     */

    /**
     * @var geo_elevation_grid_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var geo_elevation_grid_node::y_scale_
     *
     * @brief y_scale exposedField
     */

    /**
     * @var geo_elevation_grid_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var geo_elevation_grid_node::color_per_vertex_
     *
     * @brief color_per_vertex field
     */

    /**
     * @var geo_elevation_grid_node::crease_angle_
     *
     * @brief crease_angle field
     */

    /**
     * @var geo_elevation_grid_node::geo_grid_origin_
     *
     * @brief geo_grid_origin field
     */

    /**
     * @var geo_elevation_grid_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var geo_elevation_grid_node::geo_system_
     *
     * @brief geo_system field
     */

    /**
     * @var geo_elevation_grid_node::height_
     *
     * @brief height field
     */

    /**
     * @var geo_elevation_grid_node::normal_per_vertex_
     *
     * @brief normal_per_vertex field
     */

    /**
     * @var geo_elevation_grid_node::solid_
     *
     * @brief solid field
     */

    /**
     * @var geo_elevation_grid_node::x_dimension_
     *
     * @brief x_dimension field
     */

    /**
     * @var geo_elevation_grid_node::x_spacing_
     *
     * @brief x_spacing field
     */

    /**
     * @var geo_elevation_grid_node::z_dimension_
     *
     * @brief z_dimension field
     */

    /**
     * @var geo_elevation_grid_node::z_spacing_
     *
     * @brief z_spacing field
     */

    geo_elevation_grid_node::set_height_listener::
    set_height_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfdouble_listener(node)
    {}

    geo_elevation_grid_node::set_height_listener::
    ~set_height_listener() OPENVRML_NOTHROW
    {}

    void geo_elevation_grid_node::set_height_listener::
    do_process_event(const mfdouble & /* fraction */, double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    const color_node * geo_elevation_grid_node::color() const OPENVRML_NOTHROW
    {
        return node_cast<color_node *>(color_.sfnode::value().get());
    }

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a @c viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    void
    geo_elevation_grid_node::
    do_render_geometry(openvrml::viewer & /* viewer */,
                       const rendering_context /* context */)
    {}


    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool geo_elevation_grid_node::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        return this->geo_origin_.value()
            && this->geo_origin_.value()->modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    geo_elevation_grid_node::
    geo_elevation_grid_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        set_height_listener_(*this),
        color_(*this),
        normal_(*this),
        tex_coord_(*this),
        y_scale_(*this, 1),
        ccw_(true),
        color_per_vertex_(true),
        geo_system_(openvrml_node_x3d_geospatial::default_geo_system),
        normal_per_vertex_(true),
        solid_(true),
        x_spacing_(1),
        z_spacing_(1)
    {}

    /**
     * @brief Destroy.
     */
    geo_elevation_grid_node::~geo_elevation_grid_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const
openvrml_node_x3d_geospatial::geo_elevation_grid_metatype::id =
    "urn:X-openvrml:node:GeoElevationGrid";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this
 *                @c geo_elevation_grid_metatype.
 */
openvrml_node_x3d_geospatial::geo_elevation_grid_metatype::
geo_elevation_grid_metatype(openvrml::browser & browser):
    node_metatype(geo_elevation_grid_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_geospatial::geo_elevation_grid_metatype::
~geo_elevation_grid_metatype()
    OPENVRML_NOTHROW
{}

# define GEO_ELEVATION_GRID_INTERFACE_SEQ                               \
    ((exposedfield, sfnode,   "metadata",        metadata))             \
    ((eventin,      mfdouble, "set_height",      set_height_listener_)) \
    ((exposedfield, sfnode,   "color",           color_))               \
    ((exposedfield, sfnode,   "normal",          normal_))              \
    ((exposedfield, sfnode,   "texCoord",        tex_coord_))           \
    ((exposedfield, sffloat,  "yScale",          y_scale_))             \
    ((field,        sfbool,   "ccw",             ccw_))                 \
    ((field,        sfbool,   "colorPerVertex",  color_per_vertex_))    \
    ((field,        sfdouble, "creaseAngle",     crease_angle_))        \
    ((field,        sfvec3d,  "geoGridOrigin",   geo_grid_origin_))     \
    ((field,        mfstring, "geoSystem",       geo_system_))          \
    ((field,        mfdouble, "height",          height_))              \
    ((field,        sfbool,   "normalPerVertex", normal_per_vertex_))   \
    ((field,        sfbool,   "solid",           solid_))               \
    ((field,        sfint32,  "xDimension",      x_dimension_))         \
    ((field,        sfdouble, "xSpacing",        x_spacing_))           \
    ((field,        sfint32,  "zDimension",      z_dimension_))         \
    ((field,        sfdouble, "zSpacing",        z_spacing_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_geospatial,
                                              geo_elevation_grid_metatype,
                                              geo_elevation_grid_node,
                                              GEO_ELEVATION_GRID_INTERFACE_SEQ)
