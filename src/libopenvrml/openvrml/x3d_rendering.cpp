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
# include "x3d_rendering.h"

using namespace openvrml;
using namespace openvrml::node_impl_util;
using namespace std;


namespace {

    /**
     * @brief Class object for ColorRGBA nodes.
     */
    class OPENVRML_LOCAL color_rgba_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit color_rgba_metatype(openvrml::browser & browser);
        virtual ~color_rgba_metatype() throw ();

    private:
        virtual const boost::shared_ptr<node_type>
        do_create_type(const std::string & id,
                       const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for IndexedTriangleFanSet nodes.
     */
    class OPENVRML_LOCAL indexed_triangle_fan_set_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit indexed_triangle_fan_set_metatype(openvrml::browser & browser);
        virtual ~indexed_triangle_fan_set_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for IndexedTriangleSet nodes.
     */
    class OPENVRML_LOCAL indexed_triangle_set_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit indexed_triangle_set_metatype(openvrml::browser & browser);
        virtual ~indexed_triangle_set_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for IndexedTriangleStripSet nodes.
     */
    class OPENVRML_LOCAL indexed_triangle_strip_set_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit indexed_triangle_strip_set_metatype(openvrml::browser & browser);
        virtual ~indexed_triangle_strip_set_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for TriangleFanSet nodes.
     */
    class OPENVRML_LOCAL triangle_fan_set_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit triangle_fan_set_metatype(openvrml::browser & browser);
        virtual ~triangle_fan_set_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for TriangleSet nodes.
     */
    class OPENVRML_LOCAL triangle_set_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit triangle_set_metatype(openvrml::browser & browser);
        virtual ~triangle_set_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };


    /**
     * @brief Class object for TriangleStripSet nodes.
     */
    class OPENVRML_LOCAL triangle_strip_set_metatype : public node_metatype {
    public:
        static const char * const id;

        explicit triangle_strip_set_metatype(openvrml::browser & browser);
        virtual ~triangle_strip_set_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
            throw (unsupported_interface, std::bad_alloc);
    };
}

void register_rendering_node_metatypes(openvrml::browser & b)
{
    using boost::shared_ptr;
    using openvrml::node_metatype;
    b.add_node_metatype(color_rgba_metatype::id,
                        shared_ptr<node_metatype>(new color_rgba_metatype(b)));
    b.add_node_metatype(indexed_triangle_fan_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new indexed_triangle_fan_set_metatype(b)));
    b.add_node_metatype(indexed_triangle_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new indexed_triangle_set_metatype(b)));
    b.add_node_metatype(indexed_triangle_strip_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new indexed_triangle_strip_set_metatype(b)));
    b.add_node_metatype(triangle_fan_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new triangle_fan_set_metatype(b)));
    b.add_node_metatype(triangle_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new triangle_set_metatype(b)));
    b.add_node_metatype(triangle_strip_set_metatype::id,
                        shared_ptr<node_metatype>(
                            new triangle_strip_set_metatype(b)));
}

namespace {

    using namespace openvrml_;

    class OPENVRML_LOCAL color_rgba_node :
        public abstract_node<color_rgba_node>,
        public openvrml::color_rgba_node {

        friend class color_rgba_metatype;

        exposedfield<mfcolorrgba> color_;

    public:
        color_rgba_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~color_rgba_node() OPENVRML_NOTHROW;

        //
        // color_rgba_node implementation
        //
        const std::vector<openvrml::color_rgba> & color_rgba() const
            OPENVRML_NOTHROW;
    };

    class OPENVRML_LOCAL indexed_triangle_fan_set_node :
        public abstract_node<indexed_triangle_fan_set_node>,
        public geometry_node,
        public child_node {

        friend class indexed_triangle_fan_set_metatype;

        class set_index_listener : public event_listener_base<self_t>,
                                   public mfint32_listener {
        public:
            explicit set_index_listener(self_t & node);
            virtual ~set_index_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfint32 & fraction,
                                          double timestamp)
                throw (std::bad_alloc);
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
        indexed_triangle_fan_set_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~indexed_triangle_fan_set_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
        virtual const color_node * color() const OPENVRML_NOTHROW;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
        virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

        void recalc_bsphere();
    };

    class OPENVRML_LOCAL indexed_triangle_set_node :
        public abstract_node<indexed_triangle_set_node>,
        public geometry_node,
        public child_node {

        friend class indexed_triangle_set_metatype;

        class set_index_listener :
                public event_listener_base<self_t>,
                public mfint32_listener
        {
        public:
            explicit set_index_listener(self_t & node);
            virtual ~set_index_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfint32 & fraction,
                                          double timestamp)
                throw (std::bad_alloc);
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
        indexed_triangle_set_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~indexed_triangle_set_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
        virtual const color_node * color() const OPENVRML_NOTHROW;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
        virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

        void recalc_bsphere();
    };

    class OPENVRML_LOCAL indexed_triangle_strip_set_node :
        public abstract_node<indexed_triangle_strip_set_node>,
        public geometry_node,
        public child_node {

        friend class indexed_triangle_strip_set_metatype;

        class set_index_listener : public event_listener_base<self_t>,
                                   public mfint32_listener {
        public:
            explicit set_index_listener(self_t & node);
            virtual ~set_index_listener() OPENVRML_NOTHROW;

        private:
            virtual void do_process_event(const mfint32 & fraction,
                                          double timestamp)
                throw (std::bad_alloc);
        };

        set_index_listener set_index_listener_;
        exposedfield<sfnode> color_;
        exposedfield<sfnode> coord_;
        exposedfield<sffloat> crease_angle_;
        exposedfield<sfnode> normal_;
        exposedfield<sfnode> tex_coord_;
        sfbool ccw_;
        sfbool color_per_vertex_;
        sfbool normal_per_vertex_;
        sfbool solid_;
        mfint32 index_;
        bounding_sphere bsphere;

    public:
        indexed_triangle_strip_set_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~indexed_triangle_strip_set_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
        virtual const color_node * color() const OPENVRML_NOTHROW;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
        virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

        void recalc_bsphere();
    };

    class OPENVRML_LOCAL triangle_fan_set_node :
        public abstract_node<triangle_fan_set_node>,
        public geometry_node,
        public child_node {

        friend class triangle_fan_set_metatype;

        exposedfield<sfnode> color_;
        exposedfield<sfnode> coord_;
        exposedfield<mfint32> fan_count_;
        exposedfield<sfnode> normal_;
        exposedfield<sfnode> tex_coord_;
        sfbool ccw_;
        sfbool color_per_vertex_;
        sfbool normal_per_vertex_;
        sfbool solid_;
        bounding_sphere bsphere;

    public:
        triangle_fan_set_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~triangle_fan_set_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
        virtual const color_node * color() const OPENVRML_NOTHROW;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
        virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

        void recalc_bsphere();
    };

    class OPENVRML_LOCAL triangle_set_node :
        public abstract_node<triangle_set_node>,
        public geometry_node,
        public child_node {

        friend class triangle_set_metatype;

        exposedfield<sfnode> color_;
        exposedfield<sfnode> coord_;
        exposedfield<sfnode> normal_;
        exposedfield<sfnode> tex_coord_;
        sfbool ccw_;
        sfbool color_per_vertex_;
        sfbool normal_per_vertex_;
        sfbool solid_;
        bounding_sphere bsphere;

    public:
        triangle_set_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~triangle_set_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
        virtual const color_node * color() const OPENVRML_NOTHROW;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
        virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

        void recalc_bsphere();
    };

    class OPENVRML_LOCAL triangle_strip_set_node :
        public abstract_node<triangle_strip_set_node>,
        public geometry_node,
        public child_node {

        friend class triangle_strip_set_metatype;

        exposedfield<sfnode> color_;
        exposedfield<sfnode> coord_;
        exposedfield<sfnode> normal_;
        exposedfield<mfint32> strip_count_;
        exposedfield<sfnode> tex_coord_;
        sfbool ccw_;
        sfbool color_per_vertex_;
        sfbool normal_per_vertex_;
        sfbool solid_;
        bounding_sphere bsphere;

    public:
        triangle_strip_set_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~triangle_strip_set_node() OPENVRML_NOTHROW;

        virtual bool modified() const;
        virtual const color_node * color() const OPENVRML_NOTHROW;
    private:
        virtual viewer::object_t do_render_geometry(openvrml::viewer & viewer,
                                                    rendering_context context);
        virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

        void recalc_bsphere();
    };


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const color_rgba_metatype::id = "urn:X-openvrml:node:ColorRGBA";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this @c color_rgba_metatype.
     */
    color_rgba_metatype::color_rgba_metatype(openvrml::browser & browser):
        node_metatype(color_rgba_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    color_rgba_metatype::~color_rgba_metatype() throw ()
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating ColorRGBA nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by color_rgba_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    color_rgba_metatype::do_create_type(const std::string & id,
                                 const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 2> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfcolorrgba_id,
                           "color")
        };    
        typedef node_type_impl<color_rgba_node> node_type_t;

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
                        abstract_node<color_rgba_node>::exposedfield<sfnode> >(
                            &color_rgba_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<color_rgba_node>::exposedfield<sfnode> >(
                            &color_rgba_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<color_rgba_node>::exposedfield<sfnode> >(
                            &color_rgba_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<color_rgba_node>::exposedfield<mfcolorrgba> >(
                            &color_rgba_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<color_rgba_node>::exposedfield<mfcolorrgba> >(
                            &color_rgba_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<color_rgba_node>::exposedfield<mfcolorrgba> >(
                            &color_rgba_node::color_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }        
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const indexed_triangle_fan_set_metatype::id =
        "urn:X-openvrml:node:IndexedTriangleFanSet";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c indexed_triangle_fan_set_metatype.
     */
    indexed_triangle_fan_set_metatype::
    indexed_triangle_fan_set_metatype(openvrml::browser & browser):
        node_metatype(indexed_triangle_fan_set_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    indexed_triangle_fan_set_metatype::~indexed_triangle_fan_set_metatype()
        OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating IndexedTriangleFanSet nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by indexed_triangle_fan_set_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    indexed_triangle_fan_set_metatype::do_create_type(const std::string & id,
                                                   const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 11> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
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
        typedef node_type_impl<indexed_triangle_fan_set_node> node_type_t;

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
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        indexed_triangle_fan_set_node::set_index_listener>(
                            &indexed_triangle_fan_set_node::set_index_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::color_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::normal_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::normal_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::normal_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_fan_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_fan_set_node::tex_coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_fan_set_node::ccw_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_fan_set_node::color_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_fan_set_node::normal_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_fan_set_node::solid_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfint32>(
                            &indexed_triangle_fan_set_node::index_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const indexed_triangle_set_metatype::id =
        "urn:X-openvrml:node:IndexedTriangleSet";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c indexed_triangle_set_metatype.
     */
    indexed_triangle_set_metatype::
    indexed_triangle_set_metatype(openvrml::browser & browser):
        node_metatype(indexed_triangle_set_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    indexed_triangle_set_metatype::~indexed_triangle_set_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating IndexedTriangleSet nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by indexed_triangle_set_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    indexed_triangle_set_metatype::do_create_type(const std::string & id,
                                               const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 11> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
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
        typedef node_type_impl<indexed_triangle_set_node> node_type_t;

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
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        indexed_triangle_set_node::set_index_listener>(
                            &indexed_triangle_set_node::set_index_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::color_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::normal_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::normal_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::normal_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_set_node::tex_coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_set_node::ccw_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_set_node::color_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_set_node::normal_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_set_node::solid_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfint32>(
                            &indexed_triangle_set_node::index_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const indexed_triangle_strip_set_metatype::id =
        "urn:X-openvrml:node:IndexedTriangleStripSet";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c indexed_triangle_strip_set_metatype.
     */
    indexed_triangle_strip_set_metatype::
    indexed_triangle_strip_set_metatype(openvrml::browser & browser):
        node_metatype(indexed_triangle_strip_set_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    indexed_triangle_strip_set_metatype::~indexed_triangle_strip_set_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating IndexedTriangleStripSet nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by indexed_triangle_strip_set_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    indexed_triangle_strip_set_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 12> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
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
                           field_value::sffloat_id,
                           "creaseAngle"),
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
        typedef node_type_impl<indexed_triangle_strip_set_node> node_type_t;

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
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_eventin(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        indexed_triangle_strip_set_node::set_index_listener>(
                            &indexed_triangle_strip_set_node::set_index_listener_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::color_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sffloat> >(
                            &indexed_triangle_strip_set_node::crease_angle_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sffloat> >(
                            &indexed_triangle_strip_set_node::crease_angle_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sffloat> >(
                            &indexed_triangle_strip_set_node::crease_angle_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::normal_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::normal_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::normal_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<indexed_triangle_strip_set_node>::exposedfield<sfnode> >(
                            &indexed_triangle_strip_set_node::tex_coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_strip_set_node::ccw_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_strip_set_node::color_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_strip_set_node::normal_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &indexed_triangle_strip_set_node::solid_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<mfint32>(
                            &indexed_triangle_strip_set_node::index_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const triangle_fan_set_metatype::id =
        "urn:X-openvrml:node:TriangleFanSet";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c triangle_fan_set_metatype.
     */
    triangle_fan_set_metatype::triangle_fan_set_metatype(openvrml::browser & browser):
        node_metatype(triangle_fan_set_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    triangle_fan_set_metatype::~triangle_fan_set_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating TriangleFanSet nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by triangle_fan_set_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    triangle_fan_set_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 10> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "coord"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfint32_id,
                           "fanCount"),
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
                           "solid")
        };
        typedef node_type_impl<triangle_fan_set_node> node_type_t;

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
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::color_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<mfint32> >(
                            &triangle_fan_set_node::fan_count_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<mfint32> >(
                            &triangle_fan_set_node::fan_count_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<mfint32> >(
                            &triangle_fan_set_node::fan_count_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::normal_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::normal_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::normal_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_fan_set_node>::exposedfield<sfnode> >(
                            &triangle_fan_set_node::tex_coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_fan_set_node::ccw_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_fan_set_node::color_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_fan_set_node::normal_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_fan_set_node::solid_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @brief @c node_metatype identifier.
     */
    const char * const triangle_set_metatype::id =
        "urn:X-openvrml:node:TriangleSet";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c triangle_set_metatype.
     */
    triangle_set_metatype::triangle_set_metatype(openvrml::browser & browser):
        node_metatype(triangle_set_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    triangle_set_metatype::~triangle_set_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating TriangleSet nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by triangle_set_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    triangle_set_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 9> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "coord"),
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
                           "solid")
        };
        typedef node_type_impl<triangle_set_node> node_type_t;

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
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::color_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::normal_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::normal_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::normal_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_set_node>::exposedfield<sfnode> >(
                            &triangle_set_node::tex_coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_set_node::ccw_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_set_node::color_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_set_node::normal_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_set_node::solid_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }

    /**
     * @brief @c node_metatype identifier.
     */
    const char * const triangle_strip_set_metatype::id =
        "urn:X-openvrml:node:TriangleStripSet";

    /**
     * @brief Construct.
     *
     * @param browser the @c browser associated with this
     *                @c triangle_strip_set_metatype.
     */
    triangle_strip_set_metatype::triangle_strip_set_metatype(openvrml::browser & browser):
        node_metatype(triangle_strip_set_metatype::id, browser)
    {}

    /**
     * @brief Destroy.
     */
    triangle_strip_set_metatype::~triangle_strip_set_metatype() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a node_type.
     *
     * @param id            the name for the new node_type.
     * @param interfaces    the interfaces for the new node_type.
     *
     * @return a node_type_ptr to a node_type capable of creating TriangleStripSet nodes.
     *
     * @exception unsupported_interface if @p interfaces includes an interface
     *                                  not supported by triangle_strip_set_metatype.
     * @exception std::bad_alloc        if memory allocation fails.
     */
    const boost::shared_ptr<openvrml::node_type>
    triangle_strip_set_metatype::
    do_create_type(const std::string & id,
                   const node_interface_set & interfaces) const
        throw (unsupported_interface, std::bad_alloc)
    {
        typedef boost::array<node_interface, 10> supported_interfaces_t;
        static const supported_interfaces_t supported_interfaces = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "metadata"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "coord"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "normal"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfint32_id,
                           "stripCount"),
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
                           "solid")
        };
        typedef node_type_impl<triangle_strip_set_node> node_type_t;

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
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::metadata)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::metadata)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::metadata)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::color_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::color_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::color_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::normal_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::normal_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::normal_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<mfint32> >(
                            &triangle_strip_set_node::strip_count_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<mfint32> >(
                            &triangle_strip_set_node::strip_count_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<mfint32> >(
                            &triangle_strip_set_node::strip_count_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_exposedfield(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::event_listener_ptr_ptr(
                        new node_type_t::event_listener_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::tex_coord_)),
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::tex_coord_)),
                    node_type_t::event_emitter_ptr_ptr(
                        new node_type_t::event_emitter_ptr<
                        abstract_node<triangle_strip_set_node>::exposedfield<sfnode> >(
                            &triangle_strip_set_node::tex_coord_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_strip_set_node::ccw_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_strip_set_node::color_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_strip_set_node::normal_per_vertex_)));
            } else if (*interface == *++supported_interface) {
                the_node_type.add_field(
                    supported_interface->field_type,
                    supported_interface->id,
                    node_type_t::field_ptr_ptr(
                        new node_type_t::field_ptr<sfbool>(
                            &triangle_strip_set_node::solid_)));
            } else {
                throw unsupported_interface(*interface);
            }
        }
        return type;
    }


    /**
     * @class color_rgba_node
     *
     * @brief Represents ColorRGBA node instances.
     */

    /**
     * @var color_rgba_node::ColorRGBA_metatype
     *
     * @brief Class object for ColorRGBA nodes.
     */

    /**
     * @var color_rgba_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    color_rgba_node::
    color_rgba_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        abstract_node<self_t>(type, scope),
        openvrml::color_rgba_node(type, scope),
        color_(*this)
    {}

    /**
     * @brief Destroy.
     */
    color_rgba_node::~color_rgba_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the color_rgba array.
     *
     * @return the color array associated with the node.
     */
    const std::vector<openvrml::color_rgba> &
    color_rgba_node::color_rgba() const OPENVRML_NOTHROW
    {
        return this->color_.mfcolorrgba::value();
    }


    /**
     * @class indexed_triangle_fan_set_node
     *
     * @brief Represents IndexedTriangleFanSet node instances.
     */

    /**
     * @var indexed_triangle_fan_set_node::indexed_triangle_fan_set_metatype
     *
     * @brief Class object for IndexedTriangleFanSet nodes.
     */

    /**
     * @var indexed_triangle_fan_set_node::set_index_
     *
     * @brief set_index eventIn
     */

    /**
     * @var indexed_triangle_fan_set_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var indexed_triangle_fan_set_node::coord_
     *
     * @brief coord exposedField
     */

    /**
     * @var indexed_triangle_fan_set_node::normal_
     *
     * @brief normal exposedField
     */

    /**
     * @var indexed_triangle_fan_set_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var indexed_triangle_fan_set_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var indexed_triangle_fan_set_node::color_per_vertex_
     *
     * @brief color_per_vertex field
     */

    /**
     * @var indexed_triangle_fan_set_node::normal_per_vertex_
     *
     * @brief normal_per_vertex field
     */

    /**
     * @var indexed_triangle_fan_set_node::solid_
     *
     * @brief solid field
     */

    /**
     * @var indexed_triangle_fan_set_node::index_
     *
     * @brief index field
     */

    indexed_triangle_fan_set_node::set_index_listener::
    set_index_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfint32_listener(node)
    {}

    indexed_triangle_fan_set_node::set_index_listener::
    ~set_index_listener() OPENVRML_NOTHROW
    {}

    void indexed_triangle_fan_set_node::set_index_listener::
    do_process_event(const mfint32 & /* fraction */, const double /* timestamp */)
        throw (std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    indexed_triangle_fan_set_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<self_t *>(this)->recalc_bsphere();
        }
        return this->bsphere;
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void indexed_triangle_fan_set_node::recalc_bsphere()
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

    const color_node * indexed_triangle_fan_set_node::color() const OPENVRML_NOTHROW
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
    indexed_triangle_fan_set_node::
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
    bool indexed_triangle_fan_set_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    indexed_triangle_fan_set_node::
    indexed_triangle_fan_set_node(const node_type & type,
                                  const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
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
    indexed_triangle_fan_set_node::~indexed_triangle_fan_set_node() OPENVRML_NOTHROW
    {}


    /**
     * @class indexed_triangle_set_node
     *
     * @brief Represents IndexedTriangleSet node instances.
     */

    /**
     * @var indexed_triangle_set_node::indexed_triangle_set_metatype
     *
     * @brief Class object for IndexedTriangleSet nodes.
     */

    /**
     * @var indexed_triangle_set_node::set_index_
     *
     * @brief set_index eventIn
     */

    /**
     * @var indexed_triangle_set_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var indexed_triangle_set_node::coord_
     *
     * @brief coord exposedField
     */

    /**
     * @var indexed_triangle_set_node::normal_
     *
     * @brief normal exposedField
     */

    /**
     * @var indexed_triangle_set_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var indexed_triangle_set_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var indexed_triangle_set_node::color_per_vertex_
     *
     * @brief color_per_vertex field
     */

    /**
     * @var indexed_triangle_set_node::normal_per_vertex_
     *
     * @brief normal_per_vertex field
     */

    /**
     * @var indexed_triangle_set_node::solid_
     *
     * @brief solid field
     */

    /**
     * @var indexed_triangle_set_node::index_
     *
     * @brief index field
     */

    indexed_triangle_set_node::set_index_listener::
    set_index_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfint32_listener(node)
    {}

    indexed_triangle_set_node::set_index_listener::
    ~set_index_listener() OPENVRML_NOTHROW
    {}

    void indexed_triangle_set_node::set_index_listener::
    do_process_event(const mfint32 & /* fraction */, const double /* timestamp */)
        throw (std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    indexed_triangle_set_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<self_t *>(this)->recalc_bsphere();
        }
        return this->bsphere;
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void indexed_triangle_set_node::recalc_bsphere()
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

    const color_node * indexed_triangle_set_node::color() const OPENVRML_NOTHROW
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
    indexed_triangle_set_node::
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
    bool indexed_triangle_set_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    indexed_triangle_set_node::
    indexed_triangle_set_node(const node_type & type,
                              const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
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
    indexed_triangle_set_node::~indexed_triangle_set_node() OPENVRML_NOTHROW
    {}


    /**
     * @class indexed_triangle_strip_set_node
     *
     * @brief Represents IndexedTriangleStripSet node instances.
     */

    /**
     * @var indexed_triangle_strip_set_node::indexed_triangle_strip_set_metatype
     *
     * @brief Class object for IndexedTriangleStripSet nodes.
     */

    /**
     * @var indexed_triangle_strip_set_node::set_index_
     *
     * @brief set_index eventIn
     */

    /**
     * @var indexed_triangle_strip_set_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var indexed_triangle_strip_set_node::coord_
     *
     * @brief coord exposedField
     */

    /**
     * @var indexed_triangle_strip_set_node::crease_angle_
     *
     * @brief crease_angle exposedField
     */

    /**
     * @var indexed_triangle_strip_set_node::normal_
     *
     * @brief normal exposedField
     */

    /**
     * @var indexed_triangle_strip_set_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var indexed_triangle_strip_set_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var indexed_triangle_strip_set_node::color_per_vertex_
     *
     * @brief color_per_vertex field
     */

    /**
     * @var indexed_triangle_strip_set_node::normal_per_vertex_
     *
     * @brief normal_per_vertex field
     */

    /**
     * @var indexed_triangle_strip_set_node::solid_
     *
     * @brief solid field
     */

    /**
     * @var indexed_triangle_strip_set_node::index_
     *
     * @brief index field
     */

    indexed_triangle_strip_set_node::set_index_listener::
    set_index_listener(self_t & node):
        node_event_listener(node),
        event_listener_base<self_t>(node),
        mfint32_listener(node)
    {}

    indexed_triangle_strip_set_node::set_index_listener::
    ~set_index_listener() OPENVRML_NOTHROW
    {}

    void indexed_triangle_strip_set_node::set_index_listener::
    do_process_event(const mfint32 & /* fraction */, const double /* timestamp */)
        throw (std::bad_alloc)
    {
        //TODO: add logic here
    }


    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    indexed_triangle_strip_set_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<self_t *>(this)->recalc_bsphere();
        }
        return this->bsphere;
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void indexed_triangle_strip_set_node::recalc_bsphere()
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

    const color_node * indexed_triangle_strip_set_node::color() const OPENVRML_NOTHROW
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
    indexed_triangle_strip_set_node::
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
    bool indexed_triangle_strip_set_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    indexed_triangle_strip_set_node::
    indexed_triangle_strip_set_node(
        const node_type & type,
        const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        set_index_listener_(*this),
        color_(*this),
        coord_(*this),
        crease_angle_(*this),
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
    indexed_triangle_strip_set_node::~indexed_triangle_strip_set_node() OPENVRML_NOTHROW
    {}


    /**
     * @class triangle_fan_set_node
     *
     * @brief Represents TriangleFanSet node instances.
     */

    /**
     * @var triangle_fan_set_node::triangle_fan_set_metatype
     *
     * @brief Class object for TriangleFanSet nodes.
     */

    /**
     * @var triangle_fan_set_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var triangle_fan_set_node::coord_
     *
     * @brief coord exposedField
     */

    /**
     * @var triangle_fan_set_node::fan_count_
     *
     * @brief fan_count exposedField
     */

    /**
     * @var triangle_fan_set_node::normal_
     *
     * @brief normal exposedField
     */

    /**
     * @var triangle_fan_set_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var triangle_fan_set_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var triangle_fan_set_node::color_per_vertex_
     *
     * @brief color_per_vertex field
     */

    /**
     * @var triangle_fan_set_node::normal_per_vertex_
     *
     * @brief normal_per_vertex field
     */

    /**
     * @var triangle_fan_set_node::solid_
     *
     * @brief solid field
     */


    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    triangle_fan_set_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<self_t *>(this)->recalc_bsphere();
        }
        return this->bsphere;
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void triangle_fan_set_node::recalc_bsphere()
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

    const color_node * triangle_fan_set_node::color() const OPENVRML_NOTHROW
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
    triangle_fan_set_node::
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
    bool triangle_fan_set_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    triangle_fan_set_node::
    triangle_fan_set_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        color_(*this),
        coord_(*this),
        fan_count_(*this),
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
    triangle_fan_set_node::~triangle_fan_set_node() OPENVRML_NOTHROW
    {}


    /**
     * @class triangle_set_node
     *
     * @brief Represents TriangleSet node instances.
     */

    /**
     * @var triangle_set_node::triangle_set_metatype
     *
     * @brief Class object for TriangleSet nodes.
     */

    /**
     * @var triangle_set_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var triangle_set_node::coord_
     *
     * @brief coord exposedField
     */

    /**
     * @var triangle_set_node::normal_
     *
     * @brief normal exposedField
     */

    /**
     * @var triangle_set_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var triangle_set_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var triangle_set_node::color_per_vertex_
     *
     * @brief color_per_vertex field
     */

    /**
     * @var triangle_set_node::normal_per_vertex_
     *
     * @brief normal_per_vertex field
     */

    /**
     * @var triangle_set_node::solid_
     *
     * @brief solid field
     */


    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    triangle_set_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<self_t *>(this)->recalc_bsphere();
        }
        return this->bsphere;
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void triangle_set_node::recalc_bsphere()
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

    const color_node * triangle_set_node::color() const OPENVRML_NOTHROW
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
    triangle_set_node::
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
    bool triangle_set_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope the scope to which the node belongs.
     */
    triangle_set_node::
    triangle_set_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
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
    triangle_set_node::~triangle_set_node() OPENVRML_NOTHROW
    {}


    /**
     * @class triangle_strip_set_node
     *
     * @brief Represents TriangleStripSet node instances.
     */

    /**
     * @var triangle_strip_set_node::triangle_strip_set_metatype
     *
     * @brief Class object for TriangleStripSet nodes.
     */

    /**
     * @var triangle_strip_set_node::color_
     *
     * @brief color exposedField
     */

    /**
     * @var triangle_strip_set_node::coord_
     *
     * @brief coord exposedField
     */

    /**
     * @var triangle_strip_set_node::normal_
     *
     * @brief normal exposedField
     */

    /**
     * @var triangle_strip_set_node::strip_count_
     *
     * @brief strip_count exposedField
     */

    /**
     * @var triangle_strip_set_node::tex_coord_
     *
     * @brief tex_coord exposedField
     */

    /**
     * @var triangle_strip_set_node::ccw_
     *
     * @brief ccw field
     */

    /**
     * @var triangle_strip_set_node::color_per_vertex_
     *
     * @brief color_per_vertex field
     */

    /**
     * @var triangle_strip_set_node::normal_per_vertex_
     *
     * @brief normal_per_vertex field
     */

    /**
     * @var triangle_strip_set_node::solid_
     *
     * @brief solid field
     */


    /**
     * @brief Get the bounding volume.
     *
     * @return the bounding volume associated with the node.
     */
    const openvrml::bounding_volume &
    triangle_strip_set_node::do_bounding_volume() const
    {
        if (this->bounding_volume_dirty()) {
            const_cast<self_t *>(this)->recalc_bsphere();
        }
        return this->bsphere;
    }

    /**
     * @brief Recalculate the bounding volume.
     */
    void triangle_strip_set_node::recalc_bsphere()
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

    const color_node * triangle_strip_set_node::color() const OPENVRML_NOTHROW
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
    triangle_strip_set_node::
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
    bool triangle_strip_set_node::modified() const
    {
        return this->node::modified();
    }

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with this node.
     * @param scope     the scope to which the node belongs.
     */
    triangle_strip_set_node::
    triangle_strip_set_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        bounded_volume_node(type, scope),
        abstract_node<self_t>(type, scope),
        geometry_node(type, scope),
        child_node(type, scope),
        color_(*this),
        coord_(*this),
        normal_(*this),
        strip_count_(*this),
        tex_coord_(*this),
        ccw_(true),
        color_per_vertex_(true),
        normal_per_vertex_(true),
        solid_(true)
    {}

    /**
     * @brief Destroy.
     */
    triangle_strip_set_node::~triangle_strip_set_node() OPENVRML_NOTHROW
    {}
}
