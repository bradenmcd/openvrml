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

# include "extrusion.h"
# include <private.h>
# include <openvrml/node_impl_util.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL extrusion_node :
        public openvrml::node_impl_util::abstract_node<extrusion_node>,
        public openvrml::geometry_node {

        friend class openvrml_node_vrml97::extrusion_metatype;

        class set_cross_section_listener :
            public openvrml::node_impl_util::event_listener_base<self_t>,
            public mfvec2f_listener {
        public:
            explicit set_cross_section_listener(extrusion_node & node);
            virtual ~set_cross_section_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::mfvec2f & cross_section,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_orientation_listener : public openvrml::node_impl_util::event_listener_base<self_t>,
                                         public mfrotation_listener {
        public:
            explicit set_orientation_listener(extrusion_node & node);
            virtual ~set_orientation_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::mfrotation & orientation,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_scale_listener : public openvrml::node_impl_util::event_listener_base<self_t>,
                                   public mfvec2f_listener {
        public:
            explicit set_scale_listener(extrusion_node & node);
            virtual ~set_scale_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::mfvec2f & scale,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class set_spine_listener : public openvrml::node_impl_util::event_listener_base<self_t>,
                                   public mfvec3f_listener {
        public:
            explicit set_spine_listener(extrusion_node & node);
            virtual ~set_spine_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const openvrml::mfvec3f & spine,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_cross_section_listener set_cross_section_listener_;
        set_orientation_listener set_orientation_listener_;
        set_scale_listener set_scale_listener_;
        set_spine_listener set_spine_listener_;
        openvrml::sfbool begin_cap_;
        openvrml::sfbool ccw_;
        openvrml::sfbool convex_;
        openvrml::sffloat crease_angle_;
        openvrml::mfvec2f cross_section_;
        openvrml::sfbool end_cap_;
        openvrml::mfrotation orientation_;
        openvrml::mfvec2f scale_;
        openvrml::sfbool solid_;
        openvrml::mfvec3f spine_;

    public:
        extrusion_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~extrusion_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_geometry(openvrml::viewer & viewer,
                                        openvrml::rendering_context context);
    };

    /**
     * @class extrusion_node
     *
     * @brief Extrusion node instances.
     */

    /**
     * @var class extrusion_node::extrusion_metatype
     *
     * @brief Class object for Extrusion nodes.
     */

    /**
     * @internal
     *
     * @class extrusion_node::set_cross_section_listener
     *
     * @brief set_crossSection event handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  extrusion_node.
     */
    extrusion_node::set_cross_section_listener::
    set_cross_section_listener(extrusion_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<extrusion_node>(node),
        mfvec2f_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    extrusion_node::set_cross_section_listener::
    ~set_cross_section_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param cross_section cross_section value.
     * @param timestamp     the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    extrusion_node::set_cross_section_listener::
    do_process_event(const openvrml::mfvec2f & cross_section, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            extrusion_node & extrusion =
                dynamic_cast<extrusion_node &>(this->node());
            extrusion.cross_section_ = cross_section;
            extrusion.node::modified(true);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class extrusion_node::set_orientation_listener
     *
     * @brief set_orientation event handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  extrusion_node.
     */
    extrusion_node::set_orientation_listener::
    set_orientation_listener(extrusion_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<extrusion_node>(node),
        mfrotation_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    extrusion_node::set_orientation_listener::
    ~set_orientation_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param orientation   orientation value.
     * @param timestamp     the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    extrusion_node::set_orientation_listener::
    do_process_event(const openvrml::mfrotation & orientation, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            extrusion_node & extrusion =
                dynamic_cast<extrusion_node &>(this->node());
            extrusion.orientation_ = orientation;
            extrusion.node::modified(true);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class extrusion_node::set_scale_listener
     *
     * @brief set_scale event handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  extrusion_node.
     */
    extrusion_node::set_scale_listener::
    set_scale_listener(extrusion_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<extrusion_node>(node),
        mfvec2f_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    extrusion_node::set_scale_listener::
    ~set_scale_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param scale     scale value.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    extrusion_node::set_scale_listener::
    do_process_event(const openvrml::mfvec2f & scale, double) OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            extrusion_node & extrusion =
                dynamic_cast<extrusion_node &>(this->node());
            extrusion.scale_ = scale;
            extrusion.node::modified(true);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class extrusion_node::set_spine_listener
     *
     * @brief set_spine event handler.
     */

    /**
     * @brief Construct.
     *
     * @param node  extrusion_node.
     */
    extrusion_node::set_spine_listener::
    set_spine_listener(extrusion_node & node):
        openvrml::node_event_listener(node),
        openvrml::node_impl_util::event_listener_base<extrusion_node>(node),
        mfvec3f_listener(node)
    {}

    /**
     * @brief Destroy.
     */
    extrusion_node::set_spine_listener::
    ~set_spine_listener() OPENVRML_NOTHROW
    {}

    /**
     * @brief Process event.
     *
     * @param spine     spine value.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    extrusion_node::set_spine_listener::
    do_process_event(const openvrml::mfvec3f & spine, double) OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            extrusion_node & extrusion =
                dynamic_cast<extrusion_node &>(this->node());
            extrusion.spine_ = spine;
            extrusion.node::modified(true);
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var extrusion_node::set_cross_section_listener extrusion_node::set_cross_section_listener_
     *
     * @brief set_crossSection eventIn handler.
     */

    /**
     * @var extrusion_node::set_orientation_listener extrusion_node::set_orientation_listener_
     *
     * @brief set_orientation eventIn handler.
     */

    /**
     * @var extrusion_node::set_scale_listener extrusion_node::set_scale_listener_
     *
     * @brief set_scale eventIn handler.
     */

    /**
     * @var extrusion_node::set_spine_listener extrusion_node::set_spine_listener_
     *
     * @brief set_spine eventIn handler.
     */

    /**
     * @var openvrml::sfbool extrusion_node::begin_cap_
     *
     * @brief beginCap field.
     */

    /**
     * @var openvrml::sfbool extrusion_node::ccw_
     *
     * @brief ccw field.
     */

    /**
     * @var openvrml::sfbool extrusion_node::convex_
     *
     * @brief convex field.
     */

    /**
     * @var openvrml::sffloat extrusion_node::crease_angle_
     *
     * @brief creaseAngle field.
     */

    /**
     * @var openvrml::mfvec2f extrusion_node::cross_section_
     *
     * @brief crossSection field.
     */

    /**
     * @var openvrml::sfbool extrusion_node::end_cap_
     *
     * @brief endCap field.
     */

    /**
     * @var openvrml::mfrotation extrusion_node::orientation_
     *
     * @brief orientation field.
     */

    /**
     * @var openvrml::mfvec2f extrusion_node::scale_
     *
     * @brief scale field.
     */

    /**
     * @var openvrml::sfbool extrusion_node::solid_
     *
     * @brief solid field.
     */

    /**
     * @var openvrml::mfvec3f extrusion_node::spine_
     *
     * @brief spine field.
     */

    const openvrml::vec2f extrusionDefaultCrossSection_[] =
    {
        openvrml::make_vec2f(1.0, 1.0),
        openvrml::make_vec2f(1.0, -1.0),
        openvrml::make_vec2f(-1.0, -1.0),
        openvrml::make_vec2f(-1.0, 1.0),
        openvrml::make_vec2f(1.0, 1.0)
    };

    const openvrml::vec2f extrusionDefaultScale_[] =
    {
        openvrml::make_vec2f(1.0, 1.0)
    };

    const openvrml::rotation extrusionDefaultOrientation_[] =
    {
        openvrml::make_rotation(0.0, 0.0, 1.0, 0.0)
    };

    const openvrml::vec3f extrusionDefaultSpine_[] =
    {
        openvrml::make_vec3f(0.0, 0.0, 0.0),
        openvrml::make_vec3f(0.0, 1.0, 0.0)
    };

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    extrusion_node::
    extrusion_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<extrusion_node>(type, scope),
        geometry_node(type, scope),
        set_cross_section_listener_(*this),
        set_orientation_listener_(*this),
        set_scale_listener_(*this),
        set_spine_listener_(*this),
        begin_cap_(true),
        ccw_(true),
        convex_(true),
        crease_angle_(0),
        cross_section_(
            std::vector<openvrml::vec2f>(extrusionDefaultCrossSection_,
                                         extrusionDefaultCrossSection_ + 5)),
        end_cap_(true),
        orientation_(
            std::vector<openvrml::rotation>(extrusionDefaultOrientation_,
                                            extrusionDefaultOrientation_ + 1)),
        scale_(std::vector<openvrml::vec2f>(extrusionDefaultScale_,
                                            extrusionDefaultScale_ + 1)),
        solid_(true),
        spine_(std::vector<openvrml::vec3f>(extrusionDefaultSpine_,
                                            extrusionDefaultSpine_ + 2))
    {}

    /**
     * @brief Destroy.
     */
    extrusion_node::~extrusion_node() OPENVRML_NOTHROW {}

    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param v         a viewer.
     * @param context   the rendering context.
     */
    void extrusion_node::do_render_geometry(openvrml::viewer & v,
                                            openvrml::rendering_context)
    {
        using openvrml::viewer;

        if (!this->cross_section_.value().empty()
            && this->spine_.value().size() > 1) {
            unsigned int optMask = 0;
            if (this->ccw_.value())       { optMask |= viewer::mask_ccw; }
            if (this->convex_.value())    { optMask |= viewer::mask_convex; }
            if (this->solid_.value())     { optMask |= viewer::mask_solid; }
            if (this->begin_cap_.value()) { optMask |= viewer::mask_bottom; }
            if (this->end_cap_.value())   { optMask |= viewer::mask_top; }

            v.insert_extrusion(*this,
                               optMask,
                               this->spine_.value(),
                               this->cross_section_.value(),
                               this->orientation_.value(),
                               this->scale_.value());
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::extrusion_metatype::id =
    "urn:X-openvrml:node:Extrusion";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::extrusion_metatype::
extrusion_metatype(openvrml::browser & browser):
    node_metatype(extrusion_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::extrusion_metatype::~extrusion_metatype()
    OPENVRML_NOTHROW
{}

# define EXTRUSION_INTERFACE_SPEC                                       \
    ((eventin,      mfvec2f,    "set_crossSection", set_cross_section_listener_)) \
    ((eventin,      mfrotation, "set_orientation",  set_orientation_listener_)) \
    ((eventin,      mfvec2f,    "set_scale",        set_scale_listener_)) \
    ((eventin,      mfvec3f,    "set_spine",        set_spine_listener_)) \
    ((field,        sfbool,     "beginCap",         begin_cap_))        \
    ((field,        sfbool,     "ccw",              ccw_))              \
    ((field,        sfbool,     "convex",           convex_))           \
    ((field,        sffloat,    "creaseAngle",      crease_angle_))     \
    ((field,        mfvec2f,    "crossSection",     cross_section_))    \
    ((field,        sfbool,     "endCap",           end_cap_))          \
    ((field,        mfrotation, "orientation",      orientation_))      \
    ((field,        mfvec2f,    "scale",            scale_))            \
    ((field,        sfbool,     "solid",            solid_))            \
    ((field,        mfvec3f,    "spine",            spine_))            \
    ((exposedfield, sfnode,     "metadata",         metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              extrusion_metatype,
                                              extrusion_node,
                                              EXTRUSION_INTERFACE_SPEC)
