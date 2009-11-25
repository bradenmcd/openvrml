// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2006, 2007, 2008, 2009  Braden McDaniel
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

# include "static_group.h"
# include <openvrml/node_impl_util.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Represents StaticGroup node instances.
     */
    class OPENVRML_LOCAL static_group_node :
        public abstract_node<static_group_node>,
        public grouping_node {

        friend class openvrml_node_x3d_grouping::static_group_metatype;

        mfnode children_;
        sfvec3f bbox_center_;
        sfvec3f bbox_size_;

        bounding_sphere bsphere;

    public:

        static_group_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~static_group_node() OPENVRML_NOTHROW;

    protected:
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);

        virtual void do_render_child(openvrml::viewer & viewer,
                                     rendering_context context);
        virtual const openvrml::bounding_volume &
        do_bounding_volume() const;
        virtual const std::vector<boost::intrusive_ptr<node> >
        do_children() const OPENVRML_THROW1(std::bad_alloc);

        virtual void recalc_bsphere();
        void render_nocull(openvrml::viewer & viewer,
                           rendering_context context);
    };


    /**
     * @var static_group_node::static_group_metatype
     *
     * @brief Class object for StaticGroup nodes.
     */

    /**
     * @var static_group_node::children_
     *
     * @brief children field
     */

    /**
     * @var static_group_node::bbox_center_
     *
     * @brief bbox_center field
     */

    /**
     * @var static_group_node::bbox_size_
     *
     * @brief bbox_size field
     */

    /**
     * @brief Get the children in the scene graph.
     *
     * @return the child nodes in the scene graph.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    const std::vector<boost::intrusive_ptr<node> >
    static_group_node::do_children() const OPENVRML_THROW1(std::bad_alloc)
    {
        return this->children_.value();
    }

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool static_group_node::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        for (size_t i = 0; i < this->children_.value().size(); ++i) {
            if (this->children_.value()[i]->modified()) { return true; }
        }
        return false;
    }

    /**
     * @brief Render the node.
     *
     * Render each of the children.
     *
     * @param viewer    a Viewer.
     * @param context   a rendering context.
     */
    void
    static_group_node::
    do_render_child(openvrml::viewer & viewer, rendering_context context)
    {
        if (context.cull_flag != bounding_volume::inside) {
            using boost::polymorphic_downcast;
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
        this->render_nocull(viewer, context);
    }

    /**
     * because children will already have done the culling, we don't need
     * to repeat it here.
     */
    void
    static_group_node::render_nocull(openvrml::viewer & viewer,
                                     const rendering_context context)
    {
        using std::vector;

        if (this->modified()) {
            viewer.remove_object(*this);
        }

        if (!this->children_.mfnode::value().empty()) {
            vector<boost::intrusive_ptr<node> >::size_type i;
            vector<boost::intrusive_ptr<node> >::size_type n =
                this->children_.mfnode::value().size();
            size_t nSensors = 0;

            viewer.begin_object(this->id().c_str());

            // Draw nodes that impact their siblings (DirectionalLights,
            // TouchSensors, any others? ...)
            for (i = 0; i < n; ++i) {
                child_node * const child =
                    node_cast<child_node *>(
                        this->children_.mfnode::value()[i].get());
                if (child) {
                    if (node_cast<light_node *>(child)
                        && !(node_cast<scoped_light_node *>(child))) {
                        child->render_child(viewer, context);
                    } else if (
                        node_cast<pointing_device_sensor_node *>(child)) {
                        if (++nSensors == 1) {
                            viewer.set_sensitive(this);
                        }
                    }
                }
            }

            // Do the rest of the children (except the scene-level lights)
            for (i = 0; i<n; ++i) {
                child_node * const child =
                    node_cast<child_node *>(
                        this->children_.mfnode::value()[i].get());
                if (child && !(node_cast<light_node *>(child))) {
                    child->render_child(viewer, context);
                }
            }

            // Turn off sensitivity
            if (nSensors > 0) { viewer.set_sensitive(0); }

            viewer.end_object();
        }

        this->node::modified(false);
    }

    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    static_group_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<static_group_node *>(this)
                ->recalc_bsphere();
        }
        return this->bsphere;
    }

    /**
     * @fn void grouping_node_base<Derived>::recalc_bsphere()
     *
     * @brief Recalculate the bounding volume.
     */
    void static_group_node::recalc_bsphere()
    {
        this->bsphere = bounding_sphere();
        for (size_t i = 0; i < this->children_.value().size(); ++i) {
            const boost::intrusive_ptr<node> & node =
                this->children_.value()[i];
            bounded_volume_node * bounded_volume =
                node_cast<bounded_volume_node *>(node.get());
            if (bounded_volume) {
                const openvrml::bounding_volume & ci_bv =
                    bounded_volume->bounding_volume();
                this->bsphere.extend(ci_bv);
            }
        }
        this->bounding_volume_dirty(false);
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    static_group_node::
    static_group_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        abstract_node<self_t>(type, scope),
        grouping_node(type, scope),
        bbox_size_(make_vec3f(-1.0f, -1.0f, -1.0f))
    {}

    /**
     * @brief Destroy.
     */
    static_group_node::~static_group_node() OPENVRML_NOTHROW
    {}
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_x3d_grouping::static_group_metatype::id =
    "urn:X-openvrml:node:StaticGroup";

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this static_group_metatype.
 */
openvrml_node_x3d_grouping::static_group_metatype::
static_group_metatype(openvrml::browser & browser):
    node_metatype(static_group_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_x3d_grouping::static_group_metatype::~static_group_metatype()
    OPENVRML_NOTHROW
{}

# define STATIC_GROUP_INTERFACE_SEQ                       \
    ((exposedfield, sfnode,  "metadata",   metadata))     \
    ((field,        mfnode,  "children",   children_))    \
    ((field,        sfvec3f, "bboxCenter", bbox_center_)) \
    ((field,        sfvec3f, "bboxSize",   bbox_size_))

OPENVRML_NODE_IMPL_UTIL_DEFINE_DO_CREATE_TYPE(openvrml_node_x3d_grouping,
                                              static_group_metatype,
                                              static_group_node,
                                              STATIC_GROUP_INTERFACE_SEQ)
