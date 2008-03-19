// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
// Copyright 2002  S. K. Bose
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
# include <openvrml/browser.h>

# include "grouping_node_base.h"
# include "billboard.h"

namespace {

    class OPENVRML_LOCAL billboard_node :
        public openvrml_node_vrml97::grouping_node_base<billboard_node> {
        friend class openvrml_node_vrml97::billboard_metatype;

        exposedfield<openvrml::sfvec3f> axis_of_rotation_;

        openvrml::viewer::object_t xformObject;

    public:
        static const openvrml::mat4f billboard_to_matrix(const billboard_node & node,
                                               const openvrml::mat4f & modelview);

        billboard_node(const openvrml::node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~billboard_node() OPENVRML_NOTHROW;

    private:
        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
    };


    /**
     * @class billboard_node
     *
     * @brief Billboard node instances.
     */

    /**
     * @var class billboard_node::billboard_metatype
     *
     * @brief Class object for Billboard nodes.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<billboard_node>::exposedfield<openvrml::sfvec3f> billboard_node::axis_of_rotation_
     *
     * @brief axisOfRotation exposedField.
     */

    /**
     * @var openvrml::viewer::object_t billboard_node::xformObject
     *
     * @brief Handle for the renderer.
     */

    /**
     * @brief Get the bounding box transformation matrix.
     *
     * @param node      a pointer to a billboard_node.
     * @param modelview input ModelView transformation matrix.
     *
     * @return the bounding box transformation matrix.
     */
    const openvrml::mat4f
    billboard_node::
    billboard_to_matrix(const billboard_node & node, const openvrml::mat4f & modelview)
    {
        using namespace openvrml;

        const mat4f inverse_modelview = modelview.inverse();

        mat4f result;

        // Viewer position in local coordinate system
        const vec3f position = make_vec3f(inverse_modelview[3][0],
                                          inverse_modelview[3][1],
                                          inverse_modelview[3][2]).normalize();

        // Viewer-alignment
        if ((node.axis_of_rotation_.sfvec3f::value()[0] == 0)
            && (node.axis_of_rotation_.sfvec3f::value()[1] == 0)
            && (node.axis_of_rotation_.sfvec3f::value()[2] == 0)) {
            //
            // Viewer's up vector
            //
            const vec3f up = make_vec3f(inverse_modelview[1][0],
                                        inverse_modelview[1][1],
                                        inverse_modelview[1][2]).normalize();

            // get x-vector from the cross product of Viewer's
            // up vector and billboard-to-viewer vector.
            const vec3f X = up * position;

            result[0][0] = X[0];
            result[0][1] = X[1];
            result[0][2] = X[2];
            result[0][3] = 0.0;

            result[1][0] = up[0];
            result[1][1] = up[1];
            result[1][2] = up[2];
            result[1][3] = 0.0;

            result[2][0] = position[0];
            result[2][1] = position[1];
            result[2][2] = position[2];
            result[2][3] = 0.0;

            result[3][0] = 0.0;
            result[3][1] = 0.0;
            result[3][2] = 0.0;
            result[3][3] = 1.0;
        } else { // use axis of rotation
            // axis of rotation will be the y-axis vector
            const vec3f Y(node.axis_of_rotation_.sfvec3f::value());

            // Plane defined by the axisOfRotation and billboard-to-viewer vector
            const vec3f X = (Y * position).normalize();

            // Get Z axis vector from cross product of X and Y
            const vec3f Z = X * Y;

            // Transform Z axis vector of current coordinate system to new
            // coordinate system.
            float nz[3] = { X[2], Y[2], Z[2] };

            // calculate the angle by which the Z axis vector of current coordinate
            // system has to be rotated around the Y axis to new coordinate system.
            float angle = float(acos(nz[2]));
            if(nz[0] > 0) { angle = -angle; }
            result = make_rotation_mat4f(make_rotation(Y, angle));
        }
        return result;
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    billboard_node::
    billboard_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        grouping_node(type, scope),
        openvrml_node_vrml97::grouping_node_base<billboard_node>(type, scope),
        axis_of_rotation_(*this, openvrml::make_vec3f(0.0, 1.0, 0.0)),
        xformObject(0)
    {}

    /**
     * @brief Destroy.
     */
    billboard_node::~billboard_node() OPENVRML_NOTHROW
    {
        // delete xformObject...
    }

    /**
     * @brief Render the node.
     *
     * @param viewer    a viewer.
     * @param context   the rendering context.
     */
    void
    billboard_node::
    do_render_child(openvrml::viewer & viewer, openvrml::rendering_context context)
    {
        using openvrml::mat4f;

        mat4f new_LM = context.matrix();
        mat4f LM = billboard_to_matrix(*this, new_LM);
        new_LM = LM * new_LM;
        context.matrix(new_LM);

        if (this->xformObject && this->modified()) {
            viewer.remove_object(this->xformObject);
            this->xformObject = 0;
        }

        if (this->xformObject) {
            viewer.insert_reference(this->xformObject);
        } else if (this->children_.mfnode::value().size() > 0) {
            this->xformObject = viewer.begin_object(this->id().c_str());

            viewer.transform(LM);

            // Render children
            this->openvrml_node_vrml97::grouping_node_base<billboard_node>::
                do_render_child(viewer, context);

            viewer.end_object();
        }

        this->node::modified(false);
    }
}

/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::billboard_metatype::id =
    "urn:X-openvrml:node:Billboard";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::billboard_metatype::
billboard_metatype(openvrml::browser & browser):
    node_metatype(billboard_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::billboard_metatype::~billboard_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Billboard nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by billboard_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::billboard_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 7> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "axisOfRotation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef node_impl_util::node_type_impl<billboard_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & billboardNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface == *++supported_interface) {
            billboardNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    billboard_node::add_children_listener>(
                        &billboard_node::add_children_listener_)));
        } else if (*interface == *++supported_interface) {
            billboardNodeType.add_eventin(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    billboard_node::remove_children_listener>(
                        &billboard_node::remove_children_listener_)));
        } else if (*interface == *++supported_interface) {
            billboardNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<billboard_node>::exposedfield<sfvec3f> >(
                        &billboard_node::axis_of_rotation_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<billboard_node>::exposedfield<sfvec3f> >(
                        &billboard_node::axis_of_rotation_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<billboard_node>::exposedfield<sfvec3f> >(
                        &billboard_node::axis_of_rotation_)));
        } else if (*interface == *++supported_interface) {
            billboardNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    billboard_node::children_exposedfield>(
                        &billboard_node::children_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    billboard_node::children_exposedfield>(
                        &billboard_node::children_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    billboard_node::children_exposedfield>(
                        &billboard_node::children_)));
        } else if (*interface == *++supported_interface) {
            billboardNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &billboard_node::bbox_center_)));
        } else if (*interface == *++supported_interface) {
            billboardNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &billboard_node::bbox_size_)));
        } else if (*interface == *++supported_interface) {
            billboardNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    abstract_node<billboard_node>::exposedfield<sfnode> >(
                        &billboard_node::metadata)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    abstract_node<billboard_node>::exposedfield<sfnode> >(
                        &billboard_node::metadata)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    abstract_node<billboard_node>::exposedfield<sfnode> >(
                        &billboard_node::metadata)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}
