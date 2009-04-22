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

# include "shape.h"
# include <private.h>
# include <openvrml/node_impl_util.h>
# include <openvrml/viewer.h>
# include <boost/array.hpp>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

    class OPENVRML_LOCAL shape_node :
        public openvrml::node_impl_util::abstract_node<shape_node>,
        public openvrml::child_node {

        friend class openvrml_node_vrml97::shape_metatype;

        exposedfield<openvrml::sfnode> appearance_;
        exposedfield<openvrml::sfnode> geometry_;
        openvrml::sfvec3f bbox_center_;
        openvrml::sfvec3f bbox_size_;

    public:
        shape_node(const openvrml::node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~shape_node() OPENVRML_NOTHROW;

    private:
        virtual bool do_modified() const
            OPENVRML_THROW1(boost::thread_resource_error);

        virtual const openvrml::bounding_volume & do_bounding_volume() const;

        virtual void do_render_child(openvrml::viewer & viewer,
                                     openvrml::rendering_context context);
    };

    /**
     * @class shape_node
     *
     * @brief Represents Shape node instances.
     */

    /**
     * @var class shape_node::shape_metatype
     *
     * @brief Class object for Shape instances.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<shape_node>::exposedfield<openvrml::sfnode> shape_node::appearance_
     *
     * @brief appearance exposedField.
     */

    /**
     * @var openvrml::node_impl_util::abstract_node<shape_node>::exposedfield<openvrml::sfnode> shape_node::geometry_
     *
     * @brief geometry exposedField.
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node.
     * @param scope     the scope to which the node belongs.
     */
    shape_node::
    shape_node(const openvrml::node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        openvrml::node_impl_util::abstract_node<shape_node>(type, scope),
        child_node(type, scope),
        appearance_(*this),
        geometry_(*this),
        bbox_size_(openvrml::make_vec3f(-1,-1,-1))
    {}

    /**
     * @brief Destroy.
     */
    shape_node::~shape_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or one of its children has been modified,
     *      @c false otherwise.
     */
    bool shape_node::do_modified() const
        OPENVRML_THROW1(boost::thread_resource_error)
    {
        return (this->geometry_.value() && this->geometry_.value()->modified())
            || (this->appearance_.value()
                && this->appearance_.value()->modified());
    }

    OPENVRML_LOCAL void set_unlit_material(openvrml::viewer & v)
    {
        using openvrml::color;
        using openvrml::make_color;
        static const float unlit_ambient_intensity(1);
        static const color unlit_diffuse_color = make_color(1, 1, 1);
        static const color unlit_emissive_color = make_color(1, 1, 1);
        static const float unlit_shininess(0);
        static const color unlit_specular_color = make_color(1, 1, 1);
        static const float unlit_transparency(0);
        v.set_material(unlit_ambient_intensity,
                       unlit_diffuse_color,
                       unlit_emissive_color,
                       unlit_shininess,
                       unlit_specular_color,
                       unlit_transparency);
    }

    /**
     * @brief Render the node.
     *
     * @param v         a viewer.
     * @param context   a rendering context.
     */
    void shape_node::do_render_child(openvrml::viewer & v,
                                     const openvrml::rendering_context context)
    {
        using openvrml::node_cast;
        using openvrml::geometry_node;
        using openvrml::viewer;

        openvrml::appearance_node * const appearance =
            node_cast<openvrml::appearance_node *>(
                this->appearance_.sfnode::value().get());
        openvrml::material_node * const material =
            appearance
            ? node_cast<openvrml::material_node *>(
                appearance->material().get())
            : 0;
        geometry_node * const geometry =
            node_cast<geometry_node *>(this->geometry_.sfnode::value().get());

        //
        // If the appearance has changed, we need to rerender the geometry.
        //
        if (geometry && appearance && appearance->modified()) {
            geometry->modified(true);
        }

        if (this->modified()
            || (appearance && appearance->modified())
            || (geometry && geometry->modified())) {
            v.remove_object(*this);
        }

        if (geometry) {
            v.begin_object(this->id().c_str());

            // Don't care what color it is if we are picking
            bool picking = (viewer::pick_mode == v.mode());
            if (!picking) {
                size_t texture_components = 0;

                if (appearance) {
                    using openvrml::texture_node;

                    appearance->render_appearance(v, context);

                    texture_node * const texture =
                        node_cast<texture_node *>(appearance->texture().get());
                    if (texture) {
                        texture_components = texture->image().comp();
                    }
                } else {
                    v.enable_lighting(false);  // turn lighting off
                    set_unlit_material(v);
                }

                // hack for opengl material mode
                v.set_material_mode(texture_components, geometry->color());

                //
                // Set the drawing color.
                //
                // Most geometries are drawn using the diffuse color; but some
                // (notably point and line sets) are drawn with the emissive
                // color.
                //
                openvrml::color c = openvrml::make_color(1.0, 1.0, 1.0);
                float transparency = 0.0;
                if (material) {
                    if (geometry && geometry->emissive()) {
                        c = material->emissive_color();
                    } else {
                        c = material->diffuse_color();
                    }
                    transparency = material->transparency();
                }
                v.set_color(c, transparency);
            }

            geometry->render_geometry(v, context);

            v.end_object();
        }
    }

    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    shape_node::do_bounding_volume() const
    {
        using openvrml::node_cast;

        //
        // just pass off to the geometry's getbvolume() method
        //
        bounded_volume_node * bv =
            node_cast<bounded_volume_node *>(
                this->geometry_.sfnode::value().get());
        const openvrml::bounding_volume & result = bv
            ? bv->bounding_volume()
            : this->bounded_volume_node::do_bounding_volume();
        const_cast<shape_node *>(this)->bounding_volume_dirty(false);
        return result;
    }


}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::shape_metatype::id =
    "urn:X-openvrml:node:Shape";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this @c node_metatype.
 */
openvrml_node_vrml97::shape_metatype::
shape_metatype(openvrml::browser & browser):
    node_metatype(shape_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::shape_metatype::~shape_metatype() OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating Shape nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by shape_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::shape_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using namespace openvrml;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 5> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "appearance"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "geometry"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    typedef node_impl_util::node_type_impl<shape_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & shapeNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            shapeNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &shape_node::appearance_);
        } else if (*interface_ == *++supported_interface) {
            shapeNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &shape_node::geometry_);
        } else if (*interface_ == *++supported_interface) {
            shapeNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &shape_node::metadata);
        } else if (*interface_ == *++supported_interface) {
            shapeNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &shape_node::bbox_center_);
        } else if (*interface_ == *++supported_interface) {
            shapeNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &shape_node::bbox_size_);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
