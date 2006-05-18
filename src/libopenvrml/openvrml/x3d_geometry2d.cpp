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
     * @brief Class object for Arc2D nodes.
     */
    class OPENVRML_LOCAL arc2d_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit arc2d_metatype(openvrml::browser & browser);
        virtual ~arc2d_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for ArcClose2D nodes.
     */
    class OPENVRML_LOCAL arc_close2d_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit arc_close2d_metatype(openvrml::browser & browser);
        virtual ~arc_close2d_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for Circle2D nodes.
     */
    class OPENVRML_LOCAL circle2d_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit circle2d_metatype(openvrml::browser & browser);
        virtual ~circle2d_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for Disk2D nodes.
     */
    class OPENVRML_LOCAL disk2d_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit disk2d_metatype(openvrml::browser & browser);
        virtual ~disk2d_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for Polyline2D nodes.
     */
    class OPENVRML_LOCAL polyline2d_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit polyline2d_metatype(openvrml::browser & browser);
        virtual ~polyline2d_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for Polypoint2D nodes.
     */
    class OPENVRML_LOCAL polypoint2d_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit polypoint2d_metatype(openvrml::browser & browser);
        virtual ~polypoint2d_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for Rectangle2D nodes.
     */
    class OPENVRML_LOCAL rectangle2d_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit rectangle2d_metatype(openvrml::browser & browser);
        virtual ~rectangle2d_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for TriangleSet2D nodes.
     */
    class OPENVRML_LOCAL triangle_set2d_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit triangle_set2d_metatype(openvrml::browser & browser);
        virtual ~triangle_set2d_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            OPENVRML_THROW2(unsupported_interface, std::bad_alloc);
    };
}

void register_geometry2d_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    b.add_node_metatype(arc2d_metatype::id,
                        shared_ptr<node_metatype>(new arc2d_metatype(b)));
    b.add_node_metatype(arc_close2d_metatype::id,
                        shared_ptr<node_metatype>(new arc_close2d_metatype(b)));
    b.add_node_metatype(circle2d_metatype::id,
                        shared_ptr<node_metatype>(new circle2d_metatype(b)));
    b.add_node_metatype(disk2d_metatype::id,
                        shared_ptr<node_metatype>(new disk2d_metatype(b)));
    b.add_node_metatype(polyline2d_metatype::id,
                        shared_ptr<node_metatype>(new polyline2d_metatype(b)));
    b.add_node_metatype(polypoint2d_metatype::id,
                        shared_ptr<node_metatype>(new polypoint2d_metatype(b)));
    b.add_node_metatype(rectangle2d_metatype::id,
                        shared_ptr<node_metatype>(new rectangle2d_metatype(b)));
    b.add_node_metatype(triangle_set2d_metatype::id,
                        shared_ptr<node_metatype>(new triangle_set2d_metatype(b)));
}

namespace {

    using namespace openvrml_;

    class OPENVRML_LOCAL arc2d_node : public abstract_node<arc2d_node>,
                                      public geometry_node,
                                      public child_node {
        friend class arc2d_metatype;

        sffloat end_angle_;
        sffloat radius_;
        sffloat start_angle_;

    public:
        arc2d_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~arc2d_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };


    class OPENVRML_LOCAL arc_close2d_node :
        public abstract_node<arc_close2d_node>,
        public geometry_node,
        public child_node {

        friend class arc_close2d_metatype;

        sfstring closure_type_;
        sffloat end_angle_;
        sffloat radius_;
        sfbool solid_;
        sffloat start_angle_;

    public:
        arc_close2d_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~arc_close2d_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };


    class OPENVRML_LOCAL circle2d_node : public abstract_node<circle2d_node>,
                                         public geometry_node,
                                         public child_node {
        friend class circle2d_metatype;

        sffloat radius_;

    public:
        circle2d_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~circle2d_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };


    class OPENVRML_LOCAL disk2d_node : public abstract_node<disk2d_node>,
                                       public geometry_node,
                                       public child_node {
        friend class disk2d_metatype;

        sffloat inner_radius_;
        sffloat outer_radius_;
        sfbool solid_;

    public:
        disk2d_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~disk2d_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };


    class OPENVRML_LOCAL polyline2d_node :
        public abstract_node<polyline2d_node>,
        public geometry_node,
        public child_node {

        friend class polyline2d_metatype;

        mfvec2f line_segments_;

    public:
        polyline2d_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~polyline2d_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    class OPENVRML_LOCAL polypoint2d_node :
        public abstract_node<polypoint2d_node>,
        public geometry_node,
        public child_node {

        friend class polypoint2d_metatype;

        mfvec2f point_;

    public:
        polypoint2d_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~polypoint2d_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    class OPENVRML_LOCAL rectangle2d_node :
        public abstract_node<rectangle2d_node>,
        public geometry_node,
        public child_node {

        friend class rectangle2d_metatype;

        sfvec2f size_;
        sfbool solid_;

    public:
        rectangle2d_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~rectangle2d_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };

    class OPENVRML_LOCAL triangle_set2d_node :
        public abstract_node<triangle_set2d_node>,
        public geometry_node,
        public child_node {

        friend class triangle_set2d_metatype;

        exposedfield<mfvec2f> vertices_;
        sfbool solid_;

    public:
        triangle_set2d_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~triangle_set2d_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
    };


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const arc2d_metatype::id = "urn:X-openvrml:node:Arc2D";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c arc2d_metatype.
     */
    arc2d_metatype::arc2d_metatype(openvrml::browser & browser):
        node_metatype(arc2d_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    arc2d_metatype::~arc2d_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating Arc2D nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by arc2d_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    arc2d_metatype::do_create_type(const std::string & id,
                                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 4> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "endAngle"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "radius"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "startAngle")
        };
        typedef node_type_impl<arc2d_node> node_type_t;

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
                        abstract_node<arc2d_node>::exposedfield<sfnode> >(
                            &arc2d_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<arc2d_node>::exposedfield<sfnode> >(
                            &arc2d_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<arc2d_node>::exposedfield<sfnode> >(
                            &arc2d_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sffloat>(
                            &arc2d_node::end_angle_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sffloat>(
                            &arc2d_node::radius_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sffloat>(
                            &arc2d_node::start_angle_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @class arc2d_node
     *
     * @brief Represents Arc2D node instances.
     */

    /**
     * @var arc2d_node::arc2d_metatype
     *
     * @brief Class object for Arc2D nodes.
     */

    /**
     * @var arc2d_node::end_angle_
     *
     * @brief end_angle field
     */

    /**
     * @var arc2d_node::radius_
     *
     * @brief radius field
     */

    /**
     * @var arc2d_node::start_angle_
     *
     * @brief start_angle field
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
    arc2d_node::
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
    bool arc2d_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    arc2d_node::arc2d_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
      node(type, scope),
      bounded_volume_node(type, scope),
      abstract_node<self_t>(type, scope),
      geometry_node(type, scope),
      child_node(type, scope),
      end_angle_(1.57079633f),
      radius_(1)
    {}

    /**
     * @brief Destroy.
     */
    arc2d_node::~arc2d_node() OPENVRML_NOTHROW
    {}


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const arc_close2d_metatype::id =
        "urn:X-openvrml:node:ArcClose2D";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c arc_close2d_metatype.
     */
    arc_close2d_metatype::arc_close2d_metatype(openvrml::browser & browser):
        node_metatype(arc_close2d_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    arc_close2d_metatype::~arc_close2d_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating ArcClose2D nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by arc_close2d_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    arc_close2d_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 6> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::field_id,
                           field_value::sfstring_id,
                           "closureType"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "endAngle"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "radius"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "startAngle")
        };
        typedef node_type_impl<arc_close2d_node> node_type_t;

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
                        abstract_node<arc_close2d_node>::exposedfield<sfnode> >(
                            &arc_close2d_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<arc_close2d_node>::exposedfield<sfnode> >(
                            &arc_close2d_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<arc_close2d_node>::exposedfield<sfnode> >(
                            &arc_close2d_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfstring>(
                            &arc_close2d_node::closure_type_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sffloat>(
                            &arc_close2d_node::end_angle_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sffloat>(
                            &arc_close2d_node::radius_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &arc_close2d_node::solid_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sffloat>(
                            &arc_close2d_node::start_angle_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @class arc_close2d_node
     *
     * @brief Represents ArcClose2D node instances.
     */

    /**
     * @var arc_close2d_node::arc_close2d_metatype
     *
     * @brief Class object for ArcClose2D nodes.
     */

    /**
     * @var arc_close2d_node::closure_type_
     *
     * @brief closure_type field
     */

    /**
     * @var arc_close2d_node::end_angle_
     *
     * @brief end_angle field
     */

    /**
     * @var arc_close2d_node::radius_
     *
     * @brief radius field
     */

    /**
     * @var arc_close2d_node::solid_
     *
     * @brief solid field
     */

    /**
     * @var arc_close2d_node::start_angle_
     *
     * @brief start_angle field
     */


    /**
     * @brief Insert this geometry into @p viewer's display list.
     *
     * @param viewer    a @c viewer.
     * @param context   the rendering context.
     *
     * @todo Implement this!
     */
    openvrml::viewer::object_t
    arc_close2d_node::
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
    bool arc_close2d_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    arc_close2d_node::
    arc_close2d_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope):
      node(type, scope),
      bounded_volume_node(type, scope),
      abstract_node<self_t>(type, scope),
      geometry_node(type, scope),
      child_node(type, scope),
      closure_type_("PIE"),
      end_angle_(1.57079633f),
      radius_(1)
    {}

    /**
     * @brief Destroy.
     */
    arc_close2d_node::~arc_close2d_node() OPENVRML_NOTHROW
    {}


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const circle2d_metatype::id = "urn:X-openvrml:node:Circle2D";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c circle2d_metatype.
     */
    circle2d_metatype::circle2d_metatype(openvrml::browser & browser):
        node_metatype(circle2d_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    circle2d_metatype::~circle2d_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type capable of creating Circle2D nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by circle2d_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    circle2d_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 2> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "radius")
        };
        typedef node_type_impl<circle2d_node> node_type_t;

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
                        abstract_node<circle2d_node>::exposedfield<sfnode> >(
                            &circle2d_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<circle2d_node>::exposedfield<sfnode> >(
                            &circle2d_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<circle2d_node>::exposedfield<sfnode> >(
                            &circle2d_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sffloat>(
                            &circle2d_node::radius_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @class circle2d_node
     *
     * @brief Represents Circle2D node instances.
     */

    /**
     * @var circle2d_node::Circle2D_metatype
     *
     * @brief Class object for Circle2D nodes.
     */

    /**
     * @var circle2d_node::radius_
     *
     * @brief radius field
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
    circle2d_node::
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
    bool circle2d_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    circle2d_node::
    circle2d_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
      node(type, scope),
      bounded_volume_node(type, scope),
      abstract_node<self_t>(type, scope),
      geometry_node(type, scope),
      child_node(type, scope),
      radius_(1)
    {}

    /**
     * @brief Destroy.
     */
    circle2d_node::~circle2d_node() OPENVRML_NOTHROW
    {}


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const disk2d_metatype::id = "urn:X-openvrml:node:Disk2D";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c disk2d_metatype.
     */
    disk2d_metatype::disk2d_metatype(openvrml::browser & browser):
        node_metatype(disk2d_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    disk2d_metatype::~disk2d_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type capable of creating Disk2D nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by disk2d_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    disk2d_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 4> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "innerRadius"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "outerRadius"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid")
        };
        typedef node_type_impl<disk2d_node> node_type_t;

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
                        abstract_node<disk2d_node>::exposedfield<sfnode> >(
                            &disk2d_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<disk2d_node>::exposedfield<sfnode> >(
                            &disk2d_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<disk2d_node>::exposedfield<sfnode> >(
                            &disk2d_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sffloat>(
                            &disk2d_node::inner_radius_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sffloat>(
                            &disk2d_node::outer_radius_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &disk2d_node::solid_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @class disk2d_node
     *
     * @brief Represents Disk2D node instances.
     */

    /**
     * @var disk2d_node::disk2d_metatype
     *
     * @brief Class object for Disk2D nodes.
     */

    /**
     * @var disk2d_node::inner_radius_
     *
     * @brief inner_radius field
     */

    /**
     * @var disk2d_node::outer_radius_
     *
     * @brief outer_radius field
     */

    /**
     * @var disk2d_node::solid_
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
    disk2d_node::
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
    bool disk2d_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    disk2d_node::
    disk2d_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
      node(type, scope),
      bounded_volume_node(type, scope),
      abstract_node<self_t>(type, scope),
      geometry_node(type, scope),
      child_node(type, scope),
      outer_radius_(1)
    {}

    /**
     * @brief Destroy.
     */
    disk2d_node::~disk2d_node() OPENVRML_NOTHROW
    {}


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const polyline2d_metatype::id =
        "urn:X-openvrml:node:Polyline2D";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c polyline2d_metatype.
     */
    polyline2d_metatype::polyline2d_metatype(openvrml::browser & browser):
        node_metatype(polyline2d_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    polyline2d_metatype::~polyline2d_metatype() OPENVRML_NOTHROW
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
     *                                  not supported by polyline2d_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    polyline2d_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
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
     * @brief @c node_metatype identifier.
     */
    const char * const polypoint2d_metatype::id =
        "urn:X-openvrml:node:Polypoint2D";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c polypoint2d_metatype.
     */
    polypoint2d_metatype::polypoint2d_metatype(openvrml::browser & browser):
        node_metatype(polypoint2d_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    polypoint2d_metatype::~polypoint2d_metatype() OPENVRML_NOTHROW
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
     *                                  not supported by polypoint2d_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    polypoint2d_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
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
     * @brief @c node_metatype identifier.
     */
    const char * const rectangle2d_metatype::id =
        "urn:X-openvrml:node:Rectangle2D";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c rectangle2d_metatype.
     */
    rectangle2d_metatype::rectangle2d_metatype(openvrml::browser & browser):
        node_metatype(rectangle2d_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    rectangle2d_metatype::~rectangle2d_metatype() OPENVRML_NOTHROW
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
     *                                  not supported by rectangle2d_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    rectangle2d_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        OPENVRML_THROW2(unsupported_interface, std::bad_alloc)
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
     * @brief @c node_metatype identifier.
     */
    const char * const triangle_set2d_metatype::id =
        "urn:X-openvrml:node:TriangleSet2D";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c triangle_set2d_metatype.
     */
    triangle_set2d_metatype::triangle_set2d_metatype(openvrml::browser & browser):
        node_metatype(triangle_set2d_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    triangle_set2d_metatype::~triangle_set2d_metatype() OPENVRML_NOTHROW
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
     *                                  not supported by triangle_set2d_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    triangle_set2d_metatype::
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
     * @var polyline2d_node::Polyline2D_metatype
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
    polyline2d_node::~polyline2d_node() OPENVRML_NOTHROW
    {}


    /**
     * @class polypoint2d_node
     *
     * @brief Represents Polypoint2D node instances.
     */

    /**
     * @var polypoint2d_node::polypoint2d_metatype
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
     * @param viewer    a @c viewer.
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
     * @param scope the scope to which the node belongs.
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
    polypoint2d_node::~polypoint2d_node() OPENVRML_NOTHROW
    {}


    /**
     * @class rectangle2d_node
     *
     * @brief Represents Rectangle2D node instances.
     */

    /**
     * @var rectangle2d_node::rectangle2d_metatype
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
     * @param viewer    a @c viewer.
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
    rectangle2d_node::~rectangle2d_node() OPENVRML_NOTHROW
    {}


    /**
     * @class triangle_set2d_node
     *
     * @brief Represents TriangleSet2D node instances.
     */

    /**
     * @var triangle_set2d_node::triangle_set2d_metatype
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
    triangle_set2d_node::~triangle_set2d_node() OPENVRML_NOTHROW
    {}
}
