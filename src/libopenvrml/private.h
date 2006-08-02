// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2001, 2002, 2003, 2004  Braden McDaniel
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

#   include <openvrml-common.h>
#   include <boost/concept_check.hpp>

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
#     define OPENVRML_PRINT_EXCEPTION_(ex_) \
            boost::ignore_unused_variable_warning(ex_)
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
#   include <boost/concept_check.hpp>
#   include <boost/mpl/vector.hpp>

namespace openvrml {
    class sfbool;
    class sfcolor;
    class sfcolorrgba;
    class sfdouble;
    class sffloat;
    class sfimage;
    class sfint32;
    class sfnode;
    class sfrotation;
    class sfstring;
    class sftime;
    class sfvec2d;
    class sfvec2f;
    class sfvec3d;
    class sfvec3f;
    class mfbool;
    class mfcolor;
    class mfcolorrgba;
    class mfdouble;
    class mffloat;
    class mfimage;
    class mfint32;
    class mfnode;
    class mfrotation;
    class mfstring;
    class mftime;
    class mfvec2d;
    class mfvec2f;
    class mfvec3d;
    class mfvec3f;
}

namespace {
    namespace openvrml_ {

        const double pi     = 3.14159265358979323846;
        const double pi_2   = 1.57079632679489661923;
        const double pi_4   = 0.78539816339744830962;
        const double inv_pi = 0.31830988618379067154;

        template <typename Float>
        OPENVRML_LOCAL inline Float fabs(const Float f)
        {
            return f < 0.0 ? -f : f;
        }

        struct OPENVRML_LOCAL fequal_t {
            template <typename Float>
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

        const fequal_t fequal = fequal_t();

        struct OPENVRML_LOCAL fless_equal_t {
            template <typename Float>
            bool operator()(Float a, Float b) const
            {
                return a < b || fequal(a, b);
            }
        };

        const fless_equal_t fless_equal = fless_equal_t();

        struct OPENVRML_LOCAL fgreater_equal_t {
            template <typename Float>
            bool operator()(Float a, Float b) const
            {
                return a > b || fequal(a, b);
            }
        };

        const fgreater_equal_t fgreater_equal = fgreater_equal_t();


        class OPENVRML_LOCAL scope_guard_impl_base {
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
        class OPENVRML_LOCAL scope_guard_impl1 : public scope_guard_impl_base {
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
        OPENVRML_LOCAL scope_guard_impl1<Function, Param>
        make_guard(const Function & function, const Param & param)
        {
            return scope_guard_impl1<Function, Param>(function, param);
        }

        template <typename Function,
                  typename Param1,
                  typename Param2,
                  typename Param3>
        class OPENVRML_LOCAL scope_guard_impl3 : public scope_guard_impl_base {
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
        OPENVRML_LOCAL scope_guard_impl3<Function, Param1, Param2, Param3>
        make_guard(const Function & function,
                   const Param1 & param1,
                   const Param2 & param2,
                   const Param3 & param3)
        {
            return scope_guard_impl3<Function, Param1, Param2, Param3>(
                function, param1, param2, param3);
        }

        template <typename Object, typename MemberFunction>
        class OPENVRML_LOCAL obj_scope_guard_impl0 :
            public scope_guard_impl_base {
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
        OPENVRML_LOCAL obj_scope_guard_impl0<Object, MemberFunction>
        make_obj_guard(Object & obj, MemberFunction mem_fun)
        {
            return obj_scope_guard_impl0<Object, MemberFunction>(obj, mem_fun);
        }

        template <typename Object, typename MemberFunction, typename Param>
        class OPENVRML_LOCAL obj_scope_guard_impl1 :
            public scope_guard_impl_base {
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
        OPENVRML_LOCAL obj_scope_guard_impl1<Object, MemberFunction, Param>
        make_obj_guard(Object & obj,
                       MemberFunction mem_fun,
                       const Param & param)
        {
            return obj_scope_guard_impl1<Object, MemberFunction, Param>(
                    obj, mem_fun, param);
        }


        typedef boost::mpl::vector<openvrml::sfbool,
                                   openvrml::sfcolor,
                                   openvrml::sfcolorrgba,
                                   openvrml::sfdouble,
                                   openvrml::sffloat,
                                   openvrml::sfimage,
                                   openvrml::sfint32,
                                   openvrml::sfnode,
                                   openvrml::sfrotation,
                                   openvrml::sfstring,
                                   openvrml::sftime,
                                   openvrml::sfvec2d,
                                   openvrml::sfvec2f,
                                   openvrml::sfvec3d,
                                   openvrml::sfvec3f,
                                   openvrml::mfbool,
                                   openvrml::mfcolor,
                                   openvrml::mfcolorrgba,
                                   openvrml::mfdouble,
                                   openvrml::mffloat,
                                   openvrml::mfimage,
                                   openvrml::mfint32,
                                   openvrml::mfnode,
                                   openvrml::mfrotation,
                                   openvrml::mfstring,
                                   openvrml::mftime,
                                   openvrml::mfvec2d,
                                   openvrml::mfvec2f,
                                   openvrml::mfvec3d,
                                   openvrml::mfvec3f>
        field_value_types;
    }
}

# endif
