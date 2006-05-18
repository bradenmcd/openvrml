// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2005  Braden McDaniel
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

# ifndef OPENVRML_NODE_IMPL_UTIL
#   define OPENVRML_NODE_IMPL_UTIL

# include <stack>
# include <openvrml/exposedfield.h>

namespace openvrml {

    namespace node_impl_util {

        template <typename MemberBase, typename Object>
        class ptr_to_polymorphic_mem {
        public:
            virtual ~ptr_to_polymorphic_mem() OPENVRML_NOTHROW = 0;
            virtual MemberBase & deref(Object & obj) = 0;
            virtual const MemberBase & deref(const Object & obj) = 0;
        };

        template <typename MemberBase, typename Object>
        ptr_to_polymorphic_mem<MemberBase, Object>::~ptr_to_polymorphic_mem()
            OPENVRML_NOTHROW
        {}


        template <typename MemberBase, typename Member, typename Object>
        class ptr_to_polymorphic_mem_impl :
            public ptr_to_polymorphic_mem<MemberBase, Object> {

            Member Object::* ptr_to_mem;

        public:
            explicit ptr_to_polymorphic_mem_impl(Member Object::* ptr_to_mem);
            virtual ~ptr_to_polymorphic_mem_impl() OPENVRML_NOTHROW;

            virtual MemberBase & deref(Object & obj);
            virtual const MemberBase & deref(const Object & obj);
        };

        template <typename MemberBase, typename Member, typename Object>
        ptr_to_polymorphic_mem_impl<MemberBase, Member, Object>::
        ptr_to_polymorphic_mem_impl(Member Object::* ptr_to_mem):
            ptr_to_mem(ptr_to_mem)
        {}

        template <typename MemberBase, typename Member, typename Object>
        ptr_to_polymorphic_mem_impl<MemberBase, Member, Object>::
        ~ptr_to_polymorphic_mem_impl() OPENVRML_NOTHROW
        {}

        template <typename MemberBase, typename Member, typename Object>
        MemberBase &
        ptr_to_polymorphic_mem_impl<MemberBase, Member, Object>::
        deref(Object & obj)
        {
            return obj.*this->ptr_to_mem;
        }

        template <typename MemberBase, typename Member, typename Object>
        const MemberBase &
        ptr_to_polymorphic_mem_impl<MemberBase, Member, Object>::
        deref(const Object & obj)
        {
            return obj.*this->ptr_to_mem;
        }


        class OPENVRML_API abstract_node_type : public openvrml::node_type {
        public:
            virtual ~abstract_node_type() OPENVRML_NOTHROW = 0;
            virtual const openvrml::field_value &
            field_value(const openvrml::node & node,
                        const std::string & id) const
                OPENVRML_THROW1(openvrml::unsupported_interface) = 0;
            virtual openvrml::event_listener &
            event_listener(openvrml::node & node, const std::string & id) const
                OPENVRML_THROW1(openvrml::unsupported_interface) = 0;
            virtual openvrml::event_emitter &
            event_emitter(openvrml::node & node, const std::string & id) const
                OPENVRML_THROW1(openvrml::unsupported_interface) = 0;

        protected:
            abstract_node_type(const openvrml::node_metatype & metatype,
                             const std::string & id);
        };

        template <typename Node> class event_listener_base;
        template <typename Node> class event_emitter_base;


        template <typename Node>
        class node_type_impl : public abstract_node_type {
            friend class event_listener_base<Node>;
            friend class event_emitter_base<Node>;

        public:
            typedef boost::shared_ptr<
            ptr_to_polymorphic_mem<openvrml::field_value, Node> >
            field_ptr_ptr;

            template <typename FieldMember>
            class field_ptr :
                public ptr_to_polymorphic_mem_impl<openvrml::field_value,
                                                   FieldMember,
                                                   Node> {
            public:
                explicit field_ptr(FieldMember Node::* ptr_to_mem);
            };

            typedef boost::shared_ptr<
                ptr_to_polymorphic_mem<openvrml::event_listener, Node> >
            event_listener_ptr_ptr;

            template <typename EventListenerMember>
            class event_listener_ptr :
                public ptr_to_polymorphic_mem_impl<openvrml::event_listener,
                                                   EventListenerMember,
                                                   Node> {
            public:
                explicit event_listener_ptr(
                    EventListenerMember Node::* ptr_to_mem);
            };

            typedef boost::shared_ptr<
                ptr_to_polymorphic_mem<openvrml::event_emitter, Node> >
            event_emitter_ptr_ptr;

            template <typename EventEmitterMember>
            class event_emitter_ptr :
                public ptr_to_polymorphic_mem_impl<openvrml::event_emitter,
                                                   EventEmitterMember,
                                                   Node> {
            public:
                explicit event_emitter_ptr(
                    EventEmitterMember Node::* ptr_to_mem);
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
            node_type_impl(const openvrml::node_metatype & metatype,
                                  const std::string & id);
            virtual ~node_type_impl() OPENVRML_NOTHROW;

            void add_eventin(openvrml::field_value::type_id type,
                             const std::string & id,
                             const event_listener_ptr_ptr & event_listener)
                OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
            void add_eventout(openvrml::field_value::type_id type,
                              const std::string & id,
                              const event_emitter_ptr_ptr & event_emitter)
                OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
            void add_exposedfield(openvrml::field_value::type_id type,
                                  const std::string & id,
                                  const event_listener_ptr_ptr & event_listener,
                                  const field_ptr_ptr & field,
                                  const event_emitter_ptr_ptr & event_emitter)
                OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);
            void add_field(openvrml::field_value::type_id type,
                           const std::string & id,
                           const field_ptr_ptr & field)
                OPENVRML_THROW2(std::invalid_argument, std::bad_alloc);

            virtual const openvrml::field_value &
            field_value(const openvrml::node & node,
                        const std::string & id) const
                OPENVRML_THROW1(openvrml::unsupported_interface);
            virtual openvrml::event_listener &
            event_listener(openvrml::node & node, const std::string & id) const
                OPENVRML_THROW1(openvrml::unsupported_interface);
            virtual openvrml::event_emitter &
            event_emitter(openvrml::node & node, const std::string & id) const
                OPENVRML_THROW1(openvrml::unsupported_interface);

        private:
            virtual const openvrml::node_interface_set & do_interfaces() const
                OPENVRML_NOTHROW;
            virtual const boost::intrusive_ptr<openvrml::node>
            do_create_node(
                const boost::shared_ptr<openvrml::scope> & scope,
                const openvrml::initial_value_map & initial_values) const
                OPENVRML_THROW3(openvrml::unsupported_interface, std::bad_cast,
                                std::bad_alloc);

            const openvrml::field_value &
            do_field_value(const Node & node, const std::string & id) const
                OPENVRML_THROW1(openvrml::unsupported_interface);
            openvrml::event_listener &
            do_event_listener(Node & node, const std::string & id) const
                OPENVRML_THROW1(openvrml::unsupported_interface);
            openvrml::event_emitter &
            do_event_emitter(Node & node, const std::string & id) const
                OPENVRML_THROW1(openvrml::unsupported_interface);
        };


        template <typename Node>
        class event_listener_base : public virtual node_event_listener {
        public:
            virtual ~event_listener_base() OPENVRML_NOTHROW = 0;

        protected:
            explicit event_listener_base(openvrml::node & n) OPENVRML_NOTHROW;

        private:
            typedef node_type_impl<Node> node_type_t;
            typedef typename node_type_t::event_listener_map_t
            event_listener_map_t;

            struct event_listener_equal_to :
                std::unary_function<typename event_listener_map_t::value_type,
                                    bool> {
                explicit event_listener_equal_to(
                    const node_event_listener & listener):
                    listener_(&listener)
                {}

                bool operator()(
                    const typename event_listener_equal_to::argument_type & arg)
                    const
                {
                    Node & n = dynamic_cast<Node &>(this->listener_->node());
                    //
                    // This static_cast shouldn't be necessary.  However,
                    // Microsoft cl 13.10.3077 will not generate correct code
                    // without it.
                    //
                    return static_cast<const openvrml::event_listener *>(
                        this->listener_) == &arg.second->deref(n);
                }

            private:
                const node_event_listener * listener_;
            };

            virtual const std::string do_eventin_id() const OPENVRML_NOTHROW;
        };

        template <typename Node>
        event_listener_base<Node>::event_listener_base(openvrml::node & n)
            OPENVRML_NOTHROW:
            node_event_listener(n)
        {}

        template <typename Node>
        event_listener_base<Node>::~event_listener_base() OPENVRML_NOTHROW
        {}

        template <typename Node>
        const std::string
        event_listener_base<Node>::do_eventin_id() const OPENVRML_NOTHROW
        {
            const node_type_t & node_type =
                static_cast<const node_type_t &>(this->node().type());
            const typename node_type_t::event_listener_map_t &
                event_listener_map = node_type.event_listener_map;
            const typename node_type_t::event_listener_map_t::const_iterator
                pos = std::find_if(event_listener_map.begin(),
                                   event_listener_map.end(),
                                   event_listener_equal_to(*this));
            assert(pos != event_listener_map.end());
            return pos->first;
        }


        template <typename Node>
        class event_emitter_base : public virtual event_emitter {
            openvrml::node * node_;

        public:
            virtual ~event_emitter_base() OPENVRML_NOTHROW = 0;

            openvrml::node & node() const OPENVRML_NOTHROW;

        protected:
            event_emitter_base(openvrml::node & n, const field_value & value)
                OPENVRML_NOTHROW;

        private:
            typedef node_type_impl<Node> node_type_t;
            typedef typename node_type_t::event_emitter_map_t
            event_emitter_map_t;

            struct event_emitter_equal_to :
                std::unary_function<typename event_emitter_map_t::value_type,
                                    bool> {
                explicit event_emitter_equal_to(
                    const event_emitter_base<Node> & emitter):
                    emitter_(&emitter)
                {}

                bool operator()(
                    const typename event_emitter_equal_to::argument_type & arg)
                    const
                {
                    Node & n = dynamic_cast<Node &>(this->emitter_->node());
                    return this->emitter_ ==
                        &dynamic_cast<event_emitter_base<Node> &>(
                            arg.second->deref(n));
                }

            private:
                const event_emitter_base<Node> * emitter_;
            };

            virtual const std::string do_eventout_id() const OPENVRML_NOTHROW;
        };

        template <typename Node>
        event_emitter_base<Node>::
        event_emitter_base(openvrml::node & n, const field_value & value)
            OPENVRML_NOTHROW:
            event_emitter(value),
            node_(&n)
        {}

        template <typename Node>
        event_emitter_base<Node>::~event_emitter_base() OPENVRML_NOTHROW
        {}

        template <typename Node>
        openvrml::node &
        event_emitter_base<Node>::node() const OPENVRML_NOTHROW
        {
            return *this->node_;
        }

        template <typename Node>
        const std::string
        event_emitter_base<Node>::do_eventout_id() const OPENVRML_NOTHROW
        {
            const node_type_t & node_type =
                static_cast<const node_type_t &>(this->node().type());
            const typename node_type_t::event_emitter_map_t &
                event_emitter_map = node_type.event_emitter_map;
            const typename node_type_t::event_emitter_map_t::const_iterator
                pos = std::find_if(event_emitter_map.begin(),
                                   event_emitter_map.end(),
                                   event_emitter_equal_to(*this));
            assert(pos != event_emitter_map.end());
            return pos->first;
        }


        template <typename Derived>
        class abstract_node : public virtual node {
        public:
            virtual ~abstract_node() OPENVRML_NOTHROW = 0;

        protected:
            typedef Derived self_t;

            typedef node_field_value_listener<sfbool> sfbool_listener;
            typedef node_field_value_listener<sfcolor> sfcolor_listener;
            typedef node_field_value_listener<sffloat> sffloat_listener;
            typedef node_field_value_listener<sfdouble> sfdouble_listener;
            typedef node_field_value_listener<sfimage> sfimage_listener;
            typedef node_field_value_listener<sfint32> sfint32_listener;
            typedef node_field_value_listener<sfnode> sfnode_listener;
            typedef node_field_value_listener<sfrotation> sfrotation_listener;
            typedef node_field_value_listener<sfstring> sfstring_listener;
            typedef node_field_value_listener<sftime> sftime_listener;
            typedef node_field_value_listener<sfvec2f> sfvec2f_listener;
            typedef node_field_value_listener<sfvec2d> sfvec2d_listener;
            typedef node_field_value_listener<sfvec3f> sfvec3f_listener;
            typedef node_field_value_listener<sfvec3d> sfvec3d_listener;
            typedef node_field_value_listener<mfcolor> mfcolor_listener;
            typedef node_field_value_listener<mffloat> mffloat_listener;
            typedef node_field_value_listener<mfdouble> mfdouble_listener;
            typedef node_field_value_listener<mfint32> mfint32_listener;
            typedef node_field_value_listener<mfnode> mfnode_listener;
            typedef node_field_value_listener<mfrotation> mfrotation_listener;
            typedef node_field_value_listener<mfstring> mfstring_listener;
            typedef node_field_value_listener<mftime> mftime_listener;
            typedef node_field_value_listener<mfvec2f> mfvec2f_listener;
            typedef node_field_value_listener<mfvec2d> mfvec2d_listener;
            typedef node_field_value_listener<mfvec3f> mfvec3f_listener;
            typedef node_field_value_listener<mfvec3d> mfvec3d_listener;

            template <typename FieldValue>
            class event_emitter :
                public event_emitter_base<Derived>,
                public openvrml::field_value_emitter<FieldValue> {
            public:
                event_emitter(openvrml::node & node, const FieldValue & value);
                virtual ~event_emitter() OPENVRML_NOTHROW;
            };

            typedef event_emitter<sfbool> sfbool_emitter;
            typedef event_emitter<sfcolor> sfcolor_emitter;
            typedef event_emitter<sfdouble> sfdouble_emitter;
            typedef event_emitter<sffloat> sffloat_emitter;
            typedef event_emitter<sfimage> sfimage_emitter;
            typedef event_emitter<sfint32> sfint32_emitter;
            typedef event_emitter<sfnode> sfnode_emitter;
            typedef event_emitter<sfrotation> sfrotation_emitter;
            typedef event_emitter<sfstring> sfstring_emitter;
            typedef event_emitter<sftime> sftime_emitter;
            typedef event_emitter<sfvec2d> sfvec2d_emitter;
            typedef event_emitter<sfvec2f> sfvec2f_emitter;
            typedef event_emitter<sfvec3d> sfvec3d_emitter;
            typedef event_emitter<sfvec3f> sfvec3f_emitter;
            typedef event_emitter<mfcolor> mfcolor_emitter;
            typedef event_emitter<mfdouble> mfdouble_emitter;
            typedef event_emitter<mffloat> mffloat_emitter;
            typedef event_emitter<mfnode> mfnode_emitter;
            typedef event_emitter<mfrotation> mfrotation_emitter;
            typedef event_emitter<mfstring> mfstring_emitter;
            typedef event_emitter<mftime> mftime_emitter;
            typedef event_emitter<mfvec2d> mfvec2d_emitter;
            typedef event_emitter<mfvec2f> mfvec2f_emitter;
            typedef event_emitter<mfvec3d> mfvec3d_emitter;
            typedef event_emitter<mfvec3f> mfvec3f_emitter;

            template <typename FieldValue>
            class exposedfield : public event_listener_base<Derived>,
                                 public event_emitter_base<Derived>,
                                 public openvrml::exposedfield<FieldValue> {
            public:
                explicit exposedfield(
                    openvrml::node & node,
                    const typename FieldValue::value_type & value =
#   if defined(_MSC_VER) && (_MSC_VER == 1400)
                    FieldValue::value_type()
#   else
                    typename FieldValue::value_type()
#   endif
                    );
                exposedfield(const exposedfield<FieldValue> & obj)
                    OPENVRML_NOTHROW;
                virtual ~exposedfield() OPENVRML_NOTHROW;

            private:
                virtual std::auto_ptr<field_value> do_clone() const
                    OPENVRML_THROW1(std::bad_alloc);
            };

            exposedfield<sfnode> metadata;

            abstract_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);

        private:
            virtual const field_value & do_field(const std::string & id) const
                OPENVRML_THROW1(unsupported_interface);

            virtual openvrml::event_listener &
            do_event_listener(const std::string & id)
                OPENVRML_THROW1(unsupported_interface);

            virtual openvrml::event_emitter &
            do_event_emitter(const std::string & id)
                OPENVRML_THROW1(unsupported_interface);
        };

        template <typename Derived>
        template <typename FieldValue>
        abstract_node<Derived>::event_emitter<FieldValue>::
        event_emitter(openvrml::node & node, const FieldValue & value):
            openvrml::event_emitter(value),
            event_emitter_base<Derived>(node, value),
            field_value_emitter<FieldValue>(value)
        {}

        template <typename Derived>
        template <typename FieldValue>
        abstract_node<Derived>::event_emitter<FieldValue>::~event_emitter()
            OPENVRML_NOTHROW
        {}

        template <typename Derived>
        template <typename FieldValue>
        abstract_node<Derived>::exposedfield<FieldValue>::
        exposedfield(openvrml::node & node,
                     const typename FieldValue::value_type & value):
            node_event_listener(node),
            openvrml::event_emitter(static_cast<const field_value &>(*this)),
            node_field_value_listener<FieldValue>(node),
            event_listener_base<Derived>(node),
            event_emitter_base<Derived>(
                node,
                static_cast<const field_value &>(*this)),
            openvrml::exposedfield<FieldValue>(node, value)
        {}

        template <typename Derived>
        template <typename FieldValue>
        abstract_node<Derived>::exposedfield<FieldValue>::
        exposedfield(const exposedfield<FieldValue> & obj) OPENVRML_NOTHROW:
            openvrml::event_listener(),
            node_event_listener(obj.node_event_listener::node()),
            openvrml::event_emitter(static_cast<const field_value &>(*this)),
            node_field_value_listener<FieldValue>(
                obj.node_event_listener::node()),
            event_listener_base<Derived>(
                obj.event_listener_base<Derived>::node()),
            event_emitter_base<Derived>(
                obj.event_listener_base<Derived>::node(),
                static_cast<const field_value &>(*this)),
            openvrml::exposedfield<FieldValue>(obj)
        {}

        template <typename Derived>
        template <typename FieldValue>
        abstract_node<Derived>::exposedfield<FieldValue>::
        ~exposedfield() OPENVRML_NOTHROW
        {}

        template <typename Derived>
        template <typename FieldValue>
        std::auto_ptr<field_value>
        abstract_node<Derived>::exposedfield<FieldValue>::do_clone() const
            OPENVRML_THROW1(std::bad_alloc)
        {
            return std::auto_ptr<field_value>(
                new exposedfield<FieldValue>(*this));
        }

        template <typename Derived>
        abstract_node<Derived>::
        abstract_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
            node(type, scope),
            metadata(*this)
        {}

        template <typename Derived>
        abstract_node<Derived>::~abstract_node() OPENVRML_NOTHROW
        {}

        template <typename Derived>
        const field_value &
        abstract_node<Derived>::do_field(const std::string & id) const
            OPENVRML_THROW1(unsupported_interface)
        {
            using boost::polymorphic_downcast;
            const abstract_node_type & type =
                *polymorphic_downcast<const abstract_node_type *>(
                    &this->type());
            return type.field_value(*this, id);
        }

        template <typename Derived>
        event_listener &
        abstract_node<Derived>::do_event_listener(const std::string & id)
            OPENVRML_THROW1(unsupported_interface)
        {
            using boost::polymorphic_downcast;
            const abstract_node_type & type =
                *polymorphic_downcast<const abstract_node_type *>(
                    &this->type());
            return type.event_listener(*this, id);
        }

        template <typename Derived>
        event_emitter &
        abstract_node<Derived>::do_event_emitter(const std::string & id)
            OPENVRML_THROW1(unsupported_interface)
        {
            using boost::polymorphic_downcast;
            const abstract_node_type & type =
                *polymorphic_downcast<const abstract_node_type *>(
                    &this->type());
            return type.event_emitter(*this, id);
        }


        template <typename Node>
        class node_field_ptr {
        public:
            virtual ~node_field_ptr() = 0;
            virtual openvrml::field_value & dereference(Node & obj) = 0;
            virtual const openvrml::field_value &
            dereference(const Node & obj) = 0;
        };

        template <typename Node>
        node_field_ptr<Node>::~node_field_ptr()
        {}


        template <typename Node, typename ConcreteFieldValue>
        class node_field_ptr_impl : public node_field_ptr<Node> {
            ConcreteFieldValue Node::* itsPtr;

        public:
            node_field_ptr_impl(ConcreteFieldValue Node::* ptr):
                itsPtr(ptr)
                {}

            virtual ~node_field_ptr_impl();

            virtual openvrml::field_value & dereference(Node &);
            virtual const openvrml::field_value & dereference(const Node &);
        };

        template <typename Node, typename ConcreteFieldValue>
        node_field_ptr_impl<Node, ConcreteFieldValue>::~node_field_ptr_impl()
        {}

        template <typename Node, typename ConcreteFieldValue>
        openvrml::field_value &
        node_field_ptr_impl<Node, ConcreteFieldValue>::
        dereference(Node & obj)
        {
            return obj.*itsPtr;
        }

        template <typename Node, typename ConcreteFieldValue>
        const openvrml::field_value &
        node_field_ptr_impl<Node, ConcreteFieldValue>::
        dereference(const Node & obj)
        {
            return obj.*itsPtr;
        }


        template <typename Node>
        class event_listener_ptr {
        public:
            virtual ~event_listener_ptr() OPENVRML_NOTHROW = 0;
            virtual openvrml::event_listener &
            dereference(Node & obj) OPENVRML_NOTHROW = 0;
        };

        template <typename Node>
        inline event_listener_ptr<Node>::~event_listener_ptr() OPENVRML_NOTHROW
        {}


        template <typename Node, typename ConcreteEventListener>
        class event_listener_ptr_impl :
            public event_listener_ptr<Node> {
            ConcreteEventListener Node::* its_ptr;

        public:
            event_listener_ptr_impl(ConcreteEventListener Node::* ptr)
                OPENVRML_NOTHROW:
                its_ptr(ptr)
                {}

            virtual ~event_listener_ptr_impl() OPENVRML_NOTHROW;

            virtual openvrml::event_listener & dereference(Node &)
                OPENVRML_NOTHROW;
        };

        template <typename Node, typename ConcreteEventListener>
        inline event_listener_ptr_impl<Node, ConcreteEventListener>::
        ~event_listener_ptr_impl() OPENVRML_NOTHROW
        {}

        template <typename Node, typename ConcreteEventListener>
        inline openvrml::event_listener &
        event_listener_ptr_impl<Node, ConcreteEventListener>::
        dereference(Node & obj) OPENVRML_NOTHROW
        {
            return obj.*its_ptr;
        }


        template <typename Node>
        class event_emitter_ptr {
        public:
            virtual ~event_emitter_ptr() OPENVRML_NOTHROW = 0;
            virtual openvrml::event_emitter & dereference(Node & obj)
                OPENVRML_NOTHROW = 0;
        };

        template <typename Node>
        inline event_emitter_ptr<Node>::~event_emitter_ptr() OPENVRML_NOTHROW
        {}


        template <typename Node, typename ConcreteEventEmitter>
        class event_emitter_ptr_impl : public event_emitter_ptr<Node> {
            ConcreteEventEmitter Node::* its_ptr;

        public:
            event_emitter_ptr_impl(ConcreteEventEmitter Node::* ptr)
                OPENVRML_NOTHROW;
            virtual ~event_emitter_ptr_impl() OPENVRML_NOTHROW;

            virtual openvrml::event_emitter & dereference(Node &)
                OPENVRML_NOTHROW;
        };

        template <typename Node, typename ConcreteEventEmitter>
        inline event_emitter_ptr_impl<Node, ConcreteEventEmitter>::
        event_emitter_ptr_impl(ConcreteEventEmitter Node::* ptr)
            OPENVRML_NOTHROW:
            its_ptr(ptr)
        {}

        template <typename Node, typename ConcreteEventEmitter>
        inline event_emitter_ptr_impl<Node, ConcreteEventEmitter>::
        ~event_emitter_ptr_impl()
            OPENVRML_NOTHROW
        {}

        template <typename Node, typename ConcreteEventEmitter>
        inline openvrml::event_emitter &
        event_emitter_ptr_impl<Node, ConcreteEventEmitter>::
        dereference(Node & obj)
            OPENVRML_NOTHROW
        {
            return obj.*its_ptr;
        }

        template <typename Node>
        template <typename FieldMember>
        node_type_impl<Node>::field_ptr<FieldMember>::
        field_ptr(FieldMember Node::* ptr_to_mem):
            ptr_to_polymorphic_mem_impl<openvrml::field_value,
                                        FieldMember,
                                        Node>(ptr_to_mem)
        {}

        template <typename Node>
        template <typename EventListenerMember>
        node_type_impl<Node>::event_listener_ptr<EventListenerMember>::
        event_listener_ptr(EventListenerMember Node::* ptr_to_mem):
            ptr_to_polymorphic_mem_impl<openvrml::event_listener,
                                        EventListenerMember,
                                        Node>(ptr_to_mem)
        {}

        template <typename Node>
        template <typename EventEmitterMember>
        node_type_impl<Node>::event_emitter_ptr<EventEmitterMember>::
        event_emitter_ptr(EventEmitterMember Node::* ptr_to_mem):
            ptr_to_polymorphic_mem_impl<openvrml::event_emitter,
                                        EventEmitterMember,
                                        Node>(ptr_to_mem)
        {}

        template <typename Node>
        node_type_impl<Node>::
        node_type_impl(const openvrml::node_metatype & metatype,
                       const std::string & id):
            abstract_node_type(metatype, id)
        {}

        template <typename Node>
        node_type_impl<Node>::~node_type_impl() OPENVRML_NOTHROW
        {}

        template <typename Node>
        void node_type_impl<Node>::
        add_eventin(const openvrml::field_value::type_id type,
                    const std::string & id,
                    const event_listener_ptr_ptr & event_listener)
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
        {
            using openvrml::node_interface;

            const node_interface interface(node_interface::eventin_id,
                                           type,
                                           id);
            bool succeeded = this->interfaces_.insert(interface).second;
            if (!succeeded) {
                throw std::invalid_argument("interface \"" + id + "\" already "
                                            "defined for " + this->id()
                                            + " node");
            }
            const typename event_listener_map_t::value_type
                value(id, event_listener);
            succeeded = this->event_listener_map.insert(value).second;
            assert(succeeded);
        }

        template <typename Node>
        void node_type_impl<Node>::
        add_eventout(const openvrml::field_value::type_id type,
                     const std::string & id,
                     const event_emitter_ptr_ptr & event_emitter)
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
        {
            using openvrml::node_interface;

            const node_interface interface(node_interface::eventout_id,
                                           type,
                                           id);
            bool succeeded = this->interfaces_.insert(interface).second;
            if (!succeeded) {
                throw std::invalid_argument("interface \"" + id + "\" already "
                                            "defined for " + this->id()
                                            + " node");
            }
            const typename event_emitter_map_t::value_type
                value(id, event_emitter);
            succeeded = this->event_emitter_map.insert(value).second;
            assert(succeeded);
        }

        template <typename Node>
        void node_type_impl<Node>::add_exposedfield(
            const openvrml::field_value::type_id type,
            const std::string & id,
            const event_listener_ptr_ptr & event_listener,
            const field_ptr_ptr & field,
            const event_emitter_ptr_ptr & event_emitter)
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
        {
            using openvrml::node_interface;

            const node_interface interface(node_interface::exposedfield_id,
                                           type,
                                           id);
            bool succeeded = this->interfaces_.insert(interface).second;
            if (!succeeded) {
                throw std::invalid_argument("interface \"" + id + "\" already "
                                            "defined for " + this->id()
                                            + " node");
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

        template <typename Node>
        void node_type_impl<Node>::add_field(
            const openvrml::field_value::type_id type,
            const std::string & id,
            const field_ptr_ptr & nodeFieldPtrPtr)
            OPENVRML_THROW2(std::invalid_argument, std::bad_alloc)
        {
            using openvrml::node_interface;

            const node_interface interface(node_interface::field_id, type, id);
            bool succeeded = this->interfaces_.insert(interface).second;
            if (!succeeded) {
                throw std::invalid_argument("interface \"" + id + "\" already "
                                            "defined for " + this->id()
                                            + " node");
            }
            const typename field_value_map_t::value_type
                value(id, nodeFieldPtrPtr);
            succeeded = this->field_value_map.insert(value).second;
            assert(succeeded);
        }

        template <typename Node>
        const openvrml::field_value &
        node_type_impl<Node>::field_value(const openvrml::node & node,
                                                 const std::string & id) const
            OPENVRML_THROW1(openvrml::unsupported_interface)
        {
            assert(dynamic_cast<const Node *>(&node));
            return this->do_field_value(dynamic_cast<const Node &>(node), id);
        }

        template <typename Node>
        const openvrml::field_value &
        node_type_impl<Node>::
        do_field_value(const Node & node, const std::string & id) const
            OPENVRML_THROW1(openvrml::unsupported_interface)
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

        template <typename Node>
        openvrml::event_listener &
        node_type_impl<Node>::
        event_listener(openvrml::node & node, const std::string & id) const
            OPENVRML_THROW1(openvrml::unsupported_interface)
        {
            assert(dynamic_cast<Node *>(&node));
            return this->do_event_listener(dynamic_cast<Node &>(node), id);
        }

        template <typename Node>
        openvrml::event_listener &
        node_type_impl<Node>::
        do_event_listener(Node & node, const std::string & id) const
            OPENVRML_THROW1(openvrml::unsupported_interface)
        {
            using namespace openvrml;

            const typename event_listener_map_t::const_iterator end =
                this->event_listener_map.end();
            typename event_listener_map_t::const_iterator pos =
                this->event_listener_map.find(id);
            if (pos == end) {
                pos = this->event_listener_map.find("set_" + id);
            }
            if (pos == end) {
                throw unsupported_interface(node.node::type(),
                                            node_interface::eventin_id,
                                            id);
            }
            return pos->second->deref(node);
        }

        template <typename Node>
        openvrml::event_emitter &
        node_type_impl<Node>::
        event_emitter(openvrml::node & node, const std::string & id) const
            OPENVRML_THROW1(openvrml::unsupported_interface)
        {
            assert(dynamic_cast<Node *>(&node));
            return this->do_event_emitter(dynamic_cast<Node &>(node), id);
        }

        template <typename Node>
        openvrml::event_emitter &
        node_type_impl<Node>::
        do_event_emitter(Node & node, const std::string & id) const
            OPENVRML_THROW1(openvrml::unsupported_interface)
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

        template <typename Node>
        const openvrml::node_interface_set &
        node_type_impl<Node>::do_interfaces() const
            OPENVRML_NOTHROW
        {
            return this->interfaces_;
        }

        template <typename Node>
        const boost::intrusive_ptr<openvrml::node>
        node_type_impl<Node>::
        do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
                       const openvrml::initial_value_map & initial_values)
            const
            OPENVRML_THROW3(openvrml::unsupported_interface, std::bad_cast,
                            std::bad_alloc)
        {
            using namespace openvrml;

            Node * const concrete_node_ptr = new Node(*this, scope);
            const boost::intrusive_ptr<node> result(concrete_node_ptr);
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


        template <typename BindableNode>
        class bound_node_stack : private std::stack<BindableNode *> {
            typedef typename std::stack<BindableNode *>::container_type
                container_type;

        public:
            using std::stack<BindableNode *>::empty;
            using std::stack<BindableNode *>::top;

            bool bind(BindableNode & node, double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
            bool unbind(BindableNode & node, double timestamp)
                OPENVRML_THROW1(std::bad_alloc);
        };

        template <typename BindableNode>
        bool bound_node_stack<BindableNode>::bind(BindableNode & n,
                                                  const double timestamp)
            OPENVRML_THROW1(std::bad_alloc)
        {
            //
            // If the node is already the active node, do nothing.
            //
            if (!this->empty() && (&n == this->top())) { return false; }

            //
            // If the node is already on the stack, remove it.
            //
            const typename container_type::iterator pos =
                std::find(this->c.begin(), this->c.end(), &n);
            if (pos != this->c.end()) { this->c.erase(pos); }

            //
            // Send FALSE from the currently active node's isBound.
            //
            if (!this->empty()) {
                BindableNode & current = *this->top();
                current.bind(false, timestamp);
            }

            //
            // Push the node to the top of the stack and have it send
            // isBound TRUE.
            //
            this->push(&n);
            n.bind(true, timestamp);

            return true;
        }

        template <typename BindableNode>
        bool bound_node_stack<BindableNode>::unbind(BindableNode & n,
                                                    const double timestamp)
            OPENVRML_THROW1(std::bad_alloc)
        {
            const typename container_type::iterator pos =
                std::find(this->c.begin(), this->c.end(), &n);

            if (pos == this->c.end()) { return false; }

            n.bind(false, timestamp);
            BindableNode & old_active = *this->top();
            this->c.erase(pos);
            if (&n == &old_active && this->size() > 1) {
                BindableNode & new_active = *this->top();
                new_active.bind(true, timestamp);
            }
            return true;
        }
    }
}

# endif // ifndef OPENVRML_NODE_IMPL
