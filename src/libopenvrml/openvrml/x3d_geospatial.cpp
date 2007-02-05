// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2006  Braden McDaniel
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <private.h>
# include "browser.h"
# include "node_impl_util.h"
# include "x3d_geospatial.h"

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Class object for GeoCoordinate nodes.
     */
    class OPENVRML_LOCAL geo_coordinate_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit geo_coordinate_metatype(openvrml::browser & browser);
        virtual ~geo_coordinate_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for GeoElevationGrid nodes.
     */
    class OPENVRML_LOCAL geo_elevation_grid_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit geo_elevation_grid_metatype(openvrml::browser & browser);
        virtual ~geo_elevation_grid_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for GeoLocation nodes.
     */
    class OPENVRML_LOCAL geo_location_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit geo_location_metatype(openvrml::browser & browser);
        virtual ~geo_location_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for GeoLOD nodes.
     */
    class OPENVRML_LOCAL geo_lod_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit geo_lod_metatype(openvrml::browser & browser);
        virtual ~geo_lod_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for GeoMetadata nodes.
     */
    class OPENVRML_LOCAL geo_metadata_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit geo_metadata_metatype(openvrml::browser & browser);
        virtual ~geo_metadata_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for GeoOrigin nodes.
     */
    class OPENVRML_LOCAL geo_origin_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit geo_origin_metatype(openvrml::browser & browser);
        virtual ~geo_origin_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for GeoPositionInterpolator nodes.
     */
    class OPENVRML_LOCAL geo_position_interpolator_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit geo_position_interpolator_metatype(openvrml::browser & browser);
        virtual ~geo_position_interpolator_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for GeoTouchSensor nodes.
     */
    class OPENVRML_LOCAL geo_touch_sensor_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit geo_touch_sensor_metatype(openvrml::browser & browser);
        virtual ~geo_touch_sensor_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for GeoViewpoint nodes.
     */
    class OPENVRML_LOCAL geo_viewpoint_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit geo_viewpoint_metatype(openvrml::browser & browser);
        virtual ~geo_viewpoint_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };
}

void register_geospatial_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    b.add_node_metatype(geo_coordinate_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_coordinate_metatype(b)));
    b.add_node_metatype(geo_elevation_grid_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_elevation_grid_metatype(b)));
    b.add_node_metatype(geo_location_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_location_metatype(b)));
    b.add_node_metatype(geo_lod_metatype::id,
                        shared_ptr<node_metatype>(new geo_lod_metatype(b)));
    b.add_node_metatype(geo_metadata_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_metadata_metatype(b)));
    b.add_node_metatype(geo_origin_metatype::id,
                        shared_ptr<node_metatype>(new geo_origin_metatype(b)));
    b.add_node_metatype(geo_position_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_position_interpolator_metatype(b)));
    b.add_node_metatype(geo_touch_sensor_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_touch_sensor_metatype(b)));
    b.add_node_metatype(geo_viewpoint_metatype::id,
                        shared_ptr<node_metatype>(
                            new geo_viewpoint_metatype(b)));
}

namespace {
    using namespace openvrml_;

    class OPENVRML_LOCAL geo_coordinate_node :
        public abstract_node<geo_coordinate_node> {

        friend class geo_coordinate_metatype;

        exposedfield<mfvec3d> point_;
        sfnode geo_origin_;
        mfstring geo_system_;

    public:
        geo_coordinate_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_coordinate_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL geo_elevation_grid_node :
        public abstract_node<geo_elevation_grid_node>,
        public geometry_node,
        public child_node {

        friend class geo_elevation_grid_metatype;

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

        virtual bool modified() const;
        virtual const color_node * color() const OPENVRML_NOTHROW;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    class OPENVRML_LOCAL geo_location_node :
        public abstract_node<geo_location_node>,
        public child_node {

        friend class geo_location_metatype;

        class add_children_listener : public event_listener_base<self_t>,
                                      public mfnode_listener {
        public:
            explicit add_children_listener(self_t & node);
            virtual ~add_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_children_listener : public event_listener_base<self_t>,
                                         public mfnode_listener {
        public:
            explicit remove_children_listener(self_t & node);
            virtual ~remove_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        add_children_listener add_children_listener_;
        remove_children_listener remove_children_listener_;
        exposedfield<mfnode> children_;
        exposedfield<sfvec3d> geo_coords_;
        sfnode geo_origin_;
        mfstring geo_system_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        geo_location_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_location_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL geo_lod_node : public abstract_node<geo_lod_node>,
                                        public child_node {
        friend class geo_lod_metatype;

        class add_children_listener :
                public event_listener_base<self_t>,
                public mfnode_listener
        {
        public:
            explicit add_children_listener(self_t & node);
            virtual ~add_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_children_listener : public event_listener_base<self_t>,
                                         public mfnode_listener {
        public:
            explicit remove_children_listener(self_t & node);
            virtual ~remove_children_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        add_children_listener add_children_listener_;
        remove_children_listener remove_children_listener_;
        mfnode children_;
        mfnode_emitter children_emitter_;
        sfvec3d center_;
        mfstring child1url_;
        mfstring child2url_;
        mfstring child3url_;
        mfstring child4url_;
        sfnode geo_origin_;
        mfstring geo_system_;
        sffloat range_;
        mfstring root_url_;
        mfnode root_node_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        geo_lod_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_lod_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL geo_metadata_node :
        public abstract_node<geo_metadata_node>,
        public child_node {

        friend class geo_metadata_metatype;

        exposedfield<mfnode> data_;
        exposedfield<mfstring> summary_;
        exposedfield<mfstring> url_;

    public:
        geo_metadata_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_metadata_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL geo_origin_node :
        public abstract_node<geo_origin_node> {

        friend class geo_origin_metatype;

        exposedfield<sfvec3d> geo_coords_;
        exposedfield<mfstring> geo_system_;
        sfbool rotate_yup_;

    public:
        geo_origin_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_origin_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL geo_position_interpolator_node :
        public abstract_node<geo_position_interpolator_node>,
        public child_node {

        friend class geo_position_interpolator_metatype;

        class set_fraction_listener : public event_listener_base<self_t>,
                                      public sffloat_listener {
        public:
            explicit set_fraction_listener(self_t & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sffloat & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_fraction_listener set_fraction_listener_;
        exposedfield<mffloat> key_;
        exposedfield<mfvec3d> key_value_;
        sfvec3d geovalue_changed_;
        sfvec3d_emitter geovalue_changed_emitter_;
        sfvec3f value_changed_;
        sfvec3f_emitter value_changed_emitter_;
        sfnode geo_origin_;
        mfstring geo_system_;

    public:
        geo_position_interpolator_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_position_interpolator_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL geo_touch_sensor_node :
        public abstract_node<geo_touch_sensor_node>,
        public child_node {

        friend class geo_touch_sensor_metatype;

        exposedfield<sfbool> enabled_;
        sfvec3f hit_normal_changed_;
        sfvec3f_emitter hit_normal_changed_emitter_;
        sfvec3f hit_point_changed_;
        sfvec3f_emitter hit_point_changed_emitter_;
        sfvec2f hit_tex_coord_changed_;
        sfvec2f_emitter hit_tex_coord_changed_emitter_;
        sfvec3d hit_geo_coord_changed_;
        sfvec3d_emitter hit_geo_coord_changed_emitter_;
        sfbool is_active_;
        sfbool_emitter is_active_emitter_;
        sfbool is_over_;
        sfbool_emitter is_over_emitter_;
        sftime touch_time_;
        sftime_emitter touch_time_emitter_;
        sfnode geo_origin_;
        mfstring geo_system_;

    public:
        geo_touch_sensor_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_touch_sensor_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL geo_viewpoint_node :
        public abstract_node<geo_viewpoint_node>,
        public child_node {

        friend class geo_viewpoint_metatype;

        class set_bind_listener : public event_listener_base<self_t>,
                                  public sfbool_listener {
        public:
            explicit set_bind_listener(self_t & node);
            virtual ~set_bind_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfbool & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_orientation_listener : public event_listener_base<self_t>,
                                         public sfrotation_listener {
        public:
            explicit set_orientation_listener(self_t & node);
            virtual ~set_orientation_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfrotation & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_position_listener : public event_listener_base<self_t>,
                                      public sfvec3d_listener {
        public:
            explicit set_position_listener(self_t & node);
            virtual ~set_position_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfvec3d & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_bind_listener set_bind_listener_;
        set_orientation_listener set_orientation_listener_;
        set_position_listener set_position_listener_;
        exposedfield<sfstring> description_;
        exposedfield<sffloat> field_of_view_;
        exposedfield<sfbool> headlight_;
        exposedfield<sfbool> jump_;
        exposedfield<mfstring> nav_type_;
        sftime bind_time_;
        sftime_emitter bind_time_emitter_;
        sfbool is_bound_;
        sfbool_emitter is_bound_emitter_;
        sfnode geo_origin_;
        mfstring geo_system_;
        sfrotation orientation_;
        sfvec3d position_;
        sffloat speed_factor_;

    public:
        geo_viewpoint_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~geo_viewpoint_node() OPENVRML_NOTHROW;
    };


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const geo_coordinate_metatype::id =
        "urn:X-openvrml:node:GeoCoordinate";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c geo_coordinate_metatype.
     */
    geo_coordinate_metatype::geo_coordinate_metatype(openvrml::browser & browser):
        node_metatype(geo_coordinate_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    geo_coordinate_metatype::~geo_coordinate_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating GeoCoordinate nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by geo_coordinate_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    geo_coordinate_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 4> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3d_id,
                           "point"),
            node_interface(node_interface::field_id,
                           field_value::sfnode_id,
                           "geoOrigin"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "geoSystem")
        };
        typedef node_type_impl<geo_coordinate_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_coordinate_node>::exposedfield<sfnode> >(
                            &geo_coordinate_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_coordinate_node>::exposedfield<sfnode> >(
                            &geo_coordinate_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_coordinate_node>::exposedfield<sfnode> >(
                            &geo_coordinate_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_coordinate_node>::exposedfield<mfvec3d> >(
                            &geo_coordinate_node::point_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_coordinate_node>::exposedfield<mfvec3d> >(
                            &geo_coordinate_node::point_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_coordinate_node>::exposedfield<mfvec3d> >(
                            &geo_coordinate_node::point_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfnode>(
                            &geo_coordinate_node::geo_origin_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_coordinate_node::geo_system_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const geo_elevation_grid_metatype::id =
        "urn:X-openvrml:node:GeoElevationGrid";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c geo_elevation_grid_metatype.
     */
    geo_elevation_grid_metatype::
    geo_elevation_grid_metatype(openvrml::browser & browser):
        node_metatype(geo_elevation_grid_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    geo_elevation_grid_metatype::~geo_elevation_grid_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating GeoElevationGrid nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by geo_elevation_grid_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    geo_elevation_grid_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 19> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::mfdouble_id,
                           "set_height"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "normal"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "texCoord"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "yScale"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "ccw"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "colorPerVertex"),
            node_interface(node_interface::field_id,
                           field_value::sfdouble_id,
                           "creaseAngle"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3d_id,
                           "geoGridOrigin"),
            node_interface(node_interface::field_id,
                           field_value::sfnode_id,
                           "geoOrigin"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "geoSystem"),
            node_interface(node_interface::field_id,
                           field_value::mfdouble_id,
                           "height"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "normalPerVertex"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "xDimension"),
            node_interface(node_interface::field_id,
                           field_value::sfdouble_id,
                           "xSpacing"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "zDimension"),
            node_interface(node_interface::field_id,
                           field_value::sfdouble_id,
                           "zSpacing")
        };
        typedef node_type_impl<geo_elevation_grid_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        geo_elevation_grid_node::set_height_listener>(
                            &geo_elevation_grid_node::set_height_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::color_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::normal_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::normal_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::normal_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sfnode> >(
                            &geo_elevation_grid_node::tex_coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sffloat> >(
                            &geo_elevation_grid_node::y_scale_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sffloat> >(
                            &geo_elevation_grid_node::y_scale_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_elevation_grid_node>::exposedfield<sffloat> >(
                            &geo_elevation_grid_node::y_scale_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &geo_elevation_grid_node::ccw_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &geo_elevation_grid_node::color_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfdouble>(
                            &geo_elevation_grid_node::crease_angle_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3d>(
                            &geo_elevation_grid_node::geo_grid_origin_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfnode>(
                            &geo_elevation_grid_node::geo_origin_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_elevation_grid_node::geo_system_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfdouble>(
                            &geo_elevation_grid_node::height_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &geo_elevation_grid_node::normal_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &geo_elevation_grid_node::solid_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &geo_elevation_grid_node::x_dimension_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfdouble>(
                            &geo_elevation_grid_node::x_spacing_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &geo_elevation_grid_node::z_dimension_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfdouble>(
                            &geo_elevation_grid_node::z_spacing_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const geo_location_metatype::id =
        "urn:X-openvrml:node:GeoLocation";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c geo_location_metatype.
     */
    geo_location_metatype::geo_location_metatype(openvrml::browser & browser):
        node_metatype(geo_location_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    geo_location_metatype::~geo_location_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating GeoLocation nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by geo_location_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    geo_location_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 9> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3d_id,
                           "geoCoords"),
            node_interface(node_interface::field_id,
                           field_value::sfnode_id,
                           "geoOrigin"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "geoSystem"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        typedef node_type_impl<geo_location_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_location_node>::exposedfield<sfnode> >(
                            &geo_location_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_location_node>::exposedfield<sfnode> >(
                            &geo_location_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_location_node>::exposedfield<sfnode> >(
                            &geo_location_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        geo_location_node::add_children_listener>(
                            &geo_location_node::add_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        geo_location_node::remove_children_listener>(
                            &geo_location_node::remove_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_location_node>::exposedfield<mfnode> >(
                            &geo_location_node::children_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_location_node>::exposedfield<mfnode> >(
                            &geo_location_node::children_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_location_node>::exposedfield<mfnode> >(
                            &geo_location_node::children_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_location_node>::exposedfield<sfvec3d> >(
                            &geo_location_node::geo_coords_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_location_node>::exposedfield<sfvec3d> >(
                            &geo_location_node::geo_coords_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_location_node>::exposedfield<sfvec3d> >(
                            &geo_location_node::geo_coords_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfnode>(
                            &geo_location_node::geo_origin_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_location_node::geo_system_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &geo_location_node::bbox_center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &geo_location_node::bbox_size_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const geo_lod_metatype::id =
        "urn:X-openvrml:node:GeoLOD";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c geo_lod_metatype.
     */
    geo_lod_metatype::geo_lod_metatype(openvrml::browser & browser):
        node_metatype(geo_lod_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    geo_lod_metatype::~geo_lod_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating GeoLOD nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by geo_lod_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    geo_lod_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 16> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::eventout_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3d_id,
                           "center"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "child1Url"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "child2Url"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "child3Url"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "child4Url"),
            node_interface(node_interface::field_id,
                           field_value::sfnode_id,
                           "geoOrigin"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "geoSystem"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "range"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "rootUrl"),
            node_interface(node_interface::field_id,
                           field_value::mfnode_id,
                           "rootNode"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        typedef node_type_impl<geo_lod_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_lod_node>::exposedfield<sfnode> >(
                            &geo_lod_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_lod_node>::exposedfield<sfnode> >(
                            &geo_lod_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_lod_node>::exposedfield<sfnode> >(
                            &geo_lod_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        geo_lod_node::add_children_listener>(
                            &geo_lod_node::add_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        geo_lod_node::remove_children_listener>(
                            &geo_lod_node::remove_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_lod_node::mfnode_emitter>(
                            &geo_lod_node::children_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3d>(
                            &geo_lod_node::center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_lod_node::child1url_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_lod_node::child2url_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_lod_node::child3url_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_lod_node::child4url_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfnode>(
                            &geo_lod_node::geo_origin_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_lod_node::geo_system_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sffloat>(
                            &geo_lod_node::range_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_lod_node::root_url_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfnode>(
                            &geo_lod_node::root_node_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &geo_lod_node::bbox_center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &geo_lod_node::bbox_size_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const geo_metadata_metatype::id =
        "urn:X-openvrml:node:GeoMetadata";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c geo_metadata_metatype.
     */
    geo_metadata_metatype::geo_metadata_metatype(openvrml::browser & browser):
        node_metatype(geo_metadata_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    geo_metadata_metatype::~geo_metadata_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating GeoMetadata nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by geo_metadata_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    geo_metadata_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 4> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "data"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "summary"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "url")
        };
        typedef node_type_impl<geo_metadata_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<sfnode> >(
                            &geo_metadata_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<sfnode> >(
                            &geo_metadata_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<sfnode> >(
                            &geo_metadata_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<mfnode> >(
                            &geo_metadata_node::data_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<mfnode> >(
                            &geo_metadata_node::data_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<mfnode> >(
                            &geo_metadata_node::data_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<mfstring> >(
                            &geo_metadata_node::summary_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<mfstring> >(
                            &geo_metadata_node::summary_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<mfstring> >(
                            &geo_metadata_node::summary_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<mfstring> >(
                            &geo_metadata_node::url_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<mfstring> >(
                            &geo_metadata_node::url_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_metadata_node>::exposedfield<mfstring> >(
                            &geo_metadata_node::url_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const geo_origin_metatype::id =
        "urn:X-openvrml:node:GeoOrigin";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c geo_origin_metatype.
     */
    geo_origin_metatype::geo_origin_metatype(openvrml::browser & browser):
        node_metatype(geo_origin_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    geo_origin_metatype::~geo_origin_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating GeoOrigin nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by geo_origin_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    geo_origin_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 4> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3d_id,
                           "geoCoords"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "geoSystem"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "rotateYUp")
        };
        typedef node_type_impl<geo_origin_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_origin_node>::exposedfield<sfnode> >(
                            &geo_origin_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_origin_node>::exposedfield<sfnode> >(
                            &geo_origin_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_origin_node>::exposedfield<sfnode> >(
                            &geo_origin_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_origin_node>::exposedfield<sfvec3d> >(
                            &geo_origin_node::geo_coords_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_origin_node>::exposedfield<sfvec3d> >(
                            &geo_origin_node::geo_coords_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_origin_node>::exposedfield<sfvec3d> >(
                            &geo_origin_node::geo_coords_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_origin_node>::exposedfield<mfstring> >(
                            &geo_origin_node::geo_system_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_origin_node>::exposedfield<mfstring> >(
                            &geo_origin_node::geo_system_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_origin_node>::exposedfield<mfstring> >(
                            &geo_origin_node::geo_system_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &geo_origin_node::rotate_yup_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const geo_position_interpolator_metatype::id =
        "urn:X-openvrml:node:GeoPositionInterpolator";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c geo_position_interpolator_metatype.
     */
    geo_position_interpolator_metatype::
    geo_position_interpolator_metatype(openvrml::browser & browser):
        node_metatype(geo_position_interpolator_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    geo_position_interpolator_metatype::~geo_position_interpolator_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating GeoPositionInterpolator nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by geo_position_interpolator_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    geo_position_interpolator_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 8> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "key"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3d_id,
                           "keyValue"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3d_id,
                           "geovalue_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "value_changed"),
            node_interface(node_interface::field_id,
                           field_value::sfnode_id,
                           "geoOrigin"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "geoSystem")
        };
        typedef node_type_impl<geo_position_interpolator_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_position_interpolator_node>::exposedfield<sfnode> >(
                            &geo_position_interpolator_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_position_interpolator_node>::exposedfield<sfnode> >(
                            &geo_position_interpolator_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_position_interpolator_node>::exposedfield<sfnode> >(
                            &geo_position_interpolator_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        geo_position_interpolator_node::set_fraction_listener>(
                            &geo_position_interpolator_node::set_fraction_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_position_interpolator_node>::exposedfield<mffloat> >(
                            &geo_position_interpolator_node::key_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_position_interpolator_node>::exposedfield<mffloat> >(
                            &geo_position_interpolator_node::key_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_position_interpolator_node>::exposedfield<mffloat> >(
                            &geo_position_interpolator_node::key_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_position_interpolator_node>::exposedfield<mfvec3d> >(
                            &geo_position_interpolator_node::key_value_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_position_interpolator_node>::exposedfield<mfvec3d> >(
                            &geo_position_interpolator_node::key_value_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_position_interpolator_node>::exposedfield<mfvec3d> >(
                            &geo_position_interpolator_node::key_value_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_position_interpolator_node::sfvec3d_emitter>(
                            &geo_position_interpolator_node::geovalue_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_position_interpolator_node::sfvec3f_emitter>(
                            &geo_position_interpolator_node::value_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfnode>(
                            &geo_position_interpolator_node::geo_origin_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_position_interpolator_node::geo_system_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @brief @c node_metatype identifier.
     */
    const char * const geo_touch_sensor_metatype::id =
        "urn:X-openvrml:node:GeoTouchSensor";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c geo_touch_sensor_metatype.
     */
    geo_touch_sensor_metatype::
    geo_touch_sensor_metatype(openvrml::browser & browser):
        node_metatype(geo_touch_sensor_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    geo_touch_sensor_metatype::~geo_touch_sensor_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating GeoTouchSensor nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by geo_touch_sensor_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    geo_touch_sensor_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 11> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "enabled"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "hitNormal_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "hitPoint_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec2f_id,
                           "hitTexCoord_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3d_id,
                           "hitGeoCoord_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isOver"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "touchTime"),
            node_interface(node_interface::field_id,
                           field_value::sfnode_id,
                           "geoOrigin"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "geoSystem")
        };
        typedef node_type_impl<geo_touch_sensor_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_touch_sensor_node>::exposedfield<sfnode> >(
                            &geo_touch_sensor_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_touch_sensor_node>::exposedfield<sfnode> >(
                            &geo_touch_sensor_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_touch_sensor_node>::exposedfield<sfnode> >(
                            &geo_touch_sensor_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_touch_sensor_node>::exposedfield<sfbool> >(
                            &geo_touch_sensor_node::enabled_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_touch_sensor_node>::exposedfield<sfbool> >(
                            &geo_touch_sensor_node::enabled_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_touch_sensor_node>::exposedfield<sfbool> >(
                            &geo_touch_sensor_node::enabled_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_touch_sensor_node::sfvec3f_emitter>(
                            &geo_touch_sensor_node::hit_normal_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_touch_sensor_node::sfvec3f_emitter>(
                            &geo_touch_sensor_node::hit_point_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_touch_sensor_node::sfvec2f_emitter>(
                            &geo_touch_sensor_node::hit_tex_coord_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_touch_sensor_node::sfvec3d_emitter>(
                            &geo_touch_sensor_node::hit_geo_coord_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_touch_sensor_node::sfbool_emitter>(
                            &geo_touch_sensor_node::is_active_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_touch_sensor_node::sfbool_emitter>(
                            &geo_touch_sensor_node::is_over_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_touch_sensor_node::sftime_emitter>(
                            &geo_touch_sensor_node::touch_time_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfnode>(
                            &geo_touch_sensor_node::geo_origin_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_touch_sensor_node::geo_system_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @brief @c node_metatype identifier.
     */
    const char * const geo_viewpoint_metatype::id =
        "urn:X-openvrml:node:GeoViewpoint";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c geo_viewpoint_metatype.
     */
    geo_viewpoint_metatype::geo_viewpoint_metatype(openvrml::browser & browser):
        node_metatype(geo_viewpoint_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    geo_viewpoint_metatype::~geo_viewpoint_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating GeoViewpoint nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by geo_viewpoint_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    geo_viewpoint_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 16> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "set_bind"),
            node_interface(node_interface::eventin_id,
                           field_value::sfrotation_id,
                           "set_orientation"),
            node_interface(node_interface::eventin_id,
                           field_value::sfvec3d_id,
                           "set_position"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "description"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "fieldOfView"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "headlight"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "jump"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "navType"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "bindTime"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isBound"),
            node_interface(node_interface::field_id,
                           field_value::sfnode_id,
                           "geoOrigin"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "geoSystem"),
            node_interface(node_interface::field_id,
                           field_value::sfrotation_id,
                           "orientation"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3d_id,
                           "position"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "speedFactor")
        };
        typedef node_type_impl<geo_viewpoint_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfnode> >(
                            &geo_viewpoint_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfnode> >(
                            &geo_viewpoint_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfnode> >(
                            &geo_viewpoint_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        geo_viewpoint_node::set_bind_listener>(
                            &geo_viewpoint_node::set_bind_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        geo_viewpoint_node::set_orientation_listener>(
                            &geo_viewpoint_node::set_orientation_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        geo_viewpoint_node::set_position_listener>(
                            &geo_viewpoint_node::set_position_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfstring> >(
                            &geo_viewpoint_node::description_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfstring> >(
                            &geo_viewpoint_node::description_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfstring> >(
                            &geo_viewpoint_node::description_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sffloat> >(
                            &geo_viewpoint_node::field_of_view_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sffloat> >(
                            &geo_viewpoint_node::field_of_view_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sffloat> >(
                            &geo_viewpoint_node::field_of_view_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfbool> >(
                            &geo_viewpoint_node::headlight_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfbool> >(
                            &geo_viewpoint_node::headlight_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfbool> >(
                            &geo_viewpoint_node::headlight_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfbool> >(
                            &geo_viewpoint_node::jump_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfbool> >(
                            &geo_viewpoint_node::jump_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<sfbool> >(
                            &geo_viewpoint_node::jump_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<mfstring> >(
                            &geo_viewpoint_node::nav_type_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<mfstring> >(
                            &geo_viewpoint_node::nav_type_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<geo_viewpoint_node>::exposedfield<mfstring> >(
                            &geo_viewpoint_node::nav_type_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_viewpoint_node::sftime_emitter>(
                            &geo_viewpoint_node::bind_time_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        geo_viewpoint_node::sfbool_emitter>(
                            &geo_viewpoint_node::is_bound_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfnode>(
                            &geo_viewpoint_node::geo_origin_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfstring>(
                            &geo_viewpoint_node::geo_system_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfrotation>(
                            &geo_viewpoint_node::orientation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3d>(
                            &geo_viewpoint_node::position_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sffloat>(
                            &geo_viewpoint_node::speed_factor_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @class geo_coordinate_node
     *
     * @brief Represents GeoCoordinate node instances.
     */

    /**
     * @var geo_coordinate_node::geo_coordinate_metatype
     *
     * @brief Class object for GeoCoordinate nodes.
     */

    /**
     * @var geo_coordinate_node::point_
     *
     * @brief point exposedField
     */

    /**
     * @var geo_coordinate_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var geo_coordinate_node::geo_system_
     *
     * @brief geo_system field
     */


    const string default_geo_system_[] = { "GD", "WE" };
    const vector<string> default_geo_system(default_geo_system_,
                                            default_geo_system_ + 1);

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    geo_coordinate_node::
    geo_coordinate_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        point_(*this),
        geo_system_(default_geo_system)
    {}

    /**
     * @brief Destroy.
     */
    geo_coordinate_node::~geo_coordinate_node() OPENVRML_NOTHROW
    {}


    /**
     * @class geo_elevation_grid_node
     *
     * @brief Represents GeoElevationGrid node instances.
     */

    /**
     * @var geo_elevation_grid_node::geo_elevation_grid_metatype
     *
     * @brief Class object for GeoElevationGrid nodes.
     */

    /**
     * @var geo_elevation_grid_node::set_height_
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
    do_process_event(const mfdouble & /* fraction */, const double /* timestamp */)
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
    openvrml::viewer::object_t
    geo_elevation_grid_node::
    do_render_geometry(openvrml::viewer & /* viewer */,
                       const rendering_context /* context */)
    {
        //TODO: Implement this!
        return 0;
    }


    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool geo_elevation_grid_node::modified() const
    {
        return this->node::modified();
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
        geo_system_(default_geo_system),
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


    /**
     * @class geo_location_node
     *
     * @brief Represents GeoLocation node instances.
     */

    /**
     * @var geo_location_node::geo_location_metatype
     *
     * @brief Class object for GeoLocation nodes.
     */

    /**
     * @var geo_location_node::add_children_
     *
     * @brief add_children eventIn
     */

    /**
     * @var geo_location_node::remove_children_
     *
     * @brief remove_children eventIn
     */

    /**
     * @var geo_location_node::children_
     *
     * @brief children exposedField
     */

    /**
     * @var geo_location_node::geo_coords_
     *
     * @brief geo_coords exposedField
     */

    /**
     * @var geo_location_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var geo_location_node::geo_system_
     *
     * @brief geo_system field
     */

    /**
     * @var geo_location_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var geo_location_node::bbox_size_
     *
     * @brief bbox_size field
     */

    geo_location_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    geo_location_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void geo_location_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    geo_location_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    geo_location_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void geo_location_node::remove_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    geo_location_node::
    geo_location_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        children_(*this),
        geo_coords_(*this),
        geo_system_(default_geo_system),
        bbox_size_(vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    geo_location_node::~geo_location_node() OPENVRML_NOTHROW
    {}


    /**
     * @class geo_lod_node
     *
     * @brief Represents GeoLOD node instances.
     */

    /**
     * @var geo_lod_node::geo_lod_metatype
     *
     * @brief Class object for GeoLOD nodes.
     */

    /**
     * @var geo_lod_node::add_children_
     *
     * @brief add_children eventIn
     */

    /**
     * @var geo_lod_node::remove_children_
     *
     * @brief remove_children eventIn
     */

    /**
     * @var geo_lod_node::children_
     *
     * @brief children eventOut
     */

    /**
     * @var geo_lod_node::center_
     *
     * @brief center field
     */

    /**
     * @var geo_lod_node::child1url_
     *
     * @brief child1url field
     */

    /**
     * @var geo_lod_node::child2url_
     *
     * @brief child2url field
     */

    /**
     * @var geo_lod_node::child3url_
     *
     * @brief child3url field
     */

    /**
     * @var geo_lod_node::child4url_
     *
     * @brief child4url field
     */

    /**
     * @var geo_lod_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var geo_lod_node::geo_system_
     *
     * @brief geo_system field
     */

    /**
     * @var geo_lod_node::range_
     *
     * @brief range field
     */

    /**
     * @var geo_lod_node::root_url_
     *
     * @brief root_url field
     */

    /**
     * @var geo_lod_node::root_node_
     *
     * @brief root_node field
     */

    /**
     * @var geo_lod_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var geo_lod_node::bbox_size_
     *
     * @brief bbox_size field
     */

    geo_lod_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    geo_lod_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void geo_lod_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    geo_lod_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    geo_lod_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void geo_lod_node::remove_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    geo_lod_node::
    geo_lod_node(const node_type & type,
                 const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        children_emitter_(*this, this->children_),
        geo_system_(default_geo_system),
        range_(10),
        bbox_size_(vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    geo_lod_node::~geo_lod_node() OPENVRML_NOTHROW
    {}


    /**
     * @class geo_metadata_node
     *
     * @brief Represents GeoMetadata node instances.
     */

    /**
     * @var geo_metadata_node::geo_metadata_metatype
     *
     * @brief Class object for GeoMetadata nodes.
     */

    /**
     * @var geo_metadata_node::data_
     *
     * @brief data exposedField
     */

    /**
     * @var geo_metadata_node::summary_
     *
     * @brief summary exposedField
     */

    /**
     * @var geo_metadata_node::url_
     *
     * @brief url exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    geo_metadata_node::
    geo_metadata_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        data_(*this),
        summary_(*this),
        url_(*this)
    {}

    /**
     * @brief Destroy.
     */
    geo_metadata_node::~geo_metadata_node() OPENVRML_NOTHROW
    {}


    /**
     * @class geo_origin_node
     *
     * @brief Represents GeoOrigin node instances.
     */

    /**
     * @var geo_origin_node::geo_origin_metatype
     *
     * @brief Class object for GeoOrigin nodes.
     */

    /**
     * @var geo_origin_node::geo_coords_
     *
     * @brief geo_coords exposedField
     */

    /**
     * @var geo_origin_node::geo_system_
     *
     * @brief geo_system exposedField
     */

    /**
     * @var geo_origin_node::rotate_yup_
     *
     * @brief rotate_yup field
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    geo_origin_node::
    geo_origin_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        geo_coords_(*this),
        geo_system_(*this, default_geo_system)
    {}

    /**
     * @brief Destroy.
     */
    geo_origin_node::~geo_origin_node() OPENVRML_NOTHROW
    {}


    /**
     * @class geo_position_interpolator_node
     *
     * @brief Represents GeoPositionInterpolator node instances.
     */

    /**
     * @var geo_position_interpolator_node::geo_position_interpolator_metatype
     *
     * @brief Class object for GeoPositionInterpolator nodes.
     */

    /**
     * @var geo_position_interpolator_node::set_fraction_
     *
     * @brief set_fraction eventIn
     */

    /**
     * @var geo_position_interpolator_node::key_
     *
     * @brief key exposedField
     */

    /**
     * @var geo_position_interpolator_node::key_value_
     *
     * @brief key_value exposedField
     */

    /**
     * @var geo_position_interpolator_node::geovalue_changed_
     *
     * @brief geovalue_changed eventOut
     */

    /**
     * @var geo_position_interpolator_node::value_changed_
     *
     * @brief value_changed eventOut
     */

    /**
     * @var geo_position_interpolator_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var geo_position_interpolator_node::geo_system_
     *
     * @brief geo_system field
     */

    geo_position_interpolator_node::set_fraction_listener::
    set_fraction_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    geo_position_interpolator_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    void geo_position_interpolator_node::set_fraction_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    geo_position_interpolator_node::
    geo_position_interpolator_node(const node_type & type,
                                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_fraction_listener_(*this),
        key_(*this),
        key_value_(*this),
        geovalue_changed_emitter_(*this, this->geovalue_changed_),
        value_changed_emitter_(*this, this->value_changed_),
        geo_system_(default_geo_system)
    {}

    /**
     * @brief Destroy.
     */
    geo_position_interpolator_node::~geo_position_interpolator_node() OPENVRML_NOTHROW
    {}


    /**
     * @class geo_touch_sensor_node
     *
     * @brief Represents GeoTouchSensor node instances.
     */

    /**
     * @var geo_touch_sensor_node::geo_touch_sensor_metatype
     *
     * @brief Class object for GeoTouchSensor nodes.
     */

    /**
     * @var geo_touch_sensor_node::enabled_
     *
     * @brief enabled exposedField
     */

    /**
     * @var geo_touch_sensor_node::hit_normal_changed_
     *
     * @brief hit_normal_changed eventOut
     */

    /**
     * @var geo_touch_sensor_node::hit_point_changed_
     *
     * @brief hit_point_changed eventOut
     */

    /**
     * @var geo_touch_sensor_node::hit_tex_coord_changed_
     *
     * @brief hit_tex_coord_changed eventOut
     */

    /**
     * @var geo_touch_sensor_node::hit_geo_coord_changed_
     *
     * @brief hit_geo_coord_changed eventOut
     */

    /**
     * @var geo_touch_sensor_node::is_active_
     *
     * @brief is_active eventOut
     */

    /**
     * @var geo_touch_sensor_node::is_over_
     *
     * @brief is_over eventOut
     */

    /**
     * @var geo_touch_sensor_node::touch_time_
     *
     * @brief touch_time eventOut
     */

    /**
     * @var geo_touch_sensor_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var geo_touch_sensor_node::geo_system_
     *
     * @brief geo_system field
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    geo_touch_sensor_node::
    geo_touch_sensor_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        enabled_(*this, true),
        hit_normal_changed_emitter_(*this, this->hit_normal_changed_),
        hit_point_changed_emitter_(*this, this->hit_point_changed_),
        hit_tex_coord_changed_emitter_(*this, this->hit_tex_coord_changed_),
        hit_geo_coord_changed_emitter_(*this, this->hit_geo_coord_changed_),
        is_active_emitter_(*this, this->is_active_),
        is_over_emitter_(*this, this->is_over_),
        touch_time_emitter_(*this, this->touch_time_),
        geo_system_(default_geo_system)
    {}

    /**
     * @brief Destroy.
     */
    geo_touch_sensor_node::~geo_touch_sensor_node() OPENVRML_NOTHROW
    {}


    /**
     * @class geo_viewpoint_node
     *
     * @brief Represents GeoViewpoint node instances.
     */

    /**
     * @var geo_viewpoint_node::geo_viewpoint_metatype
     *
     * @brief Class object for GeoViewpoint nodes.
     */

    /**
     * @var geo_viewpoint_node::set_bind_
     *
     * @brief set_bind eventIn
     */

    /**
     * @var geo_viewpoint_node::set_orientation_
     *
     * @brief set_orientation eventIn
     */

    /**
     * @var geo_viewpoint_node::set_position_
     *
     * @brief set_position eventIn
     */

    /**
     * @var geo_viewpoint_node::description_
     *
     * @brief description exposedField
     */

    /**
     * @var geo_viewpoint_node::field_of_view_
     *
     * @brief field_of_view exposedField
     */

    /**
     * @var geo_viewpoint_node::headlight_
     *
     * @brief headlight exposedField
     */

    /**
     * @var geo_viewpoint_node::jump_
     *
     * @brief jump exposedField
     */

    /**
     * @var geo_viewpoint_node::nav_type_
     *
     * @brief navType exposedField
     */

    /**
     * @var geo_viewpoint_node::bind_time_
     *
     * @brief bind_time eventOut
     */

    /**
     * @var geo_viewpoint_node::is_bound_
     *
     * @brief is_bound eventOut
     */

    /**
     * @var geo_viewpoint_node::geo_origin_
     *
     * @brief geo_origin field
     */

    /**
     * @var geo_viewpoint_node::geo_system_
     *
     * @brief geo_system field
     */

    /**
     * @var geo_viewpoint_node::orientation_
     *
     * @brief orientation field
     */

    /**
     * @var geo_viewpoint_node::position_
     *
     * @brief position field
     */

    /**
     * @var geo_viewpoint_node::speed_factor_
     *
     * @brief speed_factor field
     */

    geo_viewpoint_node::set_bind_listener::
    set_bind_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfbool_listener(node)
    {}

    geo_viewpoint_node::set_bind_listener::
    ~set_bind_listener() OPENVRML_NOTHROW
    {}

    void geo_viewpoint_node::set_bind_listener::
    do_process_event(const sfbool & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    geo_viewpoint_node::set_orientation_listener::
    set_orientation_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfrotation_listener(node)
    {}

    geo_viewpoint_node::set_orientation_listener::
    ~set_orientation_listener() OPENVRML_NOTHROW
    {}

    void geo_viewpoint_node::set_orientation_listener::
    do_process_event(const sfrotation & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    geo_viewpoint_node::set_position_listener::
    set_position_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfvec3d_listener(node)
    {}

    geo_viewpoint_node::set_position_listener::
    ~set_position_listener() OPENVRML_NOTHROW
    {}

    void geo_viewpoint_node::set_position_listener::
    do_process_event(const sfvec3d & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    const string default_nav_type_[] = { "EXAMINE", "ANY" };
    const vector<string> default_nav_type(default_nav_type_,
                                          default_nav_type_ + 1);

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    geo_viewpoint_node::
    geo_viewpoint_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_bind_listener_(*this),
        set_orientation_listener_(*this),
        set_position_listener_(*this),
        description_(*this),
        field_of_view_(*this, 0.785398163f),
        headlight_(*this, true),
        jump_(*this, true),
        nav_type_(*this, default_nav_type),
        bind_time_emitter_(*this, this->bind_time_),
        is_bound_emitter_(*this, this->is_bound_),
        geo_system_(default_geo_system),
        position_(vec3d(0,0,100000)),
        speed_factor_(1)
    {}

    /**
     * @brief Destroy.
     */
    geo_viewpoint_node::~geo_viewpoint_node() OPENVRML_NOTHROW
    {}
}
