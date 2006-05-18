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
# include "x3d_nurbs.h"

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Class object for Contour2D nodes.
     */
    class OPENVRML_LOCAL contour2d_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit contour2d_metatype(openvrml::browser & browser);
        virtual ~contour2d_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for ContourPolyline2D nodes.
     */
    class OPENVRML_LOCAL contour_polyline2d_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit contour_polyline2d_metatype(openvrml::browser & browser);
        virtual ~contour_polyline2d_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for CoordinateDouble nodes.
     */
    class OPENVRML_LOCAL coordinate_double_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit coordinate_double_metatype(openvrml::browser & browser);
        virtual ~coordinate_double_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for NurbsCurve nodes.
     */
    class OPENVRML_LOCAL nurbs_curve_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit nurbs_curve_metatype(openvrml::browser & browser);
        virtual ~nurbs_curve_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for NurbsCurve2D nodes.
     */
    class OPENVRML_LOCAL nurbs_curve2d_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit nurbs_curve2d_metatype(openvrml::browser & browser);
        virtual ~nurbs_curve2d_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for NurbsOrientationInterpolator nodes.
     */
    class OPENVRML_LOCAL nurbs_orientation_interpolator_metatype :
        public node_metatype {
    public:
        static const char * const id;

        explicit nurbs_orientation_interpolator_metatype(
            openvrml::browser & browser);
        virtual ~nurbs_orientation_interpolator_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for NurbsPatchSurface nodes.
     */
    class OPENVRML_LOCAL nurbs_patch_surface_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit nurbs_patch_surface_metatype(openvrml::browser & browser);
        virtual ~nurbs_patch_surface_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for NurbsPositionInterpolator nodes.
     */
    class OPENVRML_LOCAL nurbs_position_interpolator_metatype :
        public node_metatype {
    public:
        static const char * const id;

        explicit nurbs_position_interpolator_metatype(
            openvrml::browser & browser);
        virtual ~nurbs_position_interpolator_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for NurbsSet nodes.
     */
    class OPENVRML_LOCAL nurbs_set_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit nurbs_set_metatype(openvrml::browser & browser);
        virtual ~nurbs_set_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for NurbsSurfaceInterpolator nodes.
     */
    class OPENVRML_LOCAL nurbs_surface_interpolator_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit nurbs_surface_interpolator_metatype(openvrml::browser & browser);
        virtual ~nurbs_surface_interpolator_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for NurbsSweptSurface nodes.
     */
    class OPENVRML_LOCAL nurbs_swept_surface_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit nurbs_swept_surface_metatype(openvrml::browser & browser);
        virtual ~nurbs_swept_surface_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for NurbsSwungSurface nodes.
     */
    class OPENVRML_LOCAL nurbs_swung_surface_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit nurbs_swung_surface_metatype(openvrml::browser & browser);
        virtual ~nurbs_swung_surface_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for NurbsTextureCoordinate nodes.
     */
    class OPENVRML_LOCAL nurbs_texture_coordinate_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit nurbs_texture_coordinate_metatype(openvrml::browser & browser);
        virtual ~nurbs_texture_coordinate_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for NurbsTrimmedSurface nodes.
     */
    class OPENVRML_LOCAL nurbs_trimmed_surface_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit nurbs_trimmed_surface_metatype(openvrml::browser & browser);
        virtual ~nurbs_trimmed_surface_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };
}

void register_nurbs_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    b.add_node_metatype(contour2d_metatype::id,
                        shared_ptr<node_metatype>(new contour2d_metatype(b)));
    b.add_node_metatype(contour_polyline2d_metatype::id,
                        shared_ptr<node_metatype>(
                            new contour_polyline2d_metatype(b)));
    b.add_node_metatype(coordinate_double_metatype::id,
                        shared_ptr<node_metatype>(
                            new coordinate_double_metatype(b)));
    b.add_node_metatype(nurbs_curve_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_curve_metatype(b)));
    b.add_node_metatype(nurbs_curve2d_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_curve2d_metatype(b)));
    b.add_node_metatype(nurbs_orientation_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_orientation_interpolator_metatype(b)));
    b.add_node_metatype(nurbs_patch_surface_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_patch_surface_metatype(b)));
    b.add_node_metatype(nurbs_position_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_position_interpolator_metatype(b)));
    b.add_node_metatype(nurbs_set_metatype::id,
                        shared_ptr<node_metatype>(new nurbs_set_metatype(b)));
    b.add_node_metatype(nurbs_surface_interpolator_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_surface_interpolator_metatype(b)));
    b.add_node_metatype(nurbs_swept_surface_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_swept_surface_metatype(b)));
    b.add_node_metatype(nurbs_swung_surface_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_swung_surface_metatype(b)));
    b.add_node_metatype(nurbs_texture_coordinate_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_texture_coordinate_metatype(b)));
    b.add_node_metatype(nurbs_trimmed_surface_metatype::id,
                        shared_ptr<node_metatype>(
                            new nurbs_trimmed_surface_metatype(b)));
}

namespace {
    using namespace openvrml_;

    class OPENVRML_LOCAL contour2d_node :
        public abstract_node<contour2d_node> {

        friend class contour2d_metatype;

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

    public:
        contour2d_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~contour2d_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL contour_polyline2d_node :
        public abstract_node<contour_polyline2d_node> {

        friend class contour_polyline2d_metatype;

        exposedfield<mfvec2f> point_;

    public:
        contour_polyline2d_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~contour_polyline2d_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL coordinate_double_node :
        public abstract_node<coordinate_double_node> {

        friend class coordinate_double_metatype;

        exposedfield<mfvec3d> point_;

    public:
        coordinate_double_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~coordinate_double_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL nurbs_curve_node :
        public abstract_node<nurbs_curve_node>,
        public geometry_node,
        public child_node {

        friend class nurbs_curve_metatype;

        exposedfield<sfnode> control_point_;
        exposedfield<sfint32> tessellation_;
        exposedfield<mfdouble> weight_;
        sfbool closed_;
        mfdouble knot_;
        sfint32 order_;

    public:
        nurbs_curve_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_curve_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    class OPENVRML_LOCAL nurbs_curve2d_node :
        public abstract_node<nurbs_curve2d_node> {

        friend class nurbs_curve2d_metatype;

        exposedfield<mfvec2d> control_point_;
        exposedfield<sfint32> tessellation_;
        exposedfield<mfdouble> weight_;
        sfbool closed_;
        mfdouble knot_;
        sfint32 order_;

    public:
        nurbs_curve2d_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_curve2d_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL nurbs_orientation_interpolator_node :
        public abstract_node<nurbs_orientation_interpolator_node>,
        public child_node {

        friend class nurbs_orientation_interpolator_metatype;

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
        exposedfield<sfnode> control_points_;
        exposedfield<mfdouble> knot_;
        exposedfield<sfint32> order_;
        exposedfield<mfdouble> weight_;
        sfrotation value_changed_;
        sfrotation_emitter value_changed_emitter_;

    public:
        nurbs_orientation_interpolator_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);

        virtual ~nurbs_orientation_interpolator_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL nurbs_patch_surface_node :
        public abstract_node<nurbs_patch_surface_node>,
        public geometry_node,
        public child_node {

        friend class nurbs_patch_surface_metatype;

        exposedfield<sfnode> control_point_;
        exposedfield<sfnode> tex_coord_;
        exposedfield<sfint32> u_tessellation_;
        exposedfield<sfint32> v_tessellation_;
        exposedfield<mfdouble> weight_;
        sfbool solid_;
        sfbool u_closed_;
        sfint32 u_dimension_;
        mfdouble u_knot_;
        sfint32 u_order_;
        sfbool v_closed_;
        sfint32 v_dimension_;
        mfdouble v_knot_;
        sfint32 v_order_;

    public:
        nurbs_patch_surface_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_patch_surface_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    class OPENVRML_LOCAL nurbs_position_interpolator_node :
        public abstract_node<nurbs_position_interpolator_node>,
        public child_node {

        friend class nurbs_position_interpolator_metatype;

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
        exposedfield<sfnode> control_points_;
        exposedfield<mfdouble> knot_;
        exposedfield<sfint32> order_;
        exposedfield<mfdouble> weight_;
        sfvec3f value_changed_;
        sfvec3f_emitter value_changed_emitter_;

    public:
        nurbs_position_interpolator_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);

        virtual ~nurbs_position_interpolator_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL nurbs_set_node :
        public abstract_node<nurbs_set_node>,
        public child_node {

        friend class nurbs_set_metatype;

        class add_geometry_listener : public event_listener_base<self_t>,
                                      public mfnode_listener {
        public:
            explicit add_geometry_listener(self_t & node);
            virtual ~add_geometry_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_geometry_listener :
                public event_listener_base<self_t>,
                public mfnode_listener
        {
        public:
            explicit remove_geometry_listener(self_t & node);
            virtual ~remove_geometry_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        add_geometry_listener add_geometry_listener_;
        remove_geometry_listener remove_geometry_listener_;
        exposedfield<mfnode> geometry_;
        exposedfield<sffloat> tessellation_scale_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

    public:
        nurbs_set_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_set_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL nurbs_surface_interpolator_node :
        public abstract_node<nurbs_surface_interpolator_node>,
        public child_node {

        friend class nurbs_surface_interpolator_metatype;

        class set_fraction_listener : public event_listener_base<self_t>,
                                      public sfvec2f_listener {
        public:
            explicit set_fraction_listener(self_t & node);
            virtual ~set_fraction_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const sfvec2f & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_fraction_listener set_fraction_listener_;
        exposedfield<sfnode> control_points_;
        exposedfield<mfdouble> weight_;
        sfvec3f position_changed_;
        sfvec3f_emitter position_changed_emitter_;
        sfvec3f normal_changed_;
        sfvec3f_emitter normal_changed_emitter_;
        sfint32 u_dimension_;
        mfdouble u_knot_;
        sfint32 u_order_;
        sfint32 v_dimension_;
        mfdouble v_knot_;
        sfint32 v_order_;

    public:
        nurbs_surface_interpolator_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_surface_interpolator_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL nurbs_swept_surface_node :
        public abstract_node<nurbs_swept_surface_node>,
        public geometry_node,
        public child_node {

        friend class nurbs_swept_surface_metatype;

        exposedfield<sfnode> cross_section_curve_;
        exposedfield<sfnode> trajectory_curve_;
        sfbool ccw_;
        sfbool solid_;

    public:
        nurbs_swept_surface_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_swept_surface_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    class OPENVRML_LOCAL nurbs_swung_surface_node :
        public abstract_node<nurbs_swung_surface_node>,
        public geometry_node,
        public child_node {

        friend class nurbs_swung_surface_metatype;

        exposedfield<sfnode> componentcurve_;
        exposedfield<sfnode> trajectory_curve_;
        sfbool ccw_;
        sfbool solid_;

    public:
        nurbs_swung_surface_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_swung_surface_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    class OPENVRML_LOCAL nurbs_texture_coordinate_node :
        public abstract_node<nurbs_texture_coordinate_node> {

        friend class nurbs_texture_coordinate_metatype;

        exposedfield<mfvec2f> control_point_;
        exposedfield<mffloat> weight_;
        sfint32 u_dimension_;
        mfdouble u_knot_;
        sfint32 u_order_;
        sfint32 v_dimension_;
        mfdouble v_knot_;
        sfint32 v_order_;

    public:
        nurbs_texture_coordinate_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_texture_coordinate_node() OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL nurbs_trimmed_surface_node :
        public abstract_node<nurbs_trimmed_surface_node>,
        public geometry_node,
        public child_node {

        friend class nurbs_trimmed_surface_metatype;

        class add_trimming_contour_listener :
            public event_listener_base<self_t>,
            public mfnode_listener {
        public:
            explicit add_trimming_contour_listener(self_t & node);
            virtual ~add_trimming_contour_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class remove_trimming_contour_listener :
            public event_listener_base<self_t>,
            public mfnode_listener {
        public:
            explicit remove_trimming_contour_listener(self_t & node);
            virtual ~remove_trimming_contour_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfnode & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        add_trimming_contour_listener add_trimming_contour_listener_;
        remove_trimming_contour_listener remove_trimming_contour_listener_;
        exposedfield<sfnode> control_point_;
        exposedfield<sfnode> tex_coord_;
        exposedfield<mfnode> trimming_contour_;
        exposedfield<sfint32> u_tessellation_;
        exposedfield<sfint32> v_tessellation_;
        exposedfield<mfdouble> weight_;
        sfbool solid_;
        sfbool u_closed_;
        sfint32 u_dimension_;
        mfdouble u_knot_;
        sfint32 u_order_;
        sfbool v_closed_;
        sfint32 v_dimension_;
        mfdouble v_knot_;
        sfint32 v_order_;

    public:
        nurbs_trimmed_surface_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~nurbs_trimmed_surface_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    /**
     * @brief @c node_metatype identifier.
     */
    const char * const contour2d_metatype::id = "urn:X-openvrml:node:Contour2D";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c contour2d_metatype.
     */
    contour2d_metatype::contour2d_metatype(openvrml::browser & browser):
        node_metatype(contour2d_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    contour2d_metatype::~contour2d_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating Contour2D nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by contour2d_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    contour2d_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 4> supported_interfaces_t;
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
                           "children")
        };
        typedef node_type_impl<contour2d_node> node_type_t;

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
                        abstract_node<contour2d_node>::exposedfield<sfnode> >(
                            &contour2d_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<contour2d_node>::exposedfield<sfnode> >(
                            &contour2d_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<contour2d_node>::exposedfield<sfnode> >(
                            &contour2d_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        contour2d_node::add_children_listener>(
                            &contour2d_node::add_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        contour2d_node::remove_children_listener>(
                            &contour2d_node::remove_children_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<contour2d_node>::exposedfield<mfnode> >(
                            &contour2d_node::children_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<contour2d_node>::exposedfield<mfnode> >(
                            &contour2d_node::children_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<contour2d_node>::exposedfield<mfnode> >(
                            &contour2d_node::children_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const contour_polyline2d_metatype::id =
        "urn:X-openvrml:node:ContourPolyline2D";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c contour_polyline2d_metatype.
     */
    contour_polyline2d_metatype::
    contour_polyline2d_metatype(openvrml::browser & browser):
        node_metatype(contour_polyline2d_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    contour_polyline2d_metatype::~contour_polyline2d_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating ContourPolyline2D nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by contour_polyline2d_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    contour_polyline2d_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 3> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            //this is wrong in the spec, so we must support both aliases
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec2f_id,
                           "point"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec2f_id,
                           "controlPoint")
        };
        typedef node_type_impl<contour_polyline2d_node> node_type_t;

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
                        abstract_node<contour_polyline2d_node>::exposedfield<sfnode> >(
                            &contour_polyline2d_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<contour_polyline2d_node>::exposedfield<sfnode> >(
                            &contour_polyline2d_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<contour_polyline2d_node>::exposedfield<sfnode> >(
                            &contour_polyline2d_node::metadata)));
            } else if (*interface == *++supported_interface ||
                       *interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<contour_polyline2d_node>::exposedfield<mfvec2f> >(
                            &contour_polyline2d_node::point_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<contour_polyline2d_node>::exposedfield<mfvec2f> >(
                            &contour_polyline2d_node::point_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<contour_polyline2d_node>::exposedfield<mfvec2f> >(
                            &contour_polyline2d_node::point_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const coordinate_double_metatype::id =
        "urn:X-openvrml:node:CoordinateDouble";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c coordinate_double_metatype.
     */
    coordinate_double_metatype::
    coordinate_double_metatype(openvrml::browser & browser):
        node_metatype(coordinate_double_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    coordinate_double_metatype::~coordinate_double_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating CoordinateDouble nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by coordinate_double_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    coordinate_double_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 2> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3d_id,
                           "point")
        };
        typedef node_type_impl<coordinate_double_node> node_type_t;

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
                        abstract_node<coordinate_double_node>::exposedfield<sfnode> >(
                            &coordinate_double_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<coordinate_double_node>::exposedfield<sfnode> >(
                            &coordinate_double_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<coordinate_double_node>::exposedfield<sfnode> >(
                            &coordinate_double_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<coordinate_double_node>::exposedfield<mfvec3d> >(
                            &coordinate_double_node::point_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<coordinate_double_node>::exposedfield<mfvec3d> >(
                            &coordinate_double_node::point_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<coordinate_double_node>::exposedfield<mfvec3d> >(
                            &coordinate_double_node::point_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const nurbs_curve_metatype::id =
        "urn:X-openvrml:node:NurbsCurve";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c nurbs_curve_metatype.
     */
    nurbs_curve_metatype::nurbs_curve_metatype(openvrml::browser & browser):
        node_metatype(nurbs_curve_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_curve_metatype::~nurbs_curve_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating NurbsCurve nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by nurbs_curve_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    nurbs_curve_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 7> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "controlPoint"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "tessellation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfdouble_id,
                           "weight"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "closed"),
            node_interface(node_interface::field_id,
                           field_value::mfdouble_id,
                           "knot"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "order")
        };
        typedef node_type_impl<nurbs_curve_node> node_type_t;

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
                        abstract_node<nurbs_curve_node>::exposedfield<sfnode> >(
                            &nurbs_curve_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_curve_node>::exposedfield<sfnode> >(
                            &nurbs_curve_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_curve_node>::exposedfield<sfnode> >(
                            &nurbs_curve_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_curve_node>::exposedfield<sfnode> >(
                            &nurbs_curve_node::control_point_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_curve_node>::exposedfield<sfnode> >(
                            &nurbs_curve_node::control_point_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_curve_node>::exposedfield<sfnode> >(
                            &nurbs_curve_node::control_point_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_curve_node>::exposedfield<sfint32> >(
                            &nurbs_curve_node::tessellation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_curve_node>::exposedfield<sfint32> >(
                            &nurbs_curve_node::tessellation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_curve_node>::exposedfield<sfint32> >(
                            &nurbs_curve_node::tessellation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_curve_node>::exposedfield<mfdouble> >(
                            &nurbs_curve_node::weight_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_curve_node>::exposedfield<mfdouble> >(
                            &nurbs_curve_node::weight_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_curve_node>::exposedfield<mfdouble> >(
                            &nurbs_curve_node::weight_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_curve_node::closed_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfdouble>(
                            &nurbs_curve_node::knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_curve_node::order_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const nurbs_curve2d_metatype::id =
        "urn:X-openvrml:node:NurbsCurve2D";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c nurbs_curve2d_metatype.
     */
    nurbs_curve2d_metatype::nurbs_curve2d_metatype(openvrml::browser & browser):
        node_metatype(nurbs_curve2d_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_curve2d_metatype::~nurbs_curve2d_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating NurbsCurve2D nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by nurbs_curve2d_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    nurbs_curve2d_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 7> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec2d_id,
                           "controlPoint"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "tessellation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfdouble_id,
                           "weight"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "closed"),
            node_interface(node_interface::field_id,
                           field_value::mfdouble_id,
                           "knot"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "order")
        };
        typedef node_type_impl<nurbs_curve2d_node> node_type_t;

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
                        abstract_node<nurbs_curve2d_node>::exposedfield<sfnode> >(
                            &nurbs_curve2d_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_curve2d_node>::exposedfield<sfnode> >(
                            &nurbs_curve2d_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_curve2d_node>::exposedfield<sfnode> >(
                            &nurbs_curve2d_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_curve2d_node>::exposedfield<mfvec2d> >(
                            &nurbs_curve2d_node::control_point_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_curve2d_node>::exposedfield<mfvec2d> >(
                            &nurbs_curve2d_node::control_point_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_curve2d_node>::exposedfield<mfvec2d> >(
                            &nurbs_curve2d_node::control_point_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_curve2d_node>::exposedfield<sfint32> >(
                            &nurbs_curve2d_node::tessellation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_curve2d_node>::exposedfield<sfint32> >(
                            &nurbs_curve2d_node::tessellation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_curve2d_node>::exposedfield<sfint32> >(
                            &nurbs_curve2d_node::tessellation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_curve2d_node>::exposedfield<mfdouble> >(
                            &nurbs_curve2d_node::weight_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_curve2d_node>::exposedfield<mfdouble> >(
                            &nurbs_curve2d_node::weight_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_curve2d_node>::exposedfield<mfdouble> >(
                            &nurbs_curve2d_node::weight_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_curve2d_node::closed_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfdouble>(
                            &nurbs_curve2d_node::knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_curve2d_node::order_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const nurbs_orientation_interpolator_metatype::id =
        "urn:X-openvrml:node:NurbsOrientationInterpolator";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c nurbs_orientation_interpolator_metatype.
     */
    nurbs_orientation_interpolator_metatype::
    nurbs_orientation_interpolator_metatype(openvrml::browser & browser):
        node_metatype(nurbs_orientation_interpolator_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_orientation_interpolator_metatype::~nurbs_orientation_interpolator_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating NurbsOrientationInterpolator
     *         nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by nurbs_orientation_interpolator_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    nurbs_orientation_interpolator_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 7> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "controlPoints"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfdouble_id,
                           "knot"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "order"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfdouble_id,
                           "weight"),
            node_interface(node_interface::eventout_id,
                           field_value::sfrotation_id,
                           "value_changed")
        };
        typedef node_type_impl<nurbs_orientation_interpolator_node> node_type_t;

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
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_orientation_interpolator_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_orientation_interpolator_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_orientation_interpolator_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        nurbs_orientation_interpolator_node::set_fraction_listener>(
                            &nurbs_orientation_interpolator_node::set_fraction_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_orientation_interpolator_node::control_points_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_orientation_interpolator_node::control_points_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_orientation_interpolator_node::control_points_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_orientation_interpolator_node::knot_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_orientation_interpolator_node::knot_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_orientation_interpolator_node::knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<sfint32> >(
                            &nurbs_orientation_interpolator_node::order_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<sfint32> >(
                            &nurbs_orientation_interpolator_node::order_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<sfint32> >(
                            &nurbs_orientation_interpolator_node::order_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_orientation_interpolator_node::weight_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_orientation_interpolator_node::weight_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_orientation_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_orientation_interpolator_node::weight_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        nurbs_orientation_interpolator_node::sfrotation_emitter>(
                            &nurbs_orientation_interpolator_node::value_changed_emitter_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const nurbs_patch_surface_metatype::id =
        "urn:X-openvrml:node:NurbsPatchSurface";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c nurbs_patch_surface_metatype.
     */
    nurbs_patch_surface_metatype::
    nurbs_patch_surface_metatype(openvrml::browser & browser):
        node_metatype(nurbs_patch_surface_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_patch_surface_metatype::~nurbs_patch_surface_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating NurbsPatchSurface nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by nurbs_patch_surface_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    nurbs_patch_surface_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 15> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "controlPoint"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "texCoord"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "uTessellation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "vTessellation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfdouble_id,
                           "weight"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "uClosed"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "uDimension"),
            node_interface(node_interface::field_id,
                           field_value::mfdouble_id,
                           "uKnot"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "uOrder"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "vClosed"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "vDimension"),
            node_interface(node_interface::field_id,
                           field_value::mfdouble_id,
                           "vKnot"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "vOrder")
        };
        typedef node_type_impl<nurbs_patch_surface_node> node_type_t;

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
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfnode> >(
                            &nurbs_patch_surface_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfnode> >(
                            &nurbs_patch_surface_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfnode> >(
                            &nurbs_patch_surface_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfnode> >(
                            &nurbs_patch_surface_node::control_point_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfnode> >(
                            &nurbs_patch_surface_node::control_point_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfnode> >(
                            &nurbs_patch_surface_node::control_point_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfnode> >(
                            &nurbs_patch_surface_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfnode> >(
                            &nurbs_patch_surface_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfnode> >(
                            &nurbs_patch_surface_node::tex_coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfint32> >(
                            &nurbs_patch_surface_node::u_tessellation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfint32> >(
                            &nurbs_patch_surface_node::u_tessellation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfint32> >(
                            &nurbs_patch_surface_node::u_tessellation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfint32> >(
                            &nurbs_patch_surface_node::v_tessellation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfint32> >(
                            &nurbs_patch_surface_node::v_tessellation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<sfint32> >(
                            &nurbs_patch_surface_node::v_tessellation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<mfdouble> >(
                            &nurbs_patch_surface_node::weight_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<mfdouble> >(
                            &nurbs_patch_surface_node::weight_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_patch_surface_node>::exposedfield<mfdouble> >(
                            &nurbs_patch_surface_node::weight_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_patch_surface_node::solid_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_patch_surface_node::u_closed_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_patch_surface_node::u_dimension_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfdouble>(
                            &nurbs_patch_surface_node::u_knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_patch_surface_node::u_order_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_patch_surface_node::v_closed_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_patch_surface_node::v_dimension_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfdouble>(
                            &nurbs_patch_surface_node::v_knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_patch_surface_node::v_order_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const nurbs_position_interpolator_metatype::id =
        "urn:X-openvrml:node:NurbsPositionInterpolator";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c nurbs_position_interpolator_metatype.
     */
    nurbs_position_interpolator_metatype::
    nurbs_position_interpolator_metatype(openvrml::browser & browser):
        node_metatype(nurbs_position_interpolator_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_position_interpolator_metatype::~nurbs_position_interpolator_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating NurbsPositionInterpolator nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by nurbs_position_interpolator_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    nurbs_position_interpolator_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 7> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "controlPoints"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfdouble_id,
                           "knot"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "order"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfdouble_id,
                           "weight"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "value_changed")
        };
        typedef node_type_impl<nurbs_position_interpolator_node> node_type_t;

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
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_position_interpolator_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_position_interpolator_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_position_interpolator_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        nurbs_position_interpolator_node::set_fraction_listener>(
                            &nurbs_position_interpolator_node::set_fraction_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_position_interpolator_node::control_points_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_position_interpolator_node::control_points_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_position_interpolator_node::control_points_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_position_interpolator_node::knot_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_position_interpolator_node::knot_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_position_interpolator_node::knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<sfint32> >(
                            &nurbs_position_interpolator_node::order_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<sfint32> >(
                            &nurbs_position_interpolator_node::order_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<sfint32> >(
                            &nurbs_position_interpolator_node::order_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_position_interpolator_node::weight_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_position_interpolator_node::weight_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_position_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_position_interpolator_node::weight_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        nurbs_position_interpolator_node::sfvec3f_emitter>(
                            &nurbs_position_interpolator_node::value_changed_emitter_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const nurbs_set_metatype::id = "urn:X-openvrml:node:NurbsSet";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c nurbs_set_metatype.
     */
    nurbs_set_metatype::nurbs_set_metatype(openvrml::browser & browser):
        node_metatype(nurbs_set_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_set_metatype::~nurbs_set_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating NurbsSet nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by nurbs_set_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    nurbs_set_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 7> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addGeometry"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeGeometry"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "geometry"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "tessellationScale"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        typedef node_type_impl<nurbs_set_node> node_type_t;

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
                        abstract_node<nurbs_set_node>::exposedfield<sfnode> >(
                            &nurbs_set_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_set_node>::exposedfield<sfnode> >(
                            &nurbs_set_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_set_node>::exposedfield<sfnode> >(
                            &nurbs_set_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        nurbs_set_node::add_geometry_listener>(
                            &nurbs_set_node::add_geometry_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        nurbs_set_node::remove_geometry_listener>(
                            &nurbs_set_node::remove_geometry_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_set_node>::exposedfield<mfnode> >(
                            &nurbs_set_node::geometry_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_set_node>::exposedfield<mfnode> >(
                            &nurbs_set_node::geometry_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_set_node>::exposedfield<mfnode> >(
                            &nurbs_set_node::geometry_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_set_node>::exposedfield<sffloat> >(
                            &nurbs_set_node::tessellation_scale_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_set_node>::exposedfield<sffloat> >(
                            &nurbs_set_node::tessellation_scale_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_set_node>::exposedfield<sffloat> >(
                            &nurbs_set_node::tessellation_scale_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &nurbs_set_node::bbox_center_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec3f>(
                            &nurbs_set_node::bbox_size_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const nurbs_surface_interpolator_metatype::id =
        "urn:X-openvrml:node:NurbsSurfaceInterpolator";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c nurbs_surface_interpolator_metatype.
     */
    nurbs_surface_interpolator_metatype::
    nurbs_surface_interpolator_metatype(openvrml::browser & browser):
        node_metatype(nurbs_surface_interpolator_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_surface_interpolator_metatype::~nurbs_surface_interpolator_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating NurbsSurfaceInterpolator nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by nurbs_surface_interpolator_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    nurbs_surface_interpolator_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 12> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::sfvec2f_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "controlPoints"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfdouble_id,
                           "weight"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "position_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "normal_changed"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "uDimension"),
            node_interface(node_interface::field_id,
                           field_value::mfdouble_id,
                           "uKnot"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "uOrder"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "vDimension"),
            node_interface(node_interface::field_id,
                           field_value::mfdouble_id,
                           "vKnot"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "vOrder")
        };
        typedef node_type_impl<nurbs_surface_interpolator_node> node_type_t;

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
                        abstract_node<nurbs_surface_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_surface_interpolator_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_surface_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_surface_interpolator_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_surface_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_surface_interpolator_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        nurbs_surface_interpolator_node::set_fraction_listener>(
                            &nurbs_surface_interpolator_node::set_fraction_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_surface_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_surface_interpolator_node::control_points_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_surface_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_surface_interpolator_node::control_points_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_surface_interpolator_node>::exposedfield<sfnode> >(
                            &nurbs_surface_interpolator_node::control_points_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_surface_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_surface_interpolator_node::weight_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_surface_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_surface_interpolator_node::weight_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_surface_interpolator_node>::exposedfield<mfdouble> >(
                            &nurbs_surface_interpolator_node::weight_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        nurbs_surface_interpolator_node::sfvec3f_emitter>(
                            &nurbs_surface_interpolator_node::position_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventout(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        nurbs_surface_interpolator_node::sfvec3f_emitter>(
                            &nurbs_surface_interpolator_node::normal_changed_emitter_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_surface_interpolator_node::u_dimension_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfdouble>(
                            &nurbs_surface_interpolator_node::u_knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_surface_interpolator_node::u_order_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_surface_interpolator_node::v_dimension_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfdouble>(
                            &nurbs_surface_interpolator_node::v_knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_surface_interpolator_node::v_order_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const nurbs_swept_surface_metatype::id =
        "urn:X-openvrml:node:NurbsSweptSurface";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c nurbs_swept_surface_metatype.
     */
    nurbs_swept_surface_metatype::
    nurbs_swept_surface_metatype(openvrml::browser & browser):
        node_metatype(nurbs_swept_surface_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_swept_surface_metatype::~nurbs_swept_surface_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating NurbsSweptSurface nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by nurbs_swept_surface_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    nurbs_swept_surface_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 5> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "crossSectionCurve"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "trajectoryCurve"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "ccw"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid")
        };
        typedef node_type_impl<nurbs_swept_surface_node> node_type_t;

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
                        abstract_node<nurbs_swept_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swept_surface_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_swept_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swept_surface_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_swept_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swept_surface_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_swept_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swept_surface_node::cross_section_curve_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_swept_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swept_surface_node::cross_section_curve_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_swept_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swept_surface_node::cross_section_curve_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_swept_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swept_surface_node::trajectory_curve_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_swept_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swept_surface_node::trajectory_curve_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_swept_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swept_surface_node::trajectory_curve_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_swept_surface_node::ccw_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_swept_surface_node::solid_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const nurbs_swung_surface_metatype::id =
        "urn:X-openvrml:node:NurbsSwungSurface";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c nurbs_swung_surface_metatype.
     */
    nurbs_swung_surface_metatype::
    nurbs_swung_surface_metatype(openvrml::browser & browser):
        node_metatype(nurbs_swung_surface_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_swung_surface_metatype::~nurbs_swung_surface_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating NurbsSwungSurface nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by nurbs_swung_surface_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    nurbs_swung_surface_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 5> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "profileCurve"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "trajectoryCurve"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "ccw"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid")
        };
        typedef node_type_impl<nurbs_swung_surface_node> node_type_t;

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
                        abstract_node<nurbs_swung_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swung_surface_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_swung_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swung_surface_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_swung_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swung_surface_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_swung_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swung_surface_node::componentcurve_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_swung_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swung_surface_node::componentcurve_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_swung_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swung_surface_node::componentcurve_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_swung_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swung_surface_node::trajectory_curve_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_swung_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swung_surface_node::trajectory_curve_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_swung_surface_node>::exposedfield<sfnode> >(
                            &nurbs_swung_surface_node::trajectory_curve_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_swung_surface_node::ccw_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_swung_surface_node::solid_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const nurbs_texture_coordinate_metatype::id =
        "urn:X-openvrml:node:NurbsTextureCoordinate";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c nurbs_texture_coordinate_metatype.
     */
    nurbs_texture_coordinate_metatype::
    nurbs_texture_coordinate_metatype(openvrml::browser & browser):
        node_metatype(nurbs_texture_coordinate_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_texture_coordinate_metatype::~nurbs_texture_coordinate_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating NurbsTextureCoordinate nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by nurbs_texture_coordinate_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    nurbs_texture_coordinate_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 9> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec2f_id,
                           "controlPoint"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "weight"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "uDimension"),
            node_interface(node_interface::field_id,
                           field_value::mfdouble_id,
                           "uKnot"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "uOrder"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "vDimension"),
            node_interface(node_interface::field_id,
                           field_value::mfdouble_id,
                           "vKnot"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "vOrder")
        };
        typedef node_type_impl<nurbs_texture_coordinate_node> node_type_t;

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
                        abstract_node<nurbs_texture_coordinate_node>::exposedfield<sfnode> >(
                            &nurbs_texture_coordinate_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_texture_coordinate_node>::exposedfield<sfnode> >(
                            &nurbs_texture_coordinate_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_texture_coordinate_node>::exposedfield<sfnode> >(
                            &nurbs_texture_coordinate_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_texture_coordinate_node>::exposedfield<mfvec2f> >(
                            &nurbs_texture_coordinate_node::control_point_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_texture_coordinate_node>::exposedfield<mfvec2f> >(
                            &nurbs_texture_coordinate_node::control_point_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_texture_coordinate_node>::exposedfield<mfvec2f> >(
                            &nurbs_texture_coordinate_node::control_point_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_texture_coordinate_node>::exposedfield<mffloat> >(
                            &nurbs_texture_coordinate_node::weight_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_texture_coordinate_node>::exposedfield<mffloat> >(
                            &nurbs_texture_coordinate_node::weight_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_texture_coordinate_node>::exposedfield<mffloat> >(
                            &nurbs_texture_coordinate_node::weight_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_texture_coordinate_node::u_dimension_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfdouble>(
                            &nurbs_texture_coordinate_node::u_knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_texture_coordinate_node::u_order_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_texture_coordinate_node::v_dimension_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfdouble>(
                            &nurbs_texture_coordinate_node::v_knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_texture_coordinate_node::v_order_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const nurbs_trimmed_surface_metatype::id =
        "urn:X-openvrml:node:NurbsTrimmedSurface";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c nurbs_trimmed_surface_metatype.
     */
    nurbs_trimmed_surface_metatype::
    nurbs_trimmed_surface_metatype(openvrml::browser & browser):
        node_metatype(nurbs_trimmed_surface_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_trimmed_surface_metatype::~nurbs_trimmed_surface_metatype()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating NurbsTrimmedSurface nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by nurbs_trimmed_surface_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    nurbs_trimmed_surface_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 18> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addTrimmingContour"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeTrimmingContour"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "controlPoint"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "texCoord"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "trimmingContour"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "uTessellation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "vTessellation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfdouble_id,
                           "weight"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "uClosed"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "uDimension"),
            node_interface(node_interface::field_id,
                           field_value::mfdouble_id,
                           "uKnot"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "uOrder"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "vClosed"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "vDimension"),
            node_interface(node_interface::field_id,
                           field_value::mfdouble_id,
                           "vKnot"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "vOrder")
        };
        typedef node_type_impl<nurbs_trimmed_surface_node> node_type_t;

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
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfnode> >(
                            &nurbs_trimmed_surface_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfnode> >(
                            &nurbs_trimmed_surface_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfnode> >(
                            &nurbs_trimmed_surface_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        nurbs_trimmed_surface_node::add_trimming_contour_listener>(
                            &nurbs_trimmed_surface_node::add_trimming_contour_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        nurbs_trimmed_surface_node::remove_trimming_contour_listener>(
                            &nurbs_trimmed_surface_node::remove_trimming_contour_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfnode> >(
                            &nurbs_trimmed_surface_node::control_point_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfnode> >(
                            &nurbs_trimmed_surface_node::control_point_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfnode> >(
                            &nurbs_trimmed_surface_node::control_point_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfnode> >(
                            &nurbs_trimmed_surface_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfnode> >(
                            &nurbs_trimmed_surface_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfnode> >(
                            &nurbs_trimmed_surface_node::tex_coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<mfnode> >(
                            &nurbs_trimmed_surface_node::trimming_contour_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<mfnode> >(
                            &nurbs_trimmed_surface_node::trimming_contour_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<mfnode> >(
                            &nurbs_trimmed_surface_node::trimming_contour_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfint32> >(
                            &nurbs_trimmed_surface_node::u_tessellation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfint32> >(
                            &nurbs_trimmed_surface_node::u_tessellation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfint32> >(
                            &nurbs_trimmed_surface_node::u_tessellation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfint32> >(
                            &nurbs_trimmed_surface_node::v_tessellation_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfint32> >(
                            &nurbs_trimmed_surface_node::v_tessellation_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<sfint32> >(
                            &nurbs_trimmed_surface_node::v_tessellation_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<mfdouble> >(
                            &nurbs_trimmed_surface_node::weight_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<mfdouble> >(
                            &nurbs_trimmed_surface_node::weight_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<nurbs_trimmed_surface_node>::exposedfield<mfdouble> >(
                            &nurbs_trimmed_surface_node::weight_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_trimmed_surface_node::solid_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_trimmed_surface_node::u_closed_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_trimmed_surface_node::u_dimension_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfdouble>(
                            &nurbs_trimmed_surface_node::u_knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_trimmed_surface_node::u_order_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &nurbs_trimmed_surface_node::v_closed_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_trimmed_surface_node::v_dimension_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfdouble>(
                            &nurbs_trimmed_surface_node::v_knot_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfint32>(
                            &nurbs_trimmed_surface_node::v_order_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @class contour2d_node
     *
     * @brief Represents Contour2D node instances.
     */

    /**
     * @var contour2d_node::contour2d_metatype
     *
     * @brief Class object for Contour2D nodes.
     */

    /**
     * @var contour2d_node::add_children_
     *
     * @brief add_children eventIn
     */

    /**
     * @var contour2d_node::remove_children_
     *
     * @brief remove_children eventIn
     */

    /**
     * @var contour2d_node::children_
     *
     * @brief children exposedField
     */

    contour2d_node::add_children_listener::
    add_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    contour2d_node::add_children_listener::
    ~add_children_listener() OPENVRML_NOTHROW
    {}

    void contour2d_node::add_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    contour2d_node::remove_children_listener::
    remove_children_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    contour2d_node::remove_children_listener::
    ~remove_children_listener() OPENVRML_NOTHROW
    {}

    void contour2d_node::remove_children_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    contour2d_node::
    contour2d_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        add_children_listener_(*this),
        remove_children_listener_(*this),
        children_(*this)
    {}

    /**
     * @brief Destroy.
     */
    contour2d_node::~contour2d_node() OPENVRML_NOTHROW
    {}


    /**
     * @class contour_polyline2d_node
     *
     * @brief Represents ContourPolyline2D node instances.
     */

    /**
     * @var contour_polyline2d_node::contour_polyline2d_metatype
     *
     * @brief Class object for ContourPolyline2D nodes.
     */

    /**
     * @var contour_polyline2d_node::point_
     *
     * @brief point exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    contour_polyline2d_node::
    contour_polyline2d_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        point_(*this)
    {}

    /**
     * @brief Destroy.
     */
    contour_polyline2d_node::~contour_polyline2d_node() OPENVRML_NOTHROW
    {}


    /**
     * @class coordinate_double_node
     *
     * @brief Represents CoordinateDouble node instances.
     */

    /**
     * @var coordinate_double_node::coordinate_double_metatype
     *
     * @brief Class object for CoordinateDouble nodes.
     */

    /**
     * @var coordinate_double_node::point_
     *
     * @brief point exposedField
     */


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    coordinate_double_node::
    coordinate_double_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        point_(*this)
    {}

    /**
     * @brief Destroy.
     */
    coordinate_double_node::~coordinate_double_node() OPENVRML_NOTHROW
    {}


    /**
     * @class nurbs_curve_node
     *
     * @brief Represents NurbsCurve node instances.
     */

    /**
     * @var nurbs_curve_node::NurbsCurve_metatype
     *
     * @brief Class object for NurbsCurve nodes.
     */

    /**
     * @var nurbs_curve_node::control_point_
     *
     * @brief control_point exposedField
     */

    /**
     * @var nurbs_curve_node::tessellation_
     *
     * @brief tessellation exposedField
     */

    /**
     * @var nurbs_curve_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_curve_node::closed_
     *
     * @brief closed field
     */

    /**
     * @var nurbs_curve_node::knot_
     *
     * @brief knot field
     */

    /**
     * @var nurbs_curve_node::order_
     *
     * @brief order field
     */


    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    openvrml::viewer::object_t
    nurbs_curve_node::
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
    bool nurbs_curve_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    nurbs_curve_node::
    nurbs_curve_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        control_point_(*this),
        tessellation_(*this),
        weight_(*this),
        order_(3)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_curve_node::~nurbs_curve_node() OPENVRML_NOTHROW
    {}


    /**
     * @class nurbs_curve2d_node
     *
     * @brief Represents NurbsCurve2D node instances.
     */

    /**
     * @var nurbs_curve2d_node::NurbsCurve2D_metatype
     *
     * @brief Class object for NurbsCurve2D nodes.
     */

    /**
     * @var nurbs_curve2d_node::control_point_
     *
     * @brief control_point exposedField
     */

    /**
     * @var nurbs_curve2d_node::tessellation_
     *
     * @brief tessellation exposedField
     */

    /**
     * @var nurbs_curve2d_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_curve2d_node::closed_
     *
     * @brief closed field
     */

    /**
     * @var nurbs_curve2d_node::knot_
     *
     * @brief knot field
     */

    /**
     * @var nurbs_curve2d_node::order_
     *
     * @brief order field
     */


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    nurbs_curve2d_node::
    nurbs_curve2d_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        control_point_(*this),
        tessellation_(*this),
        weight_(*this),
        order_(3)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_curve2d_node::~nurbs_curve2d_node() OPENVRML_NOTHROW
    {}


    /**
     * @class nurbs_orientation_interpolator_node
     *
     * @brief Represents NurbsOrientationInterpolator node instances.
     */

    /**
     * @var nurbs_orientation_interpolator_node::nurbs_orientation_interpolator_metatype
     *
     * @brief Class object for NurbsOrientationInterpolator nodes.
     */

    /**
     * @var nurbs_orientation_interpolator_node::set_fraction_
     *
     * @brief set_fraction eventIn
     */

    /**
     * @var nurbs_orientation_interpolator_node::control_points_
     *
     * @brief control_points exposedField
     */

    /**
     * @var nurbs_orientation_interpolator_node::knot_
     *
     * @brief knot exposedField
     */

    /**
     * @var nurbs_orientation_interpolator_node::order_
     *
     * @brief order exposedField
     */

    /**
     * @var nurbs_orientation_interpolator_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_orientation_interpolator_node::value_changed_
     *
     * @brief value_changed eventOut
     */

    nurbs_orientation_interpolator_node::set_fraction_listener::
    set_fraction_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    nurbs_orientation_interpolator_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    void nurbs_orientation_interpolator_node::set_fraction_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    nurbs_orientation_interpolator_node::
    nurbs_orientation_interpolator_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_fraction_listener_(*this),
        control_points_(*this),
        knot_(*this),
        order_(*this, 3),
        weight_(*this),
        value_changed_emitter_(*this, this->value_changed_)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_orientation_interpolator_node::~nurbs_orientation_interpolator_node()
        OPENVRML_NOTHROW
    {}


    /**
     * @class nurbs_patch_surface_node
     *
     * @brief Represents NurbsPatchSurface node instances.
     */

    /**
     * @var nurbs_patch_surface_node::nurbs_patch_surface_metatype
     *
     * @brief Class object for NurbsPatchSurface nodes.
     */

    /**
     * @var nurbs_patch_surface_node::control_point_
     *
     * @brief control_point exposedField
     */

    /**
     * @var nurbs_patch_surface_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var nurbs_patch_surface_node::u_tessellation_
     *
     * @brief u_tessellation exposedField
     */

    /**
     * @var nurbs_patch_surface_node::v_tessellation_
     *
     * @brief v_tessellation exposedField
     */

    /**
     * @var nurbs_patch_surface_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_patch_surface_node::solid_
     *
     * @brief solid field
     */

    /**
     * @var nurbs_patch_surface_node::u_closed_
     *
     * @brief u_closed field
     */

    /**
     * @var nurbs_patch_surface_node::u_dimension_
     *
     * @brief u_dimension field
     */

    /**
     * @var nurbs_patch_surface_node::u_knot_
     *
     * @brief u_knot field
     */

    /**
     * @var nurbs_patch_surface_node::u_order_
     *
     * @brief u_order field
     */

    /**
     * @var nurbs_patch_surface_node::v_closed_
     *
     * @brief v_closed field
     */

    /**
     * @var nurbs_patch_surface_node::v_dimension_
     *
     * @brief v_dimension field
     */

    /**
     * @var nurbs_patch_surface_node::v_knot_
     *
     * @brief v_knot field
     */

    /**
     * @var nurbs_patch_surface_node::v_order_
     *
     * @brief v_order field
     */


    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    openvrml::viewer::object_t
    nurbs_patch_surface_node::
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
    bool nurbs_patch_surface_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    nurbs_patch_surface_node::
    nurbs_patch_surface_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        control_point_(*this),
        tex_coord_(*this),
        u_tessellation_(*this),
        v_tessellation_(*this),
        weight_(*this),
        solid_(true),
        u_order_(3),
        v_order_(3)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_patch_surface_node::~nurbs_patch_surface_node() OPENVRML_NOTHROW
    {}


    /**
     * @class nurbs_position_interpolator_node
     *
     * @brief Represents NurbsPositionInterpolator node instances.
     */

    /**
     * @var nurbs_position_interpolator_node::nurbs_position_interpolator_metatype
     *
     * @brief Class object for NurbsPositionInterpolator nodes.
     */

    /**
     * @var nurbs_position_interpolator_node::set_fraction_
     *
     * @brief set_fraction eventIn
     */

    /**
     * @var nurbs_position_interpolator_node::control_points_
     *
     * @brief control_points exposedField
     */

    /**
     * @var nurbs_position_interpolator_node::knot_
     *
     * @brief knot exposedField
     */

    /**
     * @var nurbs_position_interpolator_node::order_
     *
     * @brief order exposedField
     */

    /**
     * @var nurbs_position_interpolator_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_position_interpolator_node::value_changed_
     *
     * @brief value_changed eventOut
     */

    nurbs_position_interpolator_node::set_fraction_listener::
    set_fraction_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sffloat_listener(node)
    {}

    nurbs_position_interpolator_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    void nurbs_position_interpolator_node::set_fraction_listener::
    do_process_event(const sffloat & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    nurbs_position_interpolator_node::
    nurbs_position_interpolator_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_fraction_listener_(*this),
        control_points_(*this),
        knot_(*this),
        order_(*this, 3),
        weight_(*this),
        value_changed_emitter_(*this, this->value_changed_)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_position_interpolator_node::~nurbs_position_interpolator_node()
        OPENVRML_NOTHROW
    {}


    /**
     * @class nurbs_set_node
     *
     * @brief Represents NurbsSet node instances.
     */

    /**
     * @var nurbs_set_node::nurbs_set_metatype
     *
     * @brief Class object for NurbsSet nodes.
     */

    /**
     * @var nurbs_set_node::add_geometry_
     *
     * @brief add_geometry eventIn
     */

    /**
     * @var nurbs_set_node::remove_geometry_
     *
     * @brief remove_geometry eventIn
     */

    /**
     * @var nurbs_set_node::geometry_
     *
     * @brief geometry exposedField
     */

    /**
     * @var nurbs_set_node::tessellation_scale_
     *
     * @brief tessellation_scale exposedField
     */

    /**
     * @var nurbs_set_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var nurbs_set_node::bbox_size_
     *
     * @brief bbox_size field
     */

    nurbs_set_node::add_geometry_listener::
    add_geometry_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    nurbs_set_node::add_geometry_listener::
    ~add_geometry_listener() OPENVRML_NOTHROW
    {}

    void nurbs_set_node::add_geometry_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    nurbs_set_node::remove_geometry_listener::
    remove_geometry_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    nurbs_set_node::remove_geometry_listener::
    ~remove_geometry_listener() OPENVRML_NOTHROW
    {}

    void nurbs_set_node::remove_geometry_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Construct.
     *
     * @param type  the @c node_type associated with this node.
     * @param scope the @c scope to which the node belongs.
     */
    nurbs_set_node::
    nurbs_set_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        add_geometry_listener_(*this),
        remove_geometry_listener_(*this),
        geometry_(*this),
        tessellation_scale_(*this, 1),
        bbox_size_(vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    nurbs_set_node::~nurbs_set_node() OPENVRML_NOTHROW
    {}


    /**
     * @class nurbs_surface_interpolator_node
     *
     * @brief Represents NurbsSurfaceInterpolator node instances.
     */

    /**
     * @var nurbs_surface_interpolator_node::nurbs_surface_interpolator_metatype
     *
     * @brief Class object for NurbsSurfaceInterpolator nodes.
     */

    /**
     * @var nurbs_surface_interpolator_node::set_fraction_
     *
     * @brief set_fraction eventIn
     */

    /**
     * @var nurbs_surface_interpolator_node::control_points_
     *
     * @brief control_points exposedField
     */

    /**
     * @var nurbs_surface_interpolator_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_surface_interpolator_node::position_changed_
     *
     * @brief position_changed eventOut
     */

    /**
     * @var nurbs_surface_interpolator_node::normal_changed_
     *
     * @brief normal_changed eventOut
     */

    /**
     * @var nurbs_surface_interpolator_node::u_dimension_
     *
     * @brief u_dimension field
     */

    /**
     * @var nurbs_surface_interpolator_node::u_knot_
     *
     * @brief u_knot field
     */

    /**
     * @var nurbs_surface_interpolator_node::u_order_
     *
     * @brief u_order field
     */

    /**
     * @var nurbs_surface_interpolator_node::v_dimension_
     *
     * @brief v_dimension field
     */

    /**
     * @var nurbs_surface_interpolator_node::v_knot_
     *
     * @brief v_knot field
     */

    /**
     * @var nurbs_surface_interpolator_node::v_order_
     *
     * @brief v_order field
     */

    nurbs_surface_interpolator_node::set_fraction_listener::
    set_fraction_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        sfvec2f_listener(node)
    {}

    nurbs_surface_interpolator_node::set_fraction_listener::
    ~set_fraction_listener() OPENVRML_NOTHROW
    {}

    void nurbs_surface_interpolator_node::set_fraction_listener::
    do_process_event(const sfvec2f & /* fraction */, const double /* timestamp */)
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
    nurbs_surface_interpolator_node::
    nurbs_surface_interpolator_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        child_node(type, scope),
        set_fraction_listener_(*this),
        control_points_(*this),
        weight_(*this),
        position_changed_emitter_(*this, this->position_changed_),
        normal_changed_emitter_(*this, this->normal_changed_),
        u_order_(3),
        v_order_(3)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_surface_interpolator_node::~nurbs_surface_interpolator_node()
        OPENVRML_NOTHROW
    {}


    /**
     * @class nurbs_swept_surface_node
     *
     * @brief Represents NurbsSweptSurface node instances.
     */

    /**
     * @var nurbs_swept_surface_node::nurbs_swept_surface_metatype
     *
     * @brief Class object for NurbsSweptSurface nodes.
     */

    /**
     * @var nurbs_swept_surface_node::cross_section_curve_
     *
     * @brief cross_section_curve exposedField
     */

    /**
     * @var nurbs_swept_surface_node::trajectory_curve_
     *
     * @brief trajectory_curve exposedField
     */

    /**
     * @var nurbs_swept_surface_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var nurbs_swept_surface_node::solid_
     *
     * @brief solid field
     */


    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    openvrml::viewer::object_t
    nurbs_swept_surface_node::
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
    bool nurbs_swept_surface_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    nurbs_swept_surface_node::
    nurbs_swept_surface_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        cross_section_curve_(*this),
        trajectory_curve_(*this),
        ccw_(true),
        solid_(true)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_swept_surface_node::~nurbs_swept_surface_node() OPENVRML_NOTHROW
    {}


    /**
     * @class nurbs_swung_surface_node
     *
     * @brief Represents NurbsSwungSurface node instances.
     */

    /**
     * @var nurbs_swung_surface_node::nurbs_swung_surface_metatype
     *
     * @brief Class object for NurbsSwungSurface nodes.
     */

    /**
     * @var nurbs_swung_surface_node::componentcurve_
     *
     * @brief componentcurve exposedField
     */

    /**
     * @var nurbs_swung_surface_node::trajectory_curve_
     *
     * @brief trajectory_curve exposedField
     */

    /**
     * @var nurbs_swung_surface_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var nurbs_swung_surface_node::solid_
     *
     * @brief solid field
     */


    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    openvrml::viewer::object_t
    nurbs_swung_surface_node::
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
    bool nurbs_swung_surface_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    nurbs_swung_surface_node::
    nurbs_swung_surface_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        componentcurve_(*this),
        trajectory_curve_(*this),
        ccw_(true),
        solid_(true)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_swung_surface_node::~nurbs_swung_surface_node() OPENVRML_NOTHROW
    {}


    /**
     * @class nurbs_texture_coordinate_node
     *
     * @brief Represents NurbsTextureCoordinate node instances.
     */

    /**
     * @var nurbs_texture_coordinate_node::nurbs_texture_coordinate_metatype
     *
     * @brief Class object for NurbsTextureCoordinate nodes.
     */

    /**
     * @var nurbs_texture_coordinate_node::control_point_
     *
     * @brief control_point exposedField
     */

    /**
     * @var nurbs_texture_coordinate_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_texture_coordinate_node::u_dimension_
     *
     * @brief u_dimension field
     */

    /**
     * @var nurbs_texture_coordinate_node::u_knot_
     *
     * @brief u_knot field
     */

    /**
     * @var nurbs_texture_coordinate_node::u_order_
     *
     * @brief u_order field
     */

    /**
     * @var nurbs_texture_coordinate_node::v_dimension_
     *
     * @brief v_dimension field
     */

    /**
     * @var nurbs_texture_coordinate_node::v_knot_
     *
     * @brief v_knot field
     */

    /**
     * @var nurbs_texture_coordinate_node::v_order_
     *
     * @brief v_order field
     */


    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    nurbs_texture_coordinate_node::
    nurbs_texture_coordinate_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        control_point_(*this),
        weight_(*this),
        u_order_(3),
        v_order_(3)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_texture_coordinate_node::~nurbs_texture_coordinate_node()
        OPENVRML_NOTHROW
    {}


    /**
     * @class nurbs_trimmed_surface_node
     *
     * @brief Represents NurbsTrimmedSurface node instances.
     */

    /**
     * @var nurbs_trimmed_surface_node::nurbs_trimmed_surface_metatype
     *
     * @brief Class object for NurbsTrimmedSurface nodes.
     */

    /**
     * @var nurbs_trimmed_surface_node::add_trimming_contour_
     *
     * @brief add_trimming_contour eventIn
     */

    /**
     * @var nurbs_trimmed_surface_node::remove_trimming_contour_
     *
     * @brief remove_trimming_contour eventIn
     */

    /**
     * @var nurbs_trimmed_surface_node::control_point_
     *
     * @brief control_point exposedField
     */

    /**
     * @var nurbs_trimmed_surface_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var nurbs_trimmed_surface_node::trimming_contour_
     *
     * @brief trimming_contour exposedField
     */

    /**
     * @var nurbs_trimmed_surface_node::u_tessellation_
     *
     * @brief u_tessellation exposedField
     */

    /**
     * @var nurbs_trimmed_surface_node::v_tessellation_
     *
     * @brief v_tessellation exposedField
     */

    /**
     * @var nurbs_trimmed_surface_node::weight_
     *
     * @brief weight exposedField
     */

    /**
     * @var nurbs_trimmed_surface_node::solid_
     *
     * @brief solid field
     */

    /**
     * @var nurbs_trimmed_surface_node::u_closed_
     *
     * @brief u_closed field
     */

    /**
     * @var nurbs_trimmed_surface_node::u_dimension_
     *
     * @brief u_dimension field
     */

    /**
     * @var nurbs_trimmed_surface_node::u_knot_
     *
     * @brief u_knot field
     */

    /**
     * @var nurbs_trimmed_surface_node::u_order_
     *
     * @brief u_order field
     */

    /**
     * @var nurbs_trimmed_surface_node::v_closed_
     *
     * @brief v_closed field
     */

    /**
     * @var nurbs_trimmed_surface_node::v_dimension_
     *
     * @brief v_dimension field
     */

    /**
     * @var nurbs_trimmed_surface_node::v_knot_
     *
     * @brief v_knot field
     */

    /**
     * @var nurbs_trimmed_surface_node::v_order_
     *
     * @brief v_order field
     */

    nurbs_trimmed_surface_node::add_trimming_contour_listener::
    add_trimming_contour_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    nurbs_trimmed_surface_node::add_trimming_contour_listener::
    ~add_trimming_contour_listener() OPENVRML_NOTHROW
    {}

    void nurbs_trimmed_surface_node::add_trimming_contour_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }

    nurbs_trimmed_surface_node::remove_trimming_contour_listener::
    remove_trimming_contour_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfnode_listener(node)
    {}

    nurbs_trimmed_surface_node::remove_trimming_contour_listener::
    ~remove_trimming_contour_listener() OPENVRML_NOTHROW
    {}

    void nurbs_trimmed_surface_node::remove_trimming_contour_listener::
    do_process_event(const mfnode & /* fraction */, const double /* timestamp */)
        OPENVRML_THROW1(std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    openvrml::viewer::object_t
    nurbs_trimmed_surface_node::
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
    bool nurbs_trimmed_surface_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    nurbs_trimmed_surface_node::
    nurbs_trimmed_surface_node(const node_type & type,
                               const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        add_trimming_contour_listener_(*this),
        remove_trimming_contour_listener_(*this),
        control_point_(*this),
        tex_coord_(*this),
        trimming_contour_(*this),
        u_tessellation_(*this),
        v_tessellation_(*this),
        weight_(*this),
        solid_(true),
        u_order_(3),
        v_order_(3)
    {}

    /**
     * @brief Destroy.
     */
    nurbs_trimmed_surface_node::~nurbs_trimmed_surface_node() OPENVRML_NOTHROW
    {}
}
