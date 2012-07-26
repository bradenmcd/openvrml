// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2010  Braden McDaniel
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

# include <openvrml/local/float.h>
# include <openvrml/browser.h>
# include <openvrml/scene.h>
# include <private.h>
# include <jsapi.h>
# include <boost/array.hpp>
# include <boost/scope_exit.hpp>
# include <algorithm>
# include <iostream>
# include <memory>
# include <sstream>
# include <vector>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

namespace {

# if OPENVRML_JSPROPERTYOP_USES_JSID
    typedef jsid jspropertyop_id;
# else
    typedef jsval jspropertyop_id;
# endif

    JSBool jspropertyop_id_is_int(const jspropertyop_id id)
    {
# if OPENVRML_JSPROPERTYOP_USES_JSID
        return JSID_IS_INT(id);
# else
        return JSVAL_IS_INT(id);
# endif
    }

    int32_t jspropertyop_id_to_int(const jspropertyop_id id)
    {
# if OPENVRML_JSPROPERTYOP_USES_JSID
        return JSID_TO_INT(id);
# else
        return JSVAL_TO_INT(id);
# endif
    }

    JSBool jspropertyop_id_is_string(const jspropertyop_id id)
    {
# if OPENVRML_JSPROPERTYOP_USES_JSID
        return JSID_IS_STRING(id);
# else
        return JSVAL_IS_STRING(id);
# endif
    }

    JSString * jspropertyop_id_to_string(JSContext * const cx,
                                         const jspropertyop_id id)
    {
# if OPENVRML_JSPROPERTYOP_USES_JSID
        return JSID_TO_STRING(id);
# else
        return JS_ValueToString(cx, id);
# endif
    }

    OPENVRML_LOCAL JSBool add_value_root(JSContext * const cx,
                                         jsval * const vp)
    {
# ifdef OPENVRML_JS_HAS_TYPED_ROOT_FUNCTIONS
        return JS_AddValueRoot(cx, vp);
# else
        return JS_AddRoot(cx, vp);
# endif
    }

    OPENVRML_LOCAL void remove_value_root(JSContext * const cx,
                                          jsval * const vp)
    {
# ifdef OPENVRML_JS_HAS_TYPED_ROOT_FUNCTIONS
        JS_RemoveValueRoot(cx, vp);
# else
        JS_RemoveRoot(cx, vp);
# endif
    }

    OPENVRML_LOCAL double jsval_to_double(const jsval val)
    {
# if OPENVRML_JSVAL_TO_DOUBLE_RETURNS_POINTER
        return *JSVAL_TO_DOUBLE(val);
# else
        return JSVAL_TO_DOUBLE(val);
# endif
    }

    OPENVRML_LOCAL
    JSObject *
    js_construct_object(JSContext * const cx,
                        JSClass * const clasp,
                        JSObject * const proto,
                        JSObject * const parent)
    {
        assert(!proto);
# ifdef OPENVRML_JS_CONSTRUCTOBJECT_WITHOUT_PROTO
        return JS_ConstructObject(cx, clasp, parent);
# else
        return JS_ConstructObject(cx, clasp, proto, parent);
# endif
    }

    OPENVRML_LOCAL
    JSObject *
    js_construct_object_with_arguments(JSContext * const cx,
                                       JSClass * const clasp,
                                       JSObject * const proto,
                                       JSObject * const parent,
                                       const unsigned argc,
                                       jsval * const argv)
    {
        assert(!proto);
# ifdef OPENVRML_JS_CONSTRUCTOBJECT_WITHOUT_PROTO
        return JS_ConstructObjectWithArguments(cx, clasp, parent, argc, argv);
# else
        return JS_ConstructObjectWithArguments(cx, clasp, proto, parent,
                                               argc, argv);
# endif
    }


# ifdef OPENVRML_FAST_JSNATIVE
#   define OPENVRML_DECLARE_JSNATIVE(name)                      \
    JSBool (name)(JSContext * cx, unsigned argc, jsval * vp)
# else
#   define OPENVRML_DECLARE_JSNATIVE(name)                              \
    JSBool (name)(JSContext * cx, JSObject * obj, unsigned argc, jsval * argv, \
                  jsval * rval)
# endif

# ifdef OPENVRML_FAST_JSNATIVE
#   define OPENVRML_DECLARE_MEMBER_JSNATIVE(class_name, name)           \
    JSBool class_name::name(JSContext * cx, unsigned argc, jsval * vp)
# else
#   define OPENVRML_DECLARE_MEMBER_JSNATIVE(class_name, name)   \
    JSBool class_name::name(JSContext * cx, JSObject * obj,     \
                            unsigned argc, jsval * argv,           \
                            jsval * rval)
# endif

# ifdef OPENVRML_FAST_JSNATIVE
#   define OPENVRML_DEFINE_JSNATIVE(name)                               \
    JSBool (name)(JSContext * const cx, const unsigned argc, jsval * const vp)
# else
#   define OPENVRML_DEFINE_JSNATIVE(name)                               \
    JSBool (name)(JSContext * const cx, JSObject * obj,                 \
                  const unsigned argc, jsval * const argv, jsval * const rval)
# endif

# ifdef OPENVRML_FAST_JSNATIVE
#   define OPENVRML_DEFINE_MEMBER_JSNATIVE(class_name, name)            \
    JSBool class_name::name(JSContext * const cx, const unsigned argc,     \
                            jsval * const vp)
# else
#   define OPENVRML_DEFINE_MEMBER_JSNATIVE(class_name, name)            \
    JSBool class_name::name(JSContext * const cx, JSObject * obj,       \
                            const unsigned argc, jsval * const argv,       \
                            jsval * const rval)
# endif

# ifdef OPENVRML_FAST_JSNATIVE
#   define OPENVRML_JS_ARGV(cx, vp) JS_ARGV(cx, vp)
# else
#   define OPENVRML_JS_ARGV(cx, vp) argv
# endif

# ifdef OPENVRML_FAST_JSNATIVE
#   define OPENVRML_JS_SET_RVAL(cx, vp, value) JS_SET_RVAL((cx), (vp), (value))
# else
#   define OPENVRML_JS_SET_RVAL(cx, vp, value) *rval = (value)
# endif

# ifdef OPENVRML_FAST_JSNATIVE
#   define OPENVRML_JS_RVAL(cx, vp) JS_RVAL((cx), (vp))
# else
#   define OPENVRML_JS_RVAL(cx, vp) *rval
# endif

# ifdef OPENVRML_FAST_JSNATIVE
#   define OPENVRML_JS_THIS_OBJECT(cx, vp) JS_THIS_OBJECT((cx), (vp))
# else
#   define OPENVRML_JS_THIS_OBJECT(cx, vp) obj
# endif

# ifdef OPENVRML_FAST_JSNATIVE
#   define OPENVRML_JS_IS_CONSTRUCTING(cx, vp) JS_IsConstructing(cx, vp)
# else
#   define OPENVRML_JS_IS_CONSTRUCTING(cx, vp) JS_IsConstructing(cx)
# endif

# ifdef OPENVRML_JS_HAS_STRICTPROPERTYOP
#   define OPENVRML_DECLARE_JSSTRICTPROPERTYOP(name)                    \
    JSBool (name)(JSContext *, JSObject *, jspropertyop_id, JSBool, jsval *)

#   define OPENVRML_DEFINE_JSSTRICTPROPERTYOP(name)                   \
    JSBool (name)(JSContext * cx, JSObject * obj, jspropertyop_id id, \
                  JSBool strict, jsval * vp)

#   define OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(class_name, name)  \
    JSBool class_name::name(JSContext * cx, JSObject * obj,             \
                            jspropertyop_id id, JSBool strict, jsval * vp)

#   define OPENVRML_CALL_JSSTRICTPROPERTYOP(name, cx, obj, id, vp) \
    (name)((cx), (obj), (id), JS_FALSE, (vp))

    JSStrictPropertyOp openvrml_JS_StrictPropertyStub = JS_StrictPropertyStub;
# else
#   define OPENVRML_DECLARE_JSSTRICTPROPERTYOP(name)                    \
    JSBool (name)(JSContext *, JSObject *, jspropertyop_id, jsval *)

#   define OPENVRML_DEFINE_JSSTRICTPROPERTYOP(name)                   \
    JSBool (name)(JSContext * cx, JSObject * obj, jspropertyop_id id, \
                  jsval * vp)

#   define OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(class_name, name)  \
    JSBool class_name::name(JSContext * cx, JSObject * obj,             \
                            jspropertyop_id id, jsval * vp)

#   define OPENVRML_CALL_JSSTRICTPROPERTYOP(name, cx, obj, id, vp) \
    (name)((cx), (obj), (id), (vp))

    JSPropertyOp openvrml_JS_StrictPropertyStub = JS_PropertyStub;
# endif

    class SFNode;
    class MFNode;

    namespace Browser {
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(createVrmlFromURL);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(addRoute);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(deleteRoute);
    }

    class OPENVRML_JAVASCRIPT_LOCAL script : public openvrml::script {

        friend class SFNode;
        friend class MFNode;
        friend OPENVRML_DECLARE_MEMBER_JSNATIVE(Browser, createVrmlFromURL);
        friend OPENVRML_DECLARE_MEMBER_JSNATIVE(Browser, addRoute);
        friend OPENVRML_DECLARE_MEMBER_JSNATIVE(Browser, deleteRoute);

        JSRuntime * rt;

        double d_timeStamp;

        JSContext * cx;
        JSClass & sfnode_class;
# ifndef NDEBUG
        boost::thread::id thread_id_;
# endif

    public:
        script(openvrml::script_node & node,
               const boost::shared_ptr<openvrml::resource_istream> & source)
            OPENVRML_THROW1(std::bad_alloc);
        virtual ~script();

        openvrml::script_node & script_node();

        using openvrml::script::direct_output;

        jsval vrmlFieldToJSVal(const openvrml::field_value & value)
            OPENVRML_NOTHROW;

    private:
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(field_setProperty);

        virtual void do_initialize(double timeStamp);
        virtual void do_process_event(const std::string & id,
                                      const openvrml::field_value & value,
                                      double timestamp);
        virtual void do_events_processed(double timeStamp);
        virtual void do_shutdown(double timeStamp);

        void initVrmlClasses() OPENVRML_THROW1(std::bad_alloc);
        void defineBrowserObject() OPENVRML_THROW1(std::bad_alloc);
        void defineFields() OPENVRML_THROW1(std::bad_alloc);
        void activate(double timeStamp, const std::string & fname,
                      size_t argc, const openvrml::field_value * const argv[]);
    };


    class OPENVRML_JAVASCRIPT_LOCAL script_factory : public openvrml::script_factory {
    public:
        virtual ~script_factory() OPENVRML_NOTHROW;

        virtual std::auto_ptr<openvrml::script>
            create_script(
                openvrml::script_node & node,
                const boost::shared_ptr<openvrml::resource_istream> & source);
    };
} // namespace

extern "C" OPENVRML_JAVASCRIPT_API void
openvrml_script_LTX_register_factory(
    openvrml::script_factory_registry & registry)
{
    using std::set;
    using std::string;
    using boost::array;
    using boost::shared_ptr;

    static const array<const char *, 5> media_types = {
        "application/ecmascript",
        "application/javascript",
        "application/x-javascript",
        "text/ecmascript",
        "text/javascript"
    };

    static const array<const char *, 2> uri_schemes = {
        "ecmascript",
        "javascript"
    };

    static const set<string> media_type_set(media_types.begin(),
                                            media_types.end());
    static const set<string> uri_scheme_set(uri_schemes.begin(),
                                            uri_schemes.end());

    const shared_ptr<openvrml::script_factory> factory(new script_factory);

    registry.register_factory(media_type_set, uri_scheme_set, factory);
}

namespace {

    const long MAX_HEAP_BYTES = 4L * 1024L * 1024L;
    const long STACK_CHUNK_BYTES = 4024L;

    class OPENVRML_JAVASCRIPT_LOCAL bad_conversion : public std::runtime_error {
    public:
        bad_conversion(const std::string & msg): runtime_error(msg) {}
        virtual ~bad_conversion() throw () {}
    };

    OPENVRML_JAVASCRIPT_LOCAL JSBool floatsToJSArray(size_t numFloats,
                                          const float * floats,
                                          JSContext * cx, jsval * rval);
    OPENVRML_JAVASCRIPT_LOCAL std::auto_ptr<openvrml::field_value>
    createFieldValueFromJsval(JSContext * cx, jsval val,
                              openvrml::field_value::type_id fieldType)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc);

    namespace Global {
        JSClass jsclass = {
            "global",         // name
            0,                // flags
            JS_PropertyStub,  // addProperty
            JS_PropertyStub,  // delProperty
            JS_PropertyStub,  // getProperty
            openvrml_JS_StrictPropertyStub,  // setProperty
            JS_EnumerateStub, // enumerate
            JS_ResolveStub,   // resolve
            JS_ConvertStub,   // convert
            JS_FinalizeStub,  // finalize
            0,                // getObjectOps
            0,                // checkAccess
            0,                // call
            0,                // construct
            0,                // xdrObject
            0,                // hasInstance
            0,                // mark
            0                 // spare
        };
        OPENVRML_DECLARE_JSNATIVE(print);
    }

    namespace Browser {
        JSClass jsclass = {
            "Browser",        // name
            0,                // flags
            JS_PropertyStub,  // addProperty
            JS_PropertyStub,  // delProperty
            JS_PropertyStub,  // getProperty
            openvrml_JS_StrictPropertyStub,  // setProperty
            JS_EnumerateStub, // enumerate
            JS_ResolveStub,   // resolve
            JS_ConvertStub,   // convert
            JS_FinalizeStub,  // finalize
            0,                // getObjectOps
            0,                // checkAccess
            0,                // call
            0,                // construct
            0,                // xdrObject
            0,                // hasInstance
            0,                // mark
            0                 // spare
        };
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(getName);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(getVersion);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(getCurrentSpeed);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(getCurrentFrameRate);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(getWorldURL);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(replaceWorld);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(createVrmlFromString);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(createVrmlFromURL);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(addRoute);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(deleteRoute);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(loadURL);
        OPENVRML_JAVASCRIPT_LOCAL OPENVRML_DECLARE_JSNATIVE(setDescription);
    }

    //
    // A base class for a field value "holder" that gets owned by the
    // JavaScript objects corresponding to VRML data values. The holder
    // includes essential metadata: the corresponding interface ID for
    // the object, if any, and whether or not the object corresponds to
    // an eventOut.
    //
    class OPENVRML_JAVASCRIPT_LOCAL field_data : boost::noncopyable {
    public:
        bool changed;

        virtual ~field_data() = 0;

    protected:
        field_data();
    };

    class OPENVRML_JAVASCRIPT_LOCAL sfield {
    public:
        /**
         * The sfdata class takes ownership of the field_value and deletes it
         * upon destruction.
         */
        class sfdata : public field_data {
            openvrml::field_value * const field_value_;

        public:
            explicit sfdata(openvrml::field_value * value);
            virtual ~sfdata();

            openvrml::field_value & field_value() const;
        };

    protected:
        static void finalize(JSContext * cx, JSObject * obj) OPENVRML_NOTHROW;
        static OPENVRML_DECLARE_JSNATIVE(toString);
    private:
        sfield();
    };

    class OPENVRML_JAVASCRIPT_LOCAL SFColor : public sfield {
    public:
        static JSClass jsclass;

        static JSObject * initClass(JSContext * cx, JSObject * obj)
            OPENVRML_NOTHROW;
        static JSBool toJsval(const openvrml::color & sfcolor,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::sfcolor>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);

    private:
        static OPENVRML_DECLARE_JSNATIVE(construct);
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 const double (&rgb)[3])
            OPENVRML_NOTHROW;
        static JSBool getProperty(JSContext * cx, JSObject * obj,
                                  jspropertyop_id id, jsval * vp)
            OPENVRML_NOTHROW;
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setProperty);
        static OPENVRML_DECLARE_JSNATIVE(setHSV);
        static OPENVRML_DECLARE_JSNATIVE(getHSV);
    };

    class OPENVRML_JAVASCRIPT_LOCAL SFImage : public sfield {
    public:
        static JSClass jsclass;

        static JSObject * initClass(JSContext * cx, JSObject * obj)
            OPENVRML_NOTHROW;
        static JSBool toJsval(const openvrml::sfimage & sfcolor,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::sfimage>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);

    private:
        static OPENVRML_DECLARE_JSNATIVE(construct);
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 uint32_t x, uint32_t y, uint32_t comp,
                                 JSObject * pixels_obj)
            OPENVRML_NOTHROW;
        static JSBool getProperty(JSContext * cx, JSObject * obj,
                                  jspropertyop_id id, jsval * vp)
            OPENVRML_NOTHROW;
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setProperty);
    };

    class OPENVRML_JAVASCRIPT_LOCAL SFNode : public sfield {
    public:
        static JSClass jsclass;
        static JSClass direct_output_jsclass;

        static JSObject * initClass(JSContext * cx, JSObject * obj)
            OPENVRML_NOTHROW;
        static JSBool toJsval(
            const boost::intrusive_ptr<openvrml::node> & node,
            JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::sfnode>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
        static OPENVRML_DECLARE_JSNATIVE(construct);
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 JSString * vrmlstring)
            OPENVRML_NOTHROW;
        static JSBool getProperty(JSContext * cx, JSObject * obj,
                                  jspropertyop_id id, jsval * vp)
            OPENVRML_NOTHROW;
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setProperty);
    };

    class OPENVRML_JAVASCRIPT_LOCAL SFRotation : public sfield {
    public:
        static JSClass jsclass;

        static JSObject * initClass(JSContext * cx, JSObject * obj)
            OPENVRML_NOTHROW;
        static JSBool toJsval(const openvrml::rotation & rotation,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::sfrotation>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);

    private:
        static OPENVRML_DECLARE_JSNATIVE(construct);
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 const double (&rot)[4])
            OPENVRML_NOTHROW;
        static JSBool getProperty(JSContext * cx, JSObject * obj,
                                  jspropertyop_id id, jsval * vp)
            OPENVRML_NOTHROW;
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setProperty);
        static OPENVRML_DECLARE_JSNATIVE(getAxis);
        static OPENVRML_DECLARE_JSNATIVE(inverse);
        static OPENVRML_DECLARE_JSNATIVE(multiply);
        static OPENVRML_DECLARE_JSNATIVE(multVec);
        static OPENVRML_DECLARE_JSNATIVE(setAxis);
        static OPENVRML_DECLARE_JSNATIVE(slerp);
    };

    template <typename SFVec2>
    class sfvec2_jsobject : public sfield {
    public:
        static JSObject * initClass(JSContext * cx, JSObject * obj)
            OPENVRML_NOTHROW;

    protected:
        static OPENVRML_DECLARE_JSNATIVE(constructor);
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 const double (&vec)[2])
            OPENVRML_NOTHROW;
        static JSBool getProperty(JSContext * cx, JSObject * obj,
                                  jspropertyop_id id, jsval * rval)
            OPENVRML_NOTHROW;
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setProperty);
        static OPENVRML_DECLARE_JSNATIVE(add);
        static OPENVRML_DECLARE_JSNATIVE(divide);
        static OPENVRML_DECLARE_JSNATIVE(dot);
        static OPENVRML_DECLARE_JSNATIVE(length);
        static OPENVRML_DECLARE_JSNATIVE(multiply);
        static OPENVRML_DECLARE_JSNATIVE(negate);
        static OPENVRML_DECLARE_JSNATIVE(normalize);
        static OPENVRML_DECLARE_JSNATIVE(subtract);
    };

    class OPENVRML_JAVASCRIPT_LOCAL SFVec2f : public sfvec2_jsobject<SFVec2f> {
    public:
        typedef ::openvrml::sfvec2f field_type;
        typedef field_type::value_type value_type;

        static JSClass jsclass;

        static JSBool toJsval(const value_type & vec2f,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<field_type>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class OPENVRML_JAVASCRIPT_LOCAL SFVec2d : public sfvec2_jsobject<SFVec2f> {
    public:
        typedef ::openvrml::sfvec2d field_type;
        typedef field_type::value_type value_type;

        static JSClass jsclass;

        static JSBool toJsval(const value_type & vec2d,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<field_type>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    template <typename SFVec3>
    class sfvec3_jsobject : public sfield {
    public:
        static JSObject * initClass(JSContext * cx, JSObject * obj)
            OPENVRML_NOTHROW;

    protected:
        static OPENVRML_DEFINE_JSNATIVE(constructor);
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 const double (&vec)[3])
            OPENVRML_NOTHROW;
        static JSBool getProperty(JSContext * cx, JSObject * obj,
                                  jspropertyop_id id, jsval * vp)
            OPENVRML_NOTHROW;
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setProperty);
        static OPENVRML_DEFINE_JSNATIVE(add);
        static OPENVRML_DEFINE_JSNATIVE(cross);
        static OPENVRML_DEFINE_JSNATIVE(divide);
        static OPENVRML_DEFINE_JSNATIVE(dot);
        static OPENVRML_DEFINE_JSNATIVE(length);
        static OPENVRML_DEFINE_JSNATIVE(multiply);
        static OPENVRML_DEFINE_JSNATIVE(negate);
        static OPENVRML_DEFINE_JSNATIVE(normalize);
        static OPENVRML_DEFINE_JSNATIVE(subtract);
    };

    class OPENVRML_JAVASCRIPT_LOCAL SFVec3f : public sfvec3_jsobject<SFVec3f> {
    public:
        typedef ::openvrml::sfvec3f field_type;
        typedef field_type::value_type value_type;

        static JSClass jsclass;

        static JSBool toJsval(const value_type & vec3f,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<field_type>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class OPENVRML_JAVASCRIPT_LOCAL SFVec3d : public sfvec3_jsobject<SFVec3d> {
    public:
        typedef ::openvrml::sfvec3d field_type;
        typedef field_type::value_type value_type;

        static JSClass jsclass;

        static JSBool toJsval(const value_type & vec3d,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<field_type>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class OPENVRML_JAVASCRIPT_LOCAL MField {
    public:
        typedef std::vector<jsval> JsvalArray;

        class MFData : public field_data {
        public:
            JsvalArray array;

            explicit MFData(JsvalArray::size_type size);
            virtual ~MFData();
        };

    protected:
        static void AddRoots(JSContext * cx, JsvalArray & jsvalArray)
            OPENVRML_THROW1(std::bad_alloc);
        static void RemoveRoots(JSContext * cx, JsvalArray & jsvalArray)
            OPENVRML_NOTHROW;

        static JSBool getElement(JSContext * cx, JSObject * obj,
                                 jspropertyop_id id, jsval * vp)
            OPENVRML_NOTHROW;
        static JSBool getLength(JSContext * cx, JSObject * obj,
                                jspropertyop_id id, jsval * vp)
            OPENVRML_NOTHROW;

    private:
        MField();
    };

    /**
     * @brief Base class template for SFColor, SFNode, SFRotation,
     *        SFVec2f, SFVec3f.
     */
    template <typename Subclass>
    class OPENVRML_JAVASCRIPT_LOCAL MFJSObject : public MField {
    public:
        static JSObject * initClass(JSContext * cx, JSObject * obj)
            OPENVRML_NOTHROW;

    protected:
        static OPENVRML_DECLARE_JSNATIVE(construct);
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setElement);
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setLength);
        static OPENVRML_DECLARE_JSNATIVE(toString);
        static void finalize(JSContext * cx, JSObject * obj) OPENVRML_NOTHROW;

    private:
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 unsigned argc, jsval * argv) OPENVRML_NOTHROW;
    };

    /**
     * @brief Base class template for MFFloat, MFTime.
     *
     * In JavaScript, all floating point values are treated as doubles.
     */
    template <typename Subclass>
    class OPENVRML_JAVASCRIPT_LOCAL MFJSDouble : public MField {
    public:
        static JSObject * initClass(JSContext * cx, JSObject * obj)
            OPENVRML_NOTHROW;

    protected:
        static OPENVRML_DECLARE_JSNATIVE(construct);
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setElement);
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setLength);
        static OPENVRML_DECLARE_JSNATIVE(toString);
        static void finalize(JSContext * cx, JSObject * obj) OPENVRML_NOTHROW;

    private:
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 unsigned argc, jsval * argv) OPENVRML_NOTHROW;
    };

    class OPENVRML_JAVASCRIPT_LOCAL MFBool : public MField {
    public:
        static JSClass jsclass;

        static JSBool toJsval(const std::vector<bool> & bools,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mfbool>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);

    private:
        static OPENVRML_DECLARE_JSNATIVE(construct);
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 unsigned argc, jsval * argv);
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setElement);
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setLength);
        static OPENVRML_DECLARE_JSNATIVE(toString);
        static void finalize(JSContext * cx, JSObject * obj);
    };

    class OPENVRML_JAVASCRIPT_LOCAL MFColor : public MFJSObject<MFColor> {
    public:
        static JSClass jsclass;
        static JSClass & sfjsclass;

        static JSBool toJsval(const std::vector<openvrml::color> & colors,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mfcolor>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class OPENVRML_JAVASCRIPT_LOCAL MFFloat : public MFJSDouble<MFFloat> {
    public:
        static JSClass jsclass;

        static JSBool toJsval(const std::vector<float> & floats,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mffloat>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class MFDouble : public MFJSDouble<MFDouble> {
    public:
        static JSClass jsclass;

        static JSBool toJsval(const std::vector<double> & doubles,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mfdouble>
        createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class OPENVRML_JAVASCRIPT_LOCAL MFInt32 : public MField {
    public:
        static JSClass jsclass;

        static JSObject * initClass(JSContext * cx, JSObject * obj);
        static JSBool toJsval(const std::vector<openvrml::int32> & int32s,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mfint32>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);

    private:
        static OPENVRML_DECLARE_JSNATIVE(construct);
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 unsigned argc, jsval * argv);
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setElement);
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setLength);
        static OPENVRML_DECLARE_JSNATIVE(toString);
        static void finalize(JSContext * cx, JSObject * obj);
    };

    //
    // We can't use the MFJSObject<> template to implement MFNode, since
    // the SFNode constructor requires an argument.
    //
    class OPENVRML_JAVASCRIPT_LOCAL MFNode : public MField {
    public:
        static JSClass jsclass;

        static JSObject * initClass(JSContext * cx, JSObject * obj)
            OPENVRML_NOTHROW;
        static JSBool toJsval(
            const std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
            JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mfnode>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);

    private:
        static OPENVRML_DECLARE_JSNATIVE(construct);
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 unsigned argc, jsval * argv) OPENVRML_NOTHROW;
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setElement);
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setLength);
        static OPENVRML_DECLARE_JSNATIVE(toString);
        static void finalize(JSContext * cx, JSObject * obj) OPENVRML_NOTHROW;
    };

    class OPENVRML_JAVASCRIPT_LOCAL MFRotation : public MFJSObject<MFRotation> {
    public:
        static JSClass jsclass;
        static JSClass & sfjsclass;

        static JSBool toJsval(
            const std::vector<openvrml::rotation> & rotations,
            JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mfrotation>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class OPENVRML_JAVASCRIPT_LOCAL MFString : public MField {
    public:
        static JSClass jsclass;

        static JSObject * initClass(JSContext * cx, JSObject * obj)
            OPENVRML_NOTHROW;
        static JSBool toJsval(const std::vector<std::string> & strings,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mfstring>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);

    private:
        static OPENVRML_DECLARE_JSNATIVE(construct);
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 unsigned argc, jsval * argv) OPENVRML_NOTHROW;
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setElement);
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setLength);
        static OPENVRML_DECLARE_JSNATIVE(toString);
        static void finalize(JSContext * cx, JSObject * obj) OPENVRML_NOTHROW;
    };

    class OPENVRML_JAVASCRIPT_LOCAL MFTime : public MFJSDouble<MFTime> {
    public:
        static JSClass jsclass;

        static JSBool toJsval(const std::vector<double> & times,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mftime>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class OPENVRML_JAVASCRIPT_LOCAL MFVec2f : public MFJSObject<MFVec2f> {
    public:
        static JSClass jsclass;
        static JSClass & sfjsclass;

        static JSBool toJsval(const std::vector<openvrml::vec2f> & vec2fs,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mfvec2f>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class MFVec2d : public MFJSObject<MFVec2d> {
    public:
        static JSClass jsclass;
        static JSClass & sfjsclass;

        static JSBool toJsval(const std::vector<openvrml::vec2d> & vec2ds,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mfvec2d>
        createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class OPENVRML_JAVASCRIPT_LOCAL MFVec3f : public MFJSObject<MFVec3f> {
    public:
        static JSClass jsclass;
        static JSClass & sfjsclass;

        static JSBool toJsval(const std::vector<openvrml::vec3f> & vec3fs,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mfvec3f>
            createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class MFVec3d : public MFJSObject<MFVec3d> {
    public:
        static JSClass jsclass;
        static JSClass & sfjsclass;

        static JSBool toJsval(const std::vector<openvrml::vec3d> & vec3ds,
                              JSContext * cx, JSObject * obj, jsval * rval)
            OPENVRML_NOTHROW;
        static std::auto_ptr<openvrml::mfvec3d>
        createFromJSObject(JSContext * cx, JSObject * obj)
            OPENVRML_THROW2(bad_conversion, std::bad_alloc);
    };

    class OPENVRML_JAVASCRIPT_LOCAL VrmlMatrix {
    public:
        //
        // The VrmlMatrixRow_ class is returned when using the [] operator
        // on a VrmlMatrix in order to facilitate matrix[i][j] notation. A
        // VrmlMatrixRow_ object holds a pointer to a row of the
        // openvrml::mat4f associated with the JavaScript VrmlMatrix that
        // spawned it. The only useful thing you can do with a
        // VrmlMatrixRow_ object is use the [] operator to dereference a
        // row element.
        //
        class Row {
        public:
            static JSClass jsclass;

            static JSObject * initClass(JSContext * cx, JSObject * obj)
                OPENVRML_NOTHROW;
            static OPENVRML_DECLARE_JSNATIVE(construct);
            static JSBool getElement(JSContext * cx, JSObject * obj,
                                     jspropertyop_id id, jsval * vp)
                OPENVRML_NOTHROW;
            static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setElement);
        };

        static JSClass jsclass;

        static JSObject * initClass(JSContext * cx, JSObject * obj)
            OPENVRML_NOTHROW;
        static OPENVRML_DECLARE_JSNATIVE(construct);
        static JSBool initObject(JSContext * cx, JSObject * obj,
                                 const double (&mat)[16])
            OPENVRML_NOTHROW;
        static JSBool getElement(JSContext * cx, JSObject * obj,
                                 jspropertyop_id id, jsval * vp)
            OPENVRML_NOTHROW;
        static OPENVRML_DECLARE_JSSTRICTPROPERTYOP(setElement);
        static OPENVRML_DECLARE_JSNATIVE(setTransform);
        static OPENVRML_DECLARE_JSNATIVE(getTransform);
        static OPENVRML_DECLARE_JSNATIVE(inverse);
        static OPENVRML_DECLARE_JSNATIVE(transpose);
        static OPENVRML_DECLARE_JSNATIVE(multLeft);
        static OPENVRML_DECLARE_JSNATIVE(multRight);
        static OPENVRML_DECLARE_JSNATIVE(multVecMatrix);
        static OPENVRML_DECLARE_JSNATIVE(multMatrixVec);
        static OPENVRML_DECLARE_JSNATIVE(toString);
        static void finalize(JSContext * cx, JSObject * obj) OPENVRML_NOTHROW;

    private:
        VrmlMatrix();
    };

    OPENVRML_JAVASCRIPT_LOCAL
    OPENVRML_DECLARE_JSSTRICTPROPERTYOP(eventOut_setProperty);

    OPENVRML_JAVASCRIPT_LOCAL void errorReporter(JSContext * cx,
                                      const char * message,
                                      JSErrorReport * report);


    // Construct from inline script

    script::
    script(openvrml::script_node & node,
           const boost::shared_ptr<openvrml::resource_istream> & source)
        OPENVRML_THROW1(std::bad_alloc):
        openvrml::script(node),
        rt(0),
        cx(0),
        sfnode_class(this->direct_output()
                     ? SFNode::direct_output_jsclass
                     : SFNode::jsclass)
# ifndef NDEBUG
        ,thread_id_(boost::this_thread::get_id())
# endif
    {
        using std::bad_alloc;

        //
        // Initialize the runtime.
        //
        this->rt = JS_NewRuntime(MAX_HEAP_BYTES);
        if (!this->rt) { throw bad_alloc(); }

        //
        // Initialize the context for this script object.
        //
        this->cx = JS_NewContext(rt, STACK_CHUNK_BYTES);
        if (!this->cx) { throw bad_alloc(); }

        //
        // Store a pointer to this script object in the context.
        //
        JS_SetContextPrivate(this->cx, this);

        JS_SetErrorReporter(cx, errorReporter);

        //
        // Define the global objects (builtins, Browser, SF*, MF*) ...
        //

        JSObject * const globalObj =
            JS_NewObject(this->cx, &Global::jsclass, 0, 0);
        if (!globalObj) { throw bad_alloc(); }

        if (!JS_InitStandardClasses(this->cx, globalObj)) {
            throw bad_alloc();
        }

        //
        // "print" function; non-standard, but widely used.
        //
        static JSFunctionSpec globalFunctions[] = {
            {
                "print",       // name
                Global::print, // call
                0,             // nargs
                0              // flags
            },
            { 0, 0, 0, 0 }
        };
        JSBool ok = JS_DefineFunctions(cx, globalObj, globalFunctions);
        assert(ok);

        //
        // VRML-like TRUE, FALSE syntax
        //
        if (!(JS_DefineProperty(cx, globalObj, "FALSE",
                                BOOLEAN_TO_JSVAL(false), 0, 0,
                                JSPROP_READONLY | JSPROP_PERMANENT)
              && JS_DefineProperty(cx, globalObj, "TRUE",
                                   BOOLEAN_TO_JSVAL(true), 0, 0,
                                   JSPROP_READONLY | JSPROP_PERMANENT))) {
            throw std::bad_alloc();
        }

        //
        // Browser object.
        //
        this->defineBrowserObject();

        //
        // Define SF*/MF* classes.
        //
        this->initVrmlClasses();

        //
        // Define field/eventOut vars for this script.
        //
        this->defineFields();

        /* These should indicate source location for diagnostics. */
        char *filename = 0;
        unsigned lineno = 0;

        using std::string;
        using std::istream;
        typedef std::istreambuf_iterator<istream::char_type>
            istreambuf_iterator_t;

        const string source_str((istreambuf_iterator_t(*source)),
                                istreambuf_iterator_t());

        jsval rval;
        ok = JS_EvaluateScript(cx, globalObj,
                               source_str.c_str(), source_str.length(),
                               filename, lineno, &rval);
        OPENVRML_VERIFY_(ok);
    }

    script::~script()
    {
        JS_DestroyContext(this->cx);
        JS_DestroyRuntime(this->rt);
    }

    void script::do_initialize(const double timestamp)
    {
        const openvrml::sftime arg(timestamp);
        const openvrml::field_value * argv[] = { &arg };
        this->activate(timestamp, "initialize", 1, argv);
    }

    void script::do_process_event(const std::string & id,
                                  const openvrml::field_value & value,
                                  const double timestamp)
    {
        const openvrml::sftime timestampArg(timestamp);
        const openvrml::field_value * argv[] = { &value, &timestampArg };
        this->activate(timestamp, id, 2, argv);
    }

    void script::do_events_processed(const double timestamp)
    {
        const openvrml::sftime arg(timestamp);
        const openvrml::field_value * argv[] = { &arg };
        this->activate(timestamp, "eventsProcessed", 1, argv);
    }

    void script::do_shutdown(const double timestamp)
    {
        const openvrml::sftime arg(timestamp);
        const openvrml::field_value * argv[] = { &arg };
        this->activate(timestamp, "shutdown", 1, argv);
    }

    double s_timeStamp; // XXX go away...

    /**
     * @brief Run a specified script.
     *
     * @todo Decide what to do if a call to a JavaScript function fails
     *      (probably due to an error in the script).
     */
    void script::activate(const double timeStamp, const std::string & fname,
                          const size_t argc,
                          const openvrml::field_value * const argv[])
    {
        assert(this->cx);
        assert(this->thread_id_ == boost::this_thread::get_id());

        jsval fval, rval;
        JSObject * const globalObj = JS_GetGlobalObject(this->cx);
        assert(globalObj);

        try {
            if (!JS_LookupProperty(this->cx, globalObj,
                                   fname.c_str(), &fval)) {
                throw std::bad_alloc();
            }

            //
            // The function may not be defined, in which case we should do
            // nothing.
            //
            if (JSVAL_IS_VOID(fval)) { return; }

            d_timeStamp = timeStamp;
            s_timeStamp = timeStamp; // XXX this won't work for long...

            // convert FieldValue*'s to (gc-protected) jsvals
            std::vector<jsval> jsargv(argc);
            size_t i;
            for (i = 0; i < argc; ++i) {
                assert(argv[i]);
                jsargv[i] = vrmlFieldToJSVal(*argv[i]);
                if (JSVAL_IS_GCTHING(jsargv[i])) {
                    if (!add_value_root(this->cx, &jsargv[i])) {
                        throw std::bad_alloc();
                    }
                }
            }

            JSBool ok = JS_CallFunctionValue(this->cx, globalObj,
                                             fval, argc, &jsargv[0], &rval);
            // XXX
            // XXX What should we do at this point if a function call fails?
            // XXX For now, just print a message for a debug build.
            // XXX
            OPENVRML_VERIFY_(ok);

            // Free up args
            for (i = 0; i < argc; ++i) {
                assert(jsargv[i] != JSVAL_NULL);
                if (JSVAL_IS_GCTHING(jsargv[i])) {
                    remove_value_root(cx, &jsargv[i]);
                }
            }

            //
            // Check to see if any eventOuts need to be sent.
            //
            for (openvrml::script_node::eventout_map_t::const_iterator eventout =
                     this->node.eventout_map().begin();
                 eventout != this->node.eventout_map().end();
                 ++eventout) {
                assert(eventout->second);
                jsval val;
                if (!JS_LookupProperty(this->cx, globalObj,
                                       eventout->first.c_str(), &val)) {
                    throw std::bad_alloc();
                }
                assert(val != JSVAL_VOID);
                if (JSVAL_IS_OBJECT(val)) {
                    field_data * fieldData =
                        static_cast<field_data *>
                        (JS_GetPrivate(this->cx, JSVAL_TO_OBJECT(val)));
                    if (fieldData->changed) {
                        using std::auto_ptr;
                        auto_ptr<openvrml::field_value> fieldValue =
                            createFieldValueFromJsval(
                                this->cx,
                                val,
                                eventout->second->value().type());
                        eventout->second->value(*fieldValue);
                        fieldData->changed = false;
                    }
                }
            }
        } catch (std::exception & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
        } catch (...) {
            assert(false);
        }
        JS_MaybeGC(this->cx);
    }

    openvrml::script_node & script::script_node()
    {
        return this->node;
    }

/**
 * @brief Create a jsval from an openvrml::field_value.
 */
    jsval script::vrmlFieldToJSVal(const openvrml::field_value & fieldValue)
        OPENVRML_NOTHROW
    {
        using boost::polymorphic_downcast;
        using openvrml::field_value;

        jsval rval;
        JSObject * const globalObj = JS_GetGlobalObject(this->cx);
        assert(globalObj);

        switch (fieldValue.type()) {
        case field_value::sfbool_id:
        {
            using openvrml::sfbool;
            const sfbool & b =
                *polymorphic_downcast<const sfbool *>(&fieldValue);
            rval = BOOLEAN_TO_JSVAL(b.value());
        }
        break;

        case field_value::sfcolor_id:
        {
            using openvrml::sfcolor;
            const sfcolor & c =
                *polymorphic_downcast<const sfcolor *>(&fieldValue);
            if (!SFColor::toJsval(c.value(), this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::sffloat_id:
        {
            using openvrml::sffloat;
            const sffloat & f =
                *polymorphic_downcast<const sffloat *>(&fieldValue);
            if (!JS_NewNumberValue(cx, f.value(), &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::sfdouble_id:
        {
            using openvrml::sfdouble;
            const sfdouble & f =
                *polymorphic_downcast<const sfdouble *>(&fieldValue);
            if (!JS_NewNumberValue(cx, f.value(), &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::sfimage_id:
        {
            const openvrml::sfimage & sfimage =
                *polymorphic_downcast<const openvrml::sfimage *>(&fieldValue);
            if (!SFImage::toJsval(sfimage, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::sfint32_id:
        {
            const openvrml::sfint32 & sfint32 =
                *polymorphic_downcast<const openvrml::sfint32 *>(&fieldValue);
            if (!JS_NewNumberValue(cx, static_cast<double>(sfint32.value()),
                                   &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::sfnode_id:
        {
            const openvrml::sfnode & sfnode =
                *polymorphic_downcast<const openvrml::sfnode *>(&fieldValue);
            if (!SFNode::toJsval(sfnode.value(), this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::sfrotation_id:
        {
            const openvrml::sfrotation & sfrotation =
                *polymorphic_downcast<const openvrml::sfrotation *>(
                    &fieldValue);
            if (!SFRotation::toJsval(sfrotation.value(), this->cx, globalObj,
                                     &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::sfstring_id:
        {
            const openvrml::sfstring & sfstring =
                *polymorphic_downcast<const openvrml::sfstring *>(&fieldValue);
            rval = STRING_TO_JSVAL(
                JS_NewStringCopyZ(cx, sfstring.value().c_str()));
        }
        break;

        case field_value::sftime_id:
        {
            const openvrml::sftime & sftime =
                *polymorphic_downcast<const openvrml::sftime *>(&fieldValue);
            if (!JS_NewNumberValue(cx, sftime.value(), &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::sfvec2f_id:
        {
            const openvrml::sfvec2f & sfvec2f =
                *polymorphic_downcast<const openvrml::sfvec2f *>(&fieldValue);
            if (!SFVec2f::toJsval(sfvec2f.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::sfvec2d_id:
        {
            const openvrml::sfvec2d & sfvec2d =
                *polymorphic_downcast<const openvrml::sfvec2d *>(&fieldValue);
            if (!SFVec2d::toJsval(sfvec2d.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::sfvec3f_id:
        {
            const openvrml::sfvec3f & sfvec3f =
                *polymorphic_downcast<const openvrml::sfvec3f *>(&fieldValue);
            if (!SFVec3f::toJsval(sfvec3f.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::sfvec3d_id:
        {
            const openvrml::sfvec3d & sfvec3d =
                *polymorphic_downcast<const openvrml::sfvec3d *>(&fieldValue);
            if (!SFVec3d::toJsval(sfvec3d.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mfbool_id:
        {
            const openvrml::mfbool & mfbool =
                *polymorphic_downcast<const openvrml::mfbool *>(&fieldValue);
            if (!MFBool::toJsval(mfbool.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mfcolor_id:
        {
            const openvrml::mfcolor & mfcolor =
                *polymorphic_downcast<const openvrml::mfcolor *>(&fieldValue);
            if (!MFColor::toJsval(mfcolor.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mffloat_id:
        {
            const openvrml::mffloat & mffloat =
                *polymorphic_downcast<const openvrml::mffloat *>(&fieldValue);
            if (!MFFloat::toJsval(mffloat.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mfdouble_id:
        {
            const openvrml::mfdouble & mfdouble =
                *polymorphic_downcast<const openvrml::mfdouble *>(&fieldValue);
            if (!MFDouble::toJsval(mfdouble.value(),
                                   this->cx,
                                   globalObj,
                                   &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mfint32_id:
        {
            const openvrml::mfint32 & mfint32 =
                *polymorphic_downcast<const openvrml::mfint32 *>(&fieldValue);
            if (!MFInt32::toJsval(mfint32.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mfnode_id:
        {
            const openvrml::mfnode & mfnode =
                *polymorphic_downcast<const openvrml::mfnode *>(&fieldValue);
            if (!MFNode::toJsval(mfnode.value(), this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mfrotation_id:
        {
            const openvrml::mfrotation & mfrotation =
                *polymorphic_downcast<const openvrml::mfrotation *>(
                    &fieldValue);
            if (!MFRotation::toJsval(mfrotation.value(), this->cx, globalObj,
                                     &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mfstring_id:
        {
            const openvrml::mfstring & mfstring =
                *polymorphic_downcast<const openvrml::mfstring *>(&fieldValue);
            if (!MFString::toJsval(mfstring.value(), this->cx, globalObj,
                                   &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mftime_id:
        {
            const openvrml::mftime & mftime =
                *polymorphic_downcast<const openvrml::mftime *>(&fieldValue);
            if (!MFTime::toJsval(mftime.value(), this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mfvec2f_id:
        {
            const openvrml::mfvec2f & mfvec2f =
                *polymorphic_downcast<const openvrml::mfvec2f *>(&fieldValue);
            if (!MFVec2f::toJsval(mfvec2f.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mfvec2d_id:
        {
            const openvrml::mfvec2d & mfvec2d =
                *polymorphic_downcast<const openvrml::mfvec2d *>(&fieldValue);
            if (!MFVec2d::toJsval(mfvec2d.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mfvec3f_id:
        {
            const openvrml::mfvec3f & mfvec3f =
                *polymorphic_downcast<const openvrml::mfvec3f *>(&fieldValue);
            if (!MFVec3f::toJsval(mfvec3f.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        case field_value::mfvec3d_id:
        {
            const openvrml::mfvec3d & mfvec3d =
                *polymorphic_downcast<const openvrml::mfvec3d *>(&fieldValue);
            if (!MFVec3d::toJsval(mfvec3d.value(), this->cx, globalObj, &rval))
            {
                rval = JSVAL_NULL;
            }
        }
        break;

        default:
            assert(false);
        }

        return rval;
    }


// Must assign the proper type to eventOuts

    OPENVRML_DEFINE_JSSTRICTPROPERTYOP(eventOut_setProperty)
    {
        using std::find_if;

        JSString * const str = jspropertyop_id_to_string(cx, id);
        if (!str) { return JS_FALSE; }
        const char * const eventId = JS_EncodeString(cx, str);

        //
        // The script object pointer is stored as private data in the
        // context.
        //
        script * const s = static_cast<script *>(JS_GetContextPrivate(cx));
        assert(s);

        openvrml::script_node & scriptNode = s->script_node();

        const openvrml::node_interface_set & interfaces =
            scriptNode.node::type().interfaces();
        const openvrml::node_interface_set::const_iterator interface_ =
            find_if(interfaces.begin(), interfaces.end(),
                    bind2nd(openvrml::node_interface_matches_eventout(), eventId));
        //
        // If this assertion is false, then we accidentally gave this
        // setter to an object that doesn't correspond to an eventOut!
        //
        assert(interface_ != interfaces.end());

        const openvrml::field_value::type_id field_type_id = interface_->field_type;

        //
        // Convert to an openvrml::field_value and set the eventOut value.
        //
        try {
            using std::auto_ptr;

            auto_ptr<openvrml::field_value> fieldValue =
                createFieldValueFromJsval(cx, *vp, field_type_id);
            const openvrml::script_node::eventout_map_t::const_iterator eventout =
                scriptNode.eventout_map().find(eventId);
            assert(eventout != scriptNode.eventout_map().end());
            eventout->second->value(*fieldValue);
        } catch (bad_conversion & ex) {
            JS_ReportError(cx, ex.what());
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(script, field_setProperty)
    {
        using std::find_if;

        JSString * const str = jspropertyop_id_to_string(cx, id);
        if (!str) { return JS_FALSE; }
        const char * const fieldId = JS_EncodeString(cx, str);

        //
        // The script object pointer is stored as private data in the
        // context.
        //
        script * const s = static_cast<script *>(JS_GetContextPrivate(cx));
        assert(s);

        openvrml::script_node & scriptNode = s->script_node();

        const openvrml::node_interface_set & interfaces =
            scriptNode.node::type().interfaces();
        const openvrml::node_interface_set::const_iterator interface_ =
            find_if(interfaces.begin(), interfaces.end(),
                    bind2nd(openvrml::node_interface_matches_field(), fieldId));
        //
        // If this assertion is false, then we accidentally gave this
        // setter to an object that doesn't correspond to a field!
        //
        assert(interface_ != interfaces.end());

        const openvrml::field_value::type_id field_type_id = interface_->field_type;

        //
        // Convert to an openvrml::FieldValue and set the field value.
        //
        try {
            using std::auto_ptr;
            auto_ptr<openvrml::field_value> fieldValue =
                createFieldValueFromJsval(cx, *vp, field_type_id);
            s->field(fieldId, *fieldValue);
        } catch (bad_conversion & ex) {
            JS_ReportError(cx, ex.what());
            return JS_FALSE;
        } catch (openvrml::unsupported_interface & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            assert(false);
        } catch (std::bad_cast & ex) {
            JS_ReportError(cx, ex.what());
            return JS_FALSE;
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }

        return JS_TRUE;
    }

//
// Initialize SF*/MF* types.
//
    void script::initVrmlClasses() OPENVRML_THROW1(std::bad_alloc)
    {
        JSObject * const globalObj = JS_GetGlobalObject(this->cx);
        assert(globalObj);
        if (!(SFColor::initClass(this->cx, globalObj)
              && SFImage::initClass(this->cx, globalObj)
              && SFNode::initClass(this->cx, globalObj)
              && SFRotation::initClass(this->cx, globalObj)
              && SFVec2f::initClass(this->cx, globalObj)
              && SFVec2d::initClass(this->cx, globalObj)
              && SFVec3f::initClass(this->cx, globalObj)
              && SFVec3d::initClass(this->cx, globalObj)
              && MFColor::initClass(this->cx, globalObj)
              && MFFloat::initClass(this->cx, globalObj)
              && MFDouble::initClass(this->cx, globalObj)
              && MFInt32::initClass(this->cx, globalObj)
              && MFNode::initClass(this->cx, globalObj)
              && MFRotation::initClass(this->cx, globalObj)
              && MFString::initClass(this->cx, globalObj)
              && MFTime::initClass(this->cx, globalObj)
              && MFVec2f::initClass(this->cx, globalObj)
              && MFVec2d::initClass(this->cx, globalObj)
              && MFVec3f::initClass(this->cx, globalObj)
              && MFVec3d::initClass(this->cx, globalObj)
              && VrmlMatrix::initClass(this->cx, globalObj)
              && VrmlMatrix::Row::initClass(this->cx, globalObj))) {
            throw std::bad_alloc();
        }
    }

//
// Define the Browser object.
//
    void script::defineBrowserObject() OPENVRML_THROW1(std::bad_alloc)
    {
        JSObject * const globalObj = JS_GetGlobalObject(this->cx);
        assert(globalObj);

        static JSFunctionSpec methods[] =
            {
                {
                    "getName",                     // name
                    Browser::getName,              // call
                    0,                             // nargs
                    0                              // flags
                },
                {
                    "getVersion",                  // name
                    Browser::getVersion,           // call
                    0,                             // nargs
                    0                              // flags
                },
                {
                    "getCurrentSpeed",             // name
                    Browser::getCurrentSpeed,      // call
                    0,                             // nargs
                    0                              // flags
                },
                {
                    "getCurrentFrameRate",         // name
                    Browser::getCurrentFrameRate,  // call
                    0,                             // nargs
                    0                              // flags
                },
                {
                    "getWorldURL",                 // name
                    Browser::getWorldURL,          // call
                    0,                             // nargs
                    0                              // flags
                },
                {
                    "replaceWorld",                // name
                    Browser::replaceWorld,         // call
                    1,                             // nargs
                    0                              // flags
                },
                {
                    "createVrmlFromString",        // name
                    Browser::createVrmlFromString, // call
                    1,                             // nargs
                    0                              // flags
                },
                {
                    "createVrmlFromURL",           // name
                    Browser::createVrmlFromURL,    // call
                    3,                             // nargs
                    0                              // flags
                },
                {
                    "addRoute",                    // name
                    Browser::addRoute,             // call
                    4,                             // nargs
                    0                              // flags
                },
                {
                    "deleteRoute",                 // name
                    Browser::deleteRoute,          // call
                    4,                             // nargs
                    0                              // flags
                },
                {
                    "loadURL",                     // name
                    Browser::loadURL,              // call
                    2,                             // nargs
                    0                              // flags
                },
                {
                    "setDescription",              // name
                    Browser::setDescription,       // call
                    1,                             // nargs
                    0                              // flags
                },
                { 0, 0, 0, 0 }
            };

        JSObject * const browserObj =
            JS_DefineObject(this->cx, globalObj,
                            "Browser", &Browser::jsclass, 0, 0);
        if (!browserObj) { throw std::bad_alloc(); }

        if (!JS_DefineFunctions(this->cx, browserObj, methods)) {
            throw std::bad_alloc();
        }
    }

//
// Define objects corresponding to fields/eventOuts
//
    void script::defineFields() OPENVRML_THROW1(std::bad_alloc)
    {
        JSObject * const globalObj = JS_GetGlobalObject(this->cx);
        assert(globalObj);

        {
            openvrml::script_node::field_value_map_t::const_iterator
                itr(this->node.field_value_map().begin());
            for (; itr != this->node.field_value_map().end(); ++itr) {
                assert(itr->second);
                jsval val = vrmlFieldToJSVal(*itr->second);
                if (!JS_DefineProperty(this->cx, globalObj,
                                       itr->first.c_str(), val,
                                       0, field_setProperty, // getter, setter
                                       JSPROP_PERMANENT)) {
                    throw std::bad_alloc();
                }
            }
        }

        for (openvrml::script_node::eventout_map_t::const_iterator eventout =
                 this->node.eventout_map().begin();
             eventout != this->node.eventout_map().end();
             ++eventout) {
            assert(eventout->second);
            jsval val = vrmlFieldToJSVal(eventout->second->value());
            if (!JS_DefineProperty(this->cx,
                                   globalObj,
                                   eventout->first.c_str(),
                                   val,
                                   0,                    // getter
                                   eventOut_setProperty, // setter
                                   JSPROP_PERMANENT)) {
                throw std::bad_alloc();
            }
        }
    }


    script_factory::~script_factory() OPENVRML_NOTHROW
    {}

    std::auto_ptr<openvrml::script>
    script_factory::
    create_script(openvrml::script_node & node,
                  const boost::shared_ptr<openvrml::resource_istream> & source)
    {
        return std::auto_ptr<openvrml::script>(new script(node, source));
    }


    void errorReporter(JSContext * const cx,
                       const char * const message,
                       JSErrorReport * const errorReport)
    {
        using std::endl;
        using std::ostringstream;
        using std::string;

        script * const s =
            static_cast<script *>(JS_GetContextPrivate(cx));
        assert(s);

        openvrml::browser & browser = s->script_node().scene()->browser();

        ostringstream err_msg;

        const string node_id = s->script_node().id();
        if (!node_id.empty()) {
            err_msg << node_id << ": ";
        }

        if (errorReport) {
            if (errorReport->filename) {
                err_msg << errorReport->filename << ": ";
            }
            err_msg << errorReport->lineno << ": ";
        }

        err_msg << message;

        browser.err(err_msg.str());
    }

    JSBool floatsToJSArray(const size_t numFloats,
                           const float * floats,
                           JSContext * const cx,
                           jsval * const rval)
    {
        std::vector<jsval> jsvec(numFloats);

        size_t i;
        for (i = 0; i < numFloats; ++i) {
            if (!JS_NewNumberValue(cx, floats[i], &jsvec[i])) {
                return JS_FALSE;
            }
            if (!add_value_root(cx, &jsvec[i])) { return JS_FALSE; }
        }

        JSObject * arr = 0;
        if (i == numFloats) {
            arr = JS_NewArrayObject(cx, numFloats, &jsvec[0]);
            if (arr) { *rval = OBJECT_TO_JSVAL(arr); }
        }

        for (size_t j = 0; j < i; ++j) { remove_value_root(cx, &jsvec[j]); }

        if (!arr) { return JS_FALSE; }

        *rval = OBJECT_TO_JSVAL(arr);
        return JS_TRUE;
    }

/**
 * @brief Convert a jsval to a (new) field_value.
 */
    std::auto_ptr<openvrml::field_value>
    createFieldValueFromJsval(JSContext * const cx,
                              const jsval v,
                              const openvrml::field_value::type_id expectType)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        using std::auto_ptr;
        using namespace openvrml;

        switch (expectType) {
        case field_value::sfbool_id:
        {
            if (!JSVAL_IS_BOOLEAN(v)) {
                throw bad_conversion("Boolean value expected.");
            }
            return auto_ptr<field_value>(new sfbool(JSVAL_TO_BOOLEAN(v)));
        }

        case field_value::sfcolor_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (SFColor::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::sffloat_id:
        {
            double d;
            if (!JS_ValueToNumber(cx, v, &d)) {
                throw bad_conversion("Numeric value expected.");
            }
            return auto_ptr<field_value>(new sffloat(float(d)));
        }

        case field_value::sfdouble_id:
        {
            double d;
            if (!JS_ValueToNumber(cx, v, &d)) {
                throw bad_conversion("Numeric value expected.");
            }
            return auto_ptr<field_value>(new sfdouble(d));
        }

        case field_value::sfint32_id:
        {
            int32_t i;
            if (!JS_ValueToECMAInt32(cx, v, &i)) {
                throw bad_conversion("Numeric value expected.");
            }
            return auto_ptr<field_value>(new sfint32(i));
        }

        case field_value::sfimage_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (SFImage::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::sfnode_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (SFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::sfrotation_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (SFRotation::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::sfstring_id:
        {
            if (!JSVAL_IS_STRING(v)) {
                throw bad_conversion("String value expected.");
            }
            JSString * const jsstring = JSVAL_TO_STRING(v);
            //
            // Is the null check here really necessary? Perhaps this
            // should be an assertion?
            //
            return auto_ptr<field_value>
                (new sfstring(jsstring
                              ? JS_EncodeString(cx, jsstring)
                              : ""));
        }

        case field_value::sftime_id:
        {
            double d;
            if (!JS_ValueToNumber(cx, v, &d)) {
                throw bad_conversion("Numeric value expected.");
            }
            return auto_ptr<field_value>(new sftime(d));
        }

        case field_value::sfvec2f_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (SFVec2f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::sfvec2d_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (SFVec2d::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::sfvec3f_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (SFVec3f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::sfvec3d_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (SFVec3d::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mfbool_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFBool::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mfcolor_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFColor::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mffloat_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFFloat::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mfdouble_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFDouble::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mfint32_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFInt32::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mfnode_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mfrotation_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFRotation::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mfstring_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFString::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mftime_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFTime::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mfvec2f_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFVec2f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mfvec2d_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFVec2d::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mfvec3f_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFVec3f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        case field_value::mfvec3d_id:
            if (!JSVAL_IS_OBJECT(v)) { throw bad_conversion("Object expected."); }
            return auto_ptr<field_value>
                (MFVec3d::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

        default:
            assert(false);
            return auto_ptr<field_value>();
        }
    }

    namespace Global {

        OPENVRML_DEFINE_JSNATIVE(print)
        {
            script * const s = static_cast<script *>(JS_GetContextPrivate(cx));
            assert(s);

            for (unsigned i = 0; i < argc; i++) {
                JSString * const str =
                    JS_ValueToString(cx, OPENVRML_JS_ARGV(cx, vp)[i]);
                if (!str) { return JS_FALSE; }
                s->script_node().scene()->browser()
                    .out(JS_EncodeString(cx, str));
            }
            return JS_TRUE;
        }

    }

    namespace Browser {

        OPENVRML_DEFINE_JSNATIVE(getName)
        {
            script * const s = static_cast<script *>(JS_GetContextPrivate(cx));
            assert(s);

            const char * const name =
                s->script_node().node::type().metatype().browser().name();
            OPENVRML_JS_SET_RVAL(cx, vp,
                                 STRING_TO_JSVAL(JS_InternString(cx, name)));
            return JS_TRUE;
        }

        OPENVRML_DEFINE_JSNATIVE(getVersion)
        {
            script * const s = static_cast<script *>(JS_GetContextPrivate(cx));
            assert(s);

            const char * const version =
                s->script_node().node::type().metatype().browser().version();
            OPENVRML_JS_SET_RVAL(cx, vp,
                                 STRING_TO_JSVAL(JS_InternString(cx, version)));
            return JS_TRUE;
        }

        OPENVRML_DEFINE_JSNATIVE(getCurrentSpeed)
        {
            script * const s = static_cast<script *>(JS_GetContextPrivate(cx));
            assert(s);

            const double speed =
                s->script_node().type().metatype().browser().current_speed();
            jsval result_val;
            const JSBool result = JS_NewNumberValue(cx, speed, &result_val);
            OPENVRML_JS_SET_RVAL(cx, vp, result_val);
            return result;
        }

        OPENVRML_DEFINE_JSNATIVE(getCurrentFrameRate)
        {
            script * const s = static_cast<script *>(JS_GetContextPrivate(cx));
            assert(s);

            const double frame_rate =
                s->script_node().type().metatype().browser().frame_rate();
            jsval result_val;
            const JSBool result =
                JS_NewNumberValue(cx, frame_rate, &result_val);
            OPENVRML_JS_SET_RVAL(cx, vp, result_val);
            return result;
        }

        OPENVRML_DEFINE_JSNATIVE(getWorldURL)
        {
            script * const s = static_cast<script *>(JS_GetContextPrivate(cx));
            assert(s);

            const std::string url =
                s->script_node().node::type().metatype().browser().world_url();
            OPENVRML_JS_SET_RVAL(cx, vp,
                                 STRING_TO_JSVAL(
                                     JS_InternString(cx, url.c_str())));
            return JS_TRUE;
        }


// No events will be processed after loadURL.

        OPENVRML_DEFINE_JSNATIVE(loadURL)
        {
            using std::auto_ptr;

            assert(JS_GetContextPrivate(cx));
            script & s = *static_cast<script *>(JS_GetContextPrivate(cx));

            try {
                openvrml::mfstring url, parameter;

                if (argc > 0) {
                    //
                    // Make sure our first argument (the URL) is an MFString.
                    //
                    JSObject * arg0_obj;
                    if (!JS_ValueToObject(cx, OPENVRML_JS_ARGV(cx, vp)[0],
                                          &arg0_obj)) {
                        return JS_FALSE;
                    }
                    if (!JS_InstanceOf(cx, arg0_obj, &MFString::jsclass,
                                       OPENVRML_JS_ARGV(cx, vp))) {
                        return JS_FALSE;
                    }

                    auto_ptr<openvrml::mfstring> url_ptr =
                        MFString::createFromJSObject(cx, arg0_obj);
                    assert(url_ptr.get());
                    url = *url_ptr;
                }

                if (argc > 1) {
                    //
                    // Make sure our second argument is an MFString
                    //
                    JSObject * arg1_obj;
                    if (!JS_ValueToObject(cx, OPENVRML_JS_ARGV(cx, vp)[1],
                                          &arg1_obj)) {
                        return JS_FALSE;
                    }
                    if (!JS_InstanceOf(cx, arg1_obj, &MFString::jsclass,
                                       OPENVRML_JS_ARGV(cx, vp))) {
                        return JS_FALSE;
                    }

                    auto_ptr<openvrml::mfstring> parameter_ptr =
                        MFString::createFromJSObject(cx, arg1_obj);
                    assert(parameter_ptr.get());
                    parameter = *parameter_ptr;
                }

                s.script_node().scene()->browser().load_url(url.value(),
                                                            parameter.value());
            } catch (std::bad_alloc &) {
                JS_ReportOutOfMemory(cx);
                return JS_FALSE;
            } catch (std::exception & ex) {
                JS_ReportError(cx, ex.what());
            } catch (...) {
                JS_ReportError(cx, "unexpected exception");
            }

            return JS_TRUE;
        }


// This does return, but no events will be processed after it is called.

        OPENVRML_DEFINE_JSNATIVE(replaceWorld)
        {
            using std::auto_ptr;

            assert(JS_GetContextPrivate(cx));
            script & s = *static_cast<script *>(JS_GetContextPrivate(cx));

            //
            // Make sure our argument is an MFNode.
            //
            JSObject * arg_obj;
            if (!JS_ValueToObject(cx, OPENVRML_JS_ARGV(cx, vp)[0], &arg_obj)) {
                return JS_FALSE;
            }
            if (!JS_InstanceOf(cx, arg_obj, &MFNode::jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }

            auto_ptr<openvrml::mfnode> nodes =
                MFNode::createFromJSObject(cx, arg_obj);
            assert(nodes.get());

            s.script_node().scene()->browser().replace_world(nodes->value());

            OPENVRML_JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }

        OPENVRML_DEFINE_JSNATIVE(createVrmlFromString)
        {
            assert(JS_GetContextPrivate(cx));
            script & s = *static_cast<script *>(JS_GetContextPrivate(cx));

            //
            // Make sure our argument is a string.
            //
            JSString * str = JS_ValueToString(cx, OPENVRML_JS_ARGV(cx, vp)[0]);
            if (!str) { return JS_FALSE; }

            try {
                std::istringstream in(JS_EncodeString(cx, str));

                assert(s.script_node().scene());
                openvrml::browser & browser = s.script_node().scene()->browser();
                const std::vector<boost::intrusive_ptr<openvrml::node> > nodes =
                    browser.create_vrml_from_stream(in);

                if (nodes.empty()) {
                    OPENVRML_JS_SET_RVAL(cx, vp, JSVAL_NULL);
                } else {
                    if (!MFNode::toJsval(nodes, cx,
                                         OPENVRML_JS_THIS_OBJECT(cx, vp),
                                         &OPENVRML_JS_RVAL(cx, vp))) {
                        return JS_FALSE;
                    }
                }
            } catch (std::exception & ex) {
                JS_ReportError(cx, ex.what());
                return JS_FALSE;
            } catch (...) {
                return JS_FALSE;
            }
            return JS_TRUE;
        }


// createVrmlFromURL( MFString url, SFNode node, SFString event )

        OPENVRML_DEFINE_JSNATIVE(createVrmlFromURL)
        {
            using std::auto_ptr;

            JSObject * url_obj = 0, * node_obj = 0;
            JSString * event_str = 0;
            if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "ooS",
                                     &url_obj, &node_obj, &event_str)) {
                return JS_FALSE;
            }

            if (!JS_InstanceOf(cx, url_obj, &MFString::jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            auto_ptr<openvrml::mfstring> url(MFString::createFromJSObject(cx,
                                                                          url_obj));
            assert(url.get());

            script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
            JSClass & sfnode_jsclass = s.sfnode_class;
            if (!JS_InstanceOf(cx, node_obj, &sfnode_jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            auto_ptr<openvrml::sfnode> node(SFNode::createFromJSObject(cx, node_obj));
            assert(node.get());
            if (!node->value()) {
                JS_ReportError(cx, "node argument is NULL");
                return JS_FALSE;
            }

            const char * const event = JS_EncodeString(cx, event_str);

            try {
                s.script_node().scene()
                    ->create_vrml_from_url(url->value(),
                                           node->value(),
                                           event ? event : "");
            } catch (const std::bad_cast &) {
                JS_ReportError(cx, "%s is not of type MFNode", event);
                return JS_FALSE;
            } catch (std::exception & ex) {
                JS_ReportError(cx, ex.what());
                return JS_FALSE;
            }

            OPENVRML_JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }

// addRoute(SFNode fromNode, String fromEventOut, SFNode toNode, String toEvent)

        OPENVRML_DEFINE_JSNATIVE(addRoute)
        {
            using std::auto_ptr;

            //
            // Make sure our first argument (fromNode) is a SFNode.
            //
            JSObject * arg0_obj;
            if (!JS_ValueToObject(cx, OPENVRML_JS_ARGV(cx, vp)[0], &arg0_obj)) {
                return JS_FALSE;
            }
            script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
            JSClass & sfnode_jsclass = s.sfnode_class;
            if (!JS_InstanceOf(cx, arg0_obj, &sfnode_jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            auto_ptr<openvrml::sfnode> fromNode =
                SFNode::createFromJSObject(cx, arg0_obj);
            //
            // Make sure the node is not null.
            //
            if (!fromNode.get()) {
                JS_ReportError(cx, "NULL destination node.");
                return JS_FALSE;
            }

            //
            // Makes sure our second argument (fromEventOut) is a string.
            //
            JSString * arg1_str =
                JS_ValueToString(cx, OPENVRML_JS_ARGV(cx, vp)[1]);
            if (!arg1_str) { return JS_FALSE; }
            const char * const fromEventOut = JS_EncodeString(cx, arg1_str);

            //
            // Make sure our third argument (toNode) is a SFNode.
            //
            JSObject * arg2_obj;
            if (!JS_ValueToObject(cx, OPENVRML_JS_ARGV(cx, vp)[2], &arg2_obj)) {
                return JS_FALSE;
            }
            if (!JS_InstanceOf(cx, arg2_obj, &sfnode_jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            auto_ptr<openvrml::sfnode> toNode =
                SFNode::createFromJSObject(cx, arg2_obj);
            //
            // Make sure the node is not null.
            //
            if (!toNode.get()) {
                JS_ReportError(cx, "NULL destination node.");
                return JS_FALSE;
            }

            //
            // Makes sure our fourth argument (toEventIn) is a string.
            //
            JSString * arg3_str =
                JS_ValueToString(cx, OPENVRML_JS_ARGV(cx, vp)[3]);
            if (!arg3_str) { return JS_FALSE; }
            const char * const toEventIn = JS_EncodeString(cx, arg3_str);

            try {
                add_route(*fromNode->value(),
                          fromEventOut,
                          *toNode->value(),
                          toEventIn);
            } catch (std::runtime_error & ex) {
                JS_ReportError(cx, ex.what());
                return JS_FALSE;
            }

            OPENVRML_JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }

// deleteRoute(SFNode fromNode, String fromEventOut, SFNode toNode, String toEvent)

        OPENVRML_DEFINE_JSNATIVE(deleteRoute)
        {
            using std::auto_ptr;

            //
            // Make sure our first argument (fromNode) is a SFNode.
            //
            JSObject * arg0_obj;
            if (!JS_ValueToObject(cx, OPENVRML_JS_ARGV(cx, vp)[0], &arg0_obj)) {
                return JS_FALSE;
            }
            script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
            JSClass & sfnode_jsclass = s.sfnode_class;
            if (!JS_InstanceOf(cx, arg0_obj, &sfnode_jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            auto_ptr<openvrml::sfnode> fromNode =
                SFNode::createFromJSObject(cx, arg0_obj);

            //
            // Makes sure our second argument (fromEventOut) is a string.
            //
            JSString * arg1_str =
                JS_ValueToString(cx, OPENVRML_JS_ARGV(cx, vp)[1]);
            if (!arg1_str) { return JS_FALSE; }
            const char * const fromEventOut = JS_EncodeString(cx, arg1_str);

            //
            // Make sure our third argument (toNode) is a SFNode.
            //
            JSObject * arg2_obj;
            if (!JS_ValueToObject(cx, OPENVRML_JS_ARGV(cx, vp)[2], &arg2_obj)) {
                return JS_FALSE;
            }
            if (!JS_InstanceOf(cx, arg2_obj, &sfnode_jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            auto_ptr<openvrml::sfnode> toNode =
                SFNode::createFromJSObject(cx, arg2_obj);

            //
            // Makes sure our fourth argument (toEventIn) is a string.
            //
            JSString * arg3_str =
                JS_ValueToString(cx, OPENVRML_JS_ARGV(cx, vp)[3]);
            if (!arg3_str) { return JS_FALSE; }
            const char * const toEventIn = JS_EncodeString(cx, arg3_str);

            delete_route(*fromNode->value(),
                         fromEventOut,
                         *toNode->value(),
                         toEventIn);

            OPENVRML_JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }

        OPENVRML_DEFINE_JSNATIVE(setDescription)
        {
            JSString * str = JS_ValueToString(cx, OPENVRML_JS_ARGV(cx, vp)[0]);
            if (!str) { return JS_FALSE; }
            assert(JS_GetContextPrivate(cx));
            script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
            openvrml::browser & browser = s.script_node().scene()->browser();
            browser.description(JS_EncodeString(cx, str));
            OPENVRML_JS_SET_RVAL(cx, vp, JSVAL_VOID);
            return JS_TRUE;
        }

    } // namespace Browser


//
// field_data
//

    field_data::field_data():
        changed(false)
    {}

    field_data::~field_data()
    {}


//
// sfield
//

    sfield::sfdata::sfdata(openvrml::field_value * const field_value):
        field_value_(field_value)
    {
        assert(field_value);
    }

    sfield::sfdata::~sfdata()
    {
        delete this->field_value_;
    }

    openvrml::field_value & sfield::sfdata::field_value() const
    {
        return *this->field_value_;
    }

    void sfield::finalize(JSContext * const cx, JSObject * const obj)
        OPENVRML_NOTHROW
    {
        delete static_cast<field_data *>(JS_GetPrivate(cx, obj));
        JS_SetPrivate(cx, obj, 0);
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfield, toString)
    {
        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));

        std::ostringstream out;
        out << sfdata.field_value();
        JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
        if (!jsstr) { return JS_FALSE; }
        OPENVRML_JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(jsstr));
        return JS_TRUE;
    }

    JSClass SFColor::jsclass = {
        "SFColor",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getProperty,         // getProperty
        setProperty,         // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSObject * SFColor::initClass(JSContext * const cx, JSObject * const obj)
    OPENVRML_NOTHROW
    {
        static JSPropertySpec properties[] =
            { { "r", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "g", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "b", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { 0, 0, 0, 0, 0 } };

        static JSFunctionSpec methods[] =
        // name, func ptr, argc
            { { "setHSV", setHSV, 3, 0 },
              { "getHSV", getHSV, 0, 0 },
              { "toString", toString, 0, 0 },
              { 0, 0, 0, 0 } };

        JSObject * const proto =
            JS_InitClass(cx, obj, 0, &jsclass,
                         construct, 0, // constructor function, min arg count
                         properties, methods,
                         0, 0); // static properties and methods
        static const double rgb[3] = {};
        if (!proto || !initObject(cx, proto, rgb)) { return 0; }
        return proto;
    }

    JSBool SFColor::toJsval(const openvrml::color & color,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const sfcolorObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!sfcolorObj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, sfcolorObj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, sfcolorObj));

        try {
            boost::polymorphic_downcast<openvrml::sfcolor *>(&sfdata.field_value())
                ->value(color);
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }

        *rval = OBJECT_TO_JSVAL(sfcolorObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::sfcolor>
    SFColor::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        using std::auto_ptr;

        assert(cx);
        if (!obj || !JS_InstanceOf(cx, obj, &SFColor::jsclass, 0)) {
            throw bad_conversion("SFColor object expected.");
        }
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));

        return auto_ptr<openvrml::sfcolor>(
            boost::polymorphic_downcast<openvrml::sfcolor *>(
                sfdata.field_value().clone().release()));
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFColor, construct)
    {
        double rgb[3] = {};
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "/ddd",
                                 &rgb[0], &rgb[1], &rgb[2])) {
            return JS_FALSE;
        }

# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, rgb);
    }

    JSBool SFColor::initObject(JSContext * const cx,
                               JSObject * const obj,
                               const double (&rgb)[3])
        OPENVRML_NOTHROW
    {
        for (size_t i = 0; i < 3; ++i) {
            if (!(rgb[i] >= 0.0 && rgb[i] <= 1.0)) {
                JS_ReportError(cx, "color component value out of range");
                return JS_FALSE;
            }
        }

        try {
            using std::auto_ptr;

            auto_ptr<openvrml::sfcolor>
                sfcolor(new openvrml::sfcolor(
                    openvrml::make_color(float(rgb[0]),
                                         float(rgb[1]),
                                         float(rgb[2]))));
            auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfcolor.get()));
            sfcolor.release();
            if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
            sfdata.release();
        } catch (std::exception & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return JS_FALSE;
        } catch (...) {
            assert(false);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    JSBool SFColor::getProperty(JSContext * const cx,
                                JSObject * const obj,
                                const jspropertyop_id id,
                                jsval * const rval)
        OPENVRML_NOTHROW
    {
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        const openvrml::sfcolor & thisColor =
            *boost::polymorphic_downcast<openvrml::sfcolor *>(
                &sfdata.field_value());

        if (jspropertyop_id_is_int(id)
            && jspropertyop_id_to_int(id) >= 0
            && jspropertyop_id_to_int(id) < 3) {
            if (!JS_NewNumberValue(cx, thisColor.value()[jspropertyop_id_to_int(id)],
                                   rval))
            {
                return JS_FALSE;
            }
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(SFColor, setProperty)
    {
        assert(JS_GetPrivate(cx, obj));
        sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        openvrml::sfcolor & thisColor =
            *boost::polymorphic_downcast<openvrml::sfcolor *>(
                &sfdata.field_value());

        if (!jspropertyop_id_is_int(id)
            || jspropertyop_id_to_int(id) < 0 || jspropertyop_id_to_int(id) > 2) {
            JS_ReportError(cx, "component index out of range");
            return JS_FALSE;
        }

        double d;
        if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

        if (!(d >= 0.0 && d <= 1.0)) {
            JS_ReportError(cx, "color component value out of range");
            return JS_FALSE;
        }

        openvrml::color val = thisColor.value();

        switch (jspropertyop_id_to_int(id)) {
        case 0:
            val.r(float(d));
            break;

        case 1:
            val.g(float(d));
            break;

        case 2:
            val.b(float(d));
            break;

        default:
            assert(false);
        }
        thisColor.value(val);
        sfdata.changed = true;
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFColor, setHSV)
    {
        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        openvrml::sfcolor & thisColor =
            *boost::polymorphic_downcast<openvrml::sfcolor *>(
                &sfdata.field_value());

        double h = 0.0, s = 0.0, v = 0.0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp),
                                 "/ddd", &h, &s, &v)) {
            return JS_FALSE;
        }

        if (!(h >= 0.0 && h <= 360.0)) {
            JS_ReportError(cx, "hue value out of range");
            return JS_FALSE;
        }
        if (!(s >= 0.0 && s <= 1.0)) {
            JS_ReportError(cx, "saturation value out of range");
            return JS_FALSE;
        }
        if (!(v >= 0.0 && v <= 1.0)) {
            JS_ReportError(cx, "value out of range");
            return JS_FALSE;
        }

        openvrml::color val = thisColor.value();
        val.hsv(float(h), float(s), float(v));
        thisColor.value(val);
        OPENVRML_JS_SET_RVAL(cx, vp, JSVAL_VOID);
        sfdata.changed = true;
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFColor, getHSV)
    {
        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const openvrml::sfcolor & thisColor =
            *boost::polymorphic_downcast<openvrml::sfcolor *>(
                &sfdata.field_value());

        float hsv[3];
        thisColor.value().hsv(hsv);

        return floatsToJSArray(3, hsv, cx, &OPENVRML_JS_RVAL(cx, vp));
    }


    JSClass SFImage::jsclass = {
        "SFImage",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getProperty,         // getProperty
        setProperty,         // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSObject * SFImage::initClass(JSContext * const cx, JSObject * const obj)
    OPENVRML_NOTHROW
    {
        //
        // The properties are read-only for now; this can be made smarter
        // once the underlying openvrml SFImage type uses an integer array
        // to represent the pixel data (rather than a char array).
        //
        static JSPropertySpec properties[] =
            { { "x", 0, JSPROP_PERMANENT | JSPROP_READONLY, 0, 0 },
              { "y", 1, JSPROP_PERMANENT | JSPROP_READONLY, 0, 0 },
              { "comp", 2, JSPROP_PERMANENT | JSPROP_READONLY, 0, 0 },
              { "array", 3, JSPROP_PERMANENT |JSPROP_READONLY, 0, 0 },
              { 0, 0, 0, 0, 0 } };

        static JSFunctionSpec methods[] =
        // name, func ptr, argc
            { { "toString", toString, 0, 0 },
              { 0, 0, 0, 0 } };

        JSObject * const proto = JS_InitClass(cx, obj, 0, &jsclass,
                                              construct, 3, // constructor function, min arg count
                                              properties, methods,
                                              0, 0); // static properties and methods
        if (!proto || !initObject(cx, proto, 0, 0, 0, 0)) { return 0; }
        return proto;
    }

    JSBool SFImage::toJsval(const openvrml::sfimage & sfimage,
                            JSContext * const cx, JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        //
        // Can't call JS_ConstructObject() here since SFImage's ctor
        // requires 4 arguments.
        //
        JSObject * const sfimageObj = JS_NewObject(cx, &jsclass, 0, obj);
        if (!sfimageObj) { return JS_FALSE; }

        try {
            std::auto_ptr<openvrml::sfimage> sfimageClone(
                boost::polymorphic_downcast<openvrml::sfimage *>(
                    sfimage.clone().release()));
            std::auto_ptr<sfield::sfdata> sfdata(
                new sfield::sfdata(sfimageClone.get()));
            sfimageClone.release();
            if (!JS_SetPrivate(cx, sfimageObj, sfdata.get())) { return JS_FALSE; }
            sfdata.release();
        } catch (std::exception & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return JS_FALSE;
        } catch (...) {
            assert(false);
            return JS_FALSE;
        }
        *rval = OBJECT_TO_JSVAL(sfimageObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::sfimage>
    SFImage::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        using std::auto_ptr;

        if (!JS_InstanceOf(cx, obj, &SFImage::jsclass, 0)) {
            throw bad_conversion("SFImage object expected.");
        }
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        return auto_ptr<openvrml::sfimage>(
            boost::polymorphic_downcast<openvrml::sfimage *>(
                sfdata.field_value().clone().release()));
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFImage, construct)
    {
        uint32_t x = 0, y = 0, comp = 0;
        JSObject * pixels = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "uuuo",
                                 &x, &y, &comp, &pixels)) {
            return JS_FALSE;
        }

        if (pixels && !JS_InstanceOf(cx, pixels, &MFInt32::jsclass,
                                     OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, x, y, comp, pixels);
    }

//
// This is ugly because the spec requires an MFInt array for the pixel data
// and because giving write access to the image size parameters can cause
// the library code to crash unless they are validated somehow...
//
    JSBool SFImage::initObject(JSContext * const cx,
                               JSObject * const obj,
                               const uint32_t x,
                               const uint32_t y,
                               const uint32_t comp,
                               JSObject * const pixels_obj)
        OPENVRML_NOTHROW
    {
        try {
            using std::auto_ptr;

            //
            // pixel data array
            //
            typedef std::vector<unsigned char> pixels_t;

            pixels_t pixels(x * y * comp, 0); // Throws std::bad_alloc.

            if (pixels_obj) {
                assert(JS_InstanceOf(cx, pixels_obj, &MFInt32::jsclass, 0));
                MField::MFData * const mfdata =
                    static_cast<MField::MFData *>(JS_GetPrivate(cx, pixels_obj));

                pixels_t::iterator pixelPtr = pixels.begin();
                for (MField::JsvalArray::size_type i(0);
                     i < mfdata->array.size() && i < x * y;
                     ++i, pixelPtr += comp) {
                    assert(JSVAL_IS_INT(mfdata->array[i]));
                    static const long byteMask[] =
                        { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
                    int32_t pixel;
                    if (!JS_ValueToInt32(cx, mfdata->array[i], &pixel)) {
                        return JS_FALSE;
                    }
                    for (size_t j(0); j < comp; ++j) {
                        *(pixelPtr + j) =
                            static_cast<unsigned char>(
                                (pixel & byteMask[j]) >> (8 * j));
                    }
                }
            }

            auto_ptr<openvrml::sfimage>
                sfimage(new openvrml::sfimage(
                            openvrml::image(x, y, comp, pixels)));
            auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfimage.get()));
            sfimage.release();
            if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
            sfdata.release();
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }

        return JS_TRUE;
    }

/**
 * @brief JavaScript SFImage getter
 *
 * @todo Return the pixel array as an MFInt32.
 */
    JSBool SFImage::getProperty(JSContext * const cx,
                                JSObject * const obj,
                                const jspropertyop_id id,
                                jsval * const vp)
        OPENVRML_NOTHROW
    {
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        const openvrml::sfimage & thisImage =
            *boost::polymorphic_downcast<openvrml::sfimage *>(
                &sfdata.field_value());

        if (jspropertyop_id_is_int(id)) {
            switch (jspropertyop_id_to_int(id)) {
            case 0:
                *vp = INT_TO_JSVAL(thisImage.value().x());
                break;
            case 1:
                *vp = INT_TO_JSVAL(thisImage.value().y());
                break;
            case 2:
                *vp = INT_TO_JSVAL(thisImage.value().comp());
                break;
            case 3: // *vp = convert pixels to MFInt...
            default: return JS_FALSE;
            }
        }
        return JS_TRUE;
    }


/**
 * @brief JavaScript SFImage getter
 *
 * @todo Implement me!
 */
    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(SFImage, setProperty)
    {
        // ...
        return JS_FALSE;
    }

    JSClass SFNode::jsclass = {
        "SFNode",            // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getProperty,         // getProperty
        openvrml_JS_StrictPropertyStub,     // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSClass SFNode::direct_output_jsclass = {
        "SFNode",            // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getProperty,         // getProperty
        setProperty,         // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSObject * SFNode::initClass(JSContext * const cx,
                                 JSObject * const obj)
    OPENVRML_NOTHROW
    {
        static JSFunctionSpec methods[] =
            { { "toString", SFNode::toString, 0, 0 },
              { 0, 0, 0, 0 } };

        script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
        JSClass & jsclass = s.sfnode_class;

        JSString * const vrmlstring = JS_NewStringCopyZ(cx, "Group {}");
        if (!vrmlstring) { return 0; }
        JSObject * const proto =
            JS_InitClass(cx, obj, 0, &jsclass,
                         SFNode::construct, 1, // constructor function, min arg count
                         0, methods, // instance properties, methods
                         0, 0); // static properties and methods
        if (!proto || !initObject(cx, proto, vrmlstring)) { return 0; }
        return proto;
    }

    JSBool SFNode::toJsval(const boost::intrusive_ptr<openvrml::node> & node,
                           JSContext * const cx,
                           JSObject * const obj,
                           jsval * const rval)
        OPENVRML_NOTHROW
    {
        script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
        JSClass & jsclass = s.sfnode_class;
        //
        // The SFNode constructor requires one arg (a vrmlstring),
        // so we can't use JS_ConstructObject here.
        //
        JSObject * const sfnodeObj = JS_NewObject(cx, &jsclass, 0, obj);
        if (!sfnodeObj) { return JS_FALSE; }

        try {
            using std::auto_ptr;

            auto_ptr<openvrml::sfnode> sfnodeClone(new openvrml::sfnode(node));
            auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfnodeClone.get()));
            sfnodeClone.release();
            if (!JS_SetPrivate(cx, sfnodeObj, sfdata.get())) { return JS_FALSE; }
            sfdata.release();
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        *rval = OBJECT_TO_JSVAL(sfnodeObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::sfnode>
    SFNode::createFromJSObject(JSContext * const cx,
                               JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        using std::auto_ptr;
        script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
        JSClass & sfnode_jsclass = s.sfnode_class;

        if (!JS_InstanceOf(cx, obj, &sfnode_jsclass, 0)) {
            throw bad_conversion("SFNode object expected.");
        }
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        return auto_ptr<openvrml::sfnode>(
            boost::polymorphic_downcast<openvrml::sfnode *>(
                sfdata.field_value().clone().release()));
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFNode, construct)
    {
        JSString * vrmlstring = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "S",
                                 &vrmlstring)) {
            return JS_FALSE;
        }

        script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
        JSClass & jsclass = s.sfnode_class;

# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, vrmlstring);
    }

    JSBool SFNode::initObject(JSContext * const cx,
                              JSObject * const obj,
                              JSString * const vrmlstring)
        OPENVRML_NOTHROW
    {
        using std::istringstream;

        assert(JS_GetContextPrivate(cx));
        script & s = *static_cast<script *>(JS_GetContextPrivate(cx));

        istringstream in(JS_EncodeString(cx, vrmlstring));

        assert(s.script_node().scene());
        openvrml::browser & browser = s.script_node().scene()->browser();
        std::vector<boost::intrusive_ptr<openvrml::node> > nodes;
        try {
            nodes = browser.create_vrml_from_stream(in);
        } catch (openvrml::invalid_vrml & ex) {
            JS_ReportError(cx, ex.what());
            return JS_FALSE;
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }

        //
        // Fail if the string does not produce exactly one node.
        //
        if (nodes.size() != 1) { return JS_FALSE; }

        try {
            using std::auto_ptr;
            auto_ptr<openvrml::sfnode> sfnode(new openvrml::sfnode(nodes[0]));
            auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfnode.get()));
            sfnode.release();
            if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
            sfdata.release();
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

//
// SFNode getProperty reads eventOut values, setProperty sends eventIns.
//
    JSBool SFNode::getProperty(JSContext * const cx,
                               JSObject * const obj,
                               const jspropertyop_id id,
                               jsval * const vp)
        OPENVRML_NOTHROW
    {
        if (!jspropertyop_id_is_string(id)) { return JS_TRUE; }

        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        const openvrml::sfnode & thisNode =
            *boost::polymorphic_downcast<openvrml::sfnode *>(
                &sfdata.field_value());

        if (!thisNode.value()) { return JS_TRUE; }

        assert(JS_GetContextPrivate(cx));
        script & s = *static_cast<script *>(JS_GetContextPrivate(cx));

        try {
            const char * eventOut =
                JS_EncodeString(cx, jspropertyop_id_to_string(cx, id));
            openvrml::event_emitter & emitter =
                thisNode.value()->event_emitter(eventOut);
            *vp = s.vrmlFieldToJSVal(emitter.value());
        } catch (openvrml::unsupported_interface &) {}

        return JS_TRUE;
    }

// This should only happen if directOutput is set...

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(SFNode, setProperty)
    {
        if (jspropertyop_id_is_string(id)) {
            using std::auto_ptr;
            using std::string;

            assert(JS_GetPrivate(cx, obj));
            sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
            openvrml::sfnode & thisNode =
                *boost::polymorphic_downcast<openvrml::sfnode *>(
                    &sfdata.field_value());

            if (!thisNode.value()) { return JS_TRUE; }

            boost::intrusive_ptr<openvrml::node> nodePtr = thisNode.value();

            const char * const eventInId =
                JS_EncodeString(cx, jspropertyop_id_to_string(cx, id));

            try {
                using boost::shared_ptr;

                //
                // Get the event_listener.
                //
                openvrml::event_listener & listener =
                    nodePtr->event_listener(eventInId);

                // convert vp to field, send eventIn to node
                openvrml::field_value::type_id expectType = listener.type();
                auto_ptr<openvrml::field_value> fieldValue;
                fieldValue = createFieldValueFromJsval(cx, *vp, expectType);

                assert(JS_GetContextPrivate(cx));
                script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
                s.direct_output(listener,
                                shared_ptr<openvrml::field_value>(fieldValue));
            } catch (openvrml::unsupported_interface &) {
                // We can't handle the property here, so get out of the way.
                return JS_TRUE;
            } catch (bad_conversion & ex) {
                JS_ReportError(cx, ex.what());
                return JS_FALSE;
            } catch (std::bad_alloc &) {
                JS_ReportOutOfMemory(cx);
                return JS_FALSE;
            }
        }
        return JS_TRUE;
    }


    JSClass SFRotation::jsclass = {
        "SFRotation",        // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getProperty,         // getProperty
        setProperty,         // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSObject * SFRotation::initClass(JSContext * const cx,
                                     JSObject * const obj)
    OPENVRML_NOTHROW
    {
        static JSPropertySpec properties[] =
            { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "z", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "angle", 3, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { 0, 0, 0, 0, 0 } };

        static JSFunctionSpec methods[] =
        // name, func ptr, argc
            { { "getAxis", getAxis, 0, 0 },
              { "inverse", inverse, 0, 0 },
              { "multiply", multiply, 1, 0 },
              { "multVec", multVec, 1, 0 },
              { "setAxis", setAxis, 1, 0 },
              { "slerp", slerp, 2, 0 },
              { "toString", toString, 0, 0 },
              { 0, 0, 0, 0 } };

        JSObject * const proto =
            JS_InitClass(cx, obj, 0, &jsclass,
                         construct, 0, // constructor function, min arg count
                         properties, methods, // instance properties, methods
                         0, 0); // static properties and methods
        static const double rot[4] = { 0.0, 1.0, 0.0, 0.0 };
        if (!proto || !initObject(cx, proto, rot)) { return 0; }
        return proto;
    }

    JSBool SFRotation::toJsval(const openvrml::rotation & rotation,
                               JSContext * const cx,
                               JSObject * const obj,
                               jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const sfrotationObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!sfrotationObj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, sfrotationObj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, sfrotationObj));

        try {
            boost::polymorphic_downcast<openvrml::sfrotation *>(
                &sfdata.field_value())->value(rotation);
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }

        *rval = OBJECT_TO_JSVAL(sfrotationObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::sfrotation>
    SFRotation::createFromJSObject(JSContext * const cx,
                                   JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        using std::auto_ptr;

        if (!JS_InstanceOf(cx, obj, &SFRotation::jsclass, 0)) {
            throw bad_conversion("SFRotation object expected.");
        }
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        return auto_ptr<openvrml::sfrotation>(
            boost::polymorphic_downcast<openvrml::sfrotation *>(
                sfdata.field_value().clone().release()));
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFRotation, construct)
    {
        double rot[4] = { 0.0, 1.0, 0.0, 0.0 };
        if (argc > 1 && JSVAL_IS_OBJECT(OPENVRML_JS_ARGV(cx, vp)[0])
            && JSVAL_IS_NUMBER(OPENVRML_JS_ARGV(cx, vp)[1])) {
            JSObject * axis_obj = 0;
            if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "od",
                                     &axis_obj, &rot[3])) {
                return JS_FALSE;
            }
            if (axis_obj
                && !JS_InstanceOf(cx, axis_obj, &SFVec3f::jsclass,
                                  OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            assert(JS_GetPrivate(cx, axis_obj));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, axis_obj));
            const openvrml::sfvec3f & axis =
                *boost::polymorphic_downcast<openvrml::sfvec3f *>(
                    &sfdata.field_value());
            rot[0] = axis.value().x();
            rot[1] = axis.value().y();
            rot[2] = axis.value().z();
        } else if (argc > 1 && JSVAL_IS_OBJECT(OPENVRML_JS_ARGV(cx, vp)[0])
                   && JSVAL_IS_OBJECT(OPENVRML_JS_ARGV(cx, vp)[1])) {
            JSObject * from_obj = 0, * to_obj = 0;
            if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "oo",
                                     &from_obj, &to_obj)) {
                return JS_FALSE;
            }
            if ((from_obj
                 && !JS_InstanceOf(cx, from_obj, &SFVec3f::jsclass,
                                   OPENVRML_JS_ARGV(cx, vp)))
                || (to_obj
                    && !JS_InstanceOf(cx, to_obj, &SFVec3f::jsclass,
                                      OPENVRML_JS_ARGV(cx, vp)))) {
                return JS_FALSE;
            }

            assert(JS_GetPrivate(cx, from_obj));
            const sfield::sfdata & from_sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, from_obj));
            const openvrml::sfvec3f & from_vec =
                *boost::polymorphic_downcast<openvrml::sfvec3f *>(
                    &from_sfdata.field_value());

            assert(JS_GetPrivate(cx, to_obj));
            const sfield::sfdata & to_sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, to_obj));
            const openvrml::sfvec3f & to_vec =
                *boost::polymorphic_downcast<openvrml::sfvec3f *>(
                    &to_sfdata.field_value());

            const openvrml::vec3f axis =
                (from_vec.value() * to_vec.value()).normalize();
            rot[0] = axis.x();
            rot[1] = axis.y();
            rot[2] = axis.z();
            rot[3] = acos(from_vec.value().dot(to_vec.value())
                          / (from_vec.value().length() * to_vec.value().length()));
        } else if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp),
                                        "/dddd",
                                        &rot[0], &rot[1], &rot[2], &rot[3])) {
            return JS_FALSE;
        }

# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, rot);
    }

    OPENVRML_JAVASCRIPT_LOCAL bool normalized(const openvrml::vec3f & v)
    {
        return openvrml::local::fequal(v.length(), 1.0f);
    }

    JSBool SFRotation::initObject(JSContext * const cx,
                                  JSObject * const obj,
                                  const double (&rot)[4])
        OPENVRML_NOTHROW
    {
        const openvrml::vec3f axis = openvrml::make_vec3f(float(rot[0]),
                                                          float(rot[1]),
                                                          float(rot[2]));
        if (!normalized(axis)) {
            JS_ReportError(cx, "axis component of rotation is not normalized");
        }

        try {
            using std::auto_ptr;
            auto_ptr<openvrml::sfrotation> sfrotation(
                new openvrml::sfrotation(
                    openvrml::make_rotation(float(rot[0]),
                                            float(rot[1]),
                                            float(rot[2]),
                                            float(rot[3]))));
            auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfrotation.get()));
            sfrotation.release();
            if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
            sfdata.release();
        } catch (std::exception & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return JS_FALSE;
        } catch (...) {
            assert(false);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    JSBool SFRotation::getProperty(JSContext * const cx,
                                   JSObject * const obj,
                                   const jspropertyop_id id,
                                   jsval * const rval)
        OPENVRML_NOTHROW
    {
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        const openvrml::sfrotation & thisRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &sfdata.field_value());

        if (jspropertyop_id_is_int(id)
            && jspropertyop_id_to_int(id) >= 0 && jspropertyop_id_to_int(id) < 4) {
            if (!JS_NewNumberValue(cx, thisRot.value()[jspropertyop_id_to_int(id)], rval))
            {
                return JS_FALSE;
            }
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(SFRotation, setProperty)
    {
        assert(JS_GetPrivate(cx, obj));
        sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        openvrml::sfrotation & thisRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &sfdata.field_value());

        if (jspropertyop_id_is_int(id)
            && jspropertyop_id_to_int(id) >= 0 && jspropertyop_id_to_int(id) < 4) {

            double d;
            if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

            openvrml::vec3f axis = thisRot.value().axis();
            float angle = thisRot.value().angle();
            switch (jspropertyop_id_to_int(id)) {
            case 0:
                axis.x(float(d));
                break;
            case 1:
                axis.y(float(d));
                break;
            case 2:
                axis.z(float(d));
                break;
            case 3:
                angle = float(d);
                break;
            default:
                assert(false);
            }

            if (!normalized(axis)) {
                JS_ReportError(cx, "axis component of rotation is not normalized");
                return JS_FALSE;
            }

            thisRot.value(make_rotation(axis, angle));
            sfdata.changed = true;
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFRotation, getAxis)
    {
        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const openvrml::sfrotation & thisRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &obj_sfdata.field_value());

        JSObject * const robj =
            js_construct_object(cx, &SFVec3f::jsclass, 0,
                               OPENVRML_JS_THIS_OBJECT(cx, vp));
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        const sfield::sfdata & robj_sfdata =
            *static_cast<sfdata *>(JS_GetPrivate(cx, robj));
        openvrml::sfvec3f & resultVec =
            *boost::polymorphic_downcast<openvrml::sfvec3f *>(
                &robj_sfdata.field_value());

        resultVec.value(thisRot.value().axis());
        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFRotation, inverse)
    {
        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const openvrml::sfrotation & thisRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &obj_sfdata.field_value());

        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFRotation::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        const sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        openvrml::sfrotation & resultRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &robj_sfdata.field_value());

        resultRot.value(thisRot.value().inverse());
        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFRotation, multiply)
    {
        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const openvrml::sfrotation & thisRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &obj_sfdata.field_value());

        JSObject * rot_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &rot_obj)) {
            return JS_FALSE;
        }
        if (rot_obj && !JS_InstanceOf(cx, rot_obj, &SFRotation::jsclass,
                                      OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, rot_obj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfdata *>(JS_GetPrivate(cx, rot_obj));
        const openvrml::sfrotation & argRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &arg_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFRotation::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        const sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        openvrml::sfrotation & resultRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &robj_sfdata.field_value());

        resultRot.value(thisRot.value() * argRot.value());
        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFRotation, multVec)
    {
        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const openvrml::sfrotation & thisRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &obj_sfdata.field_value());

        JSObject * vec_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &vec_obj)) {
            return JS_FALSE;
        }
        if (vec_obj && !JS_InstanceOf(cx, vec_obj, &SFVec3f::jsclass,
                                      OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, vec_obj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfdata *>(JS_GetPrivate(cx, vec_obj));
        const openvrml::sfvec3f & argVec =
            *boost::polymorphic_downcast<openvrml::sfvec3f *>(
                &arg_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec3f::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfdata *>(JS_GetPrivate(cx, robj));
        openvrml::sfvec3f & resultVec =
            *boost::polymorphic_downcast<openvrml::sfvec3f *>(
                &robj_sfdata.field_value());

        resultVec.value(argVec.value()
                        * openvrml::make_rotation_mat4f(thisRot.value()));

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFRotation, setAxis)
    {
        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        openvrml::sfrotation & thisRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &obj_sfdata.field_value());

        JSObject * vec_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &vec_obj)) {
            return JS_FALSE;
        }
        if (vec_obj && !JS_InstanceOf(cx, vec_obj, &SFVec3f::jsclass,
                                      OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, vec_obj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, vec_obj));
        const openvrml::sfvec3f & argVec =
            *boost::polymorphic_downcast<openvrml::sfvec3f *>(
                &arg_sfdata.field_value());

        if (!normalized(argVec.value())) {
            JS_ReportError(cx, "axis component of rotation is not normalized");
            return JS_FALSE;
        }

        openvrml::rotation temp = thisRot.value();
        temp.axis(argVec.value());
        thisRot.value(temp);
        obj_sfdata.changed = true;
        OPENVRML_JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(SFRotation, slerp)
    {
        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const openvrml::sfrotation & thisRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &obj_sfdata.field_value());

        JSObject * dest_obj = 0;
        double t = 0.0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "od",
                                 &dest_obj, &t)) {
            return JS_FALSE;
        }
        if (dest_obj && !JS_InstanceOf(cx, dest_obj, &SFRotation::jsclass,
                                       OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, dest_obj));
        const sfield::sfdata & dest_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, dest_obj));
        const openvrml::sfrotation & dest =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &dest_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFRotation::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        openvrml::sfrotation & resultRot =
            *boost::polymorphic_downcast<openvrml::sfrotation *>(
                &robj_sfdata.field_value());

        resultRot.value(thisRot.value().slerp(dest.value(), float(t)));

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }


    JSClass SFVec2f::jsclass = {
        "SFVec2f",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getProperty,         // getProperty
        setProperty,         // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    template <typename SFVec2>
    JSObject * sfvec2_jsobject<SFVec2>::initClass(JSContext * const cx,
                                                  JSObject * const obj)
        OPENVRML_NOTHROW
    {
        static JSPropertySpec properties[] =
            { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { 0, 0, 0, 0, 0 } };

        static JSFunctionSpec methods[] =
        /*    name          native          nargs    */
            { { "add", add, 1, 0 },
              { "divide", divide, 1, 0 },
              { "dot", dot, 1, 0 },
              { "length", length, 0, 0 },
              { "multiply", multiply, 1, 0 },
              { "negate", negate, 0, 0 },
              { "normalize", normalize, 0, 0 },
              { "subtract", subtract, 1, 0 },
              { "toString", toString, 0, 0 },
              { 0, 0, 0, 0 } };

        JSObject * const proto =
            JS_InitClass(cx, obj, 0, &SFVec2::jsclass,
                         constructor, 0, // constructor function, min arg count
                         properties, methods,
                         0, 0); // static properties and methods
        const double vec[2] = {};
        if (!proto || !initObject(cx, proto, vec)) { return 0; }
        return proto;
    }

    JSBool SFVec2f::toJsval(const value_type & vec2f,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const sfvec2Obj = js_construct_object(cx, &jsclass, 0, obj);
        if (!sfvec2Obj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, sfvec2Obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, sfvec2Obj));

        try {
            boost::polymorphic_downcast<field_type *>(&sfdata.field_value())
                ->value(vec2f);
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }

        *rval = OBJECT_TO_JSVAL(sfvec2Obj);
        return JS_TRUE;
    }

    std::auto_ptr<SFVec2f::field_type>
    SFVec2f::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        using std::auto_ptr;

        if (!JS_InstanceOf(cx, obj, &jsclass, 0)) {
            throw bad_conversion("SFVec2f object expected.");
        }
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        return auto_ptr<field_type>(boost::polymorphic_downcast<field_type *>(
                                        sfdata.field_value().clone().release()));
    }

    template <typename SFVec2>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec2_jsobject<SFVec2>, constructor)
    {
        double vec[2] = {};
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "/dd",
                                 &vec[0], &vec[1])) {
            return JS_FALSE;
        }

# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &SFVec2::jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, vec);
    }

    template <typename SFVec2>
    JSBool sfvec2_jsobject<SFVec2>::initObject(JSContext * const cx,
                                               JSObject * const obj,
                                               const double (&vec)[2])
        OPENVRML_NOTHROW
    {
        for (size_t i = 0; i < 2; ++i) {
            if (vec[i] != vec[i]) {
                JS_ReportError(cx, "argument %d of constructor is NaN",
                               i + 1);
                return JS_FALSE;
            }
        }

        try {
            using std::auto_ptr;

            typedef typename SFVec2::field_type sfvec2_t;
            typedef typename SFVec2::value_type vec2_t;

            const vec2_t vec2 = { { static_cast<float>(vec[0]),
                                    static_cast<float>(vec[1]) } };
            auto_ptr<sfvec2_t> sfvec2(new sfvec2_t(vec2));
            auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfvec2.get()));
            sfvec2.release();
            if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
            sfdata.release();
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }

        return JS_TRUE;
    }

    template <typename SFVec2>
    JSBool sfvec2_jsobject<SFVec2>::getProperty(JSContext * const cx,
                                                JSObject * const obj,
                                                const jspropertyop_id id,
                                                jsval * const rval)
        OPENVRML_NOTHROW
    {
        typedef typename SFVec2::field_type sfvec2_t;

        if (jspropertyop_id_is_int(id)
            && jspropertyop_id_to_int(id) >= 0 && jspropertyop_id_to_int(id) < 2) {
            assert(JS_GetPrivate(cx, obj));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
            const sfvec2_t & thisVec =
                *boost::polymorphic_downcast<sfvec2_t *>(&sfdata.field_value());

            if (!JS_NewNumberValue(cx, thisVec.value()[jspropertyop_id_to_int(id)],
                                   rval)) {
                return JS_FALSE;
            }
        }
        return JS_TRUE;
    }

    template <typename SFVec2>
    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(sfvec2_jsobject<SFVec2>,
                                              setProperty)
    {
        typedef typename SFVec2::field_type sfvec2_t;
        typedef typename SFVec2::value_type vec2_t;

        if (jspropertyop_id_is_int(id)
            && jspropertyop_id_to_int(id) >= 0 && jspropertyop_id_to_int(id) < 2) {
            assert(JS_GetPrivate(cx, obj));
            sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
            sfvec2_t & thisVec =
                *boost::polymorphic_downcast<sfvec2_t *>(&sfdata.field_value());

            double d;
            if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }
            if (d != d) {
                JS_ReportError(cx, "cannot set SFVec2f component to NaN");
                return JS_FALSE;
            }

            vec2_t temp = thisVec.value();
            switch (jspropertyop_id_to_int(id)) {
            case 0:
                temp.x(d);
                break;
            case 1:
                temp.y(d);
                break;
            default:
                assert(false);
            }
            thisVec.value(temp);
            sfdata.changed = true;
        }
        return JS_TRUE;
    }

    template <typename SFVec2>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec2_jsobject<SFVec2>, add)
    {
        typedef typename SFVec2::field_type sfvec2_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec2_t & thisVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&obj_sfdata.field_value());

        //
        // Make sure our argument is an SFVec2f.
        //
        JSObject * arg_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &arg_obj)) {
            return JS_FALSE;
        }
        assert(arg_obj);
        if (!JS_InstanceOf(cx, arg_obj, &SFVec2::jsclass,
                           OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, arg_obj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
        const sfvec2_t & argVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&arg_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec2::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec2_t & resultVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&robj_sfdata.field_value());

        resultVec.value(thisVec.value() + argVec.value());

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    template <typename SFVec2>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec2_jsobject<SFVec2>, divide)
    {
        typedef typename SFVec2::field_type sfvec2_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec2_t & thisVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&obj_sfdata.field_value());

        //
        // Make sure our argument is a number.
        //
        double divisor = 0.0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "d",
                                 &divisor)) {
            return JS_FALSE;
        }

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec2::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec2_t & resultVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&robj_sfdata.field_value());

        resultVec.value(thisVec.value() / divisor);

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    template <typename SFVec2>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec2_jsobject<SFVec2>, dot)
    {
        typedef typename SFVec2::field_type sfvec2_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec2_t & thisVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&sfdata.field_value());

        //
        // Make sure our argument is an SFVec2.
        //
        JSObject * arg_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &arg_obj)) {
            return JS_FALSE;
        }
        assert(arg_obj);
        if (!JS_InstanceOf(cx, arg_obj, &SFVec2::jsclass,
                           OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, arg_obj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
        const sfvec2_t & argVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&arg_sfdata.field_value());

        const double result = thisVec.value().dot(argVec.value());
        jsval result_val;
        const JSBool retval = JS_NewNumberValue(cx, result, &result_val);
        OPENVRML_JS_SET_RVAL(cx, vp, result_val);
        return retval;
    }

    template <typename SFVec2>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec2_jsobject<SFVec2>, length)
    {
        typedef typename SFVec2::field_type sfvec2_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec2_t & thisVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&sfdata.field_value());

        jsval result_val;
        const JSBool retval =
            JS_NewNumberValue(cx, thisVec.value().length(), &result_val);
        OPENVRML_JS_SET_RVAL(cx, vp, result_val);
        return retval;
    }

    template <typename SFVec2>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec2_jsobject<SFVec2>, multiply)
    {
        typedef typename SFVec2::field_type sfvec2_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec2_t & thisVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&obj_sfdata.field_value());

        //
        // Make sure our argument is a number.
        //
        double factor = 0.0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "d",
                                 &factor)) {
            return JS_FALSE;
        }

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec2::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec2_t & resultVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&robj_sfdata.field_value());

        resultVec.value(thisVec.value() * factor);

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    template <typename SFVec2>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec2_jsobject<SFVec2>, negate)
    {
        typedef typename SFVec2::field_type sfvec2_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec2_t & thisVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&obj_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec2::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec2_t & resultVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&robj_sfdata.field_value());

        resultVec.value(-thisVec.value());

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    template <typename SFVec2>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec2_jsobject<SFVec2>, normalize)
    {
        typedef typename SFVec2::field_type sfvec2_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec2_t & thisVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&obj_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec2::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec2_t & resultVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&robj_sfdata.field_value());

        resultVec.value(thisVec.value().normalize());

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    template <typename SFVec2>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec2_jsobject<SFVec2>, subtract)
    {
        typedef typename SFVec2::field_type sfvec2_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec2_t & thisVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&obj_sfdata.field_value());

        //
        // Make sure our argument is an SFVec2f.
        //
        JSObject * arg_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &arg_obj)) {
            return JS_FALSE;
        }
        assert(arg_obj);
        if (!JS_InstanceOf(cx, arg_obj, &SFVec2::jsclass,
                           OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, arg_obj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
        const sfvec2_t & argVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&arg_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec2::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec2_t & resultVec =
            *boost::polymorphic_downcast<sfvec2_t *>(&robj_sfdata.field_value());

        resultVec.value(thisVec.value() - argVec.value());

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    JSClass SFVec2d::jsclass = {
        "SFVec2d",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getProperty,         // getProperty
        setProperty,         // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSBool SFVec2d::toJsval(const openvrml::vec2d & vec2d,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const sfvec2dObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!sfvec2dObj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, sfvec2dObj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, sfvec2dObj));

        try {
            boost::polymorphic_downcast<openvrml::sfvec2d *>(
                &sfdata.field_value())->value(vec2d);
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }

        *rval = OBJECT_TO_JSVAL(sfvec2dObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::sfvec2d>
    SFVec2d::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        using std::auto_ptr;

        if (!JS_InstanceOf(cx, obj, &SFVec2d::jsclass, 0)) {
            throw bad_conversion("SFVec2d object expected");
        }
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        return auto_ptr<openvrml::sfvec2d>(
            boost::polymorphic_downcast<openvrml::sfvec2d *>(
                sfdata.field_value().clone().release()));
    }


    JSClass SFVec3f::jsclass = {
        "SFVec3f",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getProperty,         // getProperty
        setProperty,         // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    template <typename SFVec3>
    JSObject * sfvec3_jsobject<SFVec3>::initClass(JSContext * const cx,
                                                  JSObject * const obj)
        OPENVRML_NOTHROW
    {
        static JSPropertySpec properties[] =
            { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { "z", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
              { 0, 0, 0, 0, 0 } };

        static JSFunctionSpec methods[] =
        /*    name          native          nargs    */
            { { "add", add, 1, 0 },
              { "cross", cross, 1, 0 },
              { "divide", divide, 1, 0 },
              { "dot", dot, 1, 0 },
              { "length", length, 0, 0 },
              { "multiply", multiply, 1, 0 },
              { "negate", negate, 0, 0 },
              { "normalize", normalize, 0, 0 },
              { "subtract", subtract, 1, 0 },
              { "toString", toString, 0, 0 },
              { 0, 0, 0, 0 } };

        JSObject * const proto =
            JS_InitClass(cx, obj, 0, &SFVec3::jsclass,
                         constructor, 0, // constructor function, min arg count
                         properties, methods,
                         0, 0); // static properties and methods
        const double vec[3] = {};
        if (!proto || !initObject(cx, proto, vec)) { return 0; }
        return proto;
    }

    JSBool SFVec3f::toJsval(const value_type & vec3f,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const sfvec3fObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!sfvec3fObj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, sfvec3fObj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, sfvec3fObj));

        try {
            boost::polymorphic_downcast<openvrml::sfvec3f *>(
                &sfdata.field_value())->value(vec3f);
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }

        *rval = OBJECT_TO_JSVAL(sfvec3fObj);
        return JS_TRUE;
    }

    std::auto_ptr<SFVec3f::field_type>
    SFVec3f::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        using std::auto_ptr;

        if (!JS_InstanceOf(cx, obj, &SFVec3f::jsclass, 0)) {
            throw bad_conversion("SFVec3f object expected.");
        }
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        return auto_ptr<openvrml::sfvec3f>(
            boost::polymorphic_downcast<openvrml::sfvec3f *>(
                sfdata.field_value().clone().release()));
    }

    template <typename SFVec3>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec3_jsobject<SFVec3>, constructor)
    {
        double vec[3] = {};
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "/ddd",
                                 &vec[0], &vec[1], &vec[2])) {
            return JS_FALSE;
        }

# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &SFVec3::jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, vec);
    }

    template <typename SFVec3>
    JSBool sfvec3_jsobject<SFVec3>::initObject(JSContext * const cx,
                                               JSObject * obj,
                                               const double (&vec)[3])
        OPENVRML_NOTHROW
    {
        typedef typename SFVec3::field_type sfvec3_t;
        typedef typename SFVec3::value_type vec3_t;

        for (unsigned i = 0; i < 3; ++i) {
            if (vec[i] != vec[i]) {
                JS_ReportError(cx, "argument %d of constructor is NaN",
                               i + 1);
                return JS_FALSE;
            }
        }

        try {
            using std::auto_ptr;

            const vec3_t vec3 = { { static_cast<float>(vec[0]),
                                    static_cast<float>(vec[1]),
                                    static_cast<float>(vec[2]) } };
            auto_ptr<sfvec3_t> sfvec3(new sfvec3_t(vec3));
            auto_ptr<sfield::sfdata> sfdata(new sfield::sfdata(sfvec3.get()));
            sfvec3.release();
            if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
            sfdata.release();
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    template <typename SFVec3>
    JSBool sfvec3_jsobject<SFVec3>::getProperty(JSContext * const cx,
                                                JSObject * const obj,
                                                const jspropertyop_id id,
                                                jsval * const vp)
        OPENVRML_NOTHROW
    {
        typedef typename SFVec3::field_type sfvec3_t;

        if (jspropertyop_id_is_int(id)
            && jspropertyop_id_to_int(id) >= 0 && jspropertyop_id_to_int(id) < 3) {
            assert(JS_GetPrivate(cx, obj));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
            const sfvec3_t & thisVec =
                *boost::polymorphic_downcast<sfvec3_t *>(&sfdata.field_value());

            if (!JS_NewNumberValue(cx, thisVec.value()[jspropertyop_id_to_int(id)],
                                   vp)) {
                return JS_FALSE;
            }
        }
        return JS_TRUE;
    }

    template <typename SFVec3>
    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(sfvec3_jsobject<SFVec3>,
                                              setProperty)
    {
        typedef typename SFVec3::field_type sfvec3_t;
        typedef typename SFVec3::value_type vec3_t;

        if (jspropertyop_id_is_int(id)
            && jspropertyop_id_to_int(id) >= 0 && jspropertyop_id_to_int(id) < 3) {
            assert(JS_GetPrivate(cx, obj));
            sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
            sfvec3_t & thisVec =
                *boost::polymorphic_downcast<sfvec3_t *>(&sfdata.field_value());

            double d;
            if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }
            if (d != d) {
                JS_ReportError(cx, "cannot set component to NaN");
                return JS_FALSE;
            }

            vec3_t temp = thisVec.value();
            switch (jspropertyop_id_to_int(id)) {
            case 0:
                temp.x(d);
                break;
            case 1:
                temp.y(d);
                break;
            case 2:
                temp.z(d);
                break;
            default:
                assert(false);
            }
            thisVec.value(temp);
            sfdata.changed = true;
        }
        return JS_TRUE;
    }

    template <typename SFVec3>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec3_jsobject<SFVec3>, add)
    {
        typedef typename SFVec3::field_type sfvec3_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec3_t & thisVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&obj_sfdata.field_value());

        //
        // Make sure our argument is an SFVec3f.
        //
        JSObject * arg_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &arg_obj)) {
            return JS_FALSE;
        }
        assert(arg_obj);
        if (!JS_InstanceOf(cx, arg_obj, &SFVec3::jsclass,
                           OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, arg_obj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
        const sfvec3_t & argVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&arg_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec3::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec3_t & resultVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&robj_sfdata.field_value());

        resultVec.value(thisVec.value() + argVec.value());

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    template <typename SFVec3>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec3_jsobject<SFVec3>, cross)
    {
        typedef typename SFVec3::field_type sfvec3_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec3_t & thisVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&obj_sfdata.field_value());

        //
        // Make sure our argument is an SFVec3f.
        //
        JSObject * arg_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &arg_obj)) {
            return JS_FALSE;
        }
        assert(arg_obj);
        if (!JS_InstanceOf(cx, arg_obj, &SFVec3::jsclass,
                           OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, arg_obj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
        const sfvec3_t & argVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&arg_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec3::jsclass, proto, parent);
        if (!robj) {return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfdata *>(JS_GetPrivate(cx, robj));
        sfvec3_t & resultVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&robj_sfdata.field_value());

        resultVec.value(thisVec.value() * argVec.value());

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    template <typename SFVec3>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec3_jsobject<SFVec3>, divide)
    {
        typedef typename SFVec3::field_type sfvec3_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec3_t & thisVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&obj_sfdata.field_value());

        //
        // Make sure our argument is a number.
        //
        double divisor = 0.0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "d",
                                 &divisor)) {
            return JS_FALSE;
        }

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec3::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec3_t & resultVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&robj_sfdata.field_value());

        resultVec.value(thisVec.value() / divisor);

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    template <typename SFVec3>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec3_jsobject<SFVec3>, dot)
    {
        typedef typename SFVec3::field_type sfvec3_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec3_t & thisVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&obj_sfdata.field_value());

        //
        // Make sure our argument is an SFVec3[df].
        //
        JSObject * arg_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &arg_obj)) {
            return JS_FALSE;
        }
        assert(arg_obj);
        if (!JS_InstanceOf(cx, arg_obj, &SFVec3::jsclass,
                           OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, arg_obj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
        const sfvec3_t & argVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&arg_sfdata.field_value());

        const double result = thisVec.value().dot(argVec.value());
        jsval result_val;
        const JSBool retval = JS_NewNumberValue(cx, result, &result_val);
        OPENVRML_JS_SET_RVAL(cx, vp, result_val);
        return retval;
    }

    template <typename SFVec3>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec3_jsobject<SFVec3>, length)
    {
        typedef typename SFVec3::field_type sfvec3_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec3_t & thisVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&sfdata.field_value());

        jsval result_val;
        const JSBool retval =
            JS_NewNumberValue(cx, thisVec.value().length(), &result_val);
        OPENVRML_JS_SET_RVAL(cx, vp, result_val);
        return retval;
    }

    template <typename SFVec3>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec3_jsobject<SFVec3>, multiply)
    {
        typedef typename SFVec3::field_type sfvec3_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec3_t & thisVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&obj_sfdata.field_value());

        //
        // Make sure our argument is a number.
        //
        double factor = 0.0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "d",
                                 &factor)) {
            return JS_FALSE;
        }

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec3::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec3_t & resultVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&arg_sfdata.field_value());

        resultVec.value(thisVec.value() * factor);

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    template <typename SFVec3>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec3_jsobject<SFVec3>, negate)
    {
        typedef typename SFVec3::field_type sfvec3_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec3_t & thisVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&obj_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec3::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        const sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec3_t & resultVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&robj_sfdata.field_value());

        resultVec.value(-thisVec.value());

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    template <typename SFVec3>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec3_jsobject<SFVec3>, normalize)
    {
        typedef typename SFVec3::field_type sfvec3_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec3_t & thisVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&obj_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec3::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec3_t & resultVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&robj_sfdata.field_value());

        resultVec.value(thisVec.value().normalize());

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    template <typename SFVec3>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(sfvec3_jsobject<SFVec3>, subtract)
    {
        typedef typename SFVec3::field_type sfvec3_t;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfield::sfdata & obj_sfdata =
            *static_cast<sfield::sfdata *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const sfvec3_t & thisVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&obj_sfdata.field_value());

        //
        // Make sure our argument is an SFVec3f.
        //
        JSObject * arg_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &arg_obj)) {
            return JS_FALSE;
        }
        assert(arg_obj);
        if (!JS_InstanceOf(cx, arg_obj, &SFVec3f::jsclass,
                           OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, arg_obj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
        const sfvec3_t & argVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&arg_sfdata.field_value());

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec3::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        sfvec3_t & resultVec =
            *boost::polymorphic_downcast<sfvec3_t *>(&robj_sfdata.field_value());

        resultVec.value(thisVec.value() - argVec.value());

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    JSClass SFVec3d::jsclass = {
        "SFVec3d",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getProperty,         // getProperty
        setProperty,         // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };
    JSBool SFVec3d::toJsval(const value_type & vec3d,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const sfvec3dObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!sfvec3dObj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, sfvec3dObj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, sfvec3dObj));

        try {
            boost::polymorphic_downcast<openvrml::sfvec3d *>(
                &sfdata.field_value())->value(vec3d);
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }

        *rval = OBJECT_TO_JSVAL(sfvec3dObj);
        return JS_TRUE;
    }

    std::auto_ptr<SFVec3d::field_type>
    SFVec3d::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        using std::auto_ptr;

        if (!JS_InstanceOf(cx, obj, &SFVec3d::jsclass, 0)) {
            throw bad_conversion("SFVec3d object expected");
        }
        assert(JS_GetPrivate(cx, obj));
        const sfield::sfdata & sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, obj));
        return auto_ptr<openvrml::sfvec3d>(
            boost::polymorphic_downcast<openvrml::sfvec3d *>(
                sfdata.field_value().clone().release()));
    }

    MField::MFData::MFData(JsvalArray::size_type size):
        array(size)
    {}

    MField::MFData::~MFData()
    {}

    void MField::AddRoots(JSContext * const cx, JsvalArray & jsvalArray)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using std::bad_alloc;

        size_t i;
        try {
            for (i = 0; i < jsvalArray.size(); ++i) {
                if (!add_value_root(cx, &jsvalArray[i])) { throw bad_alloc(); }
            }
        } catch (bad_alloc &) {
            for (size_t j = 0; j < i - 1; ++j) {
                remove_value_root(cx, &jsvalArray[j]);
            }
            throw;
        }
    }

    void MField::RemoveRoots(JSContext * const cx, JsvalArray & jsvalArray)
        OPENVRML_NOTHROW
    {
        for (size_t i = 0; i < jsvalArray.size(); ++i) {
            remove_value_root(cx, &jsvalArray[i]);
        }
    }

    JSBool MField::getElement(JSContext * const cx,
                              JSObject * const obj,
                              const jspropertyop_id id,
                              jsval * const vp)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);
        assert(vp);
        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        if (jspropertyop_id_is_int(id)
            && jspropertyop_id_to_int(id) >= 0
            && size_t(jspropertyop_id_to_int(id)) < mfdata->array.size()) {
            *vp = mfdata->array[jspropertyop_id_to_int(id)];
        }
        return JS_TRUE;
    }

    JSBool MField::getLength(JSContext * const cx,
                             JSObject * const obj,
                             jspropertyop_id,
                             jsval * const vp)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);
        assert(vp);
        const MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        *vp = INT_TO_JSVAL(mfdata->array.size());
        return JS_TRUE;
    }


    template <typename Subclass>
    JSObject * MFJSObject<Subclass>::initClass(JSContext * const cx,
                                               JSObject * const obj)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);

        static JSPropertySpec properties[] =
            { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
              { 0, 0, 0, 0, 0 } };

        static JSFunctionSpec methods[] =
            { { "toString", toString, 0, 0 },
              { 0, 0, 0, 0 } };

        JSObject * proto = JS_InitClass(cx, obj, 0, &Subclass::jsclass,
                                        construct, 0, // constructor function, min arg count
                                        properties, methods,
                                        0, 0); // static properties and methods
        if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
        return proto;
    }

    template <typename Subclass>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFJSObject<Subclass>, construct)
    {
# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &Subclass::jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, argc, OPENVRML_JS_ARGV(cx, vp));
    }

    template <typename Subclass>
    JSBool MFJSObject<Subclass>::initObject(JSContext * const cx,
                                            JSObject * const obj,
                                            const unsigned argc,
                                            jsval * const argv)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);
        try {
            std::auto_ptr<MFData> mfdata(new MFData(argc));

            for (unsigned i = 0; i < argc; ++i) {
                if (!JSVAL_IS_OBJECT(argv[i])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[i]),
                                      &Subclass::sfjsclass, argv)) {
                    return JS_FALSE;
                }
                mfdata->array[i] = argv[i];
            }

            //
            // Protect array values from gc.
            //
            AddRoots(cx, mfdata->array);

            if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
            mfdata.release();
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    template <typename Subclass>
    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFJSObject<Subclass>, setElement)
    {
        assert(cx);
        assert(obj);
        assert(vp);

        if (jspropertyop_id_is_int(id) && jspropertyop_id_to_int(id) > 0) {
            MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);

            //
            // Make sure new value is of the correct type.
            //
            JSObject * vp_obj;
            if (!JS_ValueToObject(cx, *vp, &vp_obj)) { return JS_FALSE; }
            if (!JS_InstanceOf(cx, vp_obj, &Subclass::sfjsclass, 0)) {
                JS_ReportError(cx, "Expected a %s.", Subclass::sfjsclass.name);
                return JS_FALSE;
            }

            //
            // Grow array if necessary.
            //
            if (size_t(jspropertyop_id_to_int(id)) >= mfdata->array.size()) {
                jsval newLength = INT_TO_JSVAL(jspropertyop_id_to_int(id) + 1);
                if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, obj, 0,
                                                                 &newLength)) {
                    return JS_FALSE;
                }
            }

            //
            // Put the new element in the array.
            //
            mfdata->array[jspropertyop_id_to_int(id)] = *vp;
            mfdata->changed = true;
        }
        return JS_TRUE;
    }

    template <typename Subclass>
    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFJSObject<Subclass>, setLength)
    {
        using std::copy;

        assert(cx);
        assert(obj);
        assert(vp);

        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        uint32_t new_length;
        if (!JS_ValueToECMAUint32(cx, *vp, &new_length)) { return JS_FALSE; }

        if (new_length == mfdata->array.size()) {
            mfdata->changed = true;
            return JS_TRUE; // Nothing to do.
        }

        try {
            JsvalArray newArray(new_length);
            AddRoots(cx, newArray); // Protect from gc.

            //
            // Copy the values from the old array.
            //
            const size_t length = newArray.size() < mfdata->array.size()
                ? newArray.size()
                : mfdata->array.size();
            copy(mfdata->array.begin(), mfdata->array.begin() + length,
                 newArray.begin());

            //
            // Initialize the rest of the array with new values.
            //
            try {
                for (size_t i = length; i < newArray.size(); ++i) {
                    JSObject * const element =
                        js_construct_object(cx, &Subclass::sfjsclass, 0, 0);
                    if (!element) { throw std::bad_alloc(); }
                    newArray[i] = OBJECT_TO_JSVAL(element);
                }
            } catch (std::bad_alloc &) {
                RemoveRoots(cx, newArray);
                throw;
            }

            //
            // Remove roots for the values in the old array.
            //
            RemoveRoots(cx, mfdata->array);

            //
            // Finally, swap the new vector with the old one.
            //
            std::swap(mfdata->array, newArray);
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        mfdata->changed = true;
        return JS_TRUE;
    }

    template <typename Subclass>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFJSObject<Subclass>, toString)
    {
        MFData * const mfdata =
            static_cast<MFData *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        assert(mfdata);

        std::ostringstream out;
        out << '[';
        for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_OBJECT(mfdata->array[i]));
            const sfield::sfdata * const sfdata =
                static_cast<sfield::sfdata *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
            assert(sfdata);
            out << sfdata->field_value();
            if ((i + 1) < mfdata->array.size()) { out << ", "; }
        }
        out << ']';

        JSString * jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
        if (!jsstr) { return JS_FALSE; }
        OPENVRML_JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(jsstr));
        return JS_TRUE;
    }

    template <typename Subclass>
    void MFJSObject<Subclass>::finalize(JSContext * const cx, JSObject * const obj)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);
        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        if (mfdata) {
            RemoveRoots(cx, mfdata->array);
            delete mfdata;
            JS_SetPrivate(cx, obj, 0);
        }
    }


    template <typename Subclass>
    JSObject * MFJSDouble<Subclass>::initClass(JSContext * const cx,
                                               JSObject * const obj)
        OPENVRML_NOTHROW
    {
        static JSPropertySpec properties[] =
            { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
              { 0, 0, 0, 0, 0 } };

        static JSFunctionSpec methods[] =
            { { "toString", toString, 0, 0 },
              { 0, 0, 0, 0 } };

        JSObject * proto = JS_InitClass(cx, obj, 0, &Subclass::jsclass,
                                        construct, 0, // constructor function, min arg count
                                        properties, methods,
                                        0, 0); // static properties and methods
        if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
        return proto;
    }

    template <typename Subclass>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFJSDouble<Subclass>, construct)
    {
# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &Subclass::jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, argc, OPENVRML_JS_ARGV(cx, vp));
    }

    template <typename Subclass>
    JSBool MFJSDouble<Subclass>::initObject(JSContext * const cx,
                                            JSObject * const obj,
                                            const unsigned argc,
                                            jsval * const argv)
        OPENVRML_NOTHROW
    {
        try {
            std::auto_ptr<MFData> mfdata(new MFData(argc));
            for (unsigned i = 0; i < argc; ++i) {
                double number;
                if (!JS_ValueToNumber(cx, argv[i], &number)) { return JS_FALSE; }
                if (!JS_NewNumberValue(cx, number, &mfdata->array[i])) {
                    return JS_FALSE;
                }
            }

            if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }

            //
            // Protect array values from gc.
            //
            AddRoots(cx, mfdata->array);

            mfdata.release();

        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    template <typename Subclass>
    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFJSDouble<Subclass>, setElement)
    {
        if (jspropertyop_id_is_int(id) && jspropertyop_id_to_int(id) >= 0) {
            MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);

            //
            // Make sure new value is a number.
            //
            double number;
            if (!JS_ValueToNumber(cx, *vp, &number)) { return JS_FALSE; }

            //
            // Grow array if necessary.
            //
            if (size_t(jspropertyop_id_to_int(id)) >= mfdata->array.size()) {
                jsval newLength = INT_TO_JSVAL(jspropertyop_id_to_int(id) + 1);
                if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, obj, 0,
                                                                 &newLength)) {
                    return JS_FALSE;
                }
            }

            //
            // Put the new element in the array.
            //
            if (!JS_NewNumberValue(cx, number,
                                   &mfdata->array[jspropertyop_id_to_int(id)])) {
                return JS_FALSE;
            }
            mfdata->changed = true;
        }
        return JS_TRUE;
    }

    template <typename Subclass>
    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFJSDouble<Subclass>, setLength)
    {
        using std::copy;

        assert(cx);
        assert(obj);
        assert(vp);

        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        uint32_t new_length;
        if (!JS_ValueToECMAUint32(cx, *vp, &new_length)) { return JS_FALSE; }

        if (size_t(JSVAL_TO_INT(*vp)) == mfdata->array.size()) {
            mfdata->changed = true;
            return JS_TRUE; // Nothing to do.
        }

        try {
            JsvalArray newArray(new_length);
            AddRoots(cx, newArray); // Protect from gc.

            //
            // Copy the values from the old array.
            //
            const size_t length = newArray.size() < mfdata->array.size()
                ? newArray.size()
                : mfdata->array.size();
            copy(mfdata->array.begin(), mfdata->array.begin() + length,
                 newArray.begin());

            //
            // Initialize the rest of the array with new values.
            //
            try {
                for (size_t i = length; i < newArray.size(); ++i) {
                    if (!JS_NewNumberValue(cx, 0.0, &newArray[i])) {
                        throw std::bad_alloc();
                    }
                }
            } catch (std::bad_alloc &) {
                RemoveRoots(cx, newArray);
                throw;
            }

            //
            // Remove roots for the values in the old array.
            //
            RemoveRoots(cx, mfdata->array);

            //
            // Finally, swap the new vector with the old one.
            //
            std::swap(mfdata->array, newArray);
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        mfdata->changed = true;
        return JS_TRUE;
    }

    template <typename Subclass>
    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFJSDouble<Subclass>, toString)
    {
        MFData * const mfdata =
            static_cast<MFData *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        assert(mfdata);

        std::ostringstream out;
        out << '[';
        for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
            out << jsval_to_double(mfdata->array[i]);
            if ((i + 1) < mfdata->array.size()) { out << ", "; }
        }
        out << ']';

        JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
        if (!jsstr) { return JS_FALSE; }
        OPENVRML_JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(jsstr));
        return JS_TRUE;
    }

    template <typename Subclass>
    void MFJSDouble<Subclass>::finalize(JSContext * const cx, JSObject * const obj)
        OPENVRML_NOTHROW
    {
        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        if (mfdata) {
            RemoveRoots(cx, mfdata->array);
            delete mfdata;
            JS_SetPrivate(cx, obj, 0);
        }
    }

    JSClass MFBool::jsclass = {
        "MFBool",            // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    std::auto_ptr<openvrml::mfbool>
    MFBool::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);

        if (!JS_InstanceOf(cx, obj, &MFBool::jsclass, 0)) {
            throw bad_conversion("MFBool object expected.");
        }

        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mfbool>
            mfbool(new openvrml::mfbool(mfdata->array.size()));
        std::vector<bool> temp = mfbool->value();
        for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_BOOLEAN(mfdata->array[i]));
            temp[i] = JSVAL_TO_BOOLEAN(mfdata->array[i]);
        }
        mfbool->value(temp);
        return mfbool;
    }

    JSBool MFBool::toJsval(const std::vector<bool> & bools,
                           JSContext * const cx,
                           JSObject * const obj,
                           jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const mfboolObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mfboolObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(bools.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mfboolObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfboolObj));
        for (size_t i = 0; i < bools.size(); ++i) {
            mfdata->array[i] = BOOLEAN_TO_JSVAL(bools[i]);
        }
        *rval = OBJECT_TO_JSVAL(mfboolObj);
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFBool, construct)
    {
# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, argc, OPENVRML_JS_ARGV(cx, vp));
    }

    JSBool MFBool::initObject(JSContext * const cx,
                              JSObject * const obj,
                              const unsigned argc,
                              jsval * const argv)
    {
        assert(cx);
        assert(obj);

        try {
            std::auto_ptr<MFData> mfdata(new MFData(argc));
            for (unsigned i = 0; i < argc; ++i) {
                //
                // Convert the jsval to an int32 and back to a jsval in order
                // to remove any decimal part.
                //
                JSBool boolean;
                if (!JS_ValueToBoolean(cx, argv[i], &boolean)) {
                    return JS_FALSE;
                }
                mfdata->array[i] = BOOLEAN_TO_JSVAL(boolean);
            }
            if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
            mfdata.release();
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFBool, setElement)
    {
        assert(cx);
        assert(obj);
        assert(vp);

        if (jspropertyop_id_is_int(id) && jspropertyop_id_to_int(id) >= 0) {
            const size_t index = jspropertyop_id_to_int(id);

            MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);

            //
            // Grow array if necessary.
            //
            if (index >= mfdata->array.size()) {
                jsval newLength = INT_TO_JSVAL(jspropertyop_id_to_int(id) + 1);
                if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, obj, 0,
                                                                 &newLength)) {
                    return JS_FALSE;
                }
            }

            //
            // Convert the jsval to an int32 and back to a jsval in order
            // to remove any decimal part.
            //
            JSBool b;
            if (!JS_ValueToBoolean(cx, *vp, &b)) { return JS_FALSE; }
            mfdata->array[index] = BOOLEAN_TO_JSVAL(b);
            mfdata->changed = true;
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFBool, setLength)
    {
        assert(cx);
        assert(obj);
        assert(vp);

        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        uint32_t new_length;
        if (!JS_ValueToECMAUint32(cx, *vp, &new_length)) { return JS_FALSE; }

        try {
            if (size_t(JSVAL_TO_INT(*vp)) != mfdata->array.size()) {
                mfdata->array.resize(JSVAL_TO_INT(*vp));
            }
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        mfdata->changed = true;
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFBool, toString)
    {
        MFData * const mfdata =
            static_cast<MFData *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        assert(mfdata);

        std::ostringstream out;
        out << '[';
        for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            out << (JSVAL_TO_BOOLEAN(mfdata->array[i]) ? "TRUE" : "FALSE");
            if ((i + 1) < mfdata->array.size()) { out << ", "; }
        }
        out << ']';

        JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
        if (!jsstr) { return JS_FALSE; }
        OPENVRML_JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(jsstr));
        return JS_TRUE;
    }

    void MFBool::finalize(JSContext * const cx, JSObject * const obj)
    {
        delete static_cast<MFData *>(JS_GetPrivate(cx, obj));
        JS_SetPrivate(cx, obj, 0);
    }

    JSClass MFColor::jsclass = {
        "MFColor",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSClass & MFColor::sfjsclass = SFColor::jsclass;

    std::auto_ptr<openvrml::mfcolor>
    MFColor::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);

        if (!JS_InstanceOf(cx, obj, &MFColor::jsclass, 0)) {
            throw bad_conversion("MFColor object expected.");
        }

        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mfcolor>
            mfcolor(new openvrml::mfcolor(mfdata->array.size()));
        std::vector<openvrml::color> temp = mfcolor->value();
        for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_OBJECT(mfdata->array[i]));
            assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                                 &SFColor::jsclass, 0));
            const sfield::sfdata * const sfdata =
                static_cast<sfield::sfdata *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
            assert(sfdata);

            const openvrml::sfcolor & sfcolor =
                static_cast<openvrml::sfcolor &>(sfdata->field_value());
            temp[i] = sfcolor.value();
        }
        mfcolor->value(temp);
        return mfcolor;
    }

    JSBool MFColor::toJsval(const std::vector<openvrml::color> & colors,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const mfcolorObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mfcolorObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(colors.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mfcolorObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfcolorObj));
        for (size_t i = 0; i < colors.size(); ++i) {
            if (!SFColor::toJsval(colors[i], cx, obj, &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        *rval = OBJECT_TO_JSVAL(mfcolorObj);
        return JS_TRUE;
    }

    JSClass MFFloat::jsclass = {
        "MFFloat",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getElement
        setElement,          // setElement
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSBool MFFloat::toJsval(const std::vector<float> & floats,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);
        assert(rval);

        JSObject * const mffloatObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mffloatObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(floats.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mffloatObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mffloatObj));
        for (size_t i = 0; i < floats.size(); ++i) {
            if (!JS_NewNumberValue(cx, floats[i], &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        *rval = OBJECT_TO_JSVAL(mffloatObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::mffloat>
    MFFloat::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);

        if (!JS_InstanceOf(cx, obj, &MFFloat::jsclass, 0)) {
            throw bad_conversion("MFFloat object expected.");
        }
        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mffloat>
            mffloat(new openvrml::mffloat(mfdata->array.size()));
        std::vector<float> temp = mffloat->value();
        for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
            temp[i] = float(jsval_to_double(mfdata->array[i]));
        }
        mffloat->value(temp);
        return mffloat;
    }

    JSClass MFDouble::jsclass = {
        "MFDouble",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getElement
        setElement,          // setElement
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSBool MFDouble::toJsval(const std::vector<double> & doubles,
                             JSContext * const cx,
                             JSObject * const obj,
                             jsval * const rval)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);
        assert(rval);

        JSObject * const mfdoubleObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mfdoubleObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(doubles.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mfdoubleObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfdoubleObj));
        for (size_t i = 0; i < doubles.size(); ++i) {
            if (!JS_NewNumberValue(cx, doubles[i], &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        *rval = OBJECT_TO_JSVAL(mfdoubleObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::mfdouble>
    MFDouble::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);

        if (!JS_InstanceOf(cx, obj, &MFDouble::jsclass, 0)) {
            throw bad_conversion("MFDouble object expected.");
        }
        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mfdouble>
            mfdouble(new openvrml::mfdouble(mfdata->array.size()));
        std::vector<double> temp = mfdouble->value();
        for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
            temp[i] = jsval_to_double(mfdata->array[i]);
        }
        mfdouble->value(temp);
        return mfdouble;
    }

    JSClass MFInt32::jsclass = {
        "MFInt32",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSObject * MFInt32::initClass(JSContext * const cx, JSObject * const obj) {
        assert(cx);
        assert(obj);

        static JSPropertySpec properties[] =
            { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
              { 0, 0, 0, 0, 0 } };

        static JSFunctionSpec methods[] =
            { { "toString", toString, 0, 0 },
              { 0, 0, 0, 0 } };

        JSObject * proto =
            JS_InitClass(cx, obj, 0, &jsclass,
                         construct, 0, // constructor function, min arg count
                         properties, methods,
                         0, 0); // static properties and methods
        if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
        return proto;
    }

    JSBool MFInt32::toJsval(const std::vector<openvrml::int32> & int32s,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);
        assert(rval);

        JSObject * const mfint32Obj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mfint32Obj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(int32s.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mfint32Obj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfint32Obj));
        for (size_t i = 0; i < int32s.size(); ++i) {
            mfdata->array[i] = INT_TO_JSVAL(int32s[i]);
        }
        *rval = OBJECT_TO_JSVAL(mfint32Obj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::mfint32>
    MFInt32::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);

        if (!JS_InstanceOf(cx, obj, &MFInt32::jsclass, 0)) {
            throw bad_conversion("MFInt32 object expected.");
        }
        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mfint32>
            mfint32(new openvrml::mfint32(mfdata->array.size()));
        std::vector<openvrml::int32> temp = mfint32->value();
        for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_INT(mfdata->array[i]));
            temp[i] = JSVAL_TO_INT(mfdata->array[i]);
        }
        mfint32->value(temp);
        return mfint32;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFInt32, construct)
    {
# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, argc, OPENVRML_JS_ARGV(cx, vp));
    }

    JSBool MFInt32::initObject(JSContext * const cx,
                               JSObject * const obj,
                               const unsigned argc,
                               jsval * const argv)
    {
        assert(cx);
        assert(obj);

        try {
            std::auto_ptr<MFData> mfdata(new MFData(argc));
            for (unsigned i = 0; i < argc; ++i) {
                //
                // Convert the jsval to an int32 and back to a jsval in order
                // to remove any decimal part.
                //
                int32_t integer;
                if (!JS_ValueToECMAInt32(cx, argv[i], &integer)) {
                    return JS_FALSE;
                }
                if (!JS_NewNumberValue(cx, static_cast<double>(integer),
                                       &mfdata->array[i])) {
                    return JS_FALSE;
                }
            }
            if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
            mfdata.release();
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFInt32, setElement)
    {
        assert(cx);
        assert(obj);
        assert(vp);

        if (jspropertyop_id_is_int(id) && jspropertyop_id_to_int(id) >= 0) {
            const size_t index = jspropertyop_id_to_int(id);

            MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);

            //
            // Grow array if necessary.
            //
            if (index >= mfdata->array.size()) {
                jsval newLength = INT_TO_JSVAL(jspropertyop_id_to_int(id) + 1);
                if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, obj, 0,
                                                                 &newLength)) {
                    return JS_FALSE; }
            }

            //
            // Convert the jsval to an int32 and back to a jsval in order
            // to remove any decimal part.
            //
            int32_t i;
            if (!JS_ValueToECMAInt32(cx, *vp, &i)) { return JS_FALSE; }
            if (!JS_NewNumberValue(cx, static_cast<double>(i),
                                   &mfdata->array[index])) {
                return JS_FALSE;
            }
            mfdata->changed = true;
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFInt32, setLength)
    {
        assert(cx);
        assert(obj);
        assert(vp);

        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        uint32_t new_length;
        if (!JS_ValueToECMAUint32(cx, *vp, &new_length)) { return JS_FALSE; }

        try {
            if (size_t(JSVAL_TO_INT(*vp)) != mfdata->array.size()) {
                mfdata->array.resize(JSVAL_TO_INT(*vp));
            }
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        mfdata->changed = true;
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFInt32, toString)
    {
        MFData * const mfdata =
            static_cast<MFData *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        assert(mfdata);

        std::ostringstream out;
        out << '[';
        for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            out << JSVAL_TO_INT(mfdata->array[i]);
            if ((i + 1) < mfdata->array.size()) { out << ", "; }
        }
        out << ']';

        JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
        if (!jsstr) { return JS_FALSE; }
        OPENVRML_JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(jsstr));
        return JS_TRUE;
    }

    void MFInt32::finalize(JSContext * const cx, JSObject * const obj)
    {
        delete static_cast<MFData *>(JS_GetPrivate(cx, obj));
        JS_SetPrivate(cx, obj, 0);
    }

    JSClass MFNode::jsclass = {
        "MFNode",            // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSObject * MFNode::initClass(JSContext * const cx, JSObject * const obj)
    OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);

        static JSPropertySpec properties[] =
            { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
              { 0, 0, 0, 0, 0 } };

        static JSFunctionSpec methods[] =
            { { "toString", toString, 0, 0 },
              { 0, 0, 0, 0 } };

        JSObject * proto = JS_InitClass(cx, obj, 0, &jsclass,
                                        construct, 0, // constructor function, min arg count
                                        properties, methods,
                                        0, 0); // static properties and methods
        if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
        return proto;
    }

    JSBool MFNode::initObject(JSContext * const cx, JSObject * const obj,
                              const unsigned argc, jsval * const argv)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);

        try {
            std::auto_ptr<MFData> mfdata(new MFData(argc));

            script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
            JSClass & sfnode_jsclass = s.sfnode_class;

            for (unsigned i = 0; i < argc; ++i) {
                //
                // Make sure all args are SFNodes.
                //
                if (!JSVAL_IS_OBJECT(argv[i])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[i]),
                                      &sfnode_jsclass, argv)) {
                    return JS_FALSE;
                }
                mfdata->array[i] = argv[i];
            }
            if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
            mfdata.release();
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    JSBool
    MFNode::toJsval(
        const std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
        JSContext * const cx,
        JSObject * const obj,
        jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const mfnodeObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mfnodeObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(nodes.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mfnodeObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfnodeObj));
        for (size_t i = 0; i < nodes.size(); ++i) {
            if (!SFNode::toJsval(nodes[i], cx, obj, &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        *rval = OBJECT_TO_JSVAL(mfnodeObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::mfnode>
    MFNode::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);

# ifndef NDEBUG
        script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
        JSClass & sfnode_jsclass = s.sfnode_class;
# endif

        if (!JS_InstanceOf(cx, obj, &MFNode::jsclass, 0)) {
            throw bad_conversion("MFNode object expected.");
        }
        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mfnode>
            mfnode(new openvrml::mfnode(mfdata->array.size()));
        std::vector<boost::intrusive_ptr<openvrml::node> > temp = mfnode->value();
        for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_OBJECT(mfdata->array[i]));
            assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                                 &sfnode_jsclass, 0));
            const sfield::sfdata * const sfdata =
                static_cast<sfield::sfdata *>(
                    JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
            assert(sfdata);

            const openvrml::sfnode & sfnode =
                static_cast<openvrml::sfnode &>(sfdata->field_value());
            temp[i] = sfnode.value();
        }
        mfnode->value(temp);
        return mfnode;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFNode, construct)
    {
# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, argc, OPENVRML_JS_ARGV(cx, vp));
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFNode, setElement)
    {
        assert(cx);
        assert(obj);
        assert(vp);

        if (!jspropertyop_id_is_int(id) && jspropertyop_id_to_int(id) >= 0) {
            MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);

            //
            // Make sure new value is an SFNode.
            //
            JSObject * vp_obj;
            if (!JS_ValueToObject(cx, *vp, &vp_obj)) { return JS_FALSE; }
            script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
            JSClass & sfnode_jsclass = s.sfnode_class;
            if (!JS_InstanceOf(cx, vp_obj, &sfnode_jsclass, 0)) {
                JS_ReportError(cx, "Expected an SFNode.");
                return JS_FALSE;
            }


            //
            // Grow array if necessary.
            //
            if (size_t(jspropertyop_id_to_int(id)) >= mfdata->array.size()) {
                jsval newLength = INT_TO_JSVAL(jspropertyop_id_to_int(id) + 1);
                if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, obj, 0,
                                                                 &newLength)) {
                    return JS_FALSE;
                }
            }

            //
            // Put the new element in the array.
            //
            mfdata->array[jspropertyop_id_to_int(id)] = *vp;
            mfdata->changed = true;
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFNode, setLength)
    {
        assert(cx);
        assert(obj);
        assert(vp);

        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        uint32_t new_length;
        if (!JS_ValueToECMAUint32(cx, *vp, &new_length)) { return JS_FALSE; }

        try {
            using std::copy;
            using std::swap;

            JsvalArray newArray(new_length);
            AddRoots(cx, newArray); // Protect from gc.

            //
            // Copy the values from the old array.
            //
            const size_t length = newArray.size() < mfdata->array.size()
                ? newArray.size()
                : mfdata->array.size();
            copy(mfdata->array.begin(), mfdata->array.begin() + length,
                 newArray.begin());

            //
            // Initialize the rest of the array with new values.
            //
            try {
                jsval arg = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "Group {}"));

                script & s = *static_cast<script *>(JS_GetContextPrivate(cx));
                JSClass & sfnode_jsclass = s.sfnode_class;

                for (size_t i = length; i < newArray.size(); ++i) {
                    JSObject * const element =
                        js_construct_object_with_arguments(cx, &sfnode_jsclass,
                                                           0, 0,
                                                           1, &arg);
                    if (!element) { throw std::bad_alloc(); }
                    newArray[i] = OBJECT_TO_JSVAL(element);
                }
            } catch (std::bad_alloc &) {
                RemoveRoots(cx, newArray);
                throw;
            }

            //
            // Remove roots for the values in the old array.
            //
            RemoveRoots(cx, mfdata->array);

            //
            // Finally, swap the new vector with the old one.
            //
            swap(mfdata->array, newArray);
            mfdata->changed = true;
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFNode, toString)
    {
        MFData * const mfdata =
            static_cast<MFData *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        assert(mfdata);

        std::ostringstream out;
        out << '[';
        for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            const sfield::sfdata * const sfdata =
                static_cast<sfield::sfdata *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
            assert(sfdata);
            out << sfdata->field_value();
            if ((i + 1) < mfdata->array.size()) { out << ", "; }
        }
        out << ']';

        JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
        if (!jsstr) { return JS_FALSE; }
        OPENVRML_JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(jsstr));
        return JS_TRUE;
    }

    void MFNode::finalize(JSContext * const cx, JSObject * const obj)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);
        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        if (mfdata) {
            RemoveRoots(cx, mfdata->array);
            delete mfdata;
            JS_SetPrivate(cx, obj, 0);
        }
    }

    JSClass MFRotation::jsclass = {
        "MFRotation",        // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSClass & MFRotation::sfjsclass = SFRotation::jsclass;

    JSBool MFRotation::toJsval(const std::vector<openvrml::rotation> & rotations,
                               JSContext * const cx,
                               JSObject * const obj,
                               jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const mfrotationObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mfrotationObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(rotations.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mfrotationObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfrotationObj));
        for (size_t i = 0; i < rotations.size(); ++i) {
            if (!SFRotation::toJsval(rotations[i], cx, obj, &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        *rval = OBJECT_TO_JSVAL(mfrotationObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::mfrotation>
    MFRotation::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);

        if (!JS_InstanceOf(cx, obj, &MFRotation::jsclass, 0)) {
            throw bad_conversion("MFRotation object expected.");
        }
        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mfrotation>
            mfrotation(new openvrml::mfrotation(mfdata->array.size()));
        std::vector<openvrml::rotation> temp = mfrotation->value();
        for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_OBJECT(mfdata->array[i]));
            assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                                 &SFRotation::jsclass, 0));
            const sfield::sfdata * const sfdata =
                static_cast<sfield::sfdata *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
            assert(sfdata);

            const openvrml::sfrotation & sfrotation =
                static_cast<openvrml::sfrotation &>(sfdata->field_value());
            temp[i] = sfrotation.value();
        }
        mfrotation->value(temp);
        return mfrotation;
    }

    JSClass MFString::jsclass = {
        "MFString",          // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSObject * MFString::initClass(JSContext * const cx, JSObject * const obj)
    OPENVRML_NOTHROW
    {
        static JSPropertySpec properties[] =
            { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
              { 0, 0, 0, 0, 0 } };

        static JSFunctionSpec methods[] =
            { { "toString", toString, 0, 0 },
              { 0, 0, 0, 0 } };

        JSObject * proto =
            JS_InitClass(cx, obj, 0, &jsclass,
                         construct, 0, // constructor function, min arg count
                         properties, methods,
                         0, 0); // static properties and methods
        if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
        return proto;
    }

    JSBool MFString::toJsval(const std::vector<std::string> & strings,
                             JSContext * const cx,
                             JSObject * const obj,
                             jsval * const rval)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);
        assert(rval);
        JSObject * const mfstringObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mfstringObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(strings.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mfstringObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfstringObj));
        for (size_t i = 0; i < strings.size(); ++i) {
            JSString * jsstring = JS_NewStringCopyZ(cx, strings[i].c_str());
            if (!jsstring) { return JS_FALSE; }
            mfdata->array[i] = STRING_TO_JSVAL(jsstring);
        }
        *rval = OBJECT_TO_JSVAL(mfstringObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::mfstring>
    MFString::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);

        if (!JS_InstanceOf(cx, obj, &MFString::jsclass, 0)) {
            throw bad_conversion("MFString object expected.");
        }
        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mfstring>
            mfstring(new openvrml::mfstring(mfdata->array.size()));
        std::vector<std::string> temp = mfstring->value();
        for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_STRING(mfdata->array[i]));
            const char * const str =
                JS_EncodeString(cx, JSVAL_TO_STRING(mfdata->array[i]));
            temp[i] = str;
        }
        mfstring->value(temp);
        return mfstring;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFString, construct)
    {
# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, argc, OPENVRML_JS_ARGV(cx, vp));
    }

    JSBool MFString::initObject(JSContext * const cx,
                                JSObject * const obj,
                                const unsigned argc,
                                jsval * const argv)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);

        try {
            std::auto_ptr<MFData> mfdata(new MFData(argc));
            for (unsigned i = 0; i < argc; ++i) {
                JSString * const str = JS_ValueToString(cx, argv[i]);
                if (!str) { return JS_FALSE; }
                mfdata->array[i] = STRING_TO_JSVAL(str);
            }

            //
            // Protect array values from gc.
            //
            AddRoots(cx, mfdata->array);

            if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
            mfdata.release();
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFString, setElement)
    {
        if (jspropertyop_id_is_int(id) && jspropertyop_id_to_int(id) >= 0) {
            MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);

            //
            // Grow array if necessary.
            //
            if (size_t(jspropertyop_id_to_int(id)) >= mfdata->array.size()) {
                jsval newLength = INT_TO_JSVAL(jspropertyop_id_to_int(id) + 1);
                if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, obj, 0,
                                                                 &newLength)) {
                    return JS_FALSE;
                }
            }

            //
            // Convert the value to a JSString, if necessary.
            //
            JSString * const str = JS_ValueToString(cx, *vp);
            if (!str) { return JS_FALSE; }

            //
            // Put the new element in the array.
            //
            mfdata->array[jspropertyop_id_to_int(id)] = STRING_TO_JSVAL(str);
            mfdata->changed = true;
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(MFString, setLength)
    {
        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);

        uint32_t new_length;
        if (!JS_ValueToECMAUint32(cx, *vp, &new_length)) { return JS_FALSE; }

        try {
            JsvalArray newArray(new_length);
            AddRoots(cx, newArray); // Protect from gc.

            //
            // Copy the values from the old array.
            //
            const size_t length = newArray.size() < mfdata->array.size()
                ? newArray.size()
                : mfdata->array.size();
            std::copy(mfdata->array.begin(), mfdata->array.begin() + length,
                      newArray.begin());

            //
            // Initialize the rest of the array with new values.
            //
            try {
                for (size_t i = length; i < newArray.size(); ++i) {
                    JSString * jsstring = JS_NewStringCopyZ(cx, "");
                    if (!jsstring) { throw std::bad_alloc(); }
                    newArray[i] = STRING_TO_JSVAL(jsstring);
                }
            } catch (std::bad_alloc &) {
                RemoveRoots(cx, newArray);
                throw;
            }

            //
            // Remove roots for the values in the old array.
            //
            RemoveRoots(cx, mfdata->array);

            //
            // Finally, swap the new vector with the old one.
            //
            std::swap(mfdata->array, newArray);
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        mfdata->changed = true;
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(MFString, toString)
    {
        MFData * const mfdata =
            static_cast<MFData *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        assert(mfdata);

        std::ostringstream out;
        out << '[';
        for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_STRING(mfdata->array[i]));
            out << '\"'
                << JS_EncodeString(cx, JSVAL_TO_STRING(mfdata->array[i]))
                << '\"';
            if ((i + 1) < mfdata->array.size()) { out << ", "; }
        }
        out << ']';

        JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
        if (!jsstr) { return JS_FALSE; }
        OPENVRML_JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(jsstr));
        return JS_TRUE;
    }

    void MFString::finalize(JSContext * const cx, JSObject * const obj)
        OPENVRML_NOTHROW
    {
        MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
        if (mfdata) {
            RemoveRoots(cx, mfdata->array);
            delete mfdata;
            JS_SetPrivate(cx, obj, 0);
        }
    }

    JSClass MFTime::jsclass = {
        "MFTime",            // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSBool MFTime::toJsval(const std::vector<double> & times,
                           JSContext * const cx,
                           JSObject * const obj,
                           jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const mftimeObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mftimeObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(times.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mftimeObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mftimeObj));
        for (size_t i = 0; i < times.size(); ++i) {
            if (!JS_NewNumberValue(cx, times[i], &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        *rval = OBJECT_TO_JSVAL(mftimeObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::mftime>
    MFTime::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);

        if (!JS_InstanceOf(cx, obj, &MFTime::jsclass, 0)) {
            throw bad_conversion("MFTime object expected.");
        }
        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mftime>
            mftime(new openvrml::mftime(mfdata->array.size()));
        std::vector<double> temp = mftime->value();
        for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
            temp[i] = jsval_to_double(mfdata->array[i]);
        }
        mftime->value(temp);
        return mftime;
    }

    JSClass MFVec2f::jsclass = {
        "MFVec2f",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSClass & MFVec2f::sfjsclass = SFVec2f::jsclass;

    JSBool MFVec2f::toJsval(const std::vector<openvrml::vec2f> & vec2fs,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const mfvec2fObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mfvec2fObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(vec2fs.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mfvec2fObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfvec2fObj));
        for (size_t i = 0; i < vec2fs.size(); ++i) {
            if (!SFVec2f::toJsval(vec2fs[i], cx, obj, &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        *rval = OBJECT_TO_JSVAL(mfvec2fObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::mfvec2f>
    MFVec2f::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);
        if (!JS_InstanceOf(cx, obj, &MFVec2f::jsclass, 0)) {
            throw bad_conversion("MFVec2f object expected.");
        }
        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mfvec2f>
            mfvec2f(new openvrml::mfvec2f(mfdata->array.size()));
        std::vector<openvrml::vec2f> temp = mfvec2f->value();
        for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_OBJECT(mfdata->array[i]));
            assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                                 &SFVec2f::jsclass, 0));
            const sfield::sfdata * const sfdata =
                static_cast<sfield::sfdata *>(
                    JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
            assert(sfdata);

            const openvrml::sfvec2f & sfvec2f =
                static_cast<openvrml::sfvec2f &>(sfdata->field_value());
            temp[i] = sfvec2f.value();
        }
        mfvec2f->value(temp);
        return mfvec2f;
    }

    JSClass MFVec2d::jsclass = {
        "MFVec2d",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSClass & MFVec2d::sfjsclass = SFVec2d::jsclass;

    JSBool MFVec2d::toJsval(const std::vector<openvrml::vec2d> & vec2ds,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const mfvec2dObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mfvec2dObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(vec2ds.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mfvec2dObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfvec2dObj));
        for (size_t i = 0; i < vec2ds.size(); ++i) {
            if (!SFVec2d::toJsval(vec2ds[i], cx, obj, &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        *rval = OBJECT_TO_JSVAL(mfvec2dObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::mfvec2d>
    MFVec2d::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);
        if (!JS_InstanceOf(cx, obj, &MFVec2d::jsclass, 0)) {
            throw bad_conversion("MFVec2d object expected.");
        }
        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mfvec2d>
            mfvec2d(new openvrml::mfvec2d(mfdata->array.size()));
        std::vector<openvrml::vec2d> temp = mfvec2d->value();
        for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_OBJECT(mfdata->array[i]));
            assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                                 &SFVec2d::jsclass, 0));
            const sfield::sfdata * const sfdata =
                static_cast<sfield::sfdata *>(
                    JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
            assert(sfdata);

            const openvrml::sfvec2d & sfvec2d =
                static_cast<openvrml::sfvec2d &>(sfdata->field_value());
            temp[i] = sfvec2d.value();
        }
        mfvec2d->value(temp);
        return mfvec2d;
    }

    JSClass MFVec3f::jsclass = {
        "MFVec3f",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSClass & MFVec3f::sfjsclass = SFVec3f::jsclass;

    JSBool MFVec3f::toJsval(const std::vector<openvrml::vec3f> & vec3fs,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const mfvec3fObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mfvec3fObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(vec3fs.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mfvec3fObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfvec3fObj));
        for (size_t i = 0; i < vec3fs.size(); ++i) {
            if (!SFVec3f::toJsval(vec3fs[i], cx, obj, &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        *rval = OBJECT_TO_JSVAL(mfvec3fObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::mfvec3f>
    MFVec3f::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);
        if (!JS_InstanceOf(cx, obj, &MFVec3f::jsclass, 0)) {
            throw bad_conversion("MFVec3f object expected.");
        }
        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mfvec3f>
            mfvec3f(new openvrml::mfvec3f(mfdata->array.size()));
        std::vector<openvrml::vec3f> temp = mfvec3f->value();
        for (MField::JsvalArray::size_type i = 0;
             i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_OBJECT(mfdata->array[i]));
            assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                                 &SFVec3f::jsclass, 0));
            const sfield::sfdata * const sfdata =
                static_cast<sfield::sfdata *>(
                    JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
            assert(sfdata);

            const openvrml::sfvec3f & sfvec3f =
                static_cast<openvrml::sfvec3f &>(sfdata->field_value());
            temp[i] = sfvec3f.value();
        }
        mfvec3f->value(temp);
        return mfvec3f;
    }

    JSClass MFVec3d::jsclass = {
        "MFVec3d",           // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSClass & MFVec3d::sfjsclass = SFVec3d::jsclass;

    JSBool MFVec3d::toJsval(const std::vector<openvrml::vec3d> & vec3ds,
                            JSContext * const cx,
                            JSObject * const obj,
                            jsval * const rval)
        OPENVRML_NOTHROW
    {
        JSObject * const mfvec3dObj = js_construct_object(cx, &jsclass, 0, obj);
        if (!mfvec3dObj) { return JS_FALSE; }

        jsval length = INT_TO_JSVAL(vec3ds.size());
        if (!OPENVRML_CALL_JSSTRICTPROPERTYOP(setLength, cx, mfvec3dObj, 0,
                                                         &length)) {
            return JS_FALSE;
        }

        MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfvec3dObj));
        for (size_t i = 0; i < vec3ds.size(); ++i) {
            if (!SFVec3d::toJsval(vec3ds[i], cx, obj, &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        *rval = OBJECT_TO_JSVAL(mfvec3dObj);
        return JS_TRUE;
    }

    std::auto_ptr<openvrml::mfvec3d>
    MFVec3d::createFromJSObject(JSContext * const cx, JSObject * const obj)
        OPENVRML_THROW2(bad_conversion, std::bad_alloc)
    {
        assert(cx);
        assert(obj);
        if (!JS_InstanceOf(cx, obj, &MFVec3d::jsclass, 0)) {
            throw bad_conversion("MFVec3d object expected.");
        }
        MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
        assert(mfdata);
        std::auto_ptr<openvrml::mfvec3d>
            mfvec3d(new openvrml::mfvec3d(mfdata->array.size()));
        std::vector<openvrml::vec3d> temp = mfvec3d->value();
        for (MField::JsvalArray::size_type i = 0;
             i < mfdata->array.size(); ++i) {
            assert(JSVAL_IS_OBJECT(mfdata->array[i]));
            assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                                 &SFVec3d::jsclass, 0));
            const sfield::sfdata * const sfdata =
                static_cast<sfield::sfdata *>(
                    JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
            assert(sfdata);

            const openvrml::sfvec3d & sfvec3d =
                static_cast<openvrml::sfvec3d &>(sfdata->field_value());
            temp[i] = sfvec3d.value();
        }
        mfvec3d->value(temp);
        return mfvec3d;
    }


    JSClass VrmlMatrix::Row::jsclass = {
        "VrmlMatrixRow_",    // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        JS_FinalizeStub,     // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSObject * VrmlMatrix::Row::initClass(JSContext * const cx,
                                          JSObject * const obj)
    OPENVRML_NOTHROW
    {
        return JS_InitClass(cx, obj, 0, &jsclass,
                            construct, 0,
                            0, 0,
                            0, 0);
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(VrmlMatrix::Row, construct)
    {
# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return JS_TRUE;
    }

    JSBool VrmlMatrix::Row::getElement(JSContext * const cx,
                                       JSObject * const obj,
                                       const jspropertyop_id id,
                                       jsval * const vp)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);

        if (jspropertyop_id_is_int(id) && jspropertyop_id_to_int(id) >= 0 && jspropertyop_id_to_int(id) < 4) {
            assert(JS_GetPrivate(cx, obj));
            const float (&row)[4] =
                *static_cast<float (*)[4]>(JS_GetPrivate(cx, obj));

            if (!JS_NewNumberValue(cx, row[jspropertyop_id_to_int(id)], vp)) {
                return JS_FALSE;
            }
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(VrmlMatrix::Row, setElement)
    {
        assert(cx);
        assert(obj);

        if (jspropertyop_id_is_int(id) && jspropertyop_id_to_int(id) >= 0 && jspropertyop_id_to_int(id) < 4) {
            double d;
            if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

            assert(JS_GetPrivate(cx, obj));
            float (&row)[4] = *static_cast<float (*)[4]>(JS_GetPrivate(cx, obj));
            row[jspropertyop_id_to_int(id)] = float(d);
        }
        return JS_TRUE;
    }

    JSClass VrmlMatrix::jsclass = {
        "VrmlMatrix",        // name
        JSCLASS_HAS_PRIVATE, // flags
        JS_PropertyStub,     // addProperty
        JS_PropertyStub,     // delProperty
        getElement,          // getProperty
        setElement,          // setProperty
        JS_EnumerateStub,    // enumerate
        JS_ResolveStub,      // resolve
        JS_ConvertStub,      // convert
        finalize,            // finalize
        0,                   // getObjectOps
        0,                   // checkAccess
        0,                   // call
        0,                   // construct
        0,                   // xdrObject
        0,                   // hasInstance
        0,                   // mark
        0                    // spare
    };

    JSObject * VrmlMatrix::initClass(JSContext * const cx, JSObject * const obj)
    OPENVRML_NOTHROW
    {
        static JSFunctionSpec methods[] =
            { { "setTransform", setTransform, 0, 0 },
              { "getTransform", getTransform, 0, 0 },
              { "inverse", inverse, 0, 0 },
              { "transpose", transpose, 0, 0 },
              { "multLeft", multLeft, 1, 0 },
              { "multRight", multRight, 1, 0 },
              { "multVecMatrix", multVecMatrix, 1, 0 },
              { "multMatrixVec", multMatrixVec, 1, 0 },
              { "toString", toString, 0, 0 },
              { 0, 0, 0, 0 } };

        JSObject * const proto = JS_InitClass(cx, obj, 0, &jsclass,
                                              construct, 0,
                                              0, methods,
                                              0, 0);
        const double mat[16] = {};
        if (!proto || !initObject(cx, proto, mat)) { return 0; }
        return proto;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(VrmlMatrix, construct)
    {
        double mat[16] = {};
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp),
                                 "dddddddddddddddd",
                                 &mat[0], &mat[1], &mat[2], &mat[3],
                                 &mat[4], &mat[5], &mat[6], &mat[7],
                                 &mat[8], &mat[9], &mat[10], &mat[11],
                                 &mat[12], &mat[13], &mat[14], &mat[15])) {
            return JS_FALSE;
        }

# ifdef OPENVRML_FAST_JSNATIVE
        JSObject * obj = 0;
# endif
# ifndef OPENVRML_FAST_JSNATIVE
        //
        // If called without new, replace obj with a new object.
        //
        if (!OPENVRML_JS_IS_CONSTRUCTING(cx, vp)) {
# endif
            obj = JS_NewObject(cx, &jsclass, 0, 0);
            if (!obj) { return JS_FALSE; }
            OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
# ifndef OPENVRML_FAST_JSNATIVE
        }
# endif
        return initObject(cx, obj, mat);
    }

    JSBool VrmlMatrix::initObject(JSContext * const cx,
                                  JSObject * const obj,
                                  const double (&mat)[16])
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);

        try {
            std::auto_ptr<openvrml::mat4f> mat_ptr(
                new openvrml::mat4f(openvrml::make_mat4f(
                    float(mat[0]), float(mat[1]), float(mat[2]), float(mat[3]),
                    float(mat[4]), float(mat[5]), float(mat[6]), float(mat[7]),
                    float(mat[8]), float(mat[9]), float(mat[10]), float(mat[11]),
                    float(mat[12]), float(mat[13]), float(mat[14]), float(mat[15]))));
            if (!JS_SetPrivate(cx, obj, mat_ptr.get())) { return JS_FALSE; }
            mat_ptr.release();
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        return JS_TRUE;
    }

    JSBool VrmlMatrix::getElement(JSContext * const cx,
                                  JSObject * const obj,
                                  const jspropertyop_id id,
                                  jsval * const vp)
        OPENVRML_NOTHROW
    {
        assert(cx);
        assert(obj);

        if (jspropertyop_id_is_int(id) && jspropertyop_id_to_int(id) >= 0 && jspropertyop_id_to_int(id) < 4) {
            using openvrml::mat4f;
            mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));

            //
            // Construct the result object.
            //
            JSObject * const robj = js_construct_object(cx, &Row::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) { return JS_FALSE; }

            float (&row)[4] = (*thisMat)[jspropertyop_id_to_int(id)];
            if (!JS_SetPrivate(cx, robj, &row)) { return JS_FALSE; }
            *vp = OBJECT_TO_JSVAL(robj);
        }
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSSTRICTPROPERTYOP(VrmlMatrix, setElement)
    {
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(VrmlMatrix, setTransform)
    {
        using boost::polymorphic_downcast;
        using openvrml::mat4f;
        using openvrml::rotation;
        using openvrml::make_rotation;
        using openvrml::vec3f;
        using openvrml::make_vec3f;
        using openvrml::sfrotation;
        using openvrml::sfvec3f;

        JSObject * translation_obj = 0, * rotation_obj = 0, * scale_obj = 0,
            * scale_orientation_obj = 0, * center_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "/ooooo",
                                 &translation_obj, &rotation_obj, &scale_obj,
                                 &scale_orientation_obj, &center_obj)) {
            return JS_FALSE;
        }

        vec3f translation = make_vec3f(0.0, 0.0, 0.0);
        rotation rot = make_rotation(0.0, 0.0, 1.0, 0.0);
        vec3f scale = make_vec3f(1.0, 1.0, 1.0);
        rotation scale_orientation = make_rotation(0.0, 0.0, 1.0, 0.0);
        vec3f center = make_vec3f(0.0, 0.0, 0.0);

        if (translation_obj) {
            if (!JS_InstanceOf(cx, translation_obj, &SFVec3f::jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            assert(JS_GetPrivate(cx, translation_obj));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, translation_obj));
            translation = polymorphic_downcast<sfvec3f *>(&sfdata.field_value())
                ->value();
        }

        if (rotation_obj) {
            if (!JS_InstanceOf(cx, rotation_obj, &SFRotation::jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            assert(JS_GetPrivate(cx, rotation_obj));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, rotation_obj));
            rot = polymorphic_downcast<sfrotation *>(&sfdata.field_value())
                ->value();
        }

        if (scale_obj) {
            if (!JS_InstanceOf(cx, scale_obj, &SFVec3f::jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            assert(JS_GetPrivate(cx, scale_obj));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, scale_obj));
            scale = polymorphic_downcast<sfvec3f *>(&sfdata.field_value())
                ->value();
        }

        if (scale_orientation_obj) {
            if (!JS_InstanceOf(cx, scale_orientation_obj, &SFRotation::jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            assert(JS_GetPrivate(cx, scale_orientation_obj));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(
                    JS_GetPrivate(cx, scale_orientation_obj));
            scale_orientation = polymorphic_downcast<sfrotation *>(
                &sfdata.field_value())->value();
        }

        if (center_obj) {
            if (!JS_InstanceOf(cx, center_obj, &SFVec3f::jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            assert(JS_GetPrivate(cx, center_obj));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, center_obj));
            center = polymorphic_downcast<sfvec3f *>(&sfdata.field_value())
                ->value();
        }

        mat4f * const thisMat =
            static_cast<mat4f *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        assert(thisMat);

        *thisMat = make_transformation_mat4f(translation,
                                             rot,
                                             scale,
                                             scale_orientation,
                                             center);
        OPENVRML_JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(VrmlMatrix, getTransform)
    {
        using boost::polymorphic_downcast;
        using openvrml::mat4f;
        using openvrml::rotation;
        using openvrml::vec3f;
        using openvrml::sfrotation;
        using openvrml::sfvec3f;

        JSObject * translation_obj = 0, * rotation_obj = 0, * scale_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "/ooo",
                                 &translation_obj, &rotation_obj, &scale_obj)) {
            return JS_FALSE;
        }

        const mat4f * const thisMat =
            static_cast<mat4f *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        assert(thisMat);

        vec3f translation;
        rotation rot;
        vec3f scale;

        thisMat->transformation(translation, rot, scale);

        if (translation_obj) {
            if (!JS_InstanceOf(cx, translation_obj, &SFVec3f::jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            assert(JS_GetPrivate(cx, translation_obj));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, translation_obj));
            polymorphic_downcast<sfvec3f *>(&sfdata.field_value())
                ->value(translation);
        }

        if (rotation_obj) {
            if (!JS_InstanceOf(cx, rotation_obj, &SFRotation::jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            assert(JS_GetPrivate(cx, rotation_obj));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, rotation_obj));
            polymorphic_downcast<sfrotation *>(&sfdata.field_value())->value(rot);
        }

        if (scale_obj) {
            if (!JS_InstanceOf(cx, scale_obj, &SFVec3f::jsclass,
                               OPENVRML_JS_ARGV(cx, vp))) {
                return JS_FALSE;
            }
            assert(JS_GetPrivate(cx, scale_obj));
            const sfield::sfdata & sfdata =
                *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, scale_obj));
            polymorphic_downcast<sfvec3f *>(&sfdata.field_value())->value(scale);
        }

        OPENVRML_JS_SET_RVAL(cx, vp, JSVAL_VOID);
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(VrmlMatrix, inverse)
    {
        using openvrml::mat4f;

        JSObject * const robj =
            js_construct_object(cx, &VrmlMatrix::jsclass, 0,
                               JS_GetParent(cx,
                                            OPENVRML_JS_THIS_OBJECT(cx, vp)));
        if (!robj) { return JS_FALSE; }

        const mat4f * const thisMat =
            static_cast<mat4f *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        assert(thisMat);

        mat4f * const newMat = static_cast<mat4f *>(JS_GetPrivate(cx, robj));
        assert(newMat);
        *newMat = thisMat->inverse();
        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(VrmlMatrix, transpose)
    {
        using openvrml::mat4f;

        JSObject * const robj =
            js_construct_object(cx, &VrmlMatrix::jsclass, 0,
                               JS_GetParent(cx,
                                            OPENVRML_JS_THIS_OBJECT(cx, vp)));
        if (!robj) { return JS_FALSE; }

        const mat4f * const thisMat =
            static_cast<mat4f *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        assert(thisMat);

        mat4f * const newMat = static_cast<mat4f *>(JS_GetPrivate(cx, robj));
        assert(newMat);
        *newMat = thisMat->transpose();
        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(VrmlMatrix, multLeft)
    {
        using openvrml::mat4f;

        JSObject * arg_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &arg_obj)) {
            return JS_FALSE;
        }
        if (!JS_InstanceOf(cx, arg_obj, &VrmlMatrix::jsclass,
                           OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        void * arg_obj_private_data = JS_GetPrivate(cx, arg_obj);
        assert(arg_obj_private_data);
        const mat4f & argMat = *static_cast<mat4f *>(arg_obj_private_data);

        void * obj_private_data =
            JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        assert(obj_private_data);
        const mat4f & thisMat = *static_cast<mat4f *>(obj_private_data);

        //
        // Construct the result object.
        //
        JSObject * const robj =
            js_construct_object(cx, &VrmlMatrix::jsclass, 0,
                               JS_GetParent(cx,
                                            OPENVRML_JS_THIS_OBJECT(cx, vp)));
        if (!robj) { return JS_FALSE; }
        void * robj_private_data = JS_GetPrivate(cx, robj);
        assert(robj_private_data);
        mat4f & resultMat = *static_cast<mat4f *>(robj_private_data);

        resultMat = argMat * thisMat;

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(VrmlMatrix, multRight)
    {
        using openvrml::mat4f;

        JSObject * arg_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &arg_obj)) {
            return JS_FALSE;
        }
        if (!JS_InstanceOf(cx, arg_obj, &VrmlMatrix::jsclass,
                           OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, arg_obj));
        const mat4f & argMat = *static_cast<mat4f *>(JS_GetPrivate(cx, arg_obj));

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const mat4f & thisMat =
            *static_cast<mat4f *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));

        //
        // Construct the result object.
        //
        JSObject * const robj =
            js_construct_object(cx, &VrmlMatrix::jsclass, 0,
                               JS_GetParent(cx,
                                            OPENVRML_JS_THIS_OBJECT(cx, vp)));
        if (!robj) { return JS_FALSE; }
        void * private_data = JS_GetPrivate(cx, robj);
        assert(private_data);
        mat4f & resultMat = *static_cast<mat4f *>(private_data);

        resultMat = thisMat * argMat;

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(VrmlMatrix, multVecMatrix)
    {
        using openvrml::mat4f;

        //
        // Make sure argument is an SFVec3f.
        //
        JSObject * arg_obj = 0;
        if (!JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                 &arg_obj)) {
            return JS_FALSE;
        }
        if (!JS_InstanceOf(cx, arg_obj, &SFVec3f::jsclass,
                           OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        assert(JS_GetPrivate(cx, arg_obj));
        const sfield::sfdata & arg_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
        const openvrml::sfvec3f & argVec =
            *boost::polymorphic_downcast<openvrml::sfvec3f *>(
                &arg_sfdata.field_value());

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const mat4f & thisMat =
            *static_cast<mat4f *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));

        //
        // Construct the result object.
        //
        static JSObject * const proto = 0;
        JSObject * const parent =
            JS_GetParent(cx, OPENVRML_JS_THIS_OBJECT(cx, vp));
        JSObject * const robj =
            js_construct_object(cx, &SFVec3f::jsclass, proto, parent);
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        openvrml::sfvec3f & resultVec =
            *boost::polymorphic_downcast<openvrml::sfvec3f *>(
                &robj_sfdata.field_value());

        resultVec.value(argVec.value() * thisMat);

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(VrmlMatrix, multMatrixVec)
    {
        using openvrml::mat4f;

        //
        // Make sure argument is an SFVec3f.
        //
        JSObject * arg_obj = 0;
        if (JS_ConvertArguments(cx, argc, OPENVRML_JS_ARGV(cx, vp), "o",
                                &arg_obj)) {
            return JS_FALSE;
        }
        if (!JS_InstanceOf(cx, arg_obj, &SFVec3f::jsclass,
                           OPENVRML_JS_ARGV(cx, vp))) {
            return JS_FALSE;
        }

        const sfield::sfdata & arg_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, arg_obj));
        const openvrml::sfvec3f & argVec =
            *boost::polymorphic_downcast<openvrml::sfvec3f *>(
                &arg_sfdata.field_value());

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const mat4f & thisMat =
            *static_cast<mat4f *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));

        //
        // Construct the result object.
        //
        JSObject * const robj =
            js_construct_object(cx, &SFVec3f::jsclass, 0,
                                JS_GetParent(cx,
                                             OPENVRML_JS_THIS_OBJECT(cx, vp)));
        if (!robj) { return JS_FALSE; }

        assert(JS_GetPrivate(cx, robj));
        sfield::sfdata & robj_sfdata =
            *static_cast<sfield::sfdata *>(JS_GetPrivate(cx, robj));
        openvrml::sfvec3f & resultVec =
            *boost::polymorphic_downcast<openvrml::sfvec3f *>(
                &robj_sfdata.field_value());

        resultVec.value(thisMat * argVec.value());

        OPENVRML_JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(robj));
        return JS_TRUE;
    }

    OPENVRML_DEFINE_MEMBER_JSNATIVE(VrmlMatrix, toString)
    {
        using openvrml::mat4f;

        assert(JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));
        const mat4f & thisMat =
            *static_cast<mat4f *>(
                JS_GetPrivate(cx, OPENVRML_JS_THIS_OBJECT(cx, vp)));

        JSString * jsstr = 0;
        try {
            std::ostringstream out;
            out << thisMat;
            jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
            if (!jsstr) { return JS_FALSE; }
        } catch (std::bad_alloc &) {
            JS_ReportOutOfMemory(cx);
            return JS_FALSE;
        }
        OPENVRML_JS_SET_RVAL(cx, vp, STRING_TO_JSVAL(jsstr));
        return JS_TRUE;
    }

    void VrmlMatrix::finalize(JSContext * const cx, JSObject * const obj)
        OPENVRML_NOTHROW
    {
        delete static_cast<openvrml::mat4f *>(JS_GetPrivate(cx, obj));
        JS_SetPrivate(cx, obj, 0);
    }
} // namespace
