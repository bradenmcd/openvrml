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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <private.h>
# include "transform.h"
# include "grouping_node_base.h"

namespace {

    class OPENVRML_LOCAL transform_node :
        public openvrml_node_vrml97::grouping_node_base<transform_node>,
        public openvrml::transform_node {

        friend class openvrml_node_vrml97::transform_metatype;

        class center_exposedfield : public exposedfield<openvrml::sfvec3f> {
        public:
            explicit center_exposedfield(transform_node & node);
            center_exposedfield(const center_exposedfield & obj)
                OPENVRML_NOTHROW;
            virtual ~center_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::sfvec3f & center,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class rotation_exposedfield :
            public exposedfield<openvrml::sfrotation> {
        public:
            explicit rotation_exposedfield(transform_node & node);
            rotation_exposedfield(const rotation_exposedfield & obj)
                OPENVRML_NOTHROW;
            virtual ~rotation_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual
            void event_side_effect(const openvrml::sfrotation & rotation,
                                   double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class scale_exposedfield : public exposedfield<openvrml::sfvec3f> {
        public:
            explicit scale_exposedfield(transform_node & node);
            scale_exposedfield(const scale_exposedfield & obj) OPENVRML_NOTHROW;
            virtual ~scale_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void event_side_effect(const openvrml::sfvec3f & scale,
                                           double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class scale_orientation_exposedfield :
            public exposedfield<openvrml::sfrotation> {
        public:
            explicit scale_orientation_exposedfield(transform_node & node);
            scale_orientation_exposedfield(
                const scale_orientation_exposedfield & obj) OPENVRML_NOTHROW;
            virtual ~scale_orientation_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual void
            event_side_effect(const openvrml::sfrotation & scale_orientation,
                              double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        class translation_exposedfield :
            public exposedfield<openvrml::sfvec3f> {
        public:
            explicit translation_exposedfield(transform_node & node);
            translation_exposedfield(const translation_exposedfield & obj)
                OPENVRML_NOTHROW;
            virtual ~translation_exposedfield() OPENVRML_NOTHROW;

        private:
            virtual std::auto_ptr<openvrml::field_value> do_clone() const
                OPENVRML_THROW1(std::bad_alloc);
            virtual
            void event_side_effect(const openvrml::sfvec3f & translation,
                                   double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        center_exposedfield center_;
        rotation_exposedfield rotation_;
        scale_exposedfield scale_;
        scale_orientation_exposedfield scale_orientation_;
        translation_exposedfield translation_;

        mutable openvrml::mat4f transform_;
        mutable bool transform_dirty;

    public:
        transform_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~transform_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);

        virtual const openvrml::mat4f & do_transform() const OPENVRML_NOTHROW;

        void recalc_bsphere();
        void update_transform() const OPENVRML_NOTHROW;
    };


    /**
     * @class transform_node
     *
     * @brief Transform node instances.
     */

    /**
     * @var class transform_node::transform_metatype
     *
     * @brief Class object for Transform nodes.
     */

    /**
     * @internal
     *
     * @class transform_node::center_exposedfield
     *
     * @brief center exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  transform_node.
     */
    transform_node::center_exposedfield::
    center_exposedfield(transform_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfvec3f_listener(node),
        exposedfield<openvrml::sfvec3f>(node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    transform_node::center_exposedfield::
    center_exposedfield(const center_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfvec3f_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::sfvec3f>(obj)
    {}

    /**
     * @brief Destroy.
     */
    transform_node::center_exposedfield::~center_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    transform_node::center_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new center_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param center    center.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    transform_node::center_exposedfield::event_side_effect(const openvrml::sfvec3f &,
                                                           double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            transform_node & node =
                dynamic_cast<transform_node &>(this->node_event_listener::node());
            node.bounding_volume_dirty(true);
            node.transform_dirty = true;
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class transform_node::rotation_exposedfield
     *
     * @brief rotation exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  transform_node.
     */
    transform_node::rotation_exposedfield::
    rotation_exposedfield(transform_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfrotation_listener(node),
        exposedfield<openvrml::sfrotation>(node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    transform_node::rotation_exposedfield::
    rotation_exposedfield(const rotation_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfrotation_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::sfrotation>(obj)
    {}

    /**
     * @brief Destroy.
     */
    transform_node::rotation_exposedfield::~rotation_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    transform_node::rotation_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new rotation_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param rotation  rotation.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    transform_node::rotation_exposedfield::
    event_side_effect(const openvrml::sfrotation &, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            transform_node & node =
                dynamic_cast<transform_node &>(this->node_event_listener::node());
            node.bounding_volume_dirty(true);
            node.transform_dirty = true;
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class transform_node::scale_exposedfield
     *
     * @brief scale exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  transform_node.
     */
    transform_node::scale_exposedfield::
    scale_exposedfield(transform_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfvec3f_listener(node),
        exposedfield<openvrml::sfvec3f>(node,
                                        openvrml::make_vec3f(1.0f, 1.0f, 1.0f))
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    transform_node::scale_exposedfield::
    scale_exposedfield(const scale_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfvec3f_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::sfvec3f>(obj)
    {}

    /**
     * @brief Destroy.
     */
    transform_node::scale_exposedfield::
    ~scale_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    transform_node::scale_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new scale_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param scale     scale.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    transform_node::scale_exposedfield::event_side_effect(const openvrml::sfvec3f &,
                                                          double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            transform_node & node =
                dynamic_cast<transform_node &>(this->node_event_listener::node());
            node.bounding_volume_dirty(true);
            node.transform_dirty = true;
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class transform_node::scale_orientation_exposedfield
     *
     * @brief scaleOrientation exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  transform_node.
     */
    transform_node::scale_orientation_exposedfield::
    scale_orientation_exposedfield(transform_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfrotation_listener(node),
        exposedfield<openvrml::sfrotation>(node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    transform_node::scale_orientation_exposedfield::
    scale_orientation_exposedfield(const scale_orientation_exposedfield & obj)
        OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfrotation_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::sfrotation>(obj)
    {}

    /**
     * @brief Destroy.
     */
    transform_node::scale_orientation_exposedfield::
    ~scale_orientation_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    transform_node::scale_orientation_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new scale_orientation_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param scale_orientation scaleOrientation.
     * @param timestamp         the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    transform_node::scale_orientation_exposedfield::
    event_side_effect(const openvrml::sfrotation &, double) OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            transform_node & node =
                dynamic_cast<transform_node &>(this->node_event_listener::node());
            node.bounding_volume_dirty(true);
            node.transform_dirty = true;
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @internal
     *
     * @class transform_node::translation_exposedfield
     *
     * @brief translation exposedField implementation.
     */

    /**
     * @brief Construct.
     *
     * @param node  transform_node.
     */
    transform_node::translation_exposedfield::
    translation_exposedfield(transform_node & node):
        openvrml::node_event_listener(node),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfvec3f_listener(node),
        exposedfield<openvrml::sfvec3f>(node)
    {}

    /**
     * @brief Construct a copy.
     *
     * @param obj   instance to copy.
     */
    transform_node::translation_exposedfield::
    translation_exposedfield(const translation_exposedfield & obj) OPENVRML_NOTHROW:
        openvrml::event_listener(),
        openvrml::node_event_listener(obj.openvrml::node_event_listener::node()),
        openvrml::event_emitter(static_cast<const openvrml::field_value &>(*this)),
        sfvec3f_listener(obj.openvrml::node_event_listener::node()),
        exposedfield<openvrml::sfvec3f>(obj)
    {}

    /**
     * @brief Destroy.
     */
    transform_node::translation_exposedfield::
    ~translation_exposedfield() OPENVRML_NOTHROW
    {}

    /**
     * @brief Polymorphically construct a copy.
     *
     * @return a copy of the instance.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    std::auto_ptr<openvrml::field_value>
    transform_node::translation_exposedfield::do_clone() const
        OPENVRML_THROW1(std::bad_alloc)
    {
        return std::auto_ptr<openvrml::field_value>(
            new translation_exposedfield(*this));
    }

    /**
     * @brief Process event.
     *
     * @param translation     translation.
     * @param timestamp the current time.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void
    transform_node::translation_exposedfield::
    event_side_effect(const openvrml::sfvec3f &, double)
        OPENVRML_THROW1(std::bad_alloc)
    {
        try {
            transform_node & node =
                dynamic_cast<transform_node &>(this->node_event_listener::node());
            node.bounding_volume_dirty(true);
            node.transform_dirty = true;
        } catch (std::bad_cast & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }

    /**
     * @var transform_node::center_exposedfield transform_node::center_
     *
     * @brief center exposedField.
     */

    /**
     * @var transform_node::rotation_exposedfield transform_node::rotation_
     *
     * @brief rotation exposedField.
     */

    /**
     * @var transform_node::scale_exposedfield transform_node::scale_
     *
     * @brief scale exposedField.
     */

    /**
     * @var transform_node::scale_orientation_exposedfield transform_node::scale_orientation_
     *
     * @brief scaleOrientation exposedField.
     */

    /**
     * @var transform_node::translation_exposedfield transform_node::translation_
     *
     * @brief translation exposedField.
     */

    /**
     * @var openvrml::mat4f transform_node::transform_
     *
     * @brief Cached copy of this node's transformation.
     */

    /**
     * @var bool transform_node::transform_dirty
     *
     * @brief Flag to indicate whether @a transform_ needs to be updated.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the instance.
     * @param scope the scope that the new node will belong to.
     */
    transform_node::
    transform_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        grouping_node(type, scope),
        openvrml_node_vrml97::grouping_node_base<transform_node>(type, scope),
        openvrml::transform_node(type, scope),
        center_(*this),
        rotation_(*this),
        scale_(*this),
        scale_orientation_(*this),
        translation_(*this),
        transform_(openvrml::make_mat4f()),
        transform_dirty(true)
    {
        this->bounding_volume_dirty(true);
    }

    /**
     * @brief Destroy.
     */
    transform_node::~transform_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the transformation associated with the node as a matrix.
     *
     * @return the transformation associated with the node.
     */
    const openvrml::mat4f & transform_node::do_transform() const
        OPENVRML_NOTHROW
    {
        this->update_transform();
        return this->transform_;
    }

    /**
     * @brief Render the node.
     *
     * @param viewer    a Viewer.
     * @param context   the rendering context.
     */
    void transform_node::do_render_child(openvrml::viewer & viewer,
                                         openvrml::rendering_context context)
    {
        using openvrml::bounding_volume;
        if (context.cull_flag != bounding_volume::inside) {
            using boost::polymorphic_downcast;
            using openvrml::bounding_sphere;
            const bounding_sphere & bs =
                *polymorphic_downcast<const bounding_sphere *>(
                    &this->bounding_volume());
            bounding_sphere bv_copy(bs);
            bv_copy.transform(context.matrix());
            bounding_volume::intersection r =
                viewer.intersect_view_volume(bv_copy);
            if (context.draw_bounding_spheres) {
                viewer.draw_bounding_sphere(bs, r);
            }

            if (r == bounding_volume::outside) { return; }
            if (r == bounding_volume::inside) {
                context.cull_flag = bounding_volume::inside;
            }
        }

        openvrml::mat4f new_LM = this->transform_ * context.matrix();
        context.matrix(new_LM);

        if (modified()) {
            viewer.remove_object(*this);
        }

        if (!this->children_.mfnode::value().empty()) {
            viewer.begin_object(this->id().c_str());

            // Apply transforms
            viewer.transform(this->transform());
            // Render children
            this->openvrml_node_vrml97::grouping_node_base<transform_node>::
                render_nocull(viewer, context);

            viewer.end_object();
        }
        this->node::modified(false);
    }


    /**
     * @brief Recalculate the bounding volume.
     */
    void transform_node::recalc_bsphere()
    {
        openvrml_node_vrml97::grouping_node_base<self_t>::recalc_bsphere();
        this->bsphere.transform(this->transform());
        this->bounding_volume_dirty(false);
    }

    /**
     * @brief Update @a transform.
     *
     * If @a transform_dirty is @c true, resynchronize the cached matrix
     * @a transform with the node fields; otherwise do nothing.
     *
     * @note P' = T × C × R × SR × S × -SR × -C × P
     */
    void transform_node::update_transform() const OPENVRML_NOTHROW
    {
        if (this->transform_dirty) {
            this->transform_ =
                make_transformation_mat4f(
                    this->translation_.sfvec3f::value(),
                    this->rotation_.sfrotation::value(),
                    this->scale_.sfvec3f::value(),
                    this->scale_orientation_.sfrotation::value(),
                    this->center_.sfvec3f::value());
            this->transform_dirty = false;
        }
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::transform_metatype::id =
    "urn:X-openvrml:node:Transform";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::transform_metatype::
transform_metatype(openvrml::browser & browser):
    node_metatype(transform_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::transform_metatype::~transform_metatype()
    OPENVRML_NOTHROW
{}

# define TRANSFORM_INTERFACE_SEQ                                        \
    ((eventin,      mfnode,     "addChildren",      add_children_listener_)) \
    ((eventin,      mfnode,     "removeChildren",   remove_children_listener_)) \
    ((exposedfield, sfvec3f,    "center",           center_))           \
    ((exposedfield, mfnode,     "children",         children_))         \
    ((exposedfield, sfrotation, "rotation",         rotation_))         \
    ((exposedfield, sfvec3f,    "scale",            scale_))            \
    ((exposedfield, sfrotation, "scaleOrientation", scale_orientation_)) \
    ((exposedfield, sfvec3f,    "translation",      translation_))      \
    ((field,        sfvec3f,    "bboxCenter",       bbox_center_))      \
    ((field,        sfvec3f,    "bboxSize",         bbox_size_))        \
    ((exposedfield, sfnode,     "metadata",         metadata))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_vrml97,
                                              transform_metatype,
                                              transform_node,
                                              TRANSFORM_INTERFACE_SEQ)
