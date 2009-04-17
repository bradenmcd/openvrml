// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001  Henri Manson
// Copyright 2002  Thomas Flynn
// Copyright 2007, 2008  Braden McDaniel
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

# include <vrml_Browser.h>
# include <vrml_Field.h>
# include <vrml_ConstField.h>
# include <vrml_BaseNode.h>
# include <vrml_field_SFBool.h>
# include <vrml_field_SFColor.h>
# include <vrml_field_SFFloat.h>
# include <vrml_field_SFImage.h>
# include <vrml_field_SFInt32.h>
# include <vrml_field_SFNode.h>
# include <vrml_field_SFRotation.h>
# include <vrml_field_SFString.h>
# include <vrml_field_SFTime.h>
# include <vrml_field_SFVec2f.h>
# include <vrml_field_SFVec2d.h>
# include <vrml_field_SFVec3f.h>
# include <vrml_field_SFVec3d.h>
# include <vrml_field_MFBool.h>
# include <vrml_field_MFColor.h>
# include <vrml_field_MFFloat.h>
# include <vrml_field_MFInt32.h>
# include <vrml_field_MFNode.h>
# include <vrml_field_MFRotation.h>
# include <vrml_field_MFString.h>
# include <vrml_field_MFTime.h>
# include <vrml_field_MFVec2f.h>
# include <vrml_field_MFVec2d.h>
# include <vrml_field_MFVec3f.h>
# include <vrml_field_MFVec3d.h>
# include <vrml_field_ConstSFBool.h>
# include <vrml_field_ConstSFColor.h>
# include <vrml_field_ConstSFFloat.h>
# include <vrml_field_ConstSFImage.h>
# include <vrml_field_ConstSFInt32.h>
# include <vrml_field_ConstSFNode.h>
# include <vrml_field_ConstSFRotation.h>
# include <vrml_field_ConstSFString.h>
# include <vrml_field_ConstSFTime.h>
# include <vrml_field_ConstSFVec2f.h>
# include <vrml_field_ConstSFVec2d.h>
# include <vrml_field_ConstSFVec3f.h>
# include <vrml_field_ConstSFVec3d.h>
# include <vrml_field_ConstMFBool.h>
# include <vrml_field_ConstMFColor.h>
# include <vrml_field_ConstMFFloat.h>
# include <vrml_field_ConstMFInt32.h>
# include <vrml_field_ConstMFNode.h>
# include <vrml_field_ConstMFRotation.h>
# include <vrml_field_ConstMFString.h>
# include <vrml_field_ConstMFTime.h>
# include <vrml_field_ConstMFVec2f.h>
# include <vrml_field_ConstMFVec2d.h>
# include <vrml_field_ConstMFVec3f.h>
# include <vrml_field_ConstMFVec3d.h>
# include <vrml_node_Node.h>
# include <vrml_node_Script.h>

# include <openvrml/browser.h>
# include <openvrml/scene.h>
# include <private.h>
# include <ltdl.h>
# include <boost/array.hpp>
# include <boost/filesystem/path.hpp>
# include <boost/multi_index/detail/scope_guard.hpp>
# include <boost/scoped_array.hpp>
# include <algorithm>
# include <iostream>
# include <cstdio>
# include <cstring>
# include <sstream>

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# ifdef _WIN32
#   define PATH_SEPARATOR ";"
# else /* UNIX and macintosh */
#   define PATH_SEPARATOR ":"
# endif

using namespace boost::multi_index::detail;  // for scope_guard

namespace {

    OPENVRML_JAVA_LOCAL lt_dlhandle libjvm_handle;
    OPENVRML_JAVA_LOCAL jint (*CreateJavaVM)(JavaVM **, void **, void *);

    OPENVRML_JAVA_LOCAL JavaVM * vm;

    OPENVRML_JAVA_LOCAL class OPENVRML_JAVA_LOCAL load_libjvm {
    public:
        load_libjvm();
        ~load_libjvm();
    } load_libjvm_;

    OPENVRML_JAVA_LOCAL const std::string
    create_searchpath_from_java_home(const std::string & java_home)
    {
        assert(!java_home.empty());
        std::ostringstream searchpath;
        searchpath << java_home << "/lib/" << OPENVRML_JVM_ARCH << "/client:"
                   << java_home << "/lib/" << OPENVRML_JVM_ARCH << "/server";
        return searchpath.str();
    }

    load_libjvm::load_libjvm()
    {
        int result = lt_dlinit();
        if (result != 0) {
            std::cerr << lt_dlerror() << std::endl;
            return;
        }
        std::ostringstream jvm_searchpath;
        const char * const java_home_env = getenv("JAVA_HOME");
        if (java_home_env) {
            jvm_searchpath << create_searchpath_from_java_home(java_home_env);
        }
        const std::string java_home = JAVA_HOME;
        if (!java_home.empty()
            && (!java_home_env || java_home != java_home_env)) {
            if (!jvm_searchpath.str().empty()) { jvm_searchpath << ':'; }
            jvm_searchpath << create_searchpath_from_java_home(java_home);
        }
        result = lt_dlsetsearchpath(jvm_searchpath.str().c_str());
        if (result != 0) {
            std::cerr << lt_dlerror() << std::endl;
            return;
        }
        libjvm_handle = lt_dlopen("libjvm.so");
        if (!libjvm_handle) {
            std::cerr << lt_dlerror() << std::endl;
            return;
        }
        CreateJavaVM =
            reinterpret_cast<jint (*)(JavaVM **, void **, void *)>(
                lt_dlsym(libjvm_handle, "JNI_CreateJavaVM"));
        if (!CreateJavaVM) {
            std::cerr << lt_dlerror() << std::endl;
        }
    }

    load_libjvm::~load_libjvm()
    {
        //
        // The Java VM doesn't seem to play very nicely with DestroyJavaVM; it
        // seems to wait for some internal threads to finish; and they never
        // do.  It's quite possible we're doing something wrong to provoke
        // this.  However, as long as OpenVRML doesn't support unloading of
        // script modules, we're only here if we're shutting down the
        // application.  And in that case, we can let the OS take care of
        // cleanup.
        //
        // if (vm) {
        //     vm->DestroyJavaVM();
        // }
        if (libjvm_handle) {
            int result = lt_dlclose(libjvm_handle);
            if (result != 0) {
                std::cerr << lt_dlerror() << std::endl;
                return;
            }
        }
        lt_dlexit();
    }


    /**
     * @brief Subclass of @c openvrml::script for JSAI.
     */
    class OPENVRML_JAVA_LOCAL script : public openvrml::script {

        friend void JNICALL (::Java_vrml_node_Script_updateField)(
            JNIEnv * env,
            jobject obj,
            jstring id,
            jobject value);

        friend void JNICALL (::Java_vrml_node_Script_emitEvent)(
            JNIEnv * env,
            jobject obj,
            jstring id,
            jobject value);

        jobject class_loader_;
        jobject browser_;
        jclass script_class_;
        jobject script_;
        jmethodID process_events_;
        jmethodID events_processed_;
        jclass event_class_;
        jmethodID event_ctor_;
        std::vector<jobject> events_received_;

    public:
        script(openvrml::script_node & node,
               const boost::shared_ptr<openvrml::resource_istream> & source);
        virtual ~script();

        using openvrml::script::node;
        using openvrml::script::direct_output;
        using openvrml::script::must_evaluate;

    private:
        virtual void do_initialize(double timestamp);
        virtual void do_process_event(const std::string & id,
                                      const openvrml::field_value & value,
                                      double timestamp);
        virtual void do_events_processed(double timestamp);
        virtual void do_shutdown(double timestamp);

        jclass find_class(JNIEnv & env, const std::string & class_name)
            OPENVRML_THROW2(std::runtime_error, std::bad_alloc);
        jobject create_browser_obj(JNIEnv & env)
            OPENVRML_THROW2(std::runtime_error, std::bad_alloc);
        jobject create_script_obj(JNIEnv & env)
            OPENVRML_THROW2(std::runtime_error, std::bad_alloc);

        void update_fields(JNIEnv & env);
        void emit_events(JNIEnv & env);
    };


    class OPENVRML_JAVA_LOCAL script_factory : public openvrml::script_factory {
    public:
        virtual ~script_factory() OPENVRML_NOTHROW;

        virtual std::auto_ptr<openvrml::script>
            create_script(
                openvrml::script_node & node,
                const boost::shared_ptr<openvrml::resource_istream> & source);
    };

} // namespace

extern "C" OPENVRML_JAVA_API void
openvrml_script_LTX_register_factory(
    openvrml::script_factory_registry & registry)
{
    using std::set;
    using std::string;
    using boost::array;
    using boost::shared_ptr;

    static const array<const char *, 3> media_types = {
        "application/java",
        "application/x-java",
        "application/x-java-vm"
    };

    static const set<string> media_type_set(media_types.begin(),
                                            media_types.end());
    static const set<string> uri_scheme_set;

    const shared_ptr<openvrml::script_factory> factory(new script_factory);

    registry.register_factory(media_type_set, uri_scheme_set, factory);
}

/**
 * @brief Called when the native library is loaded.
 *
 * @param vm Java VM.
 * @param reserved unused.
 * @return JNI Version needed by native library.
 */
jint JNICALL JNI_OnLoad(JavaVM *, void *)
{
  return JNI_VERSION_1_2;
}

/**
 * @brief Called when the native library is unloaded.
 *
 * @param vm Java VM.
 * @param reserved unused.
 */
void JNICALL JNI_OnUnload(JavaVM *, void *)
{}

namespace {

    OPENVRML_JAVA_LOCAL jint create_java_vm(JavaVM *& vm, JNIEnv *& env)
    {
        using std::ostringstream;

        const char * const classpath_env = getenv("CLASSPATH");
        ostringstream classpath_option;
        classpath_option << "-Djava.class.path="
                         << (classpath_env ? classpath_env : "")
                         << (classpath_env ? ":" : "")
                         << OPENVRML_PKGDATADIR_ "/java/script.jar";

        const std::string classpath_option_str = classpath_option.str();

        JavaVMOption options[2] = {};
        options[0].optionString =
            const_cast<char *>(classpath_option_str.c_str());
        options[1].optionString =
            const_cast<char *>("-verbose:class,jni");

        JavaVMInitArgs args;
        args.version = JNI_VERSION_1_2;
        args.options = options;
        args.nOptions = 2;

        return CreateJavaVM(&vm, reinterpret_cast<void **>(&env), &args);
    }

    OPENVRML_JAVA_LOCAL jobject create_url(JNIEnv & env, const char * const url)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        using boost::ref;

        //
        // We can safely run DeleteGlobalRef in the scope guard because
        // calling DeleteGlobalRef with 0 is a no-op.
        //
        jobject result_global_ref = 0;
        scope_guard result_global_ref_guard =
            make_obj_guard(env,
                           &JNIEnv::DeleteGlobalRef,
                           ref(result_global_ref));
        boost::ignore_unused_variable_warning(result_global_ref_guard);
        {
            if (env.PushLocalFrame(3) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            const jstring url_string = env.NewStringUTF(url);
            if (!url_string) {
                env.ExceptionClear();
                throw std::runtime_error("failed to construct string for URL");
            }

            const jclass url_class = env.FindClass("java/net/URL");
            if (!url_class) {
                env.ExceptionClear();
                throw std::runtime_error("could not find java.net.URL class");
            }

            const jmethodID ctor_id =
                env.GetMethodID(url_class, "<init>", "(Ljava/lang/String;)V");
            if (!ctor_id) {
                env.ExceptionClear();
                throw std::runtime_error("failed to get "
                                         "java.net.URL.URL(java.lang.String) "
                                         "constructor");
            }

            const jobject url_obj =
                env.NewObject(url_class, ctor_id, url_string);
            if (!url_obj) {
                env.ExceptionClear();
                throw std::runtime_error("could not create java.net.URL "
                                         "instance");
            }

            //
            // Create a global reference so that the new object will outlive
            // the local frame.
            //
            result_global_ref = env.NewGlobalRef(url_obj);
            if (!result_global_ref) { throw std::bad_alloc(); }
        }

        //
        // NewLocalRef does not throw any Java exceptions.
        //
        const jobject result = env.NewLocalRef(result_global_ref);
        if (!result) { throw std::bad_alloc(); }

        return result;
    }

    OPENVRML_JAVA_LOCAL jobject create_class_loader(JNIEnv & env,
                                                    const std::string & url_str)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        using boost::ref;

        //
        // We can safely run DeleteGlobalRef in the scope guard because
        // calling DeleteGlobalRef with 0 is a no-op.
        //
        jobject result_global_ref = 0;
        scope_guard result_global_ref_guard =
            make_obj_guard(env,
                           &JNIEnv::DeleteGlobalRef,
                           ref(result_global_ref));
        boost::ignore_unused_variable_warning(result_global_ref_guard);
        {
            if (env.PushLocalFrame(4) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            const jobject url = create_url(env, url_str.c_str());

            jobjectArray url_array =
                env.NewObjectArray(1, env.GetObjectClass(url), url);
            if (!url_array) {
                env.ExceptionClear();
                throw std::runtime_error("failed to construct URL array");
            }

            const jclass class_loader_class =
                env.FindClass("java/net/URLClassLoader");
            if (!class_loader_class) {
                env.ExceptionClear();
                throw std::runtime_error("could not find "
                                         "java.net.URLClassLoader class");
            }

            const jmethodID ctor_id = env.GetMethodID(class_loader_class,
                                                      "<init>",
                                                      "([Ljava/net/URL;)V");
            if (!ctor_id) {
                env.ExceptionClear();
                throw std::runtime_error(
                    "failed to get "
                    "java.net.URLClassLoader.URLClassLoader(java.net.URL) "
                    "constructor");
            }

            const jobject class_loader =
                env.NewObject(class_loader_class, ctor_id, url_array);
            if (!class_loader) {
                env.ExceptionClear();
                throw std::runtime_error("could not create "
                                         "java.net.URLClassLoader instance");
            }

            //
            // Create a global reference so that the new object will outlive
            // the local frame.
            //
            result_global_ref = env.NewGlobalRef(class_loader);
            if (!result_global_ref) { throw std::bad_alloc(); }
        }

        //
        // NewLocalRef does not throw any Java exceptions.
        //
        const jobject result = env.NewLocalRef(result_global_ref);
        if (!result) { throw std::bad_alloc(); }

        return result;
    }

    jobject script::create_script_obj(JNIEnv & env)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        assert(this->browser_);
        assert(this->script_class_);

        using boost::ref;

        //
        // We can safely run DeleteGlobalRef in the scope guard because
        // calling DeleteGlobalRef with 0 is a no-op.
        //
        jobject result_global_ref = 0;
        scope_guard result_global_ref_guard =
            make_obj_guard(env,
                           &JNIEnv::DeleteGlobalRef,
                           ref(result_global_ref));
        boost::ignore_unused_variable_warning(result_global_ref_guard);
        {
            if (env.PushLocalFrame(1) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            //
            // We call AllocObject to create the Script instance, set the
            // peer, and then call the constructor.  We do this so that code
            // in a Script subclass constructor can safely perform operations
            // that access the peer.
            //
            const jobject java_script = env.AllocObject(this->script_class_);
            if (!java_script) {
# ifndef NDEBUG
                env.ExceptionDescribe();
# endif
                env.ExceptionClear();
                throw std::runtime_error("could not create Java script "
                                         "instance");
            }

            const jfieldID peer =
                env.GetFieldID(this->script_class_, "peer", "J");
            const jlong peer_val =
                jlong(new boost::intrusive_ptr<openvrml::node>(&this->node));
            env.SetLongField(java_script, peer, peer_val);

            const jfieldID browser =
                env.GetFieldID(this->script_class_, "browser",
                               "Lvrml/Browser;");
            env.SetObjectField(java_script, browser, this->browser_);

            const jfieldID script_peer =
                env.GetFieldID(this->script_class_, "scriptPeer", "J");
            env.SetLongField(java_script, script_peer, jlong(this));

            const jmethodID ctor_id =
                env.GetMethodID(this->script_class_, "<init>", "()V");
            if (!ctor_id) {
# ifndef NDEBUG
                env.ExceptionDescribe();
# endif
                env.ExceptionClear();
                throw std::runtime_error("failed to get Java script class "
                                         "constructor");
            }
            env.CallNonvirtualVoidMethod(java_script, this->script_class_,
                                         ctor_id);

            assert(env.GetLongField(java_script, peer));

            //
            // Create a global reference so that the new object will outlive
            // the local frame.
            //
            result_global_ref = env.NewGlobalRef(java_script);
            if (!result_global_ref) { throw std::bad_alloc(); }
        }

        //
        // NewLocalRef does not throw any Java exceptions.
        //
        const jobject result = env.NewLocalRef(result_global_ref);
        if (!result) { throw std::bad_alloc(); }

        return result;
    }

    jclass script::find_class(JNIEnv & env, const std::string & class_name)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        assert(this->class_loader_);
        assert(!class_name.empty());

        using boost::ref;

        //
        // We can safely run DeleteGlobalRef in the scope guard because
        // calling DeleteGlobalRef with 0 is a no-op.
        //
        jclass result_global_ref = 0;
        scope_guard result_global_ref_guard =
            make_obj_guard(env,
                           &JNIEnv::DeleteGlobalRef,
                           ref(result_global_ref));
        boost::ignore_unused_variable_warning(result_global_ref_guard);
        {
            if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            const jclass class_loader_class =
                env.GetObjectClass(this->class_loader_);

            const jmethodID load_class_method =
                env.GetMethodID(class_loader_class,
                                "loadClass",
                                "(Ljava/lang/String;)Ljava/lang/Class;");
            if (!load_class_method) {
                env.ExceptionClear();
                throw std::runtime_error(
                    "failed to get "
                    "java.net.URLClassLoader.loadClass(java.lang.String) "
                    "method");
            }

            const jstring class_name_jstr =
                env.NewStringUTF(class_name.c_str());
            if (!class_name_jstr) {
                env.ExceptionClear();
                throw std::runtime_error("failed to construct jstring for "
                                         "class name");
            }

            const jclass class_obj =
                static_cast<jclass>(env.CallObjectMethod(this->class_loader_,
                                                         load_class_method,
                                                         class_name_jstr));
            if (!class_obj) {
# ifndef NDEBUG
                env.ExceptionDescribe();
# endif
                env.ExceptionClear();
                throw std::runtime_error("class loader could not find class \""
                                         + class_name + "\"");
            }

            //
            // Create a global reference so that the new object will outlive
            // the local frame.
            //
            result_global_ref =
                static_cast<jclass>(env.NewGlobalRef(class_obj));
            if (!result_global_ref) { throw std::bad_alloc(); }
        }

        //
        // NewLocalRef does not throw any Java exceptions.
        //
        const jclass result =
            static_cast<jclass>(env.NewLocalRef(result_global_ref));
        if (!result) { throw std::bad_alloc(); }

        return result;
    }

    jobject script::create_browser_obj(JNIEnv & env)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        using boost::ref;

        //
        // We can safely run DeleteGlobalRef in the scope guard because
        // calling DeleteGlobalRef with 0 is a no-op.
        //
        jobject result_global_ref = 0;
        scope_guard result_global_ref_guard =
            make_obj_guard(env,
                           &JNIEnv::DeleteGlobalRef,
                           ref(result_global_ref));
        boost::ignore_unused_variable_warning(result_global_ref_guard);
        {
            if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            const jclass browser_class = this->find_class(env, "vrml.Browser");

            const jmethodID ctor_id =
                env.GetMethodID(browser_class, "<init>", "()V");
            if (!ctor_id) {
                env.ExceptionClear();
                throw std::runtime_error("failed to get "
                                         "vrml.Browser.Browser() constructor");
            }

            const jobject browser_obj = env.NewObject(browser_class, ctor_id);
            if (!browser_obj) {
# ifndef NDEBUG
                env.ExceptionDescribe();
# endif
                env.ExceptionClear();
                throw std::runtime_error("could not create vrml.Browser "
                                         "instance");
            }

            const jfieldID peer = env.GetFieldID(browser_class, "peer", "J");
            env.SetLongField(browser_obj, peer,
                             jlong(&this->node.type().metatype().browser()));

            //
            // Create a global reference so that the new object will outlive
            // the local frame.
            //
            result_global_ref = env.NewGlobalRef(browser_obj);
            if (!result_global_ref) { throw std::bad_alloc(); }
        }

        //
        // NewLocalRef does not throw any Java exceptions.
        //
        const jobject result = env.NewLocalRef(result_global_ref);
        if (!result) { throw std::bad_alloc(); }

        return result;
    }

    //
    // Create a concrete Field from an openvrml::field_value.
    //
    // This function simply returns 0 if any Java exceptions are thrown.  It
    // does not clear the exception state.
    //
    OPENVRML_JAVA_LOCAL jobject
    clone_Field(JNIEnv & env,
                const openvrml::field_value & value,
                const bool const_ = false)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        using boost::ref;

        //
        // We can safely run DeleteGlobalRef in the scope guard because
        // calling DeleteGlobalRef with 0 is a no-op.
        //
        jobject result_global_ref = 0;
        scope_guard result_global_ref_guard =
            make_obj_guard(env,
                           &JNIEnv::DeleteGlobalRef,
                           ref(result_global_ref));
        boost::ignore_unused_variable_warning(result_global_ref_guard);
        {
            if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            using std::ostringstream;
            using openvrml::field_value;

            ostringstream class_name;
            class_name << "vrml/field/" << (const_ ? "Const" : "")
                       << value.type();
            const jclass clazz = env.FindClass(class_name.str().c_str());
            if (!clazz) {
                throw std::runtime_error("could not find class "
                                         + class_name.str());
            }

            const jmethodID ctor = env.GetMethodID(clazz, "<init>", "(J)V");
            if (!ctor) {
                throw std::runtime_error("failed to get ID for "
                                         + class_name.str() + " constructor");
            }

            std::auto_ptr<field_value> fv(value.clone());

            const jobject result =
                env.NewObject(clazz, ctor, jlong(fv.release()));
            if (!result) {
                throw std::runtime_error("could not create "
                                         + class_name.str());
            }
        }

        //
        // NewLocalRef does not throw any Java exceptions.
        //
        const jobject result = env.NewLocalRef(result_global_ref);
        if (!result) { throw std::bad_alloc(); }

        return result;
    }

    //
    // Create a concrete Field from an openvrml::field_value.
    //
    // This function simply returns 0 if any Java exceptions are thrown.  It
    // does not clear the exception state.
    //
    OPENVRML_JAVA_LOCAL jobject
    create_Field(JNIEnv & env,
                 const openvrml::field_value::type_id type,
                 const bool const_ = false)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using boost::ref;

        //
        // We can safely run DeleteGlobalRef in the scope guard because
        // calling DeleteGlobalRef with 0 is a no-op.
        //
        jobject result_global_ref = 0;
        scope_guard result_global_ref_guard =
            make_obj_guard(env,
                           &JNIEnv::DeleteGlobalRef,
                           ref(result_global_ref));
        boost::ignore_unused_variable_warning(result_global_ref_guard);
        {
            if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            using std::ostringstream;
            using openvrml::field_value;

            ostringstream class_name;
            class_name << "vrml/field/" << (const_ ? "Const" : "") << type;
            jclass clazz = env.FindClass(class_name.str().c_str());
            if (!clazz) {
                throw std::runtime_error("could not find class "
                                         + class_name.str());
            }

            const jmethodID ctor = env.GetMethodID(clazz, "<init>", "(J)V");
            if (!ctor) {
                throw std::runtime_error("failed to get ID for "
                                         + class_name.str() + " constructor");
            }

            std::auto_ptr<field_value> fv = field_value::create(type);

            const jobject result =
                env.NewObject(clazz, ctor, jlong(fv.release()));
            if (!result) {
                throw std::runtime_error("could not create "
                                         + class_name.str());
            }
        }

        //
        // NewLocalRef does not throw any Java exceptions.
        //
        const jobject result = env.NewLocalRef(result_global_ref);
        if (!result) { throw std::bad_alloc(); }

        return result;
    }

    OPENVRML_JAVA_LOCAL void
    init_script_fields(
        JNIEnv & env,
        const jclass script_class,
        const jobject script,
        const openvrml::script_node::field_value_map_t & field_map)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        assert(script_class);
        assert(script);
        if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
        scope_guard local_frame_guard =
            make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
        boost::ignore_unused_variable_warning(local_frame_guard);

        const jfieldID fields = env.GetFieldID(script_class,
                                               "fields",
                                               "Ljava/util/HashMap;");
        if (!fields) {
            throw std::runtime_error("failed to get vrml.node.Script.fields "
                                     "field identifier");
        }

        const jobject hash_map = env.GetObjectField(script, fields);
        const jclass hash_map_class = env.GetObjectClass(hash_map);
        const jmethodID put =
            env.GetMethodID(
                hash_map_class,
                "put",
                "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

        typedef openvrml::script_node::field_value_map_t field_map_t;

        for (field_map_t::const_iterator field = field_map.begin();
             field != field_map.end();
             ++field) {
            if (env.PushLocalFrame(3) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            const jstring name = env.NewStringUTF(field->first.c_str());
            const jobject val = clone_Field(env, *field->second);
            const jobject put_result =
                env.CallObjectMethod(hash_map, put, name, val);
            boost::ignore_unused_variable_warning(put_result);
        }
    }

    OPENVRML_JAVA_LOCAL jobject
    clone_NodeField(JNIEnv & env,
                    const openvrml::field_value & value)
        OPENVRML_THROW1(std::bad_alloc)
    {
        using boost::ref;

        //
        // We can safely run DeleteGlobalRef in the scope guard because
        // calling DeleteGlobalRef with 0 is a no-op.
        //
        jobject result_global_ref = 0;
        scope_guard result_global_ref_guard =
            make_obj_guard(env,
                           &JNIEnv::DeleteGlobalRef,
                           ref(result_global_ref));
        boost::ignore_unused_variable_warning(result_global_ref_guard);
        {
            if (env.PushLocalFrame(2) < 0) { return 0; }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            using std::ostringstream;
            using openvrml::field_value;

            ostringstream class_name;
            class_name << "vrml/node/Node" << value.type();
            jclass clazz = env.FindClass(class_name.str().c_str());
            if (!clazz) { return 0; }

            jobject result = env.AllocObject(clazz);
            if (!result) { return 0; }

            //
            // Create a global reference so that the new object will outlive
            // the local frame.
            //
            result_global_ref = env.NewGlobalRef(result);
            if (!result_global_ref) { return 0; }

            jfieldID peer = env.GetFieldID(clazz, "peer", "J");
            if (!peer) { return 0; }

            std::auto_ptr<field_value> fv(value.clone());
            env.SetLongField(result, peer, jlong(fv.release()));
        }

        //
        // NewLocalRef does not throw any Java exceptions.
        //
        const jobject result = env.NewLocalRef(result_global_ref);
        if (!result) { throw std::bad_alloc(); }

        return result;
    }

    OPENVRML_JAVA_LOCAL void
    init_script_eventouts(
        JNIEnv & env,
        const jclass script_class,
        const jobject script,
        const openvrml::script_node::eventout_map_t & eventout_map)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        assert(script_class);
        assert(script);
        if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
        scope_guard local_frame_guard =
            make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
        boost::ignore_unused_variable_warning(local_frame_guard);

        const jfieldID eventouts = env.GetFieldID(script_class,
                                                  "eventOuts",
                                                  "Ljava/util/HashMap;");
        if (!eventouts) {
            throw std::runtime_error("failed to get "
                                     "vrml.node.Script.eventOuts field "
                                     "identifier");
        }

        const jobject hash_map = env.GetObjectField(script, eventouts);
        const jclass hash_map_class = env.GetObjectClass(hash_map);
        const jmethodID put =
            env.GetMethodID(
                hash_map_class,
                "put",
                "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

        typedef openvrml::script_node::eventout_map_t eventout_map_t;

        for (eventout_map_t::const_iterator eventout = eventout_map.begin();
             eventout != eventout_map.end();
             ++eventout) {
            if (env.PushLocalFrame(3) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            const jstring name = env.NewStringUTF(eventout->first.c_str());
            const jobject val =
                clone_NodeField(env, eventout->second->value());
            const jobject put_result =
                env.CallObjectMethod(hash_map, put, name, val);
            boost::ignore_unused_variable_warning(put_result);
        }
    }

    /**
     * @brief Construct a script object from the given class name.
     *
     * This method requires that className.class exists either in the
     * given classDir or somewhere within the CLASSPATH.
     *
     * @param[in] node      the @c openvrml::script_node that uses this Script.
     * @param[in] source    an @c openvrml::resource_istream containing the
     *                      data for the Java class referenced in the Script
     *                      node's @a url field.
     */
    script::
    script(openvrml::script_node & node,
           const boost::shared_ptr<openvrml::resource_istream> & source):
        openvrml::script(node),
        class_loader_(0),
        browser_(0),
        script_class_(0),
        script_(0),
        process_events_(0),
        events_processed_(0),
        event_class_(0),
        event_ctor_(0)
    {
        //
        // Note that the initialization of vm_ and env_ isn't at all
        // thread-safe.  There shouldn't ever be more than one thread
        // here.  The VM is thread-specific; so if there *is* more
        // than one thread, we have other problems.
        //
        JNIEnv * env = 0;
        if (!vm) {
            jint result = create_java_vm(vm, env);
            if (result < 0) {
                throw std::runtime_error("cannot create Java virtual machine");
            }
        } else {
            jint result =
                vm->AttachCurrentThread(reinterpret_cast<void **>(&env), 0);
            if (result < 0) {
                throw std::runtime_error("failed to attach current thread to "
                                         "the Java virtual machine");
            }
        }

        assert(env); // If vm is set, env had better be.

        const std::string source_url = source->url();
        const std::string::size_type last_slash_pos = source_url.rfind('/');
        //
        // We want the trailing slash.  If it's missing, the class loader will
        // treat the path as a jar file rather than a directory.
        //
        const std::string url_path_prefix =
            source_url.substr(0, last_slash_pos + 1);

        {
            if (env->PushLocalFrame(4) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(*env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            const jobject class_loader_local_ref =
                create_class_loader(*env, url_path_prefix);
            this->class_loader_ = env->NewGlobalRef(class_loader_local_ref);
            if (!this->class_loader_) {
                throw std::runtime_error("failed to create global reference "
                                         "to Java class loader instance");
            }
            scope_guard class_loader_global_ref_guard =
                make_obj_guard(*env, &JNIEnv::DeleteGlobalRef,
                               this->class_loader_);

            const jobject browser_local_ref = create_browser_obj(*env);
            this->browser_ = env->NewGlobalRef(browser_local_ref);
            if (!this->browser_) {
                throw std::runtime_error("failed to create global reference "
                                         "to Java vrml.Browser instance");
            }
            scope_guard browser_global_ref_guard =
                make_obj_guard(*env, &JNIEnv::DeleteGlobalRef, this->browser_);

            const std::string::size_type class_name_start_pos =
                (last_slash_pos != std::string::npos) ? last_slash_pos + 1
                                                      : 0;

            const std::string::size_type ext_dot_pos =
                source_url.find('.', class_name_start_pos);

            const std::string::size_type class_name_length =
                (ext_dot_pos != std::string::npos && ext_dot_pos != 0)
                ? ext_dot_pos - class_name_start_pos
                : std::string::npos;

            const std::string class_name =
                source_url.substr(class_name_start_pos, class_name_length);

            const jclass script_class_local_ref =
                this->find_class(*env, class_name);
            this->script_class_ =
                static_cast<jclass>(env->NewGlobalRef(script_class_local_ref));
            if (!this->script_class_) {
                throw std::runtime_error("failed to create global reference "
                                         "to Java " + class_name + " class");
            }
            scope_guard script_class_global_ref_guard =
                make_obj_guard(*env, &JNIEnv::DeleteGlobalRef,
                               this->script_class_);

            const jobject script_local_ref = this->create_script_obj(*env);
            this->script_ = env->NewGlobalRef(script_local_ref);
            if (!this->script_) {
                throw std::runtime_error("failed to create global reference "
                                         "to Java " + class_name
                                         + " instance");
            }
            scope_guard script_global_ref_guard =
                make_obj_guard(*env, &JNIEnv::DeleteGlobalRef, this->script_);

            this->process_events_ =
                env->GetMethodID(this->script_class_, "processEvents",
                                 "(I[Lvrml/Event;)V");
            if (!this->process_events_) {
                env->ExceptionClear();
                throw std::runtime_error("failed to get " + class_name
                                         + ".processEvents(int, vrml.Event[]) "
                                         "method identifier");
            }

            this->events_processed_ =
                env->GetMethodID(this->script_class_, "eventsProcessed",
                                 "()V");
            if (!this->events_processed_) {
                env->ExceptionClear();
                throw std::runtime_error("failed to get " + class_name
                                         + ".eventsProcessed() method "
                                         "identifier");
            }

            const jobject event_class_local_ref = env->FindClass("vrml/Event");
            if (!event_class_local_ref) {
                env->ExceptionClear();
                throw std::runtime_error("could not find class vrml.Event");
            }
            this->event_class_ =
                static_cast<jclass>(env->NewGlobalRef(event_class_local_ref));
            if (!this->event_class_) {
                throw std::runtime_error("failed to create global reference "
                                         "to Java vrml.Event instance");
            }
            scope_guard event_class_global_ref_guard =
                make_obj_guard(*env, &JNIEnv::DeleteGlobalRef, this->event_class_);

            this->event_ctor_ =
                env->GetMethodID(this->event_class_,
                                 "<init>",
                                 "(Ljava/lang/String;DLvrml/ConstField;)V");
            if (!this->event_ctor_) {
# ifndef NDEBUG
                env->ExceptionDescribe();
# endif
                env->ExceptionClear();
                throw std::runtime_error(
                    "could not get "
                    "vrml.Event.Event(java.lang.String, double, "
                    "vrml.ConstField) constructor");
            }

            init_script_fields(*env, this->script_class_, this->script_,
                               this->node.field_value_map());
            init_script_eventouts(*env, this->script_class_, this->script_,
                                  this->node.eventout_map());

            event_class_global_ref_guard.dismiss();
            script_global_ref_guard.dismiss();
            script_class_global_ref_guard.dismiss();
            browser_global_ref_guard.dismiss();
            class_loader_global_ref_guard.dismiss();
        }
    }

    /**
     * @brief Destroy.
     */
    script::~script()
    {
        JNIEnv * env;
        jint result =
            vm->AttachCurrentThread(reinterpret_cast<void **>(&env), 0);
        if (result != 0) {
            this->node.type().metatype().browser()
                .err("failed to attach to the current thread in script "
                     "destructor");
            return;
        }
        env->DeleteGlobalRef(this->event_class_);
        env->DeleteGlobalRef(this->script_);
        env->DeleteGlobalRef(this->script_class_);
        env->DeleteGlobalRef(this->browser_);
        env->DeleteGlobalRef(this->class_loader_);
    }

    /**
     * @brief Initialize the Script object.
     *
     * @param[in] timestamp Time at which script is being initialized.
     */
    void script::do_initialize(double /* timestamp */)
    {
        JNIEnv * env;
        jint result =
            vm->AttachCurrentThread(reinterpret_cast<void **>(&env), 0);
        if (result != 0) {
            throw std::runtime_error("failed to attach to the current thread "
                                     "when initializing script");
        }

        const jmethodID initialize =
            env->GetMethodID(this->script_class_, "initialize", "()V");
        if (!initialize) {
            env->ExceptionClear();
            throw std::runtime_error("could not get Script class initialize() "
                                     "method identifier");
        }

        env->CallVoidMethod(this->script_, initialize);
        env->ExceptionClear();
        // XXX
        // XXX It would be nice if we had some way to get the output from
        // XXX ExceptionDescribe and emit it via browser::err.
        // XXX

        this->update_fields(*env);
        this->emit_events(*env);
    }

    //
    // Create a concrete ConstField from an openvrml::field_value.
    //
    // This function simply returns 0 if any Java exceptions are thrown.  It
    // does not clear the exception state.
    //
    OPENVRML_JAVA_LOCAL jobject
    clone_ConstField(JNIEnv & env, const openvrml::field_value & value)
        OPENVRML_THROW1(std::bad_alloc)
    {
        return clone_Field(env, value, true);
    }

    /**
     * @brief Queue an event to be processed.
     *
     * Events actually get processed in @c #do_events_processed.  Here, we
     * simply create a @c vrml.Event and add it to the @c #events_received_
     * vector.
     *
     * @param[in] id        event identifier.
     * @param[in] value     event value.
     * @param[in] timestamp the current timestamp.
     *
     * @sa #do_events_processed
     * @sa #events_received_
     */
    void script::do_process_event(const std::string & id,
                                  const openvrml::field_value & value,
                                  const double timestamp)
    {
        JNIEnv * env = 0;
        jint result =
            vm->AttachCurrentThread(reinterpret_cast<void **>(&env), 0);
        if (result != 0) {
            throw std::runtime_error("failed to attach to the current thread "
                                     "when processing event");
        }
        assert(env);

        if (env->PushLocalFrame(3) < 0) {
            throw std::runtime_error("failed to create local frame when "
                                     "processing event");
        }
        scope_guard local_frame_guard =
            make_obj_guard(*env, &JNIEnv::PopLocalFrame, jobject(0));
        boost::ignore_unused_variable_warning(local_frame_guard);

        const jstring event_name = env->NewStringUTF(id.c_str());
        if (!event_name) {
            env->ExceptionClear();
            throw std::runtime_error("failed to construct string for event "
                                     "name");
        }

        const jobject event_value = clone_ConstField(*env, value);
        if (!event_value) {
            env->ExceptionClear();
            throw std::runtime_error("failed to create vrml.ConstField "
                                     "instance for vrml.Event");
        }

        const jobject event = env->NewObject(this->event_class_,
                                             this->event_ctor_,
                                             event_name,
                                             timestamp,
                                             event_value);
        if (!event) {
            env->ExceptionClear();
            throw std::runtime_error("failed to create vrml.Event");
        }

        const jobject event_global_ref = env->NewGlobalRef(event);
        if (!event_global_ref) { throw std::bad_alloc(); }
        scope_guard event_global_ref_guard =
            make_obj_guard(*env, &JNIEnv::DeleteGlobalRef, event_global_ref);

        this->events_received_.push_back(event_global_ref);
        event_global_ref_guard.dismiss();
    }

    /**
     * @brief Called to process one or more events that have occurred.
     *
     * @param[in] timestamp the current timestamp.
     *
     * @pre @c !this->events_received_.empty()
     *
     * @post @c this->events_received_.empty()
     */
    void script::do_events_processed(const double timestamp)
    {
        assert(!this->events_received_.empty());

        JNIEnv * env = 0;
        jint result =
            vm->AttachCurrentThread(reinterpret_cast<void **>(&env), 0);
        if (result != 0) {
            throw std::runtime_error("failed to attach to the current thread "
                                     "when processing events");
        }
        assert(env);

        {
            if (env->PushLocalFrame(1) < 0) {
                throw std::runtime_error("failed to create local frame when "
                                         "processing events");
            }
            scope_guard local_frame_guard =
                make_obj_guard(*env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            const jobjectArray events =
                env->NewObjectArray(this->events_received_.size(),
                                    this->event_class_, 0);
            for (jsize i = 0; i < jsize(this->events_received_.size()); ++i) {
                env->SetObjectArrayElement(
                    events, i, this->events_received_[i]);
            }

            env->CallVoidMethod(this->script_,
                                this->process_events_,
                                jint(this->events_received_.size()),
                                events);
            env->ExceptionClear();
        }

        for (std::vector<jobject>::const_iterator event =
                 this->events_received_.begin();
             event != this->events_received_.end();
             ++event) {
            env->DeleteGlobalRef(*event);
        }

        this->events_received_.clear();

        env->CallVoidMethod(this->script_, this->events_processed_, timestamp);
        env->ExceptionClear();

        this->update_fields(*env);
        this->emit_events(*env);
    }

    /**
     * @brief Called when the script is deleted or the world containing
     * the script is unloaded or replaced
     *
     * @param[in] timestamp Time at which script is being destroyed.
     */
    void script::do_shutdown(double /* timestamp */)
    {
        JNIEnv * env;
        jint result =
            vm->AttachCurrentThread(reinterpret_cast<void **>(&env), 0);
        if (result != 0) {
            throw std::runtime_error("failed to attach to the current thread "
                                     "when shutting down script");
        }

        const jmethodID shutdown =
            env->GetMethodID(this->script_class_, "shutdown", "()V");
        if (!shutdown) {
            env->ExceptionClear();
            throw std::runtime_error("could not get Script class shutdown() "
                                     "method identifier");
        }

        env->CallVoidMethod(this->script_, shutdown);
        env->ExceptionClear();

        this->update_fields(*env);
        this->emit_events(*env);
    }

    void script::update_fields(JNIEnv & env)
    {
        const jmethodID updateFields =
            env.GetMethodID(this->script_class_, "updateFields", "()V");
        if (!updateFields) {
            env.ExceptionClear();
            throw std::runtime_error("could not get "
                                     "vrml.node.Script.updateFields() method "
                                     "identifier");
        }
        env.CallVoidMethod(this->script_, updateFields);
        env.ExceptionClear();
    }

    void script::emit_events(JNIEnv & env)
    {
        const jmethodID emitEvents =
            env.GetMethodID(this->script_class_, "emitEvents", "()V");
        if (!emitEvents) {
            env.ExceptionClear();
            throw std::runtime_error("could not get "
                                     "vrml.node.Script.emitEvents() method "
                                     "identifier");
        }
        env.CallVoidMethod(this->script_, emitEvents);
        env.ExceptionClear();
    }


    /**
     * @class script_factory
     *
     * @brief Factory for @c script%s.
     */

    /**
     * @brief Destroy.
     */
    script_factory::~script_factory() OPENVRML_NOTHROW
    {}

    /**
     * @brief Create a @c script.
     *
     * @param[in] node
     * @param[in] source
     *
     * @return a @c script.
     */
    std::auto_ptr<openvrml::script>
    script_factory::
    create_script(openvrml::script_node & node,
                  const boost::shared_ptr<openvrml::resource_istream> & source)
    {
        return std::auto_ptr<openvrml::script>(new script(node, source));
    }

    /**
     * @brief Convenience function to throw @c java.lang.OutOfMemoryError.
     *
     * We need to catch @c std::bad_alloc wherever we allocate memory and
     * convert it to @c java.lang.OutOfMemoryError on the Java side of the
     * fence.
     *
     * @param[in] env   JNIEnv reference.
     * @param[in] msg   exception message.
     */
    void throw_out_of_memory(JNIEnv & env, const char * const msg)
    {
        env.ExceptionClear();
        const jclass exception_class =
            env.FindClass("java/lang/OutOfMemoryError");
        if (!exception_class) {
            // Presumably FindClass raised an exception.
            return;
        }
        env.ThrowNew(exception_class, msg);
    }

    /**
     * @brief Convenience function to throw
     *        @c java.lang.ArrayIndexOutOfBoundsException.
     *
     * @param[in] env   JNIEnv reference.
     * @param[in] msg   exception message.
     */
    void throw_array_index_out_of_bounds(JNIEnv & env, const char * const msg)
    {
        env.ExceptionClear();
        const jclass exception_class =
            env.FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exception_class) {
            // Presumably FindClass raised an exception.
            return;
        }
        env.ThrowNew(exception_class, msg);
    }

    /**
     * @brief Convenience function to get the C++ &ldquo;peer&rdquo; object
     *        associated with a Java object.
     *
     * This function assumes that the peer pointer is stored in a
     * @c long member named &ldquo;peer&rdquo;.
     */
    template <typename FieldValue>
    OPENVRML_JAVA_LOCAL FieldValue & get_Field_peer(JNIEnv & env,
                                                    const jobject obj)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
        scope_guard local_frame_guard =
            make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
        boost::ignore_unused_variable_warning(local_frame_guard);

        jclass clazz;
# ifndef NDEBUG
        clazz = env.FindClass("vrml/Field");
        if (!clazz) {
            throw std::runtime_error("failed to find vrml.Field class");
        }
# endif
        assert(obj);
        assert(env.IsInstanceOf(obj, clazz));

        clazz = env.GetObjectClass(obj);

        const jfieldID id = env.GetFieldID(clazz, "peer", "J");
        if (!id) {
            throw std::runtime_error("failed to get vrml.Field.peer field "
                                     "identifier");
        }

        FieldValue * const peer =
            boost::polymorphic_downcast<FieldValue *>(
                reinterpret_cast<openvrml::field_value *>(
                    env.GetLongField(obj, id)));
        if (!peer) {
            throw std::runtime_error("invalid vrml.Field.peer");
        }

        return *peer;
    }

    OPENVRML_JAVA_LOCAL openvrml::browser &
    get_Browser_peer(JNIEnv & env, const jobject obj)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
        scope_guard local_frame_guard =
            make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
        boost::ignore_unused_variable_warning(local_frame_guard);

        jclass clazz;
# ifndef NDEBUG
        clazz = env.FindClass("vrml/Browser");
# endif
        assert(obj);
        assert(env.IsInstanceOf(obj, clazz));

        clazz = env.GetObjectClass(obj);
        const jfieldID id = env.GetFieldID(clazz, "peer", "J");
        if (!id) {
            throw std::runtime_error("failed to get vrml.Browser.peer field "
                                     "identifier");
        }

        openvrml::browser * const peer =
            reinterpret_cast<openvrml::browser *>(env.GetLongField(obj, id));
        if (!peer) {
            throw std::runtime_error("invalid vrml.Browser.peer");
        }

        return *peer;
    }
} // namespace

/**
 * @brief Implementation of @c vrml.Browser.getName().
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 * @return @c "OpenVRML".
 */
jstring JNICALL Java_vrml_Browser_getName(JNIEnv * const env,
                                          const jobject obj)
{
    try {
        openvrml::browser & browser = get_Browser_peer(*env, obj);
        return env->NewStringUTF(browser.name());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

/**
 * @brief Implementation of @c vrml.Browser.getVersion().
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 * @return the version of OpenVRML being used.
 */
jstring JNICALL Java_vrml_Browser_getVersion(JNIEnv * const env,
                                             const jobject obj)
{
    try {
        openvrml::browser & browser = get_Browser_peer(*env, obj);
        return env->NewStringUTF(browser.version());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

/**
 * @brief Implementation of Browser::getCurrentSpeed.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 * @return average navigation speed in m/s.
 */
jfloat JNICALL Java_vrml_Browser_getCurrentSpeed(JNIEnv * const env,
                                                 const jobject obj)
{
    try {
        openvrml::browser & browser = get_Browser_peer(*env, obj);
        return browser.current_speed();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

/**
 * @brief Implementation of Browser::getCurrentFrameRate.
 *
 * Returns the current frame rate in frames per second.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 *
 * @return the current frame rate.
 */
jfloat JNICALL Java_vrml_Browser_getCurrentFrameRate(JNIEnv * const env,
                                                     const jobject obj)
{
    try {
        openvrml::browser & browser = get_Browser_peer(*env, obj);
        return browser.frame_rate();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

/**
 * @brief Implementation of Browser::getWorldURL.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 * @return URL of current world displayed in browser.
 */
jstring JNICALL Java_vrml_Browser_getWorldURL(JNIEnv * const env,
                                              const jobject obj)
{
    try {
        openvrml::browser & browser = get_Browser_peer(*env, obj);
        return env->NewStringUTF(browser.world_url().c_str());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

/**
 * @brief Implementation of Browser::replaceWorld.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj JNI version of a Java Browser object.
 * @param[in] basenodes ...
 *
 * @todo Implement me!
 */
void JNICALL Java_vrml_Browser_replaceWorld(JNIEnv * /* env */,
                                            jobject /* obj */,
                                            jobjectArray /* basenodes */)
{
  // First check if mustEvaluate is set, if not then method
  // should not continue.
}

namespace {

    OPENVRML_JAVA_LOCAL jobject
    create_Node(JNIEnv & env, const boost::intrusive_ptr<openvrml::node> & node)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        using boost::ref;

        //
        // We can safely run DeleteGlobalRef in the scope guard because
        // calling DeleteGlobalRef with 0 is a no-op.
        //
        jobject result_global_ref = 0;
        scope_guard result_global_ref_guard =
            make_obj_guard(env,
                           &JNIEnv::DeleteGlobalRef,
                           ref(result_global_ref));
        boost::ignore_unused_variable_warning(result_global_ref_guard);
        {
            using std::auto_ptr;
            using boost::intrusive_ptr;

            if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            const jclass clazz = env.FindClass("vrml/node/NodeImpl");
            if (!clazz) {
                throw std::runtime_error("could not find class "
                                         "vrml.node.NodeImpl");
            }

            const jmethodID ctor = env.GetMethodID(clazz, "<init>", "(J)V");
            if (!ctor) {
                throw std::runtime_error("failed to get ID for "
                                         "vrml.node.NodeImpl constructor");
            }

            auto_ptr<intrusive_ptr<openvrml::node> >
                peer(new intrusive_ptr<openvrml::node>(node));

            const jobject node_obj = env.NewObject(clazz, ctor, peer.release());
            if (!node_obj) {
                throw std::runtime_error("could not create vrml.node.NodeImpl");
            }

            //
            // Create a global reference so that the new object will outlive
            // the local frame.
            //
            result_global_ref = env.NewGlobalRef(node_obj);
            if (!result_global_ref) { throw std::bad_alloc(); }
        }

        //
        // NewLocalRef does not throw any Java exceptions.
        //
        const jobject result = env.NewLocalRef(result_global_ref);
        if (!result) { throw std::bad_alloc(); }

        return result;
    }

    OPENVRML_JAVA_LOCAL jobjectArray
    create_BaseNode_array(
        JNIEnv & env,
        std::vector<boost::intrusive_ptr<openvrml::node> > & nodes)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        using boost::ref;

        //
        // We can safely run DeleteGlobalRef in the scope guard because
        // calling DeleteGlobalRef with 0 is a no-op.
        //
        jobjectArray result_global_ref = 0;
        scope_guard result_global_ref_guard =
            make_obj_guard(env,
                           &JNIEnv::DeleteGlobalRef,
                           ref(result_global_ref));
        boost::ignore_unused_variable_warning(result_global_ref_guard);
        {
            using boost::intrusive_ptr;

            if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            const jclass clazz = env.FindClass("vrml/BaseNode");
            if (!clazz) {
                throw std::runtime_error("could not find class vrml.BaseNode");
            }

            const jobjectArray result =
                env.NewObjectArray(nodes.size(), clazz, 0);
            if (!result) { throw std::bad_alloc(); }

            for (std::vector<intrusive_ptr<openvrml::node> >::size_type i = 0;
                 i != nodes.size();
                 ++i) {
                if (env.PushLocalFrame(1) < 0) { throw std::bad_alloc(); }
                scope_guard local_frame_guard =
                    make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
                boost::ignore_unused_variable_warning(local_frame_guard);
                env.SetObjectArrayElement(result, i,
                                          create_Node(env, nodes[i]));
            }

            //
            // Create a global reference so that the new object will outlive
            // the local frame.
            //
            result_global_ref =
                static_cast<jobjectArray>(env.NewGlobalRef(result));
            if (!result_global_ref) { throw std::bad_alloc(); }
        }

        //
        // NewLocalRef does not throw any Java exceptions.
        //
        const jobjectArray result =
            static_cast<jobjectArray>(env.NewLocalRef(result_global_ref));
        if (!result) { throw std::bad_alloc(); }

        return result;
    }
}

/**
 * @brief Implementation of @c vrml.Browser.createVrmlFromString()
 *
 * @param[in] env JNI       environment
 * @param[in] obj JNI       version of a Java Browser object.
 * @param[in] vrmlSyntax    a Java String containing VRML syntax.
 *
 * @return an @c MFNode containing the result of parsing @p vrmlSyntax.
 */
jobjectArray JNICALL
Java_vrml_Browser_createVrmlFromString(JNIEnv * const env,
                                       const jobject obj,
                                       const jstring vrmlSyntax)
{
    try {
        using std::vector;
        using boost::intrusive_ptr;

        openvrml::browser & b = get_Browser_peer(*env, obj);

        const char * const vrmlSyntax_chars =
            env->GetStringUTFChars(vrmlSyntax, 0);
        scope_guard vrmlSyntax_chars_guard =
            make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars,
                           vrmlSyntax, vrmlSyntax_chars);
        boost::ignore_unused_variable_warning(vrmlSyntax_chars_guard);

        std::istringstream in(vrmlSyntax_chars);
        try {
            vector<intrusive_ptr<openvrml::node> > nodes =
                b.create_vrml_from_stream(in);
            return create_BaseNode_array(*env, nodes);
        } catch (openvrml::invalid_vrml & ex) {
            env->ExceptionClear();
            const jclass clazz =
                env->FindClass("vrml/InvalidVRMLSyntaxException");
            if (!clazz) { return 0; }
            env->ThrowNew(clazz, ex.what());
        }
    } catch (std::bad_alloc & ex) {
        env->ExceptionClear();
        throw_out_of_memory(*env, ex.what());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

/**
 * @brief Implementation of Browser::createVrmlFromURL.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 * @param[in] urls ...
 * @param[in] basenode ...
 * @param[in] event ...
 *
 * @todo Implement me!  This method should throw an
 *       InvalidVRMLSyntaxException if the syntax is invalid.
 */
void JNICALL Java_vrml_Browser_createVrmlFromURL(JNIEnv * /* env */,
                                                 jobject /* obj */,
                                                 jobjectArray /* urls */,
                                                 jobject /* basenode */,
                                                 jstring /* event */)
{}

namespace {

    OPENVRML_JAVA_LOCAL script &
    get_Script_peer(JNIEnv & env, const jobject obj)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
        scope_guard local_frame_guard =
            make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
        boost::ignore_unused_variable_warning(local_frame_guard);

# ifndef NDEBUG
        const jclass script_class = env.FindClass("vrml/node/Script");
# endif
        assert(script_class);
        assert(obj);
        assert(env.IsInstanceOf(obj, script_class));

        const jclass obj_class = env.GetObjectClass(obj);

        const jfieldID peer_id = env.GetFieldID(obj_class, "scriptPeer", "J");
        if (!peer_id) {
            throw std::runtime_error("failed to get "
                                     "vrml.node.Script.scriptPeer field "
                                     "identifier");
        }

        script * const peer =
            reinterpret_cast<script *>(env.GetLongField(obj, peer_id));
        if (!peer) {
            throw std::runtime_error("invalid vrml.node.Script.scriptPeer");
        }

        return *peer;
    }

    OPENVRML_JAVA_LOCAL const boost::intrusive_ptr<openvrml::node> &
    get_BaseNode_peer(JNIEnv & env, const jobject obj)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        if (env.PushLocalFrame(2) < 0) { throw std::bad_alloc(); }
        scope_guard local_frame_guard =
            make_obj_guard(env, &JNIEnv::PopLocalFrame, jobject(0));
        boost::ignore_unused_variable_warning(local_frame_guard);

# ifndef NDEBUG
        const jclass base_node_class = env.FindClass("vrml/BaseNode");
# endif
        assert(base_node_class);
        assert(obj);
        assert(env.IsInstanceOf(obj, base_node_class));

        const jclass clazz = env.GetObjectClass(obj);

        const jfieldID peer_id = env.GetFieldID(clazz, "peer", "J");
        if (!peer_id) {
            throw std::runtime_error("failed to get vrml.BaseNode.peer field "
                                     "identifier");
        }

        const boost::intrusive_ptr<openvrml::node> * const peer =
            reinterpret_cast<const boost::intrusive_ptr<openvrml::node> *>(
                env.GetLongField(obj, peer_id));
        if (!peer) {
            throw std::runtime_error("invalid vrml.BaseNode.peer");
        }

        return *peer;
    }
}

/**
 * @brief Implementation of vrml.Browser.addRoute().
 *
 * @param[in] env           JNI environment
 * @param[in,out] obj       JNI version of a Java Browser object.
 * @param[in] fromNode      the event source BaseNode.
 * @param[in] fromEventOut  the eventOut identifier.
 * @param[in] toNode        the event destination BaseNode.
 * @param[in] toEventIn     the eventIn identifier.
 */
void JNICALL Java_vrml_Browser_addRoute(JNIEnv * const env,
                                        const jobject obj,
                                        const jobject fromNode,
                                        const jstring fromEventOut,
                                        const jobject toNode,
                                        const jstring toEventIn)
{
    try {
        using boost::intrusive_ptr;

        script & s = get_Script_peer(*env, obj);
        //
        // If directOutput is not TRUE, this is a no-op.
        //
        if (!s.direct_output()) { return; }

        const intrusive_ptr<openvrml::node> & from =
            get_BaseNode_peer(*env, fromNode);
        assert(from);

        const char * const eventout_id =
            env->GetStringUTFChars(fromEventOut, 0);
        if (!eventout_id) { return; }
        scope_guard eventout_id_guard =
            make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars,
                           fromEventOut, eventout_id);
        boost::ignore_unused_variable_warning(eventout_id_guard);

        const intrusive_ptr<openvrml::node> & to =
            get_BaseNode_peer(*env, toNode);
        assert(to);

        const char * const eventin_id = env->GetStringUTFChars(toEventIn, 0);
        if (!eventin_id) { return; }
        scope_guard eventin_id_guard =
            make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars,
                           toEventIn, eventin_id);
        boost::ignore_unused_variable_warning(eventin_id_guard);

        openvrml::add_route(*from, eventout_id, *to, eventin_id);
    } catch (std::logic_error & ex) {
        env->ExceptionClear();
        const jclass clazz = env->FindClass("vrml/InvalidRouteException");
        if (!clazz) { return; }
        env->ThrowNew(clazz, ex.what());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of vrml.Browser.deleteRoute().
 *
 * @param[in] env           JNI environment
 * @param[in,out] obj       JNI version of a Java Browser object.
 * @param[in] fromNode      the event source BaseNode.
 * @param[in] fromEventOut  the eventOut identifier.
 * @param[in] toNode        the event destination BaseNode.
 * @param[in] toEventIn     the eventIn identifier.
 */
void JNICALL Java_vrml_Browser_deleteRoute(JNIEnv * const env,
                                           const jobject obj,
                                           const jobject fromNode,
                                           const jstring fromEventOut,
                                           const jobject toNode,
                                           const jstring toEventIn)
{
    try {
        using boost::intrusive_ptr;

        script & s = get_Script_peer(*env, obj);
        //
        // If directOutput is not TRUE, this is a no-op.
        //
        if (!s.direct_output()) { return; }

        const intrusive_ptr<openvrml::node> & from =
            get_BaseNode_peer(*env, fromNode);
        assert(from);

        const char * const eventout_id =
            env->GetStringUTFChars(fromEventOut, 0);
        if (!eventout_id) { return; }
        scope_guard eventout_id_guard =
            make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars,
                           fromEventOut, eventout_id);
        boost::ignore_unused_variable_warning(eventout_id_guard);

        const intrusive_ptr<openvrml::node> & to =
            get_BaseNode_peer(*env, toNode);
        assert(to);

        const char * const eventin_id = env->GetStringUTFChars(toEventIn, 0);
        if (!eventin_id) { return; }
        scope_guard eventin_id_guard =
            make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars,
                           toEventIn, eventin_id);
        boost::ignore_unused_variable_warning(eventin_id_guard);

        openvrml::delete_route(*from, eventout_id, *to, eventin_id);
    } catch (openvrml::unsupported_interface & ex) {
        env->ExceptionClear();
        const jclass clazz = env->FindClass("vrml/InvalidRouteException");
        if (!clazz) { return; }
        env->ThrowNew(clazz, ex.what());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.Browser.loadURL().
 *
 * This method is specified to throw @c InvalidVRMLSyntaxException if the
 * resolving URI yields a stream with incorrect VRML syntax; however, the
 * implementation of this method will never throw such an exception.  The
 * implementation delegates to @c openvrml::scene::load_url, which does not
 * block.  Given the nature of this function--i.e., the associated Script node
 * will almost certainly become unusable upon its completion--the value of
 * throwing @c InvalidVRMLSyntaxException is questionable.
 *
 * @param[in] env       JNI environment.
 * @param[in,out] obj   JNI version of a Java Browser object.
 * @param[in] url       URL to load with alternates.
 * @param[in] parameter array of parameters to load URL with.
 */
void JNICALL Java_vrml_Browser_loadURL(JNIEnv * const env,
                                       const jobject obj,
                                       const jobjectArray url,
                                       const jobjectArray parameter)
{
    try {
        using std::string;
        using std::vector;

        script & s = get_Script_peer(*env, obj);
        //
        // If directOutput is not TRUE, this is a no-op.
        //
        if (!s.must_evaluate()) { return; }

        vector<string> url_vec(env->GetArrayLength(url));
        for (vector<string>::size_type i = 0; i < url_vec.size(); ++i) {
            jstring jstr =
                static_cast<jstring>(env->GetObjectArrayElement(url, i));
            if (env->ExceptionCheck()) {
                return; // ArrayIndexOutOfBoundsException
            }
            const char * str = env->GetStringUTFChars(jstr, 0);
            if (!str) { return; } // OutOfMemoryError
            scope_guard str_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars, jstr, str);
            boost::ignore_unused_variable_warning(str_guard);
            url_vec[i] = str; // Throws std::bad_alloc.
        }

        vector<string> param_vec(env->GetArrayLength(parameter)); // Throws std::bad_alloc.
        for (vector<string>::size_type i = 0; i < param_vec.size(); ++i) {
            jstring jstr =
                static_cast<jstring>(env->GetObjectArrayElement(parameter, i));
            if (env->ExceptionCheck()) {
                return; // ArrayIndexOutOfBoundsException
            }
            const char * str = env->GetStringUTFChars(jstr, 0);
            if (!str) { return; } // OutOfMemoryError
            scope_guard str_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars, jstr, str);
            boost::ignore_unused_variable_warning(str_guard);
            param_vec[i] = str; // Throws std::bad_alloc.
        }

        s.node.scene()->load_url(url_vec, param_vec);
    } catch (std::bad_alloc & ex) {
        if (!env->ExceptionCheck()) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of Browser::setDescription.
 *
 * @param[in]     env JNI environment
 * @param[in,out] obj JNI version of a Java Browser object.
 * @param[in]     description Desired description of Browser.
 */
void JNICALL Java_vrml_Browser_setDescription(JNIEnv * const env,
                                              const jobject obj,
                                              const jstring description)
{
    const char * desc = env->GetStringUTFChars(description, 0);
    if (!desc) { return; } // OutOfMemoryError
    scope_guard desc_guard =
        make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars, description, desc);
    boost::ignore_unused_variable_warning(desc_guard);
    try {
        openvrml::browser & browser = get_Browser_peer(*env, obj);
        browser.description(desc);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.BaseNode.getType().
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java node object
 * @return Type of node
 */
jstring JNICALL Java_vrml_BaseNode_getType(JNIEnv * env, jobject obj)
{
    try {
        const boost::intrusive_ptr<openvrml::node> & node =
            get_BaseNode_peer(*env, obj);
        assert(node.get());
        return env->NewStringUTF(node->type().id().c_str());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

/**
 * @brief Implementation of @c vrml.BaseNode.dispose().
 *
 * @param[in] env   JNI environment.
 * @param[in] clazz Java vrml.BaseNode class.
 * @param[in] peer  Java @c long representation of the pointer to the peer
 *                  node instance.
 */
void JNICALL Java_vrml_BaseNode_dispose(JNIEnv *, jclass, const jlong peer)
{
    delete reinterpret_cast<boost::intrusive_ptr<openvrml::node> *>(peer);
}

/**
 * @brief Implementation of @c vrml.BaseNode.toString().
 *
 * @param[in] env   JNI environment.
 * @param[in] obj   BaseNode instance.
 */
jstring JNICALL Java_vrml_BaseNode_toString(JNIEnv * const env,
                                            const jobject obj)
{
    try {
        const boost::intrusive_ptr<openvrml::node> & node =
            get_BaseNode_peer(*env, obj);
        assert(node.get());
        std::ostringstream out;
        out << *node;
        return env->NewStringUTF(out.str().c_str());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

void JNICALL Java_vrml_node_Script_updateField(JNIEnv * const env,
                                               const jobject obj,
                                               const jstring id,
                                               const jobject value)
{
    const char * id_chars = env->GetStringUTFChars(id, 0);
    if (!id_chars) { return; }
    scope_guard id_chars_guard =
        make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars, id, id_chars);
    boost::ignore_unused_variable_warning(id_chars_guard);

    try {
        using openvrml::field_value;
        script & s = get_Script_peer(*env, obj);
        const field_value & fv = get_Field_peer<field_value>(*env, value);
        s.field(id_chars, fv);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_node_Script_emitEvent(JNIEnv * const env,
                                             const jobject obj,
                                             const jstring id,
                                             const jobject value)
{
    const char * id_chars = env->GetStringUTFChars(id, 0);
    if (!id_chars) { return; }
    scope_guard id_chars_guard =
        make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars, id, id_chars);
    boost::ignore_unused_variable_warning(id_chars_guard);

    try {
        using std::find_if;
        using openvrml::field_value;
        script & s = get_Script_peer(*env, obj);

        const openvrml::node_interface_set & interfaces =
            s.node.type().interfaces();
        const openvrml::node_interface_set::const_iterator interface_ =
            find_if(interfaces.begin(), interfaces.end(),
                    bind2nd(openvrml::node_interface_matches_eventout(),
                            id_chars));
        assert(interface_ != interfaces.end());

        const openvrml::script_node::eventout_map_t::const_iterator eventout =
            s.node.eventout_map().find(id_chars);

        const field_value & fv = get_Field_peer<field_value>(*env, value);
        eventout->second->value(fv);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.Field.dispose().
 */
void JNICALL Java_vrml_Field_dispose(JNIEnv *, jclass, const jlong peer)
{
    delete reinterpret_cast<openvrml::field_value *>(peer);
}

/**
 * @brief Implementation of @c vrml.Field.clone().
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a VRML field object.
 * @return Cloned copy of JNI version of a VRML field object.
 */
jobject JNICALL Java_vrml_Field_clone(JNIEnv * const env, const jobject obj)
{
    using openvrml::field_value;

    const jfieldID peer_id =
        env->GetFieldID(env->GetObjectClass(obj), "peer", "J");
    if (!peer_id) { return 0; }

    const field_value * const peer =
        reinterpret_cast<const field_value *>(env->GetLongField(obj, peer_id));
    if (!peer) { return 0; }

    return clone_Field(*env, *peer);
}

/**
 * @brief Implementation of @c vrml.Field.toString().
 *
 * @param[in] env JNI environment
 * @param[in] obj Java vrml.Field instance.
 * @return a string representation of the Field.
 */
jstring JNICALL Java_vrml_Field_toString(JNIEnv * const env, const jobject obj)
{
    try {
        using openvrml::field_value;
        const field_value & fv = get_Field_peer<field_value>(*env, obj);
        std::ostringstream out;
        out << fv;
        return env->NewStringUTF(out.str().c_str());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

jboolean JNICALL Java_vrml_field_ConstSFBool_getValue(JNIEnv * const env,
                                                      const jobject obj)
{
    try {
        using openvrml::sfbool;
        const sfbool & peer = get_Field_peer<sfbool>(*env, obj);
        return peer.value();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return false;
}

/**
 * @brief Implementation of @c vrml.field.SFBool.createPeer().
 *
 * @param[in] env   JNI environment.
 * @param[in] clazz SFBool class.
 * @param[in] value initial value of SFBool object.
 */
jlong JNICALL Java_vrml_field_SFBool_createPeer(JNIEnv * const env,
                                                jclass,
                                                const jboolean value)
{
    std::auto_ptr<openvrml::field_value> peer;
    try {
        peer.reset(new openvrml::sfbool(value));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of @c vrml.field.SFBool.getValue().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFBool object
 * @return Value of SFBool object.
 */
jboolean JNICALL Java_vrml_field_SFBool_getValue(JNIEnv * const env,
                                                 const jobject obj)
{
    return Java_vrml_field_ConstSFBool_getValue(env, obj);
}

/**
 * @brief Implementation of @c vrml.field.SFBool.setValue().
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFBool object
 * @param[in] value Desired value of SFBool object.
 */
void JNICALL Java_vrml_field_SFBool_setValue__Z(JNIEnv * const env,
                                                const jobject obj,
                                                const jboolean value)
{
    try {
        using openvrml::field_value;
        using openvrml::sfbool;
        sfbool & fv = get_Field_peer<sfbool>(*env, obj);
        fv.value(value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.field.ConstSFColor.getValue().
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFColor object
 * @param[out] jarr Array to fill with RGB values of SFColor.
 */
void JNICALL Java_vrml_field_ConstSFColor_getValue(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jfloatArray jarr)
{
    try {
        using openvrml::sfcolor;
        const sfcolor & peer = get_Field_peer<sfcolor>(*env, obj);

        openvrml::color c = peer.value();
        env->SetFloatArrayRegion(jarr, 0, 3, c.rgb);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.field.ConstSFColor.getRed().
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFColor object
 * @return Red component of SFColor
 */
jfloat JNICALL Java_vrml_field_ConstSFColor_getRed(JNIEnv * const env,
                                                   const jobject obj)
{
    try {
        using openvrml::sfcolor;
        const sfcolor & peer = get_Field_peer<sfcolor>(*env, obj);
        return peer.value().r();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

/**
 * @brief Implementation of @c vrml.field.ConstSFColor.getGreen().
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFColor object
 * @return Green component of SFColor
 */
jfloat JNICALL Java_vrml_field_ConstSFColor_getGreen(JNIEnv * const env,
                                                     const jobject obj)
{
    try {
        using openvrml::sfcolor;
        const sfcolor & peer = get_Field_peer<sfcolor>(*env, obj);
        return peer.value().g();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

/**
 * @brief Implementation of @c vrml.field.ConstSFColor.getBlue().
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFColor object
 * @return Blue component of SFColor
 */
jfloat JNICALL Java_vrml_field_ConstSFColor_getBlue(JNIEnv * const env,
                                                    const jobject obj)
{
    try {
        using openvrml::sfcolor;
        const sfcolor & peer = get_Field_peer<sfcolor>(*env, obj);
        return peer.value().g();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

/**
 * @brief Implementation of @c vrml.field.SFColor.createPeer().
 *
 * @param[in] env   JNI environment.
 * @param[in] clazz SFColor class.
 * @param[in] r     red component of Color
 * @param[in] g     green component of Color
 * @param[in] b     blue component of Color
 */
jlong JNICALL Java_vrml_field_SFColor_createPeer(JNIEnv * const env,
                                                 jclass,
                                                 const jfloat r,
                                                 const jfloat g,
                                                 const jfloat b)
{
    std::auto_ptr<openvrml::field_value> peer;
    try {
        peer.reset(new openvrml::sfcolor(openvrml::make_color(r, g, b)));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of @c vrml.field.SFColor.getValue().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFColor object
 * @param[out] jarr Array to populate with SFColor RGB components.
 */
void JNICALL Java_vrml_field_SFColor_getValue(JNIEnv * const env,
                                              const jobject obj,
                                              const jfloatArray jarr)
{
    Java_vrml_field_ConstSFColor_getValue(env, obj, jarr);
}

/**
 * @brief Implementation of @c vrml.field.SFColor.getRed().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFColor object
 * @return Red component of SFColor.
 */
jfloat JNICALL Java_vrml_field_SFColor_getRed(JNIEnv * const env,
                                              const jobject obj)
{
    return Java_vrml_field_ConstSFColor_getRed(env, obj);
}

/**
 * @brief Implementation of @c vrml.field.SFColor.getGreen().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFColor object
 * @return Green component of SFColor.
 */
jfloat JNICALL Java_vrml_field_SFColor_getGreen(JNIEnv * const env,
                                                const jobject obj)
{
    return Java_vrml_field_ConstSFColor_getGreen(env, obj);
}

/**
 * @brief Implementation of @c vrml.field.SFColor.getBlue().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFColor object
 * @return Blue component of SFColor.
 */
jfloat JNICALL Java_vrml_field_SFColor_getBlue(JNIEnv * const env,
                                               const jobject obj)
{
    return Java_vrml_field_ConstSFColor_getBlue(env, obj);
}

/**
 * @brief Implementation of @c vrml.field.SFColor.setValue().
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFColor object
 * @param[in] r Red component of desired color.
 * @param[in] g Green component of desired color.
 * @param[in] b Blue component of desired color.
 */
void JNICALL Java_vrml_field_SFColor_setValue__FFF(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jfloat r,
                                                   const jfloat g,
                                                   const jfloat b)
{
    try {
        using openvrml::sfcolor;
        sfcolor & peer = get_Field_peer<sfcolor>(*env, obj);
        peer.value(openvrml::make_color(r, g, b));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.field.ConstSFFloat.getValue().
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFFloat object
 * @return Value of field.
 */
jfloat JNICALL Java_vrml_field_ConstSFFloat_getValue(JNIEnv * const env,
                                                     const jobject obj)
{
    try {
        using openvrml::sffloat;
        sffloat & peer = get_Field_peer<sffloat>(*env, obj);
        return peer.value();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

/**
 * @brief Implementation of @c vrml.field.SFFloat.createPeer().
 *
 * @param[in] env   JNI environment.
 * @param[in] clazz SFFloat class.
 * @param[in] value Initial value for object.
 */
jlong JNICALL Java_vrml_field_SFFloat_createPeer(JNIEnv * const env,
                                                 jclass,
                                                 const jfloat value)
{
    std::auto_ptr<openvrml::field_value> peer;
    try {
        peer.reset(new openvrml::sffloat(value));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of @c vrml.field.SFFloat.getValue().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFFloat object
 * @return Value of field.
 */
jfloat JNICALL Java_vrml_field_SFFloat_getValue(JNIEnv * const env,
                                                const jobject obj)
{
    return Java_vrml_field_ConstSFFloat_getValue(env, obj);
}

/**
 * @brief Implementation of SFFloat::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFFloat object
 * @param[in] f Desired value
 */
void JNICALL Java_vrml_field_SFFloat_setValue__F(JNIEnv * const env,
                                                 const jobject obj,
                                                 const jfloat f)
{
    try {
        using openvrml::sffloat;
        sffloat & peer = get_Field_peer<sffloat>(*env, obj);
        peer.value(f);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.field.ConstSFDouble.getValue().
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFDouble object
 * @return Value of field.
 */
jdouble JNICALL Java_vrml_field_ConstSFDouble_getValue(JNIEnv * const env,
                                                       const jobject obj)
{
    try {
        using openvrml::sfdouble;
        sfdouble & peer = get_Field_peer<sfdouble>(*env, obj);
        return peer.value();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0;
}

/**
 * @brief Implementation of @c vrml.field.SFDouble.createPeer().
 *
 * @param[in] env   JNI environment.
 * @param[in] clazz SFDouble class.
 * @param[in] value Initial value for object.
 */
jlong JNICALL Java_vrml_field_SFDouble_createPeer(JNIEnv * const env,
                                                  jclass,
                                                  const jdouble value)
{
    std::auto_ptr<openvrml::field_value> peer;
    try {
        peer.reset(new openvrml::sfdouble(value));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of @c vrml.field.SFDouble.getValue().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFDouble object
 * @return Value of field.
 */
jdouble JNICALL Java_vrml_field_SFDouble_getValue(JNIEnv * const env,
                                                  const jobject obj)
{
    return Java_vrml_field_ConstSFDouble_getValue(env, obj);
}

/**
 * @brief Implementation of @c vrml.field.SFDouble.setValue().
 *
 * @param[in] env       JNI environment.
 * @param[in,out] obj   SFDouble object.
 * @param[in] value     new value.
 */
void JNICALL Java_vrml_field_SFDouble_setValue__F(JNIEnv * const env,
                                                  const jobject obj,
                                                  const jdouble value)
{
    try {
        using openvrml::sfdouble;
        sfdouble & peer = get_Field_peer<sfdouble>(*env, obj);
        peer.value(value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.field.ConstSFImage.getWidth().
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFImage object
 * @return Width of ConstSFImage
 */
jint JNICALL Java_vrml_field_ConstSFImage_getWidth(JNIEnv * const env,
                                                   const jobject obj)
{
    try {
        using openvrml::sfimage;
        sfimage & peer = get_Field_peer<sfimage>(*env, obj);
        return peer.value().x();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

/**
 * @brief Implementation of @c vrml.field.ConstSFImage.getHeight().
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFImage object
 * @return Height of ConstSFImage
 */
jint JNICALL Java_vrml_field_ConstSFImage_getHeight(JNIEnv * const env,
                                                    const jobject obj)
{
    try {
        using openvrml::sfimage;
        sfimage & peer = get_Field_peer<sfimage>(*env, obj);
        return peer.value().y();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

/**
 * @brief Implementation of @c vrml.field.ConstSFImage.getComponents().
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFImage object
 * @return Number of components in ConstSFImage
 */
jint JNICALL Java_vrml_field_ConstSFImage_getComponents(JNIEnv * const env,
                                                        const jobject obj)
{
    try {
        using openvrml::sfimage;
        sfimage & peer = get_Field_peer<sfimage>(*env, obj);
        return peer.value().comp();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

/**
 * @brief Implementation of @c vrml.field.ConstSFImage.getPixels().
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFImage object
 * @param[out] pixels Array to fill with ConstSFImage pixels
 */
void JNICALL Java_vrml_field_ConstSFImage_getPixels(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jbyteArray pixels)
{
    try {
        using std::vector;
        using openvrml::sfimage;
        sfimage & peer = get_Field_peer<sfimage>(*env, obj);
        try {
            //
            // It's annoying to have to copy this; however, since
            // field_values have copy-on-write semantics, it's the only
            // safe thing.
            //
            vector<unsigned char> image_pixels = peer.value().array();
            env->SetByteArrayRegion(
                pixels, 0, image_pixels.size(),
                image_pixels.empty()
                    ? 0
                    : reinterpret_cast<jbyte *>(&image_pixels.front()));
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.field.SFImage.createPeer().
 *
 * @param[in] env       JNI environment.
 * @param[in] clazz     SFImage class.
 * @param[in] width     Initial width.
 * @param[in] height    Initial height.
 * @param[in] comp      Initial number of components.
 * @param[in] pixels    Initial set of pixels.
 */
jlong JNICALL Java_vrml_field_SFImage_createPeer(JNIEnv * const env,
                                                 jclass,
                                                 const jint w,
                                                 const jint h,
                                                 const jint comp,
                                                 const jbyteArray pixels)
{
    using openvrml::image;
    using openvrml::sfimage;

    std::auto_ptr<openvrml::field_value> peer;
    try {
        if (pixels) {
            jbyte * const elements = env->GetByteArrayElements(pixels, 0);
            scope_guard elements_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseByteArrayElements,
                               pixels, elements, 0);
            boost::ignore_unused_variable_warning(elements_guard);
            peer.reset(
                new sfimage(
                    image(w, h, comp, elements, elements + (w * h * comp))));
        } else {
            peer.reset(new sfimage(image(w, h, comp)));
        }
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of @c vrml.field.SFImage.getWidth().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFImage object
 * @return Width of SFImage
 */
jint JNICALL Java_vrml_field_SFImage_getWidth(JNIEnv * const env,
                                              const jobject obj)
{
    return Java_vrml_field_ConstSFImage_getWidth(env, obj);
}

/**
 * @brief Implementation of @c vrml.field.SFImage.getHeight().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFImage object
 * @return Height of SFImage
 */
jint JNICALL Java_vrml_field_SFImage_getHeight(JNIEnv * const env,
                                               const jobject obj)
{
    return Java_vrml_field_ConstSFImage_getHeight(env, obj);
}

/**
 * @brief Implementation of @c vrml.field.SFImage.getComponents().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFImage object
 * @return Number of components in SFImage
 */
jint JNICALL Java_vrml_field_SFImage_getComponents(JNIEnv * const env,
                                                   const jobject obj)
{
    return Java_vrml_field_ConstSFImage_getComponents(env, obj);
}

/**
 * @brief Implementation of @c vrml.field.SFImage.getPixels().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFImage object
 * @param[out] pixels Array to fill with SFImage pixels
 */
void JNICALL Java_vrml_field_SFImage_getPixels(JNIEnv * const env,
                                               const jobject obj,
                                               const jbyteArray pixels)
{
    Java_vrml_field_ConstSFImage_getPixels(env, obj, pixels);
}

/**
 * @brief Implementation of @c vrml.field.SFImage.setValue().
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFImage object
 * @param[in] width New width
 * @param[in] height New height
 * @param[in] components Number of components
 * @param[in] pixels New set of pixels
 */
void JNICALL Java_vrml_field_SFImage_setValue__III_3B(JNIEnv * const env,
                                                      const jobject obj,
                                                      const jint width,
                                                      const jint height,
                                                      const jint components,
                                                      const jbyteArray pixels)
{
    try {
        using openvrml::image;
        using openvrml::sfimage;
        sfimage & peer = get_Field_peer<sfimage>(*env, obj);
        try {
            jbyte * const elements = env->GetByteArrayElements(pixels, NULL);
            scope_guard elements_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseByteArrayElements,
                               pixels, elements, 0);
            boost::ignore_unused_variable_warning(elements_guard);
            peer.value(image(width, height, components,
                             elements,
                             elements + (width * height * components)));
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.field.SFImage.setValue().
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFImage object
 * @param[in] arg ConstSFImage to copy value from
 */
void JNICALL
Java_vrml_field_SFImage_setValue__Lvrml_field_ConstSFImage_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject arg)
{
    try {
        using openvrml::sfimage;
        sfimage & peer = get_Field_peer<sfimage>(*env, obj);
        sfimage & arg_peer = get_Field_peer<sfimage>(*env, arg);
        try {
            peer = arg_peer;
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of SFImage::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFImage object
 * @param[in] value SFImage to copy value from
 */
void JNICALL
Java_vrml_field_SFImage_setValue__Lvrml_field_SFImage_2(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jobject image)
{
    Java_vrml_field_SFImage_setValue__Lvrml_field_ConstSFImage_2(
        env, obj, image);
}

/**
 * @brief Implementation of @c vrml.field.ConstSFInt32.getValue().
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFInt32 object
 * @return Value of field
 */
jint JNICALL Java_vrml_field_ConstSFInt32_getValue(JNIEnv * const env,
                                                   const jobject obj)
{
    try {
        using openvrml::sfint32;
        sfint32 peer = get_Field_peer<sfint32>(*env, obj);
        return peer.value();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

/**
 * @brief Implementation of @c vrml.field.SFInt32._create().
 *
 * @param[in] env   JNI environment.
 * @param[in] clazz SFInt32 class.
 * @param[in] value initial value.
 *
 * @return a pointer to the peer C++ object as a @c jlong.
 */
jlong JNICALL Java_vrml_field_SFInt32_createPeer(JNIEnv * const env,
                                                 jclass,
                                                 const jint value)
{
    std::auto_ptr<openvrml::field_value> peer;
    try {
        peer.reset(new openvrml::sfint32(value));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of @c vrml.field.SFInt32.getValue().
 *
 * @param[in] env JNI environment
 * @param[in] obj SFInt32 object
 * @return Value of field
 */
jint JNICALL Java_vrml_field_SFInt32_getValue(JNIEnv * const env,
                                              const jobject obj)
{
    return Java_vrml_field_ConstSFInt32_getValue(env, obj);
}

/**
 * @brief Implementation of @c vrml.field.SFInt32.setValue().
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFInt32 object
 * @param[in] value New value of SFInt32
 */
void JNICALL Java_vrml_field_SFInt32_setValue__I(JNIEnv * const env,
                                                 const jobject obj,
                                                 const jint value)
{
    try {
        using openvrml::sfint32;
        sfint32 & peer = get_Field_peer<sfint32>(*env, obj);
        peer.value(value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.field.SFNode.createPeer().
 *
 * @param[in] env JNI environment
 * @param[in] clazz SFNode class.
 * @param[in] value BaseNode object to initialize field with.
 */
jlong JNICALL Java_vrml_field_SFNode_createPeer(JNIEnv * const env,
                                                jclass,
                                                const jobject value)
{
    try {
        using boost::intrusive_ptr;

        std::auto_ptr<openvrml::field_value> peer;
        jfieldID field_id;

        if (!value) {
            peer.reset(new openvrml::sfnode);
        } else {
            field_id = env->GetFieldID(env->GetObjectClass(value), "peer", "J");
            if (!field_id) { return 0; }
            intrusive_ptr<openvrml::node> * const node_ptr =
                reinterpret_cast<intrusive_ptr<openvrml::node> *>(
                    env->GetLongField(value, field_id));
            if (!node_ptr) { return 0; }
            peer.reset(new openvrml::sfnode(*node_ptr));
        }
        return jlong(peer.release());
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
}

jobject JNICALL Java_vrml_field_SFNode_initNode(JNIEnv * const env,
                                                jclass,
                                                const jlong peer)
{
    using boost::polymorphic_downcast;
    using openvrml::field_value;
    using openvrml::sfnode;
    const sfnode * const sfn =
        polymorphic_downcast<sfnode *>(reinterpret_cast<field_value *>(peer));
    return create_Node(*env, sfn->value());
}

void JNICALL Java_vrml_field_SFNode_setPeerValue(JNIEnv * const env,
                                                 const jobject obj,
                                                 const jobject node)
{
    try {
        using openvrml::sfnode;
        sfnode & sfn = get_Field_peer<sfnode>(*env, obj);
        sfn.value(get_BaseNode_peer(*env, node));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_ConstSFRotation_getValue(JNIEnv * env,
                                                      jobject obj,
                                                      jfloatArray jarr)
{
    try {
        using openvrml::sfrotation;
        sfrotation & sfr = get_Field_peer<sfrotation>(*env, obj);
        env->SetFloatArrayRegion(jarr, 0, 4,
                                 const_cast<float *>(&sfr.value()[0]));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.field.SFRotation.createPeer().
 *
 * @param[in] env   JNI environment.
 * @param[in] clazz SFRotation class.
 * @param[in] value BaseNode object to initialize field with.
 */
jlong JNICALL Java_vrml_field_SFRotation_createPeer(JNIEnv * const env,
                                                    jclass,
                                                    const jfloat axisX,
                                                    const jfloat axisY,
                                                    const jfloat axisZ,
                                                    const jfloat angle)
{
    std::auto_ptr<openvrml::field_value> peer;
    try {
        using openvrml::make_rotation;
        peer.reset(new openvrml::sfrotation(
                       make_rotation(axisX, axisY, axisZ, angle)));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

void JNICALL Java_vrml_field_SFRotation_getValue(JNIEnv * const env,
                                                 const jobject obj,
                                                 const jfloatArray jarr)
{
    Java_vrml_field_ConstSFRotation_getValue(env, obj, jarr);
}

/**
 * JNI implementation of SFRotation::setValue.
 *
 * @brief env JNI environment
 * @brief obj SFRotation object
 * @brief axisX X-axis rotation of new value
 * @brief axisY Y-axis rotation of new value
 * @brief axisZ Z-axis rotation of new value
 * @brief angle Angle of new value
 */
void JNICALL Java_vrml_field_SFRotation_setValue__FFFF(JNIEnv * const env,
                                                       const jobject obj,
                                                       const jfloat axisX,
                                                       const jfloat axisY,
                                                       const jfloat axisZ,
                                                       const jfloat angle)
{
    try {
        using openvrml::make_rotation;
        using openvrml::sfrotation;
        sfrotation & sfr = get_Field_peer<sfrotation>(*env, obj);
        sfr.value(make_rotation(axisX, axisY, axisZ, angle));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstSFString::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFString object
 * @return Value of field
 */
jstring JNICALL Java_vrml_field_ConstSFString_getValue(JNIEnv * const env,
                                                       const jobject obj)
{
    try {
        using openvrml::sfstring;
        sfstring & sfstr = get_Field_peer<sfstring>(*env, obj);
        return env->NewStringUTF((sfstr.value()).c_str());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

jlong JNICALL Java_vrml_field_SFString_createPeer(JNIEnv * const env,
                                                  jclass,
                                                  const jstring jstr)
{
    std::auto_ptr<openvrml::field_value> peer;
    try {
        using openvrml::sfstring;

        // Need to handle null strings
        if (jstr) {
            const char * str = env->GetStringUTFChars(jstr, 0);
            if (!str) { return 0; } // OutOfMemoryError
            scope_guard str_guard =
                make_obj_guard(*env,
                               &JNIEnv::ReleaseStringUTFChars,
                               jstr,
                               str);
            boost::ignore_unused_variable_warning(str_guard);
            peer.reset(new openvrml::sfstring(str));
        } else {
            peer.reset(new openvrml::sfstring);
        }
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of SFString::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFString object
 * @return Value of field
 */
jstring JNICALL Java_vrml_field_SFString_getValue(JNIEnv * const env,
                                                  const jobject obj)
{
  return Java_vrml_field_ConstSFString_getValue(env, obj);
}

void JNICALL
Java_vrml_field_SFString_setValue__Ljava_lang_String_2(JNIEnv * const env,
                                                       const jobject obj,
                                                       const jstring jstr)
{
    try {
        using openvrml::sfstring;
        sfstring & sfstr = get_Field_peer<sfstring>(*env, obj);
        const char * const str = env->GetStringUTFChars(jstr, 0);
        if (!str) { return; } // OutOfMemoryError
        scope_guard str_guard =
            make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars, jstr, str);
        boost::ignore_unused_variable_warning(str_guard);
        sfstr.value(str);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstSFTime::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFTime object
 * @return Value of field
 */
jdouble JNICALL Java_vrml_field_ConstSFTime_getValue(JNIEnv * const env,
                                                     const jobject obj)
{
    try {
        using openvrml::sftime;
        sftime & sft = get_Field_peer<sftime>(*env, obj);
        return jdouble(sft.value());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0;
}

jlong JNICALL Java_vrml_field_SFTime_createPeer(JNIEnv * const env,
                                                jclass,
                                                const jdouble time)
{
    std::auto_ptr<openvrml::sftime> peer;;
    try {
        peer.reset(new openvrml::sftime(time));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of SFTime::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFTime object
 * @return Value of field
 */
jdouble JNICALL Java_vrml_field_SFTime_getValue(JNIEnv * const env,
                                                const jobject obj)
{
    return Java_vrml_field_ConstSFTime_getValue(env, obj);
}

void JNICALL Java_vrml_field_SFTime_setValue__D(JNIEnv * const env,
                                                const jobject obj,
                                                const jdouble time)
{
    try {
        using openvrml::sftime;
        sftime & sft = get_Field_peer<sftime>(*env, obj);
        sft.value(time);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstSFVec2f::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2f object
 * @param[out] jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_ConstSFVec2f_getValue(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jfloatArray jarr)
{
    try {
        using openvrml::sfvec2f;
        sfvec2f & sfv = get_Field_peer<sfvec2f>(*env, obj);
        env->SetFloatArrayRegion(jarr, 0, 2,
                                 const_cast<float *>(&sfv.value()[0]));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstSFVec2f::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2f object
 * @return X-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec2f_getX(JNIEnv * env, jobject obj)
{
    try {
        using openvrml::sfvec2f;
        const sfvec2f & sfv = get_Field_peer<sfvec2f>(*env, obj);
        return sfv.value().x();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

/**
 * @brief Implementation of ConstSFVec2f::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2f object
 * @return Y-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec2f_getY(JNIEnv * env, jobject obj)
{
    try {
        using openvrml::sfvec2f;
        const sfvec2f & sfv = get_Field_peer<sfvec2f>(*env, obj);
        return sfv.value().y();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

jlong JNICALL Java_vrml_field_SFVec2f_createPeer(JNIEnv * const env,
                                                 jclass,
                                                 const jfloat x,
                                                 const jfloat y)
{
    std::auto_ptr<openvrml::sfvec2f> peer;
    try {
        peer.reset(new openvrml::sfvec2f(openvrml::make_vec2f(x, y)));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of SFVec2f::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2f object
 * @param[out] jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_SFVec2f_getValue(JNIEnv * const env,
                                              const jobject obj,
                                              const jfloatArray jarr)
{
    return Java_vrml_field_ConstSFVec2f_getValue(env, obj, jarr);
}

/**
 * @brief Implementation of SFVec2f::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2f object
 * @return X-component of vector
 */
jfloat JNICALL Java_vrml_field_SFVec2f_getX(JNIEnv * const env,
                                            const jobject obj)
{
    return Java_vrml_field_ConstSFVec2f_getX(env, obj);
}

/**
 * @brief Implementation of SFVec2f::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2f object
 * @return Y-component of vector
 */
jfloat JNICALL Java_vrml_field_SFVec2f_getY(JNIEnv * const env,
                                            const jobject obj)
{
    return Java_vrml_field_ConstSFVec2f_getY(env, obj);
}

void JNICALL Java_vrml_field_SFVec2f_setValue__FF(JNIEnv * env,
                                                  jobject obj,
                                                  jfloat x,
                                                  jfloat y)
{
    try {
        using openvrml::make_vec2f;
        using openvrml::sfvec2f;
        sfvec2f & sfv = get_Field_peer<sfvec2f>(*env, obj);
        sfv.value(make_vec2f(x, y));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstSFVec2d::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2d object
 * @param[out] jarr Value of field as a double array
 */
void JNICALL Java_vrml_field_ConstSFVec2d_getValue(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jdoubleArray jarr)
{
    try {
        using openvrml::sfvec2d;
        const sfvec2d & sfv = get_Field_peer<sfvec2d>(*env, obj);
        env->SetDoubleArrayRegion(jarr, 0, 2,
                                  const_cast<double *>(&sfv.value()[0]));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstSFVec2d::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2d object
 * @return X-component of vector
 */
jdouble JNICALL Java_vrml_field_ConstSFVec2d_getX(JNIEnv * env, jobject obj)
{
    try {
        using openvrml::sfvec2d;
        const sfvec2d & sfv = get_Field_peer<sfvec2d>(*env, obj);
        return sfv.value().x();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0;
}

/**
 * @brief Implementation of ConstSFVec2d::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2d object
 * @return Y-component of vector
 */
jdouble JNICALL Java_vrml_field_ConstSFVec2d_getY(JNIEnv * env, jobject obj)
{
    try {
        using openvrml::sfvec2d;
        const sfvec2d & sfv = get_Field_peer<sfvec2d>(*env, obj);
        return sfv.value().y();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0;
}

jlong JNICALL Java_vrml_field_SFVec2d_createPeer(JNIEnv * const env,
                                                 jclass,
                                                 const jdouble x,
                                                 const jdouble y)
{
    std::auto_ptr<openvrml::sfvec2d> peer;
    try {
        peer.reset(new openvrml::sfvec2d(openvrml::make_vec2d(x, y)));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of SFVec2d::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2d object
 * @param[out] jarr Value of field as a double array
 */
void JNICALL Java_vrml_field_SFVec2d_getValue(JNIEnv * const env,
                                              const jobject obj,
                                              const jdoubleArray jarr)
{
    Java_vrml_field_ConstSFVec2d_getValue(env, obj, jarr);
}

/**
 * @brief Implementation of SFVec2d::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2d object
 * @return X-component of vector
 */
jdouble JNICALL Java_vrml_field_SFVec2d_getX(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstSFVec2d_getX(env, obj);
}

/**
 * @brief Implementation of SFVec2d::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2d object
 * @return Y-component of vector
 */
jdouble JNICALL Java_vrml_field_SFVec2d_getY(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstSFVec2d_getY(env, obj);
}

void JNICALL Java_vrml_field_SFVec2d_setValue__FF(JNIEnv * const env,
                                                  const jobject obj,
                                                  const jdouble x,
                                                  const jdouble y)
{
    try {
        using openvrml::make_vec2d;
        using openvrml::sfvec2d;
        sfvec2d & sfv = get_Field_peer<sfvec2d>(*env, obj);
        sfv.value(make_vec2d(x, y));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstSFVec3f::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3f object
 * @param[out] jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_ConstSFVec3f_getValue(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jfloatArray jarr)
{
    try {
        using openvrml::sfvec3f;
        const sfvec3f & sfv = get_Field_peer<sfvec3f>(*env, obj);
        env->SetFloatArrayRegion(jarr, 0, 3,
                                 const_cast<float *>(&sfv.value()[0]));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstSFVec3f::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3f object
 * @return X-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec3f_getX(JNIEnv * const env,
                                                 const jobject obj)
{
    try {
        using openvrml::sfvec3f;
        const sfvec3f & sfv = get_Field_peer<sfvec3f>(*env, obj);
        return sfv.value().x();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

/**
 * @brief Implementation of ConstSFVec3f::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3f object
 * @return Y-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec3f_getY(JNIEnv * const env,
                                                 const jobject obj)
{
    try {
        using openvrml::sfvec3f;
        const sfvec3f & sfv = get_Field_peer<sfvec3f>(*env, obj);
        return sfv.value().y();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

/**
 * @brief Implementation of ConstSFVec3f::getZ.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3f object
 * @return Z-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec3f_getZ(JNIEnv * const env,
                                                 const jobject obj)
{
    try {
        using openvrml::sfvec3f;
        const sfvec3f & sfv = get_Field_peer<sfvec3f>(*env, obj);
        return sfv.value().z();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

jlong JNICALL Java_vrml_field_SFVec3f_createPeer(JNIEnv * const env,
                                                 jclass,
                                                 const jfloat x,
                                                 const jfloat y,
                                                 const jfloat z)
{
    std::auto_ptr<openvrml::sfvec3f> peer;
    try {
        peer.reset(new openvrml::sfvec3f(openvrml::make_vec3f(x, y, z)));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of SFVec3f::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3f object
 * @param[out] jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_SFVec3f_getValue(JNIEnv * const env,
                                              const jobject obj,
                                              const jfloatArray jarr)
{
    Java_vrml_field_ConstSFVec3f_getValue(env, obj, jarr);
}

/**
 * @brief Implementation of SFVec3f::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3f object
 * @return X-component of vector
 */
jfloat JNICALL Java_vrml_field_SFVec3f_getX(JNIEnv * const env,
                                            const jobject obj)
{
    return Java_vrml_field_ConstSFVec3f_getX(env, obj);
}

/**
 * @brief Implementation of SFVec3f::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3f object
 * @return Y-component of vector
 */
jfloat JNICALL Java_vrml_field_SFVec3f_getY(JNIEnv * const env,
                                            const jobject obj)
{
    return Java_vrml_field_ConstSFVec3f_getY(env, obj);
}

/**
 * @brief Implementation of SFVec3f::getZ.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3f object
 * @return Z-component of vector
 */
jfloat JNICALL Java_vrml_field_SFVec3f_getZ(JNIEnv * const env,
                                            const jobject obj)
{
    return Java_vrml_field_ConstSFVec3f_getZ(env, obj);
}

void JNICALL Java_vrml_field_SFVec3f_setValue__FFF(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jfloat x,
                                                   const jfloat y,
                                                   const jfloat z)
{
    try {
        using openvrml::make_vec3f;
        using openvrml::sfvec3f;
        sfvec3f & sfv = get_Field_peer<sfvec3f>(*env, obj);
        sfv.value(make_vec3f(x, y, z));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}


/**
 * @brief Implementation of ConstSFVec3d::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3d object
 * @param[out] jarr Value of field as a double array
 */
void JNICALL Java_vrml_field_ConstSFVec3d_getValue(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jdoubleArray jarr)
{
    try {
        using openvrml::sfvec3d;
        const sfvec3d & sfv = get_Field_peer<sfvec3d>(*env, obj);
        env->SetDoubleArrayRegion(jarr, 0, 3,
                                  const_cast<double *>(&sfv.value()[0]));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstSFVec3d::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3d object
 * @return X-component of vector
 */
jdouble JNICALL Java_vrml_field_ConstSFVec3d_getX(JNIEnv * const env,
                                                  const jobject obj)
{
    try {
        using openvrml::sfvec3d;
        const sfvec3d & sfv = get_Field_peer<sfvec3d>(*env, obj);
        return sfv.value().x();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0;
}

/**
 * @brief Implementation of ConstSFVec3d::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3d object
 * @return Y-component of vector
 */
jdouble JNICALL Java_vrml_field_ConstSFVec3d_getY(JNIEnv * const env,
                                                  const jobject obj)
{
    try {
        using openvrml::sfvec3d;
        const sfvec3d & sfv = get_Field_peer<sfvec3d>(*env, obj);
        return sfv.value().y();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0;
}

/**
 * @brief Implementation of ConstSFVec3d::getZ.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3d object
 * @return Z-component of vector
 */
jdouble JNICALL Java_vrml_field_ConstSFVec3d_getZ(JNIEnv * const env,
                                                  const jobject obj)
{
    try {
        using openvrml::sfvec3d;
        const sfvec3d & sfv = get_Field_peer<sfvec3d>(*env, obj);
        return sfv.value().z();
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0;
}

jlong JNICALL Java_vrml_field_SFVec3d_createPeer(JNIEnv * const env,
                                                 jclass,
                                                 const jdouble x,
                                                 const jdouble y,
                                                 const jdouble z)
{
    std::auto_ptr<openvrml::sfvec3d> peer;
    try {
        peer.reset(new openvrml::sfvec3d(openvrml::make_vec3d(x, y, z)));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

/**
 * @brief Implementation of SFVec3d::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3d object
 * @param[out] jarr Value of field as a double array
 */
void JNICALL Java_vrml_field_SFVec3d_getValue(JNIEnv * const env,
                                              const jobject obj,
                                              const jdoubleArray jarr)
{
    Java_vrml_field_ConstSFVec3d_getValue(env, obj, jarr);
}

/**
 * @brief Implementation of SFVec3d::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3d object
 * @return X-component of vector
 */
jdouble JNICALL Java_vrml_field_SFVec3d_getX(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstSFVec3d_getX(env, obj);
}

/**
 * @brief Implementation of SFVec3d::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3d object
 * @return Y-component of vector
 */
jdouble JNICALL Java_vrml_field_SFVec3d_getY(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstSFVec3d_getY(env, obj);
}

/**
 * @brief Implementation of SFVec3d::getZ.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3d object
 * @return Z-component of vector
 */
jdouble JNICALL Java_vrml_field_SFVec3d_getZ(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstSFVec3d_getZ(env, obj);
}

void JNICALL Java_vrml_field_SFVec3d_setValue__FFF(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jdouble x,
                                                   const jdouble y,
                                                   const jdouble z)
{
    try {
        using openvrml::make_vec3d;
        using openvrml::sfvec3d;
        sfvec3d & sfv = get_Field_peer<sfvec3d>(*env, obj);
        sfv.value(make_vec3d(x, y, z));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

namespace {
    template <typename T> struct jni_type;

    template <> struct jni_type<bool> {
        typedef jboolean type;
    };

    template <> struct jni_type<float> {
        typedef jfloat type;
    };

    template <> struct jni_type<double> {
        typedef jdouble type;
    };

    template <> struct jni_type<openvrml::int32> {
        typedef jint type;
    };

    template <typename T> struct array_type;

    template <> struct array_type<jboolean> {
        typedef jbooleanArray type;
    };

    template <> struct array_type<jfloat> {
        typedef jfloatArray type;
    };

    template <> struct array_type<jdouble> {
        typedef jdoubleArray type;
    };

    template <> struct array_type<jint> {
        typedef jintArray type;
    };

    template <typename T>
    T * get_array_elements(JNIEnv & env, typename array_type<T>::type array);

    template <>
    jboolean * get_array_elements<jboolean>(JNIEnv & env,
                                            array_type<jboolean>::type array)
    {
        return env.GetBooleanArrayElements(array, 0);
    }

    template <>
    jfloat * get_array_elements<jfloat>(JNIEnv & env,
                                        array_type<jfloat>::type array)
    {
        return env.GetFloatArrayElements(array, 0);
    }

    template <>
    jdouble * get_array_elements<jdouble>(JNIEnv & env,
                                          array_type<jdouble>::type array)
    {
        return env.GetDoubleArrayElements(array, 0);
    }

    template <>
    jint * get_array_elements<jint>(JNIEnv & env, array_type<jint>::type array)
    {
        return env.GetIntArrayElements(array, 0);
    }

    template <typename T>
    void release_array_elements(JNIEnv & env,
                                typename array_type<T>::type array,
                                T * elems);

    template <>
    void release_array_elements<jboolean>(
        JNIEnv & env,
        array_type<jboolean>::type array,
        jboolean * elems)
    {
        env.ReleaseBooleanArrayElements(array, elems, 0);
    }

    template <>
    void release_array_elements<jfloat>(
        JNIEnv & env,
        array_type<jfloat>::type array,
        jfloat * elems)
    {
        env.ReleaseFloatArrayElements(array, elems, 0);
    }

    template <>
    void release_array_elements<jdouble>(
        JNIEnv & env,
        array_type<jdouble>::type array,
        jdouble * elems)
    {
        env.ReleaseDoubleArrayElements(array, elems, 0);
    }

    template <>
    void release_array_elements<jint>(JNIEnv & env,
                                      array_type<jint>::type array,
                                      jint * elems)
    {
        env.ReleaseIntArrayElements(array, elems, 0);
    }

    template <typename T>
    void set_array_region(JNIEnv & env,
                          typename array_type<T>::type array,
                          jsize start,
                          jsize len,
                          const T * buf);

    template <>
    void set_array_region<jfloat>(
        JNIEnv & env,
        const array_type<jfloat>::type array,
        const jsize start,
        const jsize len,
        const jfloat * const buf)
    {
        env.SetFloatArrayRegion(array, start, len, const_cast<jfloat *>(buf));
    }

    template <>
    void set_array_region<jdouble>(
        JNIEnv & env,
        const array_type<jdouble>::type array,
        const jsize start,
        const jsize len,
        const jdouble * const buf)
    {
        env.SetDoubleArrayRegion(array, start, len,
                                 const_cast<jdouble *>(buf));
    }

    template <>
    void set_array_region<jint>(JNIEnv & env,
                                const array_type<jint>::type array,
                                const jsize start,
                                const jsize len,
                                const jint * const buf)
    {
        env.SetIntArrayRegion(array, start, len, const_cast<jint *>(buf));
    }

    template <typename FieldValue>
    jlong mfield_createPeer(
        JNIEnv & env,
        const jint size,
        const typename array_type<typename jni_type<typename FieldValue::value_type::value_type>::type>::type array)
    {
        using boost::ref;

        typedef typename jni_type<
            typename FieldValue::value_type::value_type>::type elem_t;

        if (size > env.GetArrayLength(array)) {
            throw_array_index_out_of_bounds(env,
                                            "\"values\" array contains fewer than "
                                            "\"size\" boolean values");
            return 0;
        }
        elem_t * const elements = get_array_elements<elem_t>(env, array);
        if (!elements) { return 0; }
        scope_guard elements_guard =
            make_guard(release_array_elements<elem_t>,
                       ref(env), array, elements);
        boost::ignore_unused_variable_warning(elements_guard);
        std::auto_ptr<FieldValue> peer;
        try {
            const typename FieldValue::value_type vec(elements,
                                                      elements + size);
            peer.reset(new FieldValue(vec));
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(env, ex.what());
            return 0;
        }
        return jlong(peer.release());
    }
}

namespace {
    template <typename FieldValue>
    jint mfield_getSize(JNIEnv & env, const jobject obj)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        return jint(mf.value().size());
    }
}

jint JNICALL Java_vrml_field_ConstMFBool_getSize(JNIEnv * const env,
                                                 const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mfbool>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

void JNICALL Java_vrml_field_ConstMFBool_getValue(JNIEnv * const env,
                                                  const jobject obj,
                                                  const jbooleanArray jarr)
{
    try {
        using openvrml::mfbool;
        const mfbool & mfb = get_Field_peer<mfbool>(*env, obj);
        const size_t size = mfb.value().size();
        try {
            if (size > 0) {
                //
                // Recall that std::vector<bool> is a weird thing that we
                // can't copy directly.
                //
                const mfbool::value_type & bool_vec = mfb.value();
                const boost::scoped_array<jboolean> jboolean_array(
                    new jboolean[size]);
                std::copy(bool_vec.begin(), bool_vec.end(),
                          jboolean_array.get());
                const std::vector<char> vec(bool_vec.begin(), bool_vec.end());
                env->SetBooleanArrayRegion(jarr, 0, size, jboolean_array.get());
            }
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

namespace {
    template <typename FieldValue>
    typename jni_type<typename FieldValue::value_type::value_type>::type
    mfield_get1Value(JNIEnv & env, const jobject obj, const jint index)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        const FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        try {
            return mf.value().at(index);
        } catch (std::out_of_range & ex) {
            throw_array_index_out_of_bounds(env, ex.what());
        }
        return 0;
    }
}

jboolean JNICALL Java_vrml_field_ConstMFBool_get1Value(JNIEnv * const env,
                                                       const jobject obj,
                                                       const jint index)
{
    try {
        return mfield_get1Value<openvrml::mfbool>(*env, obj, index);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return false;
}

jlong JNICALL Java_vrml_field_MFBool_createPeer(JNIEnv * const env,
                                                jclass,
                                                const jint size,
                                                const jbooleanArray value)
{
    return mfield_createPeer<openvrml::mfbool>(*env, size, value);
}

jint JNICALL Java_vrml_field_MFBool_getSize(JNIEnv * const env,
                                            const jobject obj)
{
    return Java_vrml_field_ConstMFBool_getSize(env, obj);
}

namespace {
    template <typename FieldValue>
    void mfield_clear(JNIEnv & env, const jobject obj)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        mf.value(typename FieldValue::value_type());
    }
}

void JNICALL Java_vrml_field_MFBool_clear(JNIEnv * const env,
                                          const jobject obj)
{
    try {
        mfield_clear<openvrml::mfbool>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

namespace {
    template <typename FieldValue>
    void mfield_delete(JNIEnv & env, const jobject obj, const jint index)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        if (!(size_t(index) < mf.value().size())) {
            throw_array_index_out_of_bounds(env, "index out of bounds");
            return;
        }
        typename FieldValue::value_type temp = mf.value();
        temp.erase(temp.begin() + index);
        mf.value(temp);
    }
}

void JNICALL Java_vrml_field_MFBool_delete(JNIEnv * const env,
                                           const jobject obj,
                                           const jint index)
{
    try {
        mfield_delete<openvrml::mfbool>(*env, obj, index);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFBool_getValue(JNIEnv * const env,
                                             const jobject obj,
                                             const jbooleanArray jarr)
{
    Java_vrml_field_ConstMFBool_getValue(env, obj, jarr);
}

jboolean JNICALL Java_vrml_field_MFBool_get1Value(JNIEnv * const env,
                                                  const jobject obj,
                                                  const jint index)
{
    return Java_vrml_field_ConstMFBool_get1Value(env, obj, index);
}

namespace {
    template <typename FieldValue>
    void mfield_setValue(
        JNIEnv & env,
        const jobject obj,
        const jint size,
        const typename array_type<typename jni_type<typename FieldValue::value_type::value_type>::type>::type array)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        const jlong new_peer_value =
            mfield_createPeer<FieldValue>(env, size, array);
        if (!new_peer_value) { return; }
        std::auto_ptr<FieldValue>
            new_peer(reinterpret_cast<FieldValue *>(new_peer_value));
        FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        mf.swap(*new_peer);
    }
}

void JNICALL Java_vrml_field_MFBool_setValue__I_3F(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jint size,
                                                   const jbooleanArray value)
{
    try {
        mfield_setValue<openvrml::mfbool>(*env, obj, size, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

namespace {
    template <typename FieldValue>
    void mfield_setValue(JNIEnv & env, const jobject obj, const jobject value)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        const FieldValue & new_mf = get_Field_peer<FieldValue>(env, value);
        try {
            mf = new_mf; // Throws bad_alloc.
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(env, ex.what());
        }
    }
}

void JNICALL
Java_vrml_field_MFBool_setValue__Lvrml_field_MFBool_2(JNIEnv * const env,
                                                      const jobject obj,
                                                      const jobject value)
{
    try {
        mfield_setValue<openvrml::mfbool>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFBool_setValue__Lvrml_field_ConstMFBool_2(JNIEnv * const env,
                                                           const jobject obj,
                                                           const jobject value)
{
    Java_vrml_field_MFBool_setValue__Lvrml_field_MFBool_2(env, obj, value);
}

namespace {
    template <typename FieldValue>
    void mfield_set1Value(
        JNIEnv & env,
        const jobject obj,
        const jint index,
        const typename FieldValue::value_type::value_type value)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        try {
            typename FieldValue::value_type temp = mf.value();
            temp.at(index) = value;
            mf.value(temp);
        } catch (std::out_of_range & ex) {
            throw_array_index_out_of_bounds(env, ex.what());
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(env, ex.what());
        }
    }
}

void JNICALL Java_vrml_field_MFBool_set1Value__IF(JNIEnv * const env,
                                                  const jobject obj,
                                                  const jint index,
                                                  const jboolean value)
{
    try {
        mfield_set1Value<openvrml::mfbool>(*env, obj, index, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

namespace {
    template <typename FieldValue>
    void mfield_addValue(
        JNIEnv & env,
        const jobject obj,
        const typename FieldValue::value_type::value_type value)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        try {
            typename FieldValue::value_type temp = mf.value();
            temp.push_back(value);
            mf.value(temp);
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(env, ex.what());
        }
    }
}

/**
 * @brief Implementation of MFBool.addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFBool object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFBool_addValue__F(JNIEnv * const env,
                                                const jobject obj,
                                                const jboolean value)
{
    try {
        mfield_addValue<openvrml::mfbool>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

namespace {
    template <typename FieldValue>
    void mfield_insertValue(
        JNIEnv & env,
        const jobject obj,
        const jint index,
        const typename FieldValue::value_type::value_type value)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        if (!(size_t(index) < mf.value().size())) {
            throw_array_index_out_of_bounds(env, "index out of bounds");
            return;
        }
        try {
            typename FieldValue::value_type temp = mf.value();
            temp.insert(temp.begin() + index, value);
            mf.value(temp);
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(env, ex.what());
        }
    }
}

void JNICALL Java_vrml_field_MFBool_insertValue__IF(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint index,
                                                    const jboolean value)
{
    try {
        mfield_insertValue<openvrml::mfbool>(*env, obj, index, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}


jint JNICALL Java_vrml_field_ConstMFColor_getSize(JNIEnv * const env,
                                                  const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mfcolor>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

namespace {
    template <typename BaseType> struct component_jni_type;

    template <> struct component_jni_type<openvrml::color> {
        typedef jfloat type;
    };

    template <typename BaseType> struct num_components;

    template <> struct num_components<openvrml::color> {
        static const size_t value = 3;
    };

    template <> struct num_components<openvrml::rotation> {
        static const size_t value = 4;
    };

    template <typename T> struct set_array_region_t;

    template <> struct set_array_region_t<jfloat> {
        static void (JNIEnv::* const value)(
            array_type<jfloat>::type, jsize, jsize, const jfloat *);
    };

    void (JNIEnv::* const set_array_region_t<jfloat>::value)(
        array_type<jfloat>::type, jsize, jsize, const jfloat *) =
        &JNIEnv::SetFloatArrayRegion;

    template <> struct set_array_region_t<jdouble> {
        static void (JNIEnv::* const value)(
            array_type<jdouble>::type, jsize, jsize, const jdouble *);
    };

    void (JNIEnv::* const set_array_region_t<jdouble>::value)(
        array_type<jdouble>::type, jsize, jsize, const jdouble *) =
        &JNIEnv::SetDoubleArrayRegion;

    template <typename T>
    void set_array_region(JNIEnv & env,
                          const typename array_type<T>::type array,
                          const jsize start,
                          const jsize len,
                          T * const buf)
    {
        (env.*set_array_region_t<T>::value)(array, start, len, buf);
    }

    template <typename FieldValue>
    void mfield_getValue(JNIEnv & env,
                         const jobject obj,
                         const jobjectArray jarr)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        const FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        for (size_t i = 0; i < mf.value().size(); ++i) {
            typedef typename component_jni_type<typename FieldValue::value_type::value_type>::type
                component_t;
            typedef typename array_type<component_t>::type array_t;
            array_t element =
                static_cast<array_t>(env.GetObjectArrayElement(jarr, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return;
            }
            component_t * const arrayRegion =
                const_cast<component_t *>(&mf.value()[i][0]);
            static const size_t components =
                num_components<typename FieldValue::value_type::value_type>::value;
            set_array_region<component_t>(env, element, 0, components,
                                          arrayRegion);
            if (env.ExceptionOccurred()) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return;
            }
        }
    }
}

void JNICALL
Java_vrml_field_ConstMFColor_getValue___3_3F(JNIEnv * const env,
                                             const jobject obj,
                                             const jobjectArray jarr)
{
    try {
        mfield_getValue<openvrml::mfcolor>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

namespace {

    template <typename FieldValue>
    void mfield_getValue(
        JNIEnv & env,
        const jobject obj,
        const typename array_type<
            typename component_jni_type<
                typename FieldValue::value_type::value_type>::type>::type jarr)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        const FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        for (size_t i = 0; i < mf.value().size(); ++i) {
            typedef typename component_jni_type<
                typename FieldValue::value_type::value_type>::type
                component_t;
            component_t * const arrayRegion =
                const_cast<component_t *>(&mf.value()[i][0]);
            static const size_t components =
                num_components<
                    typename FieldValue::value_type::value_type>::value;
            set_array_region<component_t>(env, jarr,
                                          i * components, components,
                                          arrayRegion);
            if (env.ExceptionOccurred()) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return;
            }
        }
    }
}

void JNICALL Java_vrml_field_ConstMFColor_getValue___3F(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jfloatArray jarr)
{
    try {
        mfield_getValue<openvrml::mfcolor>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

namespace {

    template <typename FieldValue>
    void mfield_get1Value(
        JNIEnv & env,
        const jobject obj,
        const jint index,
        const typename array_type<
            typename component_jni_type<
                typename FieldValue::value_type::value_type>::type>::type jarr)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        const FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        try {
            typedef typename component_jni_type<
                typename FieldValue::value_type::value_type>::type
                component_t;
            component_t * const arrayRegion =
                const_cast<component_t *>(&mf.value().at(index)[0]);
            static const size_t components =
                num_components<
                    typename FieldValue::value_type::value_type>::value;
            set_array_region<component_t>(env, jarr, 0, components,
                                          arrayRegion);
        } catch (std::out_of_range & ex) {
            throw_array_index_out_of_bounds(env, ex.what());
        }
    }
}

void JNICALL
Java_vrml_field_ConstMFColor_get1Value__I_3F(JNIEnv * const env,
                                             const jobject obj,
                                             const jint index,
                                             const jfloatArray jarr)
{
    try {
        mfield_get1Value<openvrml::mfcolor>(*env, obj, index, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jlong JNICALL Java_vrml_field_MFColor_createPeer__I_3F(JNIEnv * const env,
                                                       jclass,
                                                       const jint size,
                                                       const jfloatArray jarr)
{
    if ((env->GetArrayLength(jarr) / 3) < size) {
        throw_array_index_out_of_bounds(*env,
                                        "\"colors\" array contains fewer than "
                                        "\"size\" color values");
        return 0;
    }
    jfloat * c = env->GetFloatArrayElements(jarr, 0);
    if (!c) { return 0; }
    scope_guard c_guard =
        make_obj_guard(*env, &JNIEnv::ReleaseFloatArrayElements,
                       jarr, c, 0);
    boost::ignore_unused_variable_warning(c_guard);
    std::auto_ptr<openvrml::mfcolor> peer;
    try {
        openvrml::mfcolor::value_type vec(size);
        for (jint i = 0; i < size; ++i, c += 3) {
            vec[i] = openvrml::make_color(c[0], c[1], c[2]);
        }
        peer.reset(new openvrml::mfcolor(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jlong JNICALL
Java_vrml_field_MFColor_createPeer___3_3F(JNIEnv * const env,
                                          jclass,
                                          const jobjectArray jarr)
{
    std::auto_ptr<openvrml::mfcolor> peer;
    try {
        openvrml::mfcolor::value_type vec(env->GetArrayLength(jarr)); // Throws std::bad_alloc.
        for (size_t i = 0; i < vec.size(); ++i) {
            jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return 0;
            }
            if (env->GetArrayLength(element) < 3) {
                throw_array_index_out_of_bounds(*env, "");
                return 0;
            }
            jfloat * const c = env->GetFloatArrayElements(element, 0);
            if (!c) {
                // Presumably we raised an OutOfMemoryError.
                return 0;
            }
            scope_guard c_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseFloatArrayElements,
                               element, c, 0);
            boost::ignore_unused_variable_warning(c_guard);
            vec[i] = openvrml::make_color(c[0], c[1], c[2]);
        }
        peer.reset(new openvrml::mfcolor(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jint JNICALL Java_vrml_field_MFColor_getSize(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstMFColor_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFColor_clear(JNIEnv * const env,
                                           const jobject obj)
{
    try {
        mfield_clear<openvrml::mfcolor>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFColor_delete(JNIEnv * const env,
                                            const jobject obj,
                                            const jint index)
{
    try {
        mfield_delete<openvrml::mfcolor>(*env, obj, index);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFColor_getValue___3_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jobjectArray jarr)
{
    Java_vrml_field_ConstMFColor_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFColor_getValue___3F(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jfloatArray jarr)
{
    Java_vrml_field_ConstMFColor_getValue___3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFColor_get1Value__I_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jint element,
                                                     const jfloatArray jarr)
{
    Java_vrml_field_ConstMFColor_get1Value__I_3F(env, obj, element, jarr);
}

void JNICALL Java_vrml_field_MFColor_setValue___3_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jobjectArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFColor_createPeer___3_3F(env,
                                                  env->GetObjectClass(obj),
                                                  value);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfcolor;
        std::auto_ptr<mfcolor>
            new_peer(reinterpret_cast<mfcolor *>(new_peer_value));
        mfcolor & mfc = get_Field_peer<mfcolor>(*env, obj);
        mfc.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFColor_setValue__I_3F(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint size,
                                                    const jfloatArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFColor_createPeer__I_3F(env,
                                                 env->GetObjectClass(obj),
                                                 size,
                                                 value);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfcolor;
        std::auto_ptr<mfcolor>
            new_peer(reinterpret_cast<mfcolor *>(new_peer_value));
        mfcolor & mfc = get_Field_peer<mfcolor>(*env, obj);
        mfc.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFColor_setValue__Lvrml_field_MFColor_2(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jobject value)
{
    try {
        mfield_setValue<openvrml::mfcolor>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFColor_setValue__Lvrml_field_ConstMFColor_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject value)
{
    Java_vrml_field_MFColor_setValue__Lvrml_field_MFColor_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFColor_set1Value__IFFF(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jint index,
                                                     const jfloat red,
                                                     const jfloat green,
                                                     const jfloat blue)
{
    try {
        mfield_set1Value<openvrml::mfcolor>(
            *env, obj, index, openvrml::make_color(red, green, blue));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFColor::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFColor object
 * @param[in] red Red component of color to add to end of list
 * @param[in] green Green component of color to add to end of list
 * @param[in] blue Blue component of color to add to end of list
 */
void JNICALL Java_vrml_field_MFColor_addValue__FFF(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jfloat red,
                                                   const jfloat green,
                                                   const jfloat blue)
{
    try {
        mfield_addValue<openvrml::mfcolor>(
            *env, obj, openvrml::make_color(red, green, blue));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFColor_insertValue__IFFF(JNIEnv * const env,
                                                       const jobject obj,
                                                       const jint index,
                                                       const jfloat red,
                                                       const jfloat green,
                                                       const jfloat blue)
{
    try {
        mfield_insertValue<openvrml::mfcolor>(
            *env, obj, index, openvrml::make_color(red, green, blue));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jint JNICALL Java_vrml_field_ConstMFFloat_getSize(JNIEnv * const env,
                                                  const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mffloat>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

namespace {
    template <typename FieldValue>
    void mfield_getValue(
        JNIEnv & env,
        const jobject obj,
        typename array_type<
            typename jni_type<
                typename FieldValue::value_type::value_type
            >::type
        >::type array)
        OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
    {
        const FieldValue & mf = get_Field_peer<FieldValue>(env, obj);
        const size_t size = mf.value().size();
        if (size > 0) {
            set_array_region<typename jni_type<typename FieldValue::value_type::value_type>::type>(
                env, array, 0, size, &mf.value().front());
        }
    }
}

void JNICALL Java_vrml_field_ConstMFFloat_getValue(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jfloatArray jarr)
{
    try {
        mfield_getValue<openvrml::mffloat>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jfloat JNICALL Java_vrml_field_ConstMFFloat_get1Value(JNIEnv * const env,
                                                      const jobject obj,
                                                      const jint index)
{
    try {
        return mfield_get1Value<openvrml::mffloat>(*env, obj, index);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0f;
}

jlong JNICALL Java_vrml_field_MFFloat_createPeer(JNIEnv * const env,
                                                 jclass,
                                                 const jint size,
                                                 const jfloatArray value)
{
    return mfield_createPeer<openvrml::mffloat>(*env, size, value);
}

jint JNICALL Java_vrml_field_MFFloat_getSize(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstMFFloat_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFFloat_clear(JNIEnv * const env,
                                           const jobject obj)
{
    try {
        using openvrml::mffloat;
        mffloat & mff = get_Field_peer<mffloat>(*env, obj);
        mff.value(mffloat::value_type());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFFloat_delete(JNIEnv * const env,
                                            const jobject obj,
                                            const jint index)
{
    try {
        using openvrml::mffloat;
        mffloat & mff = get_Field_peer<mffloat>(*env, obj);
        if (!(size_t(index) < mff.value().size())) {
            throw_array_index_out_of_bounds(*env, "index out of bounds");
            return;
        }
        mffloat::value_type temp = mff.value();
        temp.erase(temp.begin() + index);
        mff.value(temp);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFFloat_getValue(JNIEnv * const env,
                                              const jobject obj,
                                              const jfloatArray jarr)
{
    Java_vrml_field_ConstMFFloat_getValue(env, obj, jarr);
}

jfloat JNICALL Java_vrml_field_MFFloat_get1Value(JNIEnv * const env,
                                                 const jobject obj,
                                                 const jint index)
{
    return Java_vrml_field_ConstMFFloat_get1Value(env, obj, index);
}

void JNICALL Java_vrml_field_MFFloat_setValue__I_3F(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint size,
                                                    const jfloatArray value)
{
    try {
        mfield_setValue<openvrml::mffloat>(*env, obj, size, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFFloat_setValue__Lvrml_field_MFFloat_2(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jobject value)
{
    try {
        using openvrml::mffloat;
        mffloat & mff = get_Field_peer<mffloat>(*env, obj);
        const mffloat & new_mff = get_Field_peer<mffloat>(*env, value);
        try {
            mff = new_mff; // Throws bad_alloc.
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFFloat_setValue__Lvrml_field_ConstMFFloat_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject value)
{
    Java_vrml_field_MFFloat_setValue__Lvrml_field_MFFloat_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFFloat_set1Value__IF(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jint index,
                                                   const jfloat value)
{
    try {
        mfield_set1Value<openvrml::mffloat>(*env, obj, index, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of @c vrml.field.MFFloat.addValue().
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFFloat object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFFloat_addValue__F(JNIEnv * const env,
                                                 const jobject obj,
                                                 const jfloat value)
{
    try {
        mfield_addValue<openvrml::mffloat>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFFloat_insertValue__IF(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jint index,
                                                     const jfloat value)
{
    try {
        mfield_insertValue<openvrml::mffloat>(*env, obj, index, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}


jint JNICALL Java_vrml_field_ConstMFDouble_getSize(JNIEnv * const env,
                                                   const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mfdouble>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

void JNICALL Java_vrml_field_ConstMFDouble_getValue(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jdoubleArray jarr)
{
    try {
        mfield_getValue<openvrml::mfdouble>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jdouble JNICALL Java_vrml_field_ConstMFDouble_get1Value(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jint index)
{
    try {
        return mfield_get1Value<openvrml::mfdouble>(*env, obj, index);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0;
}

jlong JNICALL Java_vrml_field_MFDouble_createPeer(JNIEnv * const env,
                                                  jclass,
                                                  const jint size,
                                                  const jdoubleArray value)
{
    return mfield_createPeer<openvrml::mfdouble>(*env, size, value);
}

jint JNICALL Java_vrml_field_MFDouble_getSize(JNIEnv * const env,
                                              const jobject obj)
{
    return Java_vrml_field_ConstMFDouble_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFDouble_clear(JNIEnv * const env,
                                            const jobject obj)
{
    try {
        mfield_clear<openvrml::mfdouble>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFDouble_delete(JNIEnv * const env,
                                             const jobject obj,
                                             const jint index)
{
    try {
        mfield_delete<openvrml::mfdouble>(*env, obj, index);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFDouble_getValue(JNIEnv * const env,
                                               const jobject obj,
                                               const jdoubleArray jarr)
{
    Java_vrml_field_ConstMFDouble_getValue(env, obj, jarr);
}

jdouble JNICALL Java_vrml_field_MFDouble_get1Value(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jint index)
{
    return Java_vrml_field_ConstMFDouble_get1Value(env, obj, index);
}

void JNICALL Java_vrml_field_MFDouble_setValue__I_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jint size,
                                                     const jdoubleArray value)
{
    try {
        mfield_setValue<openvrml::mfdouble>(*env, obj, size, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFDouble_setValue__Lvrml_field_MFDouble_2(JNIEnv * const env,
                                                          const jobject obj,
                                                          const jobject value)
{
    try {
        mfield_setValue<openvrml::mfdouble>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFDouble_setValue__Lvrml_field_ConstMFDouble_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject value)
{
    Java_vrml_field_MFDouble_setValue__Lvrml_field_MFDouble_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFDouble_set1Value__IF(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint index,
                                                    const jdouble value)
{
    try {
        mfield_set1Value<openvrml::mfdouble>(*env, obj, index, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFDouble::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFDouble object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFDouble_addValue__F(JNIEnv * const env,
                                                  const jobject obj,
                                                  const jdouble value)
{
    try {
        mfield_addValue<openvrml::mfdouble>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFDouble_insertValue__IF(JNIEnv * const env,
                                                      const jobject obj,
                                                      const jint index,
                                                      const jdouble value)
{
    try {
        mfield_insertValue<openvrml::mfdouble>(*env, obj, index, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jint JNICALL Java_vrml_field_ConstMFInt32_getSize(JNIEnv * const env,
                                                  const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mfint32>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

void JNICALL Java_vrml_field_ConstMFInt32_getValue(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jintArray jarr)
{
    try {
        mfield_getValue<openvrml::mfint32>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jint JNICALL Java_vrml_field_ConstMFInt32_get1Value(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint index)
{
    try {
        return mfield_get1Value<openvrml::mfint32>(*env, obj, index);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

jlong JNICALL Java_vrml_field_MFInt32_createPeer(JNIEnv * const env,
                                                 jclass,
                                                 const jint size,
                                                 const jintArray value)
{
    return mfield_createPeer<openvrml::mfint32>(*env, size, value);
}

jint JNICALL Java_vrml_field_MFInt32_getSize(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstMFInt32_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFInt32_clear(JNIEnv * const env,
                                           const jobject obj)
{
    try {
        mfield_clear<openvrml::mfint32>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFInt32_delete(JNIEnv * const env,
                                            const jobject obj,
                                            const jint n)
{
    try {
        mfield_delete<openvrml::mfint32>(*env, obj, n);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFInt32_getValue(JNIEnv * const env,
                                              const jobject obj,
                                              const jintArray jarr)
{
    Java_vrml_field_ConstMFInt32_getValue(env, obj, jarr);
}

jint JNICALL Java_vrml_field_MFInt32_get1Value(JNIEnv * const env,
                                               const jobject obj,
                                               const jint index)
{
    return Java_vrml_field_ConstMFInt32_get1Value(env, obj, index);
}

void JNICALL Java_vrml_field_MFInt32_setValue__I_3I(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint size,
                                                    const jintArray value)
{
    try {
        mfield_setValue<openvrml::mfint32>(*env, obj, size, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFInt32_setValue__Lvrml_field_MFInt32_2(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jobject value)
{
    try {
        mfield_setValue<openvrml::mfint32>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFInt32_setValue__Lvrml_field_ConstMFInt32_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject value)
{
    Java_vrml_field_MFInt32_setValue__Lvrml_field_MFInt32_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFInt32_set1Value__II(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jint index,
                                                   const jint value)
{
    try {
        mfield_set1Value<openvrml::mfint32>(*env, obj, index, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFInt32::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFInt32 object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFInt32_addValue__I(JNIEnv * const env,
                                                 const jobject obj,
                                                 const jint value)
{
    try {
        mfield_addValue<openvrml::mfint32>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFInt32_insertValue__II(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jint index,
                                                     const jint value)
{
    try {
        mfield_insertValue<openvrml::mfint32>(*env, obj, index, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}


jlong JNICALL Java_vrml_field_MFNode_createPeer(JNIEnv * const env,
                                                jclass,
                                                const jint size,
                                                const jobjectArray basenode)
{
    std::auto_ptr<openvrml::mfnode> peer;
    try {
        openvrml::mfnode::value_type vec(size);
        for (jint i = 0; i < size; ++i) {
            const jobject node = env->GetObjectArrayElement(basenode, i);
            if (!node) { return 0; }
            const boost::intrusive_ptr<openvrml::node> & node_peer =
                get_BaseNode_peer(*env, node);
            vec[i] = node_peer;
        }
        peer.reset(new openvrml::mfnode(vec));
    } catch (std::bad_alloc & ex) {
        if (!env->ExceptionCheck()) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return jlong(peer.release());
}

jobjectArray JNICALL Java_vrml_field_MFNode_initNodes(JNIEnv * const env,
                                                      jclass,
                                                      const jlong peer)
{
    using boost::polymorphic_downcast;
    using openvrml::field_value;
    using openvrml::mfnode;
    const mfnode * const mfn =
        polymorphic_downcast<mfnode *>(reinterpret_cast<field_value *>(peer));

    const jclass basenode_class = env->FindClass("vrml/BaseNode");
    if (!basenode_class) { return 0; }

    const jobjectArray nodes =
        env->NewObjectArray(mfn->value().size(), basenode_class, 0);
    if (!nodes) { return 0; }

    try {
        for (jsize i = 0; i < jsize(mfn->value().size()); ++i) {
            if (env->PushLocalFrame(1) < 0) { throw std::bad_alloc(); }
            scope_guard local_frame_guard =
                make_obj_guard(*env, &JNIEnv::PopLocalFrame, jobject(0));
            boost::ignore_unused_variable_warning(local_frame_guard);

            const jobject node = create_Node(*env, mfn->value()[i]);
            env->SetObjectArrayElement(nodes, i, node);
        }
        return nodes;
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

void JNICALL Java_vrml_field_MFNode_peer_1clear(JNIEnv *,
                                                jclass,
                                                const jlong peer)
{
    openvrml::mfnode * const mfn =
        boost::polymorphic_downcast<openvrml::mfnode *>(
            reinterpret_cast<openvrml::field_value *>(peer));
    if (!mfn) { return; }
    mfn->value(openvrml::mfnode::value_type());
}

void JNICALL Java_vrml_field_MFNode_peer_1delete(JNIEnv * const env,
                                                 jclass,
                                                 const jlong peer,
                                                 const jint index)
{
    openvrml::mfnode * const mfn =
        boost::polymorphic_downcast<openvrml::mfnode *>(
            reinterpret_cast<openvrml::field_value *>(peer));
    if (!mfn) { return; }

    try {
        openvrml::mfnode::value_type temp = mfn->value();
        temp.erase(temp.begin() + index);
        mfn->value(temp);
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
    }
}

/**
 * @brief Implementation of @c vrml.field.MFNode.setValue().
 *
 * @param[in] env JNI environment.
 * @param[in] clazz MFNode class.
 * @param[in] peer Java @c long representation of the peer instance pointer.
 * @param[in] size Number of elements to set
 * @param[in] nodes Array of BaseNodes to set value of field to
 */
void JNICALL
Java_vrml_field_MFNode_peer_1setValue__JI_3Lvrml_BaseNode_2(
    JNIEnv * const env,
    const jclass clazz,
    const jlong peer,
    const jint size,
    const jobjectArray nodes)
{
    using openvrml::mfnode;

    mfnode * const mfn =
        boost::polymorphic_downcast<mfnode *>(
            reinterpret_cast<openvrml::field_value *>(peer));
    if (!mfn) { return; }

    const jlong new_peer_value =
        Java_vrml_field_MFNode_createPeer(env, clazz, size, nodes);
    if (!new_peer_value) { return; };
    std::auto_ptr<mfnode> new_peer(reinterpret_cast<mfnode *>(new_peer_value));

    mfn->swap(*new_peer);
}

void JNICALL
Java_vrml_field_MFNode_peer_1setValue__JLvrml_field_MFNode_2(
    JNIEnv * const env,
    jclass,
    const jlong peer,
    const jobject nodes)
{
    try {
        using openvrml::mfnode;

        mfnode & mfn =
            *boost::polymorphic_downcast<mfnode *>(
                reinterpret_cast<openvrml::field_value *>(peer));

        const mfnode & new_mfn = get_Field_peer<mfnode>(*env, nodes);

        try {
            mfn = new_mfn;
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFNode_peer_1setValue__JLvrml_field_ConstMFNode_2(
    JNIEnv * const env,
    const jclass clazz,
    const jlong peer,
    const jobject nodes)
{
    Java_vrml_field_MFNode_peer_1setValue__JLvrml_field_MFNode_2(
        env, clazz, peer, nodes);
}

void JNICALL Java_vrml_field_MFNode_peer_1set1Value(JNIEnv * const env,
                                                    jclass,
                                                    const jlong peer,
                                                    const jint index,
                                                    const jobject node)
{
    using openvrml::mfnode;

    mfnode * const mfn =
        boost::polymorphic_downcast<mfnode *>(
            reinterpret_cast<openvrml::field_value *>(peer));
    if (!mfn) { return; }

    try {
        const boost::intrusive_ptr<openvrml::node> & node_peer =
            get_BaseNode_peer(*env, node);

        try {
            mfnode::value_type temp = mfn->value();
            temp.at(index) = node_peer;
            mfn->value(temp);
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        } catch (std::out_of_range & ex) {
            throw_array_index_out_of_bounds(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFNode::addValue.
 *
 * @param[in] env JNI environment
 * @param[in] clazz MFNode class.
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFNode_peer_1addValue(JNIEnv * const env,
                                                   jclass,
                                                   const jlong peer,
                                                   const jobject node)
{
    using openvrml::mfnode;

    mfnode * const mfn =
        boost::polymorphic_downcast<mfnode *>(
            reinterpret_cast<openvrml::field_value *>(peer));
    if (!mfn) { return; }

    try {
        const boost::intrusive_ptr<openvrml::node> & node_peer =
            get_BaseNode_peer(*env, node);

        try {
            mfnode::value_type temp = mfn->value();
            temp.push_back(node_peer);
            mfn->value(temp);
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFNode_peer_1insertValue(JNIEnv * const env,
                                                      jclass,
                                                      const jlong peer,
                                                      const jint index,
                                                      const jobject node)
{
    using openvrml::mfnode;

    mfnode * const mfn =
        boost::polymorphic_downcast<mfnode *>(
            reinterpret_cast<openvrml::field_value *>(peer));
    if (!mfn) { return; }

    try {
        const boost::intrusive_ptr<openvrml::node> & node_peer =
            get_BaseNode_peer(*env, node);

        try {
            mfnode::value_type temp = mfn->value();
            temp.insert(temp.begin() + index, node_peer);
            mfn->value(temp);
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jlong JNICALL
Java_vrml_field_ConstMFRotation_createPeer__I_3F(JNIEnv * const env,
                                                 jclass,
                                                 const jint size,
                                                 const jfloatArray jarr)
{
    if ((env->GetArrayLength(jarr) / 4) < size) {
        throw_array_index_out_of_bounds(*env,
                                        "\"rotations\" array contains fewer "
                                        " than \"size\" color values");
        return 0;
    }
    jfloat * r = env->GetFloatArrayElements(jarr, 0);
    if (!r) { return 0; }
    scope_guard r_guard =
        make_obj_guard(*env, &JNIEnv::ReleaseFloatArrayElements,
                       jarr, r, 0);
    boost::ignore_unused_variable_warning(r_guard);
    std::auto_ptr<openvrml::mfrotation> peer;
    try {
        openvrml::mfrotation::value_type vec(size);
        for (jint i = 0; i < size; ++i, r += 4) {
            vec[i] = openvrml::make_rotation(r[0], r[1], r[2], r[3]);
        }
        peer.reset(new openvrml::mfrotation(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jint JNICALL Java_vrml_field_ConstMFRotation_getSize(JNIEnv * const env,
                                                     const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mfrotation>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

namespace {

    template <> struct component_jni_type<openvrml::rotation> {
        typedef jfloat type;
    };
}

void JNICALL
Java_vrml_field_ConstMFRotation_getValue___3_3F(JNIEnv * const env,
                                                const jobject obj,
                                                const jobjectArray jarr)
{
    try {
        mfield_getValue<openvrml::mfrotation>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_ConstMFRotation_getValue___3F(JNIEnv * const env,
                                              const jobject obj,
                                              const jfloatArray jarr)
{
    try {
        using openvrml::mfrotation;
        const mfrotation & mfr = get_Field_peer<mfrotation>(*env, obj);
        for (size_t i = 0; i < mfr.value().size(); ++i) {
            jfloat * const arrayRegion =
                const_cast<jfloat *>(&mfr.value()[i][0]);
            env->SetFloatArrayRegion(jarr, i * 4, 4, arrayRegion);
            if (env->ExceptionOccurred()) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return;
            }
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_ConstMFRotation_get1Value__I_3F(JNIEnv * const env,
                                                const jobject obj,
                                                const jint element,
                                                const jfloatArray jarr)
{
    try {
        mfield_get1Value<openvrml::mfrotation>(*env, obj, element, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}


jlong JNICALL
Java_vrml_field_MFRotation_createPeer___3_3F(JNIEnv * const env,
                                             jclass,
                                             const jobjectArray jarr)
{
    std::auto_ptr<openvrml::mfrotation> peer;
    try {
        openvrml::mfrotation::value_type vec(env->GetArrayLength(jarr)); // Throws std::bad_alloc.
        for (size_t i = 0; i < vec.size(); ++i) {
            jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return 0;
            }
            if (env->GetArrayLength(element) < 4) {
                throw_array_index_out_of_bounds(*env, "");
                return 0;
            }
            jfloat * const r = env->GetFloatArrayElements(element, 0);
            if (!r) {
                // Presumably we raised an OutOfMemoryError.
                return 0;
            }
            scope_guard r_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseFloatArrayElements,
                               element, r, 0);
            boost::ignore_unused_variable_warning(r_guard);
            vec[i] = openvrml::make_rotation(r[0], r[1], r[2], r[3]);
        }
        peer.reset(new openvrml::mfrotation(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jlong JNICALL
Java_vrml_field_MFRotation_createPeer__I_3F(JNIEnv * const env,
                                            jclass,
                                            const jint size,
                                            const jfloatArray jarr)
{
    if ((env->GetArrayLength(jarr) / 4) < size) {
        throw_array_index_out_of_bounds(*env,
                                        "\"rotations\" array contains fewer "
                                        " than \"size\" color values");
        return 0;
    }
    jfloat * r = env->GetFloatArrayElements(jarr, 0);
    if (!r) { return 0; }
    scope_guard r_guard =
        make_obj_guard(*env, &JNIEnv::ReleaseFloatArrayElements,
                       jarr, r, 0);
    boost::ignore_unused_variable_warning(r_guard);
    std::auto_ptr<openvrml::mfrotation> peer;
    try {
        openvrml::mfrotation::value_type vec(size);
        for (jint i = 0; i < size; ++i, r += 4) {
            vec[i] = openvrml::make_rotation(r[0], r[1], r[2], r[3]);
        }
        peer.reset(new openvrml::mfrotation(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jint JNICALL Java_vrml_field_MFRotation_getSize(JNIEnv * const env,
                                                const jobject obj)
{
    return Java_vrml_field_ConstMFRotation_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFRotation_clear(JNIEnv * const env,
                                              const jobject obj)
{
    try {
        using openvrml::mfrotation;
        mfrotation & mfr = get_Field_peer<mfrotation>(*env, obj);
        mfr.value(mfrotation::value_type());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFRotation_delete(JNIEnv * const env,
                                               const jobject obj,
                                               const jint index)
{
    try {
        mfield_delete<openvrml::mfrotation>(*env, obj, index);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFRotation_getValue___3_3F(JNIEnv * const env,
                                           const jobject obj,
                                           const jobjectArray jarr)
{
    Java_vrml_field_ConstMFRotation_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFRotation_getValue___3F(JNIEnv * const env,
                                                      const jobject obj,
                                                      const jfloatArray jarr)
{
    Java_vrml_field_ConstMFRotation_getValue___3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFRotation_get1Value__I_3F(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jint element,
                                                        const jfloatArray jarr)
{
    Java_vrml_field_ConstMFRotation_get1Value__I_3F(env, obj, element, jarr);
}

void JNICALL
Java_vrml_field_MFRotation_setValue___3_3F(JNIEnv * const env,
                                           const jobject obj,
                                           const jobjectArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFRotation_createPeer___3_3F(env,
                                                     env->GetObjectClass(obj),
                                                     value);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfrotation;
        std::auto_ptr<mfrotation>
            new_peer(reinterpret_cast<mfrotation *>(new_peer_value));
        mfrotation & mfr = get_Field_peer<mfrotation>(*env, obj);
        mfr.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFRotation_setValue__I_3F(JNIEnv * env,
                                                       jobject obj,
                                                       jint size,
                                                       jfloatArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFRotation_createPeer__I_3F(env,
                                                    env->GetObjectClass(obj),
                                                    size,
                                                    value);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfrotation;
        std::auto_ptr<mfrotation>
            new_peer(reinterpret_cast<mfrotation *>(new_peer_value));
        mfrotation & mfr = get_Field_peer<mfrotation>(*env, obj);
        mfr.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFRotation_setValue__Lvrml_field_MFRotation_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject value)
{
    try {
        mfield_setValue<openvrml::mfrotation>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFRotation_setValue__Lvrml_field_ConstMFRotation_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject value)
{
    Java_vrml_field_MFRotation_setValue__Lvrml_field_MFRotation_2(
        env, obj, value);
}

void JNICALL Java_vrml_field_MFRotation_set1Value__IFFFF(JNIEnv * const env,
                                                         const jobject obj,
                                                         const jint index,
                                                         const jfloat x,
                                                         const jfloat y,
                                                         const jfloat z,
                                                         const jfloat angle)
{
    try {
        mfield_set1Value<openvrml::mfrotation>(
            *env, obj, index, openvrml::make_rotation(x, y, z, angle));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFRotation::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFRotation object
 * @param[in] axisX X-component of value to add to end of list
 * @param[in] axisY Y-component of value to add to end of list
 * @param[in] axisZ Z-component of value to add to end of list
 * @param[in] angle Angle of value to add to end of list
 */
void JNICALL Java_vrml_field_MFRotation_addValue__FFFF(JNIEnv * const env,
                                                       const jobject obj,
                                                       const jfloat x,
                                                       const jfloat y,
                                                       const jfloat z,
                                                       const jfloat angle)
{
    try {
        mfield_addValue<openvrml::mfrotation>(
            *env, obj, openvrml::make_rotation(x, y, z, angle));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFRotation_insertValue__IFFFF(JNIEnv * const env,
                                                           const jobject obj,
                                                           const jint index,
                                                           const jfloat x,
                                                           const jfloat y,
                                                           const jfloat z,
                                                           const jfloat angle)
{
    try {
        mfield_insertValue<openvrml::mfrotation>(
            *env, obj, index, openvrml::make_rotation(x, y, z, angle));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jint JNICALL Java_vrml_field_ConstMFString_getSize(JNIEnv * const env,
                                                   const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mfstring>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

void JNICALL Java_vrml_field_ConstMFString_getValue(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jobjectArray jarr)
{
    try {
        using openvrml::mfstring;
        const mfstring & mfstr = get_Field_peer<mfstring>(*env, obj);
        for (size_t i = 0; i < mfstr.value().size(); ++i) {
            const jstring jstr =
                env->NewStringUTF(mfstr.value()[i].c_str());
            env->SetObjectArrayElement(jarr, i, jstr);
            if (env->ExceptionOccurred()) { return; }
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jstring JNICALL Java_vrml_field_ConstMFString_get1Value(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jint index)
{
    try {
        using openvrml::mfstring;
        const mfstring & mfstr = get_Field_peer<mfstring>(*env, obj);
        return env->NewStringUTF(mfstr.value()[index].c_str());
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

jlong JNICALL Java_vrml_field_MFString_createPeer(JNIEnv * const env,
                                                  jclass,
                                                  const jint size,
                                                  const jobjectArray jarr)
{
    std::auto_ptr<openvrml::mfstring> peer;
    try {
        openvrml::mfstring::value_type vec(size);
        for (jint i = 0; i < size; ++i) {
            const jstring el =
                static_cast<jstring>(env->GetObjectArrayElement(jarr, i));
            if (!el) { return 0; } // ArrayIndexOutOfBoundsException
            const char * const temp = env->GetStringUTFChars(el, 0);
            if (!temp) { return 0; } // OutOfMemoryError
            scope_guard temp_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars, el, temp);
            boost::ignore_unused_variable_warning(temp_guard);
            vec[i] = temp;
        }
        peer.reset(new openvrml::mfstring(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jint JNICALL Java_vrml_field_MFString_getSize(JNIEnv * const env,
                                              const jobject obj)
{
    return Java_vrml_field_ConstMFString_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFString_clear(JNIEnv * const env,
                                            const jobject obj)
{
    try {
        mfield_clear<openvrml::mfstring>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFString_delete(JNIEnv * const env,
                                             const jobject obj,
                                             const jint index)
{
    try {
        mfield_delete<openvrml::mfstring>(*env, obj, index);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFString_getValue(JNIEnv * const env,
                                               const jobject obj,
                                               const jobjectArray jarr)
{
    Java_vrml_field_ConstMFString_getValue(env, obj, jarr);
}

jstring JNICALL Java_vrml_field_MFString_get1Value(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jint index)
{
    return Java_vrml_field_ConstMFString_get1Value(env, obj, index);
}

void JNICALL
Java_vrml_field_MFString_setValue__I_3Ljava_lang_String_2(
    JNIEnv * const env,
    const jobject obj,
    const jint size,
    const jobjectArray jarr)
{
    const jlong new_peer_value =
        Java_vrml_field_MFString_createPeer(env,
                                            env->GetObjectClass(obj),
                                            size,
                                            jarr);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfstring;
        std::auto_ptr<mfstring>
            new_peer(reinterpret_cast<mfstring *>(new_peer_value));
        mfstring & mfstr = get_Field_peer<mfstring>(*env, obj);
        mfstr.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2(JNIEnv * const env,
                                                          const jobject obj,
                                                          const jobject value)
{
    try {
        mfield_setValue<openvrml::mfstring>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFString_setValue__Lvrml_field_ConstMFString_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject value)
{
    Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2(env, obj, value);
}

void JNICALL
Java_vrml_field_MFString_set1Value__ILjava_lang_String_2(JNIEnv * const env,
                                                         const jobject obj,
                                                         const jint index,
                                                         const jstring value)
{
    try {
        using openvrml::mfstring;
        mfstring & mfstr = get_Field_peer<mfstring>(*env, obj);
        try {
            const char * utf8chars = env->GetStringUTFChars(value, 0);
            if (!utf8chars) { return; } // OutOfMemoryError
            scope_guard utf8chars_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars,
                               value, utf8chars);
            boost::ignore_unused_variable_warning(utf8chars_guard);
            mfstring::value_type temp = mfstr.value();
            temp.at(index) = utf8chars;
            mfstr.value(temp);
        } catch (std::out_of_range & ex) {
            throw_array_index_out_of_bounds(*env, ex.what());
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFString::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFString object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFString_addValue__Ljava_lang_String_2(JNIEnv * env,
                                                       jobject obj,
                                                       jstring value)
{
    try {
        using openvrml::mfstring;
        mfstring & mfstr = get_Field_peer<mfstring>(*env, obj);
        try {
            const char * utf8chars = env->GetStringUTFChars(value, 0);
            if (!utf8chars) { return; } // OutOfMemoryError
            scope_guard utf8chars_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars,
                               value, utf8chars);
            boost::ignore_unused_variable_warning(utf8chars_guard);
            mfstring::value_type temp = mfstr.value();
            temp.push_back(utf8chars);
            mfstr.value(temp);
        } catch (std::out_of_range & ex) {
            throw_array_index_out_of_bounds(*env, ex.what());
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFString::insertValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFString object
 * @param[in] index Position to insert value at
 * @param[in] value Value to insert.
 */
void JNICALL
Java_vrml_field_MFString_insertValue__ILjava_lang_String_2(JNIEnv * const env,
                                                           const jobject obj,
                                                           const jint index,
                                                           const jstring value)
{
    try {
        using openvrml::mfstring;
        mfstring & mfstr = get_Field_peer<mfstring>(*env, obj);
        try {
            const char * utf8chars = env->GetStringUTFChars(value, 0);
            if (!utf8chars) { return; } // OutOfMemoryError
            scope_guard utf8chars_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseStringUTFChars,
                               value, utf8chars);
            boost::ignore_unused_variable_warning(utf8chars_guard);
            mfstring::value_type temp = mfstr.value();
            temp.insert(temp.begin() + index, utf8chars);
            mfstr.value(temp);
        } catch (std::out_of_range & ex) {
            throw_array_index_out_of_bounds(*env, ex.what());
        } catch (std::bad_alloc & ex) {
            throw_out_of_memory(*env, ex.what());
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jint JNICALL Java_vrml_field_ConstMFTime_getSize(JNIEnv * const env,
                                                 const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mftime>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

void JNICALL Java_vrml_field_ConstMFTime_getValue(JNIEnv * const env,
                                                  const jobject obj,
                                                  const jdoubleArray jarr)
{
    try {
        mfield_getValue<openvrml::mftime>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jdouble JNICALL Java_vrml_field_ConstMFTime_get1Value(JNIEnv * const env,
                                                      const jobject obj,
                                                      const jint index)
{
    try {
        return mfield_get1Value<openvrml::mftime>(*env, obj, index);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0.0;
}

jlong JNICALL Java_vrml_field_MFTime_createPeer(JNIEnv * const env,
                                                jclass,
                                                const jint size,
                                                const jdoubleArray value)
{
    return mfield_createPeer<openvrml::mftime>(*env, size, value);
}

jint JNICALL Java_vrml_field_MFTime_getSize(JNIEnv * const env,
                                            const jobject obj)
{
    return Java_vrml_field_ConstMFTime_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFTime_clear(JNIEnv * const env,
                                          const jobject obj)
{
    try {
        mfield_clear<openvrml::mftime>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFTime_delete(JNIEnv * const env,
                                           const jobject obj,
                                           const jint index)
{
    try {
        mfield_delete<openvrml::mftime>(*env, obj, index);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFTime_getValue(JNIEnv * const env,
                                             const jobject obj,
                                             const jdoubleArray jarr)
{
    Java_vrml_field_ConstMFTime_getValue(env, obj, jarr);
}

jdouble JNICALL Java_vrml_field_MFTime_get1Value(JNIEnv * const env,
                                                 const jobject obj,
                                                 const jint index)
{
    return Java_vrml_field_ConstMFTime_get1Value(env, obj, index);
}

void JNICALL Java_vrml_field_MFTime_setValue__I_3D(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jint size,
                                                   const jdoubleArray value)
{
    try {
        mfield_setValue<openvrml::mftime>(*env, obj, size, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFTime_setValue__Lvrml_field_MFTime_2(JNIEnv * const env,
                                                      const jobject obj,
                                                      const jobject value)
{
    try {
        mfield_setValue<openvrml::mftime>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFTime_setValue__Lvrml_field_ConstMFTime_2(JNIEnv * const env,
                                                           const jobject obj,
                                                           const jobject value)
{
    Java_vrml_field_MFTime_setValue__Lvrml_field_MFTime_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFTime_set1Value__ID(JNIEnv * const env,
                                                  const jobject obj,
                                                  const jint index,
                                                  const jdouble value)
{
    try {
        mfield_set1Value<openvrml::mftime>(*env, obj, index, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFTime::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFTime object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFTime_addValue__D(JNIEnv * env,
                                                jobject obj,
                                                jdouble value)
{
    try {
        mfield_addValue<openvrml::mftime>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFTime_insertValue__ID(JNIEnv * env,
                                                    jobject obj,
                                                    jint index,
                                                    jdouble value)
{
    try {
        mfield_insertValue<openvrml::mftime>(*env, obj, index, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jint JNICALL Java_vrml_field_ConstMFVec2f_getSize(JNIEnv * const env,
                                                  const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mfvec2f>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

namespace {
    template <> struct component_jni_type<openvrml::vec2f> {
        typedef jfloat type;
    };

    template <> struct num_components<openvrml::vec2f> {
        static const size_t value = 2;
    };
}

void JNICALL
Java_vrml_field_ConstMFVec2f_getValue___3_3F(JNIEnv * const env,
                                             const jobject obj,
                                             const jobjectArray jarr)
{
    try {
        mfield_getValue<openvrml::mfvec2f>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_ConstMFVec2f_getValue___3F(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jfloatArray jarr)
{
    try {
        mfield_getValue<openvrml::mfvec2f>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstMFVec2f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec2f object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y value
 */
void JNICALL
Java_vrml_field_ConstMFVec2f_get1Value__I_3F(JNIEnv * const env,
                                             const jobject obj,
                                             const jint element,
                                             const jfloatArray jarr)
{
    try {
        mfield_get1Value<openvrml::mfvec2f>(*env, obj, element, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jlong JNICALL Java_vrml_field_MFVec2f_createPeer___3_3F(JNIEnv * const env,
                                                        jclass,
                                                        const jobjectArray jarr)
{
    std::auto_ptr<openvrml::mfvec2f> peer;
    try {
        openvrml::mfvec2f::value_type vec(env->GetArrayLength(jarr)); // Throws std::bad_alloc.
        for (size_t i = 0; i < vec.size(); ++i) {
            jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return 0;
            }
            if (env->GetArrayLength(element) < 2) {
                throw_array_index_out_of_bounds(*env, "");
                return 0;
            }
            jfloat * const v = env->GetFloatArrayElements(element, 0);
            if (!v) {
                // Presumably we raised an OutOfMemoryError.
                return 0;
            }
            scope_guard v_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseFloatArrayElements,
                               element, v, 0);
            boost::ignore_unused_variable_warning(v_guard);
            vec[i] = openvrml::make_vec2f(v[0], v[1]);
        }
        peer.reset(new openvrml::mfvec2f(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jlong JNICALL Java_vrml_field_MFVec2f_createPeer__I_3F(JNIEnv * const env,
                                                       jclass,
                                                       const jint size,
                                                       const jfloatArray jarr)
{
    if ((env->GetArrayLength(jarr) / 2) < size) {
        throw_array_index_out_of_bounds(*env,
                                        "\"vec2s\" array contains fewer than "
                                        "\"size\" vector values");
        return 0;
    }
    jfloat * v = env->GetFloatArrayElements(jarr, 0);
    if (!v) { return 0; }
    scope_guard v_guard =
        make_obj_guard(*env, &JNIEnv::ReleaseFloatArrayElements,
                       jarr, v, 0);
    boost::ignore_unused_variable_warning(v_guard);
    std::auto_ptr<openvrml::mfvec2f> peer;
    try {
        openvrml::mfvec2f::value_type vec(size);
        for (jint i = 0; i < size; ++i, v += 2) {
            vec[i] = openvrml::make_vec2f(v[0], v[1]);
        }
        peer.reset(new openvrml::mfvec2f(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jint JNICALL Java_vrml_field_MFVec2f_getSize(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstMFVec2f_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFVec2f_clear(JNIEnv * const env,
                                           const jobject obj)
{
    try {
        mfield_clear<openvrml::mfvec2f>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec2f_delete(JNIEnv * const env,
                                            const jobject obj,
                                            const jint n)
{
    try {
        mfield_delete<openvrml::mfvec2f>(*env, obj, n);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec2f_getValue___3_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jobjectArray jarr)
{
    Java_vrml_field_ConstMFVec2f_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec2f_getValue___3F(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jfloatArray jarr)
{
    Java_vrml_field_ConstMFVec2f_getValue___3F(env, obj, jarr);
}

/**
 * @brief Implementation of MFVec2f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec2f object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y value
 */
void JNICALL Java_vrml_field_MFVec2f_get1Value__I_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jint element,
                                                     const jfloatArray jarr)
{
    Java_vrml_field_ConstMFVec2f_get1Value__I_3F(env, obj, element, jarr);
}

void JNICALL Java_vrml_field_MFVec2f_setValue___3_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jobjectArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFVec2f_createPeer___3_3F(env,
                                                  env->GetObjectClass(obj),
                                                  value);
    if (!new_peer_value) { return; }
    try {
        using openvrml::mfvec2f;
        std::auto_ptr<mfvec2f>
            new_peer(reinterpret_cast<mfvec2f *>(new_peer_value));
        mfvec2f & mfv = get_Field_peer<mfvec2f>(*env, obj);
        mfv.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec2f_setValue__I_3F(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint size,
                                                    const jfloatArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFVec2f_createPeer__I_3F(env,
                                                 env->GetObjectClass(obj),
                                                 size,
                                                 value);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfvec2f;
        std::auto_ptr<mfvec2f>
            new_peer(reinterpret_cast<mfvec2f *>(new_peer_value));
        mfvec2f & mfv = get_Field_peer<mfvec2f>(*env, obj);
        mfv.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFVec2f_setValue__Lvrml_field_MFVec2f_2(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jobject value)
{
    try {
        mfield_setValue<openvrml::mfvec2f>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFVec2f_setValue__Lvrml_field_ConstMFVec2f_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject value)
{
    Java_vrml_field_MFVec2f_setValue__Lvrml_field_MFVec2f_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFVec2f_set1Value__IFF(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint index,
                                                    const jfloat x,
                                                    const jfloat y)
{
    try {
        mfield_set1Value<openvrml::mfvec2f>(
            *env, obj, index, openvrml::make_vec2f(x, y));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFVec2f::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec2f object
 * @param[in] x X-component of vector to add to end of list
 * @param[in] y Y-component of vector to add to end of list
 */
void JNICALL Java_vrml_field_MFVec2f_addValue__FF(JNIEnv * const env,
                                                  const jobject obj,
                                                  const jfloat x,
                                                  const jfloat y)
{
    try {
        mfield_addValue<openvrml::mfvec2f>(*env, obj,
                                           openvrml::make_vec2f(x, y));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec2f_insertValue__IFF(JNIEnv * const env,
                                                      const jobject obj,
                                                      const jint index,
                                                      const jfloat x,
                                                      const jfloat y)
{
    try {
        mfield_insertValue<openvrml::mfvec2f>(
            *env, obj, index, openvrml::make_vec2f(x, y));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jlong JNICALL
Java_vrml_field_ConstMFVec2d_createPeer__I_3F(JNIEnv * const env,
                                              jclass,
                                              const jint size,
                                              const jdoubleArray jarr)
{
    if ((env->GetArrayLength(jarr) / 2) < size) {
        throw_array_index_out_of_bounds(*env,
                                        "\"vec2s\" array contains fewer than "
                                        "\"size\" vector values");
        return 0;
    }
    jdouble * v = env->GetDoubleArrayElements(jarr, 0);
    if (!v) { return 0; }
    scope_guard v_guard =
        make_obj_guard(*env, &JNIEnv::ReleaseDoubleArrayElements,
                       jarr, v, 0);
    boost::ignore_unused_variable_warning(v_guard);
    std::auto_ptr<openvrml::mfvec2d> peer;
    try {
        openvrml::mfvec2d::value_type vec(size);
        for (jint i = 0; i < size; ++i, v += 2) {
            vec[i] = openvrml::make_vec2d(v[0], v[1]);
        }
        peer.reset(new openvrml::mfvec2d(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jint JNICALL Java_vrml_field_ConstMFVec2d_getSize(JNIEnv * const env,
                                                  const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mfvec2f>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

namespace {
    template <> struct component_jni_type<openvrml::vec2d> {
        typedef jdouble type;
    };

    template <> struct num_components<openvrml::vec2d> {
        static const size_t value = 2;
    };
}

void JNICALL
Java_vrml_field_ConstMFVec2d_getValue___3_3F(JNIEnv * const env,
                                             const jobject obj,
                                             const jobjectArray jarr)
{
    try {
        mfield_getValue<openvrml::mfvec2d>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_ConstMFVec2d_getValue___3F(JNIEnv * const env,
                                           const jobject obj,
                                           const jdoubleArray jarr)
{
    try {
        mfield_getValue<openvrml::mfvec2d>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstMFVec2d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec2d object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y value
 */
void JNICALL
Java_vrml_field_ConstMFVec2d_get1Value__I_3F(JNIEnv * const env,
                                             const jobject obj,
                                             const jint element,
                                             const jdoubleArray jarr)
{
    try {
        mfield_get1Value<openvrml::mfvec2d>(*env, obj, element, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jlong JNICALL
Java_vrml_field_MFVec2d_createPeer___3_3F(JNIEnv * const env,
                                          jclass,
                                          const jobjectArray jarr)
{
    std::auto_ptr<openvrml::mfvec2d> peer;
    try {
        openvrml::mfvec2d::value_type vec(env->GetArrayLength(jarr)); // Throws std::bad_alloc.
        for (size_t i = 0; i < vec.size(); ++i) {
            jdoubleArray element =
                static_cast<jdoubleArray>(env->GetObjectArrayElement(jarr, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return 0;
            }
            if (env->GetArrayLength(element) < 2) {
                throw_array_index_out_of_bounds(*env, "");
                return 0;
            }
            jdouble * const v = env->GetDoubleArrayElements(element, 0);
            if (!v) {
                // Presumably we raised an OutOfMemoryError.
                return 0;
            }
            scope_guard v_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseDoubleArrayElements,
                               element, v, 0);
            boost::ignore_unused_variable_warning(v_guard);
            vec[i] = openvrml::make_vec2d(v[0], v[1]);
        }
        peer.reset(new openvrml::mfvec2d(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jlong JNICALL Java_vrml_field_MFVec2d_createPeer__I_3F(JNIEnv * const env,
                                                       jclass,
                                                       const jint size,
                                                       const jdoubleArray jarr)
{
    if ((env->GetArrayLength(jarr) / 2) < size) {
        throw_array_index_out_of_bounds(*env,
                                        "\"vec2s\" array contains fewer than "
                                        "\"size\" vector values");
        return 0;
    }
    jdouble * v = env->GetDoubleArrayElements(jarr, 0);
    if (!v) { return 0; }
    scope_guard v_guard =
        make_obj_guard(*env, &JNIEnv::ReleaseDoubleArrayElements,
                       jarr, v, 0);
    boost::ignore_unused_variable_warning(v_guard);
    std::auto_ptr<openvrml::mfvec2d> peer;
    try {
        openvrml::mfvec2d::value_type vec(size);
        for (jint i = 0; i < size; ++i, v += 2) {
            vec[i] = openvrml::make_vec2d(v[0], v[1]);
        }
        peer.reset(new openvrml::mfvec2d(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jint JNICALL Java_vrml_field_MFVec2d_getSize(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstMFVec2d_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFVec2d_clear(JNIEnv * const env,
                                           const jobject obj)
{
    try {
        mfield_clear<openvrml::mfvec2d>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec2d_delete(JNIEnv * const env,
                                            const jobject obj,
                                            const jint n)
{
    try {
        mfield_delete<openvrml::mfvec2d>(*env, obj, n);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec2d_getValue___3_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jobjectArray jarr)
{
    Java_vrml_field_ConstMFVec2d_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec2d_getValue___3F(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jdoubleArray jarr)
{
    Java_vrml_field_ConstMFVec2d_getValue___3F(env, obj, jarr);
}

/**
 * @brief Implementation of MFVec2d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec2d object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y value
 */
void JNICALL Java_vrml_field_MFVec2d_get1Value__I_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jint element,
                                                     const jdoubleArray jarr)
{
    Java_vrml_field_ConstMFVec2d_get1Value__I_3F(env, obj, element, jarr);
}

void JNICALL Java_vrml_field_MFVec2d_setValue___3_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jobjectArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFVec2d_createPeer___3_3F(env,
                                                  env->GetObjectClass(obj),
                                                  value);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfvec2d;
        std::auto_ptr<mfvec2d>
            new_peer(reinterpret_cast<mfvec2d *>(new_peer_value));
        mfvec2d & mfv = get_Field_peer<mfvec2d>(*env, obj);
        mfv.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec2d_setValue__I_3F(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint size,
                                                    const jdoubleArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFVec2d_createPeer__I_3F(env,
                                                 env->GetObjectClass(obj),
                                                 size,
                                                 value);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfvec2d;
        std::auto_ptr<mfvec2d>
            new_peer(reinterpret_cast<mfvec2d *>(new_peer_value));
        mfvec2d & mfv = get_Field_peer<mfvec2d>(*env, obj);
        mfv.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFVec2d_setValue__Lvrml_field_MFVec2d_2(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jobject value)
{
    try {
        mfield_setValue<openvrml::mfvec2d>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFVec2d_setValue__Lvrml_field_ConstMFVec2d_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject value)
{
    Java_vrml_field_MFVec2d_setValue__Lvrml_field_MFVec2d_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFVec2d_set1Value__IFF(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint index,
                                                    const jdouble x,
                                                    const jdouble y)
{
    try {
        mfield_set1Value<openvrml::mfvec2d>(
            *env, obj, index, openvrml::make_vec2d(x, y));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFVec2d::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec2d object
 * @param[in] x X-component of vector to add to end of list
 * @param[in] y Y-component of vector to add to end of list
 */
void JNICALL Java_vrml_field_MFVec2d_addValue__FF(JNIEnv * const env,
                                                  const jobject obj,
                                                  const jdouble x,
                                                  const jdouble y)
{
    try {
        mfield_addValue<openvrml::mfvec2d>(*env, obj,
                                           openvrml::make_vec2d(x, y));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec2d_insertValue__IFF(JNIEnv * const env,
                                                      const jobject obj,
                                                      const jint index,
                                                      const jdouble x,
                                                      const jdouble y)
{
    try {
        mfield_insertValue<openvrml::mfvec2d>(
            *env, obj, index, openvrml::make_vec2d(x, y));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}


jint JNICALL Java_vrml_field_ConstMFVec3f_getSize(JNIEnv * const env,
                                                  const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mfvec3f>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

namespace {
    template <> struct component_jni_type<openvrml::vec3f> {
        typedef jfloat type;
    };

    template <> struct num_components<openvrml::vec3f> {
        static const size_t value = 3;
    };
}

void JNICALL
Java_vrml_field_ConstMFVec3f_getValue___3_3F(JNIEnv * const env,
                                             const jobject obj,
                                             const jobjectArray jarr)
{
    try {
        mfield_getValue<openvrml::mfvec3f>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_ConstMFVec3f_getValue___3F(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jfloatArray jarr)
{
    try {
        mfield_getValue<openvrml::mfvec3f>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstMFVec3f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec3f object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y,z value
 */
void JNICALL
Java_vrml_field_ConstMFVec3f_get1Value__I_3F(JNIEnv * const env,
                                             const jobject obj,
                                             const jint element,
                                             const jfloatArray jarr)
{
    try {
        mfield_get1Value<openvrml::mfvec3f>(*env, obj, element, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

jlong JNICALL
Java_vrml_field_MFVec3f_createPeer___3_3F(JNIEnv * const env,
                                          jclass,
                                          const jobjectArray jarr)
{
    std::auto_ptr<openvrml::mfvec3f> peer;
    try {
        openvrml::mfvec3f::value_type vec(env->GetArrayLength(jarr)); // Throws std::bad_alloc.
        for (size_t i = 0; i < vec.size(); ++i) {
            jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return 0;
            }
            if (env->GetArrayLength(element) < 3) {
                throw_array_index_out_of_bounds(*env, "");
                return 0;
            }
            jfloat * const v = env->GetFloatArrayElements(element, 0);
            if (!v) {
                // Presumably we raised an OutOfMemoryError.
                return 0;
            }
            scope_guard v_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseFloatArrayElements,
                               element, v, 0);
            boost::ignore_unused_variable_warning(v_guard);
            vec[i] = openvrml::make_vec3f(v[0], v[1], v[2]);
        }
        peer.reset(new openvrml::mfvec3f(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jlong JNICALL Java_vrml_field_MFVec3f_createPeer__I_3F(JNIEnv * const env,
                                                       jclass,
                                                       const jint size,
                                                       const jfloatArray jarr)
{
    if ((env->GetArrayLength(jarr) / 3) < size) {
        throw_array_index_out_of_bounds(*env,
                                        "\"vec3s\" array contains fewer than "
                                        "\"size\" vector values");
        return 0;
    }
    jfloat * v = env->GetFloatArrayElements(jarr, 0);
    if (!v) { return 0; }
    scope_guard v_guard =
        make_obj_guard(*env, &JNIEnv::ReleaseFloatArrayElements,
                       jarr, v, 0);
    boost::ignore_unused_variable_warning(v_guard);
    std::auto_ptr<openvrml::mfvec3f> peer;
    try {
        openvrml::mfvec3f::value_type vec(size);
        for (jint i = 0; i < size; ++i, v += 2) {
            vec[i] = openvrml::make_vec3f(v[0], v[1], v[2]);
        }
        peer.reset(new openvrml::mfvec3f(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jint JNICALL Java_vrml_field_MFVec3f_getSize(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstMFVec3f_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFVec3f_clear(JNIEnv * const env,
                                           const jobject obj)
{
    try {
        mfield_clear<openvrml::mfvec3f>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec3f_delete(JNIEnv * const env,
                                            const jobject obj,
                                            const jint n)
{
    try {
        mfield_delete<openvrml::mfvec3f>(*env, obj, n);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec3f_getValue___3_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jobjectArray jarr)
{
    Java_vrml_field_ConstMFVec3f_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec3f_getValue___3F(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jfloatArray jarr)
{
    Java_vrml_field_ConstMFVec3f_getValue___3F(env, obj, jarr);
}

/**
 * @brief Implementation of MFVec3f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec3f object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y,z value
 */
void JNICALL Java_vrml_field_MFVec3f_get1Value__I_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jint element,
                                                     const jfloatArray jarr)
{
    Java_vrml_field_ConstMFVec3f_get1Value__I_3F(env, obj, element, jarr);
}

void JNICALL Java_vrml_field_MFVec3f_setValue___3_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jobjectArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFVec3f_createPeer___3_3F(env,
                                                  env->GetObjectClass(obj),
                                                  value);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfvec3f;
        std::auto_ptr<mfvec3f>
            new_peer(reinterpret_cast<mfvec3f *>(new_peer_value));
        mfvec3f & mfv = get_Field_peer<mfvec3f>(*env, obj);
        mfv.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec3f_setValue__I_3F(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint size,
                                                    const jfloatArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFVec3f_createPeer__I_3F(env,
                                                 env->GetObjectClass(obj),
                                                 size,
                                                 value);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfvec3f;
        std::auto_ptr<mfvec3f>
            new_peer(reinterpret_cast<mfvec3f *>(new_peer_value));
        mfvec3f & mfv = get_Field_peer<mfvec3f>(*env, obj);
        mfv.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFVec3f_setValue__Lvrml_field_MFVec3f_2(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jobject value)
{
    try {
        mfield_setValue<openvrml::mfvec3f>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFVec3f_setValue__Lvrml_field_ConstMFVec3f_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject value)
{
    Java_vrml_field_MFVec3f_setValue__Lvrml_field_MFVec3f_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFVec3f_set1Value__IFFF(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jint index,
                                                     const jfloat x,
                                                     const jfloat y,
                                                     const jfloat z)
{
    try {
        mfield_set1Value<openvrml::mfvec3f>(
            *env, obj, index, openvrml::make_vec3f(x, y, z));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFVec3f::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec3f object
 * @param[in] x X-component of vector to add to end of list
 * @param[in] y Y-component of vector to add to end of list
 * @param[in] z Z-component of vector to add to end of list
 */
void JNICALL Java_vrml_field_MFVec3f_addValue__FFF(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jfloat x,
                                                   const jfloat y,
                                                   const jfloat z)
{
    try {
        mfield_addValue<openvrml::mfvec3f>(
            *env, obj, openvrml::make_vec3f(x, y, z));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec3f_insertValue__IFFF(JNIEnv * const env,
                                                       const jobject obj,
                                                       const jint index,
                                                       const jfloat x,
                                                       const jfloat y,
                                                       const jfloat z)
{
    try {
        mfield_insertValue<openvrml::mfvec3f>(
            *env, obj, index, openvrml::make_vec3f(x, y, z));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}


jint JNICALL Java_vrml_field_ConstMFVec3d_getSize(JNIEnv * const env,
                                                  const jobject obj)
{
    try {
        return mfield_getSize<openvrml::mfvec3d>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
    return 0;
}

namespace {
    template <> struct component_jni_type<openvrml::vec3d> {
        typedef jdouble type;
    };

    template <> struct num_components<openvrml::vec3d> {
        static const size_t value = 3;
    };
}

void JNICALL
Java_vrml_field_ConstMFVec3d_getValue___3_3F(JNIEnv * const env,
                                             const jobject obj,
                                             const jobjectArray jarr)
{
    try {
        mfield_getValue<openvrml::mfvec3d>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_ConstMFVec3d_getValue___3F(JNIEnv * const env,
                                           const jobject obj,
                                           const jdoubleArray jarr)
{
    try {
        mfield_getValue<openvrml::mfvec3d>(*env, obj, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of ConstMFVec3d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec3d object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y,z value
 */
void JNICALL
Java_vrml_field_ConstMFVec3d_get1Value__I_3F(JNIEnv * const env,
                                             const jobject obj,
                                             const jint element,
                                             const jdoubleArray jarr)
{
    try {
        mfield_get1Value<openvrml::mfvec3d>(*env, obj, element, jarr);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}


jlong JNICALL
Java_vrml_field_MFVec3d_createPeer___3_3F(JNIEnv * const env,
                                          jclass,
                                          const jobjectArray jarr)
{
    std::auto_ptr<openvrml::mfvec3d> peer;
    try {
        openvrml::mfvec3d::value_type vec(env->GetArrayLength(jarr)); // Throws std::bad_alloc.
        for (size_t i = 0; i < vec.size(); ++i) {
            jdoubleArray element =
                static_cast<jdoubleArray>(env->GetObjectArrayElement(jarr, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return 0;
            }
            if (env->GetArrayLength(element) < 3) {
                throw_array_index_out_of_bounds(*env, "");
                return 0;
            }
            jdouble * const v = env->GetDoubleArrayElements(element, 0);
            if (!v) {
                // Presumably we raised an OutOfMemoryError.
                return 0;
            }
            scope_guard v_guard =
                make_obj_guard(*env, &JNIEnv::ReleaseDoubleArrayElements,
                               element, v, 0);
            boost::ignore_unused_variable_warning(v_guard);
            vec[i] = openvrml::make_vec3d(v[0], v[1], v[2]);
        }
        peer.reset(new openvrml::mfvec3d(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jlong JNICALL Java_vrml_field_MFVec3d_createPeer__I_3F(JNIEnv * const env,
                                                       jclass,
                                                       const jint size,
                                                       const jdoubleArray jarr)
{
    if ((env->GetArrayLength(jarr) / 3) < size) {
        throw_array_index_out_of_bounds(*env,
                                        "\"vec3s\" array contains fewer than "
                                        "\"size\" vector values");
        return 0;
    }
    jdouble * v = env->GetDoubleArrayElements(jarr, 0);
    if (!v) { return 0; }
    scope_guard v_guard =
        make_obj_guard(*env, &JNIEnv::ReleaseDoubleArrayElements,
                       jarr, v, 0);
    boost::ignore_unused_variable_warning(v_guard);
    std::auto_ptr<openvrml::mfvec3d> peer;
    try {
        openvrml::mfvec3d::value_type vec(size);
        for (jint i = 0; i < size; ++i, v += 2) {
            vec[i] = openvrml::make_vec3d(v[0], v[1], v[2]);
        }
        peer.reset(new openvrml::mfvec3d(vec));
    } catch (std::bad_alloc & ex) {
        throw_out_of_memory(*env, ex.what());
        return 0;
    }
    return jlong(peer.release());
}

jint JNICALL Java_vrml_field_MFVec3d_getSize(JNIEnv * const env,
                                             const jobject obj)
{
    return Java_vrml_field_ConstMFVec3d_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFVec3d_clear(JNIEnv * const env,
                                           const jobject obj)
{
    try {
        mfield_clear<openvrml::mfvec3d>(*env, obj);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec3d_delete(JNIEnv * const env,
                                            const jobject obj,
                                            const jint n)
{
    try {
        mfield_delete<openvrml::mfvec3d>(*env, obj, n);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec3d_getValue___3_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jobjectArray jarr)
{
    Java_vrml_field_ConstMFVec3d_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec3d_getValue___3F(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jdoubleArray jarr)
{
    Java_vrml_field_ConstMFVec3d_getValue___3F(env, obj, jarr);
}

/**
 * @brief Implementation of MFVec3d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec3d object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y,z value
 */
void JNICALL Java_vrml_field_MFVec3d_get1Value__I_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jint element,
                                                     const jdoubleArray jarr)
{
    Java_vrml_field_ConstMFVec3d_get1Value__I_3F(env, obj, element, jarr);
}

void JNICALL Java_vrml_field_MFVec3d_setValue___3_3F(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jobjectArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFVec3d_createPeer___3_3F(env,
                                                  env->GetObjectClass(obj),
                                                  value);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfvec3d;
        std::auto_ptr<mfvec3d>
            new_peer(reinterpret_cast<mfvec3d *>(new_peer_value));
        mfvec3d & mfv = get_Field_peer<mfvec3d>(*env, obj);
        mfv.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec3d_setValue__I_3F(JNIEnv * const env,
                                                    const jobject obj,
                                                    const jint size,
                                                    const jdoubleArray value)
{
    const jlong new_peer_value =
        Java_vrml_field_MFVec3d_createPeer__I_3F(env,
                                                 env->GetObjectClass(obj),
                                                 size,
                                                 value);
    if (!new_peer_value) { return; }

    try {
        using openvrml::mfvec3d;
        std::auto_ptr<mfvec3d>
            new_peer(reinterpret_cast<mfvec3d *>(new_peer_value));
        mfvec3d & mfv = get_Field_peer<mfvec3d>(*env, obj);
        mfv.swap(*new_peer);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFVec3d_setValue__Lvrml_field_MFVec3d_2(JNIEnv * const env,
                                                        const jobject obj,
                                                        const jobject value)
{
    try {
        mfield_setValue<openvrml::mfvec3d>(*env, obj, value);
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL
Java_vrml_field_MFVec3d_setValue__Lvrml_field_ConstMFVec3d_2(
    JNIEnv * const env,
    const jobject obj,
    const jobject value)
{
    Java_vrml_field_MFVec3d_setValue__Lvrml_field_MFVec3d_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFVec3d_set1Value__IFFF(JNIEnv * const env,
                                                     const jobject obj,
                                                     const jint index,
                                                     const jdouble x,
                                                     const jdouble y,
                                                     const jdouble z)
{
    try {
        mfield_set1Value<openvrml::mfvec3d>(
            *env, obj, index, openvrml::make_vec3d(x, y, z));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

/**
 * @brief Implementation of MFVec3d::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec3d object
 * @param[in] x X-component of vector to add to end of list
 * @param[in] y Y-component of vector to add to end of list
 * @param[in] z Z-component of vector to add to end of list
 */
void JNICALL Java_vrml_field_MFVec3d_addValue__FFF(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jdouble x,
                                                   const jdouble y,
                                                   const jdouble z)
{
    try {
        mfield_addValue<openvrml::mfvec3d>(
            *env, obj, openvrml::make_vec3d(x, y, z));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}

void JNICALL Java_vrml_field_MFVec3d_insertValue__IFFF(JNIEnv * const env,
                                                       const jobject obj,
                                                       const jint index,
                                                       const jdouble x,
                                                       const jdouble y,
                                                       const jdouble z)
{
    try {
        mfield_insertValue<openvrml::mfvec3d>(
            *env, obj, index, openvrml::make_vec3d(x, y, z));
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    }
}
