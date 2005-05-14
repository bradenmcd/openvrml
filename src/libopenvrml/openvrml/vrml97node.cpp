// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 1999  Kumaran Santhanam
// Copyright 2001, 2002, 2003, 2004, 2005  Braden McDaniel
// Copyright 2002  S. K. Bose
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

# ifdef OPENVRML_ENABLE_PNG_TEXTURES
#   include <png.h>
# endif
# ifdef OPENVRML_ENABLE_JPEG_TEXTURES
#   include <jpeglib.h>
# endif
# include <algorithm>
# include <iostream>
# include <iterator>
# include <limits>
# include <boost/algorithm/string/predicate.hpp>
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
#   include <ft2build.h>
#   include FT_FREETYPE_H
#   include FT_GLYPH_H
#   include FT_OUTLINE_H
#   include <fontconfig/fontconfig.h>
extern "C" {
#   include <fontconfig/fcfreetype.h>
}
# endif
# include <private.h>
# include "vrml97node.h"
# include "browser.h"

namespace {

    /**
     * @internal
     */
    class vrml97_node_type : public openvrml::node_type {
    public:
        virtual ~vrml97_node_type() throw () = 0;
        virtual const openvrml::field_value &
        field_value(const openvrml::node & node,
                    const std::string & id) const
            throw (openvrml::unsupported_interface) = 0;
        virtual openvrml::event_listener &
        event_listener(openvrml::node & node, const std::string & id) const
            throw (openvrml::unsupported_interface) = 0;
        virtual openvrml::event_emitter &
        event_emitter(openvrml::node & node, const std::string & id) const
            throw (openvrml::unsupported_interface) = 0;

    protected:
        vrml97_node_type(const openvrml::node_class & node_class,
                         const std::string & id);
    };

    vrml97_node_type::vrml97_node_type(const openvrml::node_class & node_class,
                                       const std::string & id):
        node_type(node_class, id)
    {}

    vrml97_node_type::~vrml97_node_type() throw ()
    {}


    template <typename NodeT>
    class node_field_ptr {
    public:
        virtual ~node_field_ptr() = 0;
        virtual openvrml::field_value & dereference(NodeT & obj) = 0;
        virtual const openvrml::field_value &
        dereference(const NodeT & obj) = 0;
    };

    template <typename NodeT>
    node_field_ptr<NodeT>::~node_field_ptr()
    {}


    template <typename NodeT, typename ConcreteFieldValue>
    class node_field_ptr_impl : public node_field_ptr<NodeT> {
        ConcreteFieldValue NodeT::* itsPtr;

    public:
        node_field_ptr_impl(ConcreteFieldValue NodeT::* ptr):
            itsPtr(ptr)
        {}

        virtual ~node_field_ptr_impl();

        virtual openvrml::field_value & dereference(NodeT &);
        virtual const openvrml::field_value & dereference(const NodeT &);
    };

    template <typename NodeT, typename ConcreteFieldValue>
    node_field_ptr_impl<NodeT, ConcreteFieldValue>::~node_field_ptr_impl()
    {}

    template <typename NodeT, typename ConcreteFieldValue>
    openvrml::field_value &
    node_field_ptr_impl<NodeT, ConcreteFieldValue>::dereference(NodeT & obj)
    {
        return obj.*itsPtr;
    }

    template <typename NodeT, typename ConcreteFieldValue>
    const openvrml::field_value &
    node_field_ptr_impl<NodeT, ConcreteFieldValue>::dereference(
        const NodeT & obj)
    {
        return obj.*itsPtr;
    }


    template <typename Node>
    class event_listener_ptr {
    public:
        virtual ~event_listener_ptr() throw () = 0;
        virtual openvrml::event_listener &
        dereference(Node & obj) throw () = 0;
    };

    template <typename Node>
    inline event_listener_ptr<Node>::~event_listener_ptr() throw ()
    {}


    template <typename Node, typename ConcreteEventListener>
    class event_listener_ptr_impl : public event_listener_ptr<Node> {
        ConcreteEventListener Node::* its_ptr;

    public:
        event_listener_ptr_impl(ConcreteEventListener Node::* ptr) throw ():
            its_ptr(ptr)
        {}

        virtual ~event_listener_ptr_impl() throw ();

        virtual openvrml::event_listener & dereference(Node &) throw ();
    };

    template <typename Node, typename ConcreteEventListener>
    inline event_listener_ptr_impl<Node, ConcreteEventListener>::
    ~event_listener_ptr_impl() throw ()
    {}

    template <typename Node, typename ConcreteEventListener>
    inline openvrml::event_listener &
    event_listener_ptr_impl<Node, ConcreteEventListener>::
    dereference(Node & obj) throw ()
    {
        return obj.*its_ptr;
    }


    template <typename Node>
    class event_emitter_ptr {
    public:
        virtual ~event_emitter_ptr() throw () = 0;
        virtual openvrml::event_emitter & dereference(Node & obj) throw () = 0;
    };

    template <typename Node>
    inline event_emitter_ptr<Node>::~event_emitter_ptr() throw ()
    {}


    template <typename Node, typename ConcreteEventEmitter>
    class event_emitter_ptr_impl : public event_emitter_ptr<Node> {
        ConcreteEventEmitter Node::* its_ptr;

    public:
        event_emitter_ptr_impl(ConcreteEventEmitter Node::* ptr) throw ();
        virtual ~event_emitter_ptr_impl() throw ();

        virtual openvrml::event_emitter & dereference(Node &) throw ();
    };

    template <typename Node, typename ConcreteEventEmitter>
    inline event_emitter_ptr_impl<Node, ConcreteEventEmitter>::
    event_emitter_ptr_impl(ConcreteEventEmitter Node::* ptr) throw ():
        its_ptr(ptr)
    {}

    template <typename Node, typename ConcreteEventEmitter>
    inline event_emitter_ptr_impl<Node, ConcreteEventEmitter>::
    ~event_emitter_ptr_impl()
        throw ()
    {}

    template <typename Node, typename ConcreteEventEmitter>
    inline openvrml::event_emitter &
    event_emitter_ptr_impl<Node, ConcreteEventEmitter>::
    dereference(Node & obj)
        throw ()
    {
        return obj.*its_ptr;
    }

    using openvrml_::ptr_to_polymorphic_mem;
    using openvrml_::ptr_to_polymorphic_mem_impl;

    template <typename NodeT>
    class vrml97_node_type_impl : public vrml97_node_type {
    public:
        typedef NodeT node_type;

        typedef boost::shared_ptr<ptr_to_polymorphic_mem<openvrml::field_value,
                                                         node_type> >
            field_ptr_ptr;

        template <typename FieldMember>
        class field_ptr :
            public ptr_to_polymorphic_mem_impl<openvrml::field_value,
                                               FieldMember,
                                               NodeT> {
        public:
            explicit field_ptr(FieldMember NodeT::* ptr_to_mem);
        };

        typedef boost::shared_ptr<
            ptr_to_polymorphic_mem<openvrml::event_listener, node_type> >
            event_listener_ptr_ptr;

        template <typename EventListenerMember>
        class event_listener_ptr :
            public ptr_to_polymorphic_mem_impl<openvrml::event_listener,
                                               EventListenerMember,
                                               node_type> {
        public:
            explicit event_listener_ptr(
                EventListenerMember NodeT::* ptr_to_mem);
        };

        typedef boost::shared_ptr<
            ptr_to_polymorphic_mem<openvrml::event_emitter, node_type> >
            event_emitter_ptr_ptr;

        template <typename EventEmitterMember>
        class event_emitter_ptr :
            public ptr_to_polymorphic_mem_impl<openvrml::event_emitter,
                                               EventEmitterMember,
                                               node_type> {
        public:
            explicit event_emitter_ptr(
                EventEmitterMember NodeT::* ptr_to_mem);
        };

    private:
        openvrml::node_interface_set interfaces_;
        typedef std::map<std::string, field_ptr_ptr> field_value_map_t;
        typedef std::map<std::string, event_listener_ptr_ptr>
            event_listener_map_t;
        typedef std::map<std::string, event_emitter_ptr_ptr>
            event_emitter_map_t;
        mutable field_value_map_t field_value_map;
        mutable event_listener_map_t event_listener_map;
        mutable event_emitter_map_t event_emitter_map;

    public:
        vrml97_node_type_impl(const openvrml::node_class & node_class,
                              const std::string & id);
        virtual ~vrml97_node_type_impl() throw ();

        void add_eventin(openvrml::field_value::type_id,
                         const std::string & id,
                         const event_listener_ptr_ptr & event_listener)
            throw (std::invalid_argument, std::bad_alloc);
        void add_eventout(openvrml::field_value::type_id,
                          const std::string & id,
                          const event_emitter_ptr_ptr & event_emitter)
            throw (std::invalid_argument, std::bad_alloc);
        void add_exposedfield(openvrml::field_value::type_id,
                              const std::string & id,
                              const event_listener_ptr_ptr & event_listener,
                              const field_ptr_ptr & field,
                              const event_emitter_ptr_ptr & event_emitter)
            throw (std::invalid_argument, std::bad_alloc);
        void add_field(openvrml::field_value::type_id, const std::string & id,
                       const field_ptr_ptr & fieldPtrPtr)
            throw (std::invalid_argument, std::bad_alloc);

        virtual const openvrml::field_value &
        field_value(const openvrml::node & node, const std::string & id) const
            throw (openvrml::unsupported_interface);
        virtual openvrml::event_listener &
        event_listener(openvrml::node & node, const std::string & id) const
            throw (openvrml::unsupported_interface);
        virtual openvrml::event_emitter &
        event_emitter(openvrml::node & node, const std::string & id) const
            throw (openvrml::unsupported_interface);

    private:
        virtual const openvrml::node_interface_set &
        do_interfaces() const throw ();
        virtual const openvrml::node_ptr
        do_create_node(
            const boost::shared_ptr<openvrml::scope> & scope,
            const openvrml::initial_value_map & initial_values) const
            throw (openvrml::unsupported_interface, std::bad_cast,
                   std::bad_alloc);

        const openvrml::field_value &
        do_field_value(const NodeT & node,
                       const std::string & id) const
            throw (openvrml::unsupported_interface);
        openvrml::event_listener &
        do_event_listener(NodeT & node, const std::string & id) const
            throw (openvrml::unsupported_interface);
        openvrml::event_emitter &
        do_event_emitter(NodeT & node, const std::string & id) const
            throw (openvrml::unsupported_interface);
    };

    template <typename NodeT>
    template <typename FieldMember>
    vrml97_node_type_impl<NodeT>::field_ptr<FieldMember>::
    field_ptr(FieldMember NodeT::* ptr_to_mem):
        ptr_to_polymorphic_mem_impl<openvrml::field_value,
                                    FieldMember,
                                    NodeT>(ptr_to_mem)
    {}

    template <typename NodeT>
    template <typename EventListenerMember>
    vrml97_node_type_impl<NodeT>::event_listener_ptr<EventListenerMember>::
    event_listener_ptr(EventListenerMember NodeT::* ptr_to_mem):
        ptr_to_polymorphic_mem_impl<openvrml::event_listener,
                                    EventListenerMember,
                                    NodeT>(ptr_to_mem)
    {}

    template <typename NodeT>
    template <typename EventEmitterMember>
    vrml97_node_type_impl<NodeT>::event_emitter_ptr<EventEmitterMember>::
    event_emitter_ptr(EventEmitterMember NodeT::* ptr_to_mem):
        ptr_to_polymorphic_mem_impl<openvrml::event_emitter,
                                    EventEmitterMember,
                                    NodeT>(ptr_to_mem)
    {}

    template <typename NodeT>
    vrml97_node_type_impl<NodeT>::
    vrml97_node_type_impl(const openvrml::node_class & node_class,
                          const std::string & id):
        vrml97_node_type(node_class, id)
    {}

    template <typename NodeT>
    vrml97_node_type_impl<NodeT>::~vrml97_node_type_impl() throw ()
    {}

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::
    add_eventin(const openvrml::field_value::type_id type,
                const std::string & id,
                const event_listener_ptr_ptr & event_listener)
        throw (std::invalid_argument, std::bad_alloc)
    {
        using openvrml::node_interface;

        const node_interface interface(node_interface::eventin_id, type, id);
        bool succeeded = this->interfaces_.insert(interface).second;
        if (!succeeded) {
            throw std::invalid_argument("Interface \"" + id + "\" already "
                                        "defined for " + this->id() + " node");
        }
        const typename event_listener_map_t::value_type value(id,
                                                              event_listener);
        succeeded = this->event_listener_map.insert(value).second;
        assert(succeeded);
    }

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::
    add_eventout(const openvrml::field_value::type_id type,
                 const std::string & id,
                 const event_emitter_ptr_ptr & event_emitter)
        throw (std::invalid_argument, std::bad_alloc)
    {
        using openvrml::node_interface;

        const node_interface interface(node_interface::eventout_id, type, id);
        bool succeeded = this->interfaces_.insert(interface).second;
        if (!succeeded) {
            throw std::invalid_argument("Interface \"" + id + "\" already "
                                        "defined for " + this->id() + " node");
        }
        const typename event_emitter_map_t::value_type value(id,
                                                             event_emitter);
        succeeded = this->event_emitter_map.insert(value).second;
        assert(succeeded);
    }

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::add_exposedfield(
            const openvrml::field_value::type_id type,
            const std::string & id,
            const event_listener_ptr_ptr & event_listener,
            const field_ptr_ptr & field,
            const event_emitter_ptr_ptr & event_emitter)
        throw (std::invalid_argument, std::bad_alloc)
    {
        using openvrml::node_interface;

        const node_interface interface(node_interface::exposedfield_id,
                                       type,
                                       id);
        bool succeeded = this->interfaces_.insert(interface).second;
        if (!succeeded) {
            throw std::invalid_argument("Interface \"" + id + "\" already "
                                        "defined for " + this->id() + " node");
        }
        {
            const typename event_listener_map_t::value_type
                value("set_" + id, event_listener);
            succeeded = this->event_listener_map.insert(value).second;
            assert(succeeded);
        }
        {
            const typename field_value_map_t::value_type value(id, field);
            succeeded = this->field_value_map.insert(value).second;
            assert(succeeded);
        }
        {
            const typename event_emitter_map_t::value_type
                value(id + "_changed", event_emitter);
            succeeded = this->event_emitter_map.insert(value).second;
            assert(succeeded);
        }
    }

    template <typename NodeT>
    void vrml97_node_type_impl<NodeT>::add_field(
            const openvrml::field_value::type_id type,
            const std::string & id,
            const field_ptr_ptr & nodeFieldPtrPtr)
        throw (std::invalid_argument, std::bad_alloc)
    {
        using openvrml::node_interface;

        const node_interface interface(node_interface::field_id, type, id);
        bool succeeded = this->interfaces_.insert(interface).second;
        if (!succeeded) {
            throw std::invalid_argument("Interface \"" + id + "\" already "
                                        "defined for " + this->id() + " node");
        }
        const typename field_value_map_t::value_type
            value(id, nodeFieldPtrPtr);
        succeeded = this->field_value_map.insert(value).second;
        assert(succeeded);
    }

    template <typename NodeT>
    const openvrml::field_value &
    vrml97_node_type_impl<NodeT>::field_value(const openvrml::node & node,
                                              const std::string & id) const
        throw (openvrml::unsupported_interface)
    {
        assert(dynamic_cast<const NodeT *>(&node));
        return this->do_field_value(dynamic_cast<const NodeT &>(node), id);
    }

    template <typename NodeT>
    openvrml::event_listener &
    vrml97_node_type_impl<NodeT>::event_listener(openvrml::node & node,
                                                 const std::string & id) const
        throw (openvrml::unsupported_interface)
    {
        assert(dynamic_cast<NodeT *>(&node));
        return this->do_event_listener(dynamic_cast<NodeT &>(node), id);
    }

    template <typename NodeT>
    openvrml::event_emitter &
    vrml97_node_type_impl<NodeT>::event_emitter(openvrml::node & node,
                                                const std::string & id) const
        throw (openvrml::unsupported_interface)
    {
        assert(dynamic_cast<NodeT *>(&node));
        return this->do_event_emitter(dynamic_cast<NodeT &>(node), id);
    }

    template <typename NodeT>
    const openvrml::node_interface_set &
    vrml97_node_type_impl<NodeT>::do_interfaces() const
        throw ()
    {
        return this->interfaces_;
    }

    template <typename NodeT>
    const openvrml::node_ptr
    vrml97_node_type_impl<NodeT>::
    do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
                   const openvrml::initial_value_map & initial_values) const
        throw (openvrml::unsupported_interface, std::bad_cast, std::bad_alloc)
    {
        using namespace openvrml;

        NodeT * const concrete_node_ptr = new NodeT(*this, scope);
        const node_ptr result(concrete_node_ptr);
        for (initial_value_map::const_iterator initial_value =
                 initial_values.begin();
             initial_value != initial_values.end();
             ++initial_value) {
            const typename field_value_map_t::const_iterator field =
                this->field_value_map.find(initial_value->first);
            if (field == this->field_value_map.end()) {
                throw unsupported_interface(*this,
                                            node_interface::field_id,
                                            initial_value->first);
            }
            field->second->deref(*concrete_node_ptr)
                .assign(*initial_value->second);
        }
        return result;
    }

    template <typename NodeT>
    const openvrml::field_value &
    vrml97_node_type_impl<NodeT>::do_field_value(const NodeT & node,
                                                 const std::string & id) const
        throw (openvrml::unsupported_interface)
    {
        using namespace openvrml;

        const typename field_value_map_t::const_iterator itr =
                this->field_value_map.find(id);
        if (itr == this->field_value_map.end()) {
            throw unsupported_interface(node.node::type(),
                                        node_interface::field_id,
                                        id);
        }
        return itr->second->deref(node);
    }

    template <typename NodeT>
    openvrml::event_listener &
    vrml97_node_type_impl<NodeT>::
    do_event_listener(NodeT & node,
                      const std::string & id) const
        throw (openvrml::unsupported_interface)
    {
        using namespace openvrml;

        const typename event_listener_map_t::const_iterator end =
            this->event_listener_map.end();
        typename event_listener_map_t::const_iterator pos =
            this->event_listener_map.find(id);
        if (pos == end) { pos = this->event_listener_map.find("set_" + id); }
        if (pos == end) {
            throw unsupported_interface(node.node::type(),
                                        node_interface::eventin_id,
                                        id);
        }
        return pos->second->deref(node);
    }

    template <typename NodeT>
    openvrml::event_emitter &
    vrml97_node_type_impl<NodeT>::
    do_event_emitter(NodeT & node,
                     const std::string & id) const
        throw (openvrml::unsupported_interface)
    {
        using namespace openvrml;

        const typename event_emitter_map_t::const_iterator end =
            this->event_emitter_map.end();
        typename event_emitter_map_t::const_iterator pos =
            this->event_emitter_map.find(id);
        if (pos == end) {
            pos = this->event_emitter_map.find(id + "_changed");
        }
        if (pos == end) {
            throw unsupported_interface(node.node::type(),
                                        node_interface::eventout_id,
                                        id);
        }
        return pos->second->deref(node);
    }

# ifdef OPENVRML_ENABLE_PNG_TEXTURES
    extern "C" void openvrml_png_info_callback(png_structp png_ptr,
                                               png_infop info_ptr);
    extern "C" void openvrml_png_row_callback(png_structp png_ptr,
                                              png_bytep new_row,
                                              png_uint_32 row_num,
                                              int pass);
    extern "C" void openvrml_png_end_callback(png_structp png_ptr,
                                              png_infop info_ptr);
# endif

# ifdef OPENVRML_ENABLE_JPEG_TEXTURES
    extern "C" void openvrml_jpeg_init_source(j_decompress_ptr cinfo);
    extern "C" boolean openvrml_jpeg_fill_input_buffer(j_decompress_ptr cinfo);
    extern "C" void openvrml_jpeg_skip_input_data(j_decompress_ptr cinfo,
                                                  long num_bytes);
    extern "C" void openvrml_jpeg_term_source(j_decompress_ptr cinfo);
# endif

    class image_stream_listener : public openvrml::stream_listener {
        boost::recursive_mutex & node_mutex_;
        openvrml::image & image_;
        openvrml::node & node_;

        class image_reader {
        public:
            void read(const std::vector<unsigned char> & data);

        private:
            virtual void do_read(const std::vector<unsigned char> & data) = 0;
        };

# ifdef OPENVRML_ENABLE_PNG_TEXTURES
        friend void openvrml_png_info_callback(png_structp png_ptr,
                                               png_infop info_ptr);
        friend void openvrml_png_row_callback(png_structp png_ptr,
                                              png_bytep new_row,
                                              png_uint_32 row_num,
                                              int pass);
        friend void openvrml_png_end_callback(png_structp png_ptr,
                                              png_infop info_ptr);

        class png_reader : public image_reader {
            png_structp png_ptr_;
            png_infop info_ptr_;

        public:
            image_stream_listener & stream_listener;
            std::vector<png_byte> old_row;
            bool gray_palette;

            explicit png_reader(image_stream_listener & stream_listener);
            virtual ~png_reader() throw ();

        private:
            virtual void do_read(const std::vector<unsigned char> & data);
        };
# endif

# ifdef OPENVRML_ENABLE_JPEG_TEXTURES
        friend void openvrml_jpeg_init_source(j_decompress_ptr cinfo);
        friend boolean openvrml_jpeg_fill_input_buffer(j_decompress_ptr cinfo);
        friend void openvrml_jpeg_skip_input_data(j_decompress_ptr cinfo,
                                                  long num_bytes);
        friend void openvrml_jpeg_term_source(j_decompress_ptr cinfo);

        class jpeg_reader : public image_reader {
        public:
            struct source_mgr {
                jpeg_source_mgr pub;
                jpeg_reader * reader;
            };

        private:
            jpeg_decompress_struct cinfo_;
            struct error_mgr {
                jpeg_error_mgr pub;
                jmp_buf jmpbuf;
            } error_mgr_;
            source_mgr source_mgr_;

        public:
            image_stream_listener & stream_listener;
            enum read_state_t {
                reading_back = 0,
                reading_new
            } read_state;
            size_t bytes_to_skip;
            size_t backtrack_buffer_bytes_unread;
            std::vector<JOCTET> buffer, backtrack_buffer;
            enum decoder_state_t {
                header,
                start_decompress,
                decompress_progressive,
                decompress_sequential,
                done,
                sink_non_jpeg_trailer,
                error
            } decoder_state;
            JSAMPARRAY scanlines;
            bool progressive_scan_started;

            explicit jpeg_reader(image_stream_listener & stream_listener);
            virtual ~jpeg_reader() throw ();

        private:
            virtual void do_read(const std::vector<unsigned char> & data);

            bool output_scanlines();
        };
# endif

        boost::scoped_ptr<image_reader> image_reader_;

    public:
        image_stream_listener(openvrml::image & image,
                              openvrml::node & node,
                              boost::recursive_mutex & node_mutex);
        virtual ~image_stream_listener() throw ();

    private:
        virtual void
        do_stream_available(const std::string & uri,
                            const std::string & media_type);

        virtual void
        do_data_available(const std::vector<unsigned char> & data);
    };

    void
    image_stream_listener::image_reader::
    read(const std::vector<unsigned char> & data)
    {
        this->do_read(data);
    }

# ifdef OPENVRML_ENABLE_PNG_TEXTURES
    void openvrml_png_info_callback(png_structp png_ptr, png_infop info_ptr)
    {
        typedef image_stream_listener::png_reader png_reader_t;
        png_reader_t & reader =
            *static_cast<png_reader_t *>(png_get_progressive_ptr(png_ptr));

        boost::recursive_mutex::scoped_lock
            lock(reader.stream_listener.node_mutex_);

        openvrml::image & image = reader.stream_listener.image_;

        image.comp(png_get_channels(png_ptr, info_ptr));
        const size_t width = png_get_image_width(png_ptr, info_ptr);
        const size_t height = png_get_image_height(png_ptr, info_ptr);
        image.resize(width, height);

        //
        // Strip 16 bit/color files to 8 bit/color.
        //
        png_set_strip_16(png_ptr);

        //
        // Extract multiple pixels with bit depths of 1, 2, and 4 from a
        // single byte into separate bytes.  (Usefule for paletted and
        // grayscale images.)
        //
        png_set_packing(png_ptr);

        //
        // Expand paletted colors into true RGB triplets.
        //
        const png_byte color_type = png_get_color_type(png_ptr, info_ptr);
        if (color_type == PNG_COLOR_TYPE_PALETTE) {
            png_set_expand(png_ptr);
            image.comp(3);
        }

        //
        // Expand grayscale images to the full 8 bits from 1, 2, or
        // 4 bits/pixel.
        //
        const png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
            png_set_expand(png_ptr);
        }

        //
        // Expand paletted or RGB images with transparency to full alpha
        // channels so the data will be available as RGBA quartets.
        //
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            png_set_expand(png_ptr);
            image.comp(image.comp() + 1);
        }

        if (color_type == PNG_COLOR_TYPE_PALETTE) {
            int num_palette;
            png_colorp palette;
            if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette)) {
                reader.gray_palette = true;
                for (int i = 0; i < num_palette; ++i) {
                    if (palette[i].red != palette[i].green
                        || palette[i].blue != palette[i].green) {
                        reader.gray_palette = false;
                        break;
                    }
                }
            }
        }

        if (reader.gray_palette) {
            if (image.comp() == 3) {
                image.comp(1);
            } else if (image.comp() == 4) {
                image.comp(2);
            }
        }

        //
        // Set gamma.
        //
        static const double screen_gamma = 2.2; // Display exponent.
        static const double default_gamma = 0.45455;
        double file_gamma;
        png_set_gamma(png_ptr,
                      screen_gamma,
                      png_get_gAMA(png_ptr, info_ptr, &file_gamma)
                      ? file_gamma
                      : default_gamma);

        png_set_interlace_handling(png_ptr);

        png_read_update_info(png_ptr, info_ptr);

        reader.old_row.resize(image.comp() * image.x());
    }

    void openvrml_png_row_callback(png_structp png_ptr,
                                   png_bytep new_row,
                                   png_uint_32 row_num,
                                   int pass)
    {
        if (!new_row) { return; }

        typedef image_stream_listener::png_reader png_reader_t;
        png_reader_t & reader =
            *static_cast<png_reader_t *>(png_get_progressive_ptr(png_ptr));

        boost::recursive_mutex::scoped_lock
            lock(reader.stream_listener.node_mutex_);

        openvrml::image & image = reader.stream_listener.image_;

        png_progressive_combine_row(png_ptr, &reader.old_row[0], new_row);

        //
        // openvrml::image pixels start at the bottom left.
        //
        const size_t image_row = (image.y() - 1) - row_num;
        const size_t bytes_per_row = reader.old_row.size();
        const size_t image_width = bytes_per_row / image.comp();
        for (size_t pixel_index = 0, byte_index = 0; pixel_index < image_width;
             ++pixel_index) {
            using openvrml::int32;
            int32 pixel = 0x00000000;
            for (size_t component = image.comp();
                 component > 0;
                 --component) {
                pixel |= int32(new_row[byte_index]) << (8 * (component - 1));
                if (reader.gray_palette) {
                    if (image.comp() == 1) {
                        // We just want every third pixel element.
                        byte_index += 3;
                    } else if (image.comp() == 2) {
                        // We just want the first and fourth pixel elements.
                        byte_index += (byte_index % 4 == 0) ? 3 : 1;
                    }
                } else {
                    ++byte_index;
                }
            }
            image.pixel(image_row * image_width + pixel_index, pixel);
        }

        reader.stream_listener.node_.modified(true);

        copy(new_row, new_row + bytes_per_row, reader.old_row.begin());
    }

    void openvrml_png_end_callback(png_structp png_ptr, png_infop info_ptr)
    {
    }

    image_stream_listener::png_reader::
    png_reader(image_stream_listener & stream_listener):
        png_ptr_(0),
        info_ptr_(0),
        stream_listener(stream_listener),
        gray_palette(false)
    {
        using openvrml_::scope_guard;
        using openvrml_::make_guard;

        this->png_ptr_ =
            png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        if (!this->png_ptr_) { throw std::bad_alloc(); }
        scope_guard guard = make_guard(&png_destroy_read_struct,
                                       &this->png_ptr_,
                                       &this->info_ptr_,
                                       png_infopp(0));

        this->info_ptr_ = png_create_info_struct(this->png_ptr_);
        if (!this->info_ptr_) { throw std::bad_alloc(); }

        png_set_progressive_read_fn(this->png_ptr_,
                                    this,
                                    openvrml_png_info_callback,
                                    openvrml_png_row_callback,
                                    openvrml_png_end_callback);
        guard.dismiss();
    }

    image_stream_listener::png_reader::~png_reader() throw ()
    {
        png_destroy_read_struct(&this->png_ptr_,
                                &this->info_ptr_,
                                png_infopp(0));
    }

    void
    image_stream_listener::png_reader::
    do_read(const std::vector<unsigned char> & data)
    {
        int jmpval = setjmp(png_jmpbuf(this->png_ptr_));
        if (jmpval != 0) { return; }

        png_process_data(this->png_ptr_,
                         this->info_ptr_,
                         const_cast<png_byte *>(&data[0]),
                         data.size());
    }
# endif // defined OPENVRML_ENABLE_PNG_TEXTURES

# ifdef OPENVRML_ENABLE_JPEG_TEXTURES
    void openvrml_jpeg_init_source(j_decompress_ptr cinfo)
    {
    }

    boolean openvrml_jpeg_fill_input_buffer(j_decompress_ptr cinfo)
    {
        typedef image_stream_listener::jpeg_reader::source_mgr source_mgr_t;
        source_mgr_t & src =
            *reinterpret_cast<source_mgr_t *>(cinfo->src);

        jpeg_source_mgr & source_mgr = src.pub;
        image_stream_listener::jpeg_reader & reader = *src.reader;

        switch (reader.read_state) {
        case image_stream_listener::jpeg_reader::reading_back:
            if (reader.buffer.empty()) {
                return false; // Suspend.
            }
            if (reader.bytes_to_skip > reader.buffer.size()) {
                reader.bytes_to_skip -= reader.buffer.size();
                reader.buffer.clear();
                return false; // Suspend.
            }

            reader.backtrack_buffer_bytes_unread = source_mgr.bytes_in_buffer;
            {
                std::vector<JOCTET>::iterator pos = reader.buffer.begin();
                advance(pos, reader.bytes_to_skip);
                source_mgr.next_input_byte = &*pos;
            }
            source_mgr.bytes_in_buffer -= reader.bytes_to_skip;
            reader.bytes_to_skip = 0;
            reader.read_state =
                image_stream_listener::jpeg_reader::reading_new;

            return true;

        case image_stream_listener::jpeg_reader::reading_new:
            if (source_mgr.next_input_byte != (reader.buffer.empty()
                                               ? 0 : &reader.buffer[0])) {
                reader.backtrack_buffer_bytes_unread = 0;
                reader.backtrack_buffer.resize(0);
            }

            {
                const size_t old_backtrack_buffer_size =
                    reader.backtrack_buffer.size();

                reader.backtrack_buffer.resize(source_mgr.bytes_in_buffer
                                               + old_backtrack_buffer_size);

                copy(source_mgr.next_input_byte,
                     source_mgr.next_input_byte + source_mgr.bytes_in_buffer,
                     reader.backtrack_buffer.begin()
                     + old_backtrack_buffer_size);

                source_mgr.next_input_byte =
                    &*(reader.backtrack_buffer.begin()
                       + old_backtrack_buffer_size
                       - reader.backtrack_buffer_bytes_unread);
            }
            source_mgr.bytes_in_buffer += reader.backtrack_buffer_bytes_unread;
            reader.read_state =
                image_stream_listener::jpeg_reader::reading_back;
        }
        return false;
    }

    void openvrml_jpeg_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
    {
        typedef image_stream_listener::jpeg_reader::source_mgr source_mgr_t;
        source_mgr_t & src =
            *reinterpret_cast<source_mgr_t *>(cinfo->src);

        jpeg_source_mgr & source_mgr = src.pub;
        image_stream_listener::jpeg_reader & reader = *src.reader;

        if (num_bytes > source_mgr.bytes_in_buffer) {
            reader.bytes_to_skip = num_bytes - source_mgr.bytes_in_buffer;
            source_mgr.next_input_byte += source_mgr.bytes_in_buffer;
            source_mgr.bytes_in_buffer = 0;
        } else {
            source_mgr.bytes_in_buffer -= num_bytes;
            source_mgr.next_input_byte += num_bytes;
        }
    }

    void openvrml_jpeg_term_source(j_decompress_ptr cinfo)
    {
    }

    image_stream_listener::jpeg_reader::
    jpeg_reader(image_stream_listener & stream_listener):
        stream_listener(stream_listener),
        read_state(reading_back),
        bytes_to_skip(0),
        backtrack_buffer_bytes_unread(0),
        decoder_state(header),
        scanlines(0),
        progressive_scan_started(false)
    {
        //std::memset(&this->cinfo, 0, sizeof this->cinfo_);
        //std::memset(&this->source_mgr_, 0, sizeof this->source_mgr_);

        this->cinfo_.err = jpeg_std_error(&this->error_mgr_.pub);
        int jmpval = setjmp(this->error_mgr_.jmpbuf);
        if (jmpval != 0) { return; }
        jpeg_create_decompress(&this->cinfo_);
        this->source_mgr_.pub.next_input_byte = 0;
        this->source_mgr_.pub.bytes_in_buffer = 0;
        this->source_mgr_.pub.init_source = openvrml_jpeg_init_source;
        this->source_mgr_.pub.fill_input_buffer =
            openvrml_jpeg_fill_input_buffer;
        this->source_mgr_.pub.skip_input_data = openvrml_jpeg_skip_input_data;
        this->source_mgr_.pub.resync_to_restart = jpeg_resync_to_restart;
        this->source_mgr_.pub.term_source = openvrml_jpeg_term_source;
        this->source_mgr_.reader = this;

        this->cinfo_.src =
            reinterpret_cast<jpeg_source_mgr *>(&this->source_mgr_);
    }

    image_stream_listener::jpeg_reader::~jpeg_reader() throw ()
    {
        jpeg_destroy_decompress(&this->cinfo_);
    }

    void
    image_stream_listener::jpeg_reader::
    do_read(const std::vector<unsigned char> & data)
    {
        if (data.size() > this->buffer.size()) {
            this->buffer.resize(data.size());
        }
        copy(data.begin(), data.end(), this->buffer.begin());

        int jmpval = setjmp(this->error_mgr_.jmpbuf);
        if (jmpval != 0) { return; }

        switch (this->decoder_state) {
        case jpeg_reader::header:
        {
            boost::recursive_mutex::scoped_lock
                lock(this->stream_listener.node_mutex_);

            static const bool require_image = true;
            const int read_header_result = jpeg_read_header(&this->cinfo_,
                                                            require_image);
            if (read_header_result == JPEG_SUSPENDED) { return; }

            switch (this->cinfo_.jpeg_color_space) {
            case JCS_GRAYSCALE:
            case JCS_RGB:
            case JCS_YCbCr:
                this->cinfo_.out_color_space = JCS_RGB;
                break;
            default:
                this->decoder_state = jpeg_reader::error;
                return;
            }

            this->cinfo_.buffered_image =
                jpeg_has_multiple_scans(&this->cinfo_);

            jpeg_calc_output_dimensions(&this->cinfo_);

            openvrml::image & image = this->stream_listener.image_;
            image.comp(this->cinfo_.num_components);
            image.resize(this->cinfo_.image_width, this->cinfo_.image_height);

            const JDIMENSION samples_per_row =
                this->cinfo_.output_width * this->cinfo_.num_components;
            static const JDIMENSION num_rows = 1;
            this->scanlines =
                (*this->cinfo_.mem->alloc_sarray)(
                    reinterpret_cast<j_common_ptr>(&this->cinfo_),
                    JPOOL_IMAGE,
                    samples_per_row,
                    num_rows);

            this->decoder_state = jpeg_reader::start_decompress;
        }
        case jpeg_reader::start_decompress:
        {
            this->cinfo_.dct_method = JDCT_ISLOW;
            this->cinfo_.dither_mode = JDITHER_FS;
            this->cinfo_.do_fancy_upsampling = true;
            this->cinfo_.enable_2pass_quant = false;
            this->cinfo_.do_block_smoothing = true;

            if (!jpeg_start_decompress(&this->cinfo_)) {
                return; // Input suspended.
            }

            this->decoder_state = this->cinfo_.buffered_image
                ? jpeg_reader::decompress_progressive
                : jpeg_reader::decompress_sequential;
        }
        case jpeg_reader::decompress_sequential:
            if (this->decoder_state == jpeg_reader::decompress_sequential) {
                if (!this->output_scanlines()) {
                    return; // Input suspended.
                }
                this->decoder_state = jpeg_reader::done;
            }
        case jpeg_reader::decompress_progressive:
            if (this->decoder_state == jpeg_reader::decompress_progressive) {
                int status;
                do {
                    status = jpeg_consume_input(&this->cinfo_);
                } while (status != JPEG_SUSPENDED
                         && status != JPEG_REACHED_EOI);

                while (true) {
                    if (this->cinfo_.output_scanline == 0
                        && !this->progressive_scan_started) {
                        int scan = this->cinfo_.input_scan_number;

                        if (this->cinfo_.output_scan_number == 0
                            && scan > 1
                            && status != JPEG_REACHED_EOI) {
                            --scan;
                        }

                        if (!jpeg_start_output(&this->cinfo_, scan)) {
                            return; // Input suspended.
                        }
                        this->progressive_scan_started = true;
                    }

                    if (!this->output_scanlines()) {
                        return; // Input suspended.
                    }

                    if (this->cinfo_.output_scanline
                        == this->cinfo_.output_height) {
                        if (!jpeg_finish_output(&this->cinfo_)) {
                            return; // Input suspended.
                        }
                        if (jpeg_input_complete(&this->cinfo_)
                            && (this->cinfo_.input_scan_number
                                == this->cinfo_.output_scan_number)) {
                            break;
                        }
                        this->cinfo_.output_scanline = 0;
                        this->progressive_scan_started = false;
                    }
                }

                this->decoder_state = jpeg_reader::done;
            }
        case jpeg_reader::done:
            if (!jpeg_finish_decompress(&this->cinfo_)) {
                return; // Input suspended.
            }
            this->decoder_state = jpeg_reader::sink_non_jpeg_trailer;
            break;

        case jpeg_reader::sink_non_jpeg_trailer:
            break;

        case jpeg_reader::error:
            break;
        }
    }

    bool image_stream_listener::jpeg_reader::output_scanlines()
    {
        boost::recursive_mutex::scoped_lock
            lock(this->stream_listener.node_mutex_);

        JDIMENSION top = this->cinfo_.output_scanline;
        bool result = true;

        openvrml::image & image = this->stream_listener.image_;
        const size_t bytes_per_row = image.comp() * image.x();

        while (this->cinfo_.output_scanline < this->cinfo_.output_height) {
            JDIMENSION scanlines_completed =
                jpeg_read_scanlines(&this->cinfo_, this->scanlines, 1);
            if (scanlines_completed != 1) {
                result = false; // Suspend.
                break;
            }

            const size_t image_row = image.y() - this->cinfo_.output_scanline;
            for (size_t pixel_index = 0, byte_index = 0;
                 pixel_index < image.x();
                 ++pixel_index) {
                using openvrml::int32;
                int32 pixel = 0x00000000;
                for (size_t component = image.comp();
                     component > 0;
                     --component, ++byte_index) {
                    const JSAMPLE sample = (*this->scanlines)[byte_index];
                    pixel |= int32(sample) << (8 * (component - 1));
                }
                image.pixel(image_row * image.x() + pixel_index, pixel);
            }
        }

        if (top != this->cinfo_.output_scanline) {
            this->stream_listener.node_.modified(true);
        }

        return result;
    }
# endif // defined OPENVRML_ENABLE_JPEG_TEXTURES

    image_stream_listener::
    image_stream_listener(openvrml::image & image,
                          openvrml::node & node,
                          boost::recursive_mutex & node_mutex):
        node_mutex_(node_mutex),
        image_(image),
        node_(node)
    {}

    image_stream_listener::~image_stream_listener() throw ()
    {}

    void
    image_stream_listener::do_stream_available(const std::string & uri,
                                               const std::string & media_type)
    {
        using boost::algorithm::iequals;
        if (iequals(media_type, "image/png")
            || iequals(media_type, "image/x-png")) {
# ifdef OPENVRML_ENABLE_PNG_TEXTURES
            this->image_reader_.reset(new png_reader(*this));
# endif
        } else if (iequals(media_type, "image/jpeg")) {
# ifdef OPENVRML_ENABLE_JPEG_TEXTURES
            this->image_reader_.reset(new jpeg_reader(*this));
# endif
        }
    }

    void
    image_stream_listener::
    do_data_available(const std::vector<unsigned char> & data)
    {
        if (this->image_reader_) { this->image_reader_->read(data); }
    }
}


/**
 * @namespace openvrml::vrml97_node
 *
 * @brief Implementations of the VRML97 nodes.
 *
 * It is a legacy of OpenVRML's initial architecture that these classes are
 * exposed as part of the public API. Once all the dependencies on members of
 * this namespace have been factored out of other parts of OpenVRML, all of
 * these classes will be moving into the library's implementation.
 */


/**
 * @class openvrml::vrml97_node::abstract_base
 *
 * @brief Abstract base class for VRML97 node implementations.
 *
 * abstract_base encapsulates the mechanisms for field access and mutation,
 * event dispatch, and eventOut access.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::abstract_base::
abstract_base(const node_type & type,
              const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::abstract_base::~abstract_base() throw ()
{}

/**
 * @brief Get a field value for a node.
 *
 * @param id    a field name.
 *
 * @exception unsupported_interface  if the node has no field @p id.
 */
const openvrml::field_value &
openvrml::vrml97_node::abstract_base::do_field(const std::string & id) const
    throw (unsupported_interface)
{
    assert(dynamic_cast<const vrml97_node_type *>(&this->type()));
    return static_cast<const vrml97_node_type &>(this->type())
        .field_value(*this, id);
}

/**
 * @brief Get an event listener.
 *
 * This method is called by node::event_listener.
 *
 * @param id    eventIn identifier.
 *
 * @return the event listener.
 *
 * @exception unsupported_interface if the node has no eventIn @p id.
 */
openvrml::event_listener &
openvrml::vrml97_node::abstract_base::do_event_listener(const std::string & id)
    throw (unsupported_interface)
{
    assert(dynamic_cast<const vrml97_node_type *>(&this->type()));
    return static_cast<const vrml97_node_type &>(this->type())
        .event_listener(*this, id);
}

/**
 * @brief Get an event emitter.
 *
 * This method is called by node::event_emitter.
 *
 * @param id    eventOut identifier.
 *
 * @return the event emitter.
 *
 * @exception unsupported_interface if the node has no eventOut @p id.
 */
openvrml::event_emitter &
openvrml::vrml97_node::abstract_base::do_event_emitter(const std::string & id)
    throw (unsupported_interface)
{
    assert(dynamic_cast<const vrml97_node_type *>(&this->type()));
    return static_cast<const vrml97_node_type &>(this->type())
        .event_emitter(*this, id);
}


/**
 * @class openvrml::vrml97_node::abstract_indexed_set_node
 *
 * @brief Abstract base class for IndexedFaceSet and IndexedLineSet.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::abstract_indexed_set_node::set_color_index_listener
 *
 * @brief set_colorIndex event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  abstract_indexed_set_node.
 */
openvrml::vrml97_node::abstract_indexed_set_node::set_color_index_listener::
set_color_index_listener(abstract_indexed_set_node & node):
    mfint32_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::abstract_indexed_set_node::set_color_index_listener::
~set_color_index_listener() throw ()
{}

/**
 * @brief Process event.
 *
 * @param color_index colorIndex.
 * @param timestamp   the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::abstract_indexed_set_node::set_color_index_listener::
do_process_event(const mfint32 & color_index,
                 const double timestamp)
    throw (std::bad_alloc)
{
    try {
        abstract_indexed_set_node & abstract_indexed_set =
            dynamic_cast<abstract_indexed_set_node &>(this->node());

        abstract_indexed_set.color_index_ = color_index;
        abstract_indexed_set.node::modified(true);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::abstract_indexed_set_node::set_coord_index_listener
 *
 * @brief set_coordIndex event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  abstract_indexed_set_node.
 */
openvrml::vrml97_node::abstract_indexed_set_node::set_coord_index_listener::
set_coord_index_listener(abstract_indexed_set_node & node):
    mfint32_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::abstract_indexed_set_node::set_coord_index_listener::
~set_coord_index_listener() throw ()
{}

/**
 * @brief Process event.
 *
 * @param coord_index coordIndex.
 * @param timestamp   the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::abstract_indexed_set_node::set_coord_index_listener::
do_process_event(const mfint32 & coord_index,
                 const double timestamp)
    throw (std::bad_alloc)
{
    try {
        abstract_indexed_set_node & abstract_indexed_set =
            dynamic_cast<abstract_indexed_set_node &>(this->node());

        abstract_indexed_set.coord_index_ = coord_index;
        abstract_indexed_set.node::modified(true);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::abstract_indexed_set_node::set_color_index_listener openvrml::vrml97_node::abstract_indexed_set_node::set_color_index_
 *
 * @brief set_colorIndex event handler.
 */

/**
 * @var openvrml::vrml97_node::abstract_indexed_set_node::set_coord_index_listener openvrml::vrml97_node::abstract_indexed_set_node::set_coord_index_
 *
 * @brief set_coordIndex event handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::abstract_indexed_set_node::color_
 *
 * @brief color exposedField.
 */

/**
 * @var openvrml::exposedfield<sfnode> openvrml::vrml97_node::abstract_indexed_set_node::coord_
 *
 * @brief coord exposedField.
 */

/**
 * @var openvrml::mfint32 openvrml::vrml97_node::abstract_indexed_set_node::color_index_
 *
 * @brief colorIndex field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::abstract_indexed_set_node::color_per_vertex_
 *
 * @brief colorPerVertex field.
 */

/**
 * @var openvrml::mfint32 openvrml::vrml97_node::abstract_indexed_set_node::coord_index_
 *
 * @brief coordIndex field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope that the new node will belong to.
 */
openvrml::vrml97_node::abstract_indexed_set_node::
abstract_indexed_set_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    geometry_node(type, scope),
    set_color_index_(*this),
    set_coord_index_(*this),
    color_(*this),
    coord_(*this),
    color_per_vertex_(true)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::abstract_indexed_set_node::~abstract_indexed_set_node()
    throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::abstract_indexed_set_node::modified() const
{
    return this->node::modified()
        || (this->color_.sfnode::value
            && this->color_.sfnode::value->modified())
        || (this->coord_.sfnode::value
            && this->coord_.sfnode::value->modified());
}

/**
 * @brief color_node.
 *
 * @return the color_node, or 0 if none is set.
 */
const openvrml::color_node *
openvrml::vrml97_node::abstract_indexed_set_node::color() const throw ()
{
    return node_cast<openvrml::color_node *>(this->color_.sfnode::value.get());
}


/**
 * @class openvrml::vrml97_node::abstract_light_node
 *
 * @brief Base class for all light nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::abstract_light_node::ambient_intensity_
 *
 * @brief ambientIntensity exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfcolor> openvrml::vrml97_node::abstract_light_node::color_
 *
 * @brief color exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::abstract_light_node::intensity_
 *
 * @brief intensity exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::abstract_light_node::on_
 *
 * @brief on exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type for the node.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::abstract_light_node::
abstract_light_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    ambient_intensity_(*this, 0.0),
    color_(*this, openvrml::color(1.0, 1.0, 1.0)),
    intensity_(*this, 1.0),
    on_(*this, true)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::abstract_light_node::~abstract_light_node() throw ()
{}

/**
 * @brief Downcast to a light node.
 *
 * @return a pointer to this object.
 */
openvrml::vrml97_node::abstract_light_node *
openvrml::vrml97_node::abstract_light_node::to_light() const
{
    return const_cast<abstract_light_node *>(this);
}

/**
 * @brief Ambient intensity.
 *
 * @return the ambient intensity.
 */
float openvrml::vrml97_node::abstract_light_node::ambient_intensity() const
    throw ()
{
    return this->ambient_intensity_.sffloat::value;
}

/**
 * @brief Intensity.
 *
 * @return the intensity.
 */
float openvrml::vrml97_node::abstract_light_node::intensity() const throw ()
{
    return this->intensity_.sffloat::value;
}

/**
 * @brief Whether the light is on.
 *
 * @return @c true if the light is on; @c false otherwise.
 */
bool openvrml::vrml97_node::abstract_light_node::on() const throw ()
{
    return this->on_.sfbool::value;
}

/**
 * @brief Light color.
 *
 * @return the light color.
 */
const openvrml::color &
openvrml::vrml97_node::abstract_light_node::color() const throw ()
{
    return this->color_.sfcolor::value;
}

/**
 * @brief Render this node as scoped.
 *
 * The default version of this method does nothing.
 *
 * @param v a renderer.
 */
void
openvrml::vrml97_node::abstract_light_node::renderScoped(openvrml::viewer & v)
{}


/**
 * @class openvrml::vrml97_node::abstract_texture_node
 *
 * @brief Abstract base class for VRML97 texture nodes.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::abstract_texture_node::repeat_s_
 *
 * @brief repeatS field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::abstract_texture_node::repeat_t_
 *
 * @brief repeatT field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type for the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::abstract_texture_node::
abstract_texture_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    texture_node(type, scope),
    repeat_s_(true),
    repeat_t_(true)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::abstract_texture_node::~abstract_texture_node() throw ()
{}

/**
 * @brief Get the repeatS flag.
 *
 * @return @c true if the texture should repeat in the <var>S</var> direction,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::abstract_texture_node::repeat_s() const throw ()
{
    return this->repeat_s_.value;
}

/**
 * @brief Get the repeatT flag.
 *
 * @return @c true if the texture should repeat in the <var>T</var> direction,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::abstract_texture_node::repeat_t() const throw ()
{
    return this->repeat_t_.value;
}


/**
 * @class openvrml::vrml97_node::anchor_class
 *
 * @brief Class object for Anchor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this anchor_class.
 */
openvrml::vrml97_node::anchor_class::anchor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::anchor_class::~anchor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a node_type_ptr to a node_type capable of creating Anchor nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by anchor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::anchor_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "description"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "parameter"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "url"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    typedef vrml97_node_type_impl<anchor_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & anchorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            anchorNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        anchor_node::add_children_listener>(
                            &anchor_node::add_children_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            anchorNodeType.add_eventin(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        anchor_node::remove_children_listener>(
                            &anchor_node::remove_children_listener_)));
        } else if (*interface == supportedInterfaces[2]) {
            anchorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        anchor_node::children_exposedfield>(
                            &anchor_node::children_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                        anchor_node::children_exposedfield>(
                            &anchor_node::children_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                        anchor_node::children_exposedfield>(
                            &anchor_node::children_)));
        } else if (*interface == supportedInterfaces[3]) {
            anchorNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sfstring> >(
                            &anchor_node::description_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfstring> >(
                        &anchor_node::description_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                        exposedfield<sfstring> >(
                            &anchor_node::description_)));
        } else if (*interface == supportedInterfaces[4]) {
            anchorNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mfstring> >(
                            &anchor_node::parameter_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfstring> >(
                        &anchor_node::parameter_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                        exposedfield<mfstring> >(
                            &anchor_node::parameter_)));
        } else if (*interface == supportedInterfaces[5]) {
            anchorNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mfstring> >(
                            &anchor_node::url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfstring> >(
                        &anchor_node::url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                        exposedfield<mfstring> >(
                            &anchor_node::url_)));
        } else if (*interface == supportedInterfaces[6]) {
            anchorNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &anchor_node::bbox_center_)));
        } else if (*interface == supportedInterfaces[7]) {
            anchorNodeType.add_field(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &anchor_node::bbox_size_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::anchor_node
 *
 * @brief Represents Anchor node instances.
 */

/**
 * @var openvrml::vrml97_node::anchor_node::anchor_class
 *
 * @brief Class object for Anchor nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfstring> openvrml::vrml97_node::anchor_node::description_
 *
 * @brief description exposedField
 */

/**
 * @var openvrml::exposedfield<openvrml::mfstring> openvrml::vrml97_node::anchor_node::parameter_
 *
 * @brief parameter exposedField
 */

/**
 * @var openvrml::exposedfield<openvrml::mfstring> openvrml::vrml97_node::anchor_node::url_
 *
 * @brief url exposedField
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::anchor_node::
anchor_node(const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    group_node(type, scope),
    description_(*this),
    parameter_(*this),
    url_(*this)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::anchor_node::~anchor_node() throw ()
{}

/**
 * @brief Cast to an anchor.
 *
 * @return a pointer to this node.
 */
openvrml::vrml97_node::anchor_node *
openvrml::vrml97_node::anchor_node::to_anchor() const
{
    return const_cast<anchor_node *>(this);
}

/**
 * @brief Render the node.
 *
 * @param viewer    a viewer.
 * @param context   a rendering context.
 */
void
openvrml::vrml97_node::anchor_node::
do_render_child(openvrml::viewer & viewer, const rendering_context context)
{
    viewer.set_sensitive(this);

    // Render children
    this->group_node::do_render_child(viewer, context);

    viewer.set_sensitive(0);
}

/**
 * @brief Handle a click by loading the url.
 */
void openvrml::vrml97_node::anchor_node::activate()
{
    assert(this->scene());
    this->scene()->load_url(this->url_.mfstring::value,
                            this->parameter_.mfstring::value);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const openvrml::bounding_volume &
openvrml::vrml97_node::anchor_node::do_bounding_volume() const
{
    return this->group_node::bounding_volume();
}

/**
 * @class openvrml::vrml97_node::appearance_class
 *
 * @brief Class object for Appearance nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::appearance_class::
appearance_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::appearance_class::~appearance_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Appearance
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface
 *                                  not supported by appearance_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::appearance_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "material"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "texture"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "textureTransform")
    };

    typedef vrml97_node_type_impl<appearance_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & appearanceNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            appearanceNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &appearance_node::material_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &appearance_node::material_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &appearance_node::material_)));
        } else if (*interface == supportedInterfaces[1]) {
            appearanceNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &appearance_node::texture_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &appearance_node::texture_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &appearance_node::texture_)));
        } else if (*interface == supportedInterfaces[2]) {
            appearanceNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &appearance_node::texture_transform_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &appearance_node::texture_transform_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &appearance_node::texture_transform_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::appearance_node
 *
 * @brief Appearance node instances.
 */

/**
 * @var openvrml::vrml97_node::appearance_node::appearance_class
 *
 * @brief Class object for Appearance nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::appearance_node::material_
 *
 * @brief material exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::appearance_node::texture_
 *
 * @brief texture exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::appearance_node::texture_transform_
 *
 * @brief textureTransform exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::appearance_node::
appearance_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::appearance_node(type, scope),
    material_(*this),
    texture_(*this),
    texture_transform_(*this)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::appearance_node::~appearance_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *         @c false otherwise.
 */
bool openvrml::vrml97_node::appearance_node::modified() const
{
    return (this->node::modified()
            || (this->material_.sfnode::value
                && this->material_.sfnode::value->modified())
            || (this->texture_.sfnode::value
                && this->texture_.sfnode::value->modified())
            || (this->texture_transform_.sfnode::value
                && this->texture_transform_.sfnode::value->modified()));
}

/**
 * @brief Get the material node.
 *
 * @returns an sfnode object containing the Material node associated with
 *          this Appearance.
 */
const openvrml::node_ptr &
openvrml::vrml97_node::appearance_node::material() const throw ()
{
    return this->material_.sfnode::value;
}

/**
 * @brief Get the texture node.
 *
 * @return an sfnode object containing the texture node associated with
 *         this Appearance.
 */
const openvrml::node_ptr &
openvrml::vrml97_node::appearance_node::texture() const throw ()
{
    return this->texture_.sfnode::value;
}

/**
 * @brief Get the texture transform node.
 *
 * @return an sfnode object containing the TextureTransform node
 *         associated with this Appearance.
 */
const openvrml::node_ptr &
openvrml::vrml97_node::appearance_node::texture_transform() const throw ()
{
    return this->texture_transform_.sfnode::value;
}

namespace {
    void set_unlit_material(openvrml::viewer & v)
    {
        using openvrml::color;
        static const float unlit_ambient_intensity(1);
        static const color unlit_diffuse_color(1, 1, 1);
        static const color unlit_emissive_color(1, 1, 1);
        static const float unlit_shininess(0);
        static const color unlit_specular_color(1, 1, 1);
        static const float unlit_transparency(0);
        v.set_material(unlit_ambient_intensity,
                       unlit_diffuse_color,
                       unlit_emissive_color,
                       unlit_shininess,
                       unlit_specular_color,
                       unlit_transparency);
    }
}

/**
 * @brief render_appearance implementation.
 *
 * @param v         viewer.
 * @param context   rendering_context.
 */
void
openvrml::vrml97_node::appearance_node::
do_render_appearance(viewer & v, rendering_context context)
{
    openvrml::material_node * const material =
        node_cast<openvrml::material_node *>(
            this->material_.sfnode::value.get());
    texture_node * const texture =
        node_cast<texture_node *>(this->texture_.sfnode::value.get());

    if (material) {
        float trans = material->transparency();
        color diffuse = material->diffuse_color();
        size_t nTexComponents = texture ? texture->image().comp() : 0;
        if (nTexComponents == 2 || nTexComponents == 4) { trans = 0.0; }
        if (nTexComponents >= 3) { diffuse = color(1.0, 1.0, 1.0); }

        v.enable_lighting(true);   // turn lighting on for this object
        v.set_material(material->ambient_intensity(),
                       diffuse,
                       material->emissive_color(),
                       material->shininess(),
                       material->specular_color(),
                       trans);

        material->modified(false);
    } else {
        v.enable_lighting(false);   // turn lighting off for this object
        set_unlit_material(v);
    }

    if (texture) {
        openvrml::texture_transform_node * texture_transform =
            node_cast<openvrml::texture_transform_node *>(
                this->texture_transform_.sfnode::value.get());
        if (texture_transform) {
            texture_transform->render_texture_transform(v);
        } else {
            static const vec2f center(0.0, 0.0);
            static const float rotation = 0.0;
            static const vec2f scale(1.0, 1.0);
            static const vec2f translation(0.0, 0.0);
            v.set_texture_transform(center, rotation, scale, translation);
        }
        texture->render_texture(v);
    }
    this->node::modified(false);
}

/**
 * @class openvrml::vrml97_node::audio_clip_class
 *
 * @brief Class object for AudioClip nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::audio_clip_class::
audio_clip_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::audio_clip_class::~audio_clip_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating AudioClip nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by audio_clip_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::audio_clip_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "description"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "loop"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "pitch"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "startTime"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "stopTime"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "url"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "duration_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive")
    };

    typedef vrml97_node_type_impl<audio_clip_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & audioClipNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sfstring> >(
                            &audio_clip_node::description_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfstring> >(
                        &audio_clip_node::description_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                        exposedfield<sfstring> >(
                            &audio_clip_node::description_)));
        } else if (*interface == supportedInterfaces[1]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &audio_clip_node::loop_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &audio_clip_node::loop_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &audio_clip_node::loop_)));
        } else if (*interface == supportedInterfaces[2]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sffloat> >(
                            &audio_clip_node::pitch_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &audio_clip_node::pitch_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &audio_clip_node::pitch_)));
        } else if (*interface == supportedInterfaces[3]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sftime> >(
                        &audio_clip_node::start_time_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sftime> >(
                        &audio_clip_node::start_time_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sftime> >(
                        &audio_clip_node::start_time_)));
        } else if (*interface == supportedInterfaces[4]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sftime> >(
                        &audio_clip_node::stop_time_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sftime> >(
                        &audio_clip_node::stop_time_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sftime> >(
                        &audio_clip_node::stop_time_)));
        } else if (*interface == supportedInterfaces[5]) {
            audioClipNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mfstring> >(
                            &audio_clip_node::url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfstring> >(
                        &audio_clip_node::url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                        exposedfield<mfstring> >(
                            &audio_clip_node::url_)));
        } else if (*interface == supportedInterfaces[6]) {
            audioClipNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &audio_clip_node::duration_changed_emitter_)));
        } else if (*interface == supportedInterfaces[7]) {
            audioClipNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &audio_clip_node::is_active_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::audio_clip_node
 *
 * @brief AudioClip node instances.
 *
 * @todo Implement sound support.
 */

/**
 * @var openvrml::vrml97_node::audio_clip_node::audio_clip_class
 *
 * @brief Class object for AudioClip nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfstring> openvrml::vrml97_node::audio_clip_node::description_
 *
 * @brief description exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::audio_clip_node::loop_
 *
 * @brief loop exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::audio_clip_node::pitch_
 *
 * @brief pitch exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sftime> openvrml::vrml97_node::audio_clip_node::start_time_
 *
 * @brief startTime exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sftime> openvrml::vrml97_node::audio_clip_node::stop_time_
 *
 * @brief stopTime exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfstring> openvrml::vrml97_node::audio_clip_node::url_
 *
 * @brief url exposedField.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::audio_clip_node::duration_changed_
 *
 * @brief duration_changed eventOut value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::audio_clip_node::duration_changed_emitter_
 *
 * @brief duration_changed eventOut emitter.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::audio_clip_node::is_active_
 *
 * @brief isActive eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::audio_clip_node::is_active_emitter_
 *
 * @brief isActive eventOut emitter.
 */

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::audio_clip_node::
audio_clip_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    description_(*this),
    loop_(*this),
    pitch_(*this, 1.0),
    start_time_(*this),
    stop_time_(*this),
    url_(*this),
    duration_changed_emitter_(this->duration_changed_),
    is_active_emitter_(this->is_active_)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::audio_clip_node::~audio_clip_node() throw ()
{}

/**
 * @brief Cast to an audio_clip_node node.
 *
 * @return a pointer to this node.
 */
openvrml::vrml97_node::audio_clip_node *
openvrml::vrml97_node::audio_clip_node::to_audio_clip() const
{
    return (audio_clip_node*)this;
}

/**
 * @brief Called to update the AudioClip for the current time.
 *
 * @param time  the current time.
 *
 * @todo Implement me!
 */
void openvrml::vrml97_node::audio_clip_node::update(double time)
{}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::audio_clip_node::do_initialize(const double timestamp)
    throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser().add_audio_clip(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void
openvrml::vrml97_node::audio_clip_node::do_shutdown(const double timestamp)
    throw ()
{
    assert(this->scene());
    this->scene()->browser().remove_audio_clip(*this);
}


/**
 * @class openvrml::vrml97_node::background_class
 *
 * @brief Class object for Background nodes.
 */

/**
 * @typedef openvrml::vrml97_node::background_class::bound_nodes_t
 *
 * @brief A bound Background node stack.
 */

/**
 * @var openvrml::vrml97_node::background_node * openvrml::vrml97_node::background_class::first
 *
 * @brief The first Background node in the initial scene graph.
 */

/**
 * @var openvrml::vrml97_node::background_class::bound_nodes_t openvrml::vrml97_node::background_class::bound_nodes
 *
 * @brief The bound Background node stack.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::background_class::
background_class(openvrml::browser & browser):
    node_class(browser),
    first(0)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::background_class::~background_class() throw ()
{}

/**
 * @brief Set the first Background node in the world.
 *
 * The first Background node in the world is used as the initial background.
 * This method is used by background_node::do_initialize.
 *
 * @param background    a Background node.
 */
void
openvrml::vrml97_node::background_class::
set_first(background_node & background) throw ()
{
    this->first = &background;
}

/**
 * @brief Reset the pointer to the first Background node in the world to null.
 *
 * This function is called by <code>background_node::do_shutdown</code>.
 */
void openvrml::vrml97_node::background_class::reset_first() throw ()
{
    this->first = 0;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by background_node::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false otherwise.
 */
bool openvrml::vrml97_node::background_class::has_first() const throw ()
{
    return this->first;
}

/**
 * @brief Check to see if a node is registered as the "first" node.
 *
 * @param background    a background_node.
 *
 * @return @c true if @p background is the fist node; @c false otherwise.
 */
bool
openvrml::vrml97_node::background_class::is_first(background_node & background)
    throw ()
{
    return &background == this->first;
}

/**
 * @brief Push a background_node on the top of the bound node stack.
 *
 * @param background    the node to bind.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::background_class::bind(background_node & background,
                                              const double timestamp)
    throw (std::bad_alloc)
{
    using std::find;

    //
    // If the node is already the active node, do nothing.
    //
    if (!this->bound_nodes.empty() && &background == this->bound_nodes.back())
    {
        return;
    }

    //
    // If the node is already on the stack, remove it.
    //
    const bound_nodes_t::iterator pos =
        find(this->bound_nodes.begin(), this->bound_nodes.end(), &background);
    if (pos != this->bound_nodes.end()) { this->bound_nodes.erase(pos); }

    //
    // Send FALSE from the currently active node's isBound.
    //
    if (!this->bound_nodes.empty()) {
        background_node & current =
            dynamic_cast<background_node &>(*this->bound_nodes.back());
        current.is_bound_.value = false;
        node::emit_event(current.is_bound_emitter_, timestamp);
    }

    //
    // Push the node to the top of the stack, and have it send isBound TRUE.
    //
    this->bound_nodes.push_back(&background);
    background.is_bound_.value = true;
    node::emit_event(background.is_bound_emitter_, timestamp);
}

/**
 * @brief Remove a background_node from the bound node stack.
 *
 * @param background    the node to unbind.
 * @param timestamp     the current time.
 */
void
openvrml::vrml97_node::background_class::unbind(background_node & background,
                                                const double timestamp)
    throw ()
{
    using std::find;

    const bound_nodes_t::iterator pos =
        find(this->bound_nodes.begin(), this->bound_nodes.end(), &background);
    if (pos != this->bound_nodes.end()) {
        background.is_bound_.value = false;
        node::emit_event(background.is_bound_emitter_, timestamp);

        if (pos == this->bound_nodes.end() - 1
            && this->bound_nodes.size() > 1) {
            background_node & newActive =
                dynamic_cast<background_node &>(
                    **(this->bound_nodes.end() - 2));
            newActive.is_bound_.value = true;
            node::emit_event(newActive.is_bound_emitter_, timestamp);
        }
        this->bound_nodes.erase(pos);
    }
}

/**
 * @brief node_class-specific initialization.
 *
 * @param initial_viewpoint the viewpoint_node that should be bound initially.
 * @param timestamp         the current time.
 */
void
openvrml::vrml97_node::background_class::
do_initialize(openvrml::viewpoint_node * initial_viewpoint,
              const double timestamp)
    throw ()
{
    if (this->first) {
        try {
            event_listener & listener =
                this->first->event_listener("set_bind");
            assert(dynamic_cast<sfbool_listener *>(&listener));
            static_cast<sfbool_listener &>(listener)
                .process_event(sfbool(true), timestamp);
        } catch (unsupported_interface & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }
}

/**
 * @brief node_class-specific rendering.
 *
 * Render the active Background node.
 *
 * @param v viewer.
 */
void openvrml::vrml97_node::background_class::do_render(viewer & v) const
    throw ()
{
    if (this->bound_nodes.empty()) {
        //
        // Default background.
        //
        using std::vector;
        static const vector<float> ground_angle;
        static const vector<color> ground_color;
        static const vector<float> sky_angle;
        static const vector<color> sky_color;
        static const image front;
        static const image back;
        static const image left;
        static const image right;
        static const image top;
        static const image bottom;
        v.insert_background(ground_angle,
                            ground_color,
                            sky_angle,
                            sky_color,
                            front,
                            back,
                            left,
                            right,
                            top,
                            bottom);
    } else {
        assert(this->bound_nodes.back());
        background_node & background = *this->bound_nodes.back();

        // Background isn't selectable, so don't waste the time.
        if (v.mode() == viewer::pick_mode) { return; }

        if (background.viewerObject && background.modified()) {
            v.remove_object(background.viewerObject);
            background.viewerObject = 0;
        }

        if (background.viewerObject) {
            v.insert_reference(background.viewerObject);
        } else {
            background.update_textures();

            background.viewerObject =
                v.insert_background(background.ground_angle_.mffloat::value,
                                    background.ground_color_.mfcolor::value,
                                    background.sky_angle_.mffloat::value,
                                    background.sky_color_.mfcolor::value,
                                    background.front,
                                    background.back,
                                    background.left,
                                    background.right,
                                    background.top,
                                    background.bottom);

            background.modified(false);
        }
    }
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Background nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by background_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::background_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::sfbool_id,
                       "set_bind"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "groundAngle"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolor_id,
                       "groundColor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "backUrl"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "bottomUrl"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "frontUrl"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "leftUrl"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "rightUrl"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "topUrl"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "skyAngle"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolor_id,
                       "skyColor"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isBound")
    };

    typedef vrml97_node_type_impl<background_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & backgroundNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            backgroundNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        background_node::set_bind_listener>(
                            &background_node::set_bind_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mffloat> >(
                            &background_node::ground_angle_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mffloat> >(
                        &background_node::ground_angle_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mffloat> >(
                        &background_node::ground_angle_)));
        } else if (*interface == supportedInterfaces[2]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mfcolor> >(
                        &background_node::ground_color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfcolor> >(
                        &background_node::ground_color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mfcolor> >(
                        &background_node::ground_color_)));
        } else if (*interface == supportedInterfaces[3]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::back_url_exposedfield>(
                        &background_node::back_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    background_node::back_url_exposedfield>(
                        &background_node::back_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::back_url_exposedfield>(
                        &background_node::back_url_)));
        } else if (*interface == supportedInterfaces[4]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::bottom_url_exposedfield>(
                        &background_node::bottom_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    background_node::bottom_url_exposedfield>(
                        &background_node::bottom_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::bottom_url_exposedfield>(
                        &background_node::bottom_url_)));
        } else if (*interface == supportedInterfaces[5]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::front_url_exposedfield>(
                        &background_node::front_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                        background_node::front_url_exposedfield>(
                            &background_node::front_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::front_url_exposedfield>(
                        &background_node::front_url_)));
        } else if (*interface == supportedInterfaces[6]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::left_url_exposedfield>(
                        &background_node::left_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    background_node::left_url_exposedfield>(
                        &background_node::left_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::left_url_exposedfield>(
                        &background_node::left_url_)));
        } else if (*interface == supportedInterfaces[7]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::right_url_exposedfield>(
                        &background_node::right_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    background_node::right_url_exposedfield>(
                        &background_node::right_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::right_url_exposedfield>(
                        &background_node::right_url_)));
        } else if (*interface == supportedInterfaces[8]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    background_node::top_url_exposedfield>(
                        &background_node::top_url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    background_node::top_url_exposedfield>(
                        &background_node::top_url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    background_node::top_url_exposedfield>(
                        &background_node::top_url_)));
        } else if (*interface == supportedInterfaces[9]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mffloat> >(&background_node::sky_angle_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mffloat> >(
                        &background_node::sky_angle_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mffloat> >(
                        &background_node::sky_angle_)));
        } else if (*interface == supportedInterfaces[10]) {
            backgroundNodeType.add_exposedfield(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mfcolor> >(&background_node::sky_color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfcolor> >(
                        &background_node::sky_color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mfcolor> >(
                        &background_node::sky_color_)));
        } else if (*interface == supportedInterfaces[11]) {
            backgroundNodeType.add_eventout(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &background_node::is_bound_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::background_node
 *
 * @brief Background node instances.
 */

/**
 * @var openvrml::vrml97_node::background_node::background_class
 *
 * @brief Class object for Background nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::background_node::set_bind_listener
 *
 * @brief set_bind eventIn listener.
 */

/**
 * @brief Construct.
 *
 * @param node background_node.
 */
openvrml::vrml97_node::background_node::set_bind_listener::
set_bind_listener(background_node & node):
    sfbool_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::background_node::set_bind_listener::~set_bind_listener()
    throw ()
{}

/**
 * @brief Process an event.
 *
 * @param value     event value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::background_node::set_bind_listener::
do_process_event(const sfbool & value, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        background_node & node = dynamic_cast<background_node &>(this->node());
        background_class & node_class =
            const_cast<background_class &>(
                static_cast<const background_class &>(
                    node.type().node_class()));
        if (value.value) {
            node_class.bind(node, timestamp);
        } else {
            node_class.unbind(node, timestamp);
        }
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::background_node::back_url_exposedfield
 *
 * @brief backUrl exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  background_node.
 */
openvrml::vrml97_node::background_node::back_url_exposedfield::
back_url_exposedfield(background_node & node) throw ():
    openvrml::exposedfield<openvrml::mfstring>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::background_node::back_url_exposedfield::
~back_url_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param value     new backUrl value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::background_node::back_url_exposedfield::
event_side_effect(const mfstring & value,
                  const double timestamp)
    throw (std::bad_alloc)
{
    try {
        dynamic_cast<background_node &>(this->node()).back_needs_update = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::background_node::bottom_url_exposedfield
 *
 * @brief bottomUrl exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  background_node.
 */
openvrml::vrml97_node::background_node::bottom_url_exposedfield::
bottom_url_exposedfield(background_node & node) throw ():
    openvrml::exposedfield<openvrml::mfstring>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::background_node::bottom_url_exposedfield::
~bottom_url_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param value     new bottomUrl value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::background_node::bottom_url_exposedfield::
event_side_effect(const mfstring & value,
                  const double timestamp)
    throw (std::bad_alloc)
{
    try {
        dynamic_cast<background_node &>(this->node()).bottom_needs_update = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::background_node::front_url_exposedfield
 *
 * @brief frontUrl exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  background_node.
 */
openvrml::vrml97_node::background_node::front_url_exposedfield::
front_url_exposedfield(background_node & node) throw ():
    openvrml::exposedfield<openvrml::mfstring>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::background_node::front_url_exposedfield::
~front_url_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param value     new frontUrl value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::background_node::front_url_exposedfield::
event_side_effect(const mfstring & value,
                  const double timestamp)
    throw (std::bad_alloc)
{
    try {
        dynamic_cast<background_node &>(this->node()).front_needs_update = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::background_node::left_url_exposedfield
 *
 * @brief leftUrl exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  background_node.
 */
openvrml::vrml97_node::background_node::left_url_exposedfield::
left_url_exposedfield(background_node & node) throw ():
    openvrml::exposedfield<openvrml::mfstring>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::background_node::left_url_exposedfield::
~left_url_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param value     new leftUrl value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::background_node::left_url_exposedfield::
event_side_effect(const mfstring & value,
                  const double timestamp)
    throw (std::bad_alloc)
{
    try {
        dynamic_cast<background_node &>(this->node()).left_needs_update = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::background_node::right_url_exposedfield
 *
 * @brief rightUrl exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  background_node.
 */
openvrml::vrml97_node::background_node::right_url_exposedfield::
right_url_exposedfield(background_node & node) throw ():
    openvrml::exposedfield<openvrml::mfstring>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::background_node::right_url_exposedfield::
~right_url_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param value     new rightUrl value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::background_node::right_url_exposedfield::
event_side_effect(const mfstring & value,
                  const double timestamp)
    throw (std::bad_alloc)
{
    try {
        dynamic_cast<background_node &>(this->node()).right_needs_update = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::background_node::top_url_exposedfield
 *
 * @brief topUrl exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  background_node.
 */
openvrml::vrml97_node::background_node::top_url_exposedfield::
top_url_exposedfield(background_node & node) throw ():
    openvrml::exposedfield<openvrml::mfstring>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::background_node::top_url_exposedfield::
~top_url_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param value     new topUrl value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::background_node::top_url_exposedfield::
event_side_effect(const mfstring & value,
                 const double timestamp)
    throw (std::bad_alloc)
{
    try {
        dynamic_cast<background_node &>(this->node()).top_needs_update = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::background_node::set_bind_listener openvrml::vrml97_node::background_node::set_bind_listener_
 *
 * @brief set_bind eventIn handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::mffloat> openvrml::vrml97_node::background_node::ground_angle_
 *
 * @brief groundAngle exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfcolor> openvrml::vrml97_node::background_node::ground_color_
 *
 * @brief groundColor exposedField.
 */

/**
 * @var background_node::back_url_exposedfield openvrml::vrml97_node::background_node::back_url_
 *
 * @brief backUrl exposedField.
 */

/**
 * @var background_node::bottom_url_exposedfield openvrml::vrml97_node::background_node::bottom_url_
 *
 * @brief bottomUrl exposedField.
 */

/**
 * @var background_node::front_url_exposedfield openvrml::vrml97_node::background_node::front_url_
 *
 * @brief frontUrl exposedField.
 */

/**
 * @var background_node::left_url_exposedfield openvrml::vrml97_node::background_node::left_url_
 *
 * @brief leftUrl exposedField.
 */

/**
 * @var background_node::right_url_exposedfield openvrml::vrml97_node::background_node::right_url_
 *
 * @brief rightUrl exposedField.
 */

/**
 * @var background_node::top_url_exposedfield openvrml::vrml97_node::background_node::top_url_
 *
 * @brief topUrl exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mffloat> openvrml::vrml97_node::background_node::sky_angle_
 *
 * @brief skyAngle exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfcolor> openvrml::vrml97_node::background_node::sky_color_
 *
 * @brief skyColor exposedField.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::background_node::is_bound_
 *
 * @brief isBound eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::background_node::is_bound_emitter_
 *
 * @brief isBound eventOut emitter.
 */

/**
 * @var openvrml::image openvrml::vrml97_node::background_node::front
 *
 * @brief Front image.
 */

/**
 * @var bool openvrml::vrml97_node::background_node::front_needs_update
 *
 * @brief Flag to indicate that the front image needs to be updated.
 */

/**
 * @var openvrml::image openvrml::vrml97_node::background_node::back
 *
 * @brief Back image.
 */

/**
 * @var bool openvrml::vrml97_node::background_node::back_needs_update
 *
 * @brief Flag to indicate that the back image needs to be updated.
 */

/**
 * @var openvrml::image openvrml::vrml97_node::background_node::left
 *
 * @brief Left image.
 */

/**
 * @var bool openvrml::vrml97_node::background_node::left_needs_update
 *
 * @brief Flag to indicate that the left image needs to be updated.
 */

/**
 * @var openvrml::image openvrml::vrml97_node::background_node::right
 *
 * @brief Right image.
 */

/**
 * @var bool openvrml::vrml97_node::background_node::right_needs_update
 *
 * @brief Flag to indicate that the right image needs to be updated.
 */

/**
 * @var openvrml::image openvrml::vrml97_node::background_node::top
 *
 * @brief Top image.
 */

/**
 * @var bool openvrml::vrml97_node::background_node::top_needs_update
 *
 * @brief Flag to indicate that the top image needs to be updated.
 */

/**
 * @var openvrml::image openvrml::vrml97_node::background_node::bottom
 *
 * @brief Bottom image.
 */

/**
 * @var bool openvrml::vrml97_node::background_node::bottom_needs_update
 *
 * @brief Flag to indicate that the bottom image needs to be updated.
 */

/**
 * @var openvrml::viewer::object_t openvrml::vrml97_node::background_node::viewerObject
 *
 * @brief Handle for the renderer.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::background_node::
background_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    set_bind_listener_(*this),
    ground_angle_(*this),
    ground_color_(*this),
    back_url_(*this),
    bottom_url_(*this),
    front_url_(*this),
    left_url_(*this),
    right_url_(*this),
    top_url_(*this),
    sky_angle_(*this),
    sky_color_(*this, std::vector<color>(1, color(0.0, 0.0, 0.0))),
    is_bound_emitter_(this->is_bound_),
    front_needs_update(true),
    back_needs_update(true),
    left_needs_update(true),
    right_needs_update(true),
    top_needs_update(true),
    bottom_needs_update(true),
    viewerObject(0)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::background_node::~background_node() throw ()
{
    // remove d_viewerObject...
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void
openvrml::vrml97_node::background_node::do_initialize(const double timestamp)
    throw ()
{
    assert(dynamic_cast<const background_class *>(&this->type().node_class()));
    background_class & nodeClass =
        const_cast<background_class &>(
            static_cast<const background_class &>(this->type().node_class()));
    if (!nodeClass.has_first()) { nodeClass.set_first(*this); }
}

/**
 * @brief Shut down.
 *
 * Calls background_class::unbind to unbind the node if it is bound.
 *
 * @param timestamp the current time.
 */
void
openvrml::vrml97_node::background_node::do_shutdown(const double timestamp)
    throw ()
{
    background_class & node_class =
        const_cast<background_class &>(
            static_cast<const background_class &>(this->type().node_class()));
    node_class.unbind(*this, timestamp);

    if (node_class.is_first(*this)) { node_class.reset_first(); }
}

/**
 * @brief Called lazily to update texture data.
 */
void openvrml::vrml97_node::background_node::update_textures()
{
    if (this->front_needs_update) {
        if (this->front_url_.mfstring::value.empty()) {
            this->front = image();
        } else {
            using std::auto_ptr;
            auto_ptr<resource_istream>
                in(this->scene()
                   ->get_resource(this->front_url_.mfstring::value));
            auto_ptr<stream_listener>
                listener(new image_stream_listener(this->front,
                                                   *this,
                                                   this->mutex()));
            read_stream(in, listener);
        }
        this->front_needs_update = false;
    }
    if (this->back_needs_update) {
        if (this->back_url_.mfstring::value.empty()) {
            this->back = image();
        } else {
            using std::auto_ptr;
            auto_ptr<resource_istream>
                in(this->scene()
                   ->get_resource(this->back_url_.mfstring::value));
            auto_ptr<stream_listener>
                listener(new image_stream_listener(this->back,
                                                   *this,
                                                   this->mutex()));
            read_stream(in, listener);
        }
        this->back_needs_update = false;
    }
    if (this->left_needs_update) {
        if (this->left_url_.mfstring::value.empty()) {
            this->left = image();
        } else {
            using std::auto_ptr;
            auto_ptr<resource_istream>
                in(this->scene()
                   ->get_resource(this->left_url_.mfstring::value));
            auto_ptr<stream_listener>
                listener(new image_stream_listener(this->left,
                                                   *this,
                                                   this->mutex()));
            read_stream(in, listener);
        }
        this->left_needs_update = false;
    }
    if (this->right_needs_update) {
        if (this->right_url_.mfstring::value.empty()) {
            this->right = image();
        } else {
            using std::auto_ptr;
            auto_ptr<resource_istream>
                in(this->scene()
                   ->get_resource(this->right_url_.mfstring::value));
            auto_ptr<stream_listener>
                listener(new image_stream_listener(this->right,
                                                   *this,
                                                   this->mutex()));
            read_stream(in, listener);
        }
        this->right_needs_update = false;
    }
    if (this->top_needs_update) {
        if (this->top_url_.mfstring::value.empty()) {
            this->top = image();
        } else {
            using std::auto_ptr;
            auto_ptr<resource_istream>
                in(this->scene()
                   ->get_resource(this->top_url_.mfstring::value));
            auto_ptr<stream_listener>
                listener(new image_stream_listener(this->top,
                                                   *this,
                                                   this->mutex()));
            read_stream(in, listener);
        }
        this->top_needs_update = false;
    }
    if (this->bottom_needs_update) {
        if (this->bottom_url_.mfstring::value.empty()) {
            this->bottom = image();
        } else {
            using std::auto_ptr;
            auto_ptr<resource_istream>
                in(this->scene()
                   ->get_resource(this->bottom_url_.mfstring::value));
            auto_ptr<stream_listener>
                listener(new image_stream_listener(this->bottom,
                                                   *this,
                                                   this->mutex()));
            read_stream(in, listener);
        }
        this->bottom_needs_update = false;
    }
}


/**
 * @class billboard_class
 *
 * @brief Class object for Billboard nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::billboard_class::
billboard_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::billboard_class::~billboard_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Billboard nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by billboard_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::billboard_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
                       "bboxSize")
    };

    typedef vrml97_node_type_impl<billboard_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & billboardNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            billboardNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        billboard_node::add_children_listener>(
                            &billboard_node::add_children_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            billboardNodeType.add_eventin(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        billboard_node::remove_children_listener>(
                            &billboard_node::remove_children_listener_)));
        } else if (*interface == supportedInterfaces[2]) {
            billboardNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(
                        &billboard_node::axis_of_rotation_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &billboard_node::axis_of_rotation_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &billboard_node::axis_of_rotation_)));
        } else if (*interface == supportedInterfaces[3]) {
            billboardNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
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
        } else if (*interface == supportedInterfaces[4]) {
            billboardNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &billboard_node::bbox_center_)));
        } else if (*interface == supportedInterfaces[5]) {
            billboardNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &billboard_node::bbox_size_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::billboard_node
 *
 * @brief Billboard node instances.
 */

/**
 * @var openvrml::vrml97_node::billboard_node::billboard_class
 *
 * @brief Class object for Billboard nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::billboard_node::axis_of_rotation_
 *
 * @brief axisOfRotation exposedField.
 */

/**
 * @var openvrml::viewer::object_t openvrml::vrml97_node::billboard_node::xformObject
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
openvrml::vrml97_node::billboard_node::
billboard_to_matrix(const billboard_node & node, const mat4f & modelview)
{
    const mat4f inverse_modelview = modelview.inverse();

    mat4f result;

    // Viewer position in local coordinate system
    const vec3f position = vec3f(inverse_modelview[3][0],
                                 inverse_modelview[3][1],
                                 inverse_modelview[3][2]).normalize();

    // Viewer-alignment
    if ((node.axis_of_rotation_.sfvec3f::value[0] == 0)
        && (node.axis_of_rotation_.sfvec3f::value[1] == 0)
        && (node.axis_of_rotation_.sfvec3f::value[2] == 0)) {
        //
        // Viewer's up vector
        //
        const vec3f up = vec3f(inverse_modelview[1][0],
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
        const vec3f Y(node.axis_of_rotation_.sfvec3f::value);

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
        result = mat4f::rotation(rotation(Y, angle));
    }
    return result;
}

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::billboard_node::
billboard_node(const node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    group_node(type, scope),
    axis_of_rotation_(*this, vec3f(0.0, 1.0, 0.0)),
    xformObject(0)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::billboard_node::~billboard_node() throw ()
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
openvrml::vrml97_node::billboard_node::
do_render_child(openvrml::viewer & viewer, rendering_context context)
{
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
    } else if (this->children_.mfnode::value.size() > 0) {
        this->xformObject = viewer.begin_object(this->id().c_str());

        viewer.transform(LM);

        // Render children
        this->group_node::do_render_child(viewer, context);

        viewer.end_object();
    }

    this->node::modified(false);
}


/**
 * @class openvrml::vrml97_node::box_class
 *
 * @brief Class object for Box nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::box_class::box_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::box_class::~box_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Box nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by box_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::box_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "size");

    typedef vrml97_node_type_impl<box_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & boxNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterface) {
            boxNodeType.add_field(
                supportedInterface.field_type,
                supportedInterface.id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(&box_node::size)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::box_node
 *
 * @brief Box node instances.
 */

/**
 * @var openvrml::vrml97_node::box_node::box_class
 *
 * @brief Class object for Box nodes.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::box_node::size
 *
 * @brief size field.
 */

/**
 * @var openvrml::bounding_sphere openvrml::vrml97_node::box_node::bsphere
 *
 * @brief Bounding volume.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::box_node::
box_node(const node_type & type,
         const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    geometry_node(type, scope),
    size(vec3f(2.0, 2.0, 2.0))
{
    this->bounding_volume_dirty(true); // lazy calc of bvolume
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::box_node::~box_node() throw ()
{}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 *
 * @return display object identifier.
 */
openvrml::viewer::object_t
openvrml::vrml97_node::box_node::
do_render_geometry(openvrml::viewer & viewer, const rendering_context context)
{
    const viewer::object_t object_ref = viewer.insert_box(this->size.value);
    return object_ref;
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const openvrml::bounding_volume &
openvrml::vrml97_node::box_node::do_bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const vec3f corner = this->size.value / 2.0f;
        const float r = corner.length();
        const_cast<box_node *>(this)->bsphere.radius(r);
        const_cast<box_node *>(this)->bounding_volume_dirty(false); // logical const
    }
    return this->bsphere;
}


/**
 * @class openvrml::vrml97_node::collision_class
 *
 * @brief Class object for Collision nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::collision_class::
collision_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::collision_class::~collision_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Collision nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by collision_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::collision_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "collide"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize"),
        node_interface(node_interface::field_id,
                       field_value::sfnode_id,
                       "proxy"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "collideTime")
    };

    typedef vrml97_node_type_impl<collision_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & collisionNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            collisionNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        collision_node::add_children_listener>(
                            &collision_node::add_children_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            collisionNodeType.add_eventin(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        collision_node::remove_children_listener>(
                            &collision_node::remove_children_listener_)));
        } else if (*interface == supportedInterfaces[2]) {
            collisionNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        collision_node::children_exposedfield>(
                            &collision_node::children_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                        collision_node::children_exposedfield>(
                            &collision_node::children_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                        collision_node::children_exposedfield>(
                            &collision_node::children_)));
        } else if (*interface == supportedInterfaces[3]) {
            collisionNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &collision_node::collide_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &collision_node::collide_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &collision_node::collide_)));
        } else if (*interface == supportedInterfaces[4]) {
            collisionNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &collision_node::bbox_center_)));
        } else if (*interface == supportedInterfaces[5]) {
            collisionNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &collision_node::bbox_size_)));
        } else if (*interface == supportedInterfaces[6]) {
            collisionNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfnode>(
                        &collision_node::proxy_)));
        } else if (*interface == supportedInterfaces[7]) {
            collisionNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &collision_node::collide_time_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::collision_node
 *
 * @brief Collision node instances.
 */

/**
 * @var openvrml::vrml97_node::collision_node::collision_class
 *
 * @brief Class object for Collision nodes.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::collision_node::collide_
 *
 * @brief collide exposedField.
 */

/**
 * @var openvrml::sfnode openvrml::vrml97_node::collision_node::proxy_
 *
 * @brief proxy field.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::collision_node::collide_time_
 *
 * @brief collideTime eventOut.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::collision_node::collide_time_emitter_
 *
 * @brief collideTime eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::collision_node::
collision_node(const node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    group_node(type, scope),
    collide_(*this, true),
    collide_time_emitter_(this->collide_time_)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::collision_node::~collision_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::collision_node::modified() const
{
    return (this->proxy_.value && this->proxy_.value->modified())
        || this->group_node::modified();
}


/**
 * @class openvrml::vrml97_node::color_class
 *
 * @brief Class object for Color nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml::vrml97_node::color_class::color_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::color_class::~color_class() throw () {}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Color nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by color_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::color_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
        node_interface(node_interface::exposedfield_id,
                       field_value::mfcolor_id,
                       "color");

    typedef vrml97_node_type_impl<color_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & colorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterface) {
            colorNodeType.add_exposedfield(
                supportedInterface.field_type,
                supportedInterface.id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mfcolor> >(
                            &color_node::color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfcolor> >(
                        &color_node::color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mfcolor> >(
                        &color_node::color_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::color_node
 *
 * @brief Color node instances.
 */

/**
 * @var openvrml::vrml97_node::color_node::color_class
 *
 * @brief Class object for Color nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfcolor> openvrml::vrml97_node::color_node::color_
 *
 * @brief color exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::color_node::
color_node(const node_type & type,
           const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::color_node(type, scope),
    color_(*this)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::color_node::~color_node() throw ()
{}

/**
 * @brief Get the color array.
 *
 * @return the color array associated with the node.
 */
const std::vector<openvrml::color> &
openvrml::vrml97_node::color_node::color() const throw ()
{
    return this->color_.mfcolor::value;
}


/**
 * @class openvrml::vrml97_node::color_interpolator_class
 *
 * @brief Class object for ColorInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::color_interpolator_class::
color_interpolator_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::color_interpolator_class::~color_interpolator_class()
    throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating ColorInterpolator
 *      nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by color_interpolator_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::color_interpolator_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                      field_value::sffloat_id,
                      "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                      field_value::mffloat_id,
                      "key"),
        node_interface(node_interface::exposedfield_id,
                      field_value::mfcolor_id,
                      "keyValue"),
        node_interface(node_interface::eventout_id,
                      field_value::sfcolor_id,
                      "value_changed")
    };

    typedef vrml97_node_type_impl<color_interpolator_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & colorInterpolatorNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            colorInterpolatorNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        color_interpolator_node::set_fraction_listener>(
                            &color_interpolator_node::set_fraction_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            colorInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mffloat> >(
                            &color_interpolator_node::key_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mffloat> >(
                        &color_interpolator_node::key_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mffloat> >(
                        &color_interpolator_node::key_)));
        } else if (*interface == supportedInterfaces[2]) {
            colorInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mfcolor> >(
                            &color_interpolator_node::key_value_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfcolor> >(
                        &color_interpolator_node::key_value_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mfcolor> >(
                        &color_interpolator_node::key_value_)));
        } else if (*interface == supportedInterfaces[3]) {
            colorInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfcolor_emitter>(
                        &color_interpolator_node::value_changed_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::color_interpolator_node
 *
 * @brief ColorInterpolator node instances.
 */

/**
 * @var openvrml::vrml97_node::color_interpolator_node::color_interpolator_class
 *
 * @brief Class object for ColorInterpolator nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::color_interpolator_node::set_fraction_listener
 *
 * @brief set_fraction event handler.
 */

/**
 * @brief Construct.
 *
 * @param node color_interpolator_node.
 */
openvrml::vrml97_node::color_interpolator_node::set_fraction_listener::
set_fraction_listener(color_interpolator_node & node):
    sffloat_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::color_interpolator_node::set_fraction_listener::
~set_fraction_listener()
    throw ()
{}

/**
 * @brief Process event.
 *
 * @param value     new fraction value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::color_interpolator_node::set_fraction_listener::
do_process_event(const sffloat & value, const double timestamp)
    throw (std::bad_alloc)
{
    using std::vector;

    try {
        color_interpolator_node & node =
            dynamic_cast<color_interpolator_node &>(this->node());

        float fraction = value.value;

        vector<float>::size_type n = node.key_.mffloat::value.size() - 1;
        if (fraction < node.key_.mffloat::value[0]) {
            node.value_.value = node.key_value_.mfcolor::value[0];
        } else if (fraction > node.key_.mffloat::value[n]) {
            node.value_.value = node.key_value_.mfcolor::value[n];
        } else {
            // convert to HSV for the interpolation...
            for (vector<float>::size_type i = 0; i < n; ++i) {
                if (node.key_.mffloat::value[i] <= value.value
                    && fraction <= node.key_.mffloat::value[i + 1]) {
                    const color & rgb1 = node.key_value_.mfcolor::value[i];
                    const color & rgb2 =
                        node.key_value_.mfcolor::value[i + 1];

                    fraction = (fraction - node.key_.mffloat::value[i])
                        / (node.key_.mffloat::value[i + 1]
                           - node.key_.mffloat::value[i]);
                    float hsv1[3], hsv2[3];
                    rgb1.hsv(hsv1);
                    rgb2.hsv(hsv2);

                    // Interpolate angles via the shortest direction
                    if (fabs(hsv2[0] - hsv1[0]) > 180.0) {
                        if (hsv2[0] > hsv1[0]) {
                            hsv1[0] += 360.0;
                        } else {
                            hsv2[0] += 360.0;
                        }
                    }
                    float h = hsv1[0] + fraction * (hsv2[0] - hsv1[0]);
                    float s = hsv1[1] + fraction * (hsv2[1] - hsv1[1]);
                    float v = hsv1[2] + fraction * (hsv2[2] - hsv1[2]);
                    if (h >= 360.0) {
                        h -= 360.0;
                    } else if (h < 0.0) {
                        h += 360.0;
                    }
                    node.value_.value.hsv(h, s, v);
                    break;
                }
            }
        }
        node.emit_event(node.value_changed_, timestamp);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::color_interpolator_node::set_fraction_listener openvrml::vrml97_node::color_interpolator_node::set_fraction_listener_;
 *
 * @brief set_fraction event handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::mffloat> openvrml::vrml97_node::color_interpolator_node::key_
 *
 * @brief key exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfcolor> openvrml::vrml97_node::color_interpolator_node::key_value_
 *
 * @brief keyValue exposedField.
 */

/**
 * @var openvrml::sfcolor openvrml::vrml97_node::color_interpolator_node::value_
 *
 * @brief Current sfcolor value.
 */

/**
 * @var openvrml::sfcolor_emitter openvrml::vrml97_node::color_interpolator_node::value_changed_
 *
 * @brief value_changed eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::color_interpolator_node::
color_interpolator_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    set_fraction_listener_(*this),
    key_(*this),
    key_value_(*this),
    value_changed_(this->value_)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::color_interpolator_node::~color_interpolator_node()
    throw ()
{}


/**
 * @class openvrml::vrml97_node::cone_class
 *
 * @brief Class object for Cone nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::cone_class::cone_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::cone_class::~cone_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Cone nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by cone_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::cone_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "bottomRadius"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "height"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "side"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "bottom")
    };

    typedef vrml97_node_type_impl<cone_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & coneNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            coneNodeType.add_field(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &cone_node::bottomRadius)));
        } else if (*interface == supportedInterfaces[1]) {
            coneNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &cone_node::height)));
        } else if (*interface == supportedInterfaces[2]) {
            coneNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &cone_node::side)));
        } else if (*interface == supportedInterfaces[3]) {
            coneNodeType.add_field(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &cone_node::bottom)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::cone_node
 *
 * @brief Cone node instances.
 */

/**
 * @var openvrml::vrml97_node::cone_node::cone_class
 *
 * @brief Class object for Cone nodes.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::cone_node::bottom
 *
 * @brief bottom field.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::cone_node::bottomRadius
 *
 * @brief bottomRadius field.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::cone_node::height
 *
 * @brief height field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::cone_node::side
 *
 * @brief side field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::cone_node::
cone_node(const node_type & type,
          const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    geometry_node(type, scope),
    bottom(true),
    bottomRadius(1.0),
    height(2.0),
    side(true)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::cone_node::~cone_node() throw ()
{}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
openvrml::viewer::object_t
openvrml::vrml97_node::cone_node::
do_render_geometry(openvrml::viewer & viewer, const rendering_context context)
{
    const viewer::object_t object_ref =
        viewer.insert_cone(this->height.value,
                           this->bottomRadius.value,
                           this->bottom.value,
                           this->side.value);
    return object_ref;
}


/**
 * @class openvrml::vrml97_node::coordinate_class
 *
 * @brief Class object for Coordinate nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml::vrml97_node::coordinate_class::
coordinate_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::coordinate_class::~coordinate_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Coordinate nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by coordinate_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::coordinate_class::do_create_type(const std::string & id,
                              const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "point");

    typedef vrml97_node_type_impl<coordinate_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & coordinateNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterface) {
            coordinateNodeType.add_exposedfield(
                supportedInterface.field_type,
                supportedInterface.id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mfvec3f> >(
                            &coordinate_node::point_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfvec3f> >(
                        &coordinate_node::point_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mfvec3f> >(
                        &coordinate_node::point_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::coordinate_node
 *
 * @brief Coordinate node instances.
 */

/**
 * @var openvrml::vrml97_node::coordinate_node::coordinate_class
 *
 * @brief Class object for Coordinate nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfvec3f> openvrml::vrml97_node::coordinate_node::point_
 *
 * @brief point exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::coordinate_node::
coordinate_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::coordinate_node(type, scope),
    point_(*this)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::coordinate_node::~coordinate_node() throw ()
{}

/**
 * @brief Get the points encapsulated by this node.
 *
 * @return the array of points for this node.
 */
const std::vector<openvrml::vec3f> &
openvrml::vrml97_node::coordinate_node::point() const throw ()
{
    return this->point_.mfvec3f::value;
}


/**
 * @class openvrml::vrml97_node::coordinate_interpolator_class
 *
 * @brief Class object for CoordinateInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::coordinate_interpolator_class::
coordinate_interpolator_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::coordinate_interpolator_class::
~coordinate_interpolator_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating
 *      CoordinateInterpolator nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by coordinate_interpolator_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::coordinate_interpolator_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id, "key"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "keyValue"),
        node_interface(node_interface::eventout_id,
                       field_value::mfvec3f_id,
                       "value_changed")
    };

    typedef vrml97_node_type_impl<coordinate_interpolator_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & coordinateInterpolatorNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            coordinateInterpolatorNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    coordinate_interpolator_node::set_fraction_listener>(
                        &coordinate_interpolator_node::set_fraction_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            coordinateInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mffloat> >(
                            &coordinate_interpolator_node::key_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mffloat> >(
                        &coordinate_interpolator_node::key_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mffloat> >(
                        &coordinate_interpolator_node::key_)));
        } else if (*interface == supportedInterfaces[2]) {
            coordinateInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mfvec3f> >(
                            &coordinate_interpolator_node::key_value_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfvec3f> >(
                        &coordinate_interpolator_node::key_value_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mfvec3f> >(
                        &coordinate_interpolator_node::key_value_)));
        } else if (*interface == supportedInterfaces[3]) {
            coordinateInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<mfvec3f_emitter>(
                        &coordinate_interpolator_node::value_changed_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::coordinate_interpolator_node
 *
 * @brief CoordinateInterpolator node instances.
 */

/**
 * @var class openvrml::vrml97_node::coordinate_interpolator_node::coordinate_interpolator_class
 *
 * @brief Class object for CoordinateInterpolator nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::coordinate_interpolator_node::set_fraction_listener
 *
 * @brief set_fraction event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  coordinate_interpolator_node.
 */
openvrml::vrml97_node::coordinate_interpolator_node::set_fraction_listener::
set_fraction_listener(coordinate_interpolator_node & node):
    sffloat_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::coordinate_interpolator_node::set_fraction_listener::
~set_fraction_listener() throw ()
{}

/**
 * @brief Process event.
 *
 * @param fraction  fraction value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::coordinate_interpolator_node::set_fraction_listener::
do_process_event(const sffloat & fraction, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        using std::vector;

        coordinate_interpolator_node & node =
            dynamic_cast<coordinate_interpolator_node &>(this->node());

        const vector<float> & key = node.key_.mffloat::value;
        const vector<vec3f> & key_value = node.key_value_.mfvec3f::value;
        vector<vec3f> & value = node.value_.mfvec3f::value;

        size_t nCoords = key_value.size() / key.size();
        size_t n = key.size() - 1;

        if (fraction.value < key[0]) {
            value.assign(key_value.begin(), key_value.begin() + nCoords);
        } else if (fraction.value > key[n]) {
            value.assign(key_value.begin() + n * nCoords,
                         key_value.begin() + (n + 1) * nCoords);
        } else {
            // Reserve enough space for the new value
            value.resize(nCoords);

            for (size_t i = 0; i < n; ++i) {
                if (key[i] <= fraction.value && fraction.value <= key[i + 1]) {
                    vector<vec3f>::const_iterator v1 =
                        key_value.begin() + i * nCoords;
                    vector<vec3f>::const_iterator v2 =
                        key_value.begin() + (i + 1) * nCoords;

                    const float f =
                        (fraction.value - key[i]) / (key[i + 1] - key[i]);

                    for (size_t j = 0; j < nCoords; ++j) {
                        value[j] = *v1 + (f * (*v2 - *v1));
                        ++v1;
                        ++v2;
                    }
                    break;
                }
            }
        }

        // Send the new value
        node::emit_event(node.value_changed_, timestamp);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::coordinate_interpolator_node::set_fraction_listener openvrml::vrml97_node::coordinate_interpolator_node::set_fraction_listener_
 *
 * @brief set_fraction event handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::mffloat> openvrml::vrml97_node::coordinate_interpolator_node::key_
 *
 * @brief key exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfvec3f> openvrml::vrml97_node::coordinate_interpolator_node::key_value_
 *
 * @brief keyValue exposedField.
 */

/**
 * @var openvrml::mfvec3f openvrml::vrml97_node::coordinate_interpolator_node::value_
 *
 * @brief Current value.
 */

/**
 * @var openvrml::mfvec3f_emitter openvrml::vrml97_node::coordinate_interpolator_node::value_changed_
 *
 * @brief value_changed eventOut.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::coordinate_interpolator_node::
coordinate_interpolator_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    set_fraction_listener_(*this),
    key_(*this),
    key_value_(*this),
    value_changed_(this->value_)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::coordinate_interpolator_node::
~coordinate_interpolator_node() throw ()
{}


/**
 * @class openvrml::vrml97_node::cylinder_class
 *
 * @brief Class object for Cylinder nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::cylinder_class::
cylinder_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::cylinder_class::~cylinder_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Cylinder nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by cylinder_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::cylinder_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "bottom"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "height"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "radius"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "side"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "top")
    };

    typedef vrml97_node_type_impl<cylinder_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & cylinderNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            cylinderNodeType.add_field(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &cylinder_node::bottom)));
        } else if (*interface == supportedInterfaces[1]) {
            cylinderNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &cylinder_node::height)));
        } else if (*interface == supportedInterfaces[2]) {
            cylinderNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &cylinder_node::radius)));
        } else if (*interface == supportedInterfaces[3]) {
            cylinderNodeType.add_field(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(&cylinder_node::side)));
        } else if (*interface == supportedInterfaces[4]) {
            cylinderNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(&cylinder_node::top)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::cylinder_node
 *
 * @brief Cylinder node instances.
 */

/**
 * @var openvrml::vrml97_node::cylinder_node::cylinder_class
 *
 * @brief Class object for Cylinder nodes.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::cylinder_node::bottom
 *
 * @brief bottom field.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::cylinder_node::height
 *
 * @brief height field.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::cylinder_node::radius
 *
 * @brief radius field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::cylinder_node::side
 *
 * @brief side field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::cylinder_node::top
 *
 * @brief top field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::cylinder_node::
cylinder_node(const node_type & type,
              const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    geometry_node(type, scope),
    bottom(true),
    height(2.0),
    radius(1.0),
    side(true),
    top(true)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::cylinder_node::~cylinder_node() throw ()
{
    // need access to viewer to remove d_viewerObject...
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a viewer.
 * @param context   the rendering context.
 */
openvrml::viewer::object_t
openvrml::vrml97_node::cylinder_node::
do_render_geometry(openvrml::viewer & viewer, const rendering_context context)
{
    const viewer::object_t object_ref =
        viewer.insert_cylinder(this->height.value,
                               this->radius.value,
                               this->bottom.value,
                               this->side.value,
                               this->top.value);
    return object_ref;
}


/**
 * @class openvrml::vrml97_node::cylinder_sensor_class
 *
 * @brief Class object for CylinderSensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::cylinder_sensor_class::
cylinder_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::cylinder_sensor_class::~cylinder_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating CylinderSensor
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by cylinder_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::cylinder_sensor_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "autoOffset"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "diskAngle"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "enabled"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "maxAngle"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "minAngle"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "offset"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfrotation_id,
                       "rotation_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "trackPoint_changed")
    };

    typedef vrml97_node_type_impl<cylinder_sensor_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & cylinderSensorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &cylinder_sensor_node::auto_offset_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &cylinder_sensor_node::auto_offset_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &cylinder_sensor_node::auto_offset_)));
        } else if (*interface == supportedInterfaces[1]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sffloat> >(
                            &cylinder_sensor_node::disk_angle_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &cylinder_sensor_node::disk_angle_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &cylinder_sensor_node::disk_angle_)));
        } else if (*interface == supportedInterfaces[2]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &cylinder_sensor_node::enabled_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &cylinder_sensor_node::enabled_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &cylinder_sensor_node::enabled_)));
        } else if (*interface == supportedInterfaces[3]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sffloat> >(
                            &cylinder_sensor_node::max_angle_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &cylinder_sensor_node::max_angle_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &cylinder_sensor_node::max_angle_)));
        } else if (*interface == supportedInterfaces[4]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sffloat> >(
                            &cylinder_sensor_node::min_angle_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &cylinder_sensor_node::min_angle_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &cylinder_sensor_node::min_angle_)));
        } else if (*interface == supportedInterfaces[5]) {
            cylinderSensorNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sffloat> >(
                            &cylinder_sensor_node::offset_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &cylinder_sensor_node::offset_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &cylinder_sensor_node::offset_)));
        } else if (*interface == supportedInterfaces[6]) {
            cylinderSensorNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &cylinder_sensor_node::is_active_emitter_)));
        } else if (*interface == supportedInterfaces[7]) {
            cylinderSensorNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfrotation_emitter>(
                        &cylinder_sensor_node::rotation_changed_emitter_)));
        } else if (*interface == supportedInterfaces[8]) {
            cylinderSensorNodeType.add_eventout(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfvec3f_emitter>(
                        &cylinder_sensor_node::track_point_changed_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::cylinder_sensor_node
 *
 * @brief CylinderSensor node instances.
 */

/**
 * @var openvrml::vrml97_node::cylinder_sensor_node::cylinder_sensor_class
 *
 * @brief Class object for CylinderSensor nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::cylinder_sensor_node::auto_offset_
 *
 * @brief autoOffset exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::cylinder_sensor_node::disk_angle_
 *
 * @brief diskAngle exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::cylinder_sensor_node::enabled_
 *
 * @brief enabled exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::cylinder_sensor_node::max_angle_
 *
 * @brief maxAngle exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::cylinder_sensor_node::min_angle_
 *
 * @brief minAngle exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::cylinder_sensor_node::offset_
 *
 * @brief offset exposedField.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::cylinder_sensor_node::is_active_
 *
 * @brief Active state.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::cylinder_sensor_node::is_active_emitter_
 *
 * @brief isActive eventOut.
 */

/**
 * @var openvrml::sfrotation openvrml::vrml97_node::cylinder_sensor_node::rotation_changed_
 *
 * @brief Rotation state.
 */

/**
 * @var openvrml::sfrotation_emitter openvrml::vrml97_node::cylinder_sensor_node::rotation_changed_emitter_
 *
 * @brief rotation_changed eventOut.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::cylinder_sensor_node::track_point_changed_
 *
 * @brief Track point state.
 */

/**
 * @var openvrml::sfvec3f_emitter openvrml::vrml97_node::cylinder_sensor_node::track_point_changed_emitter_
 *
 * @brief trackPoint_changed eventOut.
 */

/**
 * @var float openvrml::vrml97_node::cylinder_sensor_node::rotation_val
 *
 * @brief The rotation value.
 */

/**
 * @var openvrml::vec3f openvrml::vrml97_node::cylinder_sensor_node::activationPoint
 *
 * @brief The activation point.
 */

/**
 * @var bool openvrml::vrml97_node::cylinder_sensor_node::disk
 */

/**
 * @var openvrml::mat4f openvrml::vrml97_node::cylinder_sensor_node::activationMatrix
 *
 * @brief Activation matrix.
 */

/**
 * @var openvrml::mat4f openvrml::vrml97_node::cylinder_sensor_node::modelview
 *
 * @brief Modelview matrix.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::cylinder_sensor_node::
cylinder_sensor_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    auto_offset_(*this, true),
    disk_angle_(*this, 0.262f),
    enabled_(*this, true),
    max_angle_(*this, -1.0f),
    min_angle_(*this, 0.0f),
    offset_(*this, 0.0f),
    is_active_emitter_(this->is_active_),
    rotation_changed_emitter_(this->rotation_changed_),
    track_point_changed_emitter_(this->track_point_changed_),
    rotation_val(0.0f),
    disk(false)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::cylinder_sensor_node::~cylinder_sensor_node() throw ()
{}

/**
 * @brief Cast to a cylinder_sensor_node.
 *
 * @return a pointer to the cylinder_sensor_node.
 */
openvrml::vrml97_node::cylinder_sensor_node *
openvrml::vrml97_node::cylinder_sensor_node::to_cylinder_sensor() const
{
    return (cylinder_sensor_node*) this;
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void
openvrml::vrml97_node::cylinder_sensor_node::
do_render_child(openvrml::viewer & viewer, rendering_context context)
{
    //
    // Store the modelview matrix which is calculated at the time of rendering
    // in render-context. This matrix will be in use at the time of activation.
    //
    this->modelview = context.matrix();
}

/**
 * @brief Called in response to user interaction.
 */
void openvrml::vrml97_node::cylinder_sensor_node::activate(double timestamp,
                                                           bool isActive,
                                                           double * p)
{
    using openvrml_::pi;
    using openvrml_::pi_2;

    // Become active
    if (isActive && !this->is_active_.value) {
        this->is_active_.value = isActive;

        // set activation point in local coords
        vec3f v(static_cast<float>(p[0]),
                static_cast<float>(p[1]),
                static_cast<float>(p[2]));
        this->activationMatrix = this->modelview.inverse();
        v *= this->activationMatrix;
        this->activationPoint = v;
        // Bearing vector in local coordinate system
        v.x(this->activationMatrix[2][0]);
        v.y(this->activationMatrix[2][1]);
        v.z(this->activationMatrix[2][2]);
        const vec3f bearing = v.normalize();
        const vec3f up(0.0, 1.0, 0.0);
        double ang = acos(bearing.dot(up));
        if (ang > pi_2) { ang = pi - ang; }
        this->disk = (ang < this->disk_angle_.sffloat::value);
        // send message
        node::emit_event(this->is_active_emitter_, timestamp);
    }

    // Become inactive
    else if (!isActive && this->is_active_.value) {
        this->is_active_.value = isActive;
        node::emit_event(this->is_active_emitter_, timestamp);

        // save auto offset of rotation
        if (this->auto_offset_.sfbool::value) {
            this->offset_.sffloat::value = rotation_val;
            node::emit_event(this->offset_, timestamp);
        }
    }

        // Tracking
    else if (isActive) {
        using openvrml_::fequal;

        // get local coord for touch point
        vec3f Vec(static_cast<float>(p[0]),
                  static_cast<float>(p[1]),
                  static_cast<float>(p[2]));
        Vec = Vec * this->activationMatrix;
        this->track_point_changed_.value = Vec;
        node::emit_event(this->track_point_changed_emitter_, timestamp);
        vec3f tempv;
        float rot, radius;
        vec3f dir1(Vec[0], 0, Vec[2]);
        radius = this->disk
               ? 1.0f
               : dir1.length();
        dir1 = dir1.normalize();
        vec3f dir2(this->activationPoint.x(), 0, this->activationPoint.z());
        dir2 = dir2.normalize();
        tempv = dir2 * dir1;
        vec3f cx(tempv);
        cx = cx.normalize();
        if (cx.length() == 0.0) { return; }
        rot = radius * float(acos(dir2.dot(dir1)));
        if (fequal<float>()(cx.y(), -1.0f)) { rot = -rot; }
        if (this->auto_offset_.sfbool::value) {
            rot = this->offset_.sffloat::value + rot;
        }
        if (this->min_angle_.sffloat::value
            < this->max_angle_.sffloat::value) {
            if (rot < this->min_angle_.sffloat::value) {
                rot = this->min_angle_.sffloat::value;
            } else if (rot > this->max_angle_.sffloat::value) {
                rot = this->max_angle_.sffloat::value;
            }
        }
        this->rotation_val = rot;
        this->rotation_changed_.sfrotation::value =
            openvrml::rotation(0, 1, 0, rot);

        node::emit_event(this->rotation_changed_emitter_, timestamp);
    }
}

/**
 * @brief Indicate whether the sensor is enabled.
 *
 * @return @c true if the sensor is enabled; @c false otherwise.
 */
bool openvrml::vrml97_node::cylinder_sensor_node::enabled() const
{
    return this->enabled_.sfbool::value;
}


/**
 * @class openvrml::vrml97_node::directional_light_class
 *
 * @brief Class object for DirectionalLight nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::directional_light_class::
directional_light_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::directional_light_class::~directional_light_class()
    throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating DirectionalLight
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by directional_light_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::directional_light_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "ambientIntensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "direction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "intensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "on")
    };

    typedef vrml97_node_type_impl<directional_light_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & directionalLightNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface = interfaces.begin();
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            directionalLightNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(
                        &directional_light_node::ambient_intensity_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &directional_light_node::ambient_intensity_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &directional_light_node::ambient_intensity_)));
        } else if (*interface == supportedInterfaces[1]) {
            directionalLightNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sfcolor> >(
                            &directional_light_node::color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfcolor> >(
                        &directional_light_node::color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfcolor> >(
                        &directional_light_node::color_)));
        } else if (*interface == supportedInterfaces[2]) {
            directionalLightNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sfvec3f> >(
                            &directional_light_node::direction_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &directional_light_node::direction_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &directional_light_node::direction_)));
        } else if (*interface == supportedInterfaces[3]) {
            directionalLightNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sffloat> >(
                            &directional_light_node::intensity_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &directional_light_node::intensity_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &directional_light_node::intensity_)));
        } else if (*interface == supportedInterfaces[4]) {
            directionalLightNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &directional_light_node::on_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &directional_light_node::on_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &directional_light_node::on_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::directional_light_node
 *
 * @brief DirectionalLight node instances.
 */

/**
 * @var openvrml::vrml97_node::directional_light_node::directional_light_class
 *
 * @brief Class object for DirectionalLight nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::directional_light_node::direction_
 *
 * @brief direction exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::directional_light_node::
directional_light_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    abstract_light_node(type, scope),
    direction_(*this, vec3f(0.0, 0.0, -1.0))
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::directional_light_node::~directional_light_node()
    throw ()
{}

/**
 * @brief Render the node.
 *
 * This should be called before rendering any sibling nodes.
 *
 * @param viewer    a viewer.
 * @param context   a rendering context.
 */
void
openvrml::vrml97_node::directional_light_node::
do_render_child(openvrml::viewer & viewer, const rendering_context rc)
{
    if (this->on_.sfbool::value) {
        viewer.insert_dir_light(this->ambient_intensity_.sffloat::value,
                                this->intensity_.sffloat::value,
                                this->color_.sfcolor::value,
                                this->direction_.sfvec3f::value);
    }
    this->node::modified(false);
}


/**
 * @class openvrml::vrml97_node::elevation_grid_class
 *
 * @brief Class object for ElevationGrid nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::elevation_grid_class::
elevation_grid_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::elevation_grid_class::~elevation_grid_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating ElevationGrid
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by elevation_grid_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::elevation_grid_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::mffloat_id,
                       "set_height"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "normal"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "texCoord"),
        node_interface(node_interface::field_id,
                       field_value::mffloat_id,
                       "height"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "ccw"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "colorPerVertex"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "creaseAngle"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "normalPerVertex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "xDimension"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "xSpacing"),
        node_interface(node_interface::field_id,
                       field_value::sfint32_id,
                       "zDimension"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "zSpacing")
    };

    typedef vrml97_node_type_impl<elevation_grid_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & elevationGridNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            elevationGridNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        elevation_grid_node::set_height_listener>(
                            &elevation_grid_node::set_height_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            elevationGridNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &elevation_grid_node::color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &elevation_grid_node::color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &elevation_grid_node::color_)));
        } else if (*interface == supportedInterfaces[2]) {
            elevationGridNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &elevation_grid_node::normal_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &elevation_grid_node::normal_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &elevation_grid_node::normal_)));
        } else if (*interface == supportedInterfaces[3]) {
            elevationGridNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &elevation_grid_node::tex_coord_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &elevation_grid_node::tex_coord_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &elevation_grid_node::tex_coord_)));
        } else if (*interface == supportedInterfaces[4]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mffloat>(
                        &elevation_grid_node::height_)));
        } else if (*interface == supportedInterfaces[5]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &elevation_grid_node::ccw_)));
        } else if (*interface == supportedInterfaces[6]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &elevation_grid_node::color_per_vertex_)));
        } else if (*interface == supportedInterfaces[7]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &elevation_grid_node::crease_angle_)));
        } else if (*interface == supportedInterfaces[8]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &elevation_grid_node::normal_per_vertex_)));
        } else if (*interface == supportedInterfaces[9]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &elevation_grid_node::solid_)));
        } else if (*interface == supportedInterfaces[10]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfint32>(
                        &elevation_grid_node::x_dimension_)));
        } else if (*interface == supportedInterfaces[11]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &elevation_grid_node::x_spacing_)));
        } else if (*interface == supportedInterfaces[12]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[12].field_type,
                supportedInterfaces[12].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfint32>(
                        &elevation_grid_node::z_dimension_)));
        } else if (*interface == supportedInterfaces[13]) {
            elevationGridNodeType.add_field(
                supportedInterfaces[13].field_type,
                supportedInterfaces[13].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &elevation_grid_node::z_spacing_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::elevation_grid_node
 *
 * @brief ElevationGrid node instances.
 */

/**
 * @var openvrml::vrml97_node::elevation_grid_node::elevation_grid_class
 *
 * @brief Class object for ElevationGrid nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::elevation_grid_node::set_height_listener
 *
 * @brief set_height event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  elevation_grid_node.
 */
openvrml::vrml97_node::elevation_grid_node::set_height_listener::
set_height_listener(elevation_grid_node & node):
    mffloat_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::elevation_grid_node::set_height_listener::
~set_height_listener() throw ()
{}

/**
 * @brief Process event.
 *
 * @param height    height value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::elevation_grid_node::set_height_listener::
do_process_event(const mffloat & height, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        elevation_grid_node & elevation_grid =
            dynamic_cast<elevation_grid_node &>(this->node());

        elevation_grid.height_ = height;
        elevation_grid.node::modified(true);

    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::elevation_grid_node::set_height_listener openvrml::vrml97_node::elevation_grid_node::set_height_listener_
 *
 * @brief set_height eventIn handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::elevation_grid_node::color_
 *
 * @brief color exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::elevation_grid_node::normal_
 *
 * @brief normal exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::elevation_grid_node::tex_coord_
 *
 * @brief texCoord exposedField.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::elevation_grid_node::ccw_
 *
 * @brief ccw field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::elevation_grid_node::color_per_vertex_
 *
 * @brief colorPerVertex field.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::elevation_grid_node::crease_angle_
 *
 * @brief creaseAngle field.
 */

/**
 * @var openvrml::mffloat openvrml::vrml97_node::elevation_grid_node::height_
 *
 * @brief height field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::elevation_grid_node::normal_per_vertex_
 *
 * @brief normalPerVertex field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::elevation_grid_node::solid_
 *
 * @brief solid field.
 */

/**
 * @var openvrml::sfint32 openvrml::vrml97_node::elevation_grid_node::x_dimension_
 *
 * @brief xDimension field.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::elevation_grid_node::x_spacing_
 *
 * @brief xSpacing field.
 */

/**
 * @var openvrml::sfint32 openvrml::vrml97_node::elevation_grid_node::z_dimension_
 *
 * @brief zDimension field.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::,elevation_grid_node::z_spacing_
 *
 * @brief zSpacing field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::elevation_grid_node::
elevation_grid_node(const node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    geometry_node(type, scope),
    set_height_listener_(*this),
    color_(*this),
    normal_(*this),
    tex_coord_(*this),
    ccw_(true),
    color_per_vertex_(true),
    normal_per_vertex_(true),
    solid_(true),
    x_dimension_(0),
    x_spacing_(1.0f),
    z_dimension_(0),
    z_spacing_(1.0f)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::elevation_grid_node::~elevation_grid_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::elevation_grid_node::modified() const
{
    return this->node::modified()
        || (this->color_.sfnode::value
            && this->color_.sfnode::value->modified())
        || (this->normal_.sfnode::value
            && this->normal_.sfnode::value->modified())
        || (this->tex_coord_.sfnode::value
            && this->tex_coord_.sfnode::value->modified());
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
openvrml::viewer::object_t
openvrml::vrml97_node::elevation_grid_node::
do_render_geometry(openvrml::viewer & viewer, const rendering_context context)
{
    viewer::object_t obj = 0;

    openvrml::color_node * const colorNode =
        node_cast<openvrml::color_node *>(this->color_.sfnode::value.get());
    openvrml::normal_node * const normalNode =
        node_cast<openvrml::normal_node *>(this->normal_.sfnode::value.get());
    openvrml::texture_coordinate_node * const texCoordNode =
        node_cast<openvrml::texture_coordinate_node *>(
            this->tex_coord_.sfnode::value.get());

    if (!this->height_.mffloat::value.empty()) {
        using std::vector;

        const vector<openvrml::color> & color =
            colorNode
            ? colorNode->color()
            : vector<openvrml::color>();

        const vector<vec3f> & normal =
            normalNode
            ? normalNode->vector()
            : vector<vec3f>();

        const vector<vec2f> & texCoord =
            texCoordNode
            ? texCoordNode->point()
            : vector<vec2f>();
        // insert geometry
        unsigned int optMask = 0;
        if (this->ccw_.value) {
            optMask |= viewer::mask_ccw;
        }
        if (this->solid_.value) {
            optMask |= viewer::mask_solid;
        }
        if (this->color_per_vertex_.value) {
            optMask |= viewer::mask_color_per_vertex;
        }
        if (this->normal_per_vertex_.value) {
            optMask |= viewer::mask_normal_per_vertex;
        }

        obj = viewer.insert_elevation_grid(optMask,
                                           this->height_.mffloat::value,
                                           this->x_dimension_.sfint32::value,
                                           this->z_dimension_.sfint32::value,
                                           this->x_spacing_.sffloat::value,
                                           this->z_spacing_.sffloat::value,
                                           color,
                                           normal,
                                           texCoord);
    }

    if (colorNode) { colorNode->modified(false); }
    if (normalNode) { normalNode->modified(false); }
    if (texCoordNode) { texCoordNode->modified(false); }

    return obj;
}


/**
 * @class openvrml::vrml97_node::extrusion_class
 *
 * @brief Class object for Extrusion nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::extrusion_class::
extrusion_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::extrusion_class::~extrusion_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Extrusion nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by extrusion_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::extrusion_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
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
                       "spine")
    };

    typedef vrml97_node_type_impl<extrusion_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & extrusionNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            extrusionNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        extrusion_node::set_cross_section_listener>(
                            &extrusion_node::set_cross_section_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            extrusionNodeType.add_eventin(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        extrusion_node::set_orientation_listener>(
                            &extrusion_node::set_orientation_listener_)));
        } else if (*interface == supportedInterfaces[2]) {
            extrusionNodeType.add_eventin(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        extrusion_node::set_scale_listener>(
                            &extrusion_node::set_scale_listener_)));
        } else if (*interface == supportedInterfaces[3]) {
            extrusionNodeType.add_eventin(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        extrusion_node::set_spine_listener>(
                            &extrusion_node::set_spine_listener_)));
        } else if (*interface == supportedInterfaces[4]) {
            extrusionNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &extrusion_node::begin_cap_)));
        } else if (*interface == supportedInterfaces[5]) {
            extrusionNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &extrusion_node::ccw_)));
        } else if (*interface == supportedInterfaces[6]) {
            extrusionNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &extrusion_node::convex_)));
        } else if (*interface == supportedInterfaces[7]) {
            extrusionNodeType.add_field(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &extrusion_node::crease_angle_)));
        } else if (*interface == supportedInterfaces[8]) {
            extrusionNodeType.add_field(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfvec2f>(
                        &extrusion_node::cross_section_)));
        } else if (*interface == supportedInterfaces[9]) {
            extrusionNodeType.add_field(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &extrusion_node::end_cap_)));
        } else if (*interface == supportedInterfaces[10]) {
            extrusionNodeType.add_field(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfrotation>(
                        &extrusion_node::orientation_)));
        } else if (*interface == supportedInterfaces[11]) {
            extrusionNodeType.add_field(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfvec2f>(
                        &extrusion_node::scale_)));
        } else if (*interface == supportedInterfaces[12]) {
            extrusionNodeType.add_field(
                supportedInterfaces[12].field_type,
                supportedInterfaces[12].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &extrusion_node::solid_)));
        } else if (*interface == supportedInterfaces[13]) {
            extrusionNodeType.add_field(
                supportedInterfaces[13].field_type,
                supportedInterfaces[13].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfvec3f>(
                        &extrusion_node::spine_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::extrusion_node
 *
 * @brief Extrusion node instances.
 */

/**
 * @var openvrml::vrml97_node::extrusion_node::extrusion_class
 *
 * @brief Class object for Extrusion nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::extrusion_node::set_cross_section_listener
 *
 * @brief set_crossSection event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  extrusion_node.
 */
openvrml::vrml97_node::extrusion_node::set_cross_section_listener::
set_cross_section_listener(extrusion_node & node):
    mfvec2f_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::extrusion_node::set_cross_section_listener::
~set_cross_section_listener() throw ()
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
openvrml::vrml97_node::extrusion_node::set_cross_section_listener::
do_process_event(const mfvec2f & cross_section, const double timestamp)
    throw (std::bad_alloc)
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
 * @class openvrml::vrml97_node::extrusion_node::set_orientation_listener
 *
 * @brief set_orientation event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  extrusion_node.
 */
openvrml::vrml97_node::extrusion_node::set_orientation_listener::
set_orientation_listener(extrusion_node & node):
    mfrotation_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::extrusion_node::set_orientation_listener::
~set_orientation_listener() throw ()
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
openvrml::vrml97_node::extrusion_node::set_orientation_listener::
do_process_event(const mfrotation & orientation, const double timestamp)
    throw (std::bad_alloc)
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
 * @class openvrml::vrml97_node::extrusion_node::set_scale_listener
 *
 * @brief set_scale event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  extrusion_node.
 */
openvrml::vrml97_node::extrusion_node::set_scale_listener::
set_scale_listener(extrusion_node & node):
    mfvec2f_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::extrusion_node::set_scale_listener::
~set_scale_listener() throw ()
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
openvrml::vrml97_node::extrusion_node::set_scale_listener::
do_process_event(const mfvec2f & scale, const double timestamp)
    throw (std::bad_alloc)
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
 * @class openvrml::vrml97_node::extrusion_node::set_spine_listener
 *
 * @brief set_spine event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  extrusion_node.
 */
openvrml::vrml97_node::extrusion_node::set_spine_listener::
set_spine_listener(extrusion_node & node):
    mfvec3f_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::extrusion_node::set_spine_listener::
~set_spine_listener() throw ()
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
openvrml::vrml97_node::extrusion_node::set_spine_listener::
do_process_event(const mfvec3f & spine, const double timestamp)
    throw (std::bad_alloc)
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
 * @var openvrml::vrml97_node::extrusion_node::set_cross_section_listener openvrml::vrml97_node::extrusion_node::set_cross_section_listener_
 *
 * @brief set_crossSection eventIn handler.
 */

/**
 * @var openvrml::vrml97_node::extrusion_node::set_orientation_listener openvrml::vrml97_node::extrusion_node::set_orientation_listener_
 *
 * @brief set_orientation eventIn handler.
 */

/**
 * @var openvrml::vrml97_node::extrusion_node::set_scale_listener openvrml::vrml97_node::extrusion_node::set_scale_listener_
 *
 * @brief set_scale eventIn handler.
 */

/**
 * @var openvrml::vrml97_node::extrusion_node::set_spine_listener openvrml::vrml97_node::extrusion_node::set_spine_listener_
 *
 * @brief set_spine eventIn handler.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::extrusion_node::begin_cap_
 *
 * @brief beginCap field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::extrusion_node::ccw_
 *
 * @brief ccw field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::extrusion_node::convex_
 *
 * @brief convex field.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::extrusion_node::crease_angle_
 *
 * @brief creaseAngle field.
 */

/**
 * @var openvrml::mfvec2f openvrml::vrml97_node::extrusion_node::cross_section_
 *
 * @brief crossSection field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::extrusion_node::end_cap_
 *
 * @brief endCap field.
 */

/**
 * @var openvrml::mfrotation openvrml::vrml97_node::extrusion_node::orientation_
 *
 * @brief orientation field.
 */

/**
 * @var openvrml::mfvec2f openvrml::vrml97_node::extrusion_node::scale_
 *
 * @brief scale field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::extrusion_node::solid_
 *
 * @brief solid field.
 */

/**
 * @var openvrml::mfvec3f openvrml::vrml97_node::extrusion_node::spine_
 *
 * @brief spine field.
 */

namespace {
    const openvrml::vec2f extrusionDefaultCrossSection_[] =
    {
        openvrml::vec2f(1.0, 1.0),
        openvrml::vec2f(1.0, -1.0),
        openvrml::vec2f(-1.0, -1.0),
        openvrml::vec2f(-1.0, 1.0),
        openvrml::vec2f(1.0, 1.0)
    };

    const openvrml::vec2f extrusionDefaultScale_[] =
    {
        openvrml::vec2f(1.0, 1.0)
    };

    const openvrml::rotation extrusionDefaultOrientation_[] =
    {
        openvrml::rotation(0.0, 0.0, 1.0, 0.0)
    };

    const openvrml::vec3f extrusionDefaultSpine_[] =
    {
        openvrml::vec3f(0.0, 0.0, 0.0),
        openvrml::vec3f(0.0, 1.0, 0.0)
    };
}

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::extrusion_node::
extrusion_node(const node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    geometry_node(type, scope),
    set_cross_section_listener_(*this),
    set_orientation_listener_(*this),
    set_scale_listener_(*this),
    set_spine_listener_(*this),
    begin_cap_(true),
    ccw_(true),
    convex_(true),
    crease_angle_(0),
    cross_section_(extrusionDefaultCrossSection_,
                   extrusionDefaultCrossSection_ + 5),
    end_cap_(true),
    orientation_(extrusionDefaultOrientation_,
                 extrusionDefaultOrientation_ + 1),
    scale_(extrusionDefaultScale_, extrusionDefaultScale_ + 1),
    solid_(true),
    spine_(extrusionDefaultSpine_, extrusionDefaultSpine_ + 2)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::extrusion_node::~extrusion_node() throw () {}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
openvrml::viewer::object_t
openvrml::vrml97_node::extrusion_node::
do_render_geometry(openvrml::viewer & viewer, const rendering_context context)
{
    viewer::object_t obj = 0;
    if (!this->cross_section_.value.empty() && this->spine_.value.size() > 1) {
        unsigned int optMask = 0;
        if (this->ccw_.value)        { optMask |= viewer::mask_ccw; }
        if (this->convex_.value)     { optMask |= viewer::mask_convex; }
        if (this->solid_.value)      { optMask |= viewer::mask_solid; }
        if (this->begin_cap_.value)  { optMask |= viewer::mask_bottom; }
        if (this->end_cap_.value)    { optMask |= viewer::mask_top; }

        obj = viewer.insert_extrusion(optMask,
                                      this->spine_.value,
                                      this->cross_section_.value,
                                      this->orientation_.value,
                                      this->scale_.value);
    }

    return obj;
}


/**
 * @class openvrml::vrml97_node::fog_class
 *
 * @brief Class object for Fog nodes.
 */

/**
 * @typedef openvrml::vrml97_node::fog_class::bound_nodes_t
 *
 * @brief A bound Fog node stack.
 */

/**
 * @var openvrml::vrml97_node::fog_node * openvrml::vrml97_node::fog_class::first
 *
 * @brief The first Fog node in the initial scene graph.
 */

/**
 * @var openvrml::vrml97_node::fog_class::bound_nodes_t openvrml::vrml97_node::fog_class::bound_nodes
 *
 * @brief The bound Fog node stack.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::fog_class::fog_class(openvrml::browser & browser):
    node_class(browser),
    first(0)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::fog_class::~fog_class() throw ()
{}

/**
 * @brief Set the first Fog node in the world.
 *
 * The first Fog node in the world is used for the initial fog.
 * This method is used by fog_node::do_initialize.
 *
 * @param fog   a Fog node.
 */
void openvrml::vrml97_node::fog_class::set_first(fog_node & fog) throw ()
{
    this->first = &fog;
}

/**
 * @brief Reset the pointer to the first Fog node in the world to null.
 *
 * This function is called by <code>fog_node::do_shutdown</code>.
 */
void openvrml::vrml97_node::fog_class::reset_first() throw ()
{
    this->first = 0;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by fog_node::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false otherwise.
 */
bool openvrml::vrml97_node::fog_class::has_first() const throw ()
{
    return this->first;
}

/**
 * @brief Check to see if a node is registered as the "first" node.
 *
 * @param fog   a fog_node.
 *
 * @return @c true if @p fog is the fist node; @c false otherwise.
 */
bool openvrml::vrml97_node::fog_class::is_first(fog_node & fog) throw ()
{
    return &fog == this->first;
}

/**
 * @brief Push a Fog on the top of the bound node stack.
 *
 * @param fog       the node to bind.
 * @param timestamp the current time.
 */
void openvrml::vrml97_node::fog_class::bind(fog_node & fog,
                                            const double timestamp)
    throw (std::bad_alloc)
{
    using std::find;

    //
    // If the node is already the active node, do nothing.
    //
    if (!this->bound_nodes.empty() && &fog == this->bound_nodes.back()) {
        return;
    }

    //
    // If the node is already on the stack, remove it.
    //
    const bound_nodes_t::iterator pos =
        find(this->bound_nodes.begin(), this->bound_nodes.end(), &fog);
    if (pos != this->bound_nodes.end()) { this->bound_nodes.erase(pos); }

    //
    // Send FALSE from the currently active node's isBound.
    //
    if (!this->bound_nodes.empty()) {
        fog_node & current =
            dynamic_cast<fog_node &>(*this->bound_nodes.back());
        current.is_bound_.value = false;
        node::emit_event(current.is_bound_emitter_, timestamp);
    }

    //
    // Push the node to the top of the stack, and have it send isBound TRUE.
    //
    this->bound_nodes.push_back(&fog);
    fog.is_bound_.value = true;
    node::emit_event(fog.is_bound_emitter_, timestamp);
}

/**
 * @brief Remove a Fog from the bound node stack.
 *
 * @param fog       the node to unbind.
 * @param timestamp the current time.
 */
void openvrml::vrml97_node::fog_class::unbind(fog_node & fog,
                                              const double timestamp) throw ()
{
    const bound_nodes_t::iterator pos =
        std::find(this->bound_nodes.begin(), this->bound_nodes.end(), &fog);
    if (pos != this->bound_nodes.end()) {
        fog.is_bound_.value = false;
        node::emit_event(fog.is_bound_emitter_, timestamp);

        if (pos == this->bound_nodes.end() - 1
            && this->bound_nodes.size() > 1) {
            fog_node & newActive =
                dynamic_cast<fog_node &>(**(this->bound_nodes.end() - 2));
            newActive.is_bound_.value = true;
            node::emit_event(newActive.is_bound_emitter_, timestamp);
        }
        this->bound_nodes.erase(pos);
    }
}

/**
 * @brief node_class-specific initialization.
 *
 * @param initial_viewpoint the viewpoint_node that should be bound initially.
 * @param timestamp         the current time.
 */
void
openvrml::vrml97_node::fog_class::
do_initialize(openvrml::viewpoint_node * initial_viewpoint,
              const double timestamp)
    throw ()
{
    if (this->first) {
        try {
            event_listener & listener =
                this->first->event_listener("set_bind");
            assert(dynamic_cast<sfbool_listener *>(&listener));
            static_cast<sfbool_listener &>(listener)
                .process_event(sfbool(true), timestamp);
        } catch (unsupported_interface & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }
}

/**
 * @brief node_class-specific rendering.
 *
 * Render the active Fog node.
 *
 * @param v a viewer.
 */
void openvrml::vrml97_node::fog_class::do_render(viewer & v) const throw ()
{
    if (!this->bound_nodes.empty()) {
        fog_node & fog = dynamic_cast<fog_node &>(*this->bound_nodes.back());
        v.set_fog(fog.color_.sfcolor::value,
                  fog.visibility_range_.sffloat::value,
                  fog.fog_type_.sfstring::value.c_str());
    }
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Fog nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by fog_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::fog_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::sfbool_id,
                       "set_bind"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfstring_id,
                       "fogType"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "visibilityRange"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isBound")
    };

    typedef vrml97_node_type_impl<fog_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & fogNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            fogNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    fog_node::set_bind_listener>(
                        &fog_node::set_bind_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            fogNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfcolor> >(&fog_node::color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfcolor> >(
                        &fog_node::color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfcolor> >(
                        &fog_node::color_)));
        } else if (*interface == supportedInterfaces[2]) {
            fogNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfstring> >(&fog_node::fog_type_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfstring> >(
                        &fog_node::fog_type_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                        exposedfield<sfstring> >(&fog_node::fog_type_)));
        } else if (*interface == supportedInterfaces[3]) {
            fogNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(&fog_node::visibility_range_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &fog_node::visibility_range_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &fog_node::visibility_range_)));
        } else if (*interface == supportedInterfaces[4]) {
            fogNodeType.add_eventout(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &fog_node::is_bound_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::fog_node
 *
 * @brief Fog node instances.
 */

/**
 * @var class openvrml::vrml97_node::fog_node::fog_class
 *
 * @brief Class object for Fog nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::fog_node::set_bind_listener
 *
 * @brief set_bind event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  fog_node.
 */
openvrml::vrml97_node::fog_node::set_bind_listener::
set_bind_listener(fog_node & node):
    sfbool_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::fog_node::set_bind_listener::~set_bind_listener()
    throw ()
{}

/**
 * @brief Process event.
 *
 * @param bind      bind state.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::fog_node::set_bind_listener::
do_process_event(const sfbool & bind, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        fog_node & fog = dynamic_cast<fog_node &>(this->node());
        fog_class & node_class =
            const_cast<fog_class &>(
                static_cast<const fog_class &>(fog.type().node_class()));
        if (bind.value) {
            node_class.bind(fog, timestamp);
        } else {
            node_class.unbind(fog, timestamp);
        }
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::fog_node::set_bind_listener openvrml::vrml97_node::fog_node::set_bind_listener_
 *
 * @brief set_bind eventIn handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfcolor> openvrml::vrml97_node::fog_node::color_
 *
 * @brief color exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfstring> openvrml::vrml97_node::fog_node::fog_type_
 *
 * @brief fogType exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::fog_node::visibility_range_
 *
 * @brief visibilityRange exposedField.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::fog_node::is_bound_
 *
 * @brief isBound eventOut value.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::fog_node::is_bound_emitter_
 *
 * @brief isBound eventOut emitter.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::fog_node::
fog_node(const node_type & type,
         const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    set_bind_listener_(*this),
    color_(*this, openvrml::color(1.0, 1.0, 1.0)),
    fog_type_(*this, "LINEAR"),
    visibility_range_(*this, 0.0),
    is_bound_emitter_(this->is_bound_)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::fog_node::~fog_node() throw ()
{}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void openvrml::vrml97_node::fog_node::do_initialize(const double timestamp)
    throw ()
{
    fog_class & nodeClass =
        const_cast<fog_class &>(
            static_cast<const fog_class &>(this->type().node_class()));
    if (!nodeClass.has_first()) { nodeClass.set_first(*this); }
}

/**
 * @brief Shut down.
 *
 * Calls fog_class::unbind to unbind the node if it is bound.
 *
 * @param timestamp the current time.
 */
void openvrml::vrml97_node::fog_node::do_shutdown(const double timestamp)
    throw ()
{
    fog_class & node_class =
        const_cast<fog_class &>(
            static_cast<const fog_class &>(this->type().node_class()));
    node_class.unbind(*this, timestamp);

    if (node_class.is_first(*this)) { node_class.reset_first(); }
}


/**
 * @class openvrml::vrml97_node::font_style_class
 *
 * @brief Class object for FontStyle nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::font_style_class::
font_style_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::font_style_class::~font_style_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating FontStyle nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by font_style_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::font_style_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "family"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "horizontal"),
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "justify"),
        node_interface(node_interface::field_id,
                       field_value::sfstring_id,
                       "language"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "leftToRight"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "size"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "spacing"),
        node_interface(node_interface::field_id,
                       field_value::sfstring_id,
                       "style"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "topToBottom")
    };

    typedef vrml97_node_type_impl<font_style_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & fontStyleNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfstring>(
                        &font_style_node::family_)));
        } else if (*interface == supportedInterfaces[1]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &font_style_node::horizontal_)));
        } else if (*interface == supportedInterfaces[2]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfstring>(
                        &font_style_node::justify_)));
        } else if (*interface == supportedInterfaces[3]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfstring>(
                        &font_style_node::language_)));
        } else if (*interface == supportedInterfaces[4]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &font_style_node::leftToRight)));
        } else if (*interface == supportedInterfaces[5]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &font_style_node::size_)));
        } else if (*interface == supportedInterfaces[6]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &font_style_node::spacing_)));
        } else if (*interface == supportedInterfaces[7]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfstring>(
                        &font_style_node::style_)));
        } else if (*interface == supportedInterfaces[8]) {
            fontStyleNodeType.add_field(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &font_style_node::topToBottom)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::font_style_node
 *
 * @brief FontStyle node instances.
 */

/**
 * @var openvrml::vrml97_node::font_style_node::font_style_class
 *
 * @brief Class object for FontStyle nodes.
 */

/**
 * @var openvrml::mfstring openvrml::vrml97_node::font_style_node::family_
 *
 * @brief family field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::font_style_node::horizontal_
 *
 * @brief horizontal field.
 */

/**
 * @var openvrml::mfstring openvrml::vrml97_node::font_style_node::justify_
 *
 * @brief justify field.
 */

/**
 * @var openvrml::sfstring openvrml::vrml97_node::font_style_node::language_
 *
 * @brief language field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::font_style_node::leftToRight
 *
 * @brief leftToRight field.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::font_style_node::size_
 *
 * @brief size field.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::font_style_node::spacing_
 *
 * @brief spacing field.
 */

/**
 * @var openvrml::sfstring openvrml::vrml97_node::font_style_node::style_
 *
 * @brief style field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::font_style_node::topToBottom
 *
 * @brief topToBottom field.
 */

namespace {
    const std::string fontStyleInitFamily_[] = { "SERIF" };
    const std::string fontStyleInitJustify_[] = { "BEGIN", "FIRST" };
}

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::font_style_node::
font_style_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::font_style_node(type, scope),
    family_(fontStyleInitFamily_, fontStyleInitFamily_ + 1),
    horizontal_(true),
    justify_(fontStyleInitJustify_, fontStyleInitJustify_ + 2),
    leftToRight(true),
    size_(1.0),
    spacing_(1.0),
    style_("PLAIN"),
    topToBottom(true)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::font_style_node::~font_style_node() throw ()
{}

/**
 * @brief Get the list of font families.
 *
 * @return an mfstring containing the font families that may be used for this
 *      font_style_node.
 */
const std::vector<std::string> &
openvrml::vrml97_node::font_style_node::family() const throw ()
{
    return this->family_.value;
}

/**
 * @brief Get the flag indicating whether the text should be horizontal or
 *      vertical.
 *
 * @return @c true if the text should be horizontal, or @c false if the text
 *      should be vertical.
 */
bool openvrml::vrml97_node::font_style_node::horizontal() const throw ()
{
    return this->horizontal_.value;
}

/**
 * @brief Get the descriptor for the text justification.
 *
 * @return a string array describing the characteristics of the text
 *      justification.
 */
const std::vector<std::string> &
openvrml::vrml97_node::font_style_node::justify() const throw ()
{
    return this->justify_.value;
}

/**
 * @brief Get the language code.
 *
 * @return the language code.
 */
const std::string & openvrml::vrml97_node::font_style_node::language() const
    throw ()
{
    return this->language_.value;
}

/**
 * @brief Get the flag indicating whether the text should be rendered
 *      left-to-right.
 *
 * @return @c true if the text should be rendered left-to-right, or @c false if
 *      the text should be rendered right-to-left.
 */
bool openvrml::vrml97_node::font_style_node::left_to_right() const throw ()
{
    return this->leftToRight.value;
}

/**
 * @brief Get the size of the text.
 *
 * @return the size of the text.
 */
float openvrml::vrml97_node::font_style_node::size() const throw ()
{
    return this->size_.value;
}

/**
 * @brief Get the spacing for the text.
 *
 * @return the spacing for the text.
 */
float openvrml::vrml97_node::font_style_node::spacing() const throw ()
{
    return this->spacing_.value;
}

/**
 * @brief Get the style for the text.
 *
 * @return an string descriptor of the text style.
 */
const std::string & openvrml::vrml97_node::font_style_node::style() const
    throw ()
{
    return this->style_.value;
}

/**
 * @brief Get the flag indicating whether the text should be rendered
 *      top-to-bottom.
 *
 * @return @c true if the text should be rendered top-to-bottom, or @c false if
 *      the text should be rendered bottom-to-top.
 */
bool openvrml::vrml97_node::font_style_node::top_to_bottom() const throw ()
{
    return this->topToBottom.value;
}


/**
 * @class openvrml::vrml97_node::group_class
 *
 * @brief Class object for Group nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml::vrml97_node::group_class::group_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::group_class::~group_class() throw () {}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating group nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by group_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::group_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    typedef vrml97_node_type_impl<group_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & groupNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
            interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            groupNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        group_node::add_children_listener>(
                            &group_node::add_children_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            groupNodeType.add_eventin(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        group_node::remove_children_listener>(
                            &group_node::remove_children_listener_)));
        } else if (*interface == supportedInterfaces[2]) {
            groupNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        group_node::children_exposedfield>(
                            &group_node::children_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                        group_node::children_exposedfield>(
                            &group_node::children_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                        group_node::children_exposedfield>(
                            &group_node::children_)));
        } else if (*interface == supportedInterfaces[3]) {
            groupNodeType.add_field(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &group_node::bbox_center_)));
        } else if (*interface == supportedInterfaces[4]) {
            groupNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &group_node::bbox_size_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::group_node
 *
 * @brief Represents Group node instances.
 */

/**
 * @var openvrml::vrml97_node::group_node::group_class
 *
 * @brief Class object for Group nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::group_node::add_children_listener
 *
 * @brief addChildren event listener.
 */

/**
 * @brief Construct.
 *
 * @param node  group_node.
 */
openvrml::vrml97_node::group_node::add_children_listener::
add_children_listener(group_node & node):
    mfnode_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::group_node::add_children_listener::
~add_children_listener() throw ()
{}

/**
 * @brief Process an event.
 *
 * If this function throws, it is possible (even likely) that only part of the
 * nodes in @p value have been added to the Group node's children exposedField.
 *
 * @param value     @link openvrml::child_node child_nodes@endlink to add.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc        if memory allocation fails.
 */
void
openvrml::vrml97_node::group_node::add_children_listener::
do_process_event(const mfnode & value, const double timestamp)
    throw (std::bad_alloc)
{
    using std::vector;

    group_node & group = dynamic_cast<group_node &>(this->node());

    for (vector<node_ptr>::const_iterator node = value.value.begin();
         node != value.value.end();
         ++node) {
        //
        // Don't add NULLs.
        //
        if (*node) {
            using std::find;
            using openvrml_::scope_guard;
            using openvrml_::make_obj_guard;

            vector<node_ptr>::iterator pos =
                find(group.children_.mfnode::value.begin(),
                     group.children_.mfnode::value.end(),
                     *node);
            if (pos == group.children_.mfnode::value.end()) {
                //
                // Throws std::bad_alloc.
                //
                group.children_.mfnode::value.push_back(*node);
                scope_guard guard =
                    make_obj_guard(group.children_.mfnode::value,
                                   &vector<node_ptr>::pop_back);
                child_node * const child =
                    node_cast<child_node *>(node->get());
                if (child) { child->relocate(); } // Throws std::bad_alloc.
                guard.dismiss();
            }
        }
    }

    group.node::modified(true);
    group.bounding_volume_dirty(true);
    node::emit_event(group.children_, timestamp);
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::group_node::remove_children_listener
 *
 * @brief removeChildren event listener.
 */

/**
 * @brief Construct.
 *
 * @param node  group_node.
 */
openvrml::vrml97_node::group_node::
remove_children_listener::remove_children_listener(group_node & node):
    mfnode_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::group_node::remove_children_listener::
~remove_children_listener() throw ()
{}

/**
 * @brief Process an event.
 */
void
openvrml::vrml97_node::group_node::remove_children_listener::
do_process_event(const mfnode & value, const double timestamp)
    throw (std::bad_alloc)
{
    using std::vector;

    group_node & group = dynamic_cast<group_node &>(this->node());

    for (vector<node_ptr>::const_iterator node = value.value.begin();
         node != value.value.end();
         ++node) {
        using std::remove;
        group.children_.mfnode::value
            .erase(remove(group.children_.mfnode::value.begin(),
                          group.children_.mfnode::value.end(),
                          *node),
                   group.children_.mfnode::value.end());
    }

    group.node::modified(true);
    group.bounding_volume_dirty(true);
    node::emit_event(group.children_, timestamp);
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::group_node::children_exposedfield
 *
 * @brief children exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  group_node.
 */
openvrml::vrml97_node::group_node::children_exposedfield::
children_exposedfield(openvrml::node & node) throw ():
    openvrml::exposedfield<openvrml::mfnode>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::group_node::children_exposedfield::
~children_exposedfield() throw ()
{}

/**
 * @brief Handle event.
 */
void
openvrml::vrml97_node::group_node::children_exposedfield::
event_side_effect(const mfnode & value, const double timestamp)
    throw (std::bad_alloc)
{
    using std::vector;

    group_node & group = dynamic_cast<group_node &>(this->node());

    this->mfnode::value.clear();

    for (vector<node_ptr>::const_iterator node = value.value.begin();
         node != value.value.end();
         ++node) {
        using openvrml_::scope_guard;
        using openvrml_::make_obj_guard;

        //
        // The spec is ambiguous about whether the children field of grouping
        // nodes can contain NULLs. We allow it; for now, at least.
        //
        this->mfnode::value.push_back(*node); // Throws std::bad_alloc.
        scope_guard guard = make_obj_guard(this->mfnode::value,
                                           &vector<node_ptr>::pop_back);
        child_node * const child = node_cast<child_node *>(node->get());
        if (child) { child->relocate(); } // Throws std::bad_alloc.
        guard.dismiss();
    }

    group.bounding_volume_dirty(true);
}

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::group_node::bbox_center_
 *
 * @brief bboxCenter field.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::group_node::bbox_size_
 *
 * @brief bboxSize field.
 */

/**
 * @var openvrml::vrml97_node::group_node::add_children_listener openvrml::vrml97_node::group_node::add_children_listener_
 *
 * @brief addChildren eventIn handler.
 */

/**
 * @var openvrml::vrml97_node::group_node::remove_children_listener openvrml::vrml97_node::group_node::remove_children_listener_
 *
 * @brief removeChildren eventIn handler.
 */

/**
 * @var openvrml::vrml97_node::group_node::children_exposedfield openvrml::vrml97_node::group_node::children_
 *
 * @brief children exposedField.
 */

/**
 * @var openvrml::viewer::object_t openvrml::vrml97_node::group_node::viewerObject
 *
 * @brief Handle for the renderer.
 */

/**
 * @var openvrml::vrml97_node::bounding_sphere openvrml::vrml97_node::group_node::bsphere
 *
 * @brief Cached copy of the bounding_sphere enclosing this node's children.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::group_node::
group_node(const node_type & type,
           const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    abstract_base(type, scope),
    bbox_size_(vec3f(-1.0, -1.0, -1.0)),
    add_children_listener_(*this),
    remove_children_listener_(*this),
    children_(*this),
    viewerObject(0)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::group_node::~group_node() throw ()
{
    // delete viewerObject...
}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::group_node::modified() const
{
    if (this->node::modified()) { return true; }
    for (size_t i = 0; i < this->children_.mfnode::value.size(); ++i) {
        if (this->children_.mfnode::value[i]->modified()) { return true; }
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
openvrml::vrml97_node::group_node::do_render_child(openvrml::viewer & viewer,
                                                   rendering_context context)
{
    if (context.cull_flag != bounding_volume::inside) {
        assert(dynamic_cast<const bounding_sphere *>
               (&this->bounding_volume()));
        const bounding_sphere & bs =
            static_cast<const bounding_sphere &>(this->bounding_volume());
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
openvrml::vrml97_node::group_node::render_nocull(openvrml::viewer & viewer,
                                                 rendering_context context)
{
    using std::vector;

    if (this->viewerObject && this->modified()) {
        viewer.remove_object(this->viewerObject);
        this->viewerObject = 0;
    }

    if (this->viewerObject) {
        viewer.insert_reference(this->viewerObject);
    } else if (!this->children_.mfnode::value.empty()) {
        vector<node_ptr>::size_type i;
        vector<node_ptr>::size_type n = this->children_.mfnode::value.size();
        size_t nSensors = 0;

        this->viewerObject = viewer.begin_object(this->id().c_str());

        // Draw nodes that impact their siblings (DirectionalLights,
        // TouchSensors, any others? ...)
        for (i = 0; i < n; ++i) {
            child_node * const child =
                node_cast<child_node *>(
                    this->children_.mfnode::value[i].get());
            if (child) {
                if (child->to_light()
                    && !(child->to_point_light() || child->to_spot_light())) {
                    child->render_child(viewer, context);
                } else if ((child->to_touch_sensor()
                            && child->to_touch_sensor()->enabled())
                           || (child->to_plane_sensor()
                               && child->to_plane_sensor()->enabled())
                           || (child->to_cylinder_sensor()
                               && child->to_cylinder_sensor()->enabled())
                           || (child->to_sphere_sensor()
                               && child->to_sphere_sensor()->isEnabled())) {
                    if (++nSensors == 1) { viewer.set_sensitive(this); }
                }
            }
        }

        // Do the rest of the children (except the scene-level lights)
        for (i = 0; i<n; ++i) {
            child_node * const child =
                node_cast<child_node *>(
                    this->children_.mfnode::value[i].get());
            if (child && !(child->to_light()
//                    || child->to_plane_sensor()
//                    || child->to_cylinder_sensor()
//                    || child->to_sphere_sensor()
                    || child->to_touch_sensor())) {
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
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */
const std::vector<openvrml::node_ptr> &
openvrml::vrml97_node::group_node::children() const throw ()
{
    return this->children_.mfnode::value;
}

/**
 * Pass on to enabled touchsensor child.
 */
void
openvrml::vrml97_node::group_node::activate(double time,
                                            bool isOver,
                                            bool isActive,
                                            double *p)
{
    for (size_t i = 0; i < this->children_.mfnode::value.size(); ++i) {
        const node_ptr & node = this->children_.mfnode::value[i];
        if (node) {
            if (node->to_touch_sensor()
                    && node->to_touch_sensor()->enabled()) {
                node->to_touch_sensor()->activate(time, isOver, isActive, p);
            } else if (node->to_plane_sensor()
                    && node->to_plane_sensor()->enabled()) {
                node->to_plane_sensor()->activate(time, isActive, p);
            } else if (node->to_cylinder_sensor()
                    && node->to_cylinder_sensor()->enabled()) {
                node->to_cylinder_sensor()->activate(time, isActive, p);
            } else if (node->to_sphere_sensor()
                    && node->to_sphere_sensor()->isEnabled()) {
                node->to_sphere_sensor()->activate(time, isActive, p);
            }
        }
    }
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const openvrml::bounding_volume &
openvrml::vrml97_node::group_node::do_bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<group_node *>(this)->recalc_bsphere();
    }
    return this->bsphere;
}

/**
 * @brief Recalculate the bounding volume.
 */
void openvrml::vrml97_node::group_node::recalc_bsphere()
{
    this->bsphere = bounding_sphere();
    for (size_t i = 0; i < this->children_.mfnode::value.size(); ++i) {
        const node_ptr & node = this->children_.mfnode::value[i];
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
 * @class openvrml::vrml97_node::image_texture_class
 *
 * @brief Class object for ImageTexture nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml::vrml97_node::image_texture_class::
image_texture_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::image_texture_class::~image_texture_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating ImageTexture
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by image_texture_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::image_texture_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "url"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "repeatS"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "repeatT")
    };

    typedef vrml97_node_type_impl<image_texture_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & imageTextureNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            imageTextureNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        image_texture_node::url_exposedfield>(
                            &image_texture_node::url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                        image_texture_node::url_exposedfield>(
                            &image_texture_node::url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                        image_texture_node::url_exposedfield>(
                            &image_texture_node::url_)));
        } else if (*interface == supportedInterfaces[1]) {
            imageTextureNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &image_texture_node::repeat_s_)));
        } else if (*interface == supportedInterfaces[2]) {
            imageTextureNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &image_texture_node::repeat_t_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::image_texture_node
 *
 * @brief Represents ImageTexture node instances.
 */

/**
 * @var class openvrml::vrml97_node::image_texture_node::image_texture_class
 *
 * @brief Class object for ImageTexture nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::image_texture_node::url_exposedfield
 *
 * @brief url exposedField.
 */

/**
 * @brief Construct.
 *
 * @param node  image_texture_node.
 */
openvrml::vrml97_node::image_texture_node::url_exposedfield::
url_exposedfield(image_texture_node & node):
    openvrml::exposedfield<openvrml::mfstring>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::image_texture_node::url_exposedfield::
~url_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param url       url.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::image_texture_node::url_exposedfield::
event_side_effect(const mfstring & url, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        image_texture_node & image_texture =
            dynamic_cast<image_texture_node &>(this->node());

        image_texture.url_.mfstring::value = url.value;
        image_texture.texture_needs_update = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::image_texture_node::url_exposedfield openvrml::vrml97_node::image_texture_node::url_
 *
 * @brief url exposedField.
 */

/**
 * @var openvrml::image openvrml::vrml97_node::image_texture_node::image_
 *
 * @brief Image data.
 */

/**
 * @var bool openvrml::vrml97_node::image_texture_node::texture_needs_update
 *
 * @brief Flag to indicate that image_texture_node::update_texture should be
 *        called.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::image_texture_node::
image_texture_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_texture_node(type, scope),
    url_(*this),
    texture_needs_update(true)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::image_texture_node::~image_texture_node() throw ()
{
    // delete texObject...
}

/**
 * @brief The image.
 *
 * @return the image.
 */
const openvrml::image &
openvrml::vrml97_node::image_texture_node::image() const throw ()
{
    return this->image_;
}

/**
 * @brief render_texture implementation.
 *
 * @param v viewer.
 *
 * @return object identifier for the inserted texture.
 */
openvrml::viewer::texture_object_t
openvrml::vrml97_node::image_texture_node::do_render_texture(viewer & v)
{
    this->update_texture();
    return v.insert_texture(this->image_,
                            this->repeat_s_.value,
                            this->repeat_t_.value,
                            true);
}

/**
 * @brief Called to lazily update texture data.
 */
void openvrml::vrml97_node::image_texture_node::update_texture()
{
    if (this->texture_needs_update) {
        if (!this->url_.mfstring::value.empty()) {
            using std::auto_ptr;
            auto_ptr<resource_istream>
                in(this->scene()->get_resource(this->url_.mfstring::value));
            auto_ptr<stream_listener>
                listener(new image_stream_listener(this->image_,
                                                   *this,
                                                   this->mutex()));
            read_stream(in, listener);
        }
        this->texture_needs_update = false;
    }
}


/**
 * @class openvrml::vrml97_node::indexed_face_set_class
 *
 * @brief Class object for IndexedFaceSet nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml::vrml97_node::indexed_face_set_class::
indexed_face_set_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::indexed_face_set_class::~indexed_face_set_class()
    throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating IndexedFaceSet
 *         nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by indexed_face_set_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::indexed_face_set_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::mfint32_id,
                       "set_colorIndex"),
        node_interface(node_interface::eventin_id,
                       field_value::mfint32_id,
                       "set_coordIndex"),
        node_interface(node_interface::eventin_id,
                       field_value::mfint32_id,
                       "set_normalIndex"),
        node_interface(node_interface::eventin_id,
                       field_value::mfint32_id,
                       "set_texCoordIndex"),
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
                       field_value::mfint32_id,
                       "colorIndex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "colorPerVertex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "convex"),
        node_interface(node_interface::field_id,
                       field_value::mfint32_id,
                       "coordIndex"),
        node_interface(node_interface::field_id,
                       field_value::sffloat_id,
                       "creaseAngle"),
        node_interface(node_interface::field_id,
                       field_value::mfint32_id,
                       "normalIndex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "normalPerVertex"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "solid"),
        node_interface(node_interface::field_id,
                       field_value::mfint32_id,
                       "texCoordIndex")
    };

    typedef vrml97_node_type_impl<indexed_face_set_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & indexedFaceSetNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            indexedFaceSetNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        indexed_face_set_node::set_color_index_listener>(
                            &indexed_face_set_node::set_color_index_)));
        } else if (*interface == supportedInterfaces[1]) {
            indexedFaceSetNodeType.add_eventin(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        indexed_face_set_node::set_coord_index_listener>(
                            &indexed_face_set_node::set_coord_index_)));
        } else if (*interface == supportedInterfaces[2]) {
            indexedFaceSetNodeType.add_eventin(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        indexed_face_set_node::set_normal_index_listener>(
                            &indexed_face_set_node::set_normal_index_listener_)));
        } else if (*interface == supportedInterfaces[3]) {
            indexedFaceSetNodeType.add_eventin(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        indexed_face_set_node::set_tex_coord_index_listener>(
                            &indexed_face_set_node::set_tex_coord_index_listener_)));
        } else if (*interface == supportedInterfaces[4]) {
            indexedFaceSetNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::color_)));
        } else if (*interface == supportedInterfaces[5]) {
            indexedFaceSetNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::coord_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::coord_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::coord_)));
        } else if (*interface == supportedInterfaces[6]) {
            indexedFaceSetNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::normal_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::normal_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::normal_)));
        } else if (*interface == supportedInterfaces[7]) {
            indexedFaceSetNodeType.add_exposedfield(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::tex_coord_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::tex_coord_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &indexed_face_set_node::tex_coord_)));
        } else if (*interface == supportedInterfaces[8]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_face_set_node::ccw_)));
        } else if (*interface == supportedInterfaces[9]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_face_set_node::color_index_)));
        } else if (*interface == supportedInterfaces[10]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[10].field_type,
                supportedInterfaces[10].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_face_set_node::color_per_vertex_)));
        } else if (*interface == supportedInterfaces[11]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[11].field_type,
                supportedInterfaces[11].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_face_set_node::convex_)));
        } else if (*interface == supportedInterfaces[12]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[12].field_type,
                supportedInterfaces[12].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_face_set_node::coord_index_)));
        } else if (*interface == supportedInterfaces[13]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[13].field_type,
                supportedInterfaces[13].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &indexed_face_set_node::crease_angle_)));
        } else if (*interface == supportedInterfaces[14]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[14].field_type,
                supportedInterfaces[14].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_face_set_node::normal_index_)));
        } else if (*interface == supportedInterfaces[15]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[15].field_type,
                supportedInterfaces[15].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_face_set_node::normal_per_vertex_)));
        } else if (*interface == supportedInterfaces[16]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[16].field_type,
                supportedInterfaces[16].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_face_set_node::solid_)));
        } else if (*interface == supportedInterfaces[17]) {
            indexedFaceSetNodeType.add_field(
                supportedInterfaces[17].field_type,
                supportedInterfaces[17].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_face_set_node::tex_coord_index_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::indexed_face_set_node
 *
 * @brief Represents IndexedFaceSet node instances.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::indexed_face_set_node::set_normal_index_listener
 *
 * @brief set_normalIndex event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  indexed_face_set_node.
 */
openvrml::vrml97_node::indexed_face_set_node::set_normal_index_listener::
set_normal_index_listener(indexed_face_set_node & node):
    mfint32_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::indexed_face_set_node::set_normal_index_listener::
~set_normal_index_listener() throw ()
{}

/**
 * @brief Process event.
 *
 * @param normal_index normalIndex.
 * @param timestamp   the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::indexed_face_set_node::set_normal_index_listener::
do_process_event(const mfint32 & normal_index,
              const double timestamp)
    throw (std::bad_alloc)
{
    try {
        indexed_face_set_node & indexed_face_set =
            dynamic_cast<indexed_face_set_node &>(this->node());

        indexed_face_set.normal_index_ = normal_index;
        indexed_face_set.node::modified(true);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::indexed_face_set_node::set_tex_coord_index_listener
 *
 * @brief set_coordIndex event handler.
 */

/**
 * @brief Construct.
 *
 * @param node  indexed_face_set_node.
 */
openvrml::vrml97_node::indexed_face_set_node::set_tex_coord_index_listener::
set_tex_coord_index_listener(indexed_face_set_node & node):
    mfint32_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::indexed_face_set_node::set_tex_coord_index_listener::
~set_tex_coord_index_listener() throw ()
{}

/**
 * @brief Process event.
 *
 * @param tex_coord_index   texCoordIndex.
 * @param timestamp         the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::indexed_face_set_node::set_tex_coord_index_listener::
do_process_event(const mfint32 & tex_coord_index, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        indexed_face_set_node & indexed_face_set =
            dynamic_cast<indexed_face_set_node &>(this->node());

        indexed_face_set.tex_coord_index_ = tex_coord_index;
        indexed_face_set.node::modified(true);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::indexed_face_set_node::indexed_face_set_class
 *
 * @brief Class object for IndexedFaceSet nodes.
 */

/**
 * @var openvrml::vrml97_node::indexed_face_set_node::set_normal_index_listener openvrml::vrml97_node::indexed_face_set_node::set_normal_index_listener_
 *
 * @brief set_normalIndex eventIn handler.
 */

/**
 * @var openvrml::vrml97_node::indexed_face_set_node::set_tex_coord_index_listener openvrml::vrml97_node::indexed_face_set_node::set_tex_coord_index_listener_
 *
 * @brief set_texCoordIndex eventIn handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::indexed_face_set_node::normal_
 *
 * @brief normal exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::indexed_face_set_node::tex_coord_
 *
 * @brief texCoord exposedField.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::indexed_face_set_node::ccw_
 *
 * @brief ccw field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::indexed_face_set_node::convex_
 *
 * @brief convex field.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::indexed_face_set_node::crease_angle_
 *
 * @brief creaseAngle field.
 */

/**
 * @var openvrml::mfint32 openvrml::vrml97_node::indexed_face_set_node::normal_index_
 *
 * @brief set_normalIndex eventIn.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::indexed_face_set_node::normal_per_vertex_
 *
 * @brief normalPerVertex field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::indexed_face_set_node::solid_
 *
 * @brief solid field.
 */

/**
 * @var openvrml::mfint32 openvrml::vrml97_node::indexed_face_set_node::tex_coord_index_
 *
 * @brief set_texCoordIndex eventIn.
 */

/**
 * @var bounding_sphere openvrml::vrml97_node::indexed_face_set_node::bsphere
 *
 * @brief Bounding volume.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::indexed_face_set_node::
indexed_face_set_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    abstract_indexed_set_node(type, scope),
    set_normal_index_listener_(*this),
    set_tex_coord_index_listener_(*this),
    normal_(*this),
    tex_coord_(*this),
    ccw_(true),
    convex_(true),
    crease_angle_(0.0),
    normal_per_vertex_(true),
    solid_(true)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::indexed_face_set_node::~indexed_face_set_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::indexed_face_set_node::modified() const
{
    return this->node::modified()
        || (this->color_.sfnode::value
            && this->color_.sfnode::value->modified())
        || (this->coord_.sfnode::value
            && this->coord_.sfnode::value->modified())
        || (this->normal_.sfnode::value
            && this->normal_.sfnode::value->modified())
        || (this->tex_coord_.sfnode::value
            && this->tex_coord_.sfnode::value->modified());
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 *
 * @todo stripify, crease angle, generate normals ...
 */
openvrml::viewer::object_t
openvrml::vrml97_node::indexed_face_set_node::
do_render_geometry(openvrml::viewer & viewer, const rendering_context context)
{
    using std::vector;

    if (context.draw_bounding_spheres) {
        assert(dynamic_cast<const bounding_sphere *>(
                   &this->bounding_volume()));
        const bounding_sphere & bs =
            static_cast<const bounding_sphere &>(this->bounding_volume());
        viewer.draw_bounding_sphere(bs,
                                static_cast<bounding_volume::intersection>(4));
    }

    openvrml::coordinate_node * const coordinateNode =
        node_cast<openvrml::coordinate_node *>(
            this->coord_.sfnode::value.get());
    const vector<vec3f> & coord = coordinateNode
                                ? coordinateNode->point()
                                : vector<vec3f>();

    openvrml::color_node * const colorNode =
        node_cast<openvrml::color_node *>(this->color_.sfnode::value.get());
    const vector<openvrml::color> & color = colorNode
                                          ? colorNode->color()
                                          : vector<openvrml::color>();

    openvrml::normal_node * const normalNode =
        node_cast<openvrml::normal_node *>(this->normal_.sfnode::value.get());
    const vector<vec3f> & normal = normalNode
                                 ? normalNode->vector()
                                 : vector<vec3f>();

    openvrml::texture_coordinate_node * const texCoordNode =
        node_cast<openvrml::texture_coordinate_node *>(
            this->tex_coord_.sfnode::value.get());
    const vector<vec2f> & texCoord = texCoordNode
                                   ? texCoordNode->point()
                                   : vector<vec2f>();

    unsigned int optMask = 0;
    if (this->ccw_.value) {
        optMask |= viewer::mask_ccw;
    }
    if (this->convex_.value) {
        optMask |= viewer::mask_convex;
    }
    if (this->solid_.value) {
        optMask |= viewer::mask_solid;
    }
    if (this->color_per_vertex_.value) {
        optMask |= viewer::mask_color_per_vertex;
    }
    if (this->normal_per_vertex_.value) {
        optMask |= viewer::mask_normal_per_vertex;
    }

    const viewer::object_t obj =
        viewer.insert_shell(optMask,
                            coord, this->coord_index_.value,
                            color, this->color_index_.value,
                            normal, this->normal_index_.value,
                            texCoord, this->tex_coord_index_.value);

    if (colorNode) { colorNode->modified(false); }
    if (coordinateNode) { coordinateNode->modified(false); }
    if (normalNode) { normalNode->modified(false); }
    if (texCoordNode) { texCoordNode->modified(false); }

    return obj;
}

/**
 * @brief Recalculate the bounding volume.
 */
void openvrml::vrml97_node::indexed_face_set_node::recalcBSphere()
{
    // take the bvolume of all the points. technically, we should figure
    // out just which points are used by the index and just use those,
    // but for a first pass this is fine (also: if we do it this way
    // then we don't have to update the bvolume when the index
    // changes). motto: always do it the simple way first...
    //
    openvrml::coordinate_node * const coordinateNode =
        node_cast<openvrml::coordinate_node *>(
            this->coord_.sfnode::value.get());
    if (coordinateNode) {
        const std::vector<vec3f> & coord = coordinateNode->point();
        this->bsphere = bounding_sphere();
        this->bsphere.enclose(coord);
    }
    this->bounding_volume_dirty(false);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const openvrml::bounding_volume &
openvrml::vrml97_node::indexed_face_set_node::do_bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<indexed_face_set_node *>(this)->recalcBSphere();
    }
    return this->bsphere;
}


/**
 * @class openvrml::vrml97_node::indexed_line_set_class
 *
 * @brief Class object for IndexedLineSet nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml::vrml97_node::indexed_line_set_class::
indexed_line_set_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::indexed_line_set_class::~indexed_line_set_class()
    throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating IndexedLineSet
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by indexed_line_set_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::indexed_line_set_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                      field_value::mfint32_id,
                      "set_colorIndex"),
        node_interface(node_interface::eventin_id,
                      field_value::mfint32_id,
                      "set_coordIndex"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sfnode_id,
                      "color"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sfnode_id,
                      "coord"),
        node_interface(node_interface::field_id,
                      field_value::mfint32_id,
                      "colorIndex"),
        node_interface(node_interface::field_id,
                      field_value::sfbool_id,
                      "colorPerVertex"),
        node_interface(node_interface::field_id,
                      field_value::mfint32_id,
                      "coordIndex")
    };

    typedef vrml97_node_type_impl<indexed_line_set_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & indexedLineSetNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            indexedLineSetNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        indexed_line_set_node::set_color_index_listener>(
                            &indexed_line_set_node::set_color_index_)));
        } else if (*interface == supportedInterfaces[1]) {
            indexedLineSetNodeType.add_eventin(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        indexed_line_set_node::set_coord_index_listener>(
                            &indexed_line_set_node::set_coord_index_)));
        } else if (*interface == supportedInterfaces[2]) {
            indexedLineSetNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &indexed_line_set_node::color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &indexed_line_set_node::color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &indexed_line_set_node::color_)));
        } else if (*interface == supportedInterfaces[3]) {
            indexedLineSetNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &indexed_line_set_node::coord_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &indexed_line_set_node::coord_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &indexed_line_set_node::coord_)));
        } else if (*interface == supportedInterfaces[4]) {
            indexedLineSetNodeType.add_field(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_line_set_node::color_index_)));
        } else if (*interface == supportedInterfaces[5]) {
            indexedLineSetNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &indexed_line_set_node::color_per_vertex_)));
        } else if (*interface == supportedInterfaces[6]) {
            indexedLineSetNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfint32>(
                        &indexed_line_set_node::coord_index_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::indexed_line_set_node
 *
 * @brief Represents IndexedLineSet node instances.
 */

/**
 * @var openvrml::vrml97_node::indexed_line_set_node::indexed_line_set_class
 *
 * @brief Class object for IndexedLineSet nodes.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::indexed_line_set_node::
indexed_line_set_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    abstract_indexed_set_node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::indexed_line_set_node::~indexed_line_set_node() throw ()
{}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 *
 * @todo colors
 */
openvrml::viewer::object_t
openvrml::vrml97_node::indexed_line_set_node::
do_render_geometry(openvrml::viewer & viewer, const rendering_context context)
{
    using std::vector;

    openvrml::coordinate_node * const coordinateNode =
        node_cast<openvrml::coordinate_node *>(
            this->coord_.sfnode::value.get());
    const vector<vec3f> & coord = coordinateNode
                                ? coordinateNode->point()
                                : vector<vec3f>();

    openvrml::color_node * const colorNode =
        node_cast<openvrml::color_node *>(this->color_.sfnode::value.get());
    const vector<openvrml::color> & color = colorNode
                                          ? colorNode->color()
                                          : vector<openvrml::color>();

    viewer::object_t obj =
        viewer.insert_line_set(coord, this->coord_index_.value,
                               this->color_per_vertex_.value,
                               color, this->color_index_.value);

    if (colorNode) { colorNode->modified(false); }
    if (coordinateNode) { coordinateNode->modified(false); }

    return obj;
}

/**
 * @brief Indicate that line sets should be drawn with the emissive color.
 *
 * @return @c true.
 */
bool openvrml::vrml97_node::indexed_line_set_node::do_emissive() const throw ()
{
    return true;
}


/**
 * @class openvrml::vrml97_node::inline_class
 *
 * @brief Class object for Inline nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml::vrml97_node::inline_class::inline_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::inline_class::~inline_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Inline nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by inline_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::inline_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "url"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    typedef vrml97_node_type_impl<inline_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & inlineNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            inlineNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<mfstring> >(&inline_node::url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfstring> >(
                        &inline_node::url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                        exposedfield<mfstring> >(&inline_node::url_)));
        } else if (*interface == supportedInterfaces[1]) {
            inlineNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &inline_node::bbox_center_)));
        } else if (*interface == supportedInterfaces[2]) {
            inlineNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &inline_node::bbox_size_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::inline_node
 *
 * @brief Represents Inline node instances.
 */

/**
 * @var openvrml::vrml97_node::inline_node::inline_class
 *
 * @brief Class object for Inline nodes.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::inline_node::bbox_center_
 *
 * @brief bboxCenter field.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::inline_node::bbox_size_
 *
 * @brief bboxSize field.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfstring> openvrml::vrml97_node::inline_node::url_
 *
 * @brief url exposedField.
 */

/**
 * @var openvrml::scene * openvrml::vrml97_node::inline_node::inlineScene
 *
 * @brief The contained scene.
 */

/**
 * @var bool openvrml::vrml97_node::inline_node::hasLoaded
 *
 * @brief Flag to indicate whether the scene has been loaded.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::inline_node::
inline_node(const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    abstract_base(type, scope),
    grouping_node(type, scope),
    url_(*this),
    inlineScene(0),
    hasLoaded(false)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::inline_node::~inline_node() throw ()
{}

/**
 * @brief Render the node.
 *
 * Render each of the children.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void
openvrml::vrml97_node::inline_node::
do_render_child(openvrml::viewer & viewer,
                const rendering_context context)
{
    this->load();
    if (this->inlineScene) { this->inlineScene->render(viewer, context); }
}

/**
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */
const std::vector<openvrml::node_ptr> &
openvrml::vrml97_node::inline_node::children() const throw ()
{
    static const std::vector<openvrml::node_ptr> empty;
    return this->inlineScene
            ? this->inlineScene->nodes()
            : empty;
}

/**
 * Pass on to enabled touchsensor child.
 */
void openvrml::vrml97_node::inline_node::activate(double time,
                                                  bool isOver,
                                                  bool isActive,
                                                  double *p)
{
    const std::vector<node_ptr> & children = this->children();
    for (size_t i = 0; i < children.size(); ++i) {
        const node_ptr & node = children[i];
        if (node) {
            if (node->to_touch_sensor()
                    && node->to_touch_sensor()->enabled()) {
                node->to_touch_sensor()->activate(time, isOver, isActive, p);
            } else if (node->to_plane_sensor()
                    && node->to_plane_sensor()->enabled()) {
                node->to_plane_sensor()->activate(time, isActive, p);
            } else if (node->to_cylinder_sensor()
                    && node->to_cylinder_sensor()->enabled()) {
                node->to_cylinder_sensor()->activate(time, isActive, p);
            } else if (node->to_sphere_sensor()
                    && node->to_sphere_sensor()->isEnabled()) {
                node->to_sphere_sensor()->activate(time, isActive, p);
            }
        }
    }
}

/**
 * @brief Load the children from the URL.
 */
void openvrml::vrml97_node::inline_node::load()
{
    class inline_scene : public openvrml::scene {
    public:
        inline_scene(openvrml::browser & b, openvrml::scene * parent):
            openvrml::scene(b, parent)
        {}

    private:
        virtual void scene_loaded()
        {
            this->initialize(openvrml::browser::current_time());
        }
    };

    //
    // XXX Need to check whether Url has been modified.
    //
    if (this->hasLoaded) { return; }

    this->hasLoaded = true; // although perhaps not successfully
    this->bounding_volume_dirty(true);

    assert(this->scene());
    this->inlineScene = new inline_scene(this->scene()->browser(),
                                         this->scene());
    this->inlineScene->load(this->url_.mfstring::value);
}


/**
 * @class openvrml::vrml97_node::lod_class
 *
 * @brief Class object for LOD nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml::vrml97_node::lod_class::lod_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::lod_class::~lod_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating LOD nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by lod_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::lod_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "level"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "center"),
        node_interface(node_interface::field_id,
                       field_value::mffloat_id,
                       "range")
    };

    typedef vrml97_node_type_impl<lod_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & lodNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            lodNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<mfnode> >(
                        &lod_node::level_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfnode> >(
                        &lod_node::level_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mfnode> >(
                        &lod_node::level_)));
        } else if (*interface == supportedInterfaces[1]) {
            lodNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(&lod_node::center_)));
        } else if (*interface == supportedInterfaces[2]) {
            lodNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mffloat>(&lod_node::range_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::lod_node
 *
 * @brief Represents LOD node instances.
 */

/**
 * @var openvrml::vrml97_node::lod_node::lod_class
 *
 * @brief Class object for LOD nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfnode> openvrml::vrml97_node::lod_node::level_
 *
 * @brief level exposedField.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::lod_node::center_
 *
 * @brief center field.
 */

/**
 * @var openvrml::mffloat openvrml::vrml97_node::lod_node::range_
 *
 * @brief range field.
 */

/**
 * @var openvrml::mfnode openvrml::vrml97_node::lod_node::children_
 *
 * @brief Caches the active level for fast access by lod_node::children.
 */

/**
 * @var openvrml::bounding_sphere openvrml::vrml97_node::lod_node::bsphere
 *
 * @brief Cached copy of the bounding_sphere enclosing this node's children.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::lod_node::
lod_node(const node_type & type,
         const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    abstract_base(type, scope),
    grouping_node(type, scope),
    level_(*this),
    children_(1)
{
    this->bounding_volume_dirty(true); // lazy calc of bvolume
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::lod_node::~lod_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::lod_node::modified() const
{
    if (this->node::modified()) { return true; }

    // This should really check which range is being rendered...
    for (size_t i = 0; i < this->level_.mfnode::value.size(); ++i) {
        if (this->level_.mfnode::value[i]->modified()) { return true; }
    }
    return false;
}

/**
 * @brief Render the node.
 *
 * Render one of the children.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void
openvrml::vrml97_node::lod_node::
do_render_child(openvrml::viewer & viewer,
                const rendering_context context)
{
    this->node::modified(false);
    if (this->level_.mfnode::value.empty()) { return; }

    float x, y, z;

    mat4f MV = context.matrix();
    MV = MV.inverse();
    x = MV[3][0]; y = MV[3][1]; z = MV[3][2];
    float dx = x - this->center_.value.x();
    float dy = y - this->center_.value.y();
    float dz = z - this->center_.value.z();
    float d2 = dx * dx + dy * dy + dz * dz;

    size_t i;
    for (i = 0; i < this->range_.value.size(); ++i) {
        if (d2 < this->range_.value[i] * this->range_.value[i]) { break; }
    }

    // Should choose an "optimal" level...
    if (this->range_.value.empty()) {
        i = this->level_.mfnode::value.size() - 1;
    }

    // Not enough levels...
    if (i >= this->level_.mfnode::value.size()) {
        i = this->level_.mfnode::value.size() - 1;
    }

    child_node * const child =
        node_cast<child_node *>(this->level_.mfnode::value[i].get());
    if (child) { child->render_child(viewer, context); }

    // Don't re-render on their accounts
    for (i = 0; i < this->level_.mfnode::value.size(); ++i) {
        this->level_.mfnode::value[i]->modified(false);
    }
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const openvrml::bounding_volume &
openvrml::vrml97_node::lod_node::do_bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<lod_node *>(this)->recalcBSphere();
    }
    return this->bsphere;
}

/**
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */
const std::vector<openvrml::node_ptr> &
openvrml::vrml97_node::lod_node::children() const throw ()
{
    return this->children_.value;
}

/**
 * Pass on to enabled touchsensor child.
 */
void openvrml::vrml97_node::lod_node::activate(double time,
                                               bool isOver,
                                               bool isActive,
                                               double *p)
{
    const std::vector<node_ptr> & children = this->children();
    const node_ptr & node = children[0];
    if (node) {
        if (node->to_touch_sensor() && node->to_touch_sensor()->enabled()) {
            node->to_touch_sensor()->activate(time, isOver, isActive, p);
        } else if (node->to_plane_sensor()
                && node->to_plane_sensor()->enabled()) {
            node->to_plane_sensor()->activate(time, isActive, p);
        } else if (node->to_cylinder_sensor()
                && node->to_cylinder_sensor()->enabled()) {
            node->to_cylinder_sensor()->activate(time, isActive, p);
        } else if (node->to_sphere_sensor()
                && node->to_sphere_sensor()->isEnabled()) {
            node->to_sphere_sensor()->activate(time, isActive, p);
        }
    }
}

/**
 * @brief Recalculate the bounding volume.
 */
void openvrml::vrml97_node::lod_node::recalcBSphere()
{
    this->bsphere = bounding_sphere();

    // let's say our bsphere is the union of the bspheres of all the
    // levels. we could have said it was just the bsphere of the current
    // level, but the current level depends on the viewer position, and
    // we'd like to make the calculation idependent of that. we could do
    // some sort of trick where we reset the bsphere during render, but
    // that seems like overkill unless this simpler method proves to be
    // a bottleneck.
    //
    // hmm: just thought of a problem: one of the uses of the lod is to
    // switch in delayed-load inlines. this would necessarily switch
    // them in all at once. live with it for now.
    //
    for (size_t i = 0; i < this->level_.mfnode::value.size(); i++) {
        const node_ptr & node = this->level_.mfnode::value[i];
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
 * @class openvrml::vrml97_node::material_class
 *
 * @brief Class object for Material nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml::vrml97_node::material_class::
material_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::material_class::~material_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Material nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by material_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::material_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "ambientIntensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "diffuseColor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "emissiveColor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "shininess"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "specularColor"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "transparency")
    };

    typedef vrml97_node_type_impl<material_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & materialNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sffloat> >(
                            &material_node::ambient_intensity_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &material_node::ambient_intensity_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &material_node::ambient_intensity_)));
        } else if (*interface == supportedInterfaces[1]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sfcolor> >(
                            &material_node::diffuse_color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfcolor> >(
                        &material_node::diffuse_color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfcolor> >(
                        &material_node::diffuse_color_)));
        } else if (*interface == supportedInterfaces[2]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sfcolor> >(
                            &material_node::emissive_color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfcolor> >(
                        &material_node::emissive_color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfcolor> >(
                        &material_node::emissive_color_)));
        } else if (*interface == supportedInterfaces[3]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sffloat> >(
                            &material_node::shininess_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &material_node::shininess_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &material_node::shininess_)));
        } else if (*interface == supportedInterfaces[4]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sfcolor> >(
                            &material_node::specular_color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfcolor> >(
                        &material_node::specular_color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfcolor> >(
                        &material_node::specular_color_)));
        } else if (*interface == supportedInterfaces[5]) {
            materialNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                        exposedfield<sffloat> >(
                            &material_node::transparency_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &material_node::transparency_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &material_node::transparency_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::material_node
 *
 * @brief Material node instances.
 */

/**
 * @var openvrml::vrml97_node::material_node::material_class
 *
 * @brief Class object for Material nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::material_node::ambient_intensity_
 *
 * @brief ambientIntensity exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfcolor> openvrml::vrml97_node::material_node::diffuse_color_
 *
 * @brief diffuseColor exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfcolor> openvrml::vrml97_node::material_node::emissive_color_
 *
 * @brief emissiveColor exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::material_node::shininess_
 *
 * @brief shininess exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfcolor> openvrml::vrml97_node::material_node::specular_color_
 *
 * @brief specularColor exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::material_node::transparency_
 *
 * @brief transparency exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with this node.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::material_node::
material_node(const node_type & type,
              const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::material_node(type, scope),
    ambient_intensity_(*this, 0.2f),
    diffuse_color_(*this, color(0.8f, 0.8f, 0.8f)),
    emissive_color_(*this, color(0.0, 0.0, 0.0)),
    shininess_(*this, 0.2f),
    specular_color_(*this, color(0.0, 0.0, 0.0)),
    transparency_(*this, 0.0)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::material_node::~material_node() throw ()
{}

/**
 * @brief Get the ambient intensity.
 *
 * @return the ambient intensity.
 */
float openvrml::vrml97_node::material_node::ambient_intensity() const throw ()
{
    return this->ambient_intensity_.sffloat::value;
}

/**
 * @brief Get the diffuse color.
 *
 * @return the diffuse color.
 */
const openvrml::color &
openvrml::vrml97_node::material_node::diffuse_color() const throw ()
{
    return this->diffuse_color_.sfcolor::value;
}

/**
 * @brief Get the emissive color.
 *
 * @return the emissive color.
 */
const openvrml::color &
openvrml::vrml97_node::material_node::emissive_color() const throw ()
{
    return this->emissive_color_.sfcolor::value;
}

/**
 * @brief Get the shininess.
 *
 * @return the shininess.
 */
float openvrml::vrml97_node::material_node::shininess() const throw ()
{
    return this->shininess_.sffloat::value;
}

/**
 * @brief Get the specular color.
 *
 * @return the specular color.
 */
const openvrml::color &
openvrml::vrml97_node::material_node::specular_color() const throw ()
{
    return this->specular_color_.sfcolor::value;
}

/**
 * @brief Get the transparency.
 *
 * @return the transparency.
 */
float openvrml::vrml97_node::material_node::transparency() const throw ()
{
    return this->transparency_.sffloat::value;
}


/**
 * @class openvrml::vrml97_node::movie_texture_class
 *
 * @brief Class object for MovieTexture nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::movie_texture_class::
movie_texture_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::movie_texture_class::~movie_texture_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating MovieTexture
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by movie_texture_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::movie_texture_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                      field_value::sfbool_id,
                      "loop"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sffloat_id,
                      "speed"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sftime_id,
                      "startTime"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sftime_id,
                      "stopTime"),
        node_interface(node_interface::exposedfield_id,
                      field_value::mfstring_id,
                      "url"),
        node_interface(node_interface::field_id,
                      field_value::sfbool_id,
                      "repeatS"),
        node_interface(node_interface::field_id,
                      field_value::sfbool_id,
                      "repeatT"),
        node_interface(node_interface::eventout_id,
                      field_value::sftime_id,
                      "duration_changed"),
        node_interface(node_interface::eventout_id,
                      field_value::sfbool_id,
                      "isActive")
    };

    typedef vrml97_node_type_impl<movie_texture_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & movieTextureNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            movieTextureNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &movie_texture_node::loop_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &movie_texture_node::loop_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &movie_texture_node::loop_)));
        } else if (*interface == supportedInterfaces[1]) {
            movieTextureNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    movie_texture_node::set_speed_listener >(
                        &movie_texture_node::set_speed_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &movie_texture_node::speed_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sffloat_emitter>(
                        &movie_texture_node::speed_changed_)));
        } else if (*interface == supportedInterfaces[2]) {
            movieTextureNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sftime> >(
                        &movie_texture_node::start_time_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sftime> >(
                        &movie_texture_node::start_time_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sftime> >(
                        &movie_texture_node::start_time_)));
        } else if (*interface == supportedInterfaces[3]) {
            movieTextureNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sftime> >(
                        &movie_texture_node::stop_time_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sftime> >(
                        &movie_texture_node::stop_time_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sftime> >(
                        &movie_texture_node::stop_time_)));
        } else if (*interface == supportedInterfaces[4]) {
            movieTextureNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mfstring> >(&movie_texture_node::url_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfstring> >(
                        &movie_texture_node::url_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    exposedfield<mfstring> >(&movie_texture_node::url_)));
        } else if (*interface == supportedInterfaces[5]) {
            movieTextureNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &movie_texture_node::repeat_s_)));
        } else if (*interface == supportedInterfaces[6]) {
            movieTextureNodeType.add_field(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &movie_texture_node::repeat_t_)));
        } else if (*interface == supportedInterfaces[7]) {
            movieTextureNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &movie_texture_node::duration_changed_)));
        } else if (*interface == supportedInterfaces[8]) {
            movieTextureNodeType.add_eventout(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &movie_texture_node::is_active_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::movie_texture_node
 *
 * @brief MovieTexture node instances.
 */

/**
 * @var class openvrml::vrml97_node::movie_texture_node::movie_texture_class
 *
 * @brief Class object for MovieTexture nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::movie_texture_node::set_speed_listener
 *
 * @brief set_speed eventIn handler.
 *
 * Unlike most exposedFields which are implemented either with an instance of
 * the exposedfield template or a class derived from it, MovieTexture's speed
 * exposedField is implemented using the listener and emitter primitives. This
 * is because unlike most exposedFields, changes to the speed field (that is,
 * set_speed events) are ignored if the node is not active.
 */

/**
 * @brief Constructor.
 *
 * @param node  movie_texture_node.
 */
openvrml::vrml97_node::movie_texture_node::set_speed_listener::
set_speed_listener(movie_texture_node & node):
    sffloat_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::movie_texture_node::set_speed_listener::
~set_speed_listener() throw ()
{}

/**
 * @brief Process event.
 *
 * @param speed     speed value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::movie_texture_node::set_speed_listener::
do_process_event(const sffloat & speed, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        movie_texture_node & node =
            dynamic_cast<movie_texture_node &>(this->node());

        //
        // set_speed is ignored if the MovieTexture is active.
        //
        if (!node.active_.value) {
            node.speed_ = speed;
            node.node::modified(true);
            node::emit_event(node.speed_changed_, timestamp);
        }
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::movie_texture_node::loop_
 *
 * @brief loop exposedField.
 */

/**
 * @var openvrml::vrml97_node::movie_texture_node::set_speed_listener openvrml::vrml97_node::movie_texture_node::set_speed_
 *
 * @brief set_speed event handler for the speed exposedField.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::movie_texture_node::speed_
 *
 * @brief Value for speed exposedField.
 */

/**
 * @var openvrml::sffloat_emitter openvrml::vrml97_node::movie_texture_node::speed_changed_
 *
 * @brief speed_changed event emitter for the speed exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sftime> openvrml::vrml97_node::movie_texture_node::start_time_
 *
 * @brief startTime exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sftime> openvrml::vrml97_node::movie_texture_node::stop_time_
 *
 * @brief stopTime exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfstring> openvrml::vrml97_node::movie_texture_node::url_
 *
 * @brief url exposedField.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::movie_texture_node::duration_
 *
 * @brief duration value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::movie_texture_node::duration_changed_
 *
 * @brief duration_changed eventOut
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::movie_texture_node::active_
 *
 * @brief active state.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::movie_texture_node::is_active_
 *
 * @brief isActive eventOut.
 */

/**
 * @var openvrml::image openvrml::vrml97_node::movie_texture_node::image_
 *
 * @brief Frame data.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::movie_texture_node::
movie_texture_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_texture_node(type, scope),
    loop_(*this, false),
    set_speed_(*this),
    speed_(1.0),
    speed_changed_(this->speed_),
    start_time_(*this),
    stop_time_(*this),
    url_(*this),
    duration_changed_(this->duration_),
    is_active_(this->active_)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::movie_texture_node::~movie_texture_node() throw ()
{}

/**
 * @brief Cast to a movie_texture_node.
 *
 * @return a pointer to the movie_texture_node.
 */
openvrml::vrml97_node::movie_texture_node *
openvrml::vrml97_node::movie_texture_node::to_movie_texture() const
{ return (movie_texture_node*) this; }

/**
 * @brief Update the node for the current timestamp.
 *
 * @param time  the current time.
 */
void openvrml::vrml97_node::movie_texture_node::update(const double time)
{
# if 0
    if (this->modified()) {
        if (this->img_) {
            const char * imageUrl = this->img_->url();
            size_t imageLen = strlen(imageUrl);
            size_t i, nUrls = this->url_.mfstring::value.size();
            for (i = 0; i < nUrls; ++i) {
                size_t len = this->url_.mfstring::value[i].length();

                if (this->url_.mfstring::value[i] == imageUrl
                    || ((imageLen > len)
                        && (this->url_.mfstring::value[i]
                            == (imageUrl + imageLen - len)))) {
                    break;
                }
            }

            // if (d_image->url() not in d_url list) ...
            if (i == nUrls) {
                delete this->img_;
                this->img_ = 0;
            }
        }
    }

    // Load the movie if needed (should check startTime...)
    if (!this->img_ && !this->url_.mfstring::value.empty()) {
        doc2 baseDoc(this->scene()->url());
        this->img_ = new img;
        if (!this->img_->try_urls(this->url_.mfstring::value, &baseDoc)) {
            std::cerr << "Error: couldn't read MovieTexture from URL "
                      << this->url_ << std::endl;
        }

        const size_t nFrames = this->img_->nframes();
        this->duration_.value = (nFrames >= 0)
                              ? double(nFrames)
                              : -1.0;
        node::emit_event(this->duration_changed_, time);
        this->frame = int((this->speed_.value >= 0)
                          ? 0
                          : nFrames - 1);
        // Set the last frame equal to the start time.
        // This is needed to properly handle the case where the startTime
        // and stopTime are set at runtime to the same value (spec says
        // that a single loop should occur in this case...)
        this->lastFrameTime = this->start_time_.sftime::value;
    }

    // No pictures to show
    if (!this->img_ || this->img_->nframes() == 0) { return; }

    // See section 4.6.9 of the VRML97 spec for a detailed explanation
    // of the logic here.
    if (!this->active_.value) {
        if (time >= this->start_time_.sftime::value) {
            if (time >= this->stop_time_.sftime::value) {
                if (this->start_time_.sftime::value
                    >= this->stop_time_.sftime::value) {
                    if (this->loop_.sfbool::value) {
                        this->active_.value = true;
                        node::emit_event(this->is_active_, time);
                        this->lastFrameTime = time;
                        this->frame = int((this->speed_.value >= 0)
                                          ? 0
                                          : this->img_->nframes() - 1);
                        this->modified(true);
	            } else if (this->start_time_.sftime::value
                               > this->lastFrameTime) {
                        this->active_.value = true;
                        node::emit_event(this->is_active_, time);
                        this->lastFrameTime = time;
                        this->frame = int((this->speed_.value >= 0)
                                          ? 0
                                          : this->img_->nframes() - 1);
                        this->modified(true);
	            }
	        }
            } else if (this->stop_time_.sftime::value > time) {
                this->active_.value = true;
                node::emit_event(this->is_active_, time);
                this->lastFrameTime = time;
                this->frame = int((this->speed_.value >= 0)
                                  ? 0
                                  : this->img_->nframes() - 1);
                this->modified(true);
            }
        }
    }
    // Check whether stopTime has passed
    else if (this->active_.value
             && ((this->stop_time_.sftime::value
                  > this->start_time_.sftime::value
		  && this->stop_time_.sftime::value <= time))
             || ((this->frame < 0) && !this->loop_.sfbool::value)) {
        this->active_.value = false;
        node::emit_event(this->is_active_, time);
        this->modified(true);
    } else if (this->frame < 0 && this->loop_.sfbool::value) {
        // Reset frame to 0 to begin loop again.
        this->frame = 0;
    }

    // Check whether the frame should be advanced
    else if (this->active_.value
             && this->lastFrameTime + fabs(1 / this->speed_.value) <= time) {
        this->frame = (this->speed_.value >= 0)
            ? int(time - this->start_time_.sftime::value)
                % int(this->duration_.value / this->speed_.value)
            : -(int(time - this->start_time_.sftime::value)
                 % int(fabs(this->duration_.value / this->speed_.value)));
        assert(this->frame >= 0);

        this->lastFrameTime = time;
        this->modified(true);
    }

    const size_t frame_bytes =
        this->img_->w() * this->img_->h() * this->img_->nc();
    this->image_ =
        openvrml::image(this->img_->w(),
                        this->img_->h(),
                        this->img_->nc(),
                        this->img_->pixels(this->frame),
                        this->img_->pixels(this->frame) + frame_bytes);

    // Tell the scene when the next update is needed.
    if (this->active_.value) {
        double d = this->lastFrameTime + fabs(1 / this->speed_.value) - time;
        this->type().node_class().browser().delta(0.9 * d);
    }
# endif
}

/**
 * @brief The image.
 *
 * @return the image.
 */
const openvrml::image &
openvrml::vrml97_node::movie_texture_node::image() const throw ()
{
    return this->image_;
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::movie_texture_node::
do_initialize(const double timestamp)
    throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser().add_movie(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void
openvrml::vrml97_node::movie_texture_node::do_shutdown(const double timestamp)
    throw ()
{
    assert(this->scene());
    this->scene()->browser().remove_movie(*this);
}

/**
 * @brief render_texture implementation.
 *
 * @param v viewer.
 *
 * @return object identifier for the inserted texture.
 */
openvrml::viewer::texture_object_t
openvrml::vrml97_node::movie_texture_node::do_render_texture(viewer & v)
{
# if 0
    if (!this->img_ || this->frame < 0) { return 0; }

    viewer::texture_object_t texture_object = 0;

    if (!this->img_->pixels(this->frame)) {
        this->frame = -1;
    } else {
        texture_object = v.insert_texture(this->image_,
                                          this->repeat_s_.value,
                                          this->repeat_t_.value,
                                          !this->active_.value);
    }

    this->lastFrame = this->frame;
    return texture_object;
# endif
    return 0;
}


/**
 * @class openvrml::vrml97_node::navigation_info_class
 *
 * @brief Class object for NavigationInfo nodes.
 */

/**
 * @typedef openvrml::vrml97_node::navigation_info_class::bound_nodes_t
 *
 * @brief A bound NavigationInfo node stack.
 */

/**
 * @var openvrml::vrml97_node::navigation_info_node * openvrml::vrml97_node::navigation_info_class::first
 *
 * @brief The first NavigationInfo node in the initial scene graph.
 */

/**
 * @var openvrml::vrml97_node::navigation_info_class::bound_nodes_t openvrml::vrml97_node::navigation_info_class::bound_nodes
 *
 * @brief The bound NavigationInfo node stack.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::navigation_info_class::
navigation_info_class(openvrml::browser & browser):
    node_class(browser),
    first(0)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::navigation_info_class::~navigation_info_class() throw ()
{}

/**
 * @brief Set the first NavigationInfo node in the world.
 *
 * The first NavigationInfo node in the world is used as the initially active
 * NavigationInfo. This method is used by navigation_info_node::do_initialize.
 *
 * @param nav_info    a Background node.
 */
void
openvrml::vrml97_node::navigation_info_class::
set_first(navigation_info_node & nav_info) throw ()
{
    this->first = &nav_info;
}

/**
 * @brief Reset the pointer to the first NavigationInfo node in the world to
 *        null.
 *
 * This function is called by <code>navigation_info_node::do_shutdown</code>.
 */
void openvrml::vrml97_node::navigation_info_class::reset_first() throw ()
{
    this->first = 0;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by navigation_info_node::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false otherwise.
 */
bool openvrml::vrml97_node::navigation_info_class::has_first() const throw ()
{
    return this->first;
}

/**
 * @brief Check to see if a node is registered as the "first" node.
 *
 * @param nav_info  a navigation_info_node.
 *
 * @return @c true if @p nav_info is the fist node; @c false otherwise.
 */
bool
openvrml::vrml97_node::navigation_info_class::
is_first(navigation_info_node & nav_info) throw ()
{
    return &nav_info == this->first;
}

/**
 * @brief Push a navigation_info_node on the top of the bound node stack.
 *
 * @param nav_info    the node to bind.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::navigation_info_class::
bind(navigation_info_node & nav_info, const double timestamp)
    throw (std::bad_alloc)
{
    using std::find;

    //
    // If the node is already the active node, do nothing.
    //
    if (!this->bound_nodes.empty() && &nav_info == this->bound_nodes.back())
    {
        return;
    }

    //
    // If the node is already on the stack, remove it.
    //
    const bound_nodes_t::iterator pos =
        find(this->bound_nodes.begin(), this->bound_nodes.end(), &nav_info);
    if (pos != this->bound_nodes.end()) { this->bound_nodes.erase(pos); }

    //
    // Send FALSE from the currently active node's isBound.
    //
    if (!this->bound_nodes.empty()) {
        navigation_info_node & current =
            dynamic_cast<navigation_info_node &>(*this->bound_nodes.back());
        current.is_bound_.value = false;
        node::emit_event(current.is_bound_emitter_, timestamp);
    }

    //
    // Push the node to the top of the stack, and have it send isBound TRUE.
    //
    this->bound_nodes.push_back(&nav_info);
    nav_info.is_bound_.value = true;
    node::emit_event(nav_info.is_bound_emitter_, timestamp);

    this->browser().active_navigation_info(nav_info);
}

/**
 * @brief Remove a navigation_info_node from the bound node stack.
 *
 * @param nav_info    the node to unbind.
 * @param timestamp     the current time.
 */
void
openvrml::vrml97_node::navigation_info_class::
unbind(navigation_info_node & nav_info, const double timestamp)
    throw ()
{
    using std::find;

    const bound_nodes_t::iterator pos =
        find(this->bound_nodes.begin(), this->bound_nodes.end(), &nav_info);
    if (pos != this->bound_nodes.end()) {
        nav_info.is_bound_.value = false;
        node::emit_event(nav_info.is_bound_emitter_, timestamp);

        if (pos == this->bound_nodes.end() - 1
            && this->bound_nodes.size() > 1) {
            navigation_info_node & newActive =
                dynamic_cast<navigation_info_node &>(
                    **(this->bound_nodes.end() - 2));
            newActive.is_bound_.value = true;
            node::emit_event(newActive.is_bound_emitter_, timestamp);

            this->browser().active_navigation_info(nav_info);
        } else {
            this->browser().reset_default_navigation_info();
        }
        this->bound_nodes.erase(pos);
    }
}

/**
 * @brief node_class-specific initialization.
 *
 * @param initial_viewpoint the viewpoint_node that should be bound initially.
 * @param timestamp         the current time.
 */
void
openvrml::vrml97_node::navigation_info_class::
do_initialize(openvrml::viewpoint_node * initial_viewpoint,
              const double timestamp)
    throw ()
{
    if (this->first) {
        try {
            event_listener & listener =
                this->first->event_listener("set_bind");
            assert(dynamic_cast<sfbool_listener *>(&listener));
            static_cast<sfbool_listener &>(listener)
                .process_event(sfbool(true), timestamp);
        } catch (unsupported_interface & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating NavigationInfo
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by navigation_info_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::navigation_info_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::sfbool_id,
                       "set_bind"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "avatarSize"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "headlight"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "speed"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id, "type"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "visibilityLimit"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isBound")
    };

    typedef vrml97_node_type_impl<navigation_info_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & navigationInfoNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            navigationInfoNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    navigation_info_node::set_bind_listener>(
                        &navigation_info_node::set_bind_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            navigationInfoNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mffloat> >(
                        &navigation_info_node::avatar_size_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mffloat> >(
                        &navigation_info_node::avatar_size_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mffloat> >(
                        &navigation_info_node::avatar_size_)));
        } else if (*interface == supportedInterfaces[2]) {
            navigationInfoNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &navigation_info_node::headlight_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &navigation_info_node::headlight_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &navigation_info_node::headlight_)));
        } else if (*interface == supportedInterfaces[3]) {
            navigationInfoNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(&navigation_info_node::speed_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &navigation_info_node::speed_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &navigation_info_node::speed_)));
        } else if (*interface == supportedInterfaces[4]) {
            navigationInfoNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mfstring> >(&navigation_info_node::type_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfstring> >(
                        &navigation_info_node::type_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    exposedfield<mfstring> >(&navigation_info_node::type_)));
        } else if (*interface == supportedInterfaces[5]) {
            navigationInfoNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(
                        &navigation_info_node::visibility_limit_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &navigation_info_node::visibility_limit_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &navigation_info_node::visibility_limit_)));
        } else if (*interface == supportedInterfaces[6]) {
            navigationInfoNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &navigation_info_node::is_bound_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::navigation_info_node
 *
 * @brief NavigationInfo node instances.
 */

/**
 * @var class openvrml::vrml97_node::navigation_info_node::navigation_info_class
 *
 * @brief Class object for NavigationInfo nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::navigation_info_node::set_bind_listener
 *
 * @brief set_bind eventIn handler.
 */

/**
 * @brief Construct.
 *
 * @param node  navigation_info_node.
 */
openvrml::vrml97_node::navigation_info_node::set_bind_listener::
set_bind_listener(navigation_info_node & node):
    sfbool_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::navigation_info_node::set_bind_listener::
~set_bind_listener() throw ()
{}

/**
 * @brief Process event.
 *
 * @param bind      bind state.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::navigation_info_node::set_bind_listener::
do_process_event(const sfbool & bind,
                 const double timestamp)
    throw (std::bad_alloc)
{
    try {
        navigation_info_node & node =
            dynamic_cast<navigation_info_node &>(this->node());
        navigation_info_class & node_class =
            const_cast<navigation_info_class &>(
                static_cast<const navigation_info_class &>(
                    this->node().type().node_class()));
        if (bind.value) {
            node_class.bind(node, timestamp);
        } else {
            node_class.unbind(node, timestamp);
        }
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::navigation_info_node::set_bind_listener openvrml::vrml97_node::navigation_info_node::set_bind_listener_
 *
 * @brief set_bind eventIn handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::mffloat> openvrml::vrml97_node::navigation_info_node::avatar_size_
 *
 * @brief avatarSize exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::navigation_info_node::headlight_
 *
 * @brief headlight exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::navigation_info_node::speed_
 *
 * @brief speed exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfstring> openvrml::vrml97_node::navigation_info_node::type_
 *
 * @brief type exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::navigation_info_node::visibility_limit_
 *
 * @brief visibilityLimit exposedField.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::navigation_info_node::is_bound_
 *
 * @brief isBound eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::navigation_info_node::is_bound_emitter_
 *
 * @brief isBound eventOut emitter.
 */

namespace {
    const float navigation_avatar_size_[] = { 0.25f, 1.6f, 0.75f };
    const std::string navigation_type_[] = { "WALK", "ANY" };
}

/**
 * @brief Construct.
 *
 * @param t     the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::navigation_info_node::
navigation_info_node(const node_type & t,
                     const boost::shared_ptr<openvrml::scope> & scope):
    node(t, scope),
    abstract_base(t, scope),
    bounded_volume_node(t, scope),
    child_node(t, scope),
    openvrml::navigation_info_node(t, scope),
    set_bind_listener_(*this),
    avatar_size_(*this, std::vector<float>(navigation_avatar_size_,
                                           navigation_avatar_size_ + 3)),
    headlight_(*this, true),
    speed_(*this, 1.0),
    type_(*this, std::vector<std::string>(navigation_type_,
                                          navigation_type_ + 2)),
    visibility_limit_(*this, 0.0),
    is_bound_emitter_(this->is_bound_)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::navigation_info_node::~navigation_info_node() throw ()
{}

/**
 * @brief The avatar size.
 *
 * @return the avatar size.
 */
const std::vector<float> &
openvrml::vrml97_node::navigation_info_node::avatar_size() const throw ()
{
    return this->avatar_size_.mffloat::value;
}

/**
 * @brief The headlight state.
 *
 * @return @c true if the headlight is on; @c false otherwise.
 */
bool openvrml::vrml97_node::navigation_info_node::headlight() const throw ()
{
    return this->headlight_.sfbool::value;
}

/**
 * @brief The speed.
 *
 * @return the speed.
 */
float openvrml::vrml97_node::navigation_info_node::speed() const throw ()
{
    return this->speed_.sffloat::value;
}

/**
 * @brief The navigation type.
 *
 * @return the navigation type.
 */
const std::vector<std::string> &
openvrml::vrml97_node::navigation_info_node::type() const throw ()
{
    return this->type_.mfstring::value;
}

/**
 * @brief The visibility limit.
 *
 * @return the visibility limit.
 */
float openvrml::vrml97_node::navigation_info_node::visibility_limit() const
    throw ()
{
    return this->visibility_limit_.sffloat::value;
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void
openvrml::vrml97_node::navigation_info_node::
do_initialize(const double timestamp) throw ()
{
    using boost::polymorphic_downcast;

    navigation_info_class & node_class =
        const_cast<navigation_info_class &>(
            *polymorphic_downcast<const navigation_info_class *>(
                &this->node::type().node_class()));
    if (!node_class.has_first()) { node_class.set_first(*this); }
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void
openvrml::vrml97_node::navigation_info_node::
do_shutdown(const double timestamp) throw ()
{
    using boost::polymorphic_downcast;

    navigation_info_class & node_class =
        const_cast<navigation_info_class &>(
            *polymorphic_downcast<const navigation_info_class *>(
                &this->node::type().node_class()));
    node_class.unbind(*this, timestamp);

    if (node_class.is_first(*this)) { node_class.reset_first(); }
}


/**
 * @class openvrml::vrml97_node::normal_class
 *
 * @brief Class object for Normal nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml::vrml97_node::normal_class::normal_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::normal_class::~normal_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Normal nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by normal_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::normal_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3f_id,
                           "vector");

    typedef vrml97_node_type_impl<normal_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & normalNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterface) {
            normalNodeType.add_exposedfield(
                supportedInterface.field_type,
                supportedInterface.id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mfvec3f> >(&normal_node::vector_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfvec3f> >(
                        &normal_node::vector_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mfvec3f> >(
                        &normal_node::vector_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::normal_node
 *
 * @brief Normal node instances.
 */

/**
 * @var openvrml::vrml97_node::normal_node::normal_class
 *
 * @brief Class object for Normal nodes.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfvec3f> openvrml::vrml97_node::normal_node::vector_
 *
 * @brief vector exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::normal_node::
normal_node(const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::normal_node(type, scope),
    vector_(*this)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::normal_node::~normal_node() throw ()
{}

/**
 * @brief Get the array of normal vectors.
 *
 * @return the array of normal vectors.
 */
const std::vector<openvrml::vec3f> &
openvrml::vrml97_node::normal_node::vector() const throw ()
{
    return this->vector_.mfvec3f::value;
}


/**
 * @class openvrml::vrml97_node::normal_interpolator_class
 *
 * @brief Class object for NormalInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::normal_interpolator_class::
normal_interpolator_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::normal_interpolator_class::~normal_interpolator_class()
    throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating
 *         NormalInterpolator nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by normal_interpolator_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::normal_interpolator_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "key"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfvec3f_id,
                       "keyValue"),
        node_interface(node_interface::eventout_id,
                       field_value::mfvec3f_id,
                       "value_changed")
    };

    typedef vrml97_node_type_impl<normal_interpolator_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & normalInterpolatorNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            normalInterpolatorNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    normal_interpolator_node::set_fraction_listener>(
                        &normal_interpolator_node::set_fraction_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            normalInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mffloat> >(&normal_interpolator_node::key_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mffloat> >(
                        &normal_interpolator_node::key_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mffloat> >(
                        &normal_interpolator_node::key_)));
        } else if (*interface == supportedInterfaces[2]) {
            normalInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mfvec3f> >(
                        &normal_interpolator_node::key_value_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfvec3f> >(
                        &normal_interpolator_node::key_value_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mfvec3f> >(
                        &normal_interpolator_node::key_value_)));
        } else if (*interface == supportedInterfaces[3]) {
            normalInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<mfvec3f_emitter>(
                        &normal_interpolator_node::value_changed_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::normal_interpolator_node
 *
 * @brief NormalInterpolator node instances.
 */

/**
 * @var class openvrml::vrml97_node::normal_interpolator_node::normal_interpolator_class
 *
 * @brief Class object for NormalInterpolator nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::normal_interpolator_node::set_fraction_listener
 *
 * @brief set_fraction eventIn handler.
 */

/**
 * @brief Construct.
 *
 * @param node  normal_interpolator_node.
 */
openvrml::vrml97_node::normal_interpolator_node::set_fraction_listener::
set_fraction_listener(normal_interpolator_node & node):
    sffloat_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::normal_interpolator_node::set_fraction_listener::
~set_fraction_listener()
    throw ()
{}

/**
 * @brief Process event.
 *
 * @param fraction  fraction.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::normal_interpolator_node::set_fraction_listener::
do_process_event(const sffloat & fraction, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        using std::vector;

        normal_interpolator_node & node =
            dynamic_cast<normal_interpolator_node &>(this->node());

        const vector<float> & key = node.key_.mffloat::value;
        const vector<vec3f> & key_value = node.key_value_.mfvec3f::value;
        vector<vec3f> & value = node.value_changed_.mfvec3f::value;

        size_t nNormals = key_value.size() / key.size();
        size_t n = key.size() - 1;

        if (fraction.value < key[0]) {
            value.assign(key_value.begin(), key_value.begin() + nNormals);
        } else if (fraction.value > key[n]) {
            value.assign(key_value.begin() + n * nNormals,
                         key_value.begin() + (n + 1) * nNormals);
        } else {
            // Reserve enough space for the new value
            value.resize(nNormals);

            for (size_t i = 0; i < n; ++i) {
                if (key[i] <= fraction.value && fraction.value <= key[i + 1]) {

                    vector<vec3f>::const_iterator v1 =
                        key_value.begin() + i * nNormals;
                    vector<vec3f>::const_iterator v2 =
                        key_value.begin() + (i + 1) * nNormals;

                    const float f =
                        (fraction.value - key[i]) / (key[i + 1] - key[i]);

                    // Interpolate on the surface of unit sphere.
                    for (size_t j = 0; j < nNormals; ++j) {
                        using openvrml_::fequal;

                        float alpha, beta;
                        const float dot_product = v1->dot(*v2);
                        if (!fequal<float>()(dot_product, 1.0f)
                            && v1->normalize() != v2->normalize()) {
                            // Vectors are not opposite and not coincident.
                            const float omega = float(acos(dot_product));
                            const float sinomega = float(sin(omega));
                            alpha = float(sin((1.0 - f) * omega)) / sinomega;
                            beta = float(sin(f * omega)) / sinomega;
                        } else {
                            // Do linear interpolation.
                            alpha = 1.0f - f;
                            beta = f;
                        }
                        value[j] = (alpha * *v1) + (beta * *v2);

                        ++v1;
                        ++v2;
                    }
                    break;
                }
            }
        }

        // Send the new value
        node::emit_event(node.value_changed_emitter_, timestamp);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::normal_interpolator_node::set_fraction_listener openvrml::vrml97_node::normal_interpolator_node::set_fraction_listener_
 *
 * @brief set_fraction eventIn handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::mffloat> openvrml::vrml97_node::normal_interpolator_node::key_
 *
 * @brief key exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfvec3f> openvrml::vrml97_node::normal_interpolator_node::key_value_
 *
 * @brief keyValue exposedField.
 */

/**
 * @var openvrml::mfvec3f openvrml::vrml97_node::normal_interpolator_node::value_changed_
 *
 * @brief value_changed eventOut value.
 */

/**
 * @var openvrml::mfvec3f_emitter openvrml::vrml97_node::normal_interpolator_node::value_changed_emitter_
 *
 * @brief value_changed eventOut emitter.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::normal_interpolator_node::
normal_interpolator_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    set_fraction_listener_(*this),
    key_(*this),
    key_value_(*this),
    value_changed_emitter_(this->value_changed_)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::normal_interpolator_node::~normal_interpolator_node()
    throw ()
{}


/**
 * @class openvrml::vrml97_node::orientation_interpolator_class
 *
 * @brief Class object for OrientationInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::orientation_interpolator_class::
orientation_interpolator_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::orientation_interpolator_class::
~orientation_interpolator_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating
 *      OrientationInterpolator nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by orientation_interpolator_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::orientation_interpolator_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                      field_value::sffloat_id,
                      "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                      field_value::mffloat_id,
                      "key"),
        node_interface(node_interface::exposedfield_id,
                      field_value::mfrotation_id,
                      "keyValue"),
        node_interface(node_interface::eventout_id,
                      field_value::sfrotation_id,
                      "value_changed")
    };

    typedef vrml97_node_type_impl<orientation_interpolator_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & orientationInterpolatorNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            orientationInterpolatorNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    orientation_interpolator_node::set_fraction_listener>(
                        &orientation_interpolator_node::set_fraction_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            orientationInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mffloat> >(
                        &orientation_interpolator_node::key_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mffloat> >(
                        &orientation_interpolator_node::key_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mffloat> >(
                        &orientation_interpolator_node::key_)));
        } else if (*interface == supportedInterfaces[2]) {
            orientationInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mfrotation> >(
                        &orientation_interpolator_node::key_value_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfrotation> >(
                        &orientation_interpolator_node::key_value_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    exposedfield<mfrotation> >(
                        &orientation_interpolator_node::key_value_)));
        } else if (*interface == supportedInterfaces[3]) {
            orientationInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfrotation_emitter>(
                        &orientation_interpolator_node::value_changed_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::orientation_interpolator_node
 *
 * @brief OrientationInterpolator node instances.
 */

/**
 * @var class openvrml::vrml97_node::orientation_interpolator_node::orientation_interpolator_class
 *
 * @brief Class object for OrientationInterpolator nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::orientation_interpolator_node::set_fraction_listener
 *
 * @brief set_fraction eventIn handler.
 */

/**
 * @brief Construct.
 *
 * @param node  orientation_interpolator_node.
 */
openvrml::vrml97_node::orientation_interpolator_node::set_fraction_listener::
set_fraction_listener(orientation_interpolator_node & node):
    sffloat_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::orientation_interpolator_node::set_fraction_listener::
~set_fraction_listener() throw ()
{}

/**
 * @brief Process event.
 *
 * @param fraction  fraction.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::orientation_interpolator_node::set_fraction_listener::
do_process_event(const sffloat & fraction, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        using std::vector;

        orientation_interpolator_node & node =
            dynamic_cast<orientation_interpolator_node &>(this->node());

        const vector<float> & key = node.key_.mffloat::value;
        const vector<rotation> & key_value = node.key_value_.mfrotation::value;

        size_t n = key.size() - 1;
        if (fraction.value < key[0]) {
            node.value_changed_.value = key_value[0];
        } else if (fraction.value > key[n]) {
            node.value_changed_.value = key_value[n];
        } else {
            for (size_t i = 0; i < n; ++i) {
                if (key[i] <= fraction.value && fraction.value <= key[i + 1]) {
                    using openvrml_::pi;

                    const rotation & v1 = key_value[i];
                    const rotation & v2 = key_value[i + 1];

                    // Interpolation factor
                    const float f =
                        (fraction.value - key[i]) / (key[i + 1] - key[i]);

                    float x, y, z, r1, r2;
                    r1 = v1[3];

                    // Make sure the vectors are not pointing opposite ways
                    if (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2] < 0.0) {
                        x = v1[0] + f * (-v2[0] - v1[0]);
                        y = v1[1] + f * (-v2[1] - v1[1]);
                        z = v1[2] + f * (-v2[2] - v1[2]);
                        r2 = -v2[3];
                    } else {
                        x = v1[0] + f * (v2[0] - v1[0]);
                        y = v1[1] + f * (v2[1] - v1[1]);
                        z = v1[2] + f * (v2[2] - v1[2]);
                        r2 = v2[3];
                    }

                    // Interpolate angles via the shortest direction
                    if (fabs(r2 - r1) > pi) {
                        if (r2 > r1) {
                            r1 += float(2.0 * pi);
                        } else {
                            r2 += float(2.0 * pi);
                        }
                    }
                    float angle = r1 + f * (r2 - r1);
                    if (angle >= 2.0 * pi) {
                        angle -= float(2.0 * pi);
                    } else if (angle < 0.0) {
                        angle += float(2.0 * pi);
                    }
                    node.value_changed_.value.axis(vec3f(x, y, z).normalize());
                    node.value_changed_.value.angle(angle);
                    break;
                }
            }
        }

        // Send the new value
        node::emit_event(node.value_changed_emitter_, timestamp);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::orientation_interpolator_node::set_fraction_listener openvrml::vrml97_node::orientation_interpolator_node::set_fraction_listener_
 *
 * @brief set_fraction eventIn handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::mffloat> openvrml::vrml97_node::orientation_interpolator_node::key_
 *
 * @brief key exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfrotation> openvrml::vrml97_node::orientation_interpolator_node::key_value_
 *
 * @brief keyValue exposedField.
 */

/**
 * @var openvrml::sfrotation openvrml::vrml97_node::orientation_interpolator_node::value_changed_
 *
 * @brief value_changed eventOut value.
 */

/**
 * @var openvrml::sfrotation_emitter openvrml::vrml97_node::orientation_interpolator_node::value_changed_emitter_
 *
 * @brief value_changed eventOut emitter.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::orientation_interpolator_node::
orientation_interpolator_node(
    const node_type & type,
    const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    set_fraction_listener_(*this),
    key_(*this),
    key_value_(*this),
    value_changed_emitter_(this->value_changed_)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::orientation_interpolator_node::
~orientation_interpolator_node() throw ()
{}


/**
 * @class openvrml::vrml97_node::pixel_texture_class
 *
 * @brief Class object for PixelTexture nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::pixel_texture_class::
pixel_texture_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::pixel_texture_class::~pixel_texture_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating PixelTexture
 *         nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by pixel_texture_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::pixel_texture_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                      field_value::sfimage_id,
                      "image"),
        node_interface(node_interface::field_id,
                      field_value::sfbool_id,
                      "repeatS"),
        node_interface(node_interface::field_id,
                      field_value::sfbool_id,
                      "repeatT")
    };

    typedef vrml97_node_type_impl<pixel_texture_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & pixelTextureNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end(); ++interface) {
        if (*interface == supportedInterfaces[0]) {
            pixelTextureNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfimage> >(
                        &pixel_texture_node::image_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfimage> >(
                        &pixel_texture_node::image_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfimage> >(
                        &pixel_texture_node::image_)));
        } else if (*interface == supportedInterfaces[1]) {
            pixelTextureNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &pixel_texture_node::repeat_s_)));
        } else if (*interface == supportedInterfaces[2]) {
            pixelTextureNodeType.add_field(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &pixel_texture_node::repeat_t_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::pixel_texture_node
 *
 * @brief Represents PixelTexture node instances.
 */

/**
 * @var openvrml::vrml97_node::pixel_texture_node::pixel_texture_class
 *
 * @brief Class object for PixelTexture nodes.
 */

/**
 * @var openvrml::sfimage openvrml::vrml97_node::pixel_texture_node::image_
 *
 * @brief image exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::pixel_texture_node::
pixel_texture_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_texture_node(type, scope),
    image_(*this)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::pixel_texture_node::~pixel_texture_node() throw ()
{
    // viewer.remove_texture_object(this->texObject); ...
}

/**
 * @brief The image.
 *
 * @return the image.
 */
const openvrml::image &
openvrml::vrml97_node::pixel_texture_node::image() const throw ()
{
    return this->image_.sfimage::value;
}

/**
 * @brief render_texture implementation.
 *
 * @param v viewer.
 *
 * @return object identifier for the inserted texture.
 */
openvrml::viewer::texture_object_t
openvrml::vrml97_node::pixel_texture_node::do_render_texture(viewer & v)
{
    return v.insert_texture(this->image_.sfimage::value,
                            this->repeat_s_.value,
                            this->repeat_t_.value,
                            true);
}


/**
 * @class openvrml::vrml97_node::plane_sensor_class
 *
 * @brief Class object for PlaneSensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node class object.
 */
openvrml::vrml97_node::plane_sensor_class::
plane_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::plane_sensor_class::~plane_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating PlaneSensor
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by plane_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::plane_sensor_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "autoOffset"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "enabled"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec2f_id,
                       "maxPosition"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec2f_id,
                       "minPosition"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "offset"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "trackPoint_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "translation_changed")
    };

    typedef vrml97_node_type_impl<plane_sensor_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & planeSensorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            planeSensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &plane_sensor_node::auto_offset_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &plane_sensor_node::auto_offset_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &plane_sensor_node::auto_offset_)));
        } else if (*interface == supportedInterfaces[1]) {
            planeSensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &plane_sensor_node::enabled_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &plane_sensor_node::enabled_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &plane_sensor_node::enabled_)));
        } else if (*interface == supportedInterfaces[2]) {
            planeSensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec2f> >(
                        &plane_sensor_node::max_position_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec2f> >(
                        &plane_sensor_node::max_position_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec2f> >(
                        &plane_sensor_node::max_position_)));
        } else if (*interface == supportedInterfaces[3]) {
            planeSensorNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec2f> >(
                        &plane_sensor_node::min_position_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec2f> >(
                        &plane_sensor_node::min_position_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec2f> >(
                        &plane_sensor_node::min_position_)));
        } else if (*interface == supportedInterfaces[4]) {
            planeSensorNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(
                        &plane_sensor_node::offset_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &plane_sensor_node::offset_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &plane_sensor_node::offset_)));
        } else if (*interface == supportedInterfaces[5]) {
            planeSensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &plane_sensor_node::is_active_emitter_)));
        } else if (*interface == supportedInterfaces[6]) {
            planeSensorNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfvec3f_emitter>(
                        &plane_sensor_node::track_point_changed_emitter_)));
        } else if (*interface == supportedInterfaces[7]) {
            planeSensorNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfvec3f_emitter>(
                        &plane_sensor_node::translation_changed_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::plane_sensor_node
 *
 * The PlaneSensor node maps pointing device motion into two-dimensional
 * translation in a plane parallel to the <var>z</var>&nbsp;=&nbsp;0 plane of
 * the local coordinate system. The PlaneSensor node uses the descendent
 * geometry of its parent node to determine whether it is liable to generate
 * events.
 */

/**
 * @var openvrml::vrml97_node::plane_sensor_node::plane_sensor_class
 *
 * @brief Class object for PlaneSensor instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::plane_sensor_node::auto_offset_
 *
 * @brief autoOffset exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::plane_sensor_node::enabled_
 *
 * @brief enabled exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec2f> openvrml::vrml97_node::plane_sensor_node::max_position_
 *
 * @brief maxPosition exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec2f> openvrml::vrml97_node::plane_sensor_node::min_position_
 *
 * @brief minPosition exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::plane_sensor_node::offset_
 *
 * @brief offset exposedField.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::plane_sensor_node::is_active_
 *
 * @brief isActive eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::plane_sensor_node::is_active_emitter_
 *
 * @brief isActive eventOut emitter.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::plane_sensor_node::track_point_changed_
 *
 * @brief trackPoint_changed eventOut value.
 */

/**
 * @var openvrml::sfvec3f_emitter openvrml::vrml97_node::plane_sensor_node::track_point_changed_emitter_
 *
 * @brief trackPoint_changed eventOut emitter.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::plane_sensor_node::translation_changed_
 *
 * @brief translation_changed eventOut value.
 */

/**
 * @var openvrml::sfvec3f_emitter openvrml::vrml97_node::plane_sensor_node::translation_changed_emitter_
 *
 * @brief translation_changed eventOut emitter.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::plane_sensor_node::activationPoint
 *
 * @brief The point at which the PlaneSensor was activated.
 */

/**
 * @var openvrml::mat4f openvrml::vrml97_node::plane_sensor_node::activationMatrix
 *
 * @brief Activation matrix.
 */

/**
 * @var openvrml::mat4f openvrml::vrml97_node::plane_sensor_node::modelview
 *
 * @brief The modelview matrix.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::plane_sensor_node::
plane_sensor_node(const node_type & type,
                  const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    auto_offset_(*this, true),
    enabled_(*this, true),
    max_position_(*this, vec2f(-1.0, -1.0)),
    min_position_(*this, vec2f(0.0, 0.0)),
    offset_(*this, vec3f(0.0, 0.0, 0.0)),
    is_active_(false),
    is_active_emitter_(this->is_active_),
    track_point_changed_emitter_(this->track_point_changed_),
    translation_changed_emitter_(this->translation_changed_)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::plane_sensor_node::~plane_sensor_node() throw ()
{}

/**
 * @brief Cast to a plane_sensor_node.
 *
 * @return a pointer to the plane_sensor_node.
 */
openvrml::vrml97_node::plane_sensor_node *
openvrml::vrml97_node::plane_sensor_node::to_plane_sensor() const
{
    return (plane_sensor_node*) this;
}

/**
 * @brief Render the node.
 *
 * Render a frame if there is one available.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void
openvrml::vrml97_node::plane_sensor_node::
do_render_child(openvrml::viewer & viewer, const rendering_context context)
{
    //
    // Store the ModelView matrix which is calculated at the time of rendering
    // in render-context. This matrix will be in use at the time of activation.
    //
    this->modelview = context.matrix();
}

/**
 * @todo The local coords are computed for one instance; do we need to convert
 *       @p p to local coords for each instance (USE) of the sensor?
 */
void openvrml::vrml97_node::plane_sensor_node::activate(double timestamp,
                                                        bool isActive,
                                                        double * p)
{
    // Become active
    if (isActive && !this->is_active_.value) {
        this->is_active_.value = isActive;

        vec3f V(static_cast<float>(p[0]),
                static_cast<float>(p[1]),
                static_cast<float>(p[2]));
        this->activationMatrix = this->modelview.inverse();
        V *= this->activationMatrix;
        this->activationPoint.value = V;
        node::emit_event(this->is_active_emitter_, timestamp);
    }

    // Become inactive
    else if (!isActive && this->is_active_.value) {
        this->is_active_.value = isActive;
        node::emit_event(this->is_active_emitter_, timestamp);

        // auto offset
        if (this->auto_offset_.sfbool::value) {
            this->offset_.sfvec3f::value = this->translation_changed_.value;
            node::emit_event(this->offset_, timestamp);
        }
    }

    // Tracking
    else if (isActive) {
        vec3f V(static_cast<float>(p[0]),
                static_cast<float>(p[1]),
                static_cast<float>(p[2]));
        V *= this->activationMatrix;
        this->track_point_changed_.value = V;
        node::emit_event(this->track_point_changed_emitter_, timestamp);

        vec3f t(V[0] - this->activationPoint.value.x()
                + this->offset_.sfvec3f::value.x(),
                V[1] - this->activationPoint.value.y()
                + this->offset_.sfvec3f::value.y(),
                0.0);

        const vec2f & min_pos = this->min_position_.sfvec2f::value;
        const vec2f & max_pos = this->max_position_.sfvec2f::value;

        if (min_pos.x() == max_pos.x()) {
            t.x(min_pos.x());
        } else if (min_pos.x() < max_pos.x()) {
            if (t.x() < min_pos.x()) {
                t.x(min_pos.x());
            } else if (t.x() > max_pos.x()) {
                t.x(max_pos.x());
            }
        }

        if (min_pos.y() == max_pos.y()) {
            t.y(min_pos.y());
        } else if (min_pos.y() < max_pos.y()) {
            if (t.y() < min_pos.y()) {
                t.y(min_pos.y());
            } else if (t.y() > max_pos.y()) {
                t.y(max_pos.y());
            }
        }

        this->translation_changed_.value = t;
        node::emit_event(this->translation_changed_emitter_, timestamp);
    }
}

/**
 * @brief Return whether the PlaneSensor is enabled.
 *
 * @return @c true if the PlaneSensor is enabled, @c false otherwise.
 */
bool openvrml::vrml97_node::plane_sensor_node::enabled() const
{
    return this->enabled_.sfbool::value;
}


/**
 * @class openvrml::vrml97_node::point_light_class
 *
 * @brief Class object for PointLight nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this class object.
 */
openvrml::vrml97_node::point_light_class::
point_light_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::point_light_class::~point_light_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating PointLight nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by point_light_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::point_light_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "ambientIntensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "attenuation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "intensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "location"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "on"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "radius")
    };

    typedef vrml97_node_type_impl<point_light_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & pointLightNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(
                        &point_light_node::ambient_intensity_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &point_light_node::ambient_intensity_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &point_light_node::ambient_intensity_)));
        } else if (*interface == supportedInterfaces[1]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(
                        &point_light_node::attenuation_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &point_light_node::attenuation_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &point_light_node::attenuation_)));
        } else if (*interface == supportedInterfaces[2]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfcolor> >(
                        &point_light_node::color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfcolor> >(
                        &point_light_node::color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfcolor> >(
                        &point_light_node::color_)));
        } else if (*interface == supportedInterfaces[3]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(
                        &point_light_node::intensity_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &point_light_node::intensity_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &point_light_node::intensity_)));
        } else if (*interface == supportedInterfaces[4]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(
                        &point_light_node::location_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &point_light_node::location_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &point_light_node::location_)));
        } else if (*interface == supportedInterfaces[5]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &point_light_node::on_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &point_light_node::on_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &point_light_node::on_)));
        } else if (*interface == supportedInterfaces[6]) {
            pointLightNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(
                        &point_light_node::radius_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &point_light_node::radius_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &point_light_node::radius_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::point_light_node
 *
 * @brief PointLight node instances.
 */

/**
 * @var openvrml::vrml97_node::point_light_node::point_light_class
 *
 * @brief Class object for PointLight instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::point_light_node::attenuation_
 *
 * @brief attenuation exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::point_light_node::location_
 *
 * @brief location exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::point_light_node::radius_
 *
 * @brief radius exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::point_light_node::
point_light_node(const node_type & type,
                 const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    abstract_light_node(type, scope),
    attenuation_(*this, vec3f(1.0, 0.0, 0.0)),
    location_(*this, vec3f(0.0, 0.0, 0.0)),
    radius_(*this, 100)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::point_light_node::~point_light_node() throw ()
{}

/**
 * @brief Cast to a point_light_node.
 *
 * @return a pointer to the point_light_node.
 */
openvrml::vrml97_node::point_light_node *
openvrml::vrml97_node::point_light_node::to_point_light() const
{
    return (point_light_node*) this;
}

/**
 * @brief Render the scoped light.
 *
 * This should be called before rendering any geometry nodes in the scene.
 * Since this is called from Scene::render() before traversing the
 * scene graph, the proper transformation matrix hasn't been set up.
 * Somehow it needs to figure out the accumulated xforms of its
 * parents and apply them before rendering. This is not easy with
 * DEF/USEd nodes...
 *
 * @param viewer    a viewer.
 */
void
openvrml::vrml97_node::point_light_node::
renderScoped(openvrml::viewer & viewer)
{
    if (this->on_.sfbool::value && this->radius_.sffloat::value > 0.0) {
        viewer.insert_point_light(this->ambient_intensity_.sffloat::value,
                                  this->attenuation_.sfvec3f::value,
                                  this->color_.sfcolor::value,
                                  this->intensity_.sffloat::value,
                                  this->location_.sfvec3f::value,
                                  this->radius_.sffloat::value);
    }
    this->node::modified(false);
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::point_light_node::do_initialize(const double timestamp)
    throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser().add_scoped_light(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void
openvrml::vrml97_node::point_light_node::do_shutdown(const double timestamp)
    throw ()
{
    assert(this->scene());
    this->scene()->browser().remove_scoped_light(*this);
}


/**
 * @class openvrml::vrml97_node::point_set_class
 *
 * @brief Class object for PointSet nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::point_set_class::
point_set_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::point_set_class::~point_set_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating PointSet nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by point_set_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::point_set_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "coord")
    };

    typedef vrml97_node_type_impl<point_set_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & pointSetNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            pointSetNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &point_set_node::color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &point_set_node::color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &point_set_node::color_)));
        } else if (*interface == supportedInterfaces[1]) {
            pointSetNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &point_set_node::coord_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &point_set_node::coord_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &point_set_node::coord_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::point_set_node
 *
 * @brief Represents PointSet node instances.
 */

/**
 * @var openvrml::vrml97_node::point_set_node::point_set_class
 *
 * @brief Class object for PointSet instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::point_set_node::color_
 *
 * @brief color exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::point_set_node::coord_
 *
 * @brief coord exposedField.
 */

/**
 * @var openvrml::bounding_sphere openvrml::vrml97_node::point_set_node::bsphere
 *
 * @brief Bounding volume.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::point_set_node::
point_set_node(const node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    geometry_node(type, scope),
    color_(*this),
    coord_(*this)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::point_set_node::~point_set_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::point_set_node::modified() const
{
    return this->node::modified()
        || (this->color_.sfnode::value
            && this->color_.sfnode::value->modified())
        || (this->coord_.sfnode::value
            && this->coord_.sfnode::value->modified());
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
openvrml::viewer::object_t
openvrml::vrml97_node::point_set_node::
do_render_geometry(openvrml::viewer & viewer, const rendering_context context)
{
    using std::vector;

    if (context.draw_bounding_spheres) {
        assert(dynamic_cast<const bounding_sphere *>
               (&this->bounding_volume()));
        const bounding_sphere & bs =
            static_cast<const bounding_sphere &>(this->bounding_volume());
        viewer.draw_bounding_sphere(bs, bounding_volume::intersection(4));
    }

    openvrml::coordinate_node * const coordinateNode =
        node_cast<openvrml::coordinate_node *>(
            this->coord_.sfnode::value.get());
    const vector<vec3f> & coord = coordinateNode
                                ? coordinateNode->point()
                                : vector<vec3f>();

    openvrml::color_node * const colorNode =
        node_cast<openvrml::color_node *>(this->color_.sfnode::value.get());
    const vector<openvrml::color> & color = colorNode
                                          ? colorNode->color()
                                          : vector<openvrml::color>();

    viewer::object_t obj = viewer.insert_point_set(coord, color);

    if (colorNode) { colorNode->modified(false); }
    if (coordinateNode) { coordinateNode->modified(false); }

    return obj;
}

/**
 * @brief Indicate that point sets should be drawn with the emissive color.
 *
 * @return @c true.
 */
bool openvrml::vrml97_node::point_set_node::do_emissive() const throw ()
{
    return true;
}

/**
 * @brief Recalculate the bounding volume.
 */
void openvrml::vrml97_node::point_set_node::recalcBSphere()
{
    this->bsphere = bounding_sphere();
    openvrml::coordinate_node * const coordinateNode =
        node_cast<openvrml::coordinate_node *>(
            this->coord_.sfnode::value.get());
    if (coordinateNode) {
        const std::vector<vec3f> & coord = coordinateNode->point();
        for(std::vector<vec3f>::const_iterator vec(coord.begin());
                vec != coord.end(); ++vec) {
            this->bsphere.extend(*vec);
        }
    }
    this->bounding_volume_dirty(false);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const openvrml::bounding_volume &
openvrml::vrml97_node::point_set_node::do_bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<point_set_node *>(this)->recalcBSphere();
    }
    return this->bsphere;
}


/**
 * @class openvrml::vrml97_node::position_interpolator_class
 *
 * @brief Class object for PositionInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::position_interpolator_class::
position_interpolator_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::position_interpolator_class::
~position_interpolator_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating
 *      PositionInterpolator nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by position_interpolator_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::position_interpolator_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                      field_value::sffloat_id,
                      "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                      field_value::mffloat_id,
                      "key"),
        node_interface(node_interface::exposedfield_id,
                      field_value::mfvec3f_id,
                      "keyValue"),
        node_interface(node_interface::eventout_id,
                      field_value::sfvec3f_id,
                      "value_changed")
    };

    typedef vrml97_node_type_impl<position_interpolator_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & positionInterpolatorNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            positionInterpolatorNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    position_interpolator_node::set_fraction_listener>(
                        &position_interpolator_node::set_fraction_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            positionInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mffloat> >(
                        &position_interpolator_node::key_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mffloat> >(
                        &position_interpolator_node::key_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mffloat> >(
                        &position_interpolator_node::key_)));
        } else if (*interface == supportedInterfaces[2]) {
            positionInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mfvec3f> >(
                        &position_interpolator_node::key_value_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfvec3f> >(
                        &position_interpolator_node::key_value_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mfvec3f> >(
                        &position_interpolator_node::key_value_)));
        } else if (*interface == supportedInterfaces[3]) {
            positionInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfvec3f_emitter>(
                        &position_interpolator_node::value_changed_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::position_interpolator_node
 *
 * @brief PositionInterpolator node instances.
 */

/**
 * @var class openvrml::vrml97_node::position_interpolator_node::position_interpolator_class
 *
 * @brief Class object for PositionInterpolator nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::position_interpolator_node::set_fraction_listener
 *
 * @brief set_fraction eventIn handler.
 */

/**
 * @brief Construct.
 *
 * @param node  position_interpolator_node.
 */
openvrml::vrml97_node::position_interpolator_node::set_fraction_listener::
set_fraction_listener(position_interpolator_node & node):
    sffloat_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::position_interpolator_node::set_fraction_listener::
~set_fraction_listener() throw ()
{}

/**
 * @brief Process event.
 *
 * @param fraction  fraction.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::position_interpolator_node::set_fraction_listener::
do_process_event(const sffloat & fraction, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        using std::vector;

        position_interpolator_node & node =
            dynamic_cast<position_interpolator_node &>(this->node());

        const vector<float> & key = node.key_.mffloat::value;
        const vector<vec3f> & key_value = node.key_value_.mfvec3f::value;

        vector<float>::size_type n = key.size() - 1;
        if (fraction.value < key[0]) {
            node.value_changed_.value = key_value[0];
        } else if (fraction.value > key[n]) {
            node.value_changed_.value = key_value[n];
        } else {
            // should cache the last index used...
            for (vector<float>::size_type i = 0; i < n; ++i) {
                if (key[i] <= fraction.value && fraction.value <= key[i + 1]) {
                    const vec3f & v1 = key_value[i];
                    const vec3f & v2 = key_value[i + 1];

                    const float f =
                        (fraction.value - key[i]) / (key[i + 1] - key[i]);
                    node.value_changed_.value = v1 + f * (v2 - v1);
                    break;
                }
            }
        }

        // Send the new value
        node::emit_event(node.value_changed_emitter_, timestamp);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::position_interpolator_node::set_fraction_listener openvrml::vrml97_node::position_interpolator_node::set_fraction_listener_
 *
 * @brief set_fraction eventIn handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::mffloat> openvrml::vrml97_node::position_interpolator_node::key_
 *
 * @brief key exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfvec3f> openvrml::vrml97_node::position_interpolator_node::key_value_
 *
 * @brief keyValue exposedField.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::position_interpolator_node::value_changed_
 *
 * @brief value_changed eventOut value.
 */

/**
 * @var openvrml::sfvec3f_emitter openvrml::vrml97_node::position_interpolator_node::value_changed_emitter_
 *
 * @brief value_changed eventOut emitter.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::position_interpolator_node::
position_interpolator_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    set_fraction_listener_(*this),
    key_(*this),
    key_value_(*this),
    value_changed_emitter_(this->value_changed_)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::position_interpolator_node::
~position_interpolator_node() throw ()
{}


/**
 * @class openvrml::vrml97_node::proximity_sensor_class
 *
 * @brief Class object for ProximitySensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::proximity_sensor_class::
proximity_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::proximity_sensor_class::~proximity_sensor_class()
    throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating ProximitySensor
 *      nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by proximity_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::proximity_sensor_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "center"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "size"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "enabled"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "position_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfrotation_id,
                       "orientation_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "enterTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "exitTime")
    };

    typedef vrml97_node_type_impl<proximity_sensor_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & proximitySensorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            proximitySensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(&proximity_sensor_node::center_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &proximity_sensor_node::center_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &proximity_sensor_node::center_)));
        } else if (*interface == supportedInterfaces[1]) {
            proximitySensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(&proximity_sensor_node::size_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &proximity_sensor_node::size_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &proximity_sensor_node::size_)));
        } else if (*interface == supportedInterfaces[2]) {
            proximitySensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &proximity_sensor_node::enabled_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &proximity_sensor_node::enabled_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &proximity_sensor_node::enabled_)));
        } else if (*interface == supportedInterfaces[3]) {
            proximitySensorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &proximity_sensor_node::is_active_emitter_)));
        } else if (*interface == supportedInterfaces[4]) {
            proximitySensorNodeType.add_eventout(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfvec3f_emitter>(
                        &proximity_sensor_node::position_changed_emitter_)));
        } else if (*interface == supportedInterfaces[5]) {
            proximitySensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfrotation_emitter>(
                        &proximity_sensor_node::orientation_changed_emitter_)));
        } else if (*interface == supportedInterfaces[6]) {
            proximitySensorNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &proximity_sensor_node::enter_time_emitter_)));
        } else if (*interface == supportedInterfaces[7]) {
            proximitySensorNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &proximity_sensor_node::exit_time_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::proximity_sensor_node
 *
 * @brief Represents ProximitySensor node instances.
 */

/**
 * @var openvrml::vrml97_node::proximity_sensor_node::proximity_sensor_class
 *
 * @brief Class object for ProximitySensor instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::proximity_sensor_node::center_
 *
 * @brief center exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::proximity_sensor_node::enabled_
 *
 * @brief enabled exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::proximity_sensor_node::size_
 *
 * @brief size exposedField.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::proximity_sensor_node::is_active_
 *
 * @brief isActive eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::proximity_sensor_node::is_active_emitter_
 *
 * @brief isActive eventOut emitter.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::proximity_sensor_node::position_changed_
 *
 * @brief position_changed eventOut value.
 */

/**
 * @var openvrml::sfvec3f_emitter openvrml::vrml97_node::proximity_sensor_node::position_changed_emitter_
 *
 * @brief position_changed eventOut emitter.
 */

/**
 * @var openvrml::sfrotation openvrml::vrml97_node::proximity_sensor_node::orientation_changed_
 *
 * @brief orientation_changed eventOut value.
 */

/**
 * @var openvrml::sfrotation openvrml::vrml97_node::proximity_sensor_node::orientation_changed_emitter_
 *
 * @brief orientation_changed eventOut emitter.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::proximity_sensor_node::enter_time_
 *
 * @brief enterTime eventOut value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::proximity_sensor_node::enter_time_emitter_
 *
 * @brief enterTime eventOut emitter.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::proximity_sensor_node::exit_time_
 *
 * @brief exitTime eventOut value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::proximity_sensor_node::exit_time_emitter_
 *
 * @brief exitTime eventOut emitter.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::proximity_sensor_node::
proximity_sensor_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    center_(*this, vec3f(0.0, 0.0, 0.0)),
    enabled_(*this, true),
    size_(*this, vec3f(0.0, 0.0, 0.0)),
    is_active_(false),
    is_active_emitter_(this->is_active_),
    position_changed_emitter_(this->position_changed_),
    orientation_changed_emitter_(this->orientation_changed_),
    enter_time_(0.0),
    enter_time_emitter_(this->enter_time_),
    exit_time_(0.0),
    exit_time_emitter_(this->exit_time_)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::proximity_sensor_node::~proximity_sensor_node() throw ()
{}

/**
 * @brief Render the node: generate proximity events.
 *
 * If necessary, events prior to the current time are generated due to
 * interpolation of enterTimes and exitTimes. The timestamp should never be
 * increased.
 *
 * This is in a render() method since the it needs the viewer position
 * with respect to the local coordinate system.
 * Could do this with Node::inverseTransform(double [4][4]) now...
 *
 * The positions and times are not interpolated to report the exact
 * place and time of entries and exits from the sensor regions as
 * required by the spec, since that would require the last viewer position
 * to be stored in the node, which is problematic in the presence of
 * DEF/USEd nodes...
 * I suppose the scene could keep the last viewer position in the global
 * coordinate system and it could be transformed all the way down the
 * scenegraph, but that sounds painful.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void
openvrml::vrml97_node::proximity_sensor_node::
do_render_child(openvrml::viewer & viewer, const rendering_context context)
{
    if (this->enabled_.sfbool::value
        && this->size_.sfvec3f::value.x() > 0.0
        && this->size_.sfvec3f::value.y() > 0.0
        && this->size_.sfvec3f::value.z() > 0.0
        && viewer.mode() == viewer::draw_mode) {
        using openvrml_::fabs;
        using openvrml_::fless_equal;

        sftime timeNow(browser::current_time());
        float x, y, z;

        // Is viewer inside the box?
        mat4f MV = context.matrix();
        MV = MV.inverse();
        x = MV[3][0]; y = MV[3][1]; z = MV[3][2];
        bool inside = fless_equal<float>()(fabs(x - this->center_.sfvec3f::value.x()),
                                           0.5f * this->size_.sfvec3f::value.x())
            && fless_equal<float>()(fabs(y - this->center_.sfvec3f::value.y()),
                                    0.5f * this->size_.sfvec3f::value.y())
            && fless_equal<float>()(fabs(z - this->center_.sfvec3f::value.z()),
                                    0.5f * this->size_.sfvec3f::value.z());
        bool wasIn = this->is_active_.value;

        // Check if viewer has entered the box
        if (inside && ! wasIn) {
            this->is_active_.value = true;
            node::emit_event(this->is_active_emitter_, timeNow.value);

            this->enter_time_ = timeNow;
            node::emit_event(this->enter_time_emitter_, timeNow.value);
        }

        // Check if viewer has left the box
        else if (wasIn && !inside) {
            this->is_active_.value = false;
            node::emit_event(this->is_active_emitter_, timeNow.value);

            this->exit_time_ = timeNow;
            node::emit_event(this->exit_time_emitter_, timeNow.value);
        }

        // Check for movement within the box
        if (wasIn || inside) {
            if (this->position_changed_.value != vec3f(x, y, z)) {
                this->position_changed_.value = vec3f(x, y, z);
                node::emit_event(this->position_changed_emitter_,
                                 timeNow.value);
            }

            vec3f trans, scale, shear;
            rotation orientation;
            MV.transformation(trans, orientation, scale, shear);
            if (this->orientation_changed_.value != orientation) {
                this->orientation_changed_.value = orientation;
                node::emit_event(this->orientation_changed_emitter_,
                                 timeNow.value);
            }
        }
    } else {
        this->node::modified(false);
    }
}


/**
 * @class openvrml::vrml97_node::scalar_interpolator_class
 *
 * @brief Class object for ScalarInterpolator nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::scalar_interpolator_class::
scalar_interpolator_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::scalar_interpolator_class::~scalar_interpolator_class()
    throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating
 *      CoordinateInterpolator nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by CoordinateInterpolatorClass.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::scalar_interpolator_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::sffloat_id,
                       "set_fraction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "key"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "keyValue"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "value_changed")
    };

    typedef vrml97_node_type_impl<scalar_interpolator_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & scalarInterpolatorNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            scalarInterpolatorNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    scalar_interpolator_node::set_fraction_listener>(
                        &scalar_interpolator_node::set_fraction_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            scalarInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mffloat> >(&scalar_interpolator_node::key_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mffloat> >(
                        &scalar_interpolator_node::key_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mffloat> >(
                        &scalar_interpolator_node::key_)));
        } else if (*interface == supportedInterfaces[2]) {
            scalarInterpolatorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mffloat> >(
                        &scalar_interpolator_node::key_value_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mffloat> >(
                        &scalar_interpolator_node::key_value_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mffloat> >(
                        &scalar_interpolator_node::key_value_)));
        } else if (*interface == supportedInterfaces[3]) {
            scalarInterpolatorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sffloat_emitter>(
                        &scalar_interpolator_node::value_changed_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::scalar_interpolator_node
 *
 * @brief ScalarInterpolator node instances.
 */

/**
 * @var class openvrml::vrml97_node::scalar_interpolator_node::scalar_interpolator_class
 *
 * @brief Class object for ScalarInterpolator nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::scalar_interpolator_node::set_fraction_listener
 *
 * @brief set_fraction eventIn handler.
 */

/**
 * @brief Construct.
 *
 * @param node  scalar_interpolator_node.
 */
openvrml::vrml97_node::scalar_interpolator_node::set_fraction_listener::
set_fraction_listener(scalar_interpolator_node & node):
    sffloat_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::scalar_interpolator_node::set_fraction_listener::
~set_fraction_listener() throw ()
{}

/**
 * @brief Process event.
 *
 * @param fraction  fraction.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::scalar_interpolator_node::set_fraction_listener::
do_process_event(const sffloat & fraction, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        using std::vector;

        scalar_interpolator_node & node =
            dynamic_cast<scalar_interpolator_node &>(this->node());

        const vector<float> & key = node.key_.mffloat::value;
        const vector<float> & key_value = node.key_value_.mffloat::value;

        vector<float>::size_type n = key.size() - 1;
        if (fraction.value < key[0]) {
            node.value_changed_.value = key_value[0];
        } else if (fraction.value > key[n]) {
            node.value_changed_.value = key_value[n];
        } else {
            for (vector<float>::size_type i = 0; i < n; ++i) {
                if (key[i] <= fraction.value && fraction.value <= key[i + 1]) {
                    float v1 = key_value[i];
                    float v2 = key_value[i + 1];

                    const float f =
                        (fraction.value - key[i]) / (key[i + 1] - key[i]);
                    node.value_changed_.value = v1 + f * (v2 - v1);
                    break;
                }
            }
        }

        // Send the new value
        node::emit_event(node.value_changed_emitter_, timestamp);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::scalar_interpolator_node::set_fraction_listener openvrml::vrml97_node::scalar_interpolator_node::set_fraction_listener_
 *
 * @brief set_fraction eventIn handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::mffloat> openvrml::vrml97_node::scalar_interpolator_node::key_
 *
 * @brief key exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::mffloat> openvrml::vrml97_node::scalar_interpolator_node::key_value_
 *
 * @brief keyValue exposedField.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::scalar_interpolator_node::value_changed_
 *
 * @brief value_changed eventOut value.
 */

/**
 * @var openvrml::sffloat_emitter openvrml::vrml97_node::scalar_interpolator_node::value_changed_emitter_
 *
 * @brief value_changed eventOut emitter.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::scalar_interpolator_node::
scalar_interpolator_node(const node_type & type,
                         const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    set_fraction_listener_(*this),
    key_(*this),
    key_value_(*this),
    value_changed_emitter_(this->value_changed_)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::scalar_interpolator_node::~scalar_interpolator_node()
    throw ()
{}


/**
 * @class openvrml::vrml97_node::shape_class
 *
 * @brief Class object for Shape nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::shape_class::shape_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::shape_class::~shape_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Shape nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by shape_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::shape_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "appearance"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "geometry")
    };

    typedef vrml97_node_type_impl<shape_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & shapeNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            shapeNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &shape_node::appearance_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &shape_node::appearance_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &shape_node::appearance_)));
        } else if (*interface == supportedInterfaces[1]) {
            shapeNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &shape_node::geometry_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &shape_node::geometry_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &shape_node::geometry_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::shape_node
 *
 * @brief Represents Shape node instances.
 */

/**
 * @var openvrml::vrml97_node::shape_node::shape_class
 *
 * @brief Class object for Shape instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::shape_node::appearance_
 *
 * @brief appearance exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::shape_node::geometry_
 *
 * @brief geometry exposedField.
 */

/**
 * @var openvrml::viewer::object_t openvrml::vrml97_node::shape_node::viewerObject
 *
 * @brief A reference to the node's previously used rendering data.
 *
 * If supported by the Viewer implementation, this member holds a reference
 * to the node's rendering data once the node has already been rendered once.
 * The intent is to capitalize on USE references in the VRML scene graph.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::shape_node::
shape_node(const node_type & type,
           const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    appearance_(*this),
    geometry_(*this),
    viewerObject(0)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::shape_node::~shape_node() throw ()
{
    // need viewer to free viewerObject ...
}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::shape_node::modified() const
{
    return this->node::modified()
        || (this->geometry_.sfnode::value
            && this->geometry_.sfnode::value->modified())
        || (this->appearance_.sfnode::value
            && this->appearance_.sfnode::value->modified());
}

/**
 * @brief Render the node.
 *
 * @param viewer    a viewer.
 * @param context   a rendering context.
 */
void
openvrml::vrml97_node::shape_node::
do_render_child(openvrml::viewer & viewer, const rendering_context context)
{
    openvrml::appearance_node * const appearance =
        node_cast<openvrml::appearance_node *>(
            this->appearance_.sfnode::value.get());
    openvrml::material_node * const material =
        appearance
        ? node_cast<openvrml::material_node *>(appearance->material().get())
        : 0;
    geometry_node * const geometry =
        node_cast<geometry_node *>(this->geometry_.sfnode::value.get());

    if (this->viewerObject && (this->modified()
                               || (appearance && appearance->modified())
                               || (geometry && geometry->modified()))) {
        viewer.remove_object(this->viewerObject);
        this->viewerObject = 0;
    }

    if (this->viewerObject) {
        viewer.insert_reference(this->viewerObject);
    } else if (geometry) {
        this->viewerObject = viewer.begin_object(this->id().c_str());

        // Don't care what color it is if we are picking
        bool picking = (viewer::pick_mode == viewer.mode());
        if (!picking) {
            size_t texture_components = 0;

            if (appearance) {
                appearance->render_appearance(viewer, context);

                texture_node * const texture =
                    node_cast<texture_node *>(appearance->texture().get());
                if (texture) { texture_components = texture->image().comp(); }
            } else {
                viewer.enable_lighting(false);  // turn lighting off
                set_unlit_material(viewer);
            }

            // hack for opengl material mode
            viewer.set_material_mode(texture_components, geometry->color());

            //
            // Set the drawing color.
            //
            // Most geometries are drawn using the diffuse color; but some
            // (notably point and line sets) are drawn with the emissive color.
            //
            color c(1.0, 1.0, 1.0);
            float transparency = 0.0;
            if (material) {
                if (geometry && geometry->emissive()) {
                    c = material->emissive_color();
                } else {
                    c = material->diffuse_color();
                }
                transparency = material->transparency();
            }
            viewer.set_color(c, transparency);
        }

        geometry->render_geometry(viewer, context);

        viewer.end_object();
    }
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const openvrml::bounding_volume &
openvrml::vrml97_node::shape_node::do_bounding_volume() const
{
    //
    // just pass off to the geometry's getbvolume() method
    //
    bounded_volume_node * bv =
        node_cast<bounded_volume_node *>(this->geometry_.sfnode::value.get());
    const openvrml::bounding_volume & result = bv
        ? bv->bounding_volume()
        : this->bounded_volume_node::do_bounding_volume();
    const_cast<shape_node *>(this)->bounding_volume_dirty(false);
    return result;
}


/**
 * @class openvrml::vrml97_node::sound_class
 *
 * @brief Class object for Sound nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::sound_class::sound_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::sound_class::~sound_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Sound nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by sound_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::sound_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "direction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "intensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "location"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "maxBack"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "maxFront"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "minBack"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "minFront"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "priority"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "source"),
        node_interface(node_interface::field_id,
                       field_value::sfbool_id,
                       "spatialize")
    };

    typedef vrml97_node_type_impl<sound_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & soundNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(&sound_node::direction_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &sound_node::direction_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &sound_node::direction_)));
        } else if (*interface == supportedInterfaces[1]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(&sound_node::intensity_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &sound_node::intensity_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &sound_node::intensity_)));
        } else if (*interface == supportedInterfaces[2]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(&sound_node::location_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &sound_node::location_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &sound_node::location_)));
        } else if (*interface == supportedInterfaces[3]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(&sound_node::max_back_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &sound_node::max_back_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &sound_node::max_back_)));
        } else if (*interface == supportedInterfaces[4]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(&sound_node::max_front_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &sound_node::max_front_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &sound_node::max_front_)));
        } else if (*interface == supportedInterfaces[5]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(&sound_node::min_back_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &sound_node::min_back_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &sound_node::min_back_)));
        } else if (*interface == supportedInterfaces[6]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(&sound_node::min_front_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &sound_node::min_front_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &sound_node::min_front_)));
        } else if (*interface == supportedInterfaces[7]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(&sound_node::priority_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &sound_node::priority_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &sound_node::priority_)));
        } else if (*interface == supportedInterfaces[8]) {
            soundNodeType.add_exposedfield(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfnode> >(
                        &sound_node::source_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfnode> >(
                        &sound_node::source_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfnode> >(
                        &sound_node::source_)));
        } else if (*interface == supportedInterfaces[9]) {
            soundNodeType.add_field(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfbool>(
                        &sound_node::spatialize_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::sound_node
 *
 * @brief Represents Sound node instances.
 */

/**
 * @var openvrml::vrml97_node::sound_node::sound_class
 *
 * @brief Class object for Sound instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::sound_node::direction_
 *
 * @brief direction exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::sound_node::intensity_
 *
 * @brief intensity exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::sound_node::location_
 *
 * @brief location exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::sound_node::max_back_
 *
 * @brief maxBack exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::sound_node::max_front_
 *
 * @brief maxFront exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::sound_node::min_back_
 *
 * @brief minBack exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::sound_node::min_front_
 *
 * @brief minFront exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::sound_node::priority_
 *
 * @brief priority exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfnode> openvrml::vrml97_node::sound_node::source_
 *
 * @brief source exposedField.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::sound_node::spatialize_
 *
 * @brief spatialize field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope associated with the instance.
 */
openvrml::vrml97_node::sound_node::
sound_node(const node_type & type,
           const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    direction_(*this, vec3f(0, 0, 1)),
    intensity_(*this, 1.0f),
    location_(*this),
    max_back_(*this, 10.0f),
    max_front_(*this, 10.0f),
    min_back_(*this, 1.0f),
    min_front_(*this, 1.0f),
    priority_(*this),
    source_(*this),
    spatialize_(true)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::sound_node::~sound_node() throw ()
{}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void
openvrml::vrml97_node::sound_node::
do_render_child(openvrml::viewer & viewer, const rendering_context context)
{
    // If this clip has been modified, update the internal data
    if (this->source_.sfnode::value
        && this->source_.sfnode::value->modified()) {
//        this->source.value->render(viewer, context);
    }
}


/**
 * @class openvrml::vrml97_node::sphere_class
 *
 * @brief Class object for Sphere nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::sphere_class::sphere_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::sphere_class::~sphere_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Sphere nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by sphere_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::sphere_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface(node_interface::field_id,
                                                   field_value::sffloat_id,
                                                   "radius");

    typedef vrml97_node_type_impl<sphere_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & spereNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterface) {
            spereNodeType.add_field(
                supportedInterface.field_type,
                supportedInterface.id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sffloat>(
                        &sphere_node::radius)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::sphere_node
 *
 * @brief Sphere node instances.
 */

/**
 * @var openvrml::vrml97_node::sphere_node::sphere_class
 *
 * @brief Class object for Sphere instances.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::sphere_node::radius
 *
 * @brief radius field.
 */

/**
 * @var openvrml::bounding_sphere openvrml::vrml97_node::sphere_node::bsphere
 *
 * @brief Bounding volume.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::sphere_node::
sphere_node(const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    geometry_node(type, scope),
    radius(1.0)
{
    this->bounding_volume_dirty(true); // lazy calc of bvolumes
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::sphere_node::~sphere_node() throw ()
{}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
openvrml::viewer::object_t
openvrml::vrml97_node::sphere_node::
do_render_geometry(openvrml::viewer & viewer, const rendering_context context)
{
    const viewer::object_t object_ref =
        viewer.insert_sphere(this->radius.value);
    return object_ref;
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const openvrml::bounding_volume &
openvrml::vrml97_node::sphere_node::do_bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<sphere_node *>(this)->bsphere.radius(this->radius.value);
        const_cast<sphere_node *>(this)->bounding_volume_dirty(false); // logical const
    }
    return this->bsphere;
}


/**
 * @class openvrml::vrml97_node::sphere_sensor_class
 *
 * @brief Class object for SphereSensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::sphere_sensor_class::
sphere_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::sphere_sensor_class::~sphere_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating SphereSensor
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by sphere_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::sphere_sensor_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "autoOffset"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "enabled"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfrotation_id,
                       "offset"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sfrotation_id,
                       "rotation_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfvec3f_id,
                       "trackPoint_changed")
    };

    typedef vrml97_node_type_impl<sphere_sensor_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & sphereSensorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            sphereSensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &sphere_sensor_node::auto_offset_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &sphere_sensor_node::auto_offset_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &sphere_sensor_node::auto_offset_)));
        } else if (*interface == supportedInterfaces[1]) {
            sphereSensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &sphere_sensor_node::enabled_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &sphere_sensor_node::enabled_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &sphere_sensor_node::enabled_)));
        } else if (*interface == supportedInterfaces[2]) {
            sphereSensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfrotation> >(&sphere_sensor_node::offset_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfrotation> >(
                        &sphere_sensor_node::offset_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    exposedfield<sfrotation> >(&sphere_sensor_node::offset_)));
        } else if (*interface == supportedInterfaces[3]) {
            sphereSensorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &sphere_sensor_node::is_active_emitter_)));
        } else if (*interface == supportedInterfaces[4]) {
            sphereSensorNodeType.add_eventout(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfrotation_emitter>(
                        &sphere_sensor_node::rotation_changed_emitter_)));
        } else if (*interface == supportedInterfaces[5]) {
            sphereSensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfvec3f_emitter>(
                        &sphere_sensor_node::track_point_changed_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::sphere_sensor_node
 *
 * @brief SphereSensor node instances.
 */

/**
 * @var openvrml::vrml97_node::sphere_sensor_node::sphere_sensor_class
 *
 * @brief Class object for SphereSensor instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::sphere_sensor_node::auto_offset_
 *
 * @brief autoOffset exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::sphere_sensor_node::enabled_
 *
 * @brief enabled exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfrotation> openvrml::vrml97_node::sphere_sensor_node::offset_
 *
 * @brief offset exposedField.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::sphere_sensor_node::is_active_
 *
 * @brief isActive eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::sphere_sensor_node::is_active_emitter_
 *
 * @brief isActive eventOut emitter.
 */

/**
 * @var openvrml::sfrotation openvrml::vrml97_node::sphere_sensor_node::rotation_changed_
 *
 * @brief rotation_changed eventOut value.
 */

/**
 * @var openvrml::sfrotation_emitter openvrml::vrml97_node::sphere_sensor_node::rotation_changed_emitter_
 *
 * @brief rotation_changed eventOut emitter.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::sphere_sensor_node::track_point_changed_
 *
 * @brief trackPoint_changed eventOut.
 */

/**
 * @var openvrml::sfvec3f_emitter openvrml::vrml97_node::sphere_sensor_node::track_point_changed_emitter_
 *
 * @brief trackPoint_changed eventOut emitter.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::sphere_sensor_node::activationPoint
 *
 * @brief The start point of a drag operation.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::sphere_sensor_node::centerPoint
 *
 * @brief Center point.
 */

/**
 * @var openvrml::mat4f openvrml::vrml97_node::sphere_sensor_node::modelview
 *
 * @brief Modelview matrix.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::sphere_sensor_node::
sphere_sensor_node(const node_type & type,
                   const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    auto_offset_(*this, true),
    enabled_(*this, true),
    offset_(*this, openvrml::rotation(0.0, 1.0, 0.0, 0.0)),
    is_active_(false),
    is_active_emitter_(this->is_active_),
    rotation_changed_emitter_(this->rotation_changed_),
    track_point_changed_emitter_(this->track_point_changed_)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::sphere_sensor_node::~sphere_sensor_node() throw ()
{}

/**
 * @brief Cast to a sphere_sensor_node.
 *
 * @return a pointer to the node.
 */
openvrml::vrml97_node::sphere_sensor_node *
openvrml::vrml97_node::sphere_sensor_node::to_sphere_sensor() const
{
    return const_cast<sphere_sensor_node *>(this);
}

/**
 * @brief Render the node.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void
openvrml::vrml97_node::sphere_sensor_node::
do_render_child(openvrml::viewer & viewer, const rendering_context context)
{
    //
    // Store the ModelView matrix which is calculated at the time of rendering
    // in render-context. This matrix will be in use at the time of activation
    //
    this->modelview = context.matrix();
}

/**
 * @brief Activate or deactivate the sphere_sensor_node.
 *
 * Activating a drag sensor means that the pointing device button has been
 * depressed and a drag operation has been initiated. The sensor is deactivated
 * when the button is released at the end of the operation.
 *
 * @param timestamp the current time.
 * @param isActive  @c true if the drag operation is in progress; @c false
 *                  otherwise.
 * @param p         the pointing device position.
 */
void openvrml::vrml97_node::sphere_sensor_node::activate(double timestamp,
                                                         bool isActive,
                                                         double * p)
{
    // Become active
    if (isActive && !this->is_active_.value) {
        this->is_active_.value = isActive;

        // set activation point in world coords
        const vec3f floatVec(static_cast<float>(p[0]),
                             static_cast<float>(p[1]),
                             static_cast<float>(p[2]));
        this->activationPoint.value = floatVec;

        if (this->auto_offset_.sfbool::value) {
            this->rotation_changed_ = this->offset_;
        }

        // calculate the center of the object in world coords
        vec3f V;
        mat4f M = this->modelview.inverse();
        V = V * M;
        this->centerPoint.value = V;

        // send message
        node::emit_event(this->is_active_emitter_, timestamp);
    }
    // Become inactive
    else if (!isActive && this->is_active_.value) {
        this->is_active_.value = isActive;
        node::emit_event(this->is_active_emitter_, timestamp);

        // save auto offset of rotation
        if (this->auto_offset_.sfbool::value) {
            this->offset_.sfrotation::value = this->rotation_changed_.value;
            node::emit_event(this->offset_, timestamp);
        }
    }
    // Tracking
    else if (isActive) {
        // get local coord for touch point
        vec3f V(static_cast<float>(p[0]),
                static_cast<float>(p[1]),
                static_cast<float>(p[2]));
        mat4f M = this->modelview.inverse();
        V = V * M;
        this->track_point_changed_.value = V;
        node::emit_event(this->track_point_changed_emitter_, timestamp);

        vec3f V2(static_cast<float>(p[0]),
                 static_cast<float>(p[1]),
                 static_cast<float>(p[2]));
        vec3f tempv = V2 - this->centerPoint.value;
        vec3f dir1(tempv);

        //
        // Get the length of the pre-normalized vector.
        //
        const float dist = dir1.length();

        dir1 = dir1.normalize();
        tempv = this->activationPoint.value - this->centerPoint.value;
        vec3f dir2(tempv);
        dir2 = dir2.normalize();

        tempv = dir1 * dir2;
        vec3f cx(tempv);
        cx = cx.normalize();

        openvrml::rotation newRot(cx, dist * float(acos(dir1.dot(dir2))));
        if (this->auto_offset_.sfbool::value) {
            newRot = newRot * this->offset_.sfrotation::value;
        }
        this->rotation_changed_.value = newRot;

        node::emit_event(this->rotation_changed_emitter_, timestamp);
    }
}

/**
 * @brief Determine whether the SphereSensor is enabled.
 *
 * @return @c true if the SphereSensor is enabled; @c false otherwise.
 */
bool openvrml::vrml97_node::sphere_sensor_node::isEnabled() const throw ()
{
    return this->enabled_.sfbool::value;
}


/**
 * @class openvrml::vrml97_node::spot_light_class
 *
 * @brief Class object for SpotLight nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::spot_light_class::
spot_light_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::spot_light_class::~spot_light_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating SpotLight nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by spot_light_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::spot_light_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "ambientIntensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "attenuation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "beamWidth"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfcolor_id,
                       "color"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "cutOffAngle"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "direction"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "intensity"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "location"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "on"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "radius")
    };

    typedef vrml97_node_type_impl<spot_light_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & spotLightNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(
                        &spot_light_node::ambient_intensity_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &spot_light_node::ambient_intensity_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &spot_light_node::ambient_intensity_)));
        } else if (*interface == supportedInterfaces[1]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(
                        &spot_light_node::attenuation_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &spot_light_node::attenuation_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &spot_light_node::attenuation_)));
        } else if (*interface == supportedInterfaces[2]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(
                        &spot_light_node::beam_width_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &spot_light_node::beam_width_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &spot_light_node::beam_width_)));
        } else if (*interface == supportedInterfaces[3]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfcolor> >(
                        &spot_light_node::color_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfcolor> >(
                        &spot_light_node::color_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfcolor> >(
                        &spot_light_node::color_)));
        } else if (*interface == supportedInterfaces[4]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(
                        &spot_light_node::cut_off_angle_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &spot_light_node::cut_off_angle_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &spot_light_node::cut_off_angle_)));
        } else if (*interface == supportedInterfaces[5]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(
                        &spot_light_node::direction_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &spot_light_node::direction_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &spot_light_node::direction_)));
        } else if (*interface == supportedInterfaces[6]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(
                        &spot_light_node::intensity_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &spot_light_node::intensity_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &spot_light_node::intensity_)));
        } else if (*interface == supportedInterfaces[7]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(
                        &spot_light_node::location_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &spot_light_node::location_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &spot_light_node::location_)));
        } else if (*interface == supportedInterfaces[8]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &spot_light_node::on_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &spot_light_node::on_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &spot_light_node::on_)));
        } else if (*interface == supportedInterfaces[9]) {
            spotLightNodeType.add_exposedfield(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(
                        &spot_light_node::radius_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &spot_light_node::radius_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &spot_light_node::radius_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::spot_light_node
 *
 * @brief SpotLight node instances.
 */

/**
 * @var openvrml::vrml97_node::spot_light_node::spot_light_class
 *
 * @brief Class object for SpotLight instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::spot_light_node::attenuation_
 *
 * @brief attenuation exposedField.
 */

/**
 * @var expoosedfield<sffloat> openvrml::vrml97_node::spot_light_node::beam_width_
 *
 * @brief beamWidth exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::spot_light_node::cut_off_angle_
 *
 * @brief cutOffAngle exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::spot_light_node::direction_
 *
 * @brief direction exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::spot_light_node::location_
 *
 * @brief location exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::spot_light_node::radius_
 *
 * @brief radius exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope the scope to which the node belongs.
 */
openvrml::vrml97_node::spot_light_node::
spot_light_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    abstract_light_node(type, scope),
    attenuation_(*this, vec3f(1.0, 0.0, 0.0)),
    beam_width_(*this, 1.570796f),
    cut_off_angle_(*this, 0.785398f),
    direction_(*this, vec3f(0.0, 0.0, -1.0)),
    location_(*this, vec3f(0.0, 0.0, 0.0)),
    radius_(*this, 100)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::spot_light_node::~spot_light_node() throw ()
{}

/**
 * @brief Cast to a spot_light_node.
 *
 * @return a pointer to the node.
 */
openvrml::vrml97_node::spot_light_node *
openvrml::vrml97_node::spot_light_node::to_spot_light() const
{
    return const_cast<spot_light_node *>(this);
}

/**
 * @brief Render the scoped light.
 *
 * @param viewer    a Viewer.
 *
 * @todo This should be called before rendering any geometry in the scene.
 *      Since this is called from Scene::render before traversing the
 *      scene graph, the proper transformation matrix hasn't been set up.
 *      Somehow it needs to figure out the accumulated xforms of its
 *      parents and apply them before rendering. This is not easy with
 *      DEF/USEd nodes...
 */
void
openvrml::vrml97_node::spot_light_node::renderScoped(openvrml::viewer & viewer)
{
    if (this->on_.sfbool::value && this->radius_.sffloat::value > 0.0) {
        viewer.insert_spot_light(this->ambient_intensity_.sffloat::value,
                                 this->attenuation_.sfvec3f::value,
                                 this->beam_width_.sffloat::value,
                                 this->color_.sfcolor::value,
                                 this->cut_off_angle_.sffloat::value,
                                 this->direction_.sfvec3f::value,
                                 this->intensity_.sffloat::value,
                                 this->location_.sfvec3f::value,
                                 this->radius_.sffloat::value);
    }
    this->node::modified(false);
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::spot_light_node::do_initialize(const double timestamp)
    throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser().add_scoped_light(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void
openvrml::vrml97_node::spot_light_node::do_shutdown(const double timestamp)
    throw ()
{
    assert(this->scene());
    this->scene()->browser().remove_scoped_light(*this);
}


/**
 * @class openvrml::vrml97_node::switch_class
 *
 * @brief Class object for Switch nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::switch_class::switch_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::switch_class::~switch_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Switch nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by switch_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::switch_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                      field_value::mfnode_id,
                      "choice"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sfint32_id,
                      "whichChoice")
    };

    typedef vrml97_node_type_impl<switch_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & switchNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            switchNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    switch_node::choice_exposedfield>(&switch_node::choice_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    switch_node::choice_exposedfield>(&switch_node::choice_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    switch_node::choice_exposedfield>(&switch_node::choice_)));
        } else if (*interface == supportedInterfaces[1]) {
            switchNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    switch_node::which_choice_exposedfield>(
                        &switch_node::which_choice_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    switch_node::which_choice_exposedfield>(
                        &switch_node::which_choice_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    switch_node::which_choice_exposedfield>(
                        &switch_node::which_choice_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::switch_node
 *
 * @brief Switch node instances.
 */

/**
 * @var class openvrml::vrml97_node::switch_node::switch_class
 *
 * @brief Class object for Switch nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::switch_node::choice_exposedfield
 *
 * @brief choice exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  switch_node.
 */
openvrml::vrml97_node::switch_node::choice_exposedfield::
choice_exposedfield(switch_node & node):
    openvrml::exposedfield<openvrml::mfnode>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::switch_node::choice_exposedfield::~choice_exposedfield()
    throw ()
{}

/**
 * @brief Process event.
 *
 * @param choice    choice nodes.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::switch_node::choice_exposedfield::
event_side_effect(const mfnode & choice, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        switch_node & node = dynamic_cast<switch_node &>(this->node());

        const size_t which_choice = size_t(node.which_choice_.sfint32::value);
        assert(!node.children_.value.empty());
        node.children_.value[0] =
            (which_choice >= 0
             && which_choice < node.choice_.mfnode::value.size())
            ? node.choice_.mfnode::value[which_choice]
            : node_ptr(0);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::switch_node::which_choice_exposedfield
 *
 * @brief choice exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  switch_node.
 */
openvrml::vrml97_node::switch_node::which_choice_exposedfield::
which_choice_exposedfield(switch_node & node):
    openvrml::exposedfield<openvrml::sfint32>(node, -1)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::switch_node::which_choice_exposedfield::
~which_choice_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param which_choice  choice nodes.
 * @param timestamp     the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::switch_node::which_choice_exposedfield::
event_side_effect(const sfint32 & which_choice,
                 const double timestamp)
    throw (std::bad_alloc)
{
    try {
        switch_node & node = dynamic_cast<switch_node &>(this->node());

        assert(!node.children_.value.empty());
        node.children_.value[0] =
            (which_choice.value >= 0
             && which_choice.value < int32(node.choice_.mfnode::value.size()))
            ? node.choice_.mfnode::value[which_choice.value]
            : node_ptr(0);
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::switch_node::choice_exposedfield openvrml::vrml97_node::switch_node::choice_
 *
 * @brief choice exposedField.
 */

/**
 * @var openvrml::vrml97_node::switch_node::which_choice_exposedfield openvrml::vrml97_node::switch_node::which_choice_
 *
 * @brief whichChoice exposedField.
 */

/**
 * @var openvrml::mfnode openvrml::vrml97_node::switch_node::children_
 *
 * @brief The children currently in the scene graph.
 */

/**
 * @var openvrml::bounding_sphere openvrml::vrml97_node::switch_node::bsphere
 *
 * @brief Cached copy of the bsphere enclosing this node's children.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the node instance.
 * @param scope     the scope to which the node belongs.
 */
openvrml::vrml97_node::switch_node::
switch_node(const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    abstract_base(type, scope),
    grouping_node(type, scope),
    choice_(*this),
    which_choice_(*this),
    children_(1)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::switch_node::~switch_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::switch_node::modified() const
{
    if (this->node::modified()) { return true; }

    long w = this->which_choice_.sfint32::value;

    return w >= 0 && size_t(w) < this->choice_.mfnode::value.size()
        && this->choice_.mfnode::value[w]->modified();
}

/**
 * @brief Render the node.
 *
 * The child corresponding to @a whichChoice is rendered. Nothing is rendered
 * if @a whichChoice is -1.
 *
 * @param viewer    a Viewer.
 * @param context   a rendering context.
 */
void
openvrml::vrml97_node::switch_node::
do_render_child(openvrml::viewer & viewer, const rendering_context context)
{
    assert(!this->children_.value.empty());
    child_node * const child =
        node_cast<child_node *>(this->children_.value[0].get());
    if (child) { child->render_child(viewer, context); }
    this->node::modified(false);
}

/**
 * @brief Get the bounding volume.
 *
 * @return the bounding volume associated with the node.
 */
const openvrml::bounding_volume &
openvrml::vrml97_node::switch_node::do_bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<switch_node *>(this)->recalcBSphere();
    }
    return this->bsphere;
}

/**
 * @brief Get the children in the scene graph.
 *
 * @return the child nodes in the scene graph.
 */
const std::vector<openvrml::node_ptr> &
openvrml::vrml97_node::switch_node::children() const throw ()
{
    return this->children_.value;
}

/**
 * Pass on to enabled touchsensor child.
 */
void openvrml::vrml97_node::switch_node::activate(double time,
                                                  bool isOver,
                                                  bool isActive,
                                                  double *p)
{
    const std::vector<node_ptr> & children = this->children();
    const node_ptr & node = children[0];
    if (node) {
        if (node->to_touch_sensor() && node->to_touch_sensor()->enabled()) {
            node->to_touch_sensor()->activate(time, isOver, isActive, p);
        } else if (node->to_plane_sensor()
                && node->to_plane_sensor()->enabled()) {
            node->to_plane_sensor()->activate(time, isActive, p);
        } else if (node->to_cylinder_sensor()
                && node->to_cylinder_sensor()->enabled()) {
            node->to_cylinder_sensor()->activate(time, isActive, p);
        } else if (node->to_sphere_sensor()
                && node->to_sphere_sensor()->isEnabled()) {
            node->to_sphere_sensor()->activate(time, isActive, p);
        }
    }
}

/**
 * @brief Recalculate the bounding volume.
 */
void openvrml::vrml97_node::switch_node::recalcBSphere()
{
    this->bsphere = bounding_sphere();
    long w = this->which_choice_.sfint32::value;
    if (w >= 0 && size_t(w) < this->choice_.mfnode::value.size()) {
        const node_ptr & node = this->choice_.mfnode::value[w];
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
 * @class openvrml::vrml97_node::text_class
 *
 * @brief Class object for Text nodes.
 */

/**
 * @var FT_Library openvrml::vrml97_node::text_class::freeTypeLibrary
 *
 * @brief FreeType library handle.
 *
 * @see http://freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Library
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::text_class::text_class(openvrml::browser & browser):
    node_class(browser)
{
# if OPENVRML_ENABLE_RENDER_TEXT_NODE
    FT_Error error = 0;
    error = FT_Init_FreeType(&this->freeTypeLibrary);
    if (error) {
        browser.err << "Error initializing FreeType library." << std::endl;
    }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::text_class::~text_class() throw ()
{
# if OPENVRML_ENABLE_RENDER_TEXT_NODE
    FT_Error error = 0;
    error = FT_Done_FreeType(this->freeTypeLibrary);
    if (error) {
        this->browser().err << "Error shutting down FreeType library."
                            << std::endl;
    }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Text nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by text_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::text_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::mfstring_id,
                       "string"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "fontStyle"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mffloat_id,
                       "length"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "maxExtent")
    };

    typedef vrml97_node_type_impl<text_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & textNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            textNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    text_node::string_exposedfield>(&text_node::string_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<text_node::string_exposedfield>(
                        &text_node::string_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    text_node::string_exposedfield>(&text_node::string_)));
        } else if (*interface == supportedInterfaces[1]) {
            textNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    text_node::font_style_exposedfield>(
                        &text_node::font_style_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    text_node::font_style_exposedfield>(
                        &text_node::font_style_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    text_node::font_style_exposedfield>(
                        &text_node::font_style_)));
        } else if (*interface == supportedInterfaces[2]) {
            textNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    text_node::length_exposedfield>(&text_node::length_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<text_node::length_exposedfield>(
                        &text_node::length_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    text_node::length_exposedfield>(&text_node::length_)));
        } else if (*interface == supportedInterfaces[3]) {
            textNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    text_node::max_extent_exposedfield>(
                        &text_node::max_extent_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    text_node::max_extent_exposedfield>(
                        &text_node::max_extent_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    text_node::max_extent_exposedfield>(
                        &text_node::max_extent_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::text_node
 *
 * @brief Text node instances.
 */

/**
 * @var class openvrml::vrml97_node::text_node::text_class
 *
 * @brief Class object for Text nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::text_node::string_exposedfield
 *
 * @brief string exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  text_node.
 */
openvrml::vrml97_node::text_node::string_exposedfield::
string_exposedfield(text_node & node):
    openvrml::exposedfield<openvrml::mfstring>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::text_node::string_exposedfield::
~string_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param string    text strings.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::text_node::string_exposedfield::
event_side_effect(const mfstring & string, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        text_node & node = dynamic_cast<text_node &>(this->node());
        node.update_ucs4();
        node.update_geometry();
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::text_node::font_style_exposedfield
 *
 * @brief fontStyle exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  text_node.
 */
openvrml::vrml97_node::text_node::font_style_exposedfield::
font_style_exposedfield(text_node & node):
    openvrml::exposedfield<openvrml::sfnode>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::text_node::font_style_exposedfield::
~font_style_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param font_style    text strings.
 * @param timestamp     the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::text_node::font_style_exposedfield::
event_side_effect(const sfnode & font_style, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        text_node & node = dynamic_cast<text_node &>(this->node());
        node.update_ucs4();
        node.update_geometry();
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::text_node::length_exposedfield
 *
 * @brief length exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  text_node.
 */
openvrml::vrml97_node::text_node::length_exposedfield::
length_exposedfield(text_node & node):
    openvrml::exposedfield<openvrml::mffloat>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::text_node::length_exposedfield::~length_exposedfield()
    throw ()
{}

/**
 * @brief Process event.
 *
 * @param length    length of the text strings.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::text_node::length_exposedfield::
event_side_effect(const mffloat & length, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        text_node & node = dynamic_cast<text_node &>(this->node());
        node.update_geometry();
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::text_node::max_extent_exposedfield
 *
 * @brief maxExtent exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  text_node.
 */
openvrml::vrml97_node::text_node::max_extent_exposedfield::
max_extent_exposedfield(text_node & node):
    openvrml::exposedfield<openvrml::sffloat>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::text_node::max_extent_exposedfield::
~max_extent_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param max_extent    maximum extent of the text strings.
 * @param timestamp     the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::text_node::max_extent_exposedfield::
event_side_effect(const sffloat & max_extent,
                  const double timestamp)
    throw (std::bad_alloc)
{
    try {
        text_node & node = dynamic_cast<text_node &>(this->node());
        node.update_geometry();
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::text_node::string_exposedfield openvrml::vrml97_node::text_node::string_
 *
 * @brief string exposedField.
 */

/**
 * @var openvrml::vrml97_node::text_node::font_style_exposedfield openvrml::vrml97_node::text_node::font_style_
 *
 * @brief fontStyle exposedField.
 */

/**
 * @var openvrml::vrml97_node::text_node::length_exposedfield openvrml::vrml97_node::text_node::length_
 *
 * @brief length exposedField.
 */

/**
 * @var openvrml::vrml97_node::text_node::max_extent_exposedfield openvrml::vrml97_node::text_node::max_extent_
 *
 * @brief maxExtent exposedField.
 */

/**
 * @internal
 *
 * @struct openvrml::vrml97_node::text_node::glyph_geometry
 *
 * @brief Used to hold the geometry of individual glyphs.
 */

/**
 * @var std::vector<openvrml::vec2f> openvrml::vrml97_node::text_node::glyph_geometry::coord
 *
 * @brief Glyph coordinates.
 */

/**
 * @var std::vector<openvrml::int32> openvrml::vrml97_node::text_node::glyph_geometry::coord_index
 *
 * @brief Glyph coordinate indices.
 */

/**
 * @var float openvrml::vrml97_node::text_node::glyph_geometry::advance_width
 *
 * @brief The distance the pen should advance horizontally after drawing the
 *      glyph.
 */

/**
 * @var float openvrml::vrml97_node::text_node::glyph_geometry::advance_height
 *
 * @brief The distance the pen should advance vertically after drawing the
 *      glyph.
 */

# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
namespace {

    /**
     * @internal
     *
     * @brief Determine whether three vertices are ordered counter-clockwise.
     *
     * Does not throw.
     *
     * @param p0 first vertex.
     * @param p1 second vertex.
     * @param p2 third vertex.
     *
     * @return 1 if the vertices are counter-clockwise, -1 if the vertices are
     *         clockwise, or 0 if the vertices are neither.
     */
    int ccw_(const openvrml::vec2f & p0,
             const openvrml::vec2f & p1,
             const openvrml::vec2f & p2) throw ()
    {
        const float dx1 = p1.x() - p0.x();
        const float dy1 = p1.y() - p0.y();
        const float dx2 = p2.x() - p0.x();
        const float dy2 = p2.y() - p0.y();

        if (dx1 * dy2 > dy1 * dx2) { return 1; }
        if (dx1 * dy2 < dy1 * dx2) { return -1; }
        if ((dx1 * dx2 < 0) || (dy1 * dy2 < 0)) return -1;
        if ((dx1 * dx1 + dy1 * dy1) < (dx2 * dx2 + dy2 * dy2)) { return 1; }
        return 0;
    }

    /**
     * @internal
     *
     * @brief Determine whether two line segments intersect.
     *
     * Does not throw.
     *
     * @param l0p0 first point of the first line.
     * @param l0p1 second point of the first line.
     * @param l1p0 first point of the second line.
     * @param l1p1 second point of the second line.
     *
     * @return @c true if the line segments intersect; @c false otherwise.
     */
    bool intersect_(const openvrml::vec2f & l0p0, const openvrml::vec2f & l0p1,
                    const openvrml::vec2f & l1p0, const openvrml::vec2f & l1p1)
        throw ()
    {
        return ccw_(l0p0, l0p1, l1p0) * ccw_(l0p0, l0p1, l1p1) <= 0
            && ccw_(l1p0, l1p1, l0p0) * ccw_(l1p0, l1p1, l0p1) <= 0;
    }

    /**
     * @brief Determine whether a line segment intersects any line segments
     *        in a contour.
     *
     * Does not throw.
     *
     * @param v0      initial vertex of the line segment.
     * @param v1      final vertex of the line segment.
     * @param contour a contour (a series of line segments).
     *
     * @return @c true if the line segment defined by (@p v0, @p v1)
     *         intersects any line segment in @p contour; @c false otherwise.
     */
    bool
    intersects_segment_in_contour(const openvrml::vec2f & v0,
                                  const openvrml::vec2f & v1,
                                  const std::vector<openvrml::vec2f> & contour)
        throw ()
    {
        for (size_t j = 0; j < contour.size() - 1; ++j) {
            using openvrml::vec2f;
            //
            // Endpoints of the segment to test for intersection.
            //
            const vec2f & contour_v0 = contour[j];
            const vec2f & contour_v1 = contour[j + 1];
            //
            // We don't care if the endpoints match (and the intersection
            // test will treat this as an intersection).
            //
            if (contour_v0 == v0 || contour_v0 == v1
                || contour_v1 == v0 || contour_v1 == v1) { continue; }
            if (intersect_(v0, v1, contour_v0, contour_v1)) { return true; }
        }
        return false;
    }

    /**
     * @internal
     *
     * @brief Get the exterior vertext that should be used to connect to the
     *      interior contour.
     *
     * Finds the first vertex in @p exteriorContour such that a line segment
     * from the interior contour vertex at @p interiorIndex through the
     * exterior contour vertex does not cross @p interiorContour.
     *
     * Does not throw.
     *
     * @param exterior_contour the exterior contour.
     * @param interior_contour the interior contour.
     * @param interior_index   the index of a vertex in @p interiorContour to
     *                         be used as the interior connecting vertex.
     *
     * @return the index of a vertex in @p exteriorContour that is usable as
     *         the exterior connecting vertex, or -1 if no such vertex is
     *         found.
     */
    long get_exterior_connecting_vertex_index_(
        const std::vector<openvrml::vec2f> & exterior_contour,
        const std::vector<const std::vector<openvrml::vec2f> *> &
            interior_contours,
        const openvrml::vec2f & interior_vertex)
        throw ()
    {
        assert(exterior_contour.size() > 1);
        assert(!interior_contours.empty());

        using openvrml::vec2f;

        typedef std::vector<const std::vector<vec2f> *> interior_contours_type;

        for (size_t i = 0; i < exterior_contour.size(); ++i) {
            const vec2f & exterior_vertex = exterior_contour[i];
            bool intersects_interior = false;
            for (interior_contours_type::const_iterator interior_contour =
                     interior_contours.begin();
                 interior_contour != interior_contours.end()
                     && !intersects_interior;
                 ++interior_contour) {
                assert(*interior_contour);
                if (intersects_segment_in_contour(interior_vertex,
                                                  exterior_vertex,
                                                  **interior_contour)) {
                    intersects_interior = true;
                }
            }
            if (!intersects_interior
                && !intersects_segment_in_contour(interior_vertex,
                                                  exterior_vertex,
                                                  exterior_contour)) {
                return i;
            }
        }
        return -1;
    }

    bool inside_contour_(const std::vector<openvrml::vec2f> & contour,
                         const openvrml::vec2f & point)
        throw ()
    {
        using openvrml::vec2f;

        bool result = false;
        const size_t nvert = contour.size();
        for (size_t i = 0, j = nvert - 1; i < nvert; j = i++) {
            const vec2f & vi = contour[i];
            const vec2f & vj = contour[j];
            if ((((vi.y() <= point.y()) && (point.y() < vj.y()))
                        || ((vj.y() <= point.y()) && (point.y() < vi.y())))
                    && (point.x() < (vj.x() - vi.x())
                        * (point.y() - vi.y()) / (vj.y() - vi.y()) + vi.x())) {
                result = !result;
            }
        }
        return result;
    }

    enum contour_type_ { exterior_, interior_ };

    contour_type_
    get_type_(const std::vector<openvrml::vec2f> & contour,
              const std::vector<std::vector<openvrml::vec2f> > & contours)
        throw ()
    {
        using std::vector;

        assert(!contour.empty());
        const openvrml::vec2f & vertex = contour[0];

        bool is_interior = false;
        for (vector<vector<openvrml::vec2f> >::const_iterator test_contour =
                 contours.begin();
             test_contour != contours.end();
             ++test_contour) {
            if (&*test_contour == &contour) { continue; }
            if (inside_contour_(*test_contour, vertex)) {
                is_interior = !is_interior;
            }
        }
        return is_interior
            ? interior_
            : exterior_;
    }

    struct polygon_ {
        const std::vector<openvrml::vec2f> * exterior;
        std::vector<const std::vector<openvrml::vec2f> *> interiors;
    };

    struct inside_ : std::binary_function<const std::vector<openvrml::vec2f> *,
                                          const std::vector<openvrml::vec2f> *,
                                          bool> {
        bool operator()(const std::vector<openvrml::vec2f> * const lhs,
                        const std::vector<openvrml::vec2f> * const rhs) const
        {
            assert(lhs);
            assert(rhs);
            assert(!lhs->empty());
            //
            // Assume contours don't intersect. So if one point on lhs is
            // inside rhs, then assume all of lhs is inside rhs.
            //
            return inside_contour_(*rhs, lhs->front());
        }
    };

    const std::vector<polygon_>
    get_polygons_(const std::vector<std::vector<openvrml::vec2f> > & contours)
        throw (std::bad_alloc)
    {
        using std::vector;
        using openvrml::vec2f;
        typedef std::multiset<const vector<vec2f> *, inside_>
            segregated_contours;

        //
        // First, divide the contours into interior and exterior contours.
        //
        segregated_contours interiors, exteriors;
        for (vector<vector<vec2f> >::const_iterator contour = contours.begin();
             contour != contours.end();
             ++contour) {
            switch (get_type_(*contour, contours)) {
            case interior_:
                interiors.insert(&*contour);
                break;
            case exterior_:
                exteriors.insert(&*contour);
                break;
            default:
                assert(false);
            }
        }

        //
        // For each exterior, find its associated interiors and group them in
        // a Polygon_.
        //
        vector<polygon_> polygons;
        while (!exteriors.empty()) {
            polygon_ polygon;
            polygon.exterior = *exteriors.begin();
            segregated_contours::iterator interior = interiors.begin();
            while (interior != interiors.end()) {
                assert(!(*interior)->empty());
                if (inside_contour_(*polygon.exterior, (*interior)->front())) {
                    polygon.interiors.push_back(*interior);
                    segregated_contours::iterator next = interior;
                    ++next;
                    interiors.erase(interior);
                    interior = next;
                } else {
                    ++interior;
                }
            }
            polygons.push_back(polygon);
            exteriors.erase(exteriors.begin());
        }
        return polygons;
    }

    long get_vertex_index_(const std::vector<openvrml::vec2f> & vertices,
                           const openvrml::vec2f & vertex)
        throw ()
    {
        for (size_t i = 0; i < vertices.size(); ++i) {
            if (vertices[i] == vertex) { return i; }
        }
        return -1;
    }
}
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE

/**
 * @brief Construct from a set of contours.
 *
 * @param contours          a vector of closed contours that make up the
 *                          glyph's outline.
 * @param advance_width     the distance the pen should advance horizontally
 *                          after drawing the glyph.
 * @param advance_height    the distance the pen should advance vertically
 *                          after drawing the glyph.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::vrml97_node::text_node::glyph_geometry::
glyph_geometry(const std::vector<std::vector<openvrml::vec2f> > & contours,
               const float advance_width,
               const float advance_height)
    throw (std::bad_alloc):
    advance_width(advance_width),
    advance_height(advance_height)
{
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
    using std::vector;
    using openvrml::vec2f;

    const vector<polygon_> polygons = get_polygons_(contours);
    for (vector<polygon_>::const_iterator polygon = polygons.begin();
         polygon != polygons.end();
         ++polygon) {
        //
        // connectionMap is keyed on a pointer to a vertex on the exterior
        // contour, and maps to a pointer to the interior contour whose
        // first vertex is closest to the exterior vertex.
        //
        typedef std::multimap<const vec2f *, const std::vector<vec2f> *>
            connection_map_t;
        connection_map_t connection_map;

        //
        // Fill connection_map. For each interior contour, find the exterior
        // vertex that is closest to the first vertex in the interior contour,
        // and the put the pair in the map.
        //
        for (vector<const vector<vec2f> *>::const_iterator interior =
                 polygon->interiors.begin();
             interior != polygon->interiors.end();
             ++interior) {
            assert(*interior);
            assert(!(*interior)->empty());
            long exterior_vertex_index =
                get_exterior_connecting_vertex_index_(*polygon->exterior,
                                                      polygon->interiors,
                                                      (*interior)->front());
            assert(exterior_vertex_index > -1);
            const vec2f * const exterior_vertex =
                    &(*polygon->exterior)[exterior_vertex_index];
            assert(exterior_vertex);
            const connection_map_t::value_type value(exterior_vertex,
                                                     *interior);
            connection_map.insert(value);
        }

        //
        // Finally, draw the polygon.
        //
        assert(!polygon->exterior->empty());
        for (size_t i = 0; i < polygon->exterior->size(); ++i) {
            const vec2f & exterior_vertex = (*polygon->exterior)[i];
            long exterior_index = get_vertex_index_(this->coord,
                                                    exterior_vertex);
            if (exterior_index > -1) {
                this->coord_index.push_back(exterior_index);
            } else {
                this->coord.push_back(exterior_vertex);
                assert(!this->coord.empty());
                exterior_index = this->coord.size() - 1;
                this->coord_index.push_back(exterior_index);
            }
            connection_map_t::iterator pos;
            while ((pos = connection_map.find(&exterior_vertex))
                    != connection_map.end()) {
                for (int i = pos->second->size() - 1; i > -1; --i) {
                    const vec2f & interior_vertex = (*pos->second)[i];
                    const long interior_index =
                        get_vertex_index_(this->coord, interior_vertex);
                    if (interior_index > -1) {
                        this->coord_index.push_back(interior_index);
                    } else {
                        this->coord.push_back(interior_vertex);
                        assert(!this->coord.empty());
                        this->coord_index.push_back(this->coord.size() - 1);
                    }
                }
                this->coord_index.push_back(exterior_index);
                connection_map.erase(pos);
            }
        }
        assert(connection_map.empty());
        this->coord_index.push_back(-1);
    }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
}

/**
 * @internal
 *
 * @struct openvrml::vrml97_node::text_node::text_geometry
 *
 * @brief Holds the text geometry.
 */

/**
 * @var std::vector<openvrml::vec3f> openvrml::vrml97_node::text_node::text_geometry::coord
 *
 * @brief Text geometry coordinates.
 */

/**
 * @var std::vector<openvrml::int32> openvrml::vrml97_node::text_node::text_geometry::coord_index
 *
 * @brief Text geometry coordinate indices.
 */

/**
 * @var std::vector<openvrml::vec3f> openvrml::vrml97_node::text_node::text_geometry::normal
 *
 * @brief Text geometry normals.
 */

/**
 * @var std::vector<openvrml::vec2f> openvrml::vrml97_node::text_node::text_geometry::tex_coord
 *
 * @brief Text geometry texture coordinates.
 */

/**
 * @typedef openvrml::vrml97_node::text_node::ucs4_string_t
 *
 * @brief A vector of FcChar32 vectors.
 */

/**
 * @typedef openvrml::vrml97_node::text_node::glyph_geometry_map_t
 *
 * @brief Maps FT_UInts to glyph_geometry.
 *
 * @see http://freetype.org/freetype2/docs/reference/ft2-basic_types.html#FT_UInt
 */

/**
 * @var openvrml::vrml97_node::text_node::ucs4_string_t openvrml::vrml97_node::text_node::ucs4_string
 *
 * @brief UCS-4 equivalent of the (UTF-8) data in @a string.
 */

/**
 * @var FT_Face openvrml::vrml97_node::text_node::face
 *
 * @brief Handle to the font face.
 *
 * @see http://freetype.org/freetype2/docs/reference/ft2-base_interface.html#FT_Face
 */

/**
 * @var openvrml::vrml97_node::text_node::glyph_geometry_map_t openvrml::vrml97_node::text_node::glyph_geometry_map
 *
 * @brief Map of glyph indices to glyph_geometry.
 *
 * glyph_geometry instances are created as needed as new glyphs are
 * encountered. Once they are created, they are cached in the
 * glyph_geometry_map for rapid retrieval the next time the glyph is
 * encountered.
 */

/**
 * @var openvrml::vrml97_node::text_node::text_geometry openvrml::vrml97_node::text_node::text_geometry_
 *
 * @brief The text geometry.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope that the new node will belong to.
 */
openvrml::vrml97_node::text_node::
text_node(const node_type & type,
          const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    geometry_node(type, scope),
    string_(*this),
    font_style_(*this),
    length_(*this),
    max_extent_(*this),
    face(0)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::text_node::~text_node() throw ()
{}

/**
 * @brief Determine whether the node has been modified.
 *
 * @return @c true if the node or one of its children has been modified,
 *      @c false otherwise.
 */
bool openvrml::vrml97_node::text_node::modified() const
{
    return this->node::modified()
        || (this->font_style_.sfnode::value
            && this->font_style_.sfnode::value->modified());
}

/**
 * @brief Insert this geometry into @p viewer's display list.
 *
 * @param viewer    a Viewer.
 * @param context   the rendering context.
 */
openvrml::viewer::object_t
openvrml::vrml97_node::text_node::
do_render_geometry(openvrml::viewer & viewer, const rendering_context context)
{
    const viewer::object_t retval =
        viewer.insert_shell(viewer::mask_ccw,
                            this->text_geometry_.coord,
                            this->text_geometry_.coord_index,
                            std::vector<openvrml::color>(), // color
                            std::vector<int32>(), // colorIndex
                            this->text_geometry_.normal,
                            std::vector<int32>(), // normalIndex
                            this->text_geometry_.tex_coord,
                            std::vector<int32>()); // texCoordIndex
    if (this->font_style_.sfnode::value) {
        this->font_style_.sfnode::value->modified(false);
    }

    return retval;
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::vrml97_node::text_node::do_initialize(const double timestamp)
    throw (std::bad_alloc)
{
    this->update_ucs4();
    this->update_face();
    this->update_geometry();
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void openvrml::vrml97_node::text_node::do_shutdown(const double timestamp)
    throw ()
{
# if OPENVRML_ENABLE_RENDER_TEXT_NODE
    if (this->face) {
        FT_Error ftError = FT_Done_Face(this->face);
        assert(ftError == FT_Err_Ok); // Surely this can't fail.
    }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
}

/**
 * @brief Called when @a string changes to update the UCS-4 text.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::vrml97_node::text_node::update_ucs4() throw (std::bad_alloc)
{
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
    this->ucs4_string.clear();
    this->ucs4_string.resize(this->string_.mfstring::value.size());

    for (size_t i = 0; i < this->string_.mfstring::value.size(); ++i) {
        using std::string;
        using std::vector;

        const string & element = this->string_.mfstring::value[i];

        vector<FcChar32> & ucs4Element = this->ucs4_string[i];

        //
        // First, we need to convert the characters from UTF-8 to UCS-4.
        //
        vector<FcChar8> utf8String(element.begin(), element.end());
        int nchar = 0, wchar = 0;
        FcBool well_formed =
            FcUtf8Len(&utf8String[0], utf8String.size(), &nchar, &wchar);
        if (well_formed) {
            ucs4Element.resize(nchar);
            vector<FcChar8>::iterator utf8interface = utf8String.begin();
            vector<FcChar32>::iterator ucs4interface = ucs4Element.begin();
            while (utf8interface != utf8String.end()) {
                const int utf8bytes =
                    FcUtf8ToUcs4(&*utf8interface, &*ucs4interface,
                                 utf8String.end() - utf8interface);
                utf8interface += utf8bytes;
                ucs4interface++;
            }
        }
    }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
}

# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
namespace {

    //
    // FcChar8_traits is a model of the standard library Character Traits
    // concept.
    //
    struct FcChar8_traits {
        typedef unsigned char char_type;
        typedef int int_type;
        typedef std::streampos pos_type;
        typedef std::streamoff off_type;
        typedef mbstate_t state_type;

        static void assign(char_type & c1, const char_type & c2);
        static bool eq(const char_type & c1, const char_type & c2);
        static bool lt(const char_type & c1, const char_type & c2);
        static int compare(const char_type * s1, const char_type * s2,
                           size_t n);
        static size_t length(const char_type * s);
        static const char_type * find(const char_type * s, size_t n,
                                      const char_type & a);
        static char_type * move(char_type * s1, const char_type * s2,
                                size_t n);
        static char_type * copy(char_type * s1, const char_type * s2,
                                size_t n);
        static char_type * assign(char_type * s, size_t n, char_type a);
        static int_type eof();
        static int_type not_eof(const int_type & c);
        static char_type to_char_type(const int_type & e);
        static int_type to_int_type(const char_type & c);
        static bool eq_int_type(const int_type & e1, const int_type & e2);
    };

    inline void FcChar8_traits::assign(char_type & c1, const char_type & c2)
    {
        c1 = c2;
    }

    inline bool FcChar8_traits::eq(const char_type & c1, const char_type & c2)
    {
        return c1 == c2;
    }

    inline bool FcChar8_traits::lt(const char_type & c1, const char_type & c2)
    {
        return c1 < c2;
    }

    inline int FcChar8_traits::compare(const char_type * s1,
                                       const char_type * s2,
                                       size_t n)
    {
        for (size_t i = 0; i < n; ++i) {
            if (!eq(s1[i], s2[i])) { return lt(s1[i], s2[i]) ? -1 : 1; }
        }
        return 0;
    }

    inline size_t FcChar8_traits::length(const char_type * s)
    {
        const char_type * p = s;
        while (*p) { ++p; }
        return (p - s);
    }

    inline FcChar8_traits::char_type *
    FcChar8_traits::move(char_type * s1, const char_type * s2, size_t n)
    {
        return reinterpret_cast<char_type *>(
            memmove(s1, s2, n * sizeof(char_type)));
    }

    inline const FcChar8_traits::char_type *
    FcChar8_traits::find(const char_type * s, size_t n, const char_type & a)
    {
        for (const char_type * p = s; size_t(p - s) < n; ++p) {
            if (*p == a) { return p; }
        }
        return 0;
    }

    inline FcChar8_traits::char_type *
    FcChar8_traits::copy(char_type * s1, const char_type * s2, size_t n)
    {
        return reinterpret_cast<char_type *>(
            memcpy(s1, s2, n * sizeof(char_type)));
    }

    inline FcChar8_traits::char_type *
    FcChar8_traits::assign(char_type * s, size_t n, char_type a)
    {
        for (char_type * p = s; p < s + n; ++p) { assign(*p, a); }
        return s;
    }

    inline FcChar8_traits::int_type FcChar8_traits::eof()
    {
        return static_cast<int_type>(-1);
    }

    inline FcChar8_traits::int_type FcChar8_traits::not_eof(const int_type & c)
    {
        return eq_int_type(c, eof()) ? int_type(0) : c;
    }

    inline FcChar8_traits::char_type
    FcChar8_traits::to_char_type(const int_type & e)
    {
        return char_type(e);
    }

    inline FcChar8_traits::int_type
    FcChar8_traits::to_int_type(const char_type & c)
    {
        return int_type(c);
    }

    inline bool FcChar8_traits::eq_int_type(const int_type & e1,
                                            const int_type & e2)
    {
        return e1 == e2;
    }
}
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE

/**
 * @brief Called when @a fontStyle changes to update the font face.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::vrml97_node::text_node::update_face() throw (std::bad_alloc)
{
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
    static const char * const fcResultMessage[] = { "match",
                                                    "no match",
                                                    "type mismatch",
                                                    "no id" };

    class FontconfigError : public std::runtime_error {
    public:
        explicit FontconfigError(const FcResult result):
            std::runtime_error(fcResultMessage[result])
        {}

        virtual ~FontconfigError() throw ()
        {}
    };

    class FreeTypeError : public std::runtime_error {
    public:
        //
        // The normal build of FreeType doesn't include a means of mapping
        // error codes to human-readable strings.  There's a means of letting
        // client apps do this by defining some macros, but that's too much
        // trouble for now.
        //
        explicit FreeTypeError(const FT_Error error):
            std::runtime_error("FreeType error.")
        {}

        virtual ~FreeTypeError() throw ()
        {}
    };

    using std::string;
    typedef std::basic_string<FcChar8, FcChar8_traits> FcChar8_string;

    FcChar8_string language;

    std::vector<string> family;
    family.push_back("SERIF");

    string style;

    openvrml::font_style_node * const fontStyle =
        node_cast<openvrml::font_style_node *>(
            this->font_style_.sfnode::value.get());
    if (fontStyle) {
        if (!fontStyle->family().empty()) {
            family = fontStyle->family();
            style = fontStyle->style();
            language.assign(fontStyle->language().begin(),
                            fontStyle->language().end());
        }
    }

    try {
        FcPattern * initialPattern = 0;
        FcPattern * matchedPattern = 0;

        try {
            using std::vector;

            string fontName;
            //
            // Set the family.
            //
            for (size_t i = 0; i < family.size(); ++i) {
                const std::string & element = family[i];
                if (element == "SERIF") {
                    fontName += "serif";
                } else if (element == "SANS") {
                    fontName += "sans";
                } else if (element == "TYPEWRITER") {
                    fontName += "monospace";
                } else {
                    fontName += element;
                }
                if (i + 1 < family.size()) { fontName += ", "; }
            }

            //
            // Set the weight.
            //
            if (style.find("BOLD") != string::npos) {
                fontName += ":bold";
            }

            //
            // Set the slant.
            //
            if (style.find("ITALIC") != string::npos) {
                fontName += ":italic";
            }

            //
            // For now, at least, we only want outline fonts.
            //
            fontName += ":outline=True";

            initialPattern =
                FcNameParse(FcChar8_string(fontName.begin(),
                                           fontName.end()).c_str());
            if (!initialPattern) { throw std::bad_alloc(); }

            //
            // Set the language.
            //
            if (!language.empty()) {
                FcPatternAddString(initialPattern, FC_LANG, language.c_str());
            }

            FcConfigSubstitute(0, initialPattern, FcMatchPattern);
            FcDefaultSubstitute(initialPattern);

            FcResult result = FcResultMatch;
            matchedPattern = FcFontMatch(0, initialPattern, &result);
            if (result != FcResultMatch) { throw FontconfigError(result); }
            assert(matchedPattern);

            FcChar8 * filename = 0;
            result = FcPatternGetString(matchedPattern, FC_FILE, 0, &filename);
            if (result != FcResultMatch) { throw FontconfigError(result); }

            int id = 0;
            result = FcPatternGetInteger(matchedPattern, FC_INDEX, 0, &id);
            if (result != FcResultMatch) { throw FontconfigError(result); }

            text_class & nodeClass =
                const_cast<text_class &>(
                    static_cast<const text_class &>(
                        this->type().node_class()));

            size_t filenameLen = 0;
            for (; filename[filenameLen]; ++filenameLen) {}

            const vector<char> ftFilename(filename,
                                          filename + filenameLen + 1);

            FT_Face newFace = 0;
            FT_Error ftError = FT_Err_Ok;
            ftError = FT_New_Face(nodeClass.freeTypeLibrary,
                                  &ftFilename[0], id, &newFace);
            if (ftError) { throw FreeTypeError(ftError); }

            if (this->face) {
                ftError = FT_Done_Face(this->face);
                assert(ftError == FT_Err_Ok); // Surely this can't fail.
            }

            this->face = newFace;
            this->glyph_geometry_map.clear();

            FcPatternDestroy(initialPattern);
            FcPatternDestroy(matchedPattern);
        } catch (std::runtime_error & ex) {
            FcPatternDestroy(initialPattern);
            FcPatternDestroy(matchedPattern);
            throw;
        }
    } catch (std::bad_alloc & ex) {
        throw;
    } catch (FontconfigError & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    } catch (FreeTypeError & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
}

# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
namespace {

    struct GlyphContours_ {
        const float scale;
        std::vector<std::vector<openvrml::vec2f> > contours;

        explicit GlyphContours_(float scale);
    };

    GlyphContours_::GlyphContours_(const float scale):
        scale(scale)
    {}

    const float stepSize_ = 0.2;

    int moveTo_(FT_Vector * const to, void * const user) throw ()
    {
        using std::vector;
        using openvrml::vec2f;
        assert(user);
        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);
        try {
            c.contours.push_back(vector<vec2f>(1));
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        const vec2f vertex(to->x * c.scale, to->y * c.scale);
        c.contours.back().front() = vertex;
        return 0;
    }

    int lineTo_(FT_Vector * const to, void * const user) throw ()
    {
        assert(user);
        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);
        const openvrml::vec2f vertex(to->x * c.scale, to->y * c.scale);
        try {
            c.contours.back().push_back(vertex);
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        return 0;
    }

    /**
     * @brief de Casteljau's algorithm.
     *
     * This is a nice recursive algorithm defined by de-Casteljau which
     * calculates for a given control polygon the point that lies on the bezier
     * curve for any value of t, and can be used to evaluate and draw the
     * Bezier spline without using the Bernstein polynomials.
     *
     * The algorithm advances by creating in each step a polygons of degree one
     * less than the one created in the previous step until there is only one
     * point left, which is the point on the curve. The polygon vertices for
     * each step are defined by linear interpolation of two consecutive
     * vertices of the polygon from the previous step with a value of t (the
     * parameter):
     *
     * @param buffer    an array including the control points for the curve in
     *                  the first @p npoints elements. The total size of the
     *                  array must be @p npoints * @p npoints. The remaining
     *                  elements of the array will be used by the algorithm to
     *                  store temporary values.
     * @param npoints   the number of control points.
     * @param contour   the points on the curve are added to this array.
     *
     * @exception std::bad_alloc    if memory allocation fails.
     */
    void evaluateCurve_(openvrml::vec2f * const buffer,
                        const size_t npoints,
                        std::vector<openvrml::vec2f> & contour)
        throw (std::bad_alloc)
    {
        for (size_t i = 1; i <= (1 / stepSize_); i++){
            const float t = i * stepSize_; // Parametric points 0 <= t <= 1
            for (size_t j = 1; j < npoints; j++) {
                for (size_t k = 0; k < (npoints - j); k++) {
                    openvrml::vec2f & element = buffer[j * npoints + k];
                    element.x((1 - t) * buffer[(j - 1) * npoints + k][0]
                              + t * buffer[(j - 1) * npoints + k + 1][0]);
                    element.y((1 - t) * buffer[(j - 1) * npoints + k][1]
                              + t * buffer[(j - 1) * npoints + k + 1][1]);
                }
            }
            //
            // Specify next vertex to be included on curve
            //
            contour.push_back(buffer[(npoints - 1) * npoints]); // throws std::bad_alloc
        }
    }

    int conicTo_(FT_Vector * const control,
                 FT_Vector * const to,
                 void * const user)
        throw ()
    {
        using std::vector;
        using openvrml::vec2f;

        assert(control);
        assert(to);
        assert(user);

        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);

        assert(!c.contours.empty());
        vector<vec2f> & contour = c.contours.back();
        const vec2f & lastVertex = contour[contour.size() - 1];

        assert(!contour.empty());
        const size_t npoints = 3;
        vec2f buffer[npoints * npoints] = {
            vec2f(lastVertex[0], lastVertex[1]),
            vec2f(control->x * c.scale, control->y * c.scale),
            vec2f(to->x * c.scale, to->y * c.scale)
        };

        try {
            evaluateCurve_(buffer, npoints, contour);
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        return 0;
    }

    int cubicTo_(FT_Vector * const control1,
                 FT_Vector * const control2,
                 FT_Vector * const to,
                 void * const user)
        throw ()
    {
        using std::vector;
        using openvrml::vec2f;

        assert(control1);
        assert(control2);
        assert(to);
        assert(user);

        GlyphContours_ & c = *static_cast<GlyphContours_ *>(user);

        assert(!c.contours.empty());
        vector<vec2f> & contour = c.contours.back();
        assert(!contour.empty());
        const vec2f & lastVertex = contour.back();

        static const size_t npoints = 4;
        vec2f buffer[npoints * npoints] = {
            vec2f(lastVertex[0], lastVertex[1]),
            vec2f(control1->x * c.scale, control1->y * c.scale),
            vec2f(control2->x * c.scale, control2->y * c.scale),
            vec2f(to->x * c.scale, to->y * c.scale)
        };

        try {
            evaluateCurve_(buffer, npoints, contour);
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return FT_Err_Out_Of_Memory;
        }
        return 0;
    }
}
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE

/**
 * @brief Called to update @a text_geometry.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::vrml97_node::text_node::update_geometry() throw (std::bad_alloc)
{
# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
    using std::pair;
    using std::string;
    using std::vector;

    bool horizontal = true;
    string justify[2] = { "BEGIN", "FIRST" };
    bool leftToRight = true;
    bool topToBottom = true;
    float size = 1.0;
    float spacing = 1.0;
    openvrml::font_style_node * fontStyle =
        node_cast<openvrml::font_style_node *>(
            this->font_style_.sfnode::value.get());
    if (fontStyle) {
        horizontal = fontStyle->horizontal();
        if (!fontStyle->justify().empty()) {
            justify[0] = fontStyle->justify()[0];
        }
        if (fontStyle->justify().size() > 1) {
            justify[1] = fontStyle->justify()[1];
        }
        leftToRight = fontStyle->left_to_right();
        topToBottom = fontStyle->top_to_bottom();
        size = fontStyle->size();
        spacing = fontStyle->spacing();
    }

    text_geometry newGeometry;
    float geometryXMin = 0.0, geometryXMax = 0.0;
    float geometryYMin = 0.0, geometryYMax = 0.0;
    size_t npolygons = 0;
    const ucs4_string_t::const_iterator stringBegin =
        this->ucs4_string.begin();
    for (ucs4_string_t::const_iterator string = stringBegin;
         string != this->ucs4_string.end();
         ++string) {
        float penPos[2] = { 0.0, 0.0 };
        const size_t line = std::distance(stringBegin, string);
        const float lineAdvance = size * spacing * line;
        if (horizontal) {
            if (topToBottom) {
                penPos[1] -= lineAdvance;
            } else {
                penPos[1] += lineAdvance;
            }
        } else {
            if (leftToRight) {
                penPos[0] += lineAdvance;
            } else {
                penPos[0] -= lineAdvance;
            }
        }

        struct LineGeometry {
            vector<vec2f> coord;
            vector<int32> coordIndex;
            float xMin, xMax;
            float yMin, yMax;
        };

        LineGeometry lineGeometry = {};
        for (vector<FcChar32>::const_iterator character = string->begin();
                character != string->end(); ++character) {
            assert(this->face);
            const FT_UInt glyphIndex =
                    FcFreeTypeCharIndex(this->face, *character);

            const glyph_geometry * glyphGeometry = 0;
            const glyph_geometry_map_t::iterator pos =
                    this->glyph_geometry_map.find(glyphIndex);
            if (pos != this->glyph_geometry_map.end()) {
                glyphGeometry = &pos->second;
            } else {
                FT_Error error = FT_Err_Ok;
                error = FT_Load_Glyph(this->face,
                                      glyphIndex,
                                      FT_LOAD_NO_SCALE);
                assert(error == FT_Err_Ok);
                FT_Glyph glyph;
                error = FT_Get_Glyph(this->face->glyph, &glyph);
                assert(error == FT_Err_Ok);
                static FT_Outline_Funcs outlineFuncs = { moveTo_,
                                                         lineTo_,
                                                         conicTo_,
                                                         cubicTo_,
                                                         0,
                                                         0 };
                const float glyphScale = (this->face->bbox.yMax > 0.0)
                                       ? size / this->face->bbox.yMax
                                       : size;
                GlyphContours_ glyphContours(glyphScale);
                assert(glyph->format == ft_glyph_format_outline);
                const FT_OutlineGlyph outlineGlyph =
                        reinterpret_cast<FT_OutlineGlyph>(glyph);
                error = FT_Outline_Decompose(&outlineGlyph->outline,
                                             &outlineFuncs,
                                             &glyphContours);
                assert(error == FT_Err_Ok);

                assert(this->face->glyph);
                const float advanceWidth =
                        FT_HAS_HORIZONTAL(this->face)
                        ? this->face->glyph->metrics.horiAdvance * glyphScale
                        : 0.0;
                const float advanceHeight =
                        FT_HAS_VERTICAL(this->face)
                        ? this->face->glyph->metrics.vertAdvance * glyphScale
                        : 0.0;

                const glyph_geometry_map_t::value_type
                        value(glyphIndex,
                              glyph_geometry(glyphContours.contours,
                                             advanceWidth,
                                             advanceHeight));
                const pair<glyph_geometry_map_t::iterator, bool> result =
                        this->glyph_geometry_map.insert(value);
                assert(result.second);
                glyphGeometry = &result.first->second;
            }

            for (size_t i = 0; i < glyphGeometry->coord.size(); ++i) {
                const vec2f & glyphVertex = glyphGeometry->coord[i];
                const vec2f textVertex(glyphVertex[0] + penPos[0],
                                       glyphVertex[1] + penPos[1]);
                lineGeometry.coord.push_back(textVertex);
                lineGeometry.xMin = (lineGeometry.xMin < textVertex[0])
                                  ? lineGeometry.xMin
                                  : textVertex[0];
                lineGeometry.xMax = (lineGeometry.xMax > textVertex[0])
                                  ? lineGeometry.xMax
                                  : textVertex[0];
                lineGeometry.yMin = (lineGeometry.yMin < textVertex[1])
                                  ? lineGeometry.yMin
                                  : textVertex[1];
                lineGeometry.yMax = (lineGeometry.yMax > textVertex[1])
                                  ? lineGeometry.yMax
                                  : textVertex[1];
            }

            for (size_t i = 0; i < glyphGeometry->coord_index.size(); ++i) {
                const long index = glyphGeometry->coord_index[i];
                if (index > -1) {
                    const size_t offset = lineGeometry.coord.size()
                                          - glyphGeometry->coord.size();
                    lineGeometry.coordIndex.push_back(offset + index);
                } else {
                    lineGeometry.coordIndex.push_back(-1);
                    ++npolygons;
                }
            }
            if (horizontal) {
                const float xAdvance = glyphGeometry->advance_width;
                if (leftToRight) {
                    penPos[0] += xAdvance;
                } else {
                    penPos[0] -= xAdvance;
                }
            } else {
                const float yAdvance = glyphGeometry->advance_height;
                if (topToBottom) {
                    penPos[1] -= yAdvance;
                } else {
                    penPos[1] += yAdvance;
                }
            }
        }

        //
        // Scale to length.
        //
        const float length =
            (line < this->length_.mffloat::value.size())
            ? this->length_.mffloat::value[line]
            : 0.0;
        if (length > 0.0) {
            const float currentLength = lineGeometry.xMax - lineGeometry.xMin;
            for (size_t i = 0; i < lineGeometry.coord.size(); ++i) {
                const vec2f & vertex = lineGeometry.coord[i];
                const vec2f scaledVertex(vertex[0] / currentLength * length,
                                         vertex[1]);
                lineGeometry.coord[i] = scaledVertex;
            }
        }

        //
        // Add the line to the text geometry. We need to adjust for the major
        // alignment.
        //
        float xOffset = 0.0f, yOffset = 0.0f;
        //
        // Offset is 0 for "BEGIN" or "FIRST" (or anything else, in our case).
        //
        if (justify[0] == "MIDDLE") {
            if (horizontal) {
                xOffset = -((lineGeometry.xMax - lineGeometry.xMin) / 2.0f);
            } else {
                yOffset = (lineGeometry.yMax - lineGeometry.yMin) / 2.0f;
            }
        } else if (justify[0] == "END") {
            if (horizontal) {
                xOffset = -(lineGeometry.xMax - lineGeometry.xMin);
            } else {
                yOffset = lineGeometry.yMax - lineGeometry.yMin;
            }
        }
        for (size_t i = 0; i < lineGeometry.coordIndex.size(); ++i) {
            const long index = lineGeometry.coordIndex[i];
            if (index > -1) {
                const vec2f & lineVertex = lineGeometry.coord[index];
                const vec3f textVertex(lineVertex.x() + xOffset,
                                       lineVertex.y() + yOffset,
                                       0.0f);
                newGeometry.coord.push_back(textVertex);
                newGeometry.coord_index
                    .push_back(newGeometry.coord.size() - 1);
                geometryXMin = (geometryXMin < textVertex.x())
                             ? geometryXMin
                             : textVertex.x();
                geometryXMax = (geometryXMax > textVertex.x())
                             ? geometryXMax
                             : textVertex.x();
                geometryYMin = (geometryYMin < textVertex.y())
                             ? geometryYMin
                             : textVertex.y();
                geometryYMax = (geometryYMax > textVertex.y())
                             ? geometryYMax
                             : textVertex.y();
            } else {
                newGeometry.coord_index.push_back(-1);
            }
        }
    }

    //
    // Scale to maxExtent.
    //
    const float maxExtent =
        (this->max_extent_.sffloat::value > 0.0)
        ? this->max_extent_.sffloat::value
        : 0.0;
    if (maxExtent > 0.0) {
        const float currentMaxExtent = geometryXMax - geometryXMin;
        if (currentMaxExtent > maxExtent) {
            for (size_t i = 0; i < newGeometry.coord.size(); ++i) {
                const vec3f & vertex = newGeometry.coord[i];
                const vec3f scaledVertex(
                    vertex.x() / currentMaxExtent * maxExtent,
                    vertex.y(),
                    vertex.z()
                );
                newGeometry.coord[i] = scaledVertex;
            }
        }
    }

    //
    // Adjust for the minor alignment.
    //
    float xOffset = 0.0f, yOffset = 0.0f;
    if (justify[1] == "FIRST" || justify[1] == "") {
    } else if (justify[1] == "BEGIN") {
        if (horizontal) {
            yOffset = -(size * spacing);
        } else {
            xOffset = 0.0f;
        }
    } else if (justify[1] == "MIDDLE") {
        if (horizontal) {
            yOffset = ((size * spacing
                        * this->string_.mfstring::value.size()) / 2.0f)
                      - (size * spacing);
        } else {
            xOffset = ((size * spacing
                        * this->string_.mfstring::value.size()) / 2.0f)
                      - (size * spacing);
        }
    } else if (justify[1] == "END") {
        if (horizontal) {
            yOffset = size * spacing
                * (this->string_.mfstring::value.size() - 1);
        } else {
            xOffset = size * spacing
                * (this->string_.mfstring::value.size() - 1);
        }
    }
    for (size_t i = 0; i < newGeometry.coord.size(); ++i) {
        const vec3f & vertex = newGeometry.coord[i];
        const vec3f adjustedVertex(vertex.x() + xOffset,
                                   vertex.y() + yOffset,
                                   vertex.z());
        newGeometry.coord[i] = adjustedVertex;
    }

    //
    // Create the normals.
    //
    newGeometry.normal.resize(npolygons); // Throws std::bad_alloc.
    for (size_t i = 0; i < newGeometry.normal.size(); ++i) {
        static const vec3f normal(0.0, 0.0, 1.0);
        newGeometry.normal[i] = normal;
    }

    //
    // Create the texture coordinates.
    //
    newGeometry.tex_coord.resize(newGeometry.coord.size()); // std::bad_alloc
    for (size_t i = 0; i < newGeometry.tex_coord.size(); ++i) {
        const vec3f & vertex = newGeometry.coord[i];
        newGeometry.tex_coord[i] = vec2f(vertex.x() / size, vertex.y() / size);
    }

    this->text_geometry_ = newGeometry;
# endif // OPENVRML_ENABLE_RENDER_TEXT_NODE
}


/**
 * @class openvrml::vrml97_node::texture_coordinate_class
 *
 * @brief Class object for TextureCoordinate nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::texture_coordinate_class::
texture_coordinate_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::texture_coordinate_class::~texture_coordinate_class()
    throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating TextureCoordinate
 *      nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by texture_coordinate_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::texture_coordinate_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterface =
        node_interface(node_interface::exposedfield_id,
                      field_value::mfvec2f_id,
                      "point");

    typedef vrml97_node_type_impl<texture_coordinate_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & textureCoordinateNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterface) {
            textureCoordinateNodeType.add_exposedfield(
                supportedInterface.field_type,
                supportedInterface.id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<mfvec2f> >(&texture_coordinate_node::point_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<mfvec2f> >(
                        &texture_coordinate_node::point_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<mfvec2f> >(
                        &texture_coordinate_node::point_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::texture_coordinate_node
 *
 * @brief TextureCoordinate node instances.
 */

/**
 * @var openvrml::vrml97_node::texture_coordinate_node::texture_coordinate_class
 *
 * @brief Class object for TextureCoordinate instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::mfvec2f> openvrml::vrml97_node::texture_coordinate_node::point_
 *
 * @brief point exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope that the new node will belong to.
 */
openvrml::vrml97_node::texture_coordinate_node::
texture_coordinate_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::texture_coordinate_node(type, scope),
    point_(*this)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::texture_coordinate_node::~texture_coordinate_node()
    throw ()
{}

/**
 * @brief Get the points encapsulated by this node.
 *
 * @return the mfvec2f array of points for this node.
 */
const std::vector<openvrml::vec2f> &
openvrml::vrml97_node::texture_coordinate_node::point() const throw ()
{
    return this->point_.mfvec2f::value;
}


/**
 * @class openvrml::vrml97_node::texture_transform_class
 *
 * @brief Class object for TextureTransform nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::texture_transform_class::
texture_transform_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::texture_transform_class::~texture_transform_class()
    throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating TextureTransform
 *      nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by texture_transform_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::texture_transform_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec2f_id,
                       "center"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sffloat_id,
                       "rotation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec2f_id,
                       "scale"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec2f_id,
                       "translation")
    };

    typedef vrml97_node_type_impl<texture_transform_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & textureTransformNodeType =
        static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            textureTransformNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec2f> >(&texture_transform_node::center_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec2f> >(
                        &texture_transform_node::center_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec2f> >(
                        &texture_transform_node::center_)));
        } else if (*interface == supportedInterfaces[1]) {
            textureTransformNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(
                        &texture_transform_node::rotation_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &texture_transform_node::rotation_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &texture_transform_node::rotation_)));
        } else if (*interface == supportedInterfaces[2]) {
            textureTransformNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec2f> >(&texture_transform_node::scale_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec2f> >(
                        &texture_transform_node::scale_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec2f> >(
                        &texture_transform_node::scale_)));
        } else if (*interface == supportedInterfaces[3]) {
            textureTransformNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec2f> >(
                        &texture_transform_node::translation_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec2f> >(
                        &texture_transform_node::translation_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec2f> >(
                        &texture_transform_node::translation_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::texture_transform_node
 *
 * @brief TextureTransform node instances.
 */

/**
 * @var openvrml::vrml97_node::texture_transform_node::texture_transform_class
 *
 * @brief Class object for TextureTransform instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec2f> openvrml::vrml97_node::texture_transform_node::center_
 *
 * @brief center exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::texture_transform_node::rotation_
 *
 * @brief rotation exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec2f> openvrml::vrml97_node::texture_transform_node::scale_
 *
 * @brief scale exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec2f> openvrml::vrml97_node::texture_transform_node::translation_
 *
 * @brief translation exposedField.
 */

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
openvrml::vrml97_node::texture_transform_node::
texture_transform_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    openvrml::texture_transform_node(type, scope),
    center_(*this, vec2f(0.0, 0.0)),
    rotation_(*this, 0.0),
    scale_(*this, vec2f(1.0, 1.0)),
    translation_(*this, vec2f(0.0, 0.0))
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::texture_transform_node::~texture_transform_node()
    throw ()
{}

/**
 * @brief render_texture_transform implementation.
 *
 * @param v viewer.
 */
void
openvrml::vrml97_node::texture_transform_node::
do_render_texture_transform(viewer & v)
{
    v.set_texture_transform(this->center_.sfvec2f::value,
                            this->rotation_.sffloat::value,
                            this->scale_.sfvec2f::value,
                            this->translation_.sfvec2f::value);
}


/**
 * @class openvrml::vrml97_node::time_sensor_class
 *
 * @brief Class object for TimeSensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser   the browser associated with this node_class.
 */
openvrml::vrml97_node::time_sensor_class::
time_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::time_sensor_class::~time_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating TimeSensor nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by time_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::time_sensor_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "cycleInterval"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "enabled"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "loop"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "startTime"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sftime_id,
                       "stopTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "cycleTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sffloat_id,
                       "fraction_changed"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "time")
    };

    typedef vrml97_node_type_impl<time_sensor_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & timeSensorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            timeSensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    time_sensor_node::set_cycle_interval_listener>(
                        &time_sensor_node::set_cycle_interval_listener_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sftime>(
                        &time_sensor_node::cycle_interval_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &time_sensor_node::cycle_interval_changed_emitter_)));
        } else if (*interface == supportedInterfaces[1]) {
            timeSensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    time_sensor_node::enabled_exposedfield>(
                        &time_sensor_node::enabled_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    time_sensor_node::enabled_exposedfield>(
                        &time_sensor_node::enabled_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    time_sensor_node::enabled_exposedfield>(
                        &time_sensor_node::enabled_)));
        } else if (*interface == supportedInterfaces[2]) {
            timeSensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &time_sensor_node::loop_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &time_sensor_node::loop_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &time_sensor_node::loop_)));
        } else if (*interface == supportedInterfaces[3]) {
            timeSensorNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    time_sensor_node::set_start_time_listener>(
                        &time_sensor_node::set_start_time_listener_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sftime>(
                        &time_sensor_node::start_time_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &time_sensor_node::start_time_changed_emitter_)));
        } else if (*interface == supportedInterfaces[4]) {
            timeSensorNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sftime> >(
                        &time_sensor_node::stop_time_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sftime> >(
                        &time_sensor_node::stop_time_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sftime> >(
                        &time_sensor_node::stop_time_)));
        } else if (*interface == supportedInterfaces[5]) {
            timeSensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &time_sensor_node::cycle_time_emitter_)));
        } else if (*interface == supportedInterfaces[6]) {
            timeSensorNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sffloat_emitter>(
                        &time_sensor_node::fraction_changed_emitter_)));
        } else if (*interface == supportedInterfaces[7]) {
            timeSensorNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &time_sensor_node::is_active_emitter_)));
        } else if (*interface == supportedInterfaces[8]) {
            timeSensorNodeType.add_eventout(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &time_sensor_node::time_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::time_sensor_node
 *
 * @brief TimeSensor node instances.
 */

/**
 * @var class openvrml::vrml97_node::time_sensor_node::time_sensor_class
 *
 * @brief Class object for TimeSensor nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::time_sensor_node::set_cycle_interval_listener
 *
 * @brief set_cycleInterval eventIn handler.
 */

/**
 * @brief Construct.
 *
 * @param node  time_sensor_node.
 */
openvrml::vrml97_node::time_sensor_node::set_cycle_interval_listener::
set_cycle_interval_listener(time_sensor_node & node):
    sftime_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::time_sensor_node::set_cycle_interval_listener::
~set_cycle_interval_listener()
    throw ()
{}

/**
 * @brief Process event.
 *
 * @param cycle_interval    cycleInterval.
 * @param timestamp         the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::time_sensor_node::set_cycle_interval_listener::
do_process_event(const sftime & cycle_interval, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        time_sensor_node & node = dynamic_cast<time_sensor_node &>(this->node());

        if (!node.is_active_.value) {
            node.cycle_interval_ = cycle_interval;
            node.lastTime = timestamp;
            node::emit_event(node.cycle_interval_changed_emitter_, timestamp);
        }
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::time_sensor_node::enabled_exposedfield
 *
 * @brief enabled exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  time_sensor_node.
 */
openvrml::vrml97_node::time_sensor_node::enabled_exposedfield::
enabled_exposedfield(time_sensor_node & node):
    openvrml::exposedfield<openvrml::sfbool>(node, true)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::time_sensor_node::enabled_exposedfield::
~enabled_exposedfield()
    throw ()
{}

/**
 * @brief Process event.
 *
 * @param enabled   enabled state.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::time_sensor_node::enabled_exposedfield::
event_side_effect(const sfbool & enabled, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        time_sensor_node & node = dynamic_cast<time_sensor_node &>(this->node());

        if (enabled.value != node.is_active_.value) {
            if (node.is_active_.value) {
                using openvrml_::fequal;

                //
                // Was active; shutdown.
                //
                double cycleInt = node.cycle_interval_.value;
                double f = (cycleInt > 0.0)
                    ? fmod(node.time_.value - node.start_time_.value, cycleInt)
                    : 0.0;

                // Fraction of cycle message
                node.fraction_changed_.value =
                    float(fequal<double>()(f, 0.0) ? 1.0 : (f / cycleInt));
            } else {
                //
                // Was inactive; startup.
                //
                node.cycle_time_.value = timestamp;
                node::emit_event(node.cycle_time_emitter_, timestamp);

                // Fraction of cycle message
                node.fraction_changed_.value = 0.0;
            }
            node.time_.value = timestamp;
            node::emit_event(node.time_emitter_, timestamp);
            node::emit_event(node.fraction_changed_emitter_, timestamp);
            node.is_active_ = enabled;
            node::emit_event(node.is_active_emitter_, timestamp);
        }
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::time_sensor_node::set_start_time_listener
 *
 * @brief set_startTime eventIn handler.
 */

/**
 * @brief Construct.
 *
 * @param node  time_sensor_node.
 */
openvrml::vrml97_node::time_sensor_node::set_start_time_listener::
set_start_time_listener(time_sensor_node & node):
    sftime_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::time_sensor_node::set_start_time_listener::
~set_start_time_listener()
    throw ()
{}

/**
 * @brief Process event.
 *
 * @param start_time    startTime.
 * @param timestamp     the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::time_sensor_node::set_start_time_listener::
do_process_event(const sftime & start_time, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        time_sensor_node & node = dynamic_cast<time_sensor_node &>(this->node());

        if (!node.is_active_.value) {
            node.start_time_ = start_time;
            node.lastTime = timestamp;
            node::emit_event(node.start_time_changed_emitter_, timestamp);
        }
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::time_sensor_node::set_cycle_interval_listener openvrml::vrml97_node::time_sensor_node::set_cycle_interval_listener_
 *
 * @brief set_cycleInterval eventIn handler.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::time_sensor_node::cycle_interval_
 *
 * @brief cycleInterval exposedField value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::time_sensor_node::cycle_interval_changed_emitter_
 *
 * @brief cycleInterval_changed event emitter.
 */

/**
 * @var openvrml::vrml97_node::time_sensor_node::enabled_exposedfield openvrml::vrml97_node::time_sensor_node::enabled_
 *
 * @brief enabled exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::time_sensor_node::loop_
 *
 * @brief loop exposedField.
 */

/**
 * @var openvrml::vrml97_node::time_sensor_node::set_start_time_listener openvrml::vrml97_node::time_sensor_node::set_start_time_listener_
 *
 * @brief set_startTime event handler.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::time_sensor_node::start_time_
 *
 * @brief startTime exposedField value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::time_sensor_node::start_time_changed_emitter_
 *
 * @brief startTime_changed event emitter.
 */

/**
 * @var openvrml::exposedfield<openvrml::sftime> openvrml::vrml97_node::time_sensor_node::stop_time_
 *
 * @brief stopTime exposedField.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::time_sensor_node::cycle_time_
 *
 * @brief cycleTime eventOut value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::time_sensor_node::cycle_time_emitter_
 *
 * @brief cycleTime eventOut emitter.
 */

/**
 * @var openvrml::sffloat openvrml::vrml97_node::time_sensor_node::fraction_changed_
 *
 * @brief fraction_changed eventOut value.
 */

/**
 * @var openvrml::sffloat_emitter openvrml::vrml97_node::time_sensor_node::fraction_changed_emitter_
 *
 * @brief fraction_changed eventOut emitter.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::time_sensor_node::is_active_
 *
 * @brief isActive eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::time_sensor_node::is_active_emitter_
 *
 * @brief isActive eventOut emitter.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::time_sensor_node::time_
 *
 * @brief time eventOut value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::time_sensor_node::time_emitter_
 *
 * @brief time eventOut emitter.
 */

/**
 * @var double openvrml::vrml97_node::time_sensor_node::lastTime
 *
 * @brief The timestamp previously received.
 */

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
openvrml::vrml97_node::time_sensor_node::
time_sensor_node(const node_type & type,
                 const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    set_cycle_interval_listener_(*this),
    cycle_interval_(1.0),
    cycle_interval_changed_emitter_(this->cycle_interval_),
    enabled_(*this),
    loop_(*this, false),
    set_start_time_listener_(*this),
    start_time_(0.0),
    start_time_changed_emitter_(this->start_time_),
    stop_time_(*this, 0.0),
    cycle_time_emitter_(this->cycle_time_),
    fraction_changed_emitter_(this->fraction_changed_),
    is_active_(false),
    is_active_emitter_(this->is_active_),
    time_emitter_(this->time_),
    lastTime(-1.0)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::time_sensor_node::~time_sensor_node() throw ()
{}

/**
 * @brief Cast to a time_sensor_node.
 *
 * @return a pointer to the object.
 */
openvrml::vrml97_node::time_sensor_node *
openvrml::vrml97_node::time_sensor_node::to_time_sensor() const
{
    return (time_sensor_node*) this;
}

/**
 * Generate timer events. If necessary, events prior to the timestamp (inTime)
 * are generated to respect stopTimes and cycleIntervals. The timestamp
 * should never be increased. This assumes the event loop delivers pending
 * events in order (ascending time stamps). Should inTime be modified?
 * Should ensure continuous events are delivered before discrete ones
 * (such as cycleTime, isActive).
 */
void openvrml::vrml97_node::time_sensor_node::update(const double currentTime)
{
    sftime timeNow(currentTime);

    if (this->enabled_.sfbool::value) {
        if (this->lastTime > timeNow.value) { this->lastTime = timeNow.value; }

        // Become active at startTime if either the valid stopTime hasn't
        // passed or we are looping.
        if (!this->is_active_.value
            && this->start_time_.value <= timeNow.value
            && this->start_time_.value >= this->lastTime
            && ((this->stop_time_.sftime::value < this->start_time_.value
                 || this->stop_time_.sftime::value > timeNow.value)
                || this->loop_.sfbool::value)) {

            // Start at first tick >= startTime
            this->is_active_.value = true;
            node::emit_event(this->is_active_emitter_, timeNow.value);
            this->time_.value = timeNow.value;
            node::emit_event(this->time_emitter_, timeNow.value);
            this->fraction_changed_.value = 0.0;
            node::emit_event(this->fraction_changed_emitter_, timeNow.value);
            this->cycle_time_.value = timeNow.value;
            node::emit_event(this->cycle_time_emitter_, timeNow.value);
        }

        // Running (active and enabled)
        else if (this->is_active_.value) {
            using openvrml_::fequal;
            using openvrml_::fless_equal;

            double f, cycleInt = this->cycle_interval_.value;
            bool deactivate = false;

            // Are we done? Choose min of stopTime or start + single cycle.
            if ((this->stop_time_.sftime::value > this->start_time_.value
                 && fless_equal<double>()(this->stop_time_.sftime::value,
                                          timeNow.value))
                || (!this->loop_.sfbool::value
                    && fless_equal<double>()(this->start_time_.value
                                             + cycleInt,
                                             timeNow.value))) {
                this->is_active_.value = false;

                // Must respect stopTime/cycleInterval exactly
                if (this->start_time_.value + cycleInt
                    < this->stop_time_.sftime::value) {
                    timeNow = sftime(this->start_time_.value + cycleInt);
                } else {
                    timeNow = this->stop_time_;
                }

                deactivate = true;
            }

            f = (cycleInt > 0.0 && timeNow.value > this->start_time_.value)
              ? fmod(timeNow.value - this->start_time_.value, cycleInt)
              : 0.0;

            fequal<double> feq;

            // Fraction of cycle message
            this->fraction_changed_.value = feq(f, 0.0)
                                 ? 1.0f
                                 : float(f / cycleInt);
            node::emit_event(this->fraction_changed_emitter_, timeNow.value);

            // Current time message
            this->time_.value = timeNow.value;
            node::emit_event(this->time_emitter_, timeNow.value);

            // End of cycle message (this may miss cycles...)
            if (feq(this->fraction_changed_.value, 1.0)) {
                this->cycle_time_.value = timeNow.value;
                node::emit_event(this->cycle_time_emitter_, timeNow.value);
            }

            if (deactivate) {
                node::emit_event(this->is_active_emitter_, timeNow.value);
            }
        }

        // Tell the scene this node needs quick updates while it is active.
        // Should check whether time, fraction_changed eventOuts are
        // being used, and set delta to cycleTime if not...
        if (this->is_active_.value) {
            this->type().node_class().browser().delta(0.0);
        }
        this->lastTime = currentTime;
    }
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::time_sensor_node::do_initialize(const double timestamp)
    throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser().add_time_sensor(*this);
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void
openvrml::vrml97_node::time_sensor_node::do_shutdown(const double timestamp)
    throw ()
{
    assert(this->scene());
    this->scene()->browser().remove_time_sensor(*this);
}


/**
 * @class openvrml::vrml97_node::touch_sensor_class
 *
 * @brief Class object for TouchSensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::touch_sensor_class::
touch_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::touch_sensor_class::~touch_sensor_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating TouchSensor
 *         nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by touch_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::touch_sensor_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                      field_value::sfbool_id,
                      "enabled"),
        node_interface(node_interface::eventout_id,
                      field_value::sfvec3f_id,
                      "hitNormal_changed"),
        node_interface(node_interface::eventout_id,
                      field_value::sfvec3f_id,
                      "hitPoint_changed"),
        node_interface(node_interface::eventout_id,
                      field_value::sfvec2f_id,
                      "hitTexCoord_changed"),
        node_interface(node_interface::eventout_id,
                      field_value::sfbool_id,
                      "isActive"),
        node_interface(node_interface::eventout_id,
                      field_value::sfbool_id,
                      "isOver"),
        node_interface(node_interface::eventout_id,
                      field_value::sftime_id,
                      "touchTime")
    };

    typedef vrml97_node_type_impl<touch_sensor_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & touchSensorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            touchSensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &touch_sensor_node::enabled_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &touch_sensor_node::enabled_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &touch_sensor_node::enabled_)));
        } else if (*interface == supportedInterfaces[1]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfvec3f_emitter>(
                        &touch_sensor_node::hit_normal_changed_emitter_)));
        } else if (*interface == supportedInterfaces[2]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfvec3f_emitter>(
                        &touch_sensor_node::hit_point_changed_emitter_)));
        } else if (*interface == supportedInterfaces[3]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfvec2f_emitter>(
                        &touch_sensor_node::hit_tex_coord_changed_emitter_)));
        } else if (*interface == supportedInterfaces[4]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &touch_sensor_node::is_active_emitter_)));
        } else if (*interface == supportedInterfaces[5]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &touch_sensor_node::is_over_emitter_)));
        } else if (*interface == supportedInterfaces[6]) {
            touchSensorNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &touch_sensor_node::touch_time_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::touch_sensor_node
 *
 * @brief TouchSensor node instances.
 */

/**
 * @var openvrml::vrml97_node::touch_sensor_node::touch_sensor_class
 *
 * @brief Class object for TouchSensor instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::touch_sensor_node::enabled_
 *
 * @brief enabled exposedField.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::touch_sensor_node::hit_normal_changed_
 *
 * @brief hitNormal_changed eventOut value.
 */

/**
 * @var openvrml::sfvec3f_emitter openvrml::vrml97_node::touch_sensor_node::hit_normal_changed_emitter_
 *
 * @brief hitNormal_changed eventOut emitter.
 */

/**
 * @var openvrml::sfvec3f openvrml::vrml97_node::touch_sensor_node::hit_point_changed_
 *
 * @brief hitPoint_changed eventOut value.
 */

/**
 * @var openvrml::sfvec3f_emitter openvrml::vrml97_node::touch_sensor_node::hit_point_changed_emitter_
 *
 * @brief hitPoint_changed eventOut emitter.
 */

/**
 * @var openvrml::sfvec2f openvrml::vrml97_node::touch_sensor_node::hit_tex_coord_changed_
 *
 * @brief hitTexCoord_changed eventOut value.
 */

/**
 * @var openvrml::sfvec2f_emitter openvrml::vrml97_node::touch_sensor_node::hit_tex_coord_changed_emitter_
 *
 * @brief hitTexCoord_changed eventOut emitter.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::touch_sensor_node::is_active_
 *
 * @brief isActive eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::touch_sensor_node::is_active_emitter_
 *
 * @brief isActive eventOut emitter.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::touch_sensor_node::is_over_
 *
 * @brief isOver eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::touch_sensor_node::is_over_emitter_
 *
 * @brief isOver eventOut emitter.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::touch_sensor_node::touch_time_
 *
 * @brief touchTime eventOut value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::touch_sensor_node::touch_time_emitter_
 *
 * @brief touchTime eventOut emitter.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope that the new node will belong to.
 */
openvrml::vrml97_node::touch_sensor_node::
touch_sensor_node(const node_type & type,
                  const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    enabled_(*this, true),
    hit_normal_changed_emitter_(this->hit_normal_changed_),
    hit_point_changed_emitter_(this->hit_point_changed_),
    hit_tex_coord_changed_emitter_(this->hit_tex_coord_changed_),
    is_active_(false),
    is_active_emitter_(this->is_active_),
    is_over_(false),
    is_over_emitter_(this->is_over_),
    touch_time_(0.0),
    touch_time_emitter_(this->touch_time_)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::touch_sensor_node::~touch_sensor_node() throw ()
{}

/**
 * @brief Cast to a touch_sensor_node.
 *
 * @return a pointer to the object.
 */
openvrml::vrml97_node::touch_sensor_node *
openvrml::vrml97_node::touch_sensor_node::to_touch_sensor() const
{
    return (touch_sensor_node*) this;
}

/**
 * @todo Doesn't compute the xxx_changed eventOuts yet...
 */
void openvrml::vrml97_node::touch_sensor_node::activate(double timestamp,
                                                        bool isOver,
                                                        bool isActive,
                                                        double *)
{
    if (isOver && !isActive && this->is_active_.value) {
        this->touch_time_.value = timestamp;
        node::emit_event(this->touch_time_emitter_, timestamp);
    }

    if (isOver != this->is_over_.value) {
        this->is_over_.value = isOver;
        node::emit_event(this->is_over_emitter_, timestamp);
    }

    if (isActive != this->is_active_.value) {
        this->is_active_.value = isActive;
        node::emit_event(this->is_active_emitter_, timestamp);
    }
    // if (isOver && any routes from eventOuts)
    //   generate xxx_changed eventOuts...
}

/**
 * @brief Return whether the TouchSensor is enabled.
 *
 * @return @c true if the TouchSensor is enabled, @c false otherwise.
 */
bool openvrml::vrml97_node::touch_sensor_node::enabled() const
{
    return this->enabled_.sfbool::value;
}


/**
 * @class openvrml::vrml97_node::transform_class
 *
 * @brief Class object for Transform nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::transform_class::
transform_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::transform_class::~transform_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Transform nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                              supported by transform_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::transform_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "addChildren"),
        node_interface(node_interface::eventin_id,
                       field_value::mfnode_id,
                       "removeChildren"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "center"),
        node_interface(node_interface::exposedfield_id,
                       field_value::mfnode_id,
                       "children"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfrotation_id,
                       "rotation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "scale"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfrotation_id,
                       "scaleOrientation"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "translation"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxCenter"),
        node_interface(node_interface::field_id,
                       field_value::sfvec3f_id,
                       "bboxSize")
    };

    typedef vrml97_node_type_impl<transform_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & transformNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            transformNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    transform_node::add_children_listener>(
                        &transform_node::add_children_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            transformNodeType.add_eventin(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    transform_node::remove_children_listener>(
                        &transform_node::remove_children_listener_)));
        } else if (*interface == supportedInterfaces[2]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    transform_node::center_exposedfield>(
                        &transform_node::center_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    transform_node::center_exposedfield>(
                        &transform_node::center_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    transform_node::center_exposedfield>(
                        &transform_node::center_)));
        } else if (*interface == supportedInterfaces[3]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    transform_node::children_exposedfield>(
                        &transform_node::children_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    transform_node::children_exposedfield>(
                        &transform_node::children_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    transform_node::children_exposedfield>(
                        &transform_node::children_)));
        } else if (*interface == supportedInterfaces[4]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    transform_node::rotation_exposedfield>(
                        &transform_node::rotation_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    transform_node::rotation_exposedfield>(
                        &transform_node::rotation_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    transform_node::rotation_exposedfield>(
                        &transform_node::rotation_)));
        } else if (*interface == supportedInterfaces[5]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    transform_node::scale_exposedfield>(
                        &transform_node::scale_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    transform_node::scale_exposedfield>(
                        &transform_node::scale_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    transform_node::scale_exposedfield>(
                        &transform_node::scale_)));
        } else if (*interface == supportedInterfaces[6]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    transform_node::scale_orientation_exposedfield>(
                        &transform_node::scale_orientation_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    transform_node::scale_orientation_exposedfield>(
                        &transform_node::scale_orientation_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    transform_node::scale_orientation_exposedfield>(
                        &transform_node::scale_orientation_)));
        } else if (*interface == supportedInterfaces[7]) {
            transformNodeType.add_exposedfield(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    transform_node::translation_exposedfield>(
                        &transform_node::translation_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    transform_node::translation_exposedfield>(
                        &transform_node::translation_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    transform_node::translation_exposedfield>(
                        &transform_node::translation_)));
        } else if (*interface == supportedInterfaces[8]) {
            transformNodeType.add_field(
                supportedInterfaces[8].field_type,
                supportedInterfaces[8].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &transform_node::bbox_center_)));
        } else if (*interface == supportedInterfaces[9]) {
            transformNodeType.add_field(
                supportedInterfaces[9].field_type,
                supportedInterfaces[9].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfvec3f>(
                        &transform_node::bbox_size_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::transform_node
 *
 * @brief Transform node instances.
 */

/**
 * @var class openvrml::vrml97_node::transform_node::transform_class
 *
 * @brief Class object for Transform nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::transform_node::center_exposedfield
 *
 * @brief center exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  transform_node.
 */
openvrml::vrml97_node::transform_node::center_exposedfield::
center_exposedfield(transform_node & node):
    openvrml::exposedfield<openvrml::sfvec3f>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::transform_node::center_exposedfield::
~center_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param center    center.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::transform_node::center_exposedfield::
event_side_effect(const sfvec3f & center, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        transform_node & node = dynamic_cast<transform_node &>(this->node());
        node.bounding_volume_dirty(true);
        node.transform_dirty = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::transform_node::rotation_exposedfield
 *
 * @brief rotation exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  transform_node.
 */
openvrml::vrml97_node::transform_node::rotation_exposedfield::
rotation_exposedfield(transform_node & node):
    openvrml::exposedfield<openvrml::sfrotation>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::transform_node::rotation_exposedfield::
~rotation_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param rotation  rotation.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::transform_node::rotation_exposedfield::
event_side_effect(const sfrotation & rotation, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        transform_node & node = dynamic_cast<transform_node &>(this->node());
        node.bounding_volume_dirty(true);
        node.transform_dirty = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::transform_node::scale_exposedfield
 *
 * @brief scale exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  transform_node.
 */
openvrml::vrml97_node::transform_node::scale_exposedfield::
scale_exposedfield(transform_node & node):
    openvrml::exposedfield<openvrml::sfvec3f>(node, vec3f(1.0f, 1.0f, 1.0f))
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::transform_node::scale_exposedfield::
~scale_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param scale     scale.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::transform_node::scale_exposedfield::
event_side_effect(const sfvec3f & scale, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        transform_node & node = dynamic_cast<transform_node &>(this->node());
        node.bounding_volume_dirty(true);
        node.transform_dirty = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::transform_node::scale_orientation_exposedfield
 *
 * @brief scaleOrientation exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  transform_node.
 */
openvrml::vrml97_node::transform_node::scale_orientation_exposedfield::
scale_orientation_exposedfield(transform_node & node):
    openvrml::exposedfield<openvrml::sfrotation>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::transform_node::scale_orientation_exposedfield::
~scale_orientation_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param scale_orientation scaleOrientation.
 * @param timestamp         the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::transform_node::scale_orientation_exposedfield::
event_side_effect(const sfrotation & scale_orientation, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        transform_node & node = dynamic_cast<transform_node &>(this->node());
        node.bounding_volume_dirty(true);
        node.transform_dirty = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::transform_node::translation_exposedfield
 *
 * @brief translation exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node  transform_node.
 */
openvrml::vrml97_node::transform_node::translation_exposedfield::
translation_exposedfield(transform_node & node):
    openvrml::exposedfield<openvrml::sfvec3f>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::transform_node::translation_exposedfield::
~translation_exposedfield() throw ()
{}

/**
 * @brief Process event.
 *
 * @param translation     translation.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::transform_node::translation_exposedfield::
event_side_effect(const sfvec3f & translation, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        transform_node & node = dynamic_cast<transform_node &>(this->node());
        node.bounding_volume_dirty(true);
        node.transform_dirty = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::transform_node::center_exposedfield openvrml::vrml97_node::transform_node::center_
 *
 * @brief center exposedField.
 */

/**
 * @var openvrml::vrml97_node::transform_node::rotation_exposedfield openvrml::vrml97_node::transform_node::rotation_
 *
 * @brief rotation exposedField.
 */

/**
 * @var openvrml::vrml97_node::transform_node::scale_exposedfield openvrml::vrml97_node::transform_node::scale_
 *
 * @brief scale exposedField.
 */

/**
 * @var openvrml::vrml97_node::transform_node::scale_orientation_exposedfield openvrml::vrml97_node::transform_node::scale_orientation_
 *
 * @brief scaleOrientation exposedField.
 */

/**
 * @var openvrml::vrml97_node::transform_node::translation_exposedfield openvrml::vrml97_node::transform_node::translation_
 *
 * @brief translation exposedField.
 */

/**
 * @var openvrml::viewer::object_t openvrml::vrml97_node::transform_node::xformObject
 *
 * @brief A handle to the renderer's representation of the Transform.
 */

/**
 * @var openvrml::mat4f openvrml::vrml97_node::transform_node::transform_
 *
 * @brief Cached copy of this node's transformation.
 */

/**
 * @var bool openvrml::vrml97_node::transform_node::transform_dirty
 *
 * @brief Flag to indicate whether @a transform_ needs to be updated.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope that the new node will belong to.
 */
openvrml::vrml97_node::transform_node::
transform_node(const node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    grouping_node(type, scope),
    group_node(type, scope),
    openvrml::transform_node(type, scope),
    center_(*this),
    rotation_(*this),
    scale_(*this),
    scale_orientation_(*this),
    translation_(*this),
    transform_dirty(true),
    xformObject(0)
{
    this->bounding_volume_dirty(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::transform_node::~transform_node() throw ()
{
    // delete xformObject...
}

/**
 * @brief Get the transformation associated with the node as a matrix.
 *
 * @return the transformation associated with the node.
 */
const openvrml::mat4f &
openvrml::vrml97_node::transform_node::transform() const throw ()
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
void
openvrml::vrml97_node::transform_node::
do_render_child(openvrml::viewer & viewer, rendering_context context)
{
    if (context.cull_flag != bounding_volume::inside) {
        assert(dynamic_cast<const bounding_sphere *>
               (&this->bounding_volume()));
        const bounding_sphere & bs =
            static_cast<const bounding_sphere &>(this->bounding_volume());
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

    mat4f new_LM = this->transform_ * context.matrix();
    context.matrix(new_LM);

    if (this->xformObject && modified()) {
        viewer.remove_object(this->xformObject);
        this->xformObject = 0;
    }

    if (this->xformObject) {
        viewer.insert_reference(this->xformObject);
    } else if (!this->children_.mfnode::value.empty()) {
        this->xformObject = viewer.begin_object(this->id().c_str());

        // Apply transforms
        viewer.transform(this->transform());
        // Render children
        this->group_node::render_nocull(viewer, context);

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
openvrml::vrml97_node::transform_node::do_bounding_volume() const
{
    if (this->bounding_volume_dirty()) {
        const_cast<transform_node *>(this)->recalc_bsphere();
    }
    return this->bsphere;
}

/**
 * @brief Recalculate the bounding volume.
 */
void openvrml::vrml97_node::transform_node::recalc_bsphere()
{
    this->bsphere = bounding_sphere();
    for (size_t i = 0; i < this->children_.mfnode::value.size(); ++i) {
        const node_ptr & node = this->children_.mfnode::value[i];
        bounded_volume_node * bounded_volume =
            node_cast<bounded_volume_node *>(node.get());
        if (bounded_volume) {
            const openvrml::bounding_volume & ci_bv =
                bounded_volume->bounding_volume();
            this->bsphere.extend(ci_bv);
        }
    }
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
void openvrml::vrml97_node::transform_node::update_transform() const throw ()
{
    if (this->transform_dirty) {
        this->transform_ =
            mat4f::transformation(this->translation_.sfvec3f::value,
                                  this->rotation_.sfrotation::value,
                                  this->scale_.sfvec3f::value,
                                  this->scale_orientation_.sfrotation::value,
                                  this->center_.sfvec3f::value);
        this->transform_dirty = false;
    }
}


/**
 * @class openvrml::vrml97_node::viewpoint_class
 *
 * @brief Class object for Viewpoint nodes.
 */

/**
 * @typedef openvrml::vrml97_node::viewpoint_class::bound_nodes_t
 *
 * @brief A stack of bound Viewpoint nodes.
 */

/**
 * @var openvrml::vrml97_node::viewpoint_node * openvrml::vrml97_node::viewpoint_class::first
 *
 * @brief The first Viewpoint node in the initial scene graph.
 */

/**
 * @var openvrml::vrml97_node::viewpoint_class::bound_nodes_t openvrml::vrml97_node::viewpoint_class::bound_nodes
 *
 * @brief The stack of bound Viewpoint nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::viewpoint_class::
viewpoint_class(openvrml::browser & browser):
    node_class(browser),
    first(0)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::viewpoint_class::~viewpoint_class() throw ()
{}

/**
 * @brief Set the first Viewpoint node in the world.
 *
 * The first Viewpoint node in the world is used as the initial viewpoint.
 * This function is used by <code>viewpoint_node::do_initialize</code>.
 *
 * @param viewpoint    a Viewpoint node.
 */
void
openvrml::vrml97_node::viewpoint_class::set_first(viewpoint_node & viewpoint)
    throw ()
{
    assert(!this->has_first());
    this->first = &viewpoint;
}

/**
 * @brief Reset the pointer to the first Viewpoint node in the world to null.
 *
 * This function is called by <code>viewpoint_node::do_shutdown</code>.
 */
void openvrml::vrml97_node::viewpoint_class::reset_first() throw ()
{
    this->first = 0;
}

/**
 * @brief Check to see if the first node has been set.
 *
 * This method is used by viewpoint_node::do_initialize.
 *
 * @return @c true if the first node has already been set; @c false otherwise.
 */
bool openvrml::vrml97_node::viewpoint_class::has_first() const throw ()
{
    return this->first;
}

/**
 * @brief Check to see if a node is registered as the "first" node.
 *
 * @param viewpoint a viewpoint_node.
 *
 * @return @c true if @p viewpoint is the fist node; @c false otherwise.
 */
bool
openvrml::vrml97_node::viewpoint_class::is_first(viewpoint_node & viewpoint)
    throw ()
{
    return &viewpoint == this->first;
}

/**
 * @brief Push a Viewpoint on the top of the bound node stack.
 *
 * @param viewpoint    the node to bind.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::viewpoint_class::bind(viewpoint_node & viewpoint,
                                             const double timestamp)
    throw (std::bad_alloc)
{
    //
    // If the node is already the active node, do nothing.
    //
    if (!this->bound_nodes.empty() && &viewpoint == this->bound_nodes.back()) {
        return;
    }

    //
    // If the node is already on the stack, remove it.
    //
    const bound_nodes_t::iterator pos =
        std::find(this->bound_nodes.begin(), this->bound_nodes.end(),
                  &viewpoint);
    if (pos != this->bound_nodes.end()) { this->bound_nodes.erase(pos); }

    //
    // Send FALSE from the currently active node's isBound.
    //
    if (!this->bound_nodes.empty()) {
        viewpoint_node & current =
                dynamic_cast<viewpoint_node &>(*this->bound_nodes.back());
        current.is_bound_.value = false;
        node::emit_event(current.is_bound_emitter_, timestamp);
    }

    //
    // Push the node to the top of the stack, and have it send isBound TRUE.
    //
    this->bound_nodes.push_back(&viewpoint);
    viewpoint.is_bound_.value = true;
    node::emit_event(viewpoint.is_bound_emitter_, timestamp);

    this->browser().active_viewpoint(viewpoint);
}

/**
 * @brief Remove a Viewpoint from the bound node stack.
 *
 * @param viewpoint    the node to unbind.
 * @param timestamp    the current time.
 */
void openvrml::vrml97_node::viewpoint_class::unbind(viewpoint_node & viewpoint,
                                                    const double timestamp)
    throw ()
{
    const bound_nodes_t::iterator pos =
        std::find(this->bound_nodes.begin(), this->bound_nodes.end(),
                  &viewpoint);
    if (pos != this->bound_nodes.end()) {
        viewpoint.is_bound_.value = false;
        node::emit_event(viewpoint.is_bound_emitter_, timestamp);

        if (pos == this->bound_nodes.end() - 1
                && this->bound_nodes.size() > 1) {
            viewpoint_node & newActive =
                dynamic_cast<viewpoint_node &>(
                    **(this->bound_nodes.end() - 2));
            newActive.is_bound_.value = true;
            node::emit_event(newActive.is_bound_emitter_, timestamp);

            this->browser().active_viewpoint(viewpoint);
        } else {
            this->browser().reset_default_viewpoint();
        }
        this->bound_nodes.erase(pos);
    }
}

/**
 * @brief node_class-specific initialization.
 *
 * @param initial_viewpoint the viewpoint_node that should be bound initially.
 * @param timestamp         the current time.
 */
void
openvrml::vrml97_node::viewpoint_class::
do_initialize(openvrml::viewpoint_node * initial_viewpoint,
              const double timestamp)
    throw ()
{
    if (!initial_viewpoint) { initial_viewpoint = this->first; }
    if (initial_viewpoint) {
        try {
            event_listener & listener =
                this->first->event_listener("set_bind");
            assert(dynamic_cast<sfbool_listener *>(&listener));
            static_cast<sfbool_listener &>(listener)
                .process_event(sfbool(true), timestamp);
        } catch (unsupported_interface & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        }
    }
}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating Viewpoint nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by viewpoint_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::viewpoint_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::eventin_id,
                      field_value::sfbool_id,
                      "set_bind"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sffloat_id,
                      "fieldOfView"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sfbool_id,
                      "jump"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sfrotation_id,
                      "orientation"),
        node_interface(node_interface::exposedfield_id,
                      field_value::sfvec3f_id,
                      "position"),
        node_interface(node_interface::field_id,
                      field_value::sfstring_id,
                      "description"),
        node_interface(node_interface::eventout_id,
                      field_value::sftime_id,
                      "bindTime"),
        node_interface(node_interface::eventout_id,
                      field_value::sfbool_id,
                      "isBound")
    };

    typedef vrml97_node_type_impl<viewpoint_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & viewpointNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            viewpointNodeType.add_eventin(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    viewpoint_node::set_bind_listener>(
                        &viewpoint_node::set_bind_listener_)));
        } else if (*interface == supportedInterfaces[1]) {
            viewpointNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sffloat> >(&viewpoint_node::field_of_view_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sffloat> >(
                        &viewpoint_node::field_of_view_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sffloat> >(
                        &viewpoint_node::field_of_view_)));
        } else if (*interface == supportedInterfaces[2]) {
            viewpointNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &viewpoint_node::jump_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &viewpoint_node::jump_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &viewpoint_node::jump_)));
        } else if (*interface == supportedInterfaces[3]) {
            viewpointNodeType.add_exposedfield(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    viewpoint_node::orientation_exposedfield>(
                        &viewpoint_node::orientation_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    viewpoint_node::orientation_exposedfield>(
                        &viewpoint_node::orientation_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    viewpoint_node::orientation_exposedfield>(
                        &viewpoint_node::orientation_)));
        } else if (*interface == supportedInterfaces[4]) {
            viewpointNodeType.add_exposedfield(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    viewpoint_node::position_exposedfield>(
                        &viewpoint_node::position_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<
                    viewpoint_node::position_exposedfield>(
                        &viewpoint_node::position_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<
                    viewpoint_node::position_exposedfield>(
                        &viewpoint_node::position_)));
        } else if (*interface == supportedInterfaces[5]) {
            viewpointNodeType.add_field(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfstring>(
                        &viewpoint_node::description_)));
        } else if (*interface == supportedInterfaces[6]) {
            viewpointNodeType.add_eventout(
                supportedInterfaces[6].field_type,
                supportedInterfaces[6].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &viewpoint_node::bind_time_emitter_)));
        } else if (*interface == supportedInterfaces[7]) {
            viewpointNodeType.add_eventout(
                supportedInterfaces[7].field_type,
                supportedInterfaces[7].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &viewpoint_node::is_bound_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::viewpoint_node
 *
 * @brief Viewpoint node instance.
 */

/**
 * @var class openvrml::vrml97_node::viewpoint_node::viewpoint_class
 *
 * @brief Class object for Viewpoint nodes.
 */

/**
 * @internal
 *
 * @class openvrml::vrml97_node::viewpoint_node::set_bind_listener
 *
 * @brief set_bind eventIn listener.
 */

/**
 * @brief Construct.
 *
 * @param node viewpoint_node.
 */
openvrml::vrml97_node::viewpoint_node::set_bind_listener::
set_bind_listener(viewpoint_node & node):
    sfbool_listener(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::viewpoint_node::set_bind_listener::~set_bind_listener()
    throw ()
{}

/**
 * @brief Process an event.
 *
 * @param value     event value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::viewpoint_node::set_bind_listener::
do_process_event(const sfbool & value, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        viewpoint_node & node = dynamic_cast<viewpoint_node &>(this->node());
        viewpoint_class & node_class =
            const_cast<viewpoint_class &>(
                static_cast<const viewpoint_class &>(
                    node.type().node_class()));
        if (value.value) {
            node_class.bind(node, timestamp);
        } else {
            node_class.unbind(node, timestamp);
        }
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::viewpoint_node::orientation_exposedfield
 *
 * @brief orientation exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node viewpoint_node.
 */
openvrml::vrml97_node::viewpoint_node::orientation_exposedfield::
orientation_exposedfield(viewpoint_node & node):
    openvrml::exposedfield<openvrml::sfrotation>(node)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::viewpoint_node::orientation_exposedfield::
~orientation_exposedfield() throw ()
{}

/**
 * @brief Process an event.
 *
 * @param orientation   orientation.
 * @param timestamp     the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::viewpoint_node::orientation_exposedfield::
event_side_effect(const sfrotation & orientation, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        viewpoint_node & node = dynamic_cast<viewpoint_node &>(this->node());
        node.final_transformation_dirty = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @internal
 *
 * @class openvrml::vrml97_node::viewpoint_node::position_exposedfield
 *
 * @brief position exposedField implementation.
 */

/**
 * @brief Construct.
 *
 * @param node viewpoint_node.
 */
openvrml::vrml97_node::viewpoint_node::position_exposedfield::
position_exposedfield(viewpoint_node & node):
    openvrml::exposedfield<openvrml::sfvec3f>(node, vec3f(0.0f, 0.0f, 10.0f))
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::viewpoint_node::position_exposedfield::
~position_exposedfield() throw ()
{}

/**
 * @brief Process an event.
 *
 * @param position  position.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void
openvrml::vrml97_node::viewpoint_node::position_exposedfield::
event_side_effect(const sfvec3f & position, const double timestamp)
    throw (std::bad_alloc)
{
    try {
        viewpoint_node & node = dynamic_cast<viewpoint_node &>(this->node());
        node.final_transformation_dirty = true;
    } catch (std::bad_cast & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @var openvrml::vrml97_node::viewpoint_node::set_bind_listener openvrml::vrml97_node::viewpoint_node::set_bind_listener_
 *
 * @brief set_bind eventIn handler.
 */

/**
 * @var openvrml::exposedfield<openvrml::sffloat> openvrml::vrml97_node::viewpoint_node::field_of_view_
 *
 * @brief fieldOfView exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::viewpoint_node::jump_
 *
 * @brief jump exposedField.
 */

/**
 * @var viewpoint_node::orientation_exposedfield openvrml::vrml97_node::viewpoint_node::orientation_
 *
 * @brief orientation exposedField.
 */

/**
 * @var viewpoint_node::position_exposedfield openvrml::vrml97_node::viewpoint_node::position_
 *
 * @brief position exposedField.
 */

/**
 * @var openvrml::sfstring openvrml::vrml97_node::viewpoint_node::description_
 *
 * @brief description field.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::viewpoint_node::is_bound_
 *
 * @brief isBound eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::viewpoint_node::is_bound_emitter_
 *
 * @brief isBound eventOut emitter.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::viewpoint_node::bind_time_
 *
 * @brief bindTime eventOut value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::viewpoint_node::bind_time_emitter_
 *
 * @brief bindTime eventOut emitter.
 */

/**
 * @var openvrml::mat4f openvrml::vrml97_node::viewpoint_node::parent_transform
 *
 * @brief The accumulated transformation applied by any parent nodes.
 */

/**
 * @var openvrml::mat4f openvrml::vrml97_node::viewpoint_node::final_transformation
 *
 * @brief The product of @a parent_transform and any transformation applied
 *        by the Viewpoint node.
 */

/**
 * @var bool openvrml::vrml97_node::viewpoint_node::final_transformation_dirty
 *
 * @brief Flag that indicates if @a final_transformation needs to be updated.
 */

/**
 * @var mat4f openvrml::vrml97_node::viewpoint_node::user_view_transform_
 *
 * @brief The transformation applied to the user view.
 *
 * Generally the result of any user navigation.
 */

namespace {
    const float DEFAULT_FIELD_OF_VIEW = 0.785398f;
}

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
openvrml::vrml97_node::viewpoint_node::
viewpoint_node(const node_type & type,
               const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    abstract_base(type, scope),
    openvrml::viewpoint_node(type, scope),
    set_bind_listener_(*this),
    field_of_view_(*this, DEFAULT_FIELD_OF_VIEW),
    jump_(*this, true),
    orientation_(*this),
    position_(*this),
    is_bound_emitter_(this->is_bound_),
    bind_time_emitter_(this->bind_time_),
    final_transformation_dirty(true)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::viewpoint_node::~viewpoint_node() throw ()
{}

/**
 * @brief Get the transformation of the viewpoint_node in the global coordinate
 *      system.
 *
 * @return the transformation of the viewpoint_node in the global coordinate
 *      system.
 */
const openvrml::mat4f &
openvrml::vrml97_node::viewpoint_node::transformation() const throw ()
{
    this->update_final_transformation();
    return this->final_transformation;
}

/**
 * @brief Get the transformation of the user view relative to the
 *      viewpoint_node.
 *
 * @return the transformation of the user view relative to the viewpoint_node.
 */
const openvrml::mat4f &
openvrml::vrml97_node::viewpoint_node::user_view_transform() const throw ()
{
    return this->user_view_transform_;
}

/**
 * @brief Set the transformation of the user view relative to the
 *      viewpoint_node.
 *
 * @param transform the new transformation.
 */
void
openvrml::vrml97_node::viewpoint_node::
user_view_transform(const mat4f & transform) throw ()
{
    this->user_view_transform_ = transform;
}

/**
 * @brief description accessor.
 *
 * @return the description.
 */
const std::string & openvrml::vrml97_node::viewpoint_node::description() const
    throw ()
{
    return this->description_.value;
}

/**
 * @brief Get the field of view.
 *
 * @return the field of view in radians.
 */
float openvrml::vrml97_node::viewpoint_node::field_of_view() const throw ()
{
    return this->field_of_view_.sffloat::value;
}

/**
 * @brief orientation accessor.
 *
 * @return the orientation.
 */
const openvrml::sfrotation &
openvrml::vrml97_node::viewpoint_node::orientation() const
{
    return this->orientation_;
}

/**
 * @brief position accessor.
 *
 * @return the position.
 */
const openvrml::sfvec3f &
openvrml::vrml97_node::viewpoint_node::position() const
{
    return this->position_;
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 */
void
openvrml::vrml97_node::viewpoint_node::do_initialize(const double timestamp)
    throw ()
{
    assert(this->scene());
    this->scene()->browser().add_viewpoint(*this);
    assert(dynamic_cast<const viewpoint_class *>(&this->type().node_class()));
    viewpoint_class & nodeClass =
        const_cast<viewpoint_class &>(
            static_cast<const viewpoint_class &>(this->type().node_class()));
    if (!nodeClass.has_first()) { nodeClass.set_first(*this); }
}

namespace {

    struct accumulate_transform_ :
        std::unary_function<openvrml::node *, void> {

        explicit accumulate_transform_(openvrml::mat4f & transform) throw ():
            transform(&transform)
        {}

        void operator()(openvrml::node * node) const throw ()
        {
            using openvrml::node_cast;
            using openvrml::transform_node;
            assert(node);
            transform_node * const t = node_cast<transform_node *>(node);
            if (t) { *this->transform = t->transform() * *this->transform; }
        }

    private:
        openvrml::mat4f * transform;
    };
}

/**
 * @brief Relocate.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::vrml97_node::viewpoint_node::do_relocate()
    throw (std::bad_alloc)
{
    assert(this->scene());
    const node_path path = this->scene()->browser().find_node(*this);
    assert(!path.empty());
    this->parent_transform = mat4f();
    std::for_each(path.begin(), path.end(),
                  accumulate_transform_(this->parent_transform));
    this->final_transformation_dirty = true;
}

/**
 * @brief Shut down.
 *
 * @param timestamp the current time.
 */
void openvrml::vrml97_node::viewpoint_node::do_shutdown(const double timestamp)
    throw ()
{
    viewpoint_class & node_class =
        const_cast<viewpoint_class &>(
            static_cast<const viewpoint_class &>(this->type().node_class()));
    node_class.unbind(*this, timestamp);
    assert(this->scene());
    this->scene()->browser().remove_viewpoint(*this);

    if (node_class.is_first(*this)) { node_class.reset_first(); }
}

/**
 * @brief Update @a final_transformation.
 *
 * If @a final_transformation_dirty is @c true, update @a final_transformation
 * and sets @a final_transformation_dirty to @c false; otherwise, do nothing.
 */
void openvrml::vrml97_node::viewpoint_node::update_final_transformation() const
    throw ()
{
    if (this->final_transformation_dirty) {
        static const vec3f scale(1.0, 1.0, 1.0);
        static const rotation scaleOrientation;
        static const vec3f center;
        const mat4f & t =
            mat4f::transformation(this->position_.sfvec3f::value,
                                  this->orientation_.sfrotation::value,
                                  scale,
                                  scaleOrientation,
                                  center);
        this->final_transformation = t * this->parent_transform;
        this->final_transformation_dirty = false;
    }
}


/**
 * @class openvrml::vrml97_node::visibility_sensor_class
 *
 * @brief Class object for VisibilitySensor nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::visibility_sensor_class::
visibility_sensor_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::visibility_sensor_class::~visibility_sensor_class()
    throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating VisibilitySensor
 *      nodes.
 *
 * @exception unsupported_interface if @p interfaces includes an interface not
 *                                  supported by visibility_sensor_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::visibility_sensor_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "center"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfbool_id,
                       "enabled"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfvec3f_id,
                       "size"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "enterTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sftime_id,
                       "exitTime"),
        node_interface(node_interface::eventout_id,
                       field_value::sfbool_id,
                       "isActive")
    };

    typedef vrml97_node_type_impl<visibility_sensor_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & visibilitySensorNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            visibilitySensorNodeType.add_exposedfield(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(&visibility_sensor_node::center_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &visibility_sensor_node::center_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &visibility_sensor_node::center_)));
        } else if (*interface == supportedInterfaces[1]) {
            visibilitySensorNodeType.add_exposedfield(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<exposedfield<sfbool> >(
                        &visibility_sensor_node::enabled_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfbool> >(
                        &visibility_sensor_node::enabled_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfbool> >(
                        &visibility_sensor_node::enabled_)));
        } else if (*interface == supportedInterfaces[2]) {
            visibilitySensorNodeType.add_exposedfield(
                supportedInterfaces[2].field_type,
                supportedInterfaces[2].id,
                node_type_t::event_listener_ptr_ptr(
                    new node_type_t::event_listener_ptr<
                    exposedfield<sfvec3f> >(&visibility_sensor_node::size_)),
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<exposedfield<sfvec3f> >(
                        &visibility_sensor_node::size_)),
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<exposedfield<sfvec3f> >(
                        &visibility_sensor_node::size_)));
        } else if (*interface == supportedInterfaces[3]) {
            visibilitySensorNodeType.add_eventout(
                supportedInterfaces[3].field_type,
                supportedInterfaces[3].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &visibility_sensor_node::enter_time_emitter_)));
        } else if (*interface == supportedInterfaces[4]) {
            visibilitySensorNodeType.add_eventout(
                supportedInterfaces[4].field_type,
                supportedInterfaces[4].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sftime_emitter>(
                        &visibility_sensor_node::exit_time_emitter_)));
        } else if (*interface == supportedInterfaces[5]) {
            visibilitySensorNodeType.add_eventout(
                supportedInterfaces[5].field_type,
                supportedInterfaces[5].id,
                node_type_t::event_emitter_ptr_ptr(
                    new node_type_t::event_emitter_ptr<sfbool_emitter>(
                        &visibility_sensor_node::is_active_emitter_)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::visibility_sensor_node
 *
 * @brief VisibilitySensor node instances.
 */

/**
 * @var openvrml::vrml97_node::visibility_sensor_node::visibility_sensor_class
 *
 * @brief Class object for VisibilitySensor instances.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::visibility_sensor_node::center_
 *
 * @brief center exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfbool> openvrml::vrml97_node::visibility_sensor_node::enabled_
 *
 * @brief enabled exposedField.
 */

/**
 * @var openvrml::exposedfield<openvrml::sfvec3f> openvrml::vrml97_node::visibility_sensor_node::size_
 *
 * @brief size exposedField.
 */

/**
 * @var openvrml::sfbool openvrml::vrml97_node::visibility_sensor_node::is_active_
 *
 * @brief isActive eventOut value.
 */

/**
 * @var openvrml::sfbool_emitter openvrml::vrml97_node::visibility_sensor_node::is_active_emitter_
 *
 * @brief isActive eventOut emitter.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::visibility_sensor_node::enter_time_
 *
 * @brief enterTime eventOut value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::visibility_sensor_node::enter_time_emitter_
 *
 * @brief enterTime eventOut emitter.
 */

/**
 * @var openvrml::sftime openvrml::vrml97_node::visibility_sensor_node::exit_time_
 *
 * @brief exitTime eventOut value.
 */

/**
 * @var openvrml::sftime_emitter openvrml::vrml97_node::visibility_sensor_node::exit_time_emitter_
 *
 * @brief exitTime eventOut emitter.
 */

/**
 * @brief Construct.
 *
 * @param type      the node_type associated with the instance.
 * @param scope         the scope that the new node will belong to.
 */
openvrml::vrml97_node::visibility_sensor_node::
visibility_sensor_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope),
    center_(*this, vec3f(0.0, 0.0, 0.0)),
    enabled_(*this, true),
    size_(*this, vec3f(0.0, 0.0, 0.0)),
    is_active_(false),
    is_active_emitter_(this->is_active_),
    enter_time_(0.0),
    enter_time_emitter_(this->enter_time_),
    exit_time_(0.0),
    exit_time_emitter_(this->exit_time_)
{
    this->node::modified(true);
}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::visibility_sensor_node::~visibility_sensor_node()
    throw ()
{}

/**
 * @brief Generate visibility events.
 *
 * @todo This is in a render() method since the it needs to be computed with
 *       respect to the accumulated transformations above it in the scene
 *       graph. Move to update() when xforms are accumulated in Groups...
 *
 * @param viewer    viewer.
 * @param context   rendering_context.
 */
void
openvrml::vrml97_node::visibility_sensor_node::
do_render_child(openvrml::viewer & viewer, const rendering_context context)
{
    if (this->enabled_.sfbool::value) {
        sftime timeNow(browser::current_time());
        vec3f xyz[2] = { this->center_.sfvec3f::value,
                         this->center_.sfvec3f::value
                         + this->size_.sfvec3f::value };

        // hack: enclose box in a sphere...
        viewer.transform_points(2, &xyz[0]);
        float r  = (xyz[1] - xyz[0]).length();

        // Was the sphere visible last time through? How does this work
        // for USE'd nodes? I need a way for each USE to store whether
        // it was active.
        bool wasIn = this->is_active_.value;

        // Is the sphere visible? ...
        bool inside = xyz[0].z() < 0.0; // && z > - scene->visLimit()
        if (inside) {
            using openvrml_::fequal;

            openvrml::navigation_info_node & nav_info =
                this->type().node_class().browser().active_navigation_info();
            if (!fequal<float>()(nav_info.visibility_limit(), 0.0f)
                && xyz[0][2] < -(nav_info.visibility_limit())) {
                inside = false;
            }
        }

        // This bit assumes 90degree fieldOfView to get rid of trig calls...
        if (inside) {
            inside = fabs(xyz[0].x()) < -0.5 * xyz[0].z() + r
                    && fabs(xyz[0].y()) < -0.5 * xyz[0].z() + r;
        }

        // Just became visible
        if (inside && !wasIn) {
            this->is_active_.value = true;
            node::emit_event(this->is_active_emitter_, timeNow.value);

            this->enter_time_ = timeNow;
            node::emit_event(this->enter_time_emitter_, timeNow.value);
        }

        // Check if viewer has left the box
        else if (wasIn && !inside) {
            this->is_active_.value = false;
            node::emit_event(this->is_active_emitter_, timeNow.value);

            this->exit_time_ = timeNow;
            node::emit_event(this->exit_time_emitter_, timeNow.value);
        }
    } else {
        this->node::modified(false);
    }
}


/**
 * @class openvrml::vrml97_node::world_info_class
 *
 * @brief Class object for WorldInfo nodes.
 */

/**
 * @brief Construct.
 *
 * @param browser the browser associated with this node_class.
 */
openvrml::vrml97_node::world_info_class::
world_info_class(openvrml::browser & browser):
    node_class(browser)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::world_info_class::~world_info_class() throw ()
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of creating WorldInfo nodes.
 *
 * @exception unsupported_interface  if @p interfaces includes an interface not
 *                                  supported by world_info_class.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml::vrml97_node::world_info_class::
do_create_type(const std::string & id,
               const node_interface_set & interfaces) const
    throw (unsupported_interface, std::bad_alloc)
{
    static const node_interface supportedInterfaces[] = {
        node_interface(node_interface::field_id,
                       field_value::mfstring_id,
                       "info"),
        node_interface(node_interface::field_id,
                       field_value::sfstring_id,
                       "title")
    };

    typedef vrml97_node_type_impl<world_info_node> node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & worldInfoNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface(interfaces.begin());
         interface != interfaces.end();
         ++interface) {
        if (*interface == supportedInterfaces[0]) {
            worldInfoNodeType.add_field(
                supportedInterfaces[0].field_type,
                supportedInterfaces[0].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<mfstring>(
                        &world_info_node::info)));
        } else if (*interface == supportedInterfaces[1]) {
            worldInfoNodeType.add_field(
                supportedInterfaces[1].field_type,
                supportedInterfaces[1].id,
                node_type_t::field_ptr_ptr(
                    new node_type_t::field_ptr<sfstring>(
                        &world_info_node::title)));
        } else {
            throw unsupported_interface(*interface);
        }
    }
    return type;
}

/**
 * @class openvrml::vrml97_node::world_info_node
 *
 * @brief WorldInfo node instances.
 */

/**
 * @var openvrml::vrml97_node::world_info_node::world_info_class
 *
 * @brief Class object for WorldInfo instances.
 */

/**
 * @var openvrml::mfstring openvrml::vrml97_node::world_info_node::info
 *
 * @brief info field.
 */

/**
 * @var openvrml::sfstring openvrml::vrml97_node::world_info_node::title
 *
 * @brief title field.
 */

/**
 * @brief Construct.
 *
 * @param type  the node_type associated with the instance.
 * @param scope the scope that the new node will belong to.
 */
openvrml::vrml97_node::world_info_node::
world_info_node(const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope):
    node(type, scope),
    abstract_base(type, scope),
    bounded_volume_node(type, scope),
    child_node(type, scope)
{}

/**
 * @brief Destroy.
 */
openvrml::vrml97_node::world_info_node::~world_info_node() throw ()
{}
