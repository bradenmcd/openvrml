// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 2001  Braden McDaniel
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

# ifndef OPENVRML_PRIVATE_H
#   define OPENVRML_PRIVATE_H

#   ifndef NDEBUG
#     include <iostream>
#   endif

#   ifdef NDEBUG
#     define OPENVRML_VERIFY_(condition_)
#   else
#     define OPENVRML_VERIFY_(condition_) \
            do { \
                if (!(condition_)) { \
                    std::cerr << __FILE__ << ":" << __LINE__ \
                              << ": verification (" #condition_ ") failed" \
                              << std::endl; \
                } \
            } while (false)
#   endif

#   ifdef NDEBUG
#     define OPENVRML_PRINT_EXCEPTION_(ex_)
#   else
#     define OPENVRML_PRINT_EXCEPTION_(ex_) \
            std::cerr << __FILE__ << ":" << __LINE__ << ": " \
                      << ex_.what() << std::endl
#   endif

#   ifdef NDEBUG
#     define OPENVRML_PRINT_MESSAGE_(message_)
#   else
#     define OPENVRML_PRINT_MESSAGE_(message_) \
            std::cerr << __FILE__ << ":" << __LINE__ << ": " \
                      << message_ << std::endl
#   endif

#   include <cassert>
#   include <cmath>
#   include <cstddef>
#   include <functional>
#   include <limits>

namespace {
    namespace openvrml_ {

        template <typename Float>
        inline Float fabs(const Float f)
        {
            return f < 0.0 ? -f : f;
        }

        template <typename Float>
        struct fequal : std::binary_function<Float, Float, bool> {
            bool operator()(Float a, Float b) const
            {
                const Float diff = fabs(a - b);
                if (diff == 0.0) { return true; }
                static const int rounding_errors = 10;
                static const Float e = std::numeric_limits<Float>::epsilon();
                static const Float tolerance = e * rounding_errors / 2;
                return diff / fabs(a) <= tolerance
                    && diff / fabs(b) <= tolerance;
            }
        };

        template <typename Float>
        struct fless_equal : std::binary_function<Float, Float, bool> {
            bool operator()(Float a, Float b) const
            {
                return a < b || fequal<Float>()(a, b);
            }
        };

        template <typename Float>
        struct fgreater_equal : std::binary_function<Float, Float, bool> {
            bool operator()(Float a, Float b) const
            {
                return a > b || fequal<Float>()(a, b);
            }
        };


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
        class scope_guard_impl1 : scope_guard_impl_base {
            Function function;
            const Param param;

        public:
            scope_guard_impl1(const Function & function, const Param & param);
            ~scope_guard_impl1();
        };

        template <typename Function, typename Param>
        scope_guard_impl1<Function, Param>
        make_guard(const Function & function, const Param & param)
        {
            return scope_guard_impl1<Function, Param>(function, param);
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
    }
}

# endif
