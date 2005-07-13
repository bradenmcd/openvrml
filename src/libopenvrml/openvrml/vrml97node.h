// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 1999  Kumaran Santhanam
// Copyright 2001, 2002, 2003, 2004, 2005  Braden McDaniel
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

# ifndef OPENVRML_VRML97NODE_H
#   define OPENVRML_VRML97NODE_H

#   include <openvrml/bounding_volume.h>
#   include <openvrml/exposedfield.h>

typedef unsigned int FT_UInt;
typedef struct FT_LibraryRec_ * FT_Library;
typedef struct FT_FaceRec_ * FT_Face;
typedef unsigned int FcChar32;

template void std::vector<openvrml::node_ptr>::pop_back();

namespace openvrml {

    namespace vrml97_node {

        class scope_guard_impl_base {
        protected:
            mutable bool dismissed;

        public:
            void dismiss() const throw ();

        protected:
            scope_guard_impl_base() throw ();
            scope_guard_impl_base(const scope_guard_impl_base &) throw ();

        private:
            // Disable assignment.
            scope_guard_impl_base & operator=(const scope_guard_impl_base &);
        };

        typedef const scope_guard_impl_base & scope_guard;

        inline scope_guard_impl_base::scope_guard_impl_base() throw ():
            dismissed(false)
        {}

        inline scope_guard_impl_base::scope_guard_impl_base(
            const scope_guard_impl_base & scope_guard) throw ():
            dismissed(scope_guard.dismissed)
        {
            scope_guard.dismiss();
        }

        inline void scope_guard_impl_base::dismiss() const throw ()
        {
            this->dismissed = true;
        }

        template <typename Function, typename Param>
        class scope_guard_impl1 : public scope_guard_impl_base {
            Function function;
            const Param param;

        public:
            scope_guard_impl1(const Function & function, const Param & param);
            ~scope_guard_impl1();
        };

        template <typename Function, typename Param>
        scope_guard_impl1<Function, Param>::
        scope_guard_impl1(const Function & function, const Param & param):
            function(function),
            param(param)
        {}

        template <typename Function, typename Param>
        scope_guard_impl1<Function, Param>::~scope_guard_impl1()
        {
            if (!this->dismissed) { this->function(this->param); }
        }

        template <typename Function, typename Param>
        scope_guard_impl1<Function, Param>
        make_guard(const Function & function, const Param & param)
        {
            return scope_guard_impl1<Function, Param>(function, param);
        }

        template <typename Function,
                  typename Param1,
                  typename Param2,
                  typename Param3>
        class scope_guard_impl3 : public scope_guard_impl_base {
            Function function;
            const Param1 param1;
            const Param2 param2;
            const Param3 param3;

        public:
            scope_guard_impl3(const Function & function,
                              const Param1 & param1,
                              const Param2 & param2,
                              const Param3 & param3);
            ~scope_guard_impl3();
        };

        template <typename Function,
                  typename Param1,
                  typename Param2,
                  typename Param3>
        scope_guard_impl3<Function, Param1, Param2, Param3>::
        scope_guard_impl3(const Function & function,
                          const Param1 & param1,
                          const Param2 & param2,
                          const Param3 & param3):
            function(function),
            param1(param1),
            param2(param2),
            param3(param3)
        {}

        template <typename Function,
                  typename Param1,
                  typename Param2,
                  typename Param3>
        scope_guard_impl3<Function, Param1, Param2, Param3>::
        ~scope_guard_impl3()
        {
            if (!this->dismissed) {
                this->function(this->param1, this->param2, this->param3);
            }
        }

        template <typename Function,
                  typename Param1,
                  typename Param2,
                  typename Param3>
        scope_guard_impl3<Function, Param1, Param2, Param3>
        make_guard(const Function & function,
                   const Param1 & param1,
                   const Param2 & param2,
                   const Param3 & param3)
        {
            return scope_guard_impl3<Function, Param1, Param2, Param3>(
                function, param1, param2, param3);
        }

        template <typename Object, typename MemberFunction>
        class obj_scope_guard_impl0 : public scope_guard_impl_base {
            Object & obj;
            MemberFunction mem_fun;

        public:
            obj_scope_guard_impl0(Object & obj, MemberFunction mem_fun);
            ~obj_scope_guard_impl0();
        };

        template <typename Object, typename MemberFunction>
        inline
        obj_scope_guard_impl0<Object, MemberFunction>::
        obj_scope_guard_impl0(Object & obj, MemberFunction mem_fun):
            obj(obj),
            mem_fun(mem_fun)
        {}

        template <typename Object, typename MemberFunction>
        inline
        obj_scope_guard_impl0<Object, MemberFunction>::~obj_scope_guard_impl0()
        {
            if (!this->dismissed) { (this->obj.*this->mem_fun)(); }
        }

        template <typename Object, typename MemberFunction>
        obj_scope_guard_impl0<Object, MemberFunction>
        make_obj_guard(Object & obj, MemberFunction mem_fun)
        {
            return obj_scope_guard_impl0<Object, MemberFunction>(obj, mem_fun);
        }

        template <typename Object, typename MemberFunction, typename Param>
        class obj_scope_guard_impl1 : public scope_guard_impl_base {
            Object & obj;
            MemberFunction mem_fun;
            const Param param;

        public:
            obj_scope_guard_impl1(Object & obj,
                                  MemberFunction mem_fun,
                                  const Param & param) throw ();
            ~obj_scope_guard_impl1() throw ();
        };

        template <typename Object, typename MemberFunction, typename Param>
        inline
        obj_scope_guard_impl1<Object, MemberFunction, Param>::
        obj_scope_guard_impl1(Object & obj,
                              MemberFunction mem_fun,
                              const Param & param)
            throw ():
            obj(obj),
            mem_fun(mem_fun),
            param(param)
        {}

        template <typename Object, typename MemberFunction, typename Param>
        inline
        obj_scope_guard_impl1<Object, MemberFunction, Param>::
        ~obj_scope_guard_impl1() throw ()
        {
            if (!this->dismissed) { (this->obj.*this->mem_fun)(this->param); }
        }

        template <typename Object, typename MemberFunction, typename Param>
        obj_scope_guard_impl1<Object, MemberFunction, Param>
        make_obj_guard(Object & obj,
                       MemberFunction mem_fun,
                       const Param & param)
        {
            return obj_scope_guard_impl1<Object, MemberFunction, Param>(
                    obj, mem_fun, param);
        }


        template <typename MemberBase, typename Object>
        class ptr_to_polymorphic_mem {
        public:
            virtual ~ptr_to_polymorphic_mem() = 0;
            virtual MemberBase & deref(Object & obj) = 0;
            virtual const MemberBase & deref(const Object & obj) = 0;
        };

        template <typename MemberBase, typename Object>
        ptr_to_polymorphic_mem<MemberBase, Object>::~ptr_to_polymorphic_mem()
        {}


        template <typename MemberBase, typename Member, typename Object>
        class ptr_to_polymorphic_mem_impl :
            public ptr_to_polymorphic_mem<MemberBase, Object> {

            Member Object::* ptr_to_mem;

        public:
            explicit ptr_to_polymorphic_mem_impl(Member Object::* ptr_to_mem);
            virtual ~ptr_to_polymorphic_mem_impl();

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
        ~ptr_to_polymorphic_mem_impl()
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

        template <typename Node> class event_listener_base;
        template <typename Node> class event_emitter_base;

        template <typename Node>
        class vrml97_node_type_impl : public vrml97_node_type {
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
            vrml97_node_type_impl(const openvrml::node_class & node_class,
                                  const std::string & id);
            virtual ~vrml97_node_type_impl() throw ();

            void add_eventin(openvrml::field_value::type_id type,
                             const std::string & id,
                             const event_listener_ptr_ptr & event_listener)
                throw (std::invalid_argument, std::bad_alloc);
            void add_eventout(openvrml::field_value::type_id type,
                              const std::string & id,
                              const event_emitter_ptr_ptr & event_emitter)
                throw (std::invalid_argument, std::bad_alloc);
            void add_exposedfield(
                openvrml::field_value::type_id type,
                const std::string & id,
                const event_listener_ptr_ptr & event_listener,
                const field_ptr_ptr & field,
                const event_emitter_ptr_ptr & event_emitter)
                throw (std::invalid_argument, std::bad_alloc);
            void add_field(openvrml::field_value::type_id type,
                           const std::string & id,
                           const field_ptr_ptr & fieldPtrPtr)
                throw (std::invalid_argument, std::bad_alloc);

            virtual const openvrml::field_value &
            field_value(const openvrml::node & node,
                        const std::string & id) const
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
            do_field_value(const Node & node, const std::string & id) const
                throw (openvrml::unsupported_interface);
            openvrml::event_listener &
            do_event_listener(Node & node, const std::string & id) const
                throw (openvrml::unsupported_interface);
            openvrml::event_emitter &
            do_event_emitter(Node & node, const std::string & id) const
                throw (openvrml::unsupported_interface);
        };

        template <typename Node>
        class event_listener_base : public virtual event_listener {
        public:
            virtual ~event_listener_base() throw () = 0;

        protected:
            explicit event_listener_base(openvrml::node & n) throw ();

        private:
            typedef vrml97_node_type_impl<Node> node_type_t;
            typedef typename node_type_t::event_listener_map_t
                event_listener_map_t;

            struct event_listener_equal_to :
                std::unary_function<typename event_listener_map_t::value_type,
                                    bool> {
                explicit event_listener_equal_to(
                    const event_listener & listener):
                    listener_(&listener)
                {}

                bool operator()(
                    const typename event_listener_equal_to::argument_type & arg)
                    const
                {
                    Node & n = dynamic_cast<Node &>(this->listener_->node());
                    return this->listener_ == &arg.second->deref(n);
                }

            private:
                const event_listener * listener_;
            };

            virtual const std::string do_eventin_id() const throw ();
        };

        template <typename Node>
        event_listener_base<Node>::event_listener_base(openvrml::node & n)
            throw ():
            event_listener(n)
        {}

        template <typename Node>
        event_listener_base<Node>::~event_listener_base() throw ()
        {}

        template <typename Node>
        const std::string
        event_listener_base<Node>::do_eventin_id() const throw ()
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
            virtual ~event_emitter_base() throw () = 0;

            openvrml::node & node() const throw ();

        protected:
            event_emitter_base(openvrml::node & node,
                               const field_value & value)
                throw ();

        private:
            typedef vrml97_node_type_impl<Node> node_type_t;
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

            virtual const std::string do_eventout_id() const throw ();
        };

        template <typename Node>
        event_emitter_base<Node>::
        event_emitter_base(openvrml::node & node, const field_value & value)
            throw ():
            event_emitter(value),
            node_(&node)
        {}

        template <typename Node>
        event_emitter_base<Node>::~event_emitter_base() throw ()
        {}

        template <typename Node>
        openvrml::node &
        event_emitter_base<Node>::node() const throw ()
        {
            return *this->node_;
        }

        template <typename Node>
        const std::string
        event_emitter_base<Node>::do_eventout_id() const throw ()
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
        class abstract_base : public virtual node {
        public:
            virtual ~abstract_base() throw () = 0;

        protected:
            typedef Derived self_t;

            template <typename FieldValue>
            class event_emitter :
                public event_emitter_base<Derived>,
                public openvrml::field_value_emitter<FieldValue> {
            public:
                event_emitter(openvrml::node & node, const FieldValue & value);
                virtual ~event_emitter() throw ();
            };

            typedef event_emitter<sfbool> sfbool_emitter;
            typedef event_emitter<sfcolor> sfcolor_emitter;
            typedef event_emitter<sffloat> sffloat_emitter;
            typedef event_emitter<sfimage> sfimage_emitter;
            typedef event_emitter<sfnode> sfnode_emitter;
            typedef event_emitter<sfrotation> sfrotation_emitter;
            typedef event_emitter<sfstring> sfstring_emitter;
            typedef event_emitter<sftime> sftime_emitter;
            typedef event_emitter<sfvec2f> sfvec2f_emitter;
            typedef event_emitter<sfvec3f> sfvec3f_emitter;
            typedef event_emitter<mfcolor> mfcolor_emitter;
            typedef event_emitter<mffloat> mffloat_emitter;
            typedef event_emitter<mfnode> mfnode_emitter;
            typedef event_emitter<mfrotation> mfrotation_emitter;
            typedef event_emitter<mfstring> mfstring_emitter;
            typedef event_emitter<mftime> mftime_emitter;
            typedef event_emitter<mfvec2f> mfvec2f_emitter;
            typedef event_emitter<mfvec3f> mfvec3f_emitter;

            template <typename FieldValue>
            class exposedfield : public event_listener_base<Derived>,
                                 public event_emitter_base<Derived>,
                                 public openvrml::exposedfield<FieldValue> {
            public:
                explicit exposedfield(
                    openvrml::node & node,
                    const typename FieldValue::value_type & value =
                    typename FieldValue::value_type());
                virtual ~exposedfield() throw ();
            };


            abstract_base(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);

        private:
            virtual const field_value & do_field(const std::string & id) const
                throw (unsupported_interface);

            virtual openvrml::event_listener &
            do_event_listener(const std::string & id)
                throw (unsupported_interface);

            virtual openvrml::event_emitter &
            do_event_emitter(const std::string & id)
                throw (unsupported_interface);
        };

        template <typename Derived>
        template <typename FieldValue>
        abstract_base<Derived>::event_emitter<FieldValue>::
        event_emitter(openvrml::node & node, const FieldValue & value):
            openvrml::event_emitter(value),
            event_emitter_base<Derived>(node, value),
            field_value_emitter<FieldValue>(value)
        {}

        template <typename Derived>
        template <typename FieldValue>
        abstract_base<Derived>::event_emitter<FieldValue>::~event_emitter()
            throw ()
        {}

        template <typename Derived>
        template <typename FieldValue>
        abstract_base<Derived>::exposedfield<FieldValue>::
        exposedfield(openvrml::node & node,
                     const typename FieldValue::value_type & value):
            openvrml::event_listener(node),
            event_listener_base<Derived>(node),
            openvrml::event_emitter(static_cast<const field_value &>(*this)),
            event_emitter_base<Derived>(
                node,
                static_cast<const field_value &>(*this)),
            openvrml::exposedfield<FieldValue>(node, value)
        {}

        template <typename Derived>
        template <typename FieldValue>
        abstract_base<Derived>::exposedfield<FieldValue>::
        ~exposedfield() throw ()
        {}

        template <typename Derived>
        abstract_base<Derived>::
        abstract_base(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope):
            node(type, scope)
        {}

        template <typename Derived>
        abstract_base<Derived>::~abstract_base() throw ()
        {}

        template <typename Derived>
        const field_value &
        abstract_base<Derived>::do_field(const std::string & id) const
            throw (unsupported_interface)
        {
            using boost::polymorphic_downcast;
            const vrml97_node_type & type =
                *polymorphic_downcast<const vrml97_node_type *>(&this->type());
            return type.field_value(*this, id);
        }

        template <typename Derived>
        event_listener &
        abstract_base<Derived>::do_event_listener(const std::string & id)
            throw (unsupported_interface)
        {
            using boost::polymorphic_downcast;
            const vrml97_node_type & type =
                *polymorphic_downcast<const vrml97_node_type *>(&this->type());
            return type.event_listener(*this, id);
        }

        template <typename Derived>
        event_emitter &
        abstract_base<Derived>::do_event_emitter(const std::string & id)
            throw (unsupported_interface)
        {
            using boost::polymorphic_downcast;
            const vrml97_node_type & type =
                *polymorphic_downcast<const vrml97_node_type *>(&this->type());
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
            event_listener_ptr_impl(ConcreteEventListener Node::* ptr)
                throw ():
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
            virtual openvrml::event_emitter & dereference(Node & obj)
                throw () = 0;
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

        template <typename Node>
        template <typename FieldMember>
        vrml97_node_type_impl<Node>::field_ptr<FieldMember>::
        field_ptr(FieldMember Node::* ptr_to_mem):
            ptr_to_polymorphic_mem_impl<openvrml::field_value,
                                        FieldMember,
                                        Node>(ptr_to_mem)
        {}

        template <typename Node>
        template <typename EventListenerMember>
        vrml97_node_type_impl<Node>::event_listener_ptr<EventListenerMember>::
        event_listener_ptr(EventListenerMember Node::* ptr_to_mem):
            ptr_to_polymorphic_mem_impl<openvrml::event_listener,
                                        EventListenerMember,
                                        Node>(ptr_to_mem)
        {}

        template <typename Node>
        template <typename EventEmitterMember>
        vrml97_node_type_impl<Node>::event_emitter_ptr<EventEmitterMember>::
        event_emitter_ptr(EventEmitterMember Node::* ptr_to_mem):
            ptr_to_polymorphic_mem_impl<openvrml::event_emitter,
                                        EventEmitterMember,
                                        Node>(ptr_to_mem)
        {}

        template <typename Node>
        vrml97_node_type_impl<Node>::
        vrml97_node_type_impl(const openvrml::node_class & node_class,
                              const std::string & id):
            vrml97_node_type(node_class, id)
        {}

        template <typename Node>
        vrml97_node_type_impl<Node>::~vrml97_node_type_impl() throw ()
        {}

        template <typename Node>
        void vrml97_node_type_impl<Node>::
        add_eventin(const openvrml::field_value::type_id type,
                    const std::string & id,
                    const event_listener_ptr_ptr & event_listener)
            throw (std::invalid_argument, std::bad_alloc)
        {
            using openvrml::node_interface;

            const node_interface interface(node_interface::eventin_id,
                                           type,
                                           id);
            bool succeeded = this->interfaces_.insert(interface).second;
            if (!succeeded) {
                throw std::invalid_argument("Interface \"" + id + "\" already "
                                            "defined for " + this->id()
                                            + " node");
            }
            const typename event_listener_map_t::value_type
                value(id, event_listener);
            succeeded = this->event_listener_map.insert(value).second;
            assert(succeeded);
        }

        template <typename Node>
        void vrml97_node_type_impl<Node>::
        add_eventout(const openvrml::field_value::type_id type,
                     const std::string & id,
                     const event_emitter_ptr_ptr & event_emitter)
            throw (std::invalid_argument, std::bad_alloc)
        {
            using openvrml::node_interface;

            const node_interface interface(node_interface::eventout_id,
                                           type,
                                           id);
            bool succeeded = this->interfaces_.insert(interface).second;
            if (!succeeded) {
                throw std::invalid_argument("Interface \"" + id + "\" already "
                                            "defined for " + this->id()
                                            + " node");
            }
            const typename event_emitter_map_t::value_type
                value(id, event_emitter);
            succeeded = this->event_emitter_map.insert(value).second;
            assert(succeeded);
        }

        template <typename Node>
        void vrml97_node_type_impl<Node>::add_exposedfield(
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
        void vrml97_node_type_impl<Node>::add_field(
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
        vrml97_node_type_impl<Node>::field_value(const openvrml::node & node,
                                                  const std::string & id) const
            throw (openvrml::unsupported_interface)
        {
            assert(dynamic_cast<const Node *>(&node));
            return this->do_field_value(dynamic_cast<const Node &>(node), id);
        }

        template <typename Node>
        openvrml::event_listener &
        vrml97_node_type_impl<Node>::
        event_listener(openvrml::node & node, const std::string & id) const
            throw (openvrml::unsupported_interface)
        {
            assert(dynamic_cast<Node *>(&node));
            return this->do_event_listener(dynamic_cast<Node &>(node), id);
        }

        template <typename Node>
        openvrml::event_emitter &
        vrml97_node_type_impl<Node>::
        event_emitter(openvrml::node & node, const std::string & id) const
            throw (openvrml::unsupported_interface)
        {
            assert(dynamic_cast<Node *>(&node));
            return this->do_event_emitter(dynamic_cast<Node &>(node), id);
        }

        template <typename Node>
        const openvrml::node_interface_set &
        vrml97_node_type_impl<Node>::do_interfaces() const
            throw ()
        {
            return this->interfaces_;
        }

        template <typename Node>
        const openvrml::node_ptr
        vrml97_node_type_impl<Node>::
        do_create_node(const boost::shared_ptr<openvrml::scope> & scope,
                       const openvrml::initial_value_map & initial_values)
            const
            throw (openvrml::unsupported_interface, std::bad_cast,
                   std::bad_alloc)
        {
            using namespace openvrml;

            Node * const concrete_node_ptr = new Node(*this, scope);
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

        template <typename Node>
        const openvrml::field_value &
        vrml97_node_type_impl<Node>::
        do_field_value(const Node & node, const std::string & id) const
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

        template <typename Node>
        openvrml::event_listener &
        vrml97_node_type_impl<Node>::
        do_event_listener(Node & node, const std::string & id) const
            throw (openvrml::unsupported_interface)
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
        vrml97_node_type_impl<Node>::
        do_event_emitter(Node & node, const std::string & id) const
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


        template <typename Derived>
        class abstract_indexed_set_node : public abstract_base<Derived>,
                                          public geometry_node {
        protected:
            typedef typename abstract_base<Derived>::self_t self_t;

            class set_color_index_listener :
                public event_listener_base<self_t>,
                public mfint32_listener {
            public:
                explicit set_color_index_listener(
                    abstract_indexed_set_node & node);
                virtual ~set_color_index_listener() throw ();

            private:
                virtual void do_process_event(const mfint32 & color_index,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class set_coord_index_listener :
                public event_listener_base<self_t>,
                public mfint32_listener {
            public:
                explicit set_coord_index_listener(
                    abstract_indexed_set_node & node);
                virtual ~set_coord_index_listener() throw ();

            private:
                virtual void do_process_event(const mfint32 & coord_index,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_color_index_listener set_color_index_;
            set_coord_index_listener set_coord_index_;
            typename abstract_base<Derived>::
                template exposedfield<sfnode> color_;
            typename abstract_base<Derived>::
                template exposedfield<sfnode> coord_;
            mfint32 color_index_;
            sfbool color_per_vertex_;
            mfint32 coord_index_;

        public:
            virtual ~abstract_indexed_set_node() throw () = 0;

            virtual bool modified() const;

            virtual const color_node * color() const throw ();

        protected:
            abstract_indexed_set_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
        };

        template <typename Derived>
        abstract_indexed_set_node<Derived>::set_color_index_listener::
        set_color_index_listener(abstract_indexed_set_node & node):
            openvrml::event_listener(node),
            event_listener_base<Derived>(node),
            mfint32_listener(node)
        {}

        template <typename Derived>
        abstract_indexed_set_node<Derived>::set_color_index_listener::
        ~set_color_index_listener() throw ()
        {}

        template <typename Derived>
        void
        abstract_indexed_set_node<Derived>::set_color_index_listener::
        do_process_event(const mfint32 & color_index, const double timestamp)
            throw (std::bad_alloc)
        {
            abstract_indexed_set_node * abstract_indexed_set =
                dynamic_cast<abstract_indexed_set_node *>(&this->node());
            assert(abstract_indexed_set);
            abstract_indexed_set->color_index_ = color_index;
            abstract_indexed_set->node::modified(true);
        }

        template <typename Derived>
        abstract_indexed_set_node<Derived>::set_coord_index_listener::
        set_coord_index_listener(abstract_indexed_set_node & node):
            openvrml::event_listener(node),
            event_listener_base<Derived>(node),
            mfint32_listener(node)
        {}

        template <typename Derived>
        abstract_indexed_set_node<Derived>::set_coord_index_listener::
        ~set_coord_index_listener() throw ()
        {}

        template <typename Derived>
        void
        abstract_indexed_set_node<Derived>::set_coord_index_listener::
        do_process_event(const mfint32 & coord_index, const double timestamp)
            throw (std::bad_alloc)
        {
            abstract_indexed_set_node * abstract_indexed_set =
                dynamic_cast<abstract_indexed_set_node *>(&this->node());
            assert(abstract_indexed_set);
            abstract_indexed_set->coord_index_ = coord_index;
            abstract_indexed_set->node::modified(true);
        }

        template <typename Derived>
        abstract_indexed_set_node<Derived>::abstract_indexed_set_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope):
            node(type, scope),
            bounded_volume_node(type, scope),
            abstract_base<Derived>(type, scope),
            geometry_node(type, scope),
            set_color_index_(*this),
            set_coord_index_(*this),
            color_(*this),
            coord_(*this),
            color_per_vertex_(true)
        {}

        template <typename Derived>
        abstract_indexed_set_node<Derived>::~abstract_indexed_set_node()
            throw ()
        {}

        template <typename Derived>
        bool abstract_indexed_set_node<Derived>::modified() const
        {
            return this->node::modified()
                || (this->color_.sfnode::value
                    && this->color_.sfnode::value->modified())
                || (this->coord_.sfnode::value
                    && this->coord_.sfnode::value->modified());
        }

        template <typename Derived>
        const openvrml::color_node *
        abstract_indexed_set_node<Derived>::color() const throw ()
        {
            return node_cast<openvrml::color_node *>(
                this->color_.sfnode::value.get());
        }


        template <typename Derived>
        class abstract_light_node : public abstract_base<Derived>,
                                    public virtual light_node {
        protected:
            typename abstract_base<Derived>::template exposedfield<sffloat>
                ambient_intensity_;
            typename abstract_base<Derived>::template exposedfield<sfcolor>
                color_;
            typename abstract_base<Derived>::template exposedfield<sffloat>
                intensity_;
            typename abstract_base<Derived>::template exposedfield<sfbool> on_;

        public:
            virtual ~abstract_light_node() throw () = 0;

            float ambient_intensity() const throw ();
            float intensity() const throw ();
            bool on() const throw ();
            const openvrml::color & color() const throw ();

        protected:
            abstract_light_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
        };

        template <typename Derived>
        abstract_light_node<Derived>::
        abstract_light_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope):
            node(type, scope),
            bounded_volume_node(type, scope),
            abstract_base<Derived>(type, scope),
            child_node(type, scope),
            light_node(type, scope),
            ambient_intensity_(*this, 0.0),
            color_(*this, openvrml::color(1.0, 1.0, 1.0)),
            intensity_(*this, 1.0),
            on_(*this, true)
        {}

        template <typename Derived>
        abstract_light_node<Derived>::~abstract_light_node() throw ()
        {}

        template <typename Derived>
        float abstract_light_node<Derived>::ambient_intensity() const throw ()
        {
            return this->ambient_intensity_.sffloat::value;
        }

        template <typename Derived>
        float abstract_light_node<Derived>::intensity() const throw ()
        {
            return this->intensity_.sffloat::value;
        }

        template <typename Derived>
        bool abstract_light_node<Derived>::on() const throw ()
        {
            return this->on_.sfbool::value;
        }

        template <typename Derived>
        const openvrml::color &
        abstract_light_node<Derived>::color() const throw ()
        {
            return this->color_.sfcolor::value;
        }


        template <typename Derived>
        class abstract_texture_node : public abstract_base<Derived>,
                                      public texture_node {
        protected:
            sfbool repeat_s_;
            sfbool repeat_t_;

        public:
            virtual ~abstract_texture_node() throw () = 0;

            //
            // texture_node implementation.
            //
            virtual bool repeat_s() const throw ();
            virtual bool repeat_t() const throw ();

        protected:
            abstract_texture_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
        };

        template <typename Derived>
        abstract_texture_node<Derived>::
        abstract_texture_node(
            const node_type & type,
            const boost::shared_ptr<openvrml::scope> & scope):
            node(type, scope),
            abstract_base<Derived>(type, scope),
            texture_node(type, scope),
            repeat_s_(true),
            repeat_t_(true)
        {}

        template <typename Derived>
        abstract_texture_node<Derived>::~abstract_texture_node() throw ()
        {}

        template <typename Derived>
        bool abstract_texture_node<Derived>::repeat_s() const throw ()
        {
            return this->repeat_s_.value;
        }

        template <typename Derived>
        bool abstract_texture_node<Derived>::repeat_t() const throw ()
        {
            return this->repeat_t_.value;
        }


        template <typename Derived>
        class grouping_node_base : public abstract_base<Derived>,
                                   public virtual grouping_node {
            typedef typename abstract_base<Derived>::self_t self_t;

        protected:
            class add_children_listener : public event_listener_base<self_t>,
                                          public mfnode_listener {
            public:
                explicit add_children_listener(
                    grouping_node_base<Derived> & node);
                virtual ~add_children_listener() throw ();

            private:
                virtual void do_process_event(const mfnode & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class remove_children_listener :
                public event_listener_base<self_t>,
                public mfnode_listener {
            public:
                explicit remove_children_listener(
                    grouping_node_base<Derived> & node);
                virtual ~remove_children_listener() throw ();

            private:
                virtual void do_process_event(const mfnode & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class children_exposedfield :
                public abstract_base<Derived>::template exposedfield<mfnode> {
            public:
                explicit children_exposedfield(openvrml::node & node) throw ();
                virtual ~children_exposedfield() throw ();

            private:
                virtual void event_side_effect(const mfnode & value,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            sfvec3f bbox_center_;
            sfvec3f bbox_size_;
            add_children_listener add_children_listener_;
            remove_children_listener remove_children_listener_;
            children_exposedfield children_;

            viewer::object_t viewerObject;

            bounding_sphere bsphere;

        public:
            grouping_node_base(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~grouping_node_base() throw ();

            virtual bool modified() const;

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timeStamp, bool isOver, bool isActive,
                                  double *p);

            void render_nocull(openvrml::viewer & viewer,
                               rendering_context context);

        protected:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
            virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

            void recalc_bsphere();
        };

        template <typename Derived>
        grouping_node_base<Derived>::add_children_listener::
        add_children_listener(grouping_node_base<Derived> & node):
            openvrml::event_listener(node),
            event_listener_base<Derived>(node),
            mfnode_listener(node)
        {}

        template <typename Derived>
        grouping_node_base<Derived>::add_children_listener::
        ~add_children_listener() throw ()
        {}

        template <typename Derived>
        void
        grouping_node_base<Derived>::add_children_listener::
        do_process_event(const mfnode & value, const double timestamp)
            throw (std::bad_alloc)
        {
            using std::vector;

            Derived & group = dynamic_cast<Derived &>(this->node());

            for (vector<node_ptr>::const_iterator node = value.value.begin();
                 node != value.value.end();
                 ++node) {
                //
                // Don't add NULLs.
                //
                if (*node) {
                    using std::find;

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
                        if (child) {
                            child->relocate(); // Throws std::bad_alloc.
                        }
                        guard.dismiss();
                    }
                }
            }

            group.node::modified(true);
            group.bounding_volume_dirty(true);
            node::emit_event(group.children_, timestamp);
        }

        template <typename Derived>
        grouping_node_base<Derived>::
        remove_children_listener::
        remove_children_listener(grouping_node_base<Derived> & node):
            openvrml::event_listener(node),
            event_listener_base<Derived>(node),
            mfnode_listener(node)
        {}

        template <typename Derived>
        grouping_node_base<Derived>::remove_children_listener::
        ~remove_children_listener() throw ()
        {}

        template <typename Derived>
        void
        grouping_node_base<Derived>::remove_children_listener::
        do_process_event(const mfnode & value, const double timestamp)
            throw (std::bad_alloc)
        {
            using std::vector;

            self_t & group = dynamic_cast<self_t &>(this->node());

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

        template <typename Derived>
        grouping_node_base<Derived>::children_exposedfield::
        children_exposedfield(openvrml::node & node) throw ():
            openvrml::event_listener(node),
            openvrml::event_emitter(static_cast<const field_value &>(*this)),
            abstract_base<Derived>::template exposedfield<openvrml::mfnode>(
                node)
        {}

        template <typename Derived>
        grouping_node_base<Derived>::children_exposedfield::
        ~children_exposedfield() throw ()
        {}

        template <typename Derived>
        void
        grouping_node_base<Derived>::children_exposedfield::
        event_side_effect(const mfnode & value, const double timestamp)
            throw (std::bad_alloc)
        {
            using std::vector;

            self_t & group =
                dynamic_cast<self_t &>(this->event_listener::node());

            this->mfnode::value.clear();

            for (vector<node_ptr>::const_iterator node = value.value.begin();
                 node != value.value.end();
                 ++node) {
                //
                // The spec is ambiguous about whether the children field of
                // grouping nodes can contain NULLs. We allow it; for now, at
                // least.
                //
                this->mfnode::value.push_back(*node); // Throws std::bad_alloc.
                scope_guard guard =
                    make_obj_guard(this->mfnode::value,
                                   &vector<node_ptr>::pop_back);
                child_node * const child =
                    node_cast<child_node *>(node->get());
                if (child) { child->relocate(); } // Throws std::bad_alloc.
                guard.dismiss();
            }

            group.bounding_volume_dirty(true);
        }

        template <typename Derived>
        grouping_node_base<Derived>::
        grouping_node_base(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope):
            node(type, scope),
            bounded_volume_node(type, scope),
            child_node(type, scope),
            grouping_node(type, scope),
            abstract_base<Derived>(type, scope),
            bbox_size_(vec3f(-1.0, -1.0, -1.0)),
            add_children_listener_(*this),
            remove_children_listener_(*this),
            children_(*this),
            viewerObject(0)
        {
            this->bounding_volume_dirty(true);
        }

        template <typename Derived>
        grouping_node_base<Derived>::~grouping_node_base() throw ()
        {
            // delete viewerObject...
        }

        template <typename Derived>
        bool grouping_node_base<Derived>::modified() const
        {
            if (this->node::modified()) { return true; }
            for (size_t i = 0; i < this->children_.mfnode::value.size(); ++i) {
                if (this->children_.mfnode::value[i]->modified()) {
                    return true;
                }
            }
            return false;
        }

        template <typename Derived>
        void
        grouping_node_base<Derived>::
        do_render_child(openvrml::viewer & viewer, rendering_context context)
        {
            if (context.cull_flag != bounding_volume::inside) {
                assert(dynamic_cast<const bounding_sphere *>
                       (&this->bounding_volume()));
                const bounding_sphere & bs =
                    static_cast<const bounding_sphere &>(
                        this->bounding_volume());
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

        template <typename Derived>
        void
        grouping_node_base<Derived>::render_nocull(openvrml::viewer & viewer,
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
                vector<node_ptr>::size_type n =
                    this->children_.mfnode::value.size();
                size_t nSensors = 0;

                this->viewerObject = viewer.begin_object(this->id().c_str());

                // Draw nodes that impact their siblings (DirectionalLights,
                // TouchSensors, any others? ...)
                for (i = 0; i < n; ++i) {
                    child_node * const child =
                        node_cast<child_node *>(
                            this->children_.mfnode::value[i].get());
                    if (child) {
                        if (node_cast<light_node *>(child)
                            && !(node_cast<scoped_light_node *>(child))) {
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
                    if (child
                        && !(node_cast<light_node *>(child)
//                             || child->to_plane_sensor()
//                             || child->to_cylinder_sensor()
//                             || child->to_sphere_sensor()
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

        template <typename Derived>
        const std::vector<openvrml::node_ptr> &
        grouping_node_base<Derived>::children() const throw ()
        {
            return this->children_.mfnode::value;
        }

        template <typename Derived>
        void
        grouping_node_base<Derived>::activate(double time,
                                              bool isOver,
                                              bool isActive,
                                              double *p)
        {
            for (size_t i = 0; i < this->children_.mfnode::value.size(); ++i) {
                const node_ptr & node = this->children_.mfnode::value[i];
                if (node) {
                    if (node->to_touch_sensor()
                        && node->to_touch_sensor()->enabled()) {
                        node->to_touch_sensor()->activate(time,
                                                          isOver,
                                                          isActive,
                                                          p);
                    } else if (node->to_plane_sensor()
                               && node->to_plane_sensor()->enabled()) {
                        node->to_plane_sensor()->activate(time, isActive, p);
                    } else if (node->to_cylinder_sensor()
                               && node->to_cylinder_sensor()->enabled()) {
                        node->to_cylinder_sensor()->activate(time,
                                                             isActive,
                                                             p);
                    } else if (node->to_sphere_sensor()
                               && node->to_sphere_sensor()->isEnabled()) {
                        node->to_sphere_sensor()->activate(time, isActive, p);
                    }
                }
            }
        }

        template <typename Derived>
        const openvrml::bounding_volume &
        grouping_node_base<Derived>::do_bounding_volume() const
        {
            if (this->bounding_volume_dirty()) {
                const_cast<grouping_node_base<Derived> *>(this)
                    ->recalc_bsphere();
            }
            return this->bsphere;
        }

        template <typename Derived>
        void grouping_node_base<Derived>::recalc_bsphere()
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


        class anchor_class : public node_class {
        public:
            explicit anchor_class(openvrml::browser & browser);
            virtual ~anchor_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class anchor_node : public grouping_node_base<anchor_node> {
            friend class anchor_class;

            exposedfield<sfstring> description_;
            exposedfield<mfstring> parameter_;
            exposedfield<mfstring> url_;

        public:
            anchor_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~anchor_node() throw ();

            virtual anchor_node * to_anchor() const;

            void activate_anchor();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class appearance_class : public node_class {
        public:
            explicit appearance_class(openvrml::browser & browser);
            virtual ~appearance_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class appearance_node : public abstract_base<appearance_node>,
                                public openvrml::appearance_node {
            friend class appearance_class;

            exposedfield<sfnode> material_;
            exposedfield<sfnode> texture_;
            exposedfield<sfnode> texture_transform_;

        public:
            appearance_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~appearance_node() throw ();

            virtual bool modified() const;

            //
            // appearance_node implementation
            //
            virtual const node_ptr & material() const throw ();
            virtual const node_ptr & texture() const throw ();
            virtual const node_ptr & texture_transform() const throw ();

        private:
            virtual void do_render_appearance(viewer & v,
                                              rendering_context context);
        };


        class audio_clip_class : public node_class {
        public:
            explicit audio_clip_class(openvrml::browser & browser);
            virtual ~audio_clip_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class audio_clip_node : public abstract_base<audio_clip_node> {
            friend class audio_clip_class;

            exposedfield<sfstring> description_;
            exposedfield<sfbool> loop_;
            exposedfield<sffloat> pitch_;
            exposedfield<sftime> start_time_;
            exposedfield<sftime> stop_time_;
            exposedfield<mfstring> url_;
            sftime duration_changed_;
            sftime_emitter duration_changed_emitter_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;

        public:
            audio_clip_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~audio_clip_node() throw ();

            void update(double time);

            virtual audio_clip_node * to_audio_clip() const;

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();
        };


        class background_node;

        class background_class : public node_class {
            typedef std::vector<background_node *> bound_nodes_t;

            background_node * first;
            bound_nodes_t bound_nodes;

        public:
            explicit background_class(openvrml::browser & browser);
            virtual ~background_class() throw ();

            void set_first(background_node & background) throw ();
            void reset_first() throw ();
            bool has_first() const throw ();
            bool is_first(background_node & background) throw ();
            void bind(background_node & background, double timestamp)
                throw (std::bad_alloc);
            void unbind(background_node & background, double timestamp)
                throw ();

        private:
            virtual void
            do_initialize(openvrml::viewpoint_node * initial_viewpoint,
                          double timestamp) throw ();
            virtual void do_render(viewer & v) const throw ();
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class background_node : public abstract_base<background_node>,
                                public child_node {
            friend class background_class;

            class set_bind_listener : public event_listener_base<self_t>,
                                      public sfbool_listener {
            public:
                explicit set_bind_listener(background_node & node);
                virtual ~set_bind_listener() throw ();

            private:
                virtual void do_process_event(const sfbool & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class back_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit back_url_exposedfield(background_node & node)
                    throw ();
                virtual ~back_url_exposedfield() throw ();

            private:
                virtual void event_side_effect(const mfstring & value,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class bottom_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit bottom_url_exposedfield(background_node & node)
                    throw ();
                virtual ~bottom_url_exposedfield() throw ();

            private:
                virtual void event_side_effect(const mfstring & value,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class front_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit front_url_exposedfield(background_node & node)
                    throw ();
                virtual ~front_url_exposedfield() throw ();

            private:
                virtual void event_side_effect(const mfstring & value,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class left_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit left_url_exposedfield(background_node & node)
                    throw ();
                virtual ~left_url_exposedfield() throw ();

            private:
                virtual void event_side_effect(const mfstring & value,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class right_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit right_url_exposedfield(background_node & node)
                    throw ();
                virtual ~right_url_exposedfield() throw ();

            private:
                virtual void event_side_effect(const mfstring & value,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class top_url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit top_url_exposedfield(background_node & node) throw ();
                virtual ~top_url_exposedfield() throw ();

            private:
                virtual void event_side_effect(const mfstring & value,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            set_bind_listener set_bind_listener_;
            exposedfield<mffloat> ground_angle_;
            exposedfield<mfcolor> ground_color_;
            back_url_exposedfield back_url_;
            bottom_url_exposedfield bottom_url_;
            front_url_exposedfield front_url_;
            left_url_exposedfield left_url_;
            right_url_exposedfield right_url_;
            top_url_exposedfield top_url_;
            exposedfield<mffloat> sky_angle_;
            exposedfield<mfcolor> sky_color_;
            sfbool is_bound_;
            sfbool_emitter is_bound_emitter_;

            image front;
            bool front_needs_update;
            image back;
            bool back_needs_update;
            image left;
            bool left_needs_update;
            image right;
            bool right_needs_update;
            image top;
            bool top_needs_update;
            image bottom;
            bool bottom_needs_update;

            // Display list object for background
            viewer::object_t viewerObject;

        public:
            background_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~background_node() throw ();

        private:
            virtual void do_initialize(double timestamp) throw ();
            virtual void do_shutdown(double timestamp) throw ();

            void update_textures();
        };


        class billboard_class : public node_class {
        public:
            explicit billboard_class(openvrml::browser & browser);
            virtual ~billboard_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class billboard_node : public grouping_node_base<billboard_node> {
            friend class billboard_class;

            exposedfield<sfvec3f> axis_of_rotation_;

            viewer::object_t xformObject;

        public:
            static const mat4f billboard_to_matrix(const billboard_node & node,
                                                   const mat4f & modelview);

            billboard_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~billboard_node() throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class box_class : public node_class {
        public:
            explicit box_class(openvrml::browser & browser);
            virtual ~box_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class box_node : public abstract_base<box_node>,
                         public geometry_node {
            friend class box_class;

            sfvec3f size;

            bounding_sphere bsphere;

        public:
            box_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~box_node() throw ();

        private:
            virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class collision_class : public node_class {
        public:
            explicit collision_class(openvrml::browser & browser);
            virtual ~collision_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class collision_node : public grouping_node_base<collision_node> {
            friend class collision_class;

            exposedfield<sfbool> collide_;
            sfnode proxy_;
            sftime collide_time_;
            sftime_emitter collide_time_emitter_;

        public:
            collision_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~collision_node() throw ();

            virtual bool modified() const;
        };


        class color_class : public node_class {
        public:
            explicit color_class(openvrml::browser & browser);
            virtual ~color_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class color_node : public abstract_base<color_node>,
                           public openvrml::color_node {
            friend class color_class;

            exposedfield<mfcolor> color_;

        public:
            color_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~color_node() throw ();

            //
            // color_node implementation
            //
            virtual const std::vector<openvrml::color> & color() const
                throw ();
        };


        class color_interpolator_class : public node_class {
        public:
            explicit color_interpolator_class(openvrml::browser & browser);
            virtual ~color_interpolator_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class color_interpolator_node :
            public abstract_base<color_interpolator_node>,
            public child_node {

            friend class color_interpolator_class;

            class set_fraction_listener : public event_listener_base<self_t>,
                                          public sffloat_listener {
            public:
                explicit set_fraction_listener(color_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

            private:
                virtual void do_process_event(const sffloat & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mfcolor> key_value_;
            sfcolor value_;
            sfcolor_emitter value_changed_;

        public:
            color_interpolator_node(const node_type & type,
                                    const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~color_interpolator_node() throw ();
        };


        class cone_class : public node_class {
        public:
            explicit cone_class(openvrml::browser & browser);
            virtual ~cone_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class cone_node : public abstract_base<cone_node>,
                          public geometry_node {
            friend class cone_class;

            sfbool bottom;
            sffloat bottomRadius;
            sffloat height;
            sfbool side;

        public:
            cone_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~cone_node() throw ();

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class coordinate_class : public node_class {
        public:
            explicit coordinate_class(openvrml::browser & browser);
            virtual ~coordinate_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class coordinate_node : public abstract_base<coordinate_node>,
                                public openvrml::coordinate_node {
            friend class coordinate_class;

            exposedfield<mfvec3f> point_;

        public:
            coordinate_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~coordinate_node() throw ();

            //
            // coordinate_node implementation
            //
            virtual const std::vector<vec3f> & point() const throw ();
        };


        class coordinate_interpolator_class :
            public node_class {
        public:
            explicit coordinate_interpolator_class(
                openvrml::browser & browser);
            virtual ~coordinate_interpolator_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class coordinate_interpolator_node :
            public abstract_base<coordinate_interpolator_node>,
            public child_node {

            friend class coordinate_interpolator_class;

            class set_fraction_listener : public event_listener_base<self_t>,
                                          public sffloat_listener {
            public:
                explicit set_fraction_listener(
                    coordinate_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

            private:
                virtual void do_process_event(const sffloat & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mfvec3f> key_value_;
            mfvec3f value_;
            mfvec3f_emitter value_changed_;

        public:
            coordinate_interpolator_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~coordinate_interpolator_node() throw ();
        };


        class cylinder_class : public node_class {
        public:
            explicit cylinder_class(openvrml::browser & browser);
            virtual ~cylinder_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class cylinder_node : public abstract_base<cylinder_node>,
                              public geometry_node {
            friend class cylinder_class;

            sfbool bottom;
            sffloat height;
            sffloat radius;
            sfbool side;
            sfbool top;

        public:
            cylinder_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~cylinder_node() throw ();

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class cylinder_sensor_class : public node_class {
        public:
            explicit cylinder_sensor_class(openvrml::browser & browser);
            virtual ~cylinder_sensor_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class cylinder_sensor_node :
            public abstract_base<cylinder_sensor_node>,
            public child_node {

            friend class cylinder_sensor_class;

            exposedfield<sfbool> auto_offset_;
            exposedfield<sffloat> disk_angle_;
            exposedfield<sfbool> enabled_;
            exposedfield<sffloat> max_angle_;
            exposedfield<sffloat> min_angle_;
            exposedfield<sffloat> offset_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sfrotation rotation_changed_;
            sfrotation_emitter rotation_changed_emitter_;
            sfvec3f track_point_changed_;
            sfvec3f_emitter track_point_changed_emitter_;

            float rotation_val;
            vec3f activationPoint;
            bool disk;
            mat4f activationMatrix;
            mat4f modelview;

        public:
            cylinder_sensor_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~cylinder_sensor_node() throw ();

            virtual cylinder_sensor_node * to_cylinder_sensor() const;

            void activate(double timeStamp, bool isActive, double *p);

            bool enabled() const;

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class directional_light_class : public node_class {
        public:
            explicit directional_light_class(openvrml::browser & browser);
            virtual ~directional_light_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class directional_light_node :
            public abstract_light_node<directional_light_node> {

            friend class directional_light_class;

            exposedfield<sfvec3f> direction_;

        public:
            directional_light_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~directional_light_node() throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class elevation_grid_class : public node_class {
        public:
            explicit elevation_grid_class(openvrml::browser & browser);
            virtual ~elevation_grid_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class elevation_grid_node : public abstract_base<elevation_grid_node>,
                                    public geometry_node {
            friend class elevation_grid_class;

            class set_height_listener : public event_listener_base<self_t>,
                                        public mffloat_listener {
            public:
                explicit set_height_listener(elevation_grid_node & node);
                virtual ~set_height_listener() throw ();

            private:
                virtual void do_process_event(const mffloat & height,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_height_listener set_height_listener_;
            exposedfield<sfnode> color_;
            exposedfield<sfnode> normal_;
            exposedfield<sfnode> tex_coord_;
            mffloat height_;
            sfbool ccw_;
            sfbool color_per_vertex_;
            sffloat crease_angle_;
            sfbool normal_per_vertex_;
            sfbool solid_;
            sfint32 x_dimension_;
            sffloat x_spacing_;
            sfint32 z_dimension_;
            sffloat z_spacing_;

        public:
            elevation_grid_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~elevation_grid_node() throw ();

            virtual bool modified() const;

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class extrusion_class : public node_class {
        public:
            explicit extrusion_class(openvrml::browser & browser);
            virtual ~extrusion_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class extrusion_node : public abstract_base<extrusion_node>,
                               public geometry_node {
            friend class extrusion_class;

            class set_cross_section_listener :
                public event_listener_base<self_t>,
                public mfvec2f_listener {
            public:
                explicit set_cross_section_listener(extrusion_node & node);
                virtual ~set_cross_section_listener() throw ();

            private:
                virtual void do_process_event(const mfvec2f & cross_section,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class set_orientation_listener :
                public event_listener_base<self_t>,
                public mfrotation_listener {
            public:
                explicit set_orientation_listener(extrusion_node & node);
                virtual ~set_orientation_listener() throw ();

            private:
                virtual void do_process_event(const mfrotation & orientation,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class set_scale_listener : public event_listener_base<self_t>,
                                       public mfvec2f_listener {
            public:
                explicit set_scale_listener(extrusion_node & node);
                virtual ~set_scale_listener() throw ();

            private:
                virtual void do_process_event(const mfvec2f & scale,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class set_spine_listener : public event_listener_base<self_t>,
                                       public mfvec3f_listener {
            public:
                explicit set_spine_listener(extrusion_node & node);
                virtual ~set_spine_listener() throw ();

            private:
                virtual void do_process_event(const mfvec3f & spine,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_cross_section_listener set_cross_section_listener_;
            set_orientation_listener set_orientation_listener_;
            set_scale_listener set_scale_listener_;
            set_spine_listener set_spine_listener_;
            sfbool begin_cap_;
            sfbool ccw_;
            sfbool convex_;
            sffloat crease_angle_;
            mfvec2f cross_section_;
            sfbool end_cap_;
            mfrotation orientation_;
            mfvec2f scale_;
            sfbool solid_;
            mfvec3f spine_;

        public:
            extrusion_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~extrusion_node() throw ();

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class fog_node;

        class fog_class : public node_class {
            typedef std::vector<fog_node *> bound_nodes_t;

            fog_node * first;
            bound_nodes_t bound_nodes;

        public:
            explicit fog_class(openvrml::browser & browser);
            virtual ~fog_class() throw ();

            void set_first(fog_node & fog) throw ();
            void reset_first() throw ();
            bool has_first() const throw ();
            bool is_first(fog_node & fog) throw ();
            void bind(fog_node & fog, double timestamp) throw (std::bad_alloc);
            void unbind(fog_node & fog, double timestamp) throw ();

        private:
            virtual void
            do_initialize(openvrml::viewpoint_node * initialViewpoint,
                          double timestamp) throw ();
            virtual void do_render(viewer & v) const throw ();
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class fog_node : public abstract_base<fog_node>,
                         public child_node {
            friend class fog_class;

            class set_bind_listener : public event_listener_base<self_t>,
                                      public sfbool_listener {
            public:
                explicit set_bind_listener(fog_node & node);
                virtual ~set_bind_listener() throw ();

            private:
                virtual void do_process_event(const sfbool & bind,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_bind_listener set_bind_listener_;
            exposedfield<sfcolor> color_;
            exposedfield<sfstring> fog_type_;
            exposedfield<sffloat> visibility_range_;
            sfbool is_bound_;
            sfbool_emitter is_bound_emitter_;

        public:
            fog_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~fog_node() throw ();

        private:
            virtual void do_initialize(double timestamp) throw ();
            virtual void do_shutdown(double timestamp) throw ();
        };


        class font_style_class : public node_class {
        public:
            explicit font_style_class(openvrml::browser & browser);
            virtual ~font_style_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class font_style_node : public abstract_base<font_style_node>,
                                public openvrml::font_style_node {

            friend class font_style_class;

            mfstring family_;
            sfbool horizontal_;
            mfstring justify_;
            sfstring language_;
            sfbool leftToRight;
            sffloat size_;
            sffloat spacing_;
            sfstring style_;
            sfbool topToBottom;

        public:
            font_style_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~font_style_node() throw ();

            //
            // font_style_node implementation
            //
            virtual const std::vector<std::string> & family() const throw ();
            virtual bool horizontal() const throw ();
            virtual const std::vector<std::string> & justify() const throw ();
            virtual const std::string & language() const throw ();
            virtual bool left_to_right() const throw ();
            virtual float size() const throw ();
            virtual float spacing() const throw ();
            virtual const std::string & style() const throw ();
            virtual bool top_to_bottom() const throw ();
        };


        class group_class : public node_class {
        public:
            explicit group_class(openvrml::browser & browser);
            virtual ~group_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class group_node : public grouping_node_base<group_node> {
            friend class group_class;

        public:
            group_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~group_node() throw ();
        };


        class image_texture_class : public node_class {
        public:
            explicit image_texture_class(openvrml::browser & browser);
            virtual ~image_texture_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class image_texture_node :
            public abstract_texture_node<image_texture_node> {

            friend class image_texture_class;

            class url_exposedfield : public exposedfield<mfstring> {
            public:
                explicit url_exposedfield(image_texture_node & node);
                virtual ~url_exposedfield() throw ();

            private:
                virtual void event_side_effect(const mfstring & url,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            url_exposedfield url_;

            openvrml::image image_;
            bool texture_needs_update;

        public:
            image_texture_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~image_texture_node() throw ();

            virtual const openvrml::image & image() const throw ();

        private:
            virtual viewer::texture_object_t do_render_texture(viewer & v);

            void update_texture();
        };


        class indexed_face_set_class : public node_class {
        public:
            explicit indexed_face_set_class(openvrml::browser & browser);
            virtual ~indexed_face_set_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class indexed_face_set_node :
            public abstract_indexed_set_node<indexed_face_set_node> {

            friend class indexed_face_set_class;

            class set_normal_index_listener :
                public event_listener_base<self_t>,
                public mfint32_listener {
            public:
                explicit set_normal_index_listener(
                    indexed_face_set_node & node);
                virtual ~set_normal_index_listener() throw ();

            private:
                virtual void do_process_event(const mfint32 & normal_index,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class set_tex_coord_index_listener :
                public event_listener_base<self_t>,
                public mfint32_listener {
            public:
                explicit set_tex_coord_index_listener(
                    indexed_face_set_node & node);
                virtual ~set_tex_coord_index_listener() throw ();

            private:
                virtual void do_process_event(const mfint32 & tex_coord_index,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_normal_index_listener set_normal_index_listener_;
            set_tex_coord_index_listener set_tex_coord_index_listener_;
            exposedfield<sfnode> normal_;
            exposedfield<sfnode> tex_coord_;
            sfbool ccw_;
            sfbool convex_;
            sffloat crease_angle_;
            mfint32 normal_index_;
            sfbool normal_per_vertex_;
            sfbool solid_;
            mfint32 tex_coord_index_;

            bounding_sphere bsphere;

        public:
            indexed_face_set_node(const node_type & type,
                                  const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~indexed_face_set_node() throw ();

            virtual bool modified() const;

        private:
            virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);

            void recalcBSphere();
        };


        class indexed_line_set_class : public node_class {
        public:
            explicit indexed_line_set_class(openvrml::browser & browser);
            virtual ~indexed_line_set_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class indexed_line_set_node :
            public abstract_indexed_set_node<indexed_line_set_node> {

            friend class indexed_line_set_class;

        public:
            indexed_line_set_node(const node_type & type,
                                  const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~indexed_line_set_node() throw ();

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
            virtual bool do_emissive() const throw ();
        };


        class inline_class : public node_class {
        public:
            explicit inline_class(openvrml::browser & browser);
            virtual ~inline_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class inline_node : public abstract_base<inline_node>,
                            public grouping_node {
            friend class inline_class;

            exposedfield<mfstring> url_;
            sfvec3f bbox_center_;
            sfvec3f bbox_size_;

            openvrml::scene * inlineScene;
            bool hasLoaded;

        public:
            inline_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~inline_node() throw ();

            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);

            void load();
        };


        class lod_class : public node_class {
        public:
            explicit lod_class(openvrml::browser & browser);
            virtual ~lod_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class lod_node : public abstract_base<lod_node>, public grouping_node {
            friend class lod_class;

            exposedfield<mfnode> level_;
            sfvec3f center_;
            mffloat range_;

            mfnode children_;
            bounding_sphere bsphere;

        public:
            lod_node(const node_type & type,
                     const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~lod_node() throw ();

            virtual bool modified() const;
            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);

            virtual void recalcBSphere();
        };


        class material_class : public node_class {
        public:
            explicit material_class(openvrml::browser & browser);
            virtual ~material_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class material_node : public abstract_base<material_node>,
                              public openvrml::material_node {
            friend class material_class;

            exposedfield<sffloat> ambient_intensity_;
            exposedfield<sfcolor> diffuse_color_;
            exposedfield<sfcolor> emissive_color_;
            exposedfield<sffloat> shininess_;
            exposedfield<sfcolor> specular_color_;
            exposedfield<sffloat> transparency_;

        public:
            material_node(const node_type & type,
                          const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~material_node() throw ();

            //
            // material_node implementation
            //
            virtual float ambient_intensity() const throw ();
            virtual const color & diffuse_color() const throw ();
            virtual const color & emissive_color() const throw ();
            virtual float shininess() const throw ();
            virtual const color & specular_color() const throw ();
            virtual float transparency() const throw ();
        };


        class movie_texture_class : public node_class {
        public:
            explicit movie_texture_class(openvrml::browser & browser);
            virtual ~movie_texture_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class movie_texture_node :
            public abstract_texture_node<movie_texture_node> {

            friend class movie_texture_class;

            class set_speed_listener : public event_listener_base<self_t>,
                                       public sffloat_listener {
            public:
                explicit set_speed_listener(movie_texture_node & node);
                virtual ~set_speed_listener() throw ();

            private:
                virtual void do_process_event(const sffloat & speed,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            exposedfield<sfbool> loop_;
            set_speed_listener set_speed_;
            sffloat speed_;
            sffloat_emitter speed_changed_;
            exposedfield<sftime> start_time_;
            exposedfield<sftime> stop_time_;
            exposedfield<mfstring> url_;
            sftime duration_;
            sftime_emitter duration_changed_;
            sfbool active_;
            sfbool_emitter is_active_;

            openvrml::image image_;

        public:
            movie_texture_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~movie_texture_node() throw ();

            virtual movie_texture_node * to_movie_texture() const;

            void update(double time);

            virtual const openvrml::image & image() const throw ();

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();
            virtual viewer::texture_object_t do_render_texture(viewer & v);
        };


        class navigation_info_node;

        class navigation_info_class : public node_class {
            typedef std::vector<navigation_info_node *> bound_nodes_t;

            navigation_info_node * first;
            bound_nodes_t bound_nodes;

        public:
            explicit navigation_info_class(openvrml::browser & browser);
            virtual ~navigation_info_class() throw ();

            void set_first(navigation_info_node & nav_info) throw ();
            void reset_first() throw ();
            bool has_first() const throw ();
            bool is_first(navigation_info_node & nav_info) throw ();
            void bind(navigation_info_node & nav_info, double timestamp)
                throw (std::bad_alloc);
            void unbind(navigation_info_node & nav_info, double timestamp)
                throw ();

        private:
            virtual void
            do_initialize(openvrml::viewpoint_node * initial_viewpoint,
                          double timestamp) throw ();
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class navigation_info_node :
            public abstract_base<navigation_info_node>,
            public openvrml::navigation_info_node {

            friend class navigation_info_class;

            class set_bind_listener : public event_listener_base<self_t>,
                                      public sfbool_listener {
            public:
                explicit set_bind_listener(navigation_info_node & node);
                virtual ~set_bind_listener() throw ();

            private:
                virtual void do_process_event(const sfbool & bind,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_bind_listener set_bind_listener_;
            exposedfield<mffloat> avatar_size_;
            exposedfield<sfbool> headlight_;
            exposedfield<sffloat> speed_;
            exposedfield<mfstring> type_;
            exposedfield<sffloat> visibility_limit_;
            sfbool is_bound_;
            sfbool_emitter is_bound_emitter_;

        public:
            navigation_info_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~navigation_info_node() throw ();

            virtual const std::vector<float> & avatar_size() const throw ();
            virtual bool headlight() const throw ();
            virtual float speed() const throw ();
            virtual const std::vector<std::string> & type() const throw ();
            virtual float visibility_limit() const throw ();

        private:
            virtual void do_initialize(double timestamp) throw ();
            virtual void do_shutdown(double timestamp) throw ();
        };


        class normal_class : public node_class {
        public:
            explicit normal_class(openvrml::browser & browser);
            virtual ~normal_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class normal_node : public abstract_base<normal_node>,
                            public openvrml::normal_node {
            friend class normal_class;

            exposedfield<mfvec3f> vector_;

        public:
            normal_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~normal_node() throw ();

            //
            // normal_node implementation.
            //
            virtual const std::vector<vec3f> & vector() const throw ();
        };


        class normal_interpolator_class : public node_class {
        public:
            explicit normal_interpolator_class(openvrml::browser & browser);
            virtual ~normal_interpolator_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class normal_interpolator_node :
            public abstract_base<normal_interpolator_node>,
            public child_node {

            friend class normal_interpolator_class;

            class set_fraction_listener : public event_listener_base<self_t>,
                                          public sffloat_listener {
            public:
                explicit set_fraction_listener(
                    normal_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

            private:
                virtual void do_process_event(const sffloat & fraction,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mfvec3f> key_value_;
            mfvec3f value_changed_;
            mfvec3f_emitter value_changed_emitter_;

        public:
            normal_interpolator_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~normal_interpolator_node() throw ();
        };


        class orientation_interpolator_class : public node_class {
        public:
            explicit orientation_interpolator_class(
                openvrml::browser & browser);
            virtual ~orientation_interpolator_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class orientation_interpolator_node :
            public abstract_base<orientation_interpolator_node>,
            public child_node {

            friend class orientation_interpolator_class;

            class set_fraction_listener : public event_listener_base<self_t>,
                                          public sffloat_listener {
            public:
                explicit set_fraction_listener(
                    orientation_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

            private:
                virtual void do_process_event(const sffloat & fraction,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mfrotation> key_value_;
            sfrotation value_changed_;
            sfrotation_emitter value_changed_emitter_;

        public:
            orientation_interpolator_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~orientation_interpolator_node() throw ();
        };


        class pixel_texture_class : public node_class {
        public:
            explicit pixel_texture_class(openvrml::browser & browser);
            virtual ~pixel_texture_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class pixel_texture_node :
            public abstract_texture_node<pixel_texture_node> {

            friend class pixel_texture_class;

            exposedfield<sfimage> image_;

        public:
            pixel_texture_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~pixel_texture_node() throw ();

            virtual const openvrml::image & image() const throw ();

        private:
            virtual viewer::texture_object_t do_render_texture(viewer & v);
        };


        class plane_sensor_class : public node_class {
        public:
            explicit plane_sensor_class(openvrml::browser & browser);
            virtual ~plane_sensor_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class plane_sensor_node : public abstract_base<plane_sensor_node>,
                                  public child_node {
            friend class plane_sensor_class;

            exposedfield<sfbool> auto_offset_;
            exposedfield<sfbool> enabled_;
            exposedfield<sfvec2f> max_position_;
            exposedfield<sfvec2f> min_position_;
            exposedfield<sfvec3f> offset_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sfvec3f track_point_changed_;
            sfvec3f_emitter track_point_changed_emitter_;
            sfvec3f translation_changed_;
            sfvec3f_emitter translation_changed_emitter_;

            sfvec3f activationPoint;

            mat4f activationMatrix;
            mat4f modelview;

        public:
            plane_sensor_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~plane_sensor_node() throw ();

            virtual plane_sensor_node * to_plane_sensor() const;

            void activate( double timeStamp, bool isActive, double *p );

            bool enabled() const;

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class point_light_class : public node_class {
        public:
            explicit point_light_class(openvrml::browser & browser);
            virtual ~point_light_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class point_light_node : public abstract_light_node<point_light_node>,
                                 public scoped_light_node {
            friend class point_light_class;

            exposedfield<sfvec3f> attenuation_;
            exposedfield<sfvec3f> location_;
            exposedfield<sffloat> radius_;

        public:
            point_light_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~point_light_node() throw ();

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();
            virtual void do_render_scoped_light(viewer & v);
        };


        class point_set_class : public node_class {
        public:
            explicit point_set_class(openvrml::browser & browser);
            virtual ~point_set_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interface) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class point_set_node : public abstract_base<point_set_node>,
                               public geometry_node {
            friend class point_set_class;

            exposedfield<sfnode> color_;
            exposedfield<sfnode> coord_;

            bounding_sphere bsphere;

        public:
            point_set_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~point_set_node() throw ();

            virtual bool modified() const;

        private:
            virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
            virtual bool do_emissive() const throw ();

            void recalcBSphere();
        };


        class position_interpolator_class : public node_class {
        public:
            explicit position_interpolator_class(openvrml::browser & browser);
            virtual ~position_interpolator_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class position_interpolator_node :
            public abstract_base<position_interpolator_node>,
            public child_node {

            friend class position_interpolator_class;

            class set_fraction_listener : public event_listener_base<self_t>,
                                          public sffloat_listener {
            public:
                explicit set_fraction_listener(
                    position_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

            private:
                virtual void do_process_event(const sffloat & fraction,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mfvec3f> key_value_;
            sfvec3f value_changed_;
            sfvec3f_emitter value_changed_emitter_;

        public:
            position_interpolator_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~position_interpolator_node() throw ();
        };


        class proximity_sensor_class : public node_class {
        public:
            explicit proximity_sensor_class(openvrml::browser & browser);
            virtual ~proximity_sensor_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class proximity_sensor_node :
            public abstract_base<proximity_sensor_node>,
            public child_node {

            friend class proximity_sensor_class;

            exposedfield<sfvec3f> center_;
            exposedfield<sfbool> enabled_;
            exposedfield<sfvec3f> size_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sfvec3f position_changed_;
            sfvec3f_emitter position_changed_emitter_;
            sfrotation orientation_changed_;
            sfrotation_emitter orientation_changed_emitter_;
            sftime enter_time_;
            sftime_emitter enter_time_emitter_;
            sftime exit_time_;
            sftime_emitter exit_time_emitter_;

        public:
            proximity_sensor_node(const node_type & type,
                                  const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~proximity_sensor_node() throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class scalar_interpolator_class : public node_class {
        public:
            explicit scalar_interpolator_class(openvrml::browser & browser);
            virtual ~scalar_interpolator_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class scalar_interpolator_node :
            public abstract_base<scalar_interpolator_node>,
            public child_node {

            friend class scalar_interpolator_class;

            class set_fraction_listener : public event_listener_base<self_t>,
                                          public sffloat_listener {
            public:
                explicit set_fraction_listener(
                    scalar_interpolator_node & node);
                virtual ~set_fraction_listener() throw ();

            private:
                virtual void do_process_event(const sffloat & fraction,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_fraction_listener set_fraction_listener_;
            exposedfield<mffloat> key_;
            exposedfield<mffloat> key_value_;
            sffloat value_changed_;
            sffloat_emitter value_changed_emitter_;

        public:
            scalar_interpolator_node(const node_type & type,
                                     const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~scalar_interpolator_node() throw ();
        };


        class shape_class : public node_class {
        public:
            explicit shape_class(openvrml::browser & browser);
            virtual ~shape_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class shape_node : public abstract_base<shape_node>,
                           public child_node {
            friend class shape_class;

            exposedfield<sfnode> appearance_;
            exposedfield<sfnode> geometry_;

            viewer::object_t viewerObject;

        public:
            shape_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~shape_node() throw ();

            virtual bool modified() const;

        private:
            virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class sound_class : public node_class {
        public:
            explicit sound_class(openvrml::browser & browser);
            virtual ~sound_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interface) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class sound_node : public abstract_base<sound_node>,
                           public child_node {
            friend class sound_class;

            exposedfield<sfvec3f> direction_;
            exposedfield<sffloat> intensity_;
            exposedfield<sfvec3f> location_;
            exposedfield<sffloat> max_back_;
            exposedfield<sffloat> max_front_;
            exposedfield<sffloat> min_back_;
            exposedfield<sffloat> min_front_;
            exposedfield<sffloat> priority_;
            exposedfield<sfnode> source_;
            sfbool spatialize_;

        public:
            sound_node(const node_type & type,
                       const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~sound_node() throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class sphere_class : public node_class {
        public:
            explicit sphere_class(openvrml::browser & browser);
            virtual ~sphere_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class sphere_node : public abstract_base<sphere_node>,
                            public geometry_node {
            friend class sphere_class;

            sffloat radius;
            bounding_sphere bsphere;

        public:
            sphere_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~sphere_node() throw ();

        private:
            virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);
        };


        class sphere_sensor_class : public node_class {
        public:
            explicit sphere_sensor_class(openvrml::browser & browser);
            virtual ~sphere_sensor_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class sphere_sensor_node : public abstract_base<sphere_sensor_node>,
                                   public child_node {
            friend class sphere_sensor_class;

            exposedfield<sfbool> auto_offset_;
            exposedfield<sfbool> enabled_;
            exposedfield<sfrotation> offset_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sfrotation rotation_changed_;
            sfrotation_emitter rotation_changed_emitter_;
            sfvec3f track_point_changed_;
            sfvec3f_emitter track_point_changed_emitter_;

            sfvec3f activationPoint;
            sfvec3f centerPoint;
            mat4f modelview;

        public:
            sphere_sensor_node(const node_type & type,
                               const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~sphere_sensor_node() throw ();

            virtual sphere_sensor_node * to_sphere_sensor() const;

            void activate(double timeStamp, bool isActive, double * p);
            bool isEnabled() const throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class spot_light_class : public node_class {
        public:
            explicit spot_light_class(openvrml::browser & browser);
            virtual ~spot_light_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class spot_light_node : public abstract_light_node<spot_light_node>,
                                public scoped_light_node {
            friend class spot_light_class;

            exposedfield<sfvec3f> attenuation_;
            exposedfield<sffloat> beam_width_;
            exposedfield<sffloat> cut_off_angle_;
            exposedfield<sfvec3f> direction_;
            exposedfield<sfvec3f> location_;
            exposedfield<sffloat> radius_;

        public:
            spot_light_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~spot_light_node() throw ();

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();
            virtual void do_render_scoped_light(viewer & v);
        };


        class switch_class : public node_class {
        public:
            explicit switch_class(openvrml::browser & browser);
            virtual ~switch_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                    throw (unsupported_interface, std::bad_alloc);
        };

        class switch_node : public abstract_base<switch_node>,
                            public grouping_node {
            friend class switch_class;

            class choice_exposedfield : public exposedfield<mfnode> {
            public:
                explicit choice_exposedfield(switch_node & node);
                virtual ~choice_exposedfield() throw ();

            private:
                virtual void event_side_effect(const mfnode & choice,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class which_choice_exposedfield : public exposedfield<sfint32> {
            public:
                explicit which_choice_exposedfield(switch_node & node);
                virtual ~which_choice_exposedfield() throw ();

            private:
                virtual void event_side_effect(const sfint32 & which_choice,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            choice_exposedfield choice_;
            which_choice_exposedfield which_choice_;

            mfnode children_;
            bounding_sphere bsphere;

        public:
            switch_node(const node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~switch_node() throw ();

            virtual bool modified() const;
            virtual const std::vector<node_ptr> & children() const throw ();
            virtual void activate(double timestamp, bool over, bool active,
                                  double * p);

        private:
            virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);

            virtual void recalcBSphere();
        };


        class text_class : public node_class {
        public:
            FT_Library freeTypeLibrary;

            explicit text_class(openvrml::browser & browser);
            virtual ~text_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class text_node : public abstract_base<text_node>,
                          public geometry_node {
            friend class text_class;

            class string_exposedfield : public exposedfield<mfstring> {
            public:
                explicit string_exposedfield(text_node & node);
                virtual ~string_exposedfield() throw ();

                virtual void event_side_effect(const mfstring & string,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class font_style_exposedfield : public exposedfield<sfnode> {
            public:
                explicit font_style_exposedfield(text_node & node);
                virtual ~font_style_exposedfield() throw ();

                virtual void event_side_effect(const sfnode & font_style,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class length_exposedfield : public exposedfield<mffloat> {
            public:
                explicit length_exposedfield(text_node & node);
                virtual ~length_exposedfield() throw ();

                virtual void event_side_effect(const mffloat & length,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class max_extent_exposedfield : public exposedfield<sffloat> {
            public:
                explicit max_extent_exposedfield(text_node & node);
                virtual ~max_extent_exposedfield() throw ();

                virtual void event_side_effect(const sffloat & max_extent,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            string_exposedfield string_;
            font_style_exposedfield font_style_;
            length_exposedfield length_;
            max_extent_exposedfield max_extent_;

            struct glyph_geometry {
                std::vector<vec2f> coord;
                std::vector<int32> coord_index;
                float advance_width;
                float advance_height;

                glyph_geometry(
                    const std::vector<std::vector<vec2f> > & contours,
                    float advance_width, float advance_height)
                    throw (std::bad_alloc);
            };

            struct text_geometry {
                std::vector<vec3f> coord;
                std::vector<int32> coord_index;
                std::vector<vec3f> normal;
                std::vector<vec2f> tex_coord;
            };

            typedef std::vector<std::vector<FcChar32> > ucs4_string_t;
            typedef std::map<FT_UInt, glyph_geometry> glyph_geometry_map_t;

            ucs4_string_t ucs4_string;
            FT_Face face;
            glyph_geometry_map_t glyph_geometry_map;
            text_geometry text_geometry_;

        public:
            text_node(const node_type & type,
                      const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~text_node() throw ();

            virtual bool modified() const;

        private:
            virtual viewer::object_t
            do_render_geometry(openvrml::viewer & viewer,
                               rendering_context context);

            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            void update_ucs4() throw (std::bad_alloc);
            void update_face() throw (std::bad_alloc);
            void update_geometry() throw (std::bad_alloc);
        };


        class texture_coordinate_class : public node_class {
        public:
            explicit texture_coordinate_class(openvrml::browser & browser);
            virtual ~texture_coordinate_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class texture_coordinate_node :
            public abstract_base<texture_coordinate_node>,
            public openvrml::texture_coordinate_node {

            friend class texture_coordinate_class;

            exposedfield<mfvec2f> point_;

        public:
            texture_coordinate_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~texture_coordinate_node() throw ();

            //
            // texture_coordinate_node implementation.
            //
            virtual const std::vector<vec2f> & point() const throw();
        };


        class texture_transform_class : public node_class {
        public:
            explicit texture_transform_class(openvrml::browser & browser);
            virtual ~texture_transform_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class texture_transform_node :
            public abstract_base<texture_transform_node>,
            public openvrml::texture_transform_node {

            friend class texture_transform_class;

            exposedfield<sfvec2f> center_;
            exposedfield<sffloat> rotation_;
            exposedfield<sfvec2f> scale_;
            exposedfield<sfvec2f> translation_;

        public:
            texture_transform_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~texture_transform_node() throw ();

        private:
            virtual void do_render_texture_transform(viewer & v);
        };


        class time_sensor_class : public node_class {
        public:
            explicit time_sensor_class(openvrml::browser & browser);
            virtual ~time_sensor_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class time_sensor_node : public abstract_base<time_sensor_node>,
                                 public child_node {
            friend class time_sensor_class;

            class set_cycle_interval_listener :
                public event_listener_base<self_t>,
                public sftime_listener {
            public:
                explicit set_cycle_interval_listener(time_sensor_node & node);
                virtual ~set_cycle_interval_listener() throw ();

            private:
                virtual void do_process_event(const sftime & cycle_interval,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class enabled_exposedfield : public exposedfield<sfbool> {
            public:
                explicit enabled_exposedfield(time_sensor_node & node);
                virtual ~enabled_exposedfield() throw ();

            private:
                virtual void event_side_effect(const sfbool & enabled,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class set_start_time_listener : public event_listener_base<self_t>,
                                            public sftime_listener {
            public:
                explicit set_start_time_listener(time_sensor_node & node);
                virtual ~set_start_time_listener() throw ();

            private:
                virtual void do_process_event(const sftime & start_time,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            set_cycle_interval_listener set_cycle_interval_listener_;
            sftime cycle_interval_;
            sftime_emitter cycle_interval_changed_emitter_;
            enabled_exposedfield enabled_;
            exposedfield<sfbool> loop_;
            set_start_time_listener set_start_time_listener_;
            sftime start_time_;
            sftime_emitter start_time_changed_emitter_;
            exposedfield<sftime> stop_time_;
            sftime cycle_time_;
            sftime_emitter cycle_time_emitter_;
            sffloat fraction_changed_;
            sffloat_emitter fraction_changed_emitter_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sftime time_;
            sftime_emitter time_emitter_;

            double lastTime;

        public:
            time_sensor_node(const node_type & type,
                             const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~time_sensor_node() throw ();

            virtual time_sensor_node * to_time_sensor() const;

            void update(double time);

        private:
            virtual void do_initialize(double timestamp)
                throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();
        };


        class touch_sensor_class : public node_class {
        public:
            explicit touch_sensor_class(openvrml::browser & browser);
            virtual ~touch_sensor_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class touch_sensor_node : public abstract_base<touch_sensor_node>,
                                  public child_node {
            friend class touch_sensor_class;

            exposedfield<sfbool> enabled_;
            sfvec3f hit_normal_changed_;
            sfvec3f_emitter hit_normal_changed_emitter_;
            sfvec3f hit_point_changed_;
            sfvec3f_emitter hit_point_changed_emitter_;
            sfvec2f hit_tex_coord_changed_;
            sfvec2f_emitter hit_tex_coord_changed_emitter_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sfbool is_over_;
            sfbool_emitter is_over_emitter_;
            sftime touch_time_;
            sftime_emitter touch_time_emitter_;

        public:
            touch_sensor_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~touch_sensor_node() throw ();

            virtual touch_sensor_node * to_touch_sensor() const;

            void activate(double timeStamp, bool isOver, bool isActive,
                          double *p );

            bool enabled() const;
        };


        class transform_class : public node_class {
        public:
            explicit transform_class(openvrml::browser & browser);
            virtual ~transform_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class transform_node : public grouping_node_base<transform_node>,
                               public openvrml::transform_node {
            friend class transform_class;

            class center_exposedfield : public exposedfield<sfvec3f> {
            public:
                explicit center_exposedfield(transform_node & node);
                virtual ~center_exposedfield() throw ();

            private:
                virtual void event_side_effect(const sfvec3f & center,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class rotation_exposedfield : public exposedfield<sfrotation> {
            public:
                explicit rotation_exposedfield(transform_node & node);
                virtual ~rotation_exposedfield() throw ();

            private:
                virtual void event_side_effect(const sfrotation & rotation,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class scale_exposedfield : public exposedfield<sfvec3f> {
            public:
                explicit scale_exposedfield(transform_node & node);
                virtual ~scale_exposedfield() throw ();

            private:
                virtual void event_side_effect(const sfvec3f & scale,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class scale_orientation_exposedfield :
                public exposedfield<sfrotation> {
            public:
                explicit scale_orientation_exposedfield(transform_node & node);
                virtual ~scale_orientation_exposedfield() throw ();

            private:
                virtual void
                event_side_effect(const sfrotation & scale_orientation,
                                  double timestamp)
                    throw (std::bad_alloc);
            };

            class translation_exposedfield : public exposedfield<sfvec3f> {
            public:
                explicit translation_exposedfield(transform_node & node);
                virtual ~translation_exposedfield() throw ();

            private:
                virtual void event_side_effect(const sfvec3f & translation,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            center_exposedfield center_;
            rotation_exposedfield rotation_;
            scale_exposedfield scale_;
            scale_orientation_exposedfield scale_orientation_;
            translation_exposedfield translation_;

            mutable mat4f transform_;
            mutable bool transform_dirty;
            viewer::object_t xformObject;

        public:
            transform_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~transform_node() throw ();

            virtual const mat4f & transform() const throw ();

        private:
            virtual const openvrml::bounding_volume &
            do_bounding_volume() const;

            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);

            void recalc_bsphere();
            void update_transform() const throw ();
        };


        class viewpoint_node;

        class viewpoint_class : public node_class {
            typedef std::vector<viewpoint_node *> bound_nodes_t;

            viewpoint_node * first;
            bound_nodes_t bound_nodes;

        public:
            explicit viewpoint_class(openvrml::browser & browser);
            virtual ~viewpoint_class() throw ();

            void set_first(viewpoint_node & viewpoint) throw ();
            void reset_first() throw ();
            bool has_first() const throw ();
            bool is_first(viewpoint_node & viewpoint) throw ();
            void bind(viewpoint_node & viewpoint, double timestamp)
                throw (std::bad_alloc);
            void unbind(viewpoint_node & viewpoint, double timestamp) throw ();

        private:
            virtual void
            do_initialize(openvrml::viewpoint_node * initial_viewpoint,
                          double timestamp)
                throw ();
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class viewpoint_node : public abstract_base<viewpoint_node>,
                               public openvrml::viewpoint_node {
            friend class viewpoint_class;

            class set_bind_listener : public event_listener_base<self_t>,
                                      public sfbool_listener {
            public:
                explicit set_bind_listener(viewpoint_node & node);
                virtual ~set_bind_listener() throw ();

            private:
                virtual void do_process_event(const sfbool & value,
                                              double timestamp)
                    throw (std::bad_alloc);
            };

            class orientation_exposedfield : public exposedfield<sfrotation> {
            public:
                explicit orientation_exposedfield(viewpoint_node & node);
                virtual ~orientation_exposedfield() throw ();

            private:
                virtual void event_side_effect(const sfrotation & orientation,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            class position_exposedfield : public exposedfield<sfvec3f> {
            public:
                explicit position_exposedfield(viewpoint_node & node);
                virtual ~position_exposedfield() throw ();

            private:
                virtual void event_side_effect(const sfvec3f & position,
                                               double timestamp)
                    throw (std::bad_alloc);
            };

            set_bind_listener set_bind_listener_;
            exposedfield<sffloat> field_of_view_;
            exposedfield<sfbool> jump_;
            orientation_exposedfield orientation_;
            position_exposedfield position_;
            sfstring description_;
            sfbool is_bound_;
            sfbool_emitter is_bound_emitter_;
            sftime bind_time_;
            sftime_emitter bind_time_emitter_;

            mat4f parent_transform;
            mutable mat4f final_transformation;
            mutable bool final_transformation_dirty;
            mat4f user_view_transform_;

        public:
            viewpoint_node(const node_type & type,
                           const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~viewpoint_node() throw ();

            virtual const mat4f & transformation() const throw ();
            virtual const mat4f & user_view_transform() const throw ();
            virtual void user_view_transform(const mat4f & transform)
                throw ();
            virtual const std::string & description() const throw ();
            virtual float field_of_view() const throw ();

            const sfrotation & orientation() const;
            const sfvec3f & position() const;

        private:
            virtual void do_initialize(double timestamp) throw ();
            virtual void do_relocate() throw (std::bad_alloc);
            virtual void do_shutdown(double timestamp) throw ();

            void update_final_transformation() const throw ();
        };


        class visibility_sensor_class : public node_class {
        public:
            explicit visibility_sensor_class(openvrml::browser & browser);
            virtual ~visibility_sensor_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class visibility_sensor_node :
            public abstract_base<visibility_sensor_node>,
            public child_node {

            friend class visibility_sensor_class;

            exposedfield<sfvec3f> center_;
            exposedfield<sfbool> enabled_;
            exposedfield<sfvec3f> size_;
            sfbool is_active_;
            sfbool_emitter is_active_emitter_;
            sftime enter_time_;
            sftime_emitter enter_time_emitter_;
            sftime exit_time_;
            sftime_emitter exit_time_emitter_;

        public:
            visibility_sensor_node(
                const node_type & type,
                const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~visibility_sensor_node() throw ();

        private:
            virtual void do_render_child(openvrml::viewer & viewer,
                                         rendering_context context);
        };


        class world_info_class : public node_class {
        public:
            explicit world_info_class(openvrml::browser & browser);
            virtual ~world_info_class() throw ();

        private:
            virtual const boost::shared_ptr<node_type>
            do_create_type(const std::string & id,
                           const node_interface_set & interfaces) const
                throw (unsupported_interface, std::bad_alloc);
        };

        class world_info_node : public abstract_base<world_info_node>,
                                public child_node {
            friend class world_info_class;

            mfstring info;
            sfstring title;

        public:
            world_info_node(const node_type & type,
                            const boost::shared_ptr<openvrml::scope> & scope);
            virtual ~world_info_node() throw ();
        };
    }
}

# endif
