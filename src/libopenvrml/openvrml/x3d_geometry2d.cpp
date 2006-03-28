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
# include "x3d_geometry2d.h"
    
using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;

namespace {

    /**
     * @brief Class object for Polyline2D nodes.
     */
    class OPENVRML_LOCAL polyline2d_class : public node_class {
    public:
        explicit polyline2d_class(openvrml::browser & browser);
        virtual ~polyline2d_class() throw ();

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for Polypoint2D nodes.
     */
    class OPENVRML_LOCAL polypoint2d_class : public node_class {
    public:
        explicit polypoint2d_class(openvrml::browser & browser);
        virtual ~polypoint2d_class() throw ();

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for Rectangle2D nodes.
     */
    class OPENVRML_LOCAL rectangle2d_class : public node_class {
    public:
        explicit rectangle2d_class(openvrml::browser & browser);
        virtual ~rectangle2d_class() throw ();

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for TriangleSet2D nodes.
     */
    class OPENVRML_LOCAL triangle_set2d_class : public node_class {
    public:
        explicit triangle_set2d_class(openvrml::browser & browser);
        virtual ~triangle_set2d_class() throw ();

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };
} 

void register_geometry2d_node_classes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_class;
    b.add_node_class("urn:X-openvrml:node:Polyline2D",
                     shared_ptr<node_class>(new polyline2d_class(b)));
    b.add_node_class("urn:X-openvrml:node:Polypoint2D",
                     shared_ptr<node_class>(new polypoint2d_class(b)));
    b.add_node_class("urn:X-openvrml:node:Rectangle2D",
                     shared_ptr<node_class>(new rectangle2d_class(b)));
    b.add_node_class("urn:X-openvrml:node:TriangleSet2D",
                     shared_ptr<node_class>(new triangle_set2d_class(b)));
}

namespace {

    using namespace openvrml_;

    class OPENVRML_LOCAL polyline2d_node :
        public abstract_node<polyline2d_node>,
        public geometry_node,
        public child_node {

        friend class polyline2d_class;

        mfvec2f line_segments_;

    public:
        polyline2d_node(const node_type & type, 
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~polyline2d_node() throw ();

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    class OPENVRML_LOCAL polypoint2d_node :
        public abstract_node<polypoint2d_node>,
        public geometry_node,
        public child_node {

        friend class polypoint2d_class;

        mfvec2f point_;

    public:
        polypoint2d_node(const node_type & type, 
                         const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~polypoint2d_node() throw ();

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    class OPENVRML_LOCAL rectangle2d_node :
        public abstract_node<rectangle2d_node>,
        public geometry_node,
        public child_node {

        friend class rectangle2d_class;

        sfvec2f size_;
        sfbool solid_;

    public:
        rectangle2d_node(const node_type & type, 
                         const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~rectangle2d_node() throw ();

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    class OPENVRML_LOCAL triangle_set2d_node :
        public abstract_node<triangle_set2d_node>,
        public geometry_node,
        public child_node {

        friend class triangle_set2d_class;

        exposedfield<mfvec2f> vertices_;
        sfbool solid_;

    public:
        triangle_set2d_node(const node_type & type, 
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~triangle_set2d_node() throw ();

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };


    /**
     * @brief Construct.
     *
     * @param browser the browser associated with this polyline2d_class.
     */
    polyline2d_class::polyline2d_class(openvrml::browser & browser):
        node_class(browser)
    {}

    /**
     * @brief Destroy.
     */
    polyline2d_class::~polyline2d_class() throw ()
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating Polyline2D nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by polyline2d_class.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    polyline2d_class::do_create_type(const std::string & id,
                                     const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 2> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::field_id,
                           field_value::mfvec2f_id,
                           "lineSegments")
        };    
        typedef node_type_impl<polyline2d_node> node_type_t;

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
                        abstract_node<polyline2d_node>::exposedfield<sfnode> >(
                            &polyline2d_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<polyline2d_node>::exposedfield<sfnode> >(
                            &polyline2d_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<polyline2d_node>::exposedfield<sfnode> >(
                            &polyline2d_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfvec2f>(
                            &polyline2d_node::line_segments_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }        
        return type;
    }

    /**
     * @brief Construct.
     *
     * @param browser the browser associated with this polypoint2d_class.
     */
    polypoint2d_class::polypoint2d_class(openvrml::browser & browser):
        node_class(browser)
    {}

    /**
     * @brief Destroy.
     */
    polypoint2d_class::~polypoint2d_class() throw ()
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating Polypoint2D nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by polypoint2d_class.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    polypoint2d_class::do_create_type(const std::string & id,
                                      const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 2> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::field_id,
                           field_value::mfvec2f_id,
                           "point")
        };    
        typedef node_type_impl<polypoint2d_node> node_type_t;

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
                        abstract_node<polypoint2d_node>::exposedfield<sfnode> >(
                            &polypoint2d_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<polypoint2d_node>::exposedfield<sfnode> >(
                            &polypoint2d_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<polypoint2d_node>::exposedfield<sfnode> >(
                            &polypoint2d_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfvec2f>(
                            &polypoint2d_node::point_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }        
        return type;
    }

    /**
     * @brief Construct.
     *
     * @param browser the browser associated with this rectangle2d_class.
     */
    rectangle2d_class::rectangle2d_class(openvrml::browser & browser):
        node_class(browser)
    {}

    /**
     * @brief Destroy.
     */
    rectangle2d_class::~rectangle2d_class() throw ()
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating Rectangle2D nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by rectangle2d_class.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    rectangle2d_class::do_create_type(const std::string & id,
                                      const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 3> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::field_id,
                           field_value::sfvec2f_id,
                           "size"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid")
        };    
        typedef node_type_impl<rectangle2d_node> node_type_t;

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
                        abstract_node<rectangle2d_node>::exposedfield<sfnode> >(
                            &rectangle2d_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<rectangle2d_node>::exposedfield<sfnode> >(
                            &rectangle2d_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<rectangle2d_node>::exposedfield<sfnode> >(
                            &rectangle2d_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfvec2f>(
                            &rectangle2d_node::size_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &rectangle2d_node::solid_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }        
        return type;
    }

    /**
     * @brief Construct.
     *
     * @param browser the browser associated with this triangle_set2d_class.
     */
    triangle_set2d_class::triangle_set2d_class(openvrml::browser & browser):
        node_class(browser)
    {}

    /**
     * @brief Destroy.
     */
    triangle_set2d_class::~triangle_set2d_class() throw ()
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating TriangleSet2D nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by triangle_set2d_class.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    triangle_set2d_class::do_create_type(const std::string & id,
                                         const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 3> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec2f_id,
                           "vertices"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid")
        };    
        typedef node_type_impl<triangle_set2d_node> node_type_t;

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
                        abstract_node<triangle_set2d_node>::exposedfield<sfnode> >(
                            &triangle_set2d_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_set2d_node>::exposedfield<sfnode> >(
                            &triangle_set2d_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_set2d_node>::exposedfield<sfnode> >(
                            &triangle_set2d_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_set2d_node>::exposedfield<mfvec2f> >(
                            &triangle_set2d_node::vertices_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_set2d_node>::exposedfield<mfvec2f> >(
                            &triangle_set2d_node::vertices_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_set2d_node>::exposedfield<mfvec2f> >(
                            &triangle_set2d_node::vertices_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_set2d_node::solid_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }        
        return type;
    }


    /**
     * @class polyline2d_node
     *
     * @brief Represents Polyline2D node instances.
     */

    /**
     * @var polyline2d_node::Polyline2D_class
     *
     * @brief Class object for Polyline2D nodes.
     */

    /**
     * @var polyline2d_node::line_segments_
     *
     * @brief line_segments field
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
    polyline2d_node::
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
    bool polyline2d_node::modified() const
    {
        return this->node::modified();
    }
    
    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    polyline2d_node::
    polyline2d_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope)
    {}

    /**
     * @brief Destroy.
     */
    polyline2d_node::~polyline2d_node() throw ()
    {}


    /**
     * @class polypoint2d_node
     *
     * @brief Represents Polypoint2D node instances.
     */

    /**
     * @var polypoint2d_node::Polypoint2D_class
     *
     * @brief Class object for Polypoint2D nodes.
     */

    /**
     * @var polypoint2d_node::point_
     *
     * @brief point field
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
    polypoint2d_node::
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
    bool polypoint2d_node::modified() const
    {
        return this->node::modified();
    }
    
    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    polypoint2d_node::
    polypoint2d_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope)
    {}

    /**
     * @brief Destroy.
     */
    polypoint2d_node::~polypoint2d_node() throw ()
    {}


    /**
     * @class rectangle2d_node
     *
     * @brief Represents Rectangle2D node instances.
     */

    /**
     * @var rectangle2d_node::Rectangle2D_class
     *
     * @brief Class object for Rectangle2D nodes.
     */

    /**
     * @var rectangle2d_node::size_
     *
     * @brief size field
     */

    /**
     * @var rectangle2d_node::solid_
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
    rectangle2d_node::
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
    bool rectangle2d_node::modified() const
    {
        return this->node::modified();
    }
    
    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    rectangle2d_node::
    rectangle2d_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        size_(vec2f(2,2))
    {}

    /**
     * @brief Destroy.
     */
    rectangle2d_node::~rectangle2d_node() throw ()
    {}


    /**
     * @class triangle_set2d_node
     *
     * @brief Represents TriangleSet2D node instances.
     */

    /**
     * @var triangle_set2d_node::TriangleSet2D_class
     *
     * @brief Class object for TriangleSet2D nodes.
     */

    /**
     * @var triangle_set2d_node::vertices_
     *
     * @brief vertices exposedField
     */

    /**
     * @var triangle_set2d_node::solid_
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
    triangle_set2d_node::
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
    bool triangle_set2d_node::modified() const
    {
        return this->node::modified();
    }
    
    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    triangle_set2d_node::
    triangle_set2d_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        vertices_(*this)
    {}

    /**
     * @brief Destroy.
     */
    triangle_set2d_node::~triangle_set2d_node() throw ()
    {}
}
