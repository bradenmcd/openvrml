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
# include "x3d_cad_geometry.h"

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Class object for CADFace nodes.
     */
    class OPENVRML_LOCAL cad_face_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit cad_face_metatype(openvrml::browser & browser);
        virtual ~cad_face_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for IndexedQuadSet and QuadSet nodes.
     */
    class OPENVRML_LOCAL quad_set_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit quad_set_metatype(openvrml::browser & browser);
        virtual ~quad_set_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };
}

void register_cad_geometry_node_metatypes(openvrml::browser & b)
{
    using openvrml::node_metatype;
    using boost::shared_ptr;
    b.add_node_metatype(cad_face_metatype::id,
                        shared_ptr<node_metatype>(new cad_face_metatype(b)));
    b.add_node_metatype(quad_set_metatype::id,
                        shared_ptr<node_metatype>(new quad_set_metatype(b)));
}

namespace {
    using namespace openvrml_;

    class OPENVRML_LOCAL cad_face_node : public abstract_node<cad_face_node>,
                                         public grouping_node {
        friend class cad_face_metatype;

        exposedfield<sfstring> name_;
        exposedfield<sfnode> shape_;

        //used by do_children to return the shape
        std::vector<boost::intrusive_ptr<node> > children_;

    public:
        cad_face_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~cad_face_node() OPENVRML_NOTHROW;

        virtual bool modified() const;

    protected:
        virtual const openvrml::bounding_volume & do_bounding_volume() const;
        virtual const std::vector<boost::intrusive_ptr<node> > &
            do_children() const OPENVRML_NOTHROW;
    };



    class OPENVRML_LOCAL quad_set_node : public abstract_node<quad_set_node>,
                                         public geometry_node {
        friend class quad_set_metatype;

        class set_index_listener : public event_listener_base<self_t>,
                                   public mfint32_listener {
        public:
            explicit set_index_listener(self_t & node);
            virtual ~set_index_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfint32 & fraction,
                                          double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        set_index_listener set_index_listener_;
        exposedfield<sfnode> color_;
        exposedfield<sfnode> coord_;
        exposedfield<sfnode> normal_;
        exposedfield<sfnode> tex_coord_;
        sfbool ccw_;
        sfbool color_per_vertex_;
        sfbool normal_per_vertex_;
        sfbool solid_;
        mfint32 index_;
        bounding_sphere bsphere;

    public:
        quad_set_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~quad_set_node() OPENVRML_NOTHROW;

        virtual const color_node * color() const OPENVRML_NOTHROW;
        virtual bool modified() const;

    private:
        virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);

        void recalc_bsphere();
    };


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const cad_face_metatype::id = "urn:X-openvrml:node:CADFace";

    /**
     * @brief Construct.
     *
     * @param browser the browser associated with this cad_face_metatype.
     */
    cad_face_metatype::cad_face_metatype(openvrml::browser & browser):
        node_metatype(cad_face_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    cad_face_metatype::~cad_face_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating CADFace nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by cad_face_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    cad_face_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 3> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "name"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "shape")
        };
        typedef node_type_impl<cad_face_node> node_type_t;

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
                        abstract_node<cad_face_node>::exposedfield<sfnode> >(
                            &cad_face_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<cad_face_node>::exposedfield<sfnode> >(
                            &cad_face_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<cad_face_node>::exposedfield<sfnode> >(
                            &cad_face_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<cad_face_node>::exposedfield<sfstring> >(
                            &cad_face_node::name_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<cad_face_node>::exposedfield<sfstring> >(
                            &cad_face_node::name_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<cad_face_node>::exposedfield<sfstring> >(
                            &cad_face_node::name_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<cad_face_node>::exposedfield<sfnode> >(
                            &cad_face_node::shape_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<cad_face_node>::exposedfield<sfnode> >(
                            &cad_face_node::shape_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<cad_face_node>::exposedfield<sfnode> >(
                            &cad_face_node::shape_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const quad_set_metatype::id = "urn:X-openvrml:node:QuadSet";

    /**
     * @brief Construct.
     *
     * @param browser the browser associated with this quad_set_metatype.
     */
    quad_set_metatype::quad_set_metatype(openvrml::browser & browser):
        node_metatype(cad_face_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    quad_set_metatype::~quad_set_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a @c node_type capable of creating IndexedQuadSet nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by quad_set_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    quad_set_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 11> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::eventin_id,
                           field_value::mfint32_id,
                           "set_index"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "coord"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "normal"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "texCoord"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "ccw"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "colorPerVertex"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "normalPerVertex"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid"),
            node_interface(node_interface::field_id,
                           field_value::mfint32_id,
                           "index")
        };
        typedef node_type_impl<quad_set_node> node_type_t;

        const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
        node_type_t & the_node_type = static_cast<node_type_t &>(*type);

        for (node_interface_set::const_iterator interface(interfaces.begin());
             interface != interfaces.end();
             ++interface) {
            supported_interfaces_t::const_iterator supported_interface =
                supported_interfaces.begin() - 1;
            if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        quad_set_node::set_index_listener>(
                            &quad_set_node::set_index_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::color_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::normal_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::normal_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::normal_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<quad_set_node>::exposedfield<sfnode> >(
                            &quad_set_node::tex_coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &quad_set_node::ccw_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &quad_set_node::color_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &quad_set_node::normal_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &quad_set_node::solid_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfint32>(
                            &quad_set_node::index_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @class cad_face_node
     *
     * @brief Represents CADFace node instances.
     */

    /**
     * @var cad_face_node::cad_face_metatype
     *
     * @brief Class object for CADFace nodes.
     */

    /**
     * @var abstract_node<self_t>::exposedfield<sfstring> cad_face_node::name_
     *
     * @brief name exposedField
     */

    /**
     * @var abstract_node<self_t>::exposedfield<sfnode> cad_face_node::shape_
     *
     * @brief shape exposedField
     */

    /**
     * @brief Get the children in the scene graph.
     *
     * @return the child nodes in the scene graph.
     */
    const std::vector<boost::intrusive_ptr<node> > &
    cad_face_node::do_children() const OPENVRML_NOTHROW
    {
        cad_face_node * me = const_cast<cad_face_node *>(this);
        //if there is no child
        if (shape_.sfnode::value().get() == NULL)
            me->children_.clear();
        //if we have not set the child, or the child has changed.
        else if (this->children_.empty() || this->children_[0] != shape_.sfnode::value())
            me->children_.assign(1, shape_.sfnode::value());

        return children_;
    }

    /**
     * @brief Determine whether the node has been modified.
     *
     * @return @c true if the node or it child has been modified
     *      @c false otherwise.
     */
    bool cad_face_node::modified() const
    {
        if (this->node::modified()) { return true; }
        if (shape_.sfnode::value().get() == NULL) { return false; }
        return shape_.sfnode::value()->modified();
    }

    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    cad_face_node::do_bounding_volume() const
    {
        static bounding_sphere empty_volume;

        //get the child as a bounded_volume_node
        bounded_volume_node * node =
            node_cast<bounded_volume_node *>(shape_.sfnode::value().get());

        //return empty if there is not child
        if (node == NULL)
            return empty_volume;

        //return the child's bounded volume
        return node->bounding_volume();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    cad_face_node::
    cad_face_node(const node_type & type,
                  const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        child_node(type, scope),
        abstract_node<self_t>(type, scope),
        grouping_node(type, scope),
        name_(*this),
        shape_(*this)
    {}

    /**
     * @brief Destroy.
     */
    cad_face_node::~cad_face_node() OPENVRML_NOTHROW
    {}

    /**
     * @class quad_set_node
     *
     * @brief Represents IndexedQuadSet node instances.
     */

    /**
     * @var quad_set_node::indexed_quad_set_metatype
     *
     * @brief Class object for IndexedQuadSet nodes.
     */

    /**
     * @var quad_set_node::set_index_
     *
     * @brief set_index eventIn
     */

    /**
     * @var quad_set_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var quad_set_node::coord_
     *
     * @brief coord exposedField
     */

    /**
     * @var quad_set_node::normal_
     *
     * @brief normal exposedField
     */

    /**
     * @var quad_set_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var quad_set_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var quad_set_node::color_per_vertex_
     *
     * @brief color_per_vertex field
     */

    /**
     * @var quad_set_node::normal_per_vertex_
     *
     * @brief normal_per_vertex field
     */

    /**
     * @var quad_set_node::solid_
     *
     * @brief solid field
     */

    /**
     * @var quad_set_node::index_
     *
     * @brief index field
     */

    quad_set_node::set_index_listener::
    set_index_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfint32_listener(node)
    {}

    quad_set_node::set_index_listener::
    ~set_index_listener() OPENVRML_NOTHROW
    {}

    void quad_set_node::set_index_listener::
    do_process_event(const mfint32 & /* fraction */, const double /* timestamp */)
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
    quad_set_node::
    quad_set_node(const node_type & type,
                  const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        set_index_listener_(*this),
        color_(*this),
        coord_(*this),
        normal_(*this),
        tex_coord_(*this),
        ccw_(true),
        color_per_vertex_(true),
        normal_per_vertex_(true),
        solid_(true)
    {}

    /**
     * @brief Destroy.
     */
    quad_set_node::~quad_set_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    quad_set_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<quad_set_node *>(this)->recalc_bsphere();
        }
        return this->bsphere;
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void quad_set_node::recalc_bsphere()
    {
        // take the bvolume of all the points. technically, we should figure
        // out just which points are used by the index and just use those,
        // but for a first pass this is fine (also: if we do it this way
        // then we don't have to update the bvolume when the index
        // changes). motto: always do it the simple way first...
        //
        openvrml::coordinate_node * const coordinateNode =
            node_cast<openvrml::coordinate_node *>(
                this->coord_.sfnode::value().get());
        if (coordinateNode) {
            const std::vector<vec3f> & coord = coordinateNode->point();
            this->bsphere = bounding_sphere();
            this->bsphere.enclose(coord);
        }
        this->bounding_volume_dirty(false);
    }

    const color_node * quad_set_node::color() const OPENVRML_NOTHROW
    {
        return node_cast<color_node *>(color_.sfnode::value().get());
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
    quad_set_node::
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
    bool quad_set_node::modified() const
    {
        return this->node::modified()
            || (this->color_.sfnode::value()
                && this->color_.sfnode::value()->modified())
            || (this->coord_.sfnode::value()
                && this->coord_.sfnode::value()->modified())
            || (this->normal_.sfnode::value()
                && this->normal_.sfnode::value()->modified())
            || (this->tex_coord_.sfnode::value()
                && this->tex_coord_.sfnode::value()->modified());
    }
}
