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
# include "extrusion.h"

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
        virtual openvrml::viewer::object_t do_render_geometry(openvrml::viewer & viewer,
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
    openvrml::viewer::object_t
    extrusion_node::
    do_render_geometry(openvrml::viewer & v, openvrml::rendering_context)
    {
        using openvrml::viewer;

        viewer::object_t obj = 0;
        if (!this->cross_section_.value().empty()
            && this->spine_.value().size() > 1) {
            unsigned int optMask = 0;
            if (this->ccw_.value())       { optMask |= viewer::mask_ccw; }
            if (this->convex_.value())    { optMask |= viewer::mask_convex; }
            if (this->solid_.value())     { optMask |= viewer::mask_solid; }
            if (this->begin_cap_.value()) { optMask |= viewer::mask_bottom; }
            if (this->end_cap_.value())   { optMask |= viewer::mask_top; }

            obj = v.insert_extrusion(optMask,
                                     this->spine_.value(),
                                     this->cross_section_.value(),
                                     this->orientation_.value(),
                                     this->scale_.value());
        }

        return obj;
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

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Extrusion nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by extrusion_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::extrusion_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 15> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::eventin_id,
                       field_value::mfvec2f_id,
                       "set_crossSection"),
        node_interface(node_interface::eventin_id,
                       field_value::mfrotation_id,
                       "set_orientation"),
        node_interface(node_interface::eventin_id,
                       field_value::mfvec2f_id,
                       "set_scale"),
        node_interface(node_interface::eventin_id,
                       field_value::mfvec3f_id,
                       "set_spine"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "beginCap"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "ccw"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "convex"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "creaseAngle"),
        node_interface(node_interface::field_id,
                       field_value::mfvec2f_id,
                       "crossSection"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "endCap"),
        node_interface(node_interface::field_id,
                       field_value::mfrotation_id,
                       "orientation"),
        node_interface(node_interface::field_id,
                       field_value::mfvec2f_id,
                       "scale"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid"),
        node_interface(node_interface::field_id,
                       field_value::mfvec3f_id,
                       "spine"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<extrusion_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & extrusionNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::set_cross_section_listener_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::set_orientation_listener_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::set_scale_listener_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::set_spine_listener_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::begin_cap_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::ccw_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::convex_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::crease_angle_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::cross_section_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::end_cap_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::orientation_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::scale_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::solid_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::spine_);
        } else if (*interface_ == *++supported_interface) {
            extrusionNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &extrusion_node::metadata);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
