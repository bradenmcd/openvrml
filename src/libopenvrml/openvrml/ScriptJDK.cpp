// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001  Henri Manson
// Copyright 2002  Thomas Flynn
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

//  ScriptJDK.cpp
//  java script objects.
//  http://java.sun.com/docs/books/tutorial/native1.1/invoking/invo.html
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# if OPENVRML_ENABLE_SCRIPT_NODE_JAVA
#   include "ScriptJDK.h"

#   include <algorithm>
#   include <cstdio>
#   include <cstring>
#   include <sstream>

#   include <vrml_Browser.h>
#   include <vrml_Event.h>
#   include <vrml_Field.h>
#   include <vrml_MField.h>
#   include <vrml_ConstField.h>
#   include <vrml_ConstMField.h>
#   include <vrml_BaseNode.h>
#   include <vrml_field_SFBool.h>
#   include <vrml_field_SFColor.h>
#   include <vrml_field_SFFloat.h>
#   include <vrml_field_SFDouble.h>
#   include <vrml_field_SFImage.h>
#   include <vrml_field_SFInt32.h>
#   include <vrml_field_SFNode.h>
#   include <vrml_field_SFRotation.h>
#   include <vrml_field_SFString.h>
#   include <vrml_field_SFTime.h>
#   include <vrml_field_SFVec2f.h>
#   include <vrml_field_SFVec2d.h>
#   include <vrml_field_SFVec3f.h>
#   include <vrml_field_SFVec3d.h>
#   include <vrml_field_MFBool.h>
#   include <vrml_field_MFColor.h>
#   include <vrml_field_MFFloat.h>
#   include <vrml_field_MFDouble.h>
#   include <vrml_field_MFInt32.h>
#   include <vrml_field_MFNode.h>
#   include <vrml_field_MFRotation.h>
#   include <vrml_field_MFString.h>
#   include <vrml_field_MFTime.h>
#   include <vrml_field_MFVec2f.h>
#   include <vrml_field_MFVec2d.h>
#   include <vrml_field_MFVec3f.h>
#   include <vrml_field_MFVec3d.h>
#   include <vrml_field_ConstSFBool.h>
#   include <vrml_field_ConstSFColor.h>
#   include <vrml_field_ConstSFFloat.h>
#   include <vrml_field_ConstSFDouble.h>
#   include <vrml_field_ConstSFImage.h>
#   include <vrml_field_ConstSFInt32.h>
#   include <vrml_field_ConstSFNode.h>
#   include <vrml_field_ConstSFRotation.h>
#   include <vrml_field_ConstSFString.h>
#   include <vrml_field_ConstSFTime.h>
#   include <vrml_field_ConstSFVec2f.h>
#   include <vrml_field_ConstSFVec2d.h>
#   include <vrml_field_ConstSFVec3f.h>
#   include <vrml_field_ConstSFVec3d.h>
#   include <vrml_field_ConstMFBool.h>
#   include <vrml_field_ConstMFColor.h>
#   include <vrml_field_ConstMFFloat.h>
#   include <vrml_field_ConstMFDouble.h>
#   include <vrml_field_ConstMFInt32.h>
#   include <vrml_field_ConstMFNode.h>
#   include <vrml_field_ConstMFRotation.h>
#   include <vrml_field_ConstMFString.h>
#   include <vrml_field_ConstMFTime.h>
#   include <vrml_field_ConstMFVec2f.h>
#   include <vrml_field_ConstMFVec2d.h>
#   include <vrml_field_ConstMFVec3f.h>
#   include <vrml_field_ConstMFVec3d.h>
#   include <vrml_node_Node.h>
#   include <vrml_node_Script.h>

#   include <private.h>
#   include "script.h"
#   include "browser.h"
#   include "node.h"

#   ifdef _WIN32
#     define PATH_SEPARATOR ";"
#   else /* UNIX and macintosh */
#     define PATH_SEPARATOR ":"
#   endif

using namespace openvrml;

namespace {

   /**
    * @class VrmlEvent
    *
    * @brief Used to store events to be processed by the JSAI code.
    */
    class OPENVRML_LOCAL VrmlEvent {
        double d_timeStamp;
        std::string d_eventName;
        field_value *d_value;

    public:
        VrmlEvent(double timeStamp, const std::string& eventName,
                  const field_value * value) :
            d_timeStamp(timeStamp),
            d_eventName(eventName),
            d_value(value->clone().release())
        {}

        ~VrmlEvent() { delete d_value; }

        VrmlEvent * clone() const {
          return new VrmlEvent(this->d_timeStamp, this->d_eventName,
                               this->d_value);
        }

        double timeStamp() const { return d_timeStamp; }
        const std::string& name() const { return d_eventName; }
        const field_value * value() const { return d_value; }
    };
}

const char * ftn[] = {
  "<invalid field type>",
  "SFBool",
  "SFColor",
  "SFFloat",
  "SFDouble",
  "SFImage",
  "SFInt32",
  "SFNode",
  "SFRotation",
  "SFString",
  "SFTime",
  "SFVec2f",
  "SFVec2d",
  "SFVec3f",
  "SFVec3d",
  "MFBool",
  "MFColor",
  "MFFloat",
  "MFDouble",
  "MFInt32",
  "MFNode",
  "MFRotation",
  "MFString",
  "MFTime",
  "MFVec2f",
  "MFVec2d",
  "MFVec3f",
  "MFVec3d"
};

// Static members
JavaVM *ScriptJDK::d_jvm = 0;
JNIEnv *ScriptJDK::d_env = 0;

/**
 * @class ScriptJDK
 *
 * @brief Subclass of Script for JSAI.
 */

/**
 * @brief Construct a ScriptJDK object from the given class name.
 *
 * This method requires that className.class exists either in the
 * given classDir or somewhere within the CLASSPATH.
 *
 * @param[in] scriptNode Reference to the ScriptNode that uses this Script.
 * @param[in] className Name of the Java class.
 * @param[in] classDir Location of Java class.
 */
ScriptJDK::ScriptJDK(script_node & node,
                     const char * className,
             const char * classDir):
    script(node),
    d_class(0),
    d_object(0),
    d_processEventsID(0),
    d_processEventID(0),
    d_eventsProcessedID(0)
{
  if (! d_jvm)            // Initialize static members
  {
    JavaVMInitArgs vm_args;
    jint res;
    JavaVMOption options[3];

    /* get the currently defined CLASSPATH env variable */
    char* classPath = getenv("CLASSPATH");

    std::ostringstream appendedClassPath;

    appendedClassPath << "-Djava.class.path=."
              << PATH_SEPARATOR << classDir;

    // Only append CLASSPATH if non-null
    if (classPath)
    {
      appendedClassPath << PATH_SEPARATOR << classPath;
    }

#ifndef _WIN32
    appendedClassPath << PATH_SEPARATOR
                      << OPENVRML_PKGDATADIR_ << "/java/script.jar";
#endif
    appendedClassPath << std::ends;

    options[0].optionString =
        const_cast<char *>(appendedClassPath.str().c_str());
    options[1].optionString = "-verbose:class,jni";
#ifndef _WIN32
    options[2].optionString = "-Djava.library.path=" OPENVRML_LIBDIR_;
#endif
    vm_args.version = JNI_VERSION_1_2;
    vm_args.options = options;
#ifdef _WIN32
    vm_args.nOptions = 2;
#else
    vm_args.nOptions = 3;
#endif
    /* Create the Java VM */
    res = JNI_CreateJavaVM(&d_jvm, (void**) &d_env, &vm_args);

    if (res < 0) {
      OPENVRML_PRINT_MESSAGE_("Can't create Java VM");
    }
  }

  if (d_jvm && d_env)        // Per-object initialization
  {
    char fqClassName[1024];
    strcpy(fqClassName, "");
    strcat(fqClassName, className);

    d_class = d_env->FindClass(fqClassName);
    if (!d_class)
    {
      OPENVRML_PRINT_MESSAGE_("Can't find Java class "
                              + std::string(className) + ".");
      return;
    }

    // Call constructor
    jmethodID ctorId = d_env->GetMethodID(d_class, "<init>", "()V");

    if (ctorId)
      d_object = d_env->NewObject(d_class, ctorId);

    jfieldID fid = d_env->GetFieldID(d_class, "NodePtr", "I");
    d_env->SetIntField(d_object, fid, reinterpret_cast<int>(&node));

    // Cache other method IDs
    d_processEventID =
      d_env->GetMethodID(d_class, "processEvent", "(Lvrml/Event;)V");

    if (d_env->ExceptionOccurred())
    {
      d_env->ExceptionDescribe();
      d_env->ExceptionClear();
    }
  }
}


/**
 * @brief Destructor. Delete JVM reference.
 */
ScriptJDK::~ScriptJDK()
{
  if (d_jvm)
  {
    d_jvm->DestroyJavaVM();
    d_jvm = 0;
  }
}

namespace {

    /**
     * @brief Get the JNI fieldID for the given member name and type.
     *
     * @param[in] env JNI environment
     * @param[in] obj JNI object
     * @param[in] membername Name of desired member of JNI object
     * @param[in] sig Data type of member
     * @return JNI fieldID of member on the given object
     */
    OPENVRML_LOCAL jfieldID getFid(JNIEnv * env,
                                   jobject obj,
                                   char * membername,
                                   char * sig)
    {
        assert(obj);
        jclass clazz = env->GetObjectClass(obj);
        return env->GetFieldID(clazz, membername, sig);
    }

    /**
     * @brief Method to retrieve the corresponding field value pointer
     * in OpenVRML from a field in Java.
     *
     * @param[in] env JNI environment
     * @param[in] obj JNI version of a Java field object
     * @return OpenVRML field value pointer representing the passed in Java
     *         field object.
     *
     * @todo This method needs to be revisited. We immediately set the
     *       modified flag on eventOuts even though nothing has been set yet.
     */
    OPENVRML_LOCAL field_value * getFieldValue(JNIEnv *env, jobject obj)
    {
      field_value * fieldPtr;
      jfieldID fid;

      if ((fid = getFid(env, obj, "isEventOut", "Z")) == 0)
      {
        // Should never occur
        return 0;
      }

      bool eventOut = static_cast<bool>(env->GetBooleanField(obj, fid));

      if ((fid = getFid(env, obj, "isEventIn", "Z")) == 0)
      {
        // Should never occur
        return 0;
      }

      if ((fid = getFid(env, obj, "isExposedField", "Z")) == 0)
      {
        // Should never occur
        return 0;
      }

      if ((fid = getFid(env, obj, "FieldPtr", "I")) == 0)
      {
        // Should never occur
        return 0;
      }

      if (eventOut)
      {
        fieldPtr = reinterpret_cast<field_value *>(env->GetIntField(obj, fid));
      }
      else
      {
        fieldPtr = reinterpret_cast<field_value *>(env->GetIntField(obj, fid));
      }

      return fieldPtr;
    }

    /**
     * @brief This method converts a JNI VRML field object to its
     *        string representation.
     *
     * @param[in] env JNI environment
     * @param[in] obj JNI version of a Java field object
     * @return String representation of a VRML field
     */
    OPENVRML_LOCAL jstring fieldToString(JNIEnv *env, jobject obj)
    {
        std::ostringstream os;
        field_value * field = getFieldValue(env, obj);
        if (!field) { return 0; }
        os << *field << std::ends;
        const char * str = os.str().c_str();
        jstring result = env->NewStringUTF(str);
        return result;
    }
}

/**
 * @brief Initialize the Script object.
 *
 * @param[in] timestamp Time at which script is being initialized.
 */
void ScriptJDK::do_initialize(const double timestamp)
{
  const sftime arg(timestamp);
  const field_value * argv[] = { &arg };
  this->activate(timestamp, "initialize", 1, argv);
}

/**
 * @brief Called to process an event that has occurred.
 *
 * @param[in] id Name of the event.
 * @param[in] value Value of the event.
 * @param[in] timestamp Time at which event occurred.
 */
void ScriptJDK::do_process_event(const std::string & id,
                                 const field_value & value,
                                 const double timestamp)
{
  const sftime timestampArg(timestamp);
  const field_value * argv[] = { &value, &timestampArg };
  this->activate(timestamp, id, 2, argv);
}

/**
 * @brief Called to process one or more events that have occurred.
 *
 * @param[in] timestamp Time at which last event occurred.
 */
void ScriptJDK::do_events_processed(const double timestamp)
{
  const sftime arg(timestamp);
  const field_value * argv[] = { &arg };
  this->activate(timestamp, "eventsProcessed", 1, argv);
}

/**
 * @brief Called when the script is deleted or the world containing
 * the script is unloaded or replaced
 *
 * @param[in] timestamp Time at which script is being destroyed.
 */
void ScriptJDK::do_shutdown(const double timestamp)
{
  const sftime arg(timestamp);
  const field_value * argv[] = { &arg };
  this->activate(timestamp, "shutdown", 1, argv);
}

/**
 * @brief Run a specified script.
 *
 * @param[in] timeStamp Time at which script is being run.
 * @param[in] fname Script name
 * @param[in] argc Number of arguments to pass to script
 * @param[in] argv Array of arguments
 */
void ScriptJDK::activate( double timeStamp,
              const std::string& fname,
              size_t argc,
              const field_value* const argv[] )
{
  if (argc == 2 && d_processEventID)
  {
    jclass clazz = d_env->FindClass("vrml/Event");

    if (!clazz)
    {
      OPENVRML_PRINT_MESSAGE_("Can't find Java class vrml/Event");
      return;
    }

    jobject jEvent = d_env->AllocObject(clazz);
    jfieldID fid = getFid(d_env, jEvent, "EventPtr", "I");

    if (fid)
    {
      VrmlEvent* pEvent = new VrmlEvent(timeStamp, fname, argv[0]);
      d_env->SetIntField(jEvent, fid, reinterpret_cast<int>(pEvent));
      d_env->CallVoidMethod(d_object, d_processEventID, jEvent);
      d_env->DeleteLocalRef(jEvent);
      delete pEvent;
    }

    if (d_env->ExceptionOccurred())
    {
      d_env->ExceptionDescribe();
      d_env->ExceptionClear();
    }
  }
  else if (argc == 1)
  {
    // Call initialize()
    jmethodID initID = d_env->GetMethodID(d_class, fname.c_str(),"()V");
    if (initID)
      d_env->CallVoidMethod(d_object, initID);
    if (d_env->ExceptionOccurred())
    {
      d_env->ExceptionDescribe();
      d_env->ExceptionClear();
    }
  }
}

/**
 * @brief Create a new field_value of the given type.
 *
 * @param[in] fieldtype Type of field to create.
 * @return New field_value of given type
 */
static field_value* newField(field_value::type_id fieldtype)
{
  switch (fieldtype)
  {
    case field_value::invalid_type_id:
      return NULL;
    case field_value::sfbool_id:
      return new sfbool;
    case field_value::sfcolor_id:
      return new sfcolor;
    case field_value::sffloat_id:
      return new sffloat;
    case field_value::sfdouble_id:
      return new sfdouble;
    case field_value::sfimage_id:
      return new sfimage;
    case field_value::sfint32_id:
      return new sfint32;
    case field_value::sfnode_id:
      return new sfnode;
    case field_value::sfrotation_id:
      return new sfrotation;
    case field_value::sfstring_id:
      return new sfstring;
    case field_value::sftime_id:
      return new sftime;
    case field_value::sfvec2f_id:
      return new sfvec2f;
    case field_value::sfvec2d_id:
      return new sfvec2d;
    case field_value::sfvec3f_id:
      return new sfvec3f;
    case field_value::sfvec3d_id:
      return new sfvec3d;
    case field_value::mfbool_id:
      return new mfbool;
    case field_value::mfcolor_id:
      return new mfcolor;
    case field_value::mffloat_id:
      return new mffloat;
    case field_value::mfdouble_id:
      return new mfdouble;
    case field_value::mfint32_id:
      return new mfint32;
    case field_value::mfnode_id:
      return new mfnode;
    case field_value::mfrotation_id:
      return new mfrotation;
    case field_value::mfstring_id:
      return new mfstring;
    case field_value::mftime_id:
      return new mftime;
    case field_value::mfvec2f_id:
      return new mfvec2f;
    case field_value::mfvec2d_id:
      return new mfvec2d;
    case field_value::mfvec3f_id:
      return new mfvec3f;
    case field_value::mfvec3d_id:
      return new mfvec3d;
    default:
      return NULL;
  }
}

/**
 * @brief Clone a JNI VRML field object.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a VRML field object.
 * @return Cloned copy of JNI version of a VRML field object.
 *
 * @todo revisit this method
 */
jobject JNICALL Java_vrml_Field_clone(JNIEnv * env, jobject obj)
{
    // This method will fail if used on an eventOut (as the stored value is
    // not a field_value*. It'll all fail as soon as anyone tries to use the
    // field in any way as the flags aren't copied that indicate whether the
    // field is an eventIn, eventOut or exposedField.

    jfieldID fid = getFid(env, obj, "FieldPtr", "I");
    if (!fid) { return 0; }
    field_value* field =
        reinterpret_cast<field_value*>(env->GetIntField(obj, fid));
    if (!field) { return 0; }
    std::ostringstream os;
    os << "vrml/field/" << field->type() << std::ends;
    jclass clazz = env->FindClass(os.str().c_str());
    jobject jCloneField = env->AllocObject(clazz);
    fid = env->GetFieldID(clazz, "FieldPtr", "I");
    if (!fid) { return 0; }
    field_value* cloneField = field->clone().release();
    env->SetIntField(jCloneField, fid, reinterpret_cast<int>(cloneField));
    return jCloneField;
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

/**
 * @brief JNI implementation of ConstSFBool::CreateObject.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj ConstSFBool object
 * @param[in] value Initial value of ConstSFBool field
 */
void JNICALL Java_vrml_field_ConstSFBool_CreateObject
  (JNIEnv *env, jobject obj, jboolean value)
{
    try {
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        std::auto_ptr<openvrml::sfbool> sfbool(new openvrml::sfbool(value));
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfbool.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFBool::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFBool object
 * @return Value of ConstSFBool field
 */
jboolean JNICALL Java_vrml_field_ConstSFBool_getValue(JNIEnv * env,
                                                      jobject obj)
{
    sfbool * sfb = static_cast<sfbool *>(getFieldValue(env, obj));
    if (!sfb) { return false; }
    return sfb->value;
}

/**
 * @brief JNI implementation of ConstSFBool::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFBool object
 * @return String representation of ConstSFBool field.
 */
jstring JNICALL Java_vrml_field_ConstSFBool_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of SFBool::CreateObject.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFBool object
 * @param[in] value Initial value of SFBool object.
 */
void JNICALL Java_vrml_field_SFBool_CreateObject
  (JNIEnv *env, jobject obj, jboolean value)
{
  Java_vrml_field_ConstSFBool_CreateObject(env, obj, value);
}

/**
 * @brief JNI implementation of SFBool::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFBool object
 * @return Value of SFBool object.
 */
jboolean JNICALL Java_vrml_field_SFBool_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFBool_getValue(env, obj);
}

/**
 * @brief JNI implementation of SFBool::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFBool object
 * @param[in] value Desired value of SFBool object.
 */
void JNICALL Java_vrml_field_SFBool_setValue__Z(JNIEnv * env,
                                                jobject obj,
                                                jboolean value)
{
    sfbool * sfb = static_cast<sfbool *>(getFieldValue(env, obj));
    if (!sfb) { return; }
    sfb->value = value;
}

/**
 * @brief JNI implementation of SFBool::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFBool object
 * @param[in] value Desired value of SFBool object.
 */
void JNICALL
Java_vrml_field_SFBool_setValue__Lvrml_field_ConstSFBool_2
  (JNIEnv *env, jobject obj, jobject value)
{
  sfbool* newsfbool = static_cast<sfbool*>(getFieldValue(env, value));
  sfbool* sfb = static_cast<sfbool*>(getFieldValue(env, obj));
  if (!newsfbool || !sfb) return;
  sfb->value = newsfbool->value;
}

/**
 * @brief JNI implementation of SFBool::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFBool object
 * @param[in] value Desired value of SFBool object.
 */
void JNICALL
Java_vrml_field_SFBool_setValue__Lvrml_field_SFBool_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFBool_setValue__Lvrml_field_ConstSFBool_2(env, obj, value);
}

/**
 * @brief JNI implementation of SFBool::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFBool object
 * @return String representation of SFBool field.
 */
jstring JNICALL Java_vrml_field_SFBool_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of ConstSFColor::CreateObject.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj ConstSFColor object.
 * @param[in] r Red component of Color
 * @param[in] g Green component of Color
 * @param[in] b Blue component of Color
 */
void JNICALL Java_vrml_field_ConstSFColor_CreateObject
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
    try {
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        std::auto_ptr<openvrml::sfcolor>
            sfcolor(new openvrml::sfcolor(color(r, g, b)));
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfcolor.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFColor::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFColor object
 * @param[out] jarr Array to fill with RGB values of SFColor.
 */
void JNICALL Java_vrml_field_ConstSFColor_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr)
{
    sfcolor * sfc = static_cast<sfcolor*>(getFieldValue(env, obj));
    if (!sfc) { return; }
    env->SetFloatArrayRegion(jarr, 0, 3,
                             const_cast<float *>(&sfc->value[0]));
}

/**
 * @brief JNI implementation of ConstSFColor::getRed.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFColor object
 * @return Red component of SFColor
 */
jfloat JNICALL Java_vrml_field_ConstSFColor_getRed(JNIEnv * env, jobject obj)
{
    sfcolor * sfc = static_cast<sfcolor *>(getFieldValue(env, obj));
    if (!sfc) { return 0.0; }
    return sfc->value.r();
}

/**
 * @brief JNI implementation of ConstSFColor::getGreen.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFColor object
 * @return Green component of SFColor
 */
jfloat JNICALL Java_vrml_field_ConstSFColor_getGreen(JNIEnv * env,
                                                     jobject obj)
{
    sfcolor * sfc = static_cast<sfcolor *>(getFieldValue(env, obj));
    if (!sfc) { return 0.0; }
    return sfc->value.g();
}

/**
 * @brief JNI implementation of ConstSFColor::getBlue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFColor object
 * @return Blue component of SFColor
 */
jfloat JNICALL Java_vrml_field_ConstSFColor_getBlue(JNIEnv * env, jobject obj)
{
    sfcolor * sfc = static_cast<sfcolor *>(getFieldValue(env, obj));
    if (!sfc) { return 0.0; }
    return sfc->value.b();
}

/**
 * @brief JNI implementation of ConstSFColor::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFColor object.
 * @return String representation of ConstSFColor.
 */
jstring JNICALL Java_vrml_field_ConstSFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of SFColor::CreateObject
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFColor object
 * @param[in] r Red component of Color
 * @param[in] g Green component of Color
 * @param[in] b Blue component of Color
 */
void JNICALL Java_vrml_field_SFColor_CreateObject
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  Java_vrml_field_ConstSFColor_CreateObject(env, obj, r, g, b);
}

/**
 * @brief JNI implementation of SFColor::getValue
 *
 * @param[in] env JNI environment
 * @param[in] obj SFColor object
 * @param[out] jarr Array to populate with SFColor RGB components.
 */
void JNICALL Java_vrml_field_SFColor_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFColor_getValue(env, obj, jarr);
}

/**
 * @brief JNI implementation of SFColor::getRed.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFColor object
 * @return Red component of SFColor.
 */
jfloat JNICALL Java_vrml_field_SFColor_getRed
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFColor_getRed(env, obj);
}

/**
 * @brief JNI implementation of SFColor::getGreen.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFColor object
 * @return Green component of SFColor.
 */
jfloat JNICALL Java_vrml_field_SFColor_getGreen
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFColor_getGreen(env, obj);
}

/**
 * @brief JNI implementation of SFColor::getBlue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFColor object
 * @return Blue component of SFColor.
 */
jfloat JNICALL Java_vrml_field_SFColor_getBlue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFColor_getBlue(env, obj);
}

/**
 * @brief JNI implementation of SFColor::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFColor object
 * @param[in] jarr Array containing RGB components of desired SFColor.
 */
void JNICALL Java_vrml_field_SFColor_setValue___3F(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr)
{
    sfcolor * sfc = static_cast<sfcolor *>(getFieldValue(env, obj));
    if (!sfc) { return; }
    jfloat * pjf = env->GetFloatArrayElements(jarr, NULL);
    sfc->value = color(pjf[0], pjf[1], pjf[2]);
    env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

/**
 * @brief JNI implementation of SFColor::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFColor object
 * @param[in] r Red component of desired color.
 * @param[in] g Green component of desired color.
 * @param[in] b Blue component of desired color.
 */
void JNICALL Java_vrml_field_SFColor_setValue__FFF
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
    sfcolor* sfc = static_cast<sfcolor*>(getFieldValue(env, obj));
    if (!sfc) { return; }
    sfc->value = color(r, g, b);
}

/**
 * @brief JNI implementation of SFColor::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFColor object
 * @param[in] value ConstSFColor object to copy color from.
 */
void JNICALL
Java_vrml_field_SFColor_setValue__Lvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
    sfcolor* newSFColor = static_cast<sfcolor*>(getFieldValue(env, value));
    sfcolor* sfc = static_cast<sfcolor*>(getFieldValue(env, obj));
    if (!newSFColor || !sfc) return;
    sfc->value = newSFColor->value;
}

/**
 * @brief JNI implementation of SFColor::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFColor object
 * @param[in] value SFColor object to copy color from.
 */
void JNICALL
Java_vrml_field_SFColor_setValue__Lvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFColor_setValue__Lvrml_field_ConstSFColor_2
    (env, obj, value);
}

/**
 * @brief JNI implementation of SFColor::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFColor object
 * @return String representation of SFColor.
 */
jstring JNICALL Java_vrml_field_SFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of ConstSFFloat::CreateObject.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj ConstSFFloat object
 * @param[in] value Initial value for object
 */
void JNICALL Java_vrml_field_ConstSFFloat_CreateObject
  (JNIEnv *env, jobject obj, jfloat value)
{
    try {
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        std::auto_ptr<openvrml::sffloat> sffloat(new openvrml::sffloat(value));
        env->SetIntField(obj, fid, reinterpret_cast<int>(sffloat.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFFloat::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFFloat object
 * @return Value of field.
 */
jfloat JNICALL Java_vrml_field_ConstSFFloat_getValue(JNIEnv * env,
                                                     jobject obj)
{
    sffloat * sff = static_cast<sffloat *>(getFieldValue(env, obj));
    if (!sff) { return 0.0; }
    return sff->value;
}

/**
 * @brief JNI implementation of ConstSFFloat::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFFloat object
 * @return String representation of ConstSFFloat.
 */
jstring JNICALL Java_vrml_field_ConstSFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of SFFloat::CreateObject.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFFloat object
 * @param[in] value Initial value for object
 */
void JNICALL Java_vrml_field_SFFloat_CreateObject
  (JNIEnv *env, jobject obj, jfloat value)
{
  Java_vrml_field_ConstSFFloat_CreateObject(env, obj, value);
}

/**
 * @brief JNI implementation of SFFloat::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFFloat object
 * @return Value of field.
 */
jfloat JNICALL Java_vrml_field_SFFloat_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFFloat_getValue(env, obj);
}

/**
 * @brief JNI implementation of SFFloat::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFFloat object
 * @param[in] f Desired value
 */
void JNICALL Java_vrml_field_SFFloat_setValue__F(JNIEnv * env,
                                                 jobject obj,
                                                 jfloat f)
{
    sffloat * sff = static_cast<sffloat *>(getFieldValue(env, obj));
    if (!sff) { return; }
    sff->value = f;
}

/**
 * @brief JNI implementation of SFFloat::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFFloat object
 * @param[in] value Desired value
 */
void JNICALL
Java_vrml_field_SFFloat_setValue__Lvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
    sffloat* sff = static_cast<sffloat*>(getFieldValue(env, obj));
    sffloat* newSFFloat = static_cast<sffloat*>(getFieldValue(env, value));
    if (!sff || !newSFFloat) return;
    sff->value = newSFFloat->value;
}

/**
 * @brief JNI implementation of SFFloat::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFFloat object
 * @param[in] value Desired value
 */
void JNICALL
Java_vrml_field_SFFloat_setValue__Lvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFFloat_setValue__Lvrml_field_ConstSFFloat_2
    (env, obj, value);
}

/**
 * @brief JNI implementation of SFFloat::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFFloat object
 * @return String representation of SFFloat.
 */
jstring JNICALL Java_vrml_field_SFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

adsfadsf
/**
 * @brief JNI implementation of ConstSFDouble::CreateObject.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj ConstSFDouble object
 * @param[in] value Initial value for object
 */
void JNICALL Java_vrml_field_ConstSFDouble_CreateObject
  (JNIEnv *env, jobject obj, jdouble value)
{
    try {
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        std::auto_ptr<openvrml::sfdouble> sfdouble(new openvrml::sfdouble(value));
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfdouble.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFDouble::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFDouble object
 * @return Value of field.
 */
jdouble JNICALL Java_vrml_field_ConstSFDouble_getValue(JNIEnv * env,
                                                     jobject obj)
{
    sfdouble * sff = static_cast<sfdouble *>(getFieldValue(env, obj));
    if (!sff) { return 0.0; }
    return sff->value;
}

/**
 * @brief JNI implementation of ConstSFDouble::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFDouble object
 * @return String representation of ConstSFDouble.
 */
jstring JNICALL Java_vrml_field_ConstSFDouble_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of SFDouble::CreateObject.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFDouble object
 * @param[in] value Initial value for object
 */
void JNICALL Java_vrml_field_SFDouble_CreateObject
  (JNIEnv *env, jobject obj, jdouble value)
{
  Java_vrml_field_ConstSFDouble_CreateObject(env, obj, value);
}

/**
 * @brief JNI implementation of SFDouble::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFDouble object
 * @return Value of field.
 */
jdouble JNICALL Java_vrml_field_SFDouble_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFDouble_getValue(env, obj);
}

/**
 * @brief JNI implementation of SFDouble::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFDouble object
 * @param[in] f Desired value
 */
void JNICALL Java_vrml_field_SFDouble_setValue__F(JNIEnv * env,
                                                 jobject obj,
                                                 jdouble f)
{
    sfdouble * sff = static_cast<sfdouble *>(getFieldValue(env, obj));
    if (!sff) { return; }
    sff->value = f;
}

/**
 * @brief JNI implementation of SFDouble::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFDouble object
 * @param[in] value Desired value
 */
void JNICALL
Java_vrml_field_SFDouble_setValue__Lvrml_field_ConstSFDouble_2
  (JNIEnv *env, jobject obj, jobject value)
{
    sfdouble* sff = static_cast<sfdouble*>(getFieldValue(env, obj));
    sfdouble* newSFDouble = static_cast<sfdouble*>(getFieldValue(env, value));
    if (!sff || !newSFDouble) return;
    sff->value = newSFDouble->value;
}

/**
 * @brief JNI implementation of SFDouble::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFDouble object
 * @param[in] value Desired value
 */
void JNICALL
Java_vrml_field_SFDouble_setValue__Lvrml_field_SFDouble_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFDouble_setValue__Lvrml_field_ConstSFDouble_2
    (env, obj, value);
}

/**
 * @brief JNI implementation of SFDouble::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFDouble object
 * @return String representation of SFDouble.
 */
jstring JNICALL Java_vrml_field_SFDouble_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}
/**
 * @brief JNI implementation of ConstSFImage::CreateObject.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj ConstSFImage object
 * @param[in] width Initial width
 * @param[in] height Initial height
 * @param[in] components Initial number of components
 * @param[in] pixels Initial set of pixels
 */
void JNICALL Java_vrml_field_ConstSFImage_CreateObject
  (JNIEnv *env, jobject obj, jint width, jint height,
   jint components, jbyteArray pixels)
{
    try {
        std::auto_ptr<sfimage> sfi;
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }

        if (pixels) {
            jbyte *pjb = env->GetByteArrayElements(pixels, 0);
            sfi.reset(new sfimage(image(width,
                                        height,
                                        components,
                                        pjb,
                                        pjb + (width * height * components))));
            env->ReleaseByteArrayElements(pixels, pjb, JNI_ABORT);
        } else {
            sfi.reset(new sfimage(image(width, height, components)));
        }
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfi.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFImage::getWidth.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFImage object
 * @return Width of ConstSFImage
 */
jint JNICALL Java_vrml_field_ConstSFImage_getWidth(JNIEnv *env, jobject obj)
{
    sfimage* sfi = static_cast<sfimage*>(getFieldValue(env, obj));
    if (!sfi) return 0;
    return sfi->value.x();
}

/**
 * @brief JNI implementation of ConstSFImage::getHeight.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFImage object
 * @return Height of ConstSFImage
 */
jint JNICALL Java_vrml_field_ConstSFImage_getHeight(JNIEnv *env, jobject obj)
{
    sfimage* sfi = static_cast<sfimage*>(getFieldValue(env, obj));
    if (!sfi) return 0;
    return sfi->value.y();
}

/**
 * @brief JNI implementation of ConstSFImage::getComponents.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFImage object
 * @return Number of components in ConstSFImage
 */
jint JNICALL Java_vrml_field_ConstSFImage_getComponents(JNIEnv * env,
                                                        jobject obj)
{
    sfimage* sfi = static_cast<sfimage*>(getFieldValue(env, obj));
    if (!sfi) return 0;
    return sfi->value.comp();
}

/**
 * @brief JNI implementation of ConstSFImage::getPixels.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFImage object
 * @param[out] pixels Array to fill with ConstSFImage pixels
 */
void JNICALL Java_vrml_field_ConstSFImage_getPixels(JNIEnv *env,
                                                    jobject obj,
                                                    jbyteArray pixels)
{
    using std::vector;
    sfimage* sfi = static_cast<sfimage*>(getFieldValue(env, obj));
    if (!sfi) return;
    vector<jbyte> jbyte_pixels(sfi->value.array().begin(),
                               sfi->value.array().end());
    env->SetByteArrayRegion(pixels, 0, jbyte_pixels.size(),
                            jbyte_pixels.empty() ? 0 : &jbyte_pixels[0]);
}

/**
 * @brief JNI implementation of ConstSFImage::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFImage object
 * @return String representation of ConstSFImage.
 */
jstring JNICALL Java_vrml_field_ConstSFImage_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of SFImage::CreateObject.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFImage object
 * @param[in] width Initial width
 * @param[in] height Initial height
 * @param[in] components Initial number of components
 * @param[in] pixels Initial set of pixels
 */
void JNICALL Java_vrml_field_SFImage_CreateObject
  (JNIEnv *env, jobject obj, jint width, jint height,
   jint components, jbyteArray pixels)
{
  Java_vrml_field_ConstSFImage_CreateObject(env, obj, width, height,
                                            components, pixels);
}

/**
 * @brief JNI implementation of SFImage::getWidth.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFImage object
 * @return Width of SFImage
 */
jint JNICALL Java_vrml_field_SFImage_getWidth
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFImage_getWidth(env, obj);
}

/**
 * @brief JNI implementation of SFImage::getHeight.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFImage object
 * @return Height of SFImage
 */
jint JNICALL Java_vrml_field_SFImage_getHeight
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFImage_getHeight(env, obj);
}

/**
 * @brief JNI implementation of SFImage::getComponents.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFImage object
 * @return Number of components in SFImage
 */
jint JNICALL Java_vrml_field_SFImage_getComponents
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFImage_getComponents(env, obj);
}

/**
 * @brief JNI implementation of SFImage::getPixels.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFImage object
 * @param[out] pixels Array to fill with SFImage pixels
 */
void JNICALL Java_vrml_field_SFImage_getPixels
  (JNIEnv *env, jobject obj, jbyteArray pixels)
{
  Java_vrml_field_ConstSFImage_getPixels(env, obj, pixels);
}

/**
 * @brief JNI implementation of SFImage::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFImage object
 * @param[in] width New width
 * @param[in] height New height
 * @param[in] components Number of components
 * @param[in] pixels New set of pixels
 */
void JNICALL Java_vrml_field_SFImage_setValue__III_3B(JNIEnv * env,
                                                      jobject obj,
                                                      jint width,
                                                      jint height,
                                                      jint components,
                                                      jbyteArray pixels)
{
    sfimage* sfi = static_cast<sfimage*>(getFieldValue(env, obj));
    if (!sfi) return;
    jbyte *pjb = env->GetByteArrayElements(pixels, NULL);
    sfi->value = image(width, height, components,
                       pjb, pjb + (width * height * components));
    env->ReleaseByteArrayElements(pixels, pjb, JNI_ABORT);
}

/**
 * @brief JNI implementation of SFImage::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFImage object
 * @param[in] value ConstSFImage to copy value from
 */
void JNICALL
Java_vrml_field_SFImage_setValue__Lvrml_field_ConstSFImage_2(JNIEnv * env,
                                                             jobject obj,
                                                             jobject value)
{
    sfimage* sfi = static_cast<sfimage*>(getFieldValue(env, obj));
    if (!sfi) return;
    jfieldID fid = getFid(env, value, "FieldPtr", "I");
    if (!fid) return;
    sfimage* newsfimage =
        reinterpret_cast<sfimage*>(env->GetIntField(value, fid));
    if (!newsfimage) return;
    *sfi = *newsfimage;
}

/**
 * @brief JNI implementation of SFImage::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFImage object
 * @param[in] value SFImage to copy value from
 */
void JNICALL
Java_vrml_field_SFImage_setValue__Lvrml_field_SFImage_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFImage_setValue__Lvrml_field_ConstSFImage_2(env, obj,
                                                               value);
}

/**
 * @brief JNI implementation of SFImage::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFImage object
 * @return String representation of SFImage.
 */
jstring JNICALL Java_vrml_field_SFImage_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstSFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint value)
{
    try {
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        std::auto_ptr<openvrml::sfint32> sfint32(new openvrml::sfint32(value));
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfint32.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFInt32::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFInt32 object
 * @return Value of field
 */
jint JNICALL Java_vrml_field_ConstSFInt32_getValue(JNIEnv * env, jobject obj)
{
    sfint32 * sfi = static_cast<sfint32*>(getFieldValue(env, obj));
    if (!sfi) { return 0; }
    return jint(sfi->value);
}

/**
 * @brief JNI implementation of ConstSFInt32::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFInt32 object
 * @return String representation of ConstSFInt32.
 */
jstring JNICALL Java_vrml_field_ConstSFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_SFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint value)
{
  Java_vrml_field_ConstSFInt32_CreateObject(env, obj, value);
}

/**
 * @brief JNI implementation of SFInt32::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFInt32 object
 * @return Value of field
 */
jint JNICALL Java_vrml_field_SFInt32_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFInt32_getValue(env, obj);
}

/**
 * @brief JNI implementation of SFInt32::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFInt32 object
 * @param[in] value New value of SFInt32
 */
void JNICALL Java_vrml_field_SFInt32_setValue__I(JNIEnv * env,
                                                 jobject obj,
                                                 jint value)
{
    sfint32 * sfi = static_cast<sfint32*>(getFieldValue(env, obj));
    if (!sfi) { return; }
    sfi->value = value;
}

/**
 * @brief JNI implementation of SFInt32::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFInt32 object
 * @param[in] value New value of SFInt32
 */
void JNICALL
Java_vrml_field_SFInt32_setValue__Lvrml_field_ConstSFInt32_2(JNIEnv * env,
                                                             jobject obj,
                                                             jobject value)
{
    sfint32* sfi = static_cast<sfint32*>(getFieldValue(env, obj));
    sfint32* newSFInt32 = static_cast<sfint32*>(getFieldValue(env, value));
    if (!sfi || !newSFInt32) return;
    sfi->value = newSFInt32->value;
}

/**
 * @brief JNI implementation of SFInt32::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFInt32 object
 * @param[in] value New value of SFInt32
 */
void JNICALL
Java_vrml_field_SFInt32_setValue__Lvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFInt32_setValue__Lvrml_field_ConstSFInt32_2
    (env, obj, value);
}

/**
 * @brief JNI implementation of SFInt32::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFInt32 object
 * @return String representation of SFInt32.
 */
jstring JNICALL Java_vrml_field_SFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of ConstSFNode::CreateObject.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj ConstSFNode object
 * @param[in] value BaseNode object to initialize field with.
 */
void JNICALL Java_vrml_field_ConstSFNode_CreateObject(JNIEnv * env,
                                                      jobject obj,
                                                      jobject value)
{
    try {
        jfieldID fid;
        std::auto_ptr<sfnode> sfn;

        if (value == 0) {
            // default constructor was called
            sfn.reset(new sfnode);
        } else {
            fid = getFid(env, value, "NodePtr", "I");
            if (!fid) { return; }
            node * baseNode =
                reinterpret_cast<node *>(env->GetIntField(value, fid));
            if (!baseNode) return;
            sfn.reset(new sfnode(node_ptr(baseNode)));
        }

        fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfn.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFNode::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFNode object
 * @return BaseNode contained in SFNode.
 */
jobject JNICALL Java_vrml_field_ConstSFNode_getValue(JNIEnv * env, jobject obj)
{
    sfnode * sfn = static_cast<sfnode*>(getFieldValue(env, obj));
    if (!sfn) { return 0; }
    jclass clazz = env->FindClass("vrml/node/Node");
    jobject jNode = env->AllocObject(clazz);
    jfieldID fid = env->GetFieldID(clazz, "NodePtr", "I");
    if (!fid) { return 0; }
    env->SetIntField(jNode, fid, reinterpret_cast<int>(sfn->value.get()));
    return jNode;
}

/**
 * @brief JNI implementation of ConstSFNode::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFNode object
 * @return String representation of ConstSFNode.
 */
jstring JNICALL Java_vrml_field_ConstSFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of SFNode::CreateObject.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFNode object
 * @param[in] value BaseNode object to initialize field with.
 */
void JNICALL Java_vrml_field_SFNode_CreateObject
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_ConstSFNode_CreateObject(env, obj, value);
}

/**
 * @brief JNI implementation of SFNode::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFNode object
 * @return BaseNode contained in SFNode.
 */
jobject JNICALL Java_vrml_field_SFNode_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFNode_getValue(env, obj);
}

/**
 * @brief JNI implementation of SFNode::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFNode object
 * @param[in] value New value of SFNode
 */
void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_BaseNode_2(JNIEnv * env,
                                                               jobject obj,
                                                               jobject value)
{
    sfnode * sfn = static_cast<sfnode*>(getFieldValue(env, obj));
    if (!sfn) { return; }
    jfieldID fid = getFid(env, value, "NodePtr", "I");
    if (!fid) { return; }
    node * const n = reinterpret_cast<node *>(env->GetIntField(value, fid));
    sfn->value = node_ptr(n);
}

/**
 * @brief JNI implementation of SFNode::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFNode object
 * @param[in] value New value of SFNode
 */
void JNICALL
Java_vrml_field_SFNode_setValue__Lvrml_field_ConstSFNode_2(JNIEnv * env,
                                                           jobject obj,
                                                           jobject value)
{
    sfnode* sfn = static_cast<sfnode*>(getFieldValue(env, obj));
    sfnode* newSFNode = static_cast<sfnode*>(getFieldValue(env, value));
    if (!sfn || !newSFNode) return;
    *sfn = *newSFNode;
}

/**
 * @brief JNI implementation of SFNode::setValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj SFNode object
 * @param[in] value New value of SFNode
 */
void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFNode_setValue__Lvrml_field_ConstSFNode_2(env, obj, value);
}

/**
 * @brief JNI implementation of SFNode::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFNode object
 * @return String representation of SFNode.
 */
jstring JNICALL Java_vrml_field_SFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstSFRotation_CreateObject
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY,
   jfloat axisZ, jfloat angle)
{
    try {
        std::auto_ptr<openvrml::sfrotation>
            sfrotation(new openvrml::sfrotation(rotation(axisX,
                                                         axisY,
                                                         axisZ,
                                                         angle)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfrotation.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL Java_vrml_field_ConstSFRotation_getValue(JNIEnv * env,
                                                      jobject obj,
                                                      jfloatArray jarr)
{
    sfrotation * sfr = static_cast<sfrotation*>(getFieldValue(env, obj));
    if (!sfr) { return; }
    env->SetFloatArrayRegion(jarr, 0, 4,
                             const_cast<float *>(&sfr->value[0]));
}

/**
 * @brief JNI implementation of ConstSFRotation::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFRotation object
 * @return String representation of ConstSFRotation.
 */
jstring JNICALL Java_vrml_field_ConstSFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_SFRotation_CreateObject
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY,
   jfloat axisZ, jfloat angle)
{
  Java_vrml_field_ConstSFRotation_CreateObject(env, obj, axisX, axisY,
                                               axisZ, angle);
}

void JNICALL Java_vrml_field_SFRotation_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFRotation_getValue(env, obj, jarr);
}

/**
 * JNI implementation of SFRotation::setValue.
 *
 * @brief env JNI environment
 * @brief obj SFRotation object
 * @brief jarr New value for field
 */
void JNICALL Java_vrml_field_SFRotation_setValue___3F(JNIEnv * env,
                                                      jobject obj,
                                                      jfloatArray jarr)
{
    sfrotation * sfr = static_cast<sfrotation*>(getFieldValue(env, obj));
    if (!sfr) { return; }
    jfloat * pjf = env->GetFloatArrayElements(jarr, NULL);
    sfr->value = rotation(pjf[0], pjf[1], pjf[2], pjf[3]);
    env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
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
void JNICALL Java_vrml_field_SFRotation_setValue__FFFF(JNIEnv *env,
                                                       jobject obj,
                                                       jfloat axisX,
                                                       jfloat axisY,
                                                       jfloat axisZ,
                                                       jfloat angle)
{
    sfrotation * sfr = static_cast<sfrotation*>(getFieldValue(env, obj));
    if (!sfr) { return; }
    sfr->value = rotation(axisX, axisY, axisZ, angle);
}

/**
 * JNI implementation of SFRotation::setValue.
 *
 * @brief env JNI environment
 * @brief obj SFRotation object
 * @brief value New value for field
 */
void JNICALL
Java_vrml_field_SFRotation_setValue__Lvrml_field_ConstSFRotation_2(
    JNIEnv * env,
    jobject obj,
    jobject value)
{
    sfrotation * sfr = static_cast<sfrotation*>(getFieldValue(env, obj));
    sfrotation * newSFRotation =
        static_cast<sfrotation*>(getFieldValue(env, value));
    if (!sfr || !newSFRotation) return;
    sfr->value = newSFRotation->value;
}

/**
 * JNI implementation of SFRotation::setValue.
 *
 * @brief env JNI environment
 * @brief obj SFRotation object
 * @brief value New value for field
 */
void JNICALL
Java_vrml_field_SFRotation_setValue__Lvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFRotation_setValue__Lvrml_field_ConstSFRotation_2
    (env, obj, value);
}

/**
 * @brief JNI implementation of SFRotation::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFRotation object
 * @return String representation of SFRotation.
 */
jstring JNICALL Java_vrml_field_SFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstSFString_CreateObject(JNIEnv * env,
                                                        jobject obj,
                                                        jstring jstr)
{
    try {
        std::auto_ptr<sfstring> sfstr;

        // Need to handle null strings
        if (jstr) {
            const char* str = env->GetStringUTFChars(jstr, 0);
            if (!str) { return; } // OutOfMemoryError
            sfstr.reset(new sfstring(str));
            env->ReleaseStringUTFChars(jstr, str);
        } else {
            sfstr.reset(new sfstring);
        }

        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfstr.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFString::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFString object
 * @return Value of field
 */
jstring JNICALL Java_vrml_field_ConstSFString_getValue(JNIEnv * env,
                                                       jobject obj)
{
    sfstring* sfstr = static_cast<sfstring*>(getFieldValue(env, obj));
    if (!sfstr) { return 0; }
    return env->NewStringUTF((sfstr->value).c_str());
}

/**
 * @brief JNI implementation of ConstSFString::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFString object
 * @return String representation of ConstSFString.
 */
jstring JNICALL Java_vrml_field_ConstSFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_SFString_CreateObject
  (JNIEnv *env, jobject obj, jstring jstr)
{
  Java_vrml_field_ConstSFString_CreateObject(env, obj, jstr);
}

/**
 * @brief JNI implementation of SFString::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFString object
 * @return Value of field
 */
jstring JNICALL Java_vrml_field_SFString_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFString_getValue(env, obj);
}

void JNICALL
Java_vrml_field_SFString_setValue__Ljava_lang_String_2(JNIEnv * env,
                                                       jobject obj,
                                                       jstring jstr)
{
    sfstring * sfstr = static_cast<sfstring*>(getFieldValue(env, obj));
    if (!sfstr) return;
    const char *str = env->GetStringUTFChars(jstr, 0);
    if (!str) { return; } // OutOfMemoryError
    sfstr->value = str;
    env->ReleaseStringUTFChars(jstr, str);
}

void JNICALL
Java_vrml_field_SFString_setValue__Lvrml_field_ConstSFString_2(JNIEnv * env,
                                                               jobject obj,
                                                               jobject value)
{
    sfstring* sfstr = static_cast<sfstring*>(getFieldValue(env, obj));
    sfstring* newSFString = static_cast<sfstring*>(getFieldValue(env, value));
    if (!sfstr || !newSFString) { return; }
    sfstr->value = newSFString->value;
}

void JNICALL
Java_vrml_field_SFString_setValue__Lvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFString_setValue__Lvrml_field_ConstSFString_2(env, obj,
                                                                 value);
}

/**
 * @brief JNI implementation of SFString::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFString object
 * @return String representation of SFString.
 */
jstring JNICALL Java_vrml_field_SFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstSFTime_CreateObject
  (JNIEnv *env, jobject obj, jdouble time)
{
    try {
        std::auto_ptr<openvrml::sftime> sftime(new openvrml::sftime(time));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(sftime.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFTime::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFTime object
 * @return Value of field
 */
jdouble JNICALL Java_vrml_field_ConstSFTime_getValue(JNIEnv * env, jobject obj)
{
    sftime * sft = static_cast<sftime*>(getFieldValue(env, obj));
    if (!sft) { return 0.0; }
    return jdouble(sft->value);
}

/**
 * @brief JNI implementation of ConstSFTime::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFTime object
 * @return String representation of ConstSFTime.
 */
jstring JNICALL Java_vrml_field_ConstSFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_SFTime_CreateObject
  (JNIEnv *env, jobject obj, jdouble time)
{
  Java_vrml_field_ConstSFTime_CreateObject(env, obj, time);
}

/**
 * @brief JNI implementation of SFTime::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFTime object
 * @return Value of field
 */
jdouble JNICALL Java_vrml_field_SFTime_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFTime_getValue(env, obj);
}

void JNICALL Java_vrml_field_SFTime_setValue__D(JNIEnv * env,
                                                jobject obj,
                                                jdouble time)
{
    sftime * sft = static_cast<sftime*>(getFieldValue(env, obj));
    if (!sft) { return; }
    sft->value = time;
}

void JNICALL
Java_vrml_field_SFTime_setValue__Lvrml_field_ConstSFTime_2(JNIEnv * env,
                                                           jobject obj,
                                                           jobject value)
{
    sftime* sft = static_cast<sftime*>(getFieldValue(env, obj));
    sftime* newsftime = static_cast<sftime*>(getFieldValue(env, value));
    if (!sft || !newsftime) { return; }
    sft->value = newsftime->value;
}

void JNICALL Java_vrml_field_SFTime_setValue__Lvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFTime_setValue__Lvrml_field_ConstSFTime_2(env, obj,
                                                             value);
}

/**
 * @brief JNI implementation of SFTime::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFTime object
 * @return String representation of SFTime.
 */
jstring JNICALL Java_vrml_field_SFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstSFVec2f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
    try {
        std::auto_ptr<openvrml::sfvec2f>
            sfvec2f(new openvrml::sfvec2f(vec2f(x, y)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfvec2f.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFVec2f::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2f object
 * @param[out] jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_ConstSFVec2f_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr)
{
    sfvec2f * sfv = static_cast<sfvec2f *>(getFieldValue(env, obj));
    if (!sfv) { return; }
    env->SetFloatArrayRegion(jarr, 0, 2, const_cast<float *>(&sfv->value[0]));
}

/**
 * @brief JNI implementation of ConstSFVec2f::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2f object
 * @return X-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec2f_getX(JNIEnv * env, jobject obj)
{
    sfvec2f * sfv = static_cast<sfvec2f*>(getFieldValue(env, obj));
    if (!sfv) { return 0.0; }
    return sfv->value.x();
}

/**
 * @brief JNI implementation of ConstSFVec2f::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2f object
 * @return Y-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec2f_getY(JNIEnv * env, jobject obj)
{
    sfvec2f * sfv = static_cast<sfvec2f*>(getFieldValue(env, obj));
    if (!sfv) { return 0.0; }
    return sfv->value.y();
}

/**
 * @brief JNI implementation of ConstSFVec2f::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2f object
 * @return String representation of ConstSFVec2f.
 */
jstring JNICALL Java_vrml_field_ConstSFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_SFVec2f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  Java_vrml_field_ConstSFVec2f_CreateObject(env, obj, x, y);
}

/**
 * @brief JNI implementation of SFVec2f::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2f object
 * @param[out] jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_SFVec2f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFVec2f_getValue(env, obj, jarr);
}

/**
 * @brief JNI implementation of SFVec2f::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2f object
 * @return X-component of vector
 */
jfloat JNICALL Java_vrml_field_SFVec2f_getX
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec2f_getX(env, obj);
}

/**
 * @brief JNI implementation of SFVec2f::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2f object
 * @return Y-component of vector
 */
jfloat JNICALL Java_vrml_field_SFVec2f_getY
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec2f_getY(env, obj);
}

void JNICALL Java_vrml_field_SFVec2f_setValue___3F(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr)
{
    sfvec2f * sfv = static_cast<sfvec2f*>(getFieldValue(env, obj));
    if (!sfv) return;
    jfloat * pjf = env->GetFloatArrayElements(jarr, NULL);
    sfv->value = vec2f(pjf[0], pjf[1]);
    env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_SFVec2f_setValue__FF(JNIEnv * env,
                                                  jobject obj,
                                                  jfloat x,
                                                  jfloat y)
{
    sfvec2f * sfv = static_cast<sfvec2f*>(getFieldValue(env, obj));
    if (!sfv) return;
    sfv->value = vec2f(x, y);
}

void JNICALL
Java_vrml_field_SFVec2f_setValue__Lvrml_field_ConstSFVec2f_2(JNIEnv * env,
                                                             jobject obj,
                                                             jobject value)
{
    sfvec2f * sfv = static_cast<sfvec2f*>(getFieldValue(env, obj));
    sfvec2f * newSFVec2f = static_cast<sfvec2f*>(getFieldValue(env, value));
    if (!sfv || !newSFVec2f) { return; }
    sfv->value = newSFVec2f->value;
}

void JNICALL
Java_vrml_field_SFVec2f_setValue__Lvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFVec2f_setValue__Lvrml_field_ConstSFVec2f_2(env, obj,
                                                               value);
}

/**
 * @brief JNI implementation of SFVec2f::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2f object
 * @return String representation of SFVec2f.
 */
jstring JNICALL Java_vrml_field_SFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstSFVec3f_CreateObject(JNIEnv * env,
                                                       jobject obj,
                                                       jfloat x,
                                                       jfloat y,
                                                       jfloat z)
{
    try {
        std::auto_ptr<openvrml::sfvec3f>
            sfvec3f(new openvrml::sfvec3f(vec3f(x, y, z)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfvec3f.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL Java_vrml_field_ConstSFVec2d_CreateObject
  (JNIEnv *env, jobject obj, jdouble x, jdouble y)
{
    try {
        std::auto_ptr<openvrml::sfvec2d>
            sfvec2d(new openvrml::sfvec2d(vec2d(x, y)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfvec2d.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFVec2d::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2d object
 * @param[out] jarr Value of field as a double array
 */
void JNICALL Java_vrml_field_ConstSFVec2d_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jdoubleArray jarr)
{
    sfvec2d * sfv = static_cast<sfvec2d *>(getFieldValue(env, obj));
    if (!sfv) { return; }
    env->SetFloatArrayRegion(jarr, 0, 2, const_cast<double *>(&sfv->value[0]));
}

/**
 * @brief JNI implementation of ConstSFVec2d::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2d object
 * @return X-component of vector
 */
jdouble JNICALL Java_vrml_field_ConstSFVec2d_getX(JNIEnv * env, jobject obj)
{
    sfvec2d * sfv = static_cast<sfvec2d*>(getFieldValue(env, obj));
    if (!sfv) { return 0.0; }
    return sfv->value.x();
}

/**
 * @brief JNI implementation of ConstSFVec2d::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2d object
 * @return Y-component of vector
 */
jdouble JNICALL Java_vrml_field_ConstSFVec2d_getY(JNIEnv * env, jobject obj)
{
    sfvec2d * sfv = static_cast<sfvec2d*>(getFieldValue(env, obj));
    if (!sfv) { return 0.0; }
    return sfv->value.y();
}

/**
 * @brief JNI implementation of ConstSFVec2d::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec2d object
 * @return String representation of ConstSFVec2d.
 */
jstring JNICALL Java_vrml_field_ConstSFVec2d_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_SFVec2d_CreateObject
  (JNIEnv *env, jobject obj, jdouble x, jdouble y)
{
  Java_vrml_field_ConstSFVec2d_CreateObject(env, obj, x, y);
}

/**
 * @brief JNI implementation of SFVec2d::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2d object
 * @param[out] jarr Value of field as a double array
 */
void JNICALL Java_vrml_field_SFVec2d_getValue
  (JNIEnv *env, jobject obj, jdoubleArray jarr)
{
  Java_vrml_field_ConstSFVec2d_getValue(env, obj, jarr);
}

/**
 * @brief JNI implementation of SFVec2d::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2d object
 * @return X-component of vector
 */
jdouble JNICALL Java_vrml_field_SFVec2d_getX
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec2d_getX(env, obj);
}

/**
 * @brief JNI implementation of SFVec2d::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2d object
 * @return Y-component of vector
 */
jdouble JNICALL Java_vrml_field_SFVec2d_getY
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec2d_getY(env, obj);
}

void JNICALL Java_vrml_field_SFVec2d_setValue___3F(JNIEnv * env,
                                                   jobject obj,
                                                   jdoubleArray jarr)
{
    sfvec2d * sfv = static_cast<sfvec2d*>(getFieldValue(env, obj));
    if (!sfv) return;
    jdouble * pjf = env->GetFloatArrayElements(jarr, NULL);
    sfv->value = vec2d(pjf[0], pjf[1]);
    env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_SFVec2d_setValue__FF(JNIEnv * env,
                                                  jobject obj,
                                                  jdouble x,
                                                  jdouble y)
{
    sfvec2d * sfv = static_cast<sfvec2d*>(getFieldValue(env, obj));
    if (!sfv) return;
    sfv->value = vec2d(x, y);
}

void JNICALL
Java_vrml_field_SFVec2d_setValue__Lvrml_field_ConstSFVec2d_2(JNIEnv * env,
                                                             jobject obj,
                                                             jobject value)
{
    sfvec2d * sfv = static_cast<sfvec2d*>(getFieldValue(env, obj));
    sfvec2d * newSFVec2d = static_cast<sfvec2d*>(getFieldValue(env, value));
    if (!sfv || !newSFVec2d) { return; }
    sfv->value = newSFVec2d->value;
}

void JNICALL
Java_vrml_field_SFVec2d_setValue__Lvrml_field_SFVec2d_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFVec2d_setValue__Lvrml_field_ConstSFVec2d_2(env, obj,
                                                               value);
}

/**
 * @brief JNI implementation of SFVec2d::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec2d object
 * @return String representation of SFVec2d.
 */
jstring JNICALL Java_vrml_field_SFVec2d_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstSFVec3f_CreateObject(JNIEnv * env,
                                                       jobject obj,
                                                       jdouble x,
                                                       jdouble y,
                                                       jdouble z)
{
    try {
        std::auto_ptr<openvrml::sfvec3f>
            sfvec3f(new openvrml::sfvec3f(vec3f(x, y, z)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfvec3f.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFVec3f::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3f object
 * @param[out] jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_ConstSFVec3f_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr)
{
    sfvec3f * sfv = static_cast<sfvec3f*>(getFieldValue(env, obj));
    if (!sfv) { return; }
    env->SetFloatArrayRegion(jarr, 0, 3, const_cast<float *>(&sfv->value[0]));
}

/**
 * @brief JNI implementation of ConstSFVec3f::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3f object
 * @return X-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec3f_getX(JNIEnv * env, jobject obj)
{
    sfvec3f * sfv = static_cast<sfvec3f *>(getFieldValue(env, obj));
    if (!sfv) { return 0.0; }
    return sfv->value.x();
}

/**
 * @brief JNI implementation of ConstSFVec3f::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3f object
 * @return Y-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec3f_getY(JNIEnv * env, jobject obj)
{
    sfvec3f * sfv = static_cast<sfvec3f *>(getFieldValue(env, obj));
    if (!sfv) { return 0.0; }
    return sfv->value.y();
}

/**
 * @brief JNI implementation of ConstSFVec3f::getZ.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3f object
 * @return Z-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec3f_getZ(JNIEnv * env, jobject obj)
{
    sfvec3f * sfv = static_cast<sfvec3f *>(getFieldValue(env, obj));
    if (!sfv) { return 0.0; }
    return sfv->value.z();
}

/**
 * @brief JNI implementation of ConstSFVec3f::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3f object
 * @return String representation of ConstSFVec3f.
 */
jstring JNICALL Java_vrml_field_ConstSFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_SFVec3f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  Java_vrml_field_ConstSFVec3f_CreateObject(env, obj, x, y, z);
}

/**
 * @brief JNI implementation of SFVec3f::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3f object
 * @param[out] jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_SFVec3f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFVec3f_getValue(env, obj, jarr);
}

/**
 * @brief JNI implementation of SFVec3f::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3f object
 * @return X-component of vector
 */
jfloat JNICALL Java_vrml_field_SFVec3f_getX
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3f_getX(env, obj);
}

/**
 * @brief JNI implementation of SFVec3f::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3f object
 * @return Y-component of vector
 */
jfloat JNICALL Java_vrml_field_SFVec3f_getY
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3f_getY(env, obj);
}

/**
 * @brief JNI implementation of SFVec3f::getZ.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3f object
 * @return Z-component of vector
 */
jfloat JNICALL Java_vrml_field_SFVec3f_getZ
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3f_getZ(env, obj);
}

void JNICALL Java_vrml_field_SFVec3f_setValue___3F(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr)
{
    sfvec3f * sfv = static_cast<sfvec3f *>(getFieldValue(env, obj));
    if (!sfv) { return; }
    jfloat * pjf = env->GetFloatArrayElements(jarr, NULL);
    sfv->value = vec3f(pjf[0], pjf[1], pjf[2]);
    env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_SFVec3f_setValue__FFF(JNIEnv * env,
                                                   jobject obj,
                                                   jfloat x,
                                                   jfloat y,
                                                   jfloat z)
{
    sfvec3f * sfv = static_cast<sfvec3f*>(getFieldValue(env, obj));
    if (!sfv) { return; }
    sfv->value = vec3f(x, y, z);
}

void JNICALL
Java_vrml_field_SFVec3f_setValue__Lvrml_field_ConstSFVec3f_2(JNIEnv * env,
                                                             jobject obj,
                                                             jobject value)
{
    sfvec3f * sfv = static_cast<sfvec3f*>(getFieldValue(env, obj));
    sfvec3f * newSFVec3f = static_cast<sfvec3f*>(getFieldValue(env, value));
    if (!sfv || !newSFVec3f) { return; }
    sfv->value = newSFVec3f->value; // Does not throw.
}

void JNICALL
Java_vrml_field_SFVec3f_setValue__Lvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFVec3f_setValue__Lvrml_field_ConstSFVec3f_2(env, obj,
                                                               value);
}

/**
 * @brief JNI implementation of SFVec3f::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3f object
 * @return String representation of SFVec3f.
 */
jstring JNICALL Java_vrml_field_SFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}


void JNICALL Java_vrml_field_ConstSFVec3d_CreateObject(JNIEnv * env,
                                                       jobject obj,
                                                       jdouble x,
                                                       jdouble y,
                                                       jdouble z)
{
    try {
        std::auto_ptr<openvrml::sfvec3d>
            sfvec3d(new openvrml::sfvec3d(vec3f(x, y, z)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(sfvec3d.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstSFVec3d::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3d object
 * @param[out] jarr Value of field as a double array
 */
void JNICALL Java_vrml_field_ConstSFVec3d_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jdoubleArray jarr)
{
    sfvec3d * sfv = static_cast<sfvec3d*>(getFieldValue(env, obj));
    if (!sfv) { return; }
    env->SetDoubleArrayRegion(jarr, 0, 3, const_cast<double *>(&sfv->value[0]));
}

/**
 * @brief JNI implementation of ConstSFVec3d::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3d object
 * @return X-component of vector
 */
jdouble JNICALL Java_vrml_field_ConstSFVec3d_getX(JNIEnv * env, jobject obj)
{
    sfvec3d * sfv = static_cast<sfvec3d *>(getFieldValue(env, obj));
    if (!sfv) { return 0.0; }
    return sfv->value.x();
}

/**
 * @brief JNI implementation of ConstSFVec3d::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3d object
 * @return Y-component of vector
 */
jdouble JNICALL Java_vrml_field_ConstSFVec3d_getY(JNIEnv * env, jobject obj)
{
    sfvec3d * sfv = static_cast<sfvec3d *>(getFieldValue(env, obj));
    if (!sfv) { return 0.0; }
    return sfv->value.y();
}

/**
 * @brief JNI implementation of ConstSFVec3d::getZ.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3d object
 * @return Z-component of vector
 */
jdouble JNICALL Java_vrml_field_ConstSFVec3d_getZ(JNIEnv * env, jobject obj)
{
    sfvec3d * sfv = static_cast<sfvec3d *>(getFieldValue(env, obj));
    if (!sfv) { return 0.0; }
    return sfv->value.z();
}

/**
 * @brief JNI implementation of ConstSFVec3d::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstSFVec3d object
 * @return String representation of ConstSFVec3d.
 */
jstring JNICALL Java_vrml_field_ConstSFVec3d_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_SFVec3d_CreateObject
  (JNIEnv *env, jobject obj, jdouble x, jdouble y, jdouble z)
{
  Java_vrml_field_ConstSFVec3d_CreateObject(env, obj, x, y, z);
}

/**
 * @brief JNI implementation of SFVec3d::getValue.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3d object
 * @param[out] jarr Value of field as a double array
 */
void JNICALL Java_vrml_field_SFVec3d_getValue
  (JNIEnv *env, jobject obj, jdoubleArray jarr)
{
  Java_vrml_field_ConstSFVec3d_getValue(env, obj, jarr);
}

/**
 * @brief JNI implementation of SFVec3d::getX.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3d object
 * @return X-component of vector
 */
jdouble JNICALL Java_vrml_field_SFVec3d_getX
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3d_getX(env, obj);
}

/**
 * @brief JNI implementation of SFVec3d::getY.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3d object
 * @return Y-component of vector
 */
jdouble JNICALL Java_vrml_field_SFVec3d_getY
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3d_getY(env, obj);
}

/**
 * @brief JNI implementation of SFVec3d::getZ.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3d object
 * @return Z-component of vector
 */
jdouble JNICALL Java_vrml_field_SFVec3d_getZ
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3d_getZ(env, obj);
}

void JNICALL Java_vrml_field_SFVec3d_setValue___3F(JNIEnv * env,
                                                   jobject obj,
                                                   jdoubleArray jarr)
{
    sfvec3d * sfv = static_cast<sfvec3d *>(getFieldValue(env, obj));
    if (!sfv) { return; }
    jdouble * pjf = env->GetDoubleArrayElements(jarr, NULL);
    sfv->value = vec3f(pjf[0], pjf[1], pjf[2]);
    env->ReleaseDoubleArrayElements(jarr, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_SFVec3d_setValue__FFF(JNIEnv * env,
                                                   jobject obj,
                                                   jdouble x,
                                                   jdouble y,
                                                   jdouble z)
{
    sfvec3d * sfv = static_cast<sfvec3d*>(getFieldValue(env, obj));
    if (!sfv) { return; }
    sfv->value = vec3f(x, y, z);
}

void JNICALL
Java_vrml_field_SFVec3d_setValue__Lvrml_field_ConstSFVec3d_2(JNIEnv * env,
                                                             jobject obj,
                                                             jobject value)
{
    sfvec3d * sfv = static_cast<sfvec3d*>(getFieldValue(env, obj));
    sfvec3d * newSFVec3d = static_cast<sfvec3d*>(getFieldValue(env, value));
    if (!sfv || !newSFVec3d) { return; }
    sfv->value = newSFVec3d->value; // Does not throw.
}

void JNICALL
Java_vrml_field_SFVec3d_setValue__Lvrml_field_SFVec3d_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFVec3d_setValue__Lvrml_field_ConstSFVec3d_2(env, obj,
                                                               value);
}

/**
 * @brief JNI implementation of SFVec3d::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj SFVec3d object
 * @return String representation of SFVec3d.
 */
jstring JNICALL Java_vrml_field_SFVec3d_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstMFBool_CreateObject(JNIEnv * env,
                                                      jobject obj,
                                                      jint size,
                                                      jboolArray value)
{
    try {
        jbool *pjf = env->GetBoolArrayElements(value, NULL);
        std::auto_ptr<openvrml::mfbool>
            mfbool(new openvrml::mfbool(pjf, pjf + size));
        env->ReleaseBoolArrayElements(value, pjf, JNI_ABORT);
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfbool.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

jint JNICALL Java_vrml_field_ConstMFBool_getSize(JNIEnv * env, jobject obj)
{
    mfbool * mff = static_cast<mfbool *>(getFieldValue(env, obj));
    if (!mff) return 0;
    return mff->value.size();
}

void JNICALL Java_vrml_field_ConstMFBool_getValue(JNIEnv * env,
                                                  jobject obj,
                                                  jboolArray jarr)
{
    const mfbool * const mff =
        static_cast<mfbool *>(getFieldValue(env, obj));
    if (!mff) { return; }
    const size_t size = mff->value.size();
    if (size > 0) {
        env->SetBoolArrayRegion(jarr, 0, size,
                                 const_cast<jbool *>(&mff->value[0]));
    }
}

jbool JNICALL Java_vrml_field_ConstMFBool_get1Value(JNIEnv * env,
                                                    jobject obj,
                                                    jint index)
{
    mfbool * mff = static_cast<mfbool *>(getFieldValue(env, obj));
    if (!mff) { return 0.0; }
    return mff->value[index];
}

/**
 * @brief JNI implementation of ConstMFBool::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFBool object
 * @return String representation of ConstMFBool.
 */
jstring JNICALL Java_vrml_field_ConstMFBool_toString(JNIEnv * env,
                                                     jobject obj)
{
    return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFBool_CreateObject(JNIEnv * env,
                                                 jobject obj,
                                                 jint size,
                                                 jboolArray value)
{
    Java_vrml_field_ConstMFBool_CreateObject(env, obj, size, value);
}

jint JNICALL Java_vrml_field_MFBool_getSize(JNIEnv * env, jobject obj)
{
    return Java_vrml_field_ConstMFBool_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFBool_clear(JNIEnv * env, jobject obj)
{
    mfbool * const mff = static_cast<mfbool *>(getFieldValue(env, obj));
    if (!mff) { return; }
    mff->value.clear();
}

void JNICALL Java_vrml_field_MFBool_delete(JNIEnv * env,
                                           jobject obj,
                                           jint index)
{
    mfbool * mff = static_cast<mfbool*>(getFieldValue(env, obj));
    if (!mff) { return; }
    if (!(size_t(index) < mff->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mff->value.erase(mff->value.begin() + index);
}

void JNICALL Java_vrml_field_MFBool_getValue(JNIEnv * env,
                                             jobject obj,
                                             jboolArray jarr)
{
    Java_vrml_field_ConstMFBool_getValue(env, obj, jarr);
}

jbool JNICALL Java_vrml_field_MFBool_get1Value(JNIEnv * env,
                                               jobject obj,
                                               jint index)
{
    return Java_vrml_field_ConstMFBool_get1Value(env, obj, index);
}

void JNICALL Java_vrml_field_MFBool_setValue__I_3F(JNIEnv * env,
                                                   jobject obj,
                                                   jint size,
                                                   jboolArray value)
{
    try {
        mfbool * const mff = static_cast<mfbool *>(getFieldValue(env, obj));
        if (!mff) { return; }
        mff->value.resize(size); // Throws bad_alloc.
        jbool * const bools = env->GetBoolArrayElements(value, 0);
        if (!bools) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (size_t i = 0; i < mff->value.size(); ++i) {
            if (!(jint(i) < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                    env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than "
                                              "values.length");
                break;
            }
            mff->value[i] = bools[i];
        }
        env->ReleaseBoolArrayElements(value, bools, JNI_ABORT);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFBool_setValue__Lvrml_field_MFBool_2(JNIEnv * env,
                                                      jobject obj,
                                                      jobject value)
{
    mfbool * const mff = static_cast<mfbool *>(getFieldValue(env, obj));
    const mfbool * const newMFBool =
        static_cast<mfbool*>(getFieldValue(env, value));
    if (!mff || !newMFBool) return;
    try {
        *mff = *newMFBool; // Throws bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFBool_setValue__Lvrml_field_ConstMFBool_2(JNIEnv * env,
                                                           jobject obj,
                                                           jobject value)
{
    Java_vrml_field_MFBool_setValue__Lvrml_field_MFBool_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFBool_set1Value__IF(JNIEnv * env,
                                                  jobject obj,
                                                  jint index,
                                                  jbool value)
{
    mfbool * mff = static_cast<mfbool *>(getFieldValue(env, obj));
    mff->value[index] = value;
}

void JNICALL
Java_vrml_field_MFBool_set1Value__ILvrml_field_ConstSFBool_2(JNIEnv * env,
                                                             jobject obj,
                                                             jint index,
                                                             jobject sfboolObj)
{
    mfbool * mff = static_cast<mfbool*>(getFieldValue(env, obj));
    sfbool * sff = static_cast<sfbool*>(getFieldValue(env, sfboolObj));
    if (!mff || !sff) { return; }
    mff->value[index] = sff->value;
}

void JNICALL
Java_vrml_field_MFBool_set1Value__ILvrml_field_SFBool_2(JNIEnv * env,
                                                        jobject obj,
                                                        jint index,
                                                        jobject sfbool)
{
  Java_vrml_field_MFBool_set1Value__ILvrml_field_ConstSFBool_2
    (env, obj, index, sfbool);
}


/**
 * @brief JNI implementation of MFBool.addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFBool object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFBool_addValue__F(JNIEnv * env,
                                                jobject obj,
                                                jbool value)
{
    mfbool * mff = static_cast<mfbool*>(getFieldValue(env, obj));
    if (!mff) { return; }
    try {
        mff->value.push_back(value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFBool.addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFBool object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFBool_addValue__Lvrml_field_ConstSFBool_2(JNIEnv * env,
                                                           jobject obj,
                                                           jobject value)
{
    mfbool * mff = static_cast<mfbool*>(getFieldValue(env, obj));
    sfbool * sff = static_cast<sfbool*>(getFieldValue(env, value));
    if (!mff || !sff) { return; }
    try {
        mff->value.push_back(sff->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFBool.addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFBool object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFBool_addValue__Lvrml_field_SFBool_2(JNIEnv * env,
                                                      jobject obj,
                                                      jobject value)
{
    Java_vrml_field_MFBool_addValue__Lvrml_field_ConstSFBool_2(env,
                                                               obj,
                                                               value);
}

void JNICALL Java_vrml_field_MFBool_insertValue__IF(JNIEnv * env,
                                                    jobject obj,
                                                    jint index,
                                                    jbool value)
{
    mfbool * mff = static_cast<mfbool *>(getFieldValue(env, obj));
    if (!mff) { return; }
    if (!(size_t(index) < mff->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
    }
    try {
        mff->value.insert(mff->value.begin() + index, value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFBool_insertValue__ILvrml_field_ConstSFBool_2(JNIEnv * env,
                                                               jobject obj,
                                                               jint index,
                                                               jobject value)
{
    sfbool * sff = static_cast<sfbool*>(getFieldValue(env, value));
    if (!sff) { return; }
    Java_vrml_field_MFBool_insertValue__IF(env, obj, index, sff->value);
}

void JNICALL
Java_vrml_field_MFBool_insertValue__ILvrml_field_SFBool_2(JNIEnv * env,
                                                          jobject obj,
                                                          jint index,
                                                          jobject value)
{
  Java_vrml_field_MFBool_insertValue__ILvrml_field_ConstSFBool_2(env,
                                                                 obj,
                                                                 index,
                                                                 value);
}

/**
 * @brief JNI implementation of MFBool.toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFBool object
 * @return String representation of MFBool.
 */
jstring JNICALL Java_vrml_field_MFBool_toString(JNIEnv * env, jobject obj)
{
    return fieldToString(env, obj);
}


void JNICALL Java_vrml_field_ConstMFColor_CreateObject___3_3F(
    JNIEnv * env,
    jobject obj,
    jobjectArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfcolor>
            mfcolor(new openvrml::mfcolor(env->GetArrayLength(jarr)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfcolor.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
    Java_vrml_field_MFColor_setValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_ConstMFColor_CreateObject__I_3F(JNIEnv * env,
                                                             jobject obj,
                                                             jint size,
                                                             jfloatArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfcolor> mfcolor(new openvrml::mfcolor(size));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfcolor.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }

    Java_vrml_field_MFColor_setValue__I_3F(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_ConstMFColor_getSize(JNIEnv * env, jobject obj)
{
    mfcolor * mfc = static_cast<mfcolor *>(getFieldValue(env, obj));
    if (!mfc) { return 0; }
    return mfc->value.size();
}

void JNICALL Java_vrml_field_ConstMFColor_getValue___3_3F(JNIEnv * env,
                                                          jobject obj,
                                                          jobjectArray jarr)
{
    const mfcolor * const mfc =
            static_cast<mfcolor *>(getFieldValue(env, obj));
    if (!mfc) { return; }
    for (size_t i = 0; i < mfc->value.size(); ++i) {
        jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
        if (!element) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
        jfloat * const arrayRegion =
                const_cast<jfloat *>(&mfc->value.at(i)[0]);
        env->SetFloatArrayRegion(element, 0, 3, arrayRegion);
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

void JNICALL Java_vrml_field_ConstMFColor_getValue___3F(JNIEnv * env,
                                                        jobject obj,
                                                        jfloatArray jarr)
{
    const mfcolor * const mfc =
            static_cast<mfcolor *>(getFieldValue(env, obj));
    if (!mfc) return;
    for (size_t i = 0; i < mfc->value.size(); ++i) {
        jfloat * const arrayRegion =
                const_cast<jfloat *>(&mfc->value.at(i)[0]);
        env->SetFloatArrayRegion(jarr, i * 3, 3, arrayRegion);
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

void JNICALL Java_vrml_field_ConstMFColor_get1Value__I_3F(JNIEnv * env,
                                                          jobject obj,
                                                          jint element,
                                                          jfloatArray jarr)
{
    mfcolor * mfc = static_cast<mfcolor*>(getFieldValue(env, obj));
    if (!mfc) { return; }
    try {
        jfloat * const arrayRegion =
                const_cast<jfloat*>(&mfc->value.at(element)[0]);
        env->SetFloatArrayRegion(jarr, 0, 3, arrayRegion);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_ConstMFColor_get1Value__ILvrml_field_SFColor_2(
        JNIEnv * env,
        jobject obj,
        jint element,
        jobject sfcolorObj)
{
    const mfcolor * const mfc =
        static_cast<mfcolor *>(getFieldValue(env, obj));
    if (!mfc) return;
    sfcolor * const sfc =
        static_cast<sfcolor *>(getFieldValue(env, sfcolorObj));
    if (!sfc) { return; }
    try {
        sfc->value = mfc->value.at(element);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstMFColor::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFColor object
 * @return String representation of ConstMFColor.
 */
jstring JNICALL Java_vrml_field_ConstMFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFColor_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  Java_vrml_field_ConstMFColor_CreateObject__I_3F(env, obj, size, jarr);
}

void JNICALL Java_vrml_field_MFColor_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFColor_CreateObject___3_3F(env, obj, jarr);
}

jint JNICALL Java_vrml_field_MFColor_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFColor_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFColor_clear(JNIEnv * env, jobject obj)
{
    mfcolor * const mfc = static_cast<mfcolor *>(getFieldValue(env, obj));
    if (!mfc) { return; }
    mfc->value.clear();
}

void JNICALL Java_vrml_field_MFColor_delete(JNIEnv * env,
                                            jobject obj,
                                            jint index)
{
    mfcolor * const mfc = static_cast<mfcolor*>(getFieldValue(env, obj));
    if (!mfc) { return; }
    if (!(size_t(index) < mfc->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mfc->value.erase(mfc->value.begin() + index);
}

void JNICALL Java_vrml_field_MFColor_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFColor_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFColor_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstMFColor_getValue___3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFColor_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  Java_vrml_field_ConstMFColor_get1Value__I_3F(env, obj, element, jarr);
}

void JNICALL
Java_vrml_field_MFColor_get1Value__ILvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jint element, jobject sfcolor)
{
  Java_vrml_field_ConstMFColor_get1Value__ILvrml_field_SFColor_2
    (env, obj, element, sfcolor);
}

void JNICALL Java_vrml_field_MFColor_setValue___3_3F(JNIEnv * env,
                                                     jobject obj,
                                                     jobjectArray value)
{
    try {
        mfcolor * const mfc =
                static_cast<mfcolor *>(getFieldValue(env, obj));
        if (!mfc) return;
        mfc->value.resize(env->GetArrayLength(value)); // Throws std::bad_alloc.
        for (size_t i = 0; i < mfc->value.size(); ++i) {
            jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(value, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return;
            }
            jfloat * const c = env->GetFloatArrayElements(element, 0);
            if (!c) {
                // Presumably we raised an OutOfMemoryError.
                return;
            }
            const color rgb(c[0], c[1], c[2]);
            env->ReleaseFloatArrayElements(element, c, JNI_ABORT);
            try {
                mfc->value.at(i) = rgb;
            } catch (std::out_of_range & ex) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                    env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, ex.what());
                break;
            }
        }
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL Java_vrml_field_MFColor_setValue__I_3F(JNIEnv * env,
                                                    jobject obj,
                                                    jint size,
                                                    jfloatArray value)
{
    try {
        mfcolor * const mfc = static_cast<mfcolor *>(getFieldValue(env, obj));
        if (!mfc) { return; }
        mfc->value.resize(size); // Throws std::bad_alloc.
        jfloat * const colors = env->GetFloatArrayElements(value, 0);
        if (!colors) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (jint i = 0; i < size; ++i) {
            if (!(i * 3 + 2 < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                    env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than colors.length");
                break;
            }
            const color rgb(colors[3 * i],
                            colors[3 * i + 1],
                            colors[3 * i + 2]);
            try {
                mfc->value.at(i) = rgb;
            } catch (std::out_of_range & ex) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                    env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, ex.what());
                break;
            }
        }
        env->ReleaseFloatArrayElements(value, colors, JNI_ABORT);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFColor_setValue__Lvrml_field_MFColor_2(JNIEnv * env,
                                                        jobject obj,
                                                        jobject value)
{
    mfcolor * const mfc = static_cast<mfcolor *>(getFieldValue(env, obj));
    const mfcolor * const newMFColor =
        static_cast<mfcolor *>(getFieldValue(env, value));
    if (!mfc || !newMFColor) { return; }
    try {
        *mfc = *newMFColor; // Throws bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFColor_setValue__Lvrml_field_ConstMFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFColor_setValue__Lvrml_field_MFColor_2(env, obj, value);
}

void JNICALL
Java_vrml_field_MFColor_set1Value__ILvrml_field_ConstSFColor_2(
    JNIEnv * env,
    jobject obj,
    jint index,
    jobject sfcolor_object)
{
    mfcolor * mfc = static_cast<mfcolor*>(getFieldValue(env, obj));
    sfcolor * sfcolorptr =
        static_cast<sfcolor*>(getFieldValue(env, sfcolor_object));
    if (!mfc || !sfcolorptr) { return; }
    try {
        mfc->value.at(index) = sfcolorptr->value;
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFColor_set1Value__ILvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jint index, jobject sfcolor)
{
  Java_vrml_field_MFColor_set1Value__ILvrml_field_ConstSFColor_2
    (env, obj, index, sfcolor);
}

void JNICALL Java_vrml_field_MFColor_set1Value__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat red, jfloat green,
   jfloat blue)
{
    mfcolor * mfc = static_cast<mfcolor*>(getFieldValue(env, obj));
    if (!mfc) { return; }
    try {
        mfc->value.at(index) = color(red, green, blue);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFColor::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFColor object
 * @param[in] sfcolorObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFColor_addValue__Lvrml_field_ConstSFColor_2(
    JNIEnv * env,
    jobject obj,
    jobject sfcolor_object)
{
    mfcolor * mfc = static_cast<mfcolor*>(getFieldValue(env, obj));
    sfcolor * sfc = static_cast<sfcolor*>(getFieldValue(env, sfcolor_object));
    if (!mfc || !sfc) { return; }
    try {
        mfc->value.push_back(sfc->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFColor::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFColor object
 * @param[in] sfcolor Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFColor_addValue__Lvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jobject sfcolor)
{
  Java_vrml_field_MFColor_addValue__Lvrml_field_ConstSFColor_2(env, obj,
                                                               sfcolor);
}

/**
 * @brief JNI implementation of MFColor::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFColor object
 * @param[in] red Red component of color to add to end of list
 * @param[in] green Green component of color to add to end of list
 * @param[in] blue Blue component of color to add to end of list
 */
void JNICALL Java_vrml_field_MFColor_addValue__FFF(JNIEnv * env,
                                                   jobject obj,
                                                   jfloat red,
                                                   jfloat green,
                                                   jfloat blue)
{
    mfcolor * mfc = static_cast<mfcolor*>(getFieldValue(env, obj));
    if (!mfc) { return; }
    try {
        mfc->value.push_back(color(red, green, blue));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFColor_insertValue__ILvrml_field_ConstSFColor_2(
    JNIEnv * env,
    jobject obj,
    jint index,
    jobject sfcolor_object)
{
    sfcolor * sfcolorptr =
        static_cast<sfcolor*>(getFieldValue(env, sfcolor_object));
    if (!sfcolorptr) { return; }
    Java_vrml_field_MFColor_insertValue__IFFF(env, obj, index,
                                              sfcolorptr->value.r(),
                                              sfcolorptr->value.g(),
                                              sfcolorptr->value.b());
}

void JNICALL
Java_vrml_field_MFColor_insertValue__ILvrml_field_SFColor_2(JNIEnv * env,
                                                            jobject obj,
                                                            jint index,
                                                            jobject sfcolor)
{
    Java_vrml_field_MFColor_insertValue__ILvrml_field_ConstSFColor_2
            (env, obj, index, sfcolor);
}

void JNICALL Java_vrml_field_MFColor_insertValue__IFFF(JNIEnv * env,
                                                       jobject obj,
                                                       jint index,
                                                       jfloat red,
                                                       jfloat green,
                                                       jfloat blue)
{
    mfcolor * mfc = static_cast<mfcolor*>(getFieldValue(env, obj));
    if (!mfc) { return; }
    if (!(size_t(index) < mfc->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
    }
    try {
        mfc->value.insert(mfc->value.begin() + index, color(red, green, blue));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFColor::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFColor object
 * @return String representation of MFColor.
 */
jstring JNICALL Java_vrml_field_MFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstMFFloat_CreateObject
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
    try {
        jfloat *pjf = env->GetFloatArrayElements(value, NULL);
        std::auto_ptr<openvrml::mffloat>
            mffloat(new openvrml::mffloat(pjf, pjf + size));
        env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(mffloat.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

jint JNICALL Java_vrml_field_ConstMFFloat_getSize(JNIEnv * env, jobject obj) {
    mffloat * mff = static_cast<mffloat *>(getFieldValue(env, obj));
    if (!mff) return 0;
    return mff->value.size();
}

void JNICALL Java_vrml_field_ConstMFFloat_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr)
{
    const mffloat * const mff =
        static_cast<mffloat *>(getFieldValue(env, obj));
    if (!mff) { return; }
    const size_t size = mff->value.size();
    if (size > 0) {
        env->SetFloatArrayRegion(jarr, 0, size,
                                 const_cast<jfloat *>(&mff->value[0]));
    }
}

jfloat JNICALL Java_vrml_field_ConstMFFloat_get1Value(JNIEnv * env,
                                                      jobject obj,
                                                      jint index)
{
    mffloat * mff = static_cast<mffloat *>(getFieldValue(env, obj));
    if (!mff) { return 0.0; }
    return mff->value[index];
}

/**
 * @brief JNI implementation of ConstMFFloat::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFFloat object
 * @return String representation of ConstMFFloat.
 */
jstring JNICALL Java_vrml_field_ConstMFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFFloat_CreateObject
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  Java_vrml_field_ConstMFFloat_CreateObject(env, obj, size, value);
}

jint JNICALL Java_vrml_field_MFFloat_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFFloat_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFFloat_clear(JNIEnv * env, jobject obj)
{
    mffloat * const mff = static_cast<mffloat *>(getFieldValue(env, obj));
    if (!mff) { return; }
    mff->value.clear();
}

void JNICALL Java_vrml_field_MFFloat_delete(JNIEnv * env,
                                            jobject obj,
                                            jint index)
{
    mffloat * mff = static_cast<mffloat*>(getFieldValue(env, obj));
    if (!mff) { return; }
    if (!(size_t(index) < mff->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mff->value.erase(mff->value.begin() + index);
}

void JNICALL Java_vrml_field_MFFloat_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstMFFloat_getValue(env, obj, jarr);
}

jfloat JNICALL Java_vrml_field_MFFloat_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFFloat_get1Value(env, obj, index);
}

void JNICALL Java_vrml_field_MFFloat_setValue__I_3F(JNIEnv * env,
                                                    jobject obj,
                                                    jint size,
                                                    jfloatArray value)
{
    try {
        mffloat * const mff = static_cast<mffloat *>(getFieldValue(env, obj));
        if (!mff) { return; }
        mff->value.resize(size); // Throws bad_alloc.
        jfloat * const floats = env->GetFloatArrayElements(value, 0);
        if (!floats) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (size_t i = 0; i < mff->value.size(); ++i) {
            if (!(jint(i) < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                    env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than "
                                              "values.length");
                break;
            }
            mff->value[i] = floats[i];
        }
        env->ReleaseFloatArrayElements(value, floats, JNI_ABORT);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFFloat_setValue__Lvrml_field_MFFloat_2(JNIEnv * env,
                                                        jobject obj,
                                                        jobject value)
{
    mffloat * const mff = static_cast<mffloat *>(getFieldValue(env, obj));
    const mffloat * const newMFFloat =
        static_cast<mffloat*>(getFieldValue(env, value));
    if (!mff || !newMFFloat) return;
    try {
        *mff = *newMFFloat; // Throws bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFFloat_setValue__Lvrml_field_ConstMFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFFloat_setValue__Lvrml_field_MFFloat_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFFloat_set1Value__IF(JNIEnv * env,
                                                   jobject obj,
                                                   jint index,
                                                   jfloat value)
{
    mffloat * mff = static_cast<mffloat *>(getFieldValue(env, obj));
    mff->value[index] = value;
}

void JNICALL
Java_vrml_field_MFFloat_set1Value__ILvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject sffloatObj)
{
    mffloat * mff = static_cast<mffloat*>(getFieldValue(env, obj));
    sffloat * sff = static_cast<sffloat*>(getFieldValue(env, sffloatObj));
    if (!mff || !sff) { return; }
    mff->value[index] = sff->value;
}

void JNICALL
Java_vrml_field_MFFloat_set1Value__ILvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject sffloat)
{
  Java_vrml_field_MFFloat_set1Value__ILvrml_field_ConstSFFloat_2
    (env, obj, index, sffloat);
}


/**
 * @brief JNI implementation of MFFloat::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFFloat object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFFloat_addValue__F(JNIEnv * env,
                                                 jobject obj,
                                                 jfloat value)
{
    mffloat * mff = static_cast<mffloat*>(getFieldValue(env, obj));
    if (!mff) { return; }
    try {
        mff->value.push_back(value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFFloat::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFFloat object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFFloat_addValue__Lvrml_field_ConstSFFloat_2(JNIEnv * env,
                                                             jobject obj,
                                                             jobject value)
{
    mffloat * mff = static_cast<mffloat*>(getFieldValue(env, obj));
    sffloat * sff = static_cast<sffloat*>(getFieldValue(env, value));
    if (!mff || !sff) { return; }
    try {
        mff->value.push_back(sff->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFFloat::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFFloat object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFFloat_addValue__Lvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFFloat_addValue__Lvrml_field_ConstSFFloat_2(env, obj,
                                                               value);
}

void JNICALL Java_vrml_field_MFFloat_insertValue__IF(JNIEnv * env,
                                                     jobject obj,
                                                     jint index,
                                                     jfloat value)
{
    mffloat * mff = static_cast<mffloat *>(getFieldValue(env, obj));
    if (!mff) { return; }
    if (!(size_t(index) < mff->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
    }
    try {
        mff->value.insert(mff->value.begin() + index, value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFFloat_insertValue__ILvrml_field_ConstSFFloat_2(JNIEnv * env,
                                                                 jobject obj,
                                                                 jint index,
                                                                 jobject value)
{
    sffloat * sff = static_cast<sffloat*>(getFieldValue(env, value));
    if (!sff) { return; }
    Java_vrml_field_MFFloat_insertValue__IF(env, obj, index, sff->value);
}

void JNICALL
Java_vrml_field_MFFloat_insertValue__ILvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  Java_vrml_field_MFFloat_insertValue__ILvrml_field_ConstSFFloat_2
    (env, obj, index, value);
}

/**
 * @brief JNI implementation of MFFloat::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFFloat object
 * @return String representation of MFFloat.
 */
jstring JNICALL Java_vrml_field_MFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}


void JNICALL Java_vrml_field_ConstMFDouble_CreateObject
  (JNIEnv *env, jobject obj, jint size, jdoubleArray value)
{
    try {
        jdouble *pjf = env->GetDoubleArrayElements(value, NULL);
        std::auto_ptr<openvrml::mfdouble>
            mfdouble(new openvrml::mfdouble(pjf, pjf + size));
        env->ReleaseDoubleArrayElements(value, pjf, JNI_ABORT);
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfdouble.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

jint JNICALL Java_vrml_field_ConstMFDouble_getSize(JNIEnv * env, jobject obj) {
    mfdouble * mff = static_cast<mfdouble *>(getFieldValue(env, obj));
    if (!mff) return 0;
    return mff->value.size();
}

void JNICALL Java_vrml_field_ConstMFDouble_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jdoubleArray jarr)
{
    const mfdouble * const mff =
        static_cast<mfdouble *>(getFieldValue(env, obj));
    if (!mff) { return; }
    const size_t size = mff->value.size();
    if (size > 0) {
        env->SetDoubleArrayRegion(jarr, 0, size,
                                 const_cast<jdouble *>(&mff->value[0]));
    }
}

jdouble JNICALL Java_vrml_field_ConstMFDouble_get1Value(JNIEnv * env,
                                                      jobject obj,
                                                      jint index)
{
    mfdouble * mff = static_cast<mfdouble *>(getFieldValue(env, obj));
    if (!mff) { return 0.0; }
    return mff->value[index];
}

/**
 * @brief JNI implementation of ConstMFDouble::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFDouble object
 * @return String representation of ConstMFDouble.
 */
jstring JNICALL Java_vrml_field_ConstMFDouble_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFDouble_CreateObject
  (JNIEnv *env, jobject obj, jint size, jdoubleArray value)
{
  Java_vrml_field_ConstMFDouble_CreateObject(env, obj, size, value);
}

jint JNICALL Java_vrml_field_MFDouble_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFDouble_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFDouble_clear(JNIEnv * env, jobject obj)
{
    mfdouble * const mff = static_cast<mfdouble *>(getFieldValue(env, obj));
    if (!mff) { return; }
    mff->value.clear();
}

void JNICALL Java_vrml_field_MFDouble_delete(JNIEnv * env,
                                            jobject obj,
                                            jint index)
{
    mfdouble * mff = static_cast<mfdouble*>(getFieldValue(env, obj));
    if (!mff) { return; }
    if (!(size_t(index) < mff->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mff->value.erase(mff->value.begin() + index);
}

void JNICALL Java_vrml_field_MFDouble_getValue
  (JNIEnv *env, jobject obj, jdoubleArray jarr)
{
  Java_vrml_field_ConstMFDouble_getValue(env, obj, jarr);
}

jdouble JNICALL Java_vrml_field_MFDouble_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFDouble_get1Value(env, obj, index);
}

void JNICALL Java_vrml_field_MFDouble_setValue__I_3F(JNIEnv * env,
                                                    jobject obj,
                                                    jint size,
                                                    jdoubleArray value)
{
    try {
        mfdouble * const mff = static_cast<mfdouble *>(getFieldValue(env, obj));
        if (!mff) { return; }
        mff->value.resize(size); // Throws bad_alloc.
        jdouble * const doubles = env->GetDoubleArrayElements(value, 0);
        if (!doubles) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (size_t i = 0; i < mff->value.size(); ++i) {
            if (!(jint(i) < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                    env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than "
                                              "values.length");
                break;
            }
            mff->value[i] = doubles[i];
        }
        env->ReleaseDoubleArrayElements(value, doubles, JNI_ABORT);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFDouble_setValue__Lvrml_field_MFDouble_2(JNIEnv * env,
                                                        jobject obj,
                                                        jobject value)
{
    mfdouble * const mff = static_cast<mfdouble *>(getFieldValue(env, obj));
    const mfdouble * const newMFDouble =
        static_cast<mfdouble*>(getFieldValue(env, value));
    if (!mff || !newMFDouble) return;
    try {
        *mff = *newMFDouble; // Throws bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFDouble_setValue__Lvrml_field_ConstMFDouble_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFDouble_setValue__Lvrml_field_MFDouble_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFDouble_set1Value__IF(JNIEnv * env,
                                                   jobject obj,
                                                   jint index,
                                                   jdouble value)
{
    mfdouble * mff = static_cast<mfdouble *>(getFieldValue(env, obj));
    mff->value[index] = value;
}

void JNICALL
Java_vrml_field_MFDouble_set1Value__ILvrml_field_ConstSFDouble_2
  (JNIEnv *env, jobject obj, jint index, jobject sfdoubleObj)
{
    mfdouble * mff = static_cast<mfdouble*>(getFieldValue(env, obj));
    sfdouble * sff = static_cast<sfdouble*>(getFieldValue(env, sfdoubleObj));
    if (!mff || !sff) { return; }
    mff->value[index] = sff->value;
}

void JNICALL
Java_vrml_field_MFDouble_set1Value__ILvrml_field_SFDouble_2
  (JNIEnv *env, jobject obj, jint index, jobject sfdouble)
{
  Java_vrml_field_MFDouble_set1Value__ILvrml_field_ConstSFDouble_2
    (env, obj, index, sfdouble);
}


/**
 * @brief JNI implementation of MFDouble::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFDouble object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFDouble_addValue__F(JNIEnv * env,
                                                 jobject obj,
                                                 jdouble value)
{
    mfdouble * mff = static_cast<mfdouble*>(getFieldValue(env, obj));
    if (!mff) { return; }
    try {
        mff->value.push_back(value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFDouble::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFDouble object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFDouble_addValue__Lvrml_field_ConstSFDouble_2(JNIEnv * env,
                                                             jobject obj,
                                                             jobject value)
{
    mfdouble * mff = static_cast<mfdouble*>(getFieldValue(env, obj));
    sfdouble * sff = static_cast<sfdouble*>(getFieldValue(env, value));
    if (!mff || !sff) { return; }
    try {
        mff->value.push_back(sff->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFDouble::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFDouble object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFDouble_addValue__Lvrml_field_SFDouble_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFDouble_addValue__Lvrml_field_ConstSFDouble_2(env, obj,
                                                               value);
}

void JNICALL Java_vrml_field_MFDouble_insertValue__IF(JNIEnv * env,
                                                     jobject obj,
                                                     jint index,
                                                     jdouble value)
{
    mfdouble * mff = static_cast<mfdouble *>(getFieldValue(env, obj));
    if (!mff) { return; }
    if (!(size_t(index) < mff->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
    }
    try {
        mff->value.insert(mff->value.begin() + index, value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFDouble_insertValue__ILvrml_field_ConstSFDouble_2(JNIEnv * env,
                                                                 jobject obj,
                                                                 jint index,
                                                                 jobject value)
{
    sfdouble * sff = static_cast<sfdouble*>(getFieldValue(env, value));
    if (!sff) { return; }
    Java_vrml_field_MFDouble_insertValue__IF(env, obj, index, sff->value);
}

void JNICALL
Java_vrml_field_MFDouble_insertValue__ILvrml_field_SFDouble_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  Java_vrml_field_MFDouble_insertValue__ILvrml_field_ConstSFDouble_2
    (env, obj, index, value);
}

/**
 * @brief JNI implementation of MFDouble::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFDouble object
 * @return String representation of MFDouble.
 */
jstring JNICALL Java_vrml_field_MFDouble_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstMFInt32_CreateObject(JNIEnv * env,
                                                       jobject obj,
                                                       jint size,
                                                       jintArray value)
{
    try {
        jint * pji = env->GetIntArrayElements(value, NULL);
        std::auto_ptr<mfint32> pMFInt32(new mfint32(pji, pji + size));
        env->ReleaseIntArrayElements(value, pji, JNI_ABORT);
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(pMFInt32.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

jint JNICALL Java_vrml_field_ConstMFInt32_getSize(JNIEnv * env, jobject obj)
{
    mfint32 * mfi = static_cast<mfint32 *>(getFieldValue(env, obj));
    if (!mfi) return 0;
    return mfi->value.size();
}

void JNICALL Java_vrml_field_ConstMFInt32_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jintArray jarr)
{
    const mfint32 * const mfi = static_cast<mfint32*>(getFieldValue(env, obj));
    if (!mfi) return;
    const size_t size = mfi->value.size();
    if (size > 0) {
        try {
            std::vector<jint>
                jintVec(&mfi->value[0],
                        &mfi->value[0] + size); // Throws bad_alloc.
            env->SetIntArrayRegion(jarr, 0, size, &jintVec[0]);
        } catch (std::bad_alloc & ex) {
            env->ExceptionDescribe();
            env->ExceptionClear();
            jclass exceptionClass =
                env->FindClass("java/lang/OutOfMemoryError");
            if (!exceptionClass) {
                // Presumably FindClass raised an exception.
                return;
            }
            env->ThrowNew(exceptionClass, ex.what());
        }
    }
}

jint JNICALL Java_vrml_field_ConstMFInt32_get1Value(JNIEnv * env,
                                                    jobject obj,
                                                    jint index)
{
    mfint32 * mfi = static_cast<mfint32 *>(getFieldValue(env, obj));
    if (!mfi) { return 0; }
    return mfi->value[index];
}

/**
 * @brief JNI implementation of ConstMFInt32::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFInt32 object
 * @return String representation of ConstMFInt32.
 */
jstring JNICALL Java_vrml_field_ConstMFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint size, jintArray value)
{
  Java_vrml_field_ConstMFInt32_CreateObject(env, obj, size, value);
}

jint JNICALL Java_vrml_field_MFInt32_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFInt32_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFInt32_clear(JNIEnv * env, jobject obj)
{
    mfint32 * const mfi = static_cast<mfint32 *>(getFieldValue(env, obj));
    if (!mfi) { return; }
    mfi->value.clear();
}

void JNICALL Java_vrml_field_MFInt32_delete(JNIEnv * env, jobject obj, jint n)
{
    mfint32 * mfi = static_cast<mfint32*>(getFieldValue(env, obj));
    if (!mfi) { return; }
    if (!(size_t(n) < mfi->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mfi->value.erase(mfi->value.begin() + n);
}

void JNICALL Java_vrml_field_MFInt32_getValue
  (JNIEnv *env, jobject obj, jintArray jarr)
{
  Java_vrml_field_ConstMFInt32_getValue(env, obj, jarr);
}

jint JNICALL Java_vrml_field_MFInt32_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFInt32_get1Value(env, obj, index);
}

void JNICALL Java_vrml_field_MFInt32_setValue__I_3I(JNIEnv * env,
                                                    jobject obj,
                                                    jint size,
                                                    jintArray value)
{
    try {
        mfint32 * const mfi = static_cast<mfint32 *>(getFieldValue(env, obj));
        if (!mfi) return;
        mfi->value.resize(size); // Throws bad_alloc.
        jint * ints = env->GetIntArrayElements(value, 0);
        if (!ints) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (jint i = 0; i < size; ++i) {
            if (!(i < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                    env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than "
                                              "values.length");
                break;
            }
            mfi->value[i] = ints[i];
        }
        env->ReleaseIntArrayElements(value, ints, JNI_ABORT);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFInt32_setValue__Lvrml_field_MFInt32_2(JNIEnv * env,
                                                        jobject obj,
                                                        jobject value)
{
    mfint32 * const mfi = static_cast<mfint32 *>(getFieldValue(env, obj));
    const mfint32 * const newMFInt32 =
        static_cast<mfint32 *>(getFieldValue(env, value));
    if (!mfi || !newMFInt32) return;
    try {
        *mfi = *newMFInt32; // Throws bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFInt32_setValue__Lvrml_field_ConstMFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFInt32_setValue__Lvrml_field_MFInt32_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFInt32_set1Value__II(JNIEnv * env,
                                                   jobject obj,
                                                   jint index,
                                                   jint value)
{
    mfint32 * mfi = static_cast<mfint32*>(getFieldValue(env, obj));
    if (!mfi) { return; }
    mfi->value[index] = value;
}

void JNICALL
Java_vrml_field_MFInt32_set1Value__ILvrml_field_ConstSFInt32_2(
    JNIEnv * env,
    jobject obj,
    jint index,
    jobject sfint32_object)
{
    mfint32 * mfi = static_cast<mfint32*>(getFieldValue(env, obj));
    sfint32 * sfi = static_cast<sfint32*>(getFieldValue(env, sfint32_object));
    if (!mfi || !sfi) { return; }
    mfi->value[index] = sfi->value;
}

void JNICALL
Java_vrml_field_MFInt32_set1Value__ILvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject sfint)
{
  Java_vrml_field_MFInt32_set1Value__ILvrml_field_ConstSFInt32_2(env, obj,
                                                                 index, sfint);
}

/**
 * @brief JNI implementation of MFInt32::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFInt32 object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFInt32_addValue__I(JNIEnv * env,
                                                 jobject obj,
                                                 jint value)
{
    mfint32 * mfi = static_cast<mfint32 *>(getFieldValue(env, obj));
    if (!mfi) { return; }
    try {
        mfi->value.push_back(value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFInt32::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFInt32 object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFInt32_addValue__Lvrml_field_ConstSFInt32_2(JNIEnv * env,
                                                             jobject obj,
                                                             jobject value)
{
    mfint32 * mfi = static_cast<mfint32*>(getFieldValue(env, obj));
    sfint32 * sfi = static_cast<sfint32*>(getFieldValue(env, value));
    if (!mfi || !sfi) { return; }
    try {
        mfi->value.push_back(sfi->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFInt32::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFInt32 object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFInt32_addValue__Lvrml_field_SFInt32_2(JNIEnv *env,
                                                        jobject obj,
                                                        jobject value)
{
    Java_vrml_field_MFInt32_addValue__Lvrml_field_ConstSFInt32_2(env, obj,
                                                                 value);
}

void JNICALL Java_vrml_field_MFInt32_insertValue__II(JNIEnv *env,
                                                     jobject obj,
                                                     jint index,
                                                     jint value)
{
    mfint32 * mfi = static_cast<mfint32*>(getFieldValue(env, obj));
    if (!mfi) { return; }
    if (!(size_t(index) < mfi->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
    }
    try {
        mfi->value.insert(mfi->value.begin() + index, value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFInt32_insertValue__ILvrml_field_ConstSFInt32_2(JNIEnv * env,
                                                                 jobject obj,
                                                                 jint index,
                                                                 jobject value)
{
    sfint32 * sfi = static_cast<sfint32*>(getFieldValue(env, value));
    if (!sfi) { return; }
    Java_vrml_field_MFInt32_insertValue__II(env, obj, index, sfi->value);
}

void JNICALL
Java_vrml_field_MFInt32_insertValue__ILvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  Java_vrml_field_MFInt32_insertValue__ILvrml_field_ConstSFInt32_2
    (env, obj, index, value);
}

/**
 * @brief JNI implementation of MFInt32::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFInt32 object
 * @return String representation of MFInt32.
 */
jstring JNICALL Java_vrml_field_MFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstMFNode_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfnode> mfnode(new openvrml::mfnode(size));
        jfieldID fid;

        for (jint i = 0; i < size; i++) {
            jobject jNode = env->GetObjectArrayElement(jarr, i);
            fid = getFid(env, jNode, "NodePtr", "I");
            if (!fid) { return; }
            node * const n =
                reinterpret_cast<node *>(env->GetIntField(jNode, fid));
            if (!n) { return; }
            mfnode->value[i].reset(n);
        }

        fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfnode.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

jint JNICALL Java_vrml_field_ConstMFNode_getSize(JNIEnv * env, jobject obj)
{
    mfnode * mfn = static_cast<mfnode *>(getFieldValue(env, obj));
    if (!mfn) { return 0; }
    return mfn->value.size();
}

void JNICALL Java_vrml_field_ConstMFNode_getValue(JNIEnv * env,
                                                  jobject obj,
                                                  jobjectArray basenode)
{
    mfnode * mfn = static_cast<mfnode*>(getFieldValue(env, obj));
    if (!mfn) { return; }
    std::vector<node_ptr>::size_type arraySize = mfn->value.size();
    jclass clazz = env->FindClass("vrml/node/Node");
    jobject jNode;
    jfieldID fid = env->GetFieldID(clazz, "NodePtr", "I");
    if (!fid) { return; }

    for (std::vector<node_ptr>::size_type pos = 0; pos < arraySize; ++pos) {
        jNode = env->AllocObject(clazz);
        env->SetIntField(jNode, fid,
                         reinterpret_cast<int>(mfn->value[pos].get()));
        env->SetObjectArrayElement(basenode, pos, jNode);
    }
}

/**
 * @brief JNI implementation of ConstMFNode::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFNode object
 * @param[in] index Position of desired element
 * @return Element at specified position
 */
jobject JNICALL Java_vrml_field_ConstMFNode_get1Value(JNIEnv * env,
                                                      jobject obj,
                                                      jint index)
{
    mfnode * mfn = static_cast<mfnode*>(getFieldValue(env, obj));
    if (!mfn) { return 0; }
    jclass clazz = env->FindClass("vrml/node/Node");
    jobject jNode = env->AllocObject(clazz);
    jfieldID fid = env->GetFieldID(clazz, "NodePtr", "I");
    if (!fid) { return 0; }
    env->SetIntField(jNode, fid,
                     reinterpret_cast<int>(mfn->value[index].get()));
    return jNode;
}

/**
 * @brief JNI implementation of ConstMFNode::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFNode object
 * @return String representation of ConstMFNode.
 */
jstring JNICALL Java_vrml_field_ConstMFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFNode_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray basenode)
{
  Java_vrml_field_ConstMFNode_CreateObject(env, obj, size, basenode);
}

jint JNICALL Java_vrml_field_MFNode_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFNode_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFNode_clear(JNIEnv * env, jobject obj)
{
    mfnode * mfn = static_cast<mfnode*>(getFieldValue(env, obj));
    if (!mfn) { return; }
    mfn->value.clear();
}

void JNICALL Java_vrml_field_MFNode_delete(JNIEnv * env,
                                           jobject obj,
                                           jint index)
{
    mfnode * mfn = static_cast<mfnode*>(getFieldValue(env, obj));
    if (!mfn) { return; }
    if (!(size_t(index) < mfn->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mfn->value.erase(mfn->value.begin() + index);
}

void JNICALL Java_vrml_field_MFNode_getValue
  (JNIEnv *env, jobject obj, jobjectArray basenode)
{
  Java_vrml_field_ConstMFNode_getValue(env, obj, basenode);
}

/**
 * @brief JNI implementation of MFNode::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFNode object
 * @param[in] index Position of desired element
 * @return Element at specified position
 */
jobject JNICALL Java_vrml_field_MFNode_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFNode_get1Value(env, obj, index);
}

/**
 * @brief JNI implementation of MFNode::setValue
 *
 * @param[in] env JNI environment.
 * @param[in,out] obj MFNode object.
 * @param[in] size Number of elements to set
 * @param[in] basenode Array of BaseNodes to set value of field to
 *
 * @todo Implement me!
 */
void JNICALL
Java_vrml_field_MFNode_setValue__I_3Lvrml_BaseNode_2(JNIEnv *,
                                                     jobject,
                                                     jint,
                                                     jobjectArray)
{}

void JNICALL
Java_vrml_field_MFNode_setValue__Lvrml_field_MFNode_2(JNIEnv * env,
                                                      jobject obj,
                                                      jobject value)
{
    mfnode * mfn = static_cast<mfnode*>(getFieldValue(env, obj));
    mfnode* newMFNode = static_cast<mfnode*>(getFieldValue(env, value));
    if (!mfn || !newMFNode) { return; }

    try {
        *mfn = *newMFNode; // Throws std::bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFNode_setValue__Lvrml_field_ConstMFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFNode_setValue__Lvrml_field_MFNode_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFNode_set1Value__ILvrml_BaseNode_2(JNIEnv * env,
                                                                 jobject obj,
                                                                 jint pos,
                                                                 jobject value)
{
    mfnode * mfn = static_cast<mfnode*>(getFieldValue(env, obj));
    if (!mfn) { return; }
    jfieldID fid = getFid(env, value, "NodePtr", "I");
    if (!fid) { return; }
    node * const newNode =
        reinterpret_cast<node *>(env->GetIntField(value, fid));
    if (!newNode) { return; }
    mfn->value[pos] = node_ptr(newNode);
}

void JNICALL
Java_vrml_field_MFNode_set1Value__ILvrml_field_ConstSFNode_2(
    JNIEnv * env,
    jobject obj,
    jint index,
    jobject sfnode_object)
{
    mfnode * mfn = static_cast<mfnode*>(getFieldValue(env, obj));
    sfnode * sfn = static_cast<sfnode*>(getFieldValue(env, sfnode_object));
    if (!mfn || !sfn) { return; }
    mfn->value[index] = sfn->value;
}

void JNICALL
Java_vrml_field_MFNode_set1Value__ILvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject sfnode)
{
  Java_vrml_field_MFNode_set1Value__ILvrml_field_ConstSFNode_2(env, obj,
                                                               index, sfnode);
}

/**
 * @brief JNI implementation of MFNode::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFNode object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFNode_addValue__Lvrml_BaseNode_2(JNIEnv * env,
                                                               jobject obj,
                                                               jobject value)
{
    mfnode * mfn = static_cast<mfnode*>(getFieldValue(env, obj));
    if (!mfn) { return; }
    jfieldID fid = getFid(env, value, "NodePtr", "I");
    if (!fid) { return; }
    node * const newNode =
        reinterpret_cast<node *>(env->GetIntField(value, fid));
    if (!newNode) { return; }
    try {
        mfn->value.push_back(node_ptr(newNode));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFNode::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFNode object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFNode_addValue__Lvrml_field_ConstSFNode_2(JNIEnv * env,
                                                           jobject obj,
                                                           jobject value)
{
    mfnode * mfn = static_cast<mfnode*>(getFieldValue(env, obj));
    sfnode * sfn = static_cast<sfnode*>(getFieldValue(env, value));
    if (!mfn || !sfn) { return; }
    try {
        mfn->value.push_back(sfn->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFNode::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFNode object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFNode_addValue__Lvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFNode_addValue__Lvrml_field_ConstSFNode_2(env, obj, value);
}

void JNICALL
Java_vrml_field_MFNode_insertValue__ILvrml_BaseNode_2(JNIEnv * env,
                                                      jobject obj,
                                                      jint index,
                                                      jobject value)
{
    mfnode * mfn = static_cast<mfnode*>(getFieldValue(env, obj));
    if (!mfn) { return; }
    jfieldID fid = getFid(env, value, "NodePtr", "I");
    if (!fid) { return; }
    node * const newNode =
        reinterpret_cast<node *>(env->GetIntField(value, fid));
    if (!newNode) { return; }
    if (!(size_t(index) < mfn->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
    }
    try {
        mfn->value.insert(mfn->value.begin() + index, node_ptr(newNode));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFNode_insertValue__ILvrml_field_ConstSFNode_2(JNIEnv * env,
                                                               jobject obj,
                                                               jint index,
                                                               jobject value)
{
    mfnode * mfn = static_cast<mfnode*>(getFieldValue(env, obj));
    sfnode * sfn = static_cast<sfnode*>(getFieldValue(env, value));
    if (!mfn || !sfn) { return; }
    if (!(size_t(index) < mfn->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
    }
    try {
        mfn->value.insert(mfn->value.begin() + index, sfn->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFNode_insertValue__ILvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  Java_vrml_field_MFNode_insertValue__ILvrml_field_ConstSFNode_2
    (env, obj, index, value);
}

/**
 * @brief JNI implementation of MFNode::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFNode object
 * @return String representation of MFNode.
 */
jstring JNICALL Java_vrml_field_MFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL
Java_vrml_field_ConstMFRotation_CreateObject___3_3F(JNIEnv * env,
                                                    jobject obj,
                                                    jobjectArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfrotation>
            mfrotation(new openvrml::mfrotation(env->GetArrayLength(jarr)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfrotation.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }

    Java_vrml_field_MFRotation_setValue___3_3F(env, obj, jarr);
}

void JNICALL
Java_vrml_field_ConstMFRotation_CreateObject__I_3F(JNIEnv * env,
                                                   jobject obj,
                                                   jint size,
                                                   jfloatArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfrotation>
            mfrotation(new openvrml::mfrotation(size));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid,
                         reinterpret_cast<int>(mfrotation.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }

    Java_vrml_field_MFRotation_setValue__I_3F(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_ConstMFRotation_getSize(JNIEnv * env,
                                                     jobject obj)
{
    mfrotation * mfrot =
        static_cast<mfrotation *>(getFieldValue(env, obj));
    if (!mfrot) { return 0; }
    return mfrot->value.size();
}

void JNICALL Java_vrml_field_ConstMFRotation_getValue___3_3F(JNIEnv * env,
                                                             jobject obj,
                                                             jobjectArray jarr)
{
    const mfrotation * const mfrot =
            static_cast<mfrotation *>(getFieldValue(env, obj));
    for (size_t i = 0; i < mfrot->value.size(); ++i) {
        jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
        if (!element) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
        env->SetFloatArrayRegion(element, 0, 4,
                                 const_cast<jfloat *>(&mfrot->value[i][0]));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

void JNICALL Java_vrml_field_ConstMFRotation_getValue___3F(JNIEnv * env,
                                                           jobject obj,
                                                           jfloatArray jarr)
{
    const mfrotation * const mfrot =
            static_cast<mfrotation *>(getFieldValue(env, obj));
    for (size_t i = 0; i < mfrot->value.size(); ++i) {
        env->SetFloatArrayRegion(jarr, i * 4, 4,
                                 const_cast<jfloat *>(&mfrot->value[i][0]));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

void JNICALL Java_vrml_field_ConstMFRotation_get1Value__I_3F(JNIEnv * env,
                                                             jobject obj,
                                                             jint element,
                                                             jfloatArray jarr)
{
    mfrotation * mfrot = static_cast<mfrotation*>(getFieldValue(env, obj));
    env->SetFloatArrayRegion(jarr, 0, 4,
                             const_cast<jfloat*>(&mfrot->value[element][0]));
}

void JNICALL
Java_vrml_field_ConstMFRotation_get1Value__ILvrml_field_SFRotation_2(
        JNIEnv * env,
        jobject obj,
        jint element,
        jobject sfrotationObj)
{
    const mfrotation * const mfrot =
            static_cast<mfrotation *>(getFieldValue(env, obj));
    if (!mfrot) { return; }
    sfrotation * const sfrot =
            static_cast<sfrotation *>(getFieldValue(env, sfrotationObj));
    if (!sfrot) { return; }
    try {
        sfrot->value = mfrot->value[element];
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstMFRotation::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFRotation object
 * @return String representation of ConstMFRotation.
 */
jstring JNICALL Java_vrml_field_ConstMFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFRotation_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFRotation_CreateObject___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFRotation_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  Java_vrml_field_ConstMFRotation_CreateObject__I_3F(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_MFRotation_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFRotation_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFRotation_clear(JNIEnv * env, jobject obj)
{
    mfrotation * const mfrot =
        static_cast<mfrotation *>(getFieldValue(env, obj));
    if (!mfrot) { return; }
    mfrot->value.clear(); // Shouldn't throw bad_alloc.
}

void JNICALL Java_vrml_field_MFRotation_delete(JNIEnv * env,
                                               jobject obj,
                                               jint index)
{
    mfrotation * mfrot = static_cast<mfrotation*>(getFieldValue(env, obj));
    if (!mfrot) { return; }
    if (!(size_t(index) < mfrot->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mfrot->value.erase(mfrot->value.begin() + index);
}

void JNICALL Java_vrml_field_MFRotation_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFRotation_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFRotation_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstMFRotation_getValue___3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFRotation_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  Java_vrml_field_ConstMFRotation_get1Value__I_3F(env, obj, element, jarr);
}

void JNICALL
Java_vrml_field_MFRotation_get1Value__ILvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jint element, jobject sfrotation)
{
  Java_vrml_field_ConstMFRotation_get1Value__ILvrml_field_SFRotation_2
    (env, obj, element, sfrotation);
}

void JNICALL Java_vrml_field_MFRotation_setValue___3_3F(JNIEnv * env,
                                                        jobject obj,
                                                        jobjectArray value)
{
    try {
        mfrotation * const mfrot =
            static_cast<mfrotation *>(getFieldValue(env, obj));
        if (!mfrot) { return; }
        mfrot->value.resize(env->GetArrayLength(value));
        for (size_t i = 0; i < mfrot->value.size(); ++i) {
            jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(value, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return;
            }
            jfloat * const r = env->GetFloatArrayElements(element, 0);
            if (!r) {
                // Presumably we raised an OutOfMemoryError.
                return;
            }
            const rotation rot(r[0], r[1], r[2], r[3]);
            env->ReleaseFloatArrayElements(element, r, JNI_ABORT);
            mfrot->value[i] = rot;
        }
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL Java_vrml_field_MFRotation_setValue__I_3F(JNIEnv * env,
                                                       jobject obj,
                                                       jint size,
                                                       jfloatArray value)
{
    try {
        mfrotation * const mfrot =
                static_cast<mfrotation *>(getFieldValue(env, obj));
        if (!mfrot) { return; }
        mfrot->value.resize(size);
        jfloat * const r = env->GetFloatArrayElements(value, 0);
        if (!r) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (jint i = 0; i < size; ++i) {
            if (!(i * 4 + 3 < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                        env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than rotations.length");
                break;
            }
            mfrot->value[i] = rotation(r[i * 4],
                                       r[i * 4 + 1],
                                       r[i * 4 + 2],
                                       r[i * 4 + 3]);
        }
        env->ReleaseFloatArrayElements(value, r, JNI_ABORT);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFRotation_setValue__Lvrml_field_MFRotation_2(JNIEnv * env,
                                                              jobject obj,
                                                              jobject value)
{
    mfrotation * const mfrot =
            static_cast<mfrotation *>(getFieldValue(env, obj));
    const mfrotation * const newMFRotation =
            static_cast<mfrotation *>(getFieldValue(env, value));
    if (!mfrot || !newMFRotation) return;
    try {
        *mfrot = *newMFRotation; // Throws bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFRotation_setValue__Lvrml_field_ConstMFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFRotation_setValue__Lvrml_field_MFRotation_2(env, obj,
                                                                value);
}

void JNICALL
Java_vrml_field_MFRotation_set1Value__ILvrml_field_ConstSFRotation_2(
    JNIEnv * env,
    jobject obj,
    jint index,
    jobject sfrotation_object)
{
    mfrotation* mfrot = static_cast<mfrotation*>(getFieldValue(env, obj));
    sfrotation* sfrot =
        static_cast<sfrotation*>(getFieldValue(env, sfrotation_object));
    if (!mfrot || !sfrot) { return; }
    mfrot->value[index] = sfrot->value;
}

void JNICALL
Java_vrml_field_MFRotation_set1Value__ILvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotation)
{
  Java_vrml_field_MFRotation_set1Value__ILvrml_field_ConstSFRotation_2
    (env, obj, index, sfrotation);
}

void JNICALL Java_vrml_field_MFRotation_set1Value__IFFFF(JNIEnv * env,
                                                         jobject obj,
                                                         jint index,
                                                         jfloat axisX,
                                                         jfloat axisY,
                                                         jfloat axisZ,
                                                         jfloat angle)
{
    mfrotation * mfrot = static_cast<mfrotation*>(getFieldValue(env, obj));
    if (!mfrot) { return; }
    mfrot->value[index] = rotation(axisX, axisY, axisZ, angle);
}

/**
 * @brief JNI implementation of MFRotation::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFRotation object
 * @param[in] sfrotationObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFRotation_addValue__Lvrml_field_ConstSFRotation_2(
        JNIEnv * env,
        jobject obj,
        jobject sfrotation_object)
{
    mfrotation * mfrot = static_cast<mfrotation *>(getFieldValue(env, obj));
    sfrotation * sfrot =
        static_cast<sfrotation *>(getFieldValue(env, sfrotation_object));
    if (!mfrot || !sfrot) { return; }
    try {
        mfrot->value.push_back(sfrot->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFRotation::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFRotation object
 * @param[in] sfrotation Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFRotation_addValue__Lvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jobject sfrotation)
{
  Java_vrml_field_MFRotation_addValue__Lvrml_field_ConstSFRotation_2
    (env, obj, sfrotation);
}

/**
 * @brief JNI implementation of MFRotation::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFRotation object
 * @param[in] axisX X-component of value to add to end of list
 * @param[in] axisY Y-component of value to add to end of list
 * @param[in] axisZ Z-component of value to add to end of list
 * @param[in] angle Angle of value to add to end of list
 */
void JNICALL Java_vrml_field_MFRotation_addValue__FFFF(JNIEnv * env,
                                                       jobject obj,
                                                       jfloat axisX,
                                                       jfloat axisY,
                                                       jfloat axisZ,
                                                       jfloat angle)
{
    mfrotation * mfrot = static_cast<mfrotation*>(getFieldValue(env, obj));
    if (!mfrot) { return; }
    try {
        mfrot->value.push_back(rotation(axisX, axisY, axisZ, angle));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFRotation_insertValue__ILvrml_field_ConstSFRotation_2(
    JNIEnv *env,
    jobject obj,
    jint index,
    jobject sfrotation_object)
{
    sfrotation * sfrot =
        static_cast<sfrotation*>(getFieldValue(env, sfrotation_object));
    if (!sfrot) { return; }
    Java_vrml_field_MFRotation_insertValue__IFFFF(env, obj, index,
                                                  sfrot->value.x(),
                                                  sfrot->value.y(),
                                                  sfrot->value.z(),
                                                  sfrot->value.angle());
}

void JNICALL
Java_vrml_field_MFRotation_insertValue__ILvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotation)
{
  Java_vrml_field_MFRotation_insertValue__ILvrml_field_ConstSFRotation_2
    (env, obj, index, sfrotation);
}

void JNICALL Java_vrml_field_MFRotation_insertValue__IFFFF(JNIEnv * env,
                                                           jobject obj,
                                                           jint index,
                                                           jfloat axisX,
                                                           jfloat axisY,
                                                           jfloat axisZ,
                                                           jfloat angle)
{
    mfrotation * mfrot = static_cast<mfrotation*>(getFieldValue(env, obj));
    if (!mfrot) { return; }
    if (!(size_t(index) < mfrot->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
    }
    try {
        mfrot->value.insert(mfrot->value.begin() + index,
                            rotation(axisX, axisY, axisZ, angle));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFRotation::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFRotation object
 * @return String representation of MFRotation.
 */
jstring JNICALL Java_vrml_field_MFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstMFString_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfstring>
            mfstring(new openvrml::mfstring(size));

        for (jint i = 0; i < size; i++) {
            jstring el = (jstring) env->GetObjectArrayElement(jarr, i);
            const char * temp = env->GetStringUTFChars(el, 0);
            if (!temp) { return; } // OutOfMemoryError
            mfstring->value[i] = temp;
            env->ReleaseStringUTFChars(el, temp);
        }

        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfstring.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

jint JNICALL Java_vrml_field_ConstMFString_getSize(JNIEnv * env, jobject obj)
{
    mfstring * mfstr = static_cast<mfstring *>(getFieldValue(env, obj));
    if (!mfstr) { return 0; }
    return mfstr->value.size();
}

void JNICALL Java_vrml_field_ConstMFString_getValue(JNIEnv * env,
                                                    jobject obj,
                                                    jobjectArray jarr)
{
    mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
    if (!mfstr) return;
    for (size_t i = 0; i < mfstr->value.size(); i++) {
        jstring jstr;
        const char* str = (mfstr->value[i]).c_str();
        if (str) {
            jstr = env->NewStringUTF(str);
            env->SetObjectArrayElement(jarr, i, jstr);
        }
    }
}

jstring JNICALL Java_vrml_field_ConstMFString_get1Value(JNIEnv * env,
                                                        jobject obj,
                                                        jint index)
{
    mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
    if (!mfstr) { return 0; }
    jstring jstr = env->NewStringUTF(mfstr->value[index].c_str());
    return jstr;
}

/**
 * @brief JNI implementation of ConstMFString::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFString object
 * @return String representation of ConstMFString.
 */
jstring JNICALL Java_vrml_field_ConstMFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFString_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
  Java_vrml_field_ConstMFString_CreateObject(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_MFString_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFString_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFString_clear(JNIEnv * env, jobject obj)
{
    mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
    if (!mfstr) { return; }
    mfstr->value.clear();
}

void JNICALL Java_vrml_field_MFString_delete(JNIEnv * env,
                                             jobject obj,
                                             jint index)
{
    mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
    if (!mfstr) { return; }
    if (!(size_t(index) < mfstr->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mfstr->value.erase(mfstr->value.begin() + index);
}

void JNICALL Java_vrml_field_MFString_getValue
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFString_getValue(env, obj, jarr);
}

jstring JNICALL Java_vrml_field_MFString_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFString_get1Value(env, obj, index);
}

void JNICALL
Java_vrml_field_MFString_setValue__I_3Ljava_lang_String_2(JNIEnv * env,
                                                          jobject obj,
                                                          jint,
                                                          jobjectArray jarr)
{
    try {
        mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
        if (!mfstr) { return; }

        jsize entries = env->GetArrayLength(jarr);
        mfstr->value.resize(entries);  // Throws std::bad_alloc.

        for (jsize pos = 0; pos < entries; pos++) {
            jstring jstr =
                static_cast<jstring>(env->GetObjectArrayElement(jarr, pos));
            const char *str = env->GetStringUTFChars(jstr, 0);
            if (!str) { return; } // OutOfMemoryError
            mfstr->value[pos] = str;
            env->ReleaseStringUTFChars(jstr, str);
        }
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2(JNIEnv * env,
                                                          jobject obj,
                                                          jobject value)
{
    mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
    mfstring * newMFString = static_cast<mfstring*>(getFieldValue(env, value));
    if (!mfstr || !newMFString) return;

    try {
        *mfstr = *newMFString; // Throws std::bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFString_setValue__Lvrml_field_ConstMFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2(env, obj, value);
}

void JNICALL
Java_vrml_field_MFString_set1Value__ILjava_lang_String_2(JNIEnv * env,
                                                         jobject obj,
                                                         jint index,
                                                         jstring value)
{
    mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
    if (!mfstr) { return; }
    const char * utf8chars = env->GetStringUTFChars(value, 0);
    if (!utf8chars) { return; } // OutOfMemoryError
    mfstr->value[index] = utf8chars;
    env->ReleaseStringUTFChars(value, utf8chars);
}

void JNICALL
Java_vrml_field_MFString_set1Value__ILvrml_field_ConstSFString_2(
    JNIEnv * env,
    jobject obj,
    jint index,
    jobject sfstring_object)
{
    mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
    sfstring * sfstr =
        static_cast<sfstring*>(getFieldValue(env, sfstring_object));
    if (!mfstr || !sfstr) { return; }
    mfstr->value[index] = sfstr->value;
}

void JNICALL
Java_vrml_field_MFString_set1Value__ILvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  Java_vrml_field_MFString_set1Value__ILvrml_field_ConstSFString_2
    (env, obj, index, sfstring);
}

/**
 * @brief JNI implementation of MFString::addValue.
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
    mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
    if (!mfstr) { return; }
    const char * utf8chars = env->GetStringUTFChars(value, 0);
    if (!utf8chars) { return; } // OutOfMemoryError
    try {
        mfstr->value.push_back(utf8chars);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            env->ReleaseStringUTFChars(value, utf8chars);
            return;
        }
    }
    env->ReleaseStringUTFChars(value, utf8chars);
}

/**
 * @brief JNI implementation of MFString::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFString object
 * @param[in] sfstringObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFString_addValue__Lvrml_field_ConstSFString_2(
    JNIEnv * env,
    jobject obj,
    jobject sfstring_object)
{
    mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
    sfstring * sfstr =
        static_cast<sfstring*>(getFieldValue(env, sfstring_object));
    if (!mfstr || !sfstr) { return; }
    try {
        mfstr->value.push_back(sfstr->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFString::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFString object
 * @param[in] sfstring Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFString_addValue__Lvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jobject sfstring)
{
  Java_vrml_field_MFString_addValue__Lvrml_field_ConstSFString_2
    (env, obj, sfstring);
}

/**
 * @brief JNI implementation of MFString::insertValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFString object
 * @param[in] index Position to insert value at
 * @param[in] value Value to insert.
 */
void JNICALL
Java_vrml_field_MFString_insertValue__ILjava_lang_String_2(JNIEnv * env,
                                                           jobject obj,
                                                           jint index,
                                                           jstring value)
{
    mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
    if (!mfstr) { return; }
    const char * utf8chars = env->GetStringUTFChars(value, 0);
    if (!utf8chars) { return; } // OutOfMemoryError
    try {
        mfstr->value.insert(mfstr->value.begin() + index, utf8chars);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            env->ReleaseStringUTFChars(value, utf8chars);
            return;
        }
    }
    env->ReleaseStringUTFChars(value, utf8chars);
}

/**
 * @brief JNI implementation of MFString::insertValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFString object
 * @param[in] index Position to insert value at
 * @param[in] sfstringObj Value to insert.
 */
void JNICALL
Java_vrml_field_MFString_insertValue__ILvrml_field_ConstSFString_2(
    JNIEnv * env,
    jobject obj,
    jint index,
    jobject sfstring_object)
{
    mfstring * mfstr = static_cast<mfstring*>(getFieldValue(env, obj));
    sfstring * sfstr =
        static_cast<sfstring*>(getFieldValue(env, sfstring_object));
    if (!mfstr || !sfstr) { return; }
    try {
        mfstr->value.insert(mfstr->value.begin() + index, sfstr->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFString::insertValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFString object
 * @param[in] index Position to insert value at
 * @param[in] sfstring Value to insert.
 */
void JNICALL
Java_vrml_field_MFString_insertValue__ILvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  Java_vrml_field_MFString_insertValue__ILvrml_field_ConstSFString_2
    (env, obj, index, sfstring);
}

/**
 * @brief JNI implementation of MFString::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFString object
 * @return String representation of MFString.
 */
jstring JNICALL Java_vrml_field_MFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstMFTime_CreateObject
  (JNIEnv *env, jobject obj, jint size, jdoubleArray value)
{
    try {
        jdouble *pjd = env->GetDoubleArrayElements(value, 0);
        std::auto_ptr<openvrml::mftime>
            mftime(new openvrml::mftime(pjd, pjd + size));
        env->ReleaseDoubleArrayElements(value, pjd, JNI_ABORT);
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) { return; }
        env->SetIntField(obj, fid, reinterpret_cast<int>(mftime.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

jint JNICALL Java_vrml_field_ConstMFTime_getSize(JNIEnv * env, jobject obj)
{
    mftime * mft = static_cast<mftime *>(getFieldValue(env, obj));
    if (!mft) { return 0; }
    return mft->value.size();
}

void JNICALL Java_vrml_field_ConstMFTime_getValue(JNIEnv * env,
                                                  jobject obj,
                                                  jdoubleArray jarr)
{
    mftime * const mft = static_cast<mftime*>(getFieldValue(env, obj));
    if (!mft) { return; }
    const size_t size = mft->value.size();
    if (size > 0) {
        env->SetDoubleArrayRegion(jarr, 0, size,
                                  const_cast<jdouble *>(&mft->value[0]));
    }
}

jdouble JNICALL Java_vrml_field_ConstMFTime_get1Value(JNIEnv * env,
                                                      jobject obj,
                                                      jint index)
{
    mftime * mft = static_cast<mftime *>(getFieldValue(env, obj));
    if (!mft) { return 0.0; }
    jdouble retval;
    try {
        retval = mft->value.at(index);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return retval;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
    return retval;
}

/**
 * @brief JNI implementation of ConstMFTime::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFTime object
 * @return String representation of ConstMFTime.
 */
jstring JNICALL Java_vrml_field_ConstMFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFTime_CreateObject
  (JNIEnv *env, jobject obj, jint size, jdoubleArray value)
{
  Java_vrml_field_ConstMFTime_CreateObject(env, obj, size, value);
}

jint JNICALL Java_vrml_field_MFTime_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFTime_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFTime_clear(JNIEnv * env, jobject obj)
{
    mftime * const mft = static_cast<mftime *>(getFieldValue(env, obj));
    if (!mft) { return; }
    mft->value.clear();
}

void JNICALL Java_vrml_field_MFTime_delete(JNIEnv * env,
                                           jobject obj,
                                           jint index)
{
    mftime * mft = static_cast<mftime*>(getFieldValue(env, obj));
    if (!mft) { return; }
    if (!(size_t(index) < mft->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mft->value.erase(mft->value.begin() + index);
}

void JNICALL Java_vrml_field_MFTime_getValue
  (JNIEnv *env, jobject obj, jdoubleArray jarr)
{
  Java_vrml_field_ConstMFTime_getValue(env, obj, jarr);
}

jdouble JNICALL Java_vrml_field_MFTime_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFTime_get1Value(env, obj, index);
}

void JNICALL Java_vrml_field_MFTime_setValue__I_3D(JNIEnv * const env,
                                                   const jobject obj,
                                                   const jint size,
                                                   const jdoubleArray value)
{
    try {
        mftime * const mft = static_cast<mftime *>(getFieldValue(env, obj));
        if (!mft) { return; }
        mft->value.resize(size); // Throws std::bad_alloc.
        jdouble * const times = env->GetDoubleArrayElements(value, 0);
        if (!times) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (jint i = 0; i < size; ++i) {
            if (!(i < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                    env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than "
                                              "values.length");
                break;
            }
            mft->value[i] = times[i];
        }
        env->ReleaseDoubleArrayElements(value, times, JNI_ABORT);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFTime_setValue__Lvrml_field_MFTime_2(JNIEnv * env,
                                                      jobject obj,
                                                      jobject value)
{
    mftime * const mft = static_cast<mftime *>(getFieldValue(env, obj));
    const mftime * const newMFTime =
            static_cast<mftime *>(getFieldValue(env, value));
    if (!mft || !newMFTime) return;
    try {
        *mft = *newMFTime; // Throws bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFTime_setValue__Lvrml_field_ConstMFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFTime_setValue__Lvrml_field_MFTime_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFTime_set1Value__ID(JNIEnv * env,
                                                  jobject obj,
                                                  jint index,
                                                  jdouble value)
{
    mftime * mft = static_cast<mftime*>(getFieldValue(env, obj));
    if (!mft) { return; }
    try {
        mft->value.at(index) = value;
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFTime_set1Value__ILvrml_field_ConstSFTime_2(
    JNIEnv * env,
    jobject obj,
    jint index,
    jobject sftime_object)
{
    mftime * mft = static_cast<mftime*>(getFieldValue(env, obj));
    sftime * sft = static_cast<sftime*>(getFieldValue(env, sftime_object));
    if (!mft || !sft) { return; }
    try {
        mft->value.at(index) = sft->value;
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFTime_set1Value__ILvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  Java_vrml_field_MFTime_set1Value__ILvrml_field_ConstSFTime_2
    (env, obj, index, sftime);
}

/**
 * @brief JNI implementation of MFTime::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFTime object
 * @param[in] value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFTime_addValue__D(JNIEnv * env,
                                                jobject obj,
                                                jdouble value)
{
    mftime * mft = static_cast<mftime *>(getFieldValue(env, obj));
    if (!mft) { return; }
    try {
        mft->value.push_back(value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFTime::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFTime object
 * @param[in] sftimeObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFTime_addValue__Lvrml_field_ConstSFTime_2(
    JNIEnv * env,
    jobject obj,
    jobject sftime_object)
{
    mftime * mft = static_cast<mftime*>(getFieldValue(env, obj));
    sftime * sft = static_cast<sftime*>(getFieldValue(env, sftime_object));
    if (!mft || !sft) { return; }
    try {
        mft->value.push_back(sft->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFTime::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFTime object
 * @param[in] sftime Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFTime_addValue__Lvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jobject sftime)
{
  Java_vrml_field_MFTime_addValue__Lvrml_field_ConstSFTime_2
    (env, obj, sftime);
}

void JNICALL Java_vrml_field_MFTime_insertValue__ID(JNIEnv * env,
                                                    jobject obj,
                                                    jint index,
                                                    jdouble value)
{
    mftime * mft = static_cast<mftime*>(getFieldValue(env, obj));
    if (!mft) { return; }
    try {
        mft->value.insert(mft->value.begin() + index, value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFTime_insertValue__ILvrml_field_ConstSFTime_2(
    JNIEnv * env,
    jobject obj,
    jint index,
    jobject sftime_object)
{
    mftime * mft = static_cast<mftime*>(getFieldValue(env, obj));
    sftime * sft = static_cast<sftime*>(getFieldValue(env, sftime_object));
    if (!mft || !sft) { return; }
    try {
        mft->value.insert(mft->value.begin() + index, sft->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFTime_insertValue__ILvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  Java_vrml_field_MFTime_insertValue__ILvrml_field_ConstSFTime_2
    (env, obj, index, sftime);
}

/**
 * @brief JNI implementation of MFTime::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFTime object
 * @return String representation of MFTime.
 */
jstring JNICALL Java_vrml_field_MFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstMFVec2f_CreateObject___3_3F(JNIEnv * env,
                                                              jobject obj,
                                                              jobjectArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfvec2f>
            mfvec2f(new openvrml::mfvec2f(env->GetArrayLength(jarr)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfvec2f.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
    Java_vrml_field_MFVec2f_setValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_ConstMFVec2f_CreateObject__I_3F(JNIEnv * env,
                                                             jobject obj,
                                                             jint size,
                                                             jfloatArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfvec2f> mfvec2f(new openvrml::mfvec2f(size));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfvec2f.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
    Java_vrml_field_MFVec2f_setValue__I_3F(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_ConstMFVec2f_getSize(JNIEnv * env, jobject obj)
{
    mfvec2f * mfv = static_cast<mfvec2f *>(getFieldValue(env, obj));
    if (!mfv) { return 0; }
    return mfv->value.size();
}

void JNICALL Java_vrml_field_ConstMFVec2f_getValue___3_3F(JNIEnv * env,
                                                          jobject obj,
                                                          jobjectArray jarr)
{
    const mfvec2f * const mfv =
            static_cast<mfvec2f *>(getFieldValue(env, obj));
    if (!mfv) return;
    for (size_t i = 0; i < mfv->value.size(); ++i) {
        jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
        if (!element) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
        env->SetFloatArrayRegion(element, 0, 2,
                                 const_cast<jfloat *>(&mfv->value[i][0]));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

void JNICALL Java_vrml_field_ConstMFVec2f_getValue___3F(JNIEnv * env,
                                                        jobject obj,
                                                        jfloatArray jarr)
{
    const mfvec2f * const mfv =
            static_cast<mfvec2f *>(getFieldValue(env, obj));
    if (!mfv) return;
    for (size_t i = 0; i < mfv->value.size(); ++i) {
        env->SetFloatArrayRegion(jarr, i * 2, 2,
                                 const_cast<jfloat *>(&mfv->value[i][0]));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

/**
 * @brief JNI implementation of ConstMFVec2f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec2f object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y value
 */
void JNICALL Java_vrml_field_ConstMFVec2f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
    mfvec2f* mfv = static_cast<mfvec2f*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        jfloat * const arrayRegion =
                const_cast<jfloat*>(&mfv->value.at(element)[0]);
        env->SetFloatArrayRegion(jarr, 0, 2, arrayRegion);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstMFVec2f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec2f object
 * @param[in] element Position of object to retrieve
 * @param[out] sfvec2fObj SFVec2f to store retrieved object in
 */
void JNICALL
Java_vrml_field_ConstMFVec2f_get1Value__ILvrml_field_SFVec2f_2(
        JNIEnv * env,
        jobject obj,
        jint element,
        jobject sfvec2f_object)
{
    const mfvec2f * const mfv =
            static_cast<mfvec2f *>(getFieldValue(env, obj));
    if (!mfv) return;
    sfvec2f * const sfv =
        static_cast<sfvec2f *>(getFieldValue(env, sfvec2f_object));
    if (!sfv) { return; }
    try {
        sfv->value = mfv->value.at(element);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstMFVec2f::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec2f object
 * @return String representation of ConstMFVec2f.
 */
jstring JNICALL Java_vrml_field_ConstMFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFVec2f_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec2f_CreateObject___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec2f_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec2f_CreateObject__I_3F(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_MFVec2f_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFVec2f_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFVec2f_clear(JNIEnv * env, jobject obj)
{
    mfvec2f * const mfv = static_cast<mfvec2f *>(getFieldValue(env, obj));
    if (!mfv) return;
    mfv->value.clear();
}

void JNICALL Java_vrml_field_MFVec2f_delete
  (JNIEnv *env, jobject obj, jint n)
{
    mfvec2f* mfv = static_cast<mfvec2f*>(getFieldValue(env, obj));
    if (!mfv) return;
    if (!(size_t(n) < mfv->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mfv->value.erase(mfv->value.begin() + n);
}

void JNICALL Java_vrml_field_MFVec2f_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec2f_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec2f_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec2f_getValue___3F(env, obj, jarr);
}

/**
 * @brief JNI implementation of MFVec2f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec2f object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y value
 */
void JNICALL Java_vrml_field_MFVec2f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec2f_get1Value__I_3F(env, obj, element, jarr);
}

/**
 * @brief JNI implementation of MFVec2f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec2f object
 * @param[in] element Position of object to retrieve
 * @param[out] sfvec2f SFVec2f to store retrieved object in
 */
void JNICALL
Java_vrml_field_MFVec2f_get1Value__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec2f)
{
  Java_vrml_field_ConstMFVec2f_get1Value__ILvrml_field_SFVec2f_2
    (env, obj, element, sfvec2f);
}

void JNICALL Java_vrml_field_MFVec2f_setValue___3_3F(JNIEnv * env,
                                                     jobject obj,
                                                     jobjectArray value)
{
    try {
        mfvec2f * const mfv =
                static_cast<mfvec2f *>(getFieldValue(env, obj));
        if (!mfv) return;
        mfv->value.resize(env->GetArrayLength(value)); // Throws bad_alloc.

        for (size_t i = 0; i < mfv->value.size(); ++i) {
            jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(value, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return;
            }
            jfloat * const v = env->GetFloatArrayElements(element, 0);
            if (!v) {
                // Presumably we raised an OutOfMemoryError.
                return;
            }
            const vec2f vec(v[0], v[1]);
            env->ReleaseFloatArrayElements(element, v, JNI_ABORT);
            mfv->value[i] = vec;
        }
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL Java_vrml_field_MFVec2f_setValue__I_3F(JNIEnv * env,
                                                    jobject obj,
                                                    jint size,
                                                    jfloatArray value)
{
    try {
        mfvec2f * const mfv =
                static_cast<mfvec2f *>(getFieldValue(env, obj));
        if (!mfv) return;
        mfv->value.resize(size); // throws bad_alloc
        jfloat * const vecs = env->GetFloatArrayElements(value, 0);
        if (!vecs) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (jint i = 0; i < size; ++i) {
            if (!(i * 2 + 1 < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                    env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than vec2s.length");
                break;
            }
            const vec2f vec(vecs[2 * i], vecs[2 * i + 1]);
            mfv->value[i] = vec;
        }
        env->ReleaseFloatArrayElements(value, vecs, JNI_ABORT);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2f_setValue__Lvrml_field_MFVec2f_2(JNIEnv * env,
                                                        jobject obj,
                                                        jobject value)
{
    mfvec2f * const mfv = static_cast<mfvec2f *>(getFieldValue(env, obj));
    const mfvec2f * const newMFVec2f =
            static_cast<mfvec2f *>(getFieldValue(env, value));
    if (!mfv || !newMFVec2f) return;
    try {
        *mfv = *newMFVec2f; // Throws bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2f_setValue__Lvrml_field_ConstMFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFVec2f_setValue__Lvrml_field_MFVec2f_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFVec2f_set1Value__IFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y)
{
    mfvec2f* mfv = static_cast<mfvec2f*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        mfv->value.at(index) = vec2f(x, y);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2f_set1Value__ILvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2fObj)
{
    mfvec2f* mfv = static_cast<mfvec2f*>(getFieldValue(env, obj));
    sfvec2f* sfv = static_cast<sfvec2f*>(getFieldValue(env, sfvec2fObj));
    if (!mfv || !sfv) return;
    try {
        mfv->value.at(index) = sfv->value;
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2f_set1Value__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  Java_vrml_field_MFVec2f_set1Value__ILvrml_field_ConstSFVec2f_2
    (env, obj, index, sfvec2f);
}

/**
 * @brief JNI implementation of MFVec2f::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec2f object
 * @param[in] x X-component of vector to add to end of list
 * @param[in] y Y-component of vector to add to end of list
 */
void JNICALL Java_vrml_field_MFVec2f_addValue__FF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
    mfvec2f* mfv = static_cast<mfvec2f*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        mfv->value.push_back(vec2f(x, y));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFVec2f::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec2f object
 * @param[in] sfvec2fObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFVec2f_addValue__Lvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jobject sfvec2fObj)
{
    mfvec2f* mfv = static_cast<mfvec2f*>(getFieldValue(env, obj));
    sfvec2f* sfv = static_cast<sfvec2f*>(getFieldValue(env, sfvec2fObj));
    if (!mfv || !sfv) return;
    try {
        mfv->value.push_back(sfv->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFVec2f::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec2f object
 * @param[in] sfvec2f Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFVec2f_addValue__Lvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jobject sfvec2f)
{
  Java_vrml_field_MFVec2f_addValue__Lvrml_field_ConstSFVec2f_2
    (env, obj, sfvec2f);
}

void JNICALL Java_vrml_field_MFVec2f_insertValue__IFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y)
{
    mfvec2f* mfv = static_cast<mfvec2f*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        mfv->value.insert(mfv->value.begin() + index, vec2f(x, y));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2f_insertValue__ILvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2fObj)
{
    mfvec2f* mfv = static_cast<mfvec2f*>(getFieldValue(env, obj));
    sfvec2f* sfv = static_cast<sfvec2f*>(getFieldValue(env, sfvec2fObj));
    if (!mfv || !sfv) return;
    try {
        mfv->value.insert(mfv->value.begin() + index, sfv->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2f_insertValue__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  Java_vrml_field_MFVec2f_insertValue__ILvrml_field_ConstSFVec2f_2
    (env, obj, index, sfvec2f);
}

void JNICALL Java_vrml_field_ConstMFVec2d_CreateObject___3_3F(JNIEnv * env,
                                                              jobject obj,
                                                              jobjectArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfvec2d>
            mfvec2d(new openvrml::mfvec2d(env->GetArrayLength(jarr)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfvec2d.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
    Java_vrml_field_MFVec2d_setValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_ConstMFVec2d_CreateObject__I_3F(JNIEnv * env,
                                                             jobject obj,
                                                             jint size,
                                                             jdoubleArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfvec2d> mfvec2d(new openvrml::mfvec2d(size));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfvec2d.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
    Java_vrml_field_MFVec2d_setValue__I_3F(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_ConstMFVec2d_getSize(JNIEnv * env, jobject obj)
{
    mfvec2d * mfv = static_cast<mfvec2d *>(getFieldValue(env, obj));
    if (!mfv) { return 0; }
    return mfv->value.size();
}

void JNICALL Java_vrml_field_ConstMFVec2d_getValue___3_3F(JNIEnv * env,
                                                          jobject obj,
                                                          jobjectArray jarr)
{
    const mfvec2d * const mfv =
            static_cast<mfvec2d *>(getFieldValue(env, obj));
    if (!mfv) return;
    for (size_t i = 0; i < mfv->value.size(); ++i) {
        jdoubleArray element =
                static_cast<jdoubleArray>(env->GetObjectArrayElement(jarr, i));
        if (!element) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
        env->SetFloatArrayRegion(element, 0, 2,
                                 const_cast<jdouble *>(&mfv->value[i][0]));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

void JNICALL Java_vrml_field_ConstMFVec2d_getValue___3F(JNIEnv * env,
                                                        jobject obj,
                                                        jdoubleArray jarr)
{
    const mfvec2d * const mfv =
            static_cast<mfvec2d *>(getFieldValue(env, obj));
    if (!mfv) return;
    for (size_t i = 0; i < mfv->value.size(); ++i) {
        env->SetFloatArrayRegion(jarr, i * 2, 2,
                                 const_cast<jdouble *>(&mfv->value[i][0]));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

/**
 * @brief JNI implementation of ConstMFVec2d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec2d object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y value
 */
void JNICALL Java_vrml_field_ConstMFVec2d_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jdoubleArray jarr)
{
    mfvec2d* mfv = static_cast<mfvec2d*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        jdouble * const arrayRegion =
                const_cast<jdouble*>(&mfv->value.at(element)[0]);
        env->SetFloatArrayRegion(jarr, 0, 2, arrayRegion);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstMFVec2d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec2d object
 * @param[in] element Position of object to retrieve
 * @param[out] sfvec2dObj SFVec2d to store retrieved object in
 */
void JNICALL
Java_vrml_field_ConstMFVec2d_get1Value__ILvrml_field_SFVec2d_2(
        JNIEnv * env,
        jobject obj,
        jint element,
        jobject sfvec2d_object)
{
    const mfvec2d * const mfv =
            static_cast<mfvec2d *>(getFieldValue(env, obj));
    if (!mfv) return;
    sfvec2d * const sfv =
        static_cast<sfvec2d *>(getFieldValue(env, sfvec2d_object));
    if (!sfv) { return; }
    try {
        sfv->value = mfv->value.at(element);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstMFVec2d::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec2d object
 * @return String representation of ConstMFVec2d.
 */
jstring JNICALL Java_vrml_field_ConstMFVec2d_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFVec2d_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec2d_CreateObject___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec2d_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jdoubleArray jarr)
{
  Java_vrml_field_ConstMFVec2d_CreateObject__I_3F(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_MFVec2d_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFVec2d_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFVec2d_clear(JNIEnv * env, jobject obj)
{
    mfvec2d * const mfv = static_cast<mfvec2d *>(getFieldValue(env, obj));
    if (!mfv) return;
    mfv->value.clear();
}

void JNICALL Java_vrml_field_MFVec2d_delete
  (JNIEnv *env, jobject obj, jint n)
{
    mfvec2d* mfv = static_cast<mfvec2d*>(getFieldValue(env, obj));
    if (!mfv) return;
    if (!(size_t(n) < mfv->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mfv->value.erase(mfv->value.begin() + n);
}

void JNICALL Java_vrml_field_MFVec2d_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec2d_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec2d_getValue___3F
  (JNIEnv *env, jobject obj, jdoubleArray jarr)
{
  Java_vrml_field_ConstMFVec2d_getValue___3F(env, obj, jarr);
}

/**
 * @brief JNI implementation of MFVec2d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec2d object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y value
 */
void JNICALL Java_vrml_field_MFVec2d_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jdoubleArray jarr)
{
  Java_vrml_field_ConstMFVec2d_get1Value__I_3F(env, obj, element, jarr);
}

/**
 * @brief JNI implementation of MFVec2d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec2d object
 * @param[in] element Position of object to retrieve
 * @param[out] sfvec2d SFVec2d to store retrieved object in
 */
void JNICALL
Java_vrml_field_MFVec2d_get1Value__ILvrml_field_SFVec2d_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec2d)
{
  Java_vrml_field_ConstMFVec2d_get1Value__ILvrml_field_SFVec2d_2
    (env, obj, element, sfvec2d);
}

void JNICALL Java_vrml_field_MFVec2d_setValue___3_3F(JNIEnv * env,
                                                     jobject obj,
                                                     jobjectArray value)
{
    try {
        mfvec2d * const mfv =
                static_cast<mfvec2d *>(getFieldValue(env, obj));
        if (!mfv) return;
        mfv->value.resize(env->GetArrayLength(value)); // Throws bad_alloc.

        for (size_t i = 0; i < mfv->value.size(); ++i) {
            jdoubleArray element =
                static_cast<jdoubleArray>(env->GetObjectArrayElement(value, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return;
            }
            jdouble * const v = env->GetFloatArrayElements(element, 0);
            if (!v) {
                // Presumably we raised an OutOfMemoryError.
                return;
            }
            const vec2d vec(v[0], v[1]);
            env->ReleaseFloatArrayElements(element, v, JNI_ABORT);
            mfv->value[i] = vec;
        }
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL Java_vrml_field_MFVec2d_setValue__I_3F(JNIEnv * env,
                                                    jobject obj,
                                                    jint size,
                                                    jdoubleArray value)
{
    try {
        mfvec2d * const mfv =
                static_cast<mfvec2d *>(getFieldValue(env, obj));
        if (!mfv) return;
        mfv->value.resize(size); // throws bad_alloc
        jdouble * const vecs = env->GetFloatArrayElements(value, 0);
        if (!vecs) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (jint i = 0; i < size; ++i) {
            if (!(i * 2 + 1 < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                    env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than vec2s.length");
                break;
            }
            const vec2d vec(vecs[2 * i], vecs[2 * i + 1]);
            mfv->value[i] = vec;
        }
        env->ReleaseFloatArrayElements(value, vecs, JNI_ABORT);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2d_setValue__Lvrml_field_MFVec2d_2(JNIEnv * env,
                                                        jobject obj,
                                                        jobject value)
{
    mfvec2d * const mfv = static_cast<mfvec2d *>(getFieldValue(env, obj));
    const mfvec2d * const newMFVec2d =
            static_cast<mfvec2d *>(getFieldValue(env, value));
    if (!mfv || !newMFVec2d) return;
    try {
        *mfv = *newMFVec2d; // Throws bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2d_setValue__Lvrml_field_ConstMFVec2d_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFVec2d_setValue__Lvrml_field_MFVec2d_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFVec2d_set1Value__IFF
  (JNIEnv *env, jobject obj, jint index, jdouble x, jdouble y)
{
    mfvec2d* mfv = static_cast<mfvec2d*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        mfv->value.at(index) = vec2d(x, y);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2d_set1Value__ILvrml_field_ConstSFVec2d_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2dObj)
{
    mfvec2d* mfv = static_cast<mfvec2d*>(getFieldValue(env, obj));
    sfvec2d* sfv = static_cast<sfvec2d*>(getFieldValue(env, sfvec2dObj));
    if (!mfv || !sfv) return;
    try {
        mfv->value.at(index) = sfv->value;
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2d_set1Value__ILvrml_field_SFVec2d_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2d)
{
  Java_vrml_field_MFVec2d_set1Value__ILvrml_field_ConstSFVec2d_2
    (env, obj, index, sfvec2d);
}

/**
 * @brief JNI implementation of MFVec2d::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec2d object
 * @param[in] x X-component of vector to add to end of list
 * @param[in] y Y-component of vector to add to end of list
 */
void JNICALL Java_vrml_field_MFVec2d_addValue__FF
  (JNIEnv *env, jobject obj, jdouble x, jdouble y)
{
    mfvec2d* mfv = static_cast<mfvec2d*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        mfv->value.push_back(vec2d(x, y));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFVec2d::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec2d object
 * @param[in] sfvec2dObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFVec2d_addValue__Lvrml_field_ConstSFVec2d_2
  (JNIEnv *env, jobject obj, jobject sfvec2dObj)
{
    mfvec2d* mfv = static_cast<mfvec2d*>(getFieldValue(env, obj));
    sfvec2d* sfv = static_cast<sfvec2d*>(getFieldValue(env, sfvec2dObj));
    if (!mfv || !sfv) return;
    try {
        mfv->value.push_back(sfv->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFVec2d::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec2d object
 * @param[in] sfvec2d Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFVec2d_addValue__Lvrml_field_SFVec2d_2
  (JNIEnv *env, jobject obj, jobject sfvec2d)
{
  Java_vrml_field_MFVec2d_addValue__Lvrml_field_ConstSFVec2d_2
    (env, obj, sfvec2d);
}

void JNICALL Java_vrml_field_MFVec2d_insertValue__IFF
  (JNIEnv *env, jobject obj, jint index, jdouble x, jdouble y)
{
    mfvec2d* mfv = static_cast<mfvec2d*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        mfv->value.insert(mfv->value.begin() + index, vec2d(x, y));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2d_insertValue__ILvrml_field_ConstSFVec2d_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2dObj)
{
    mfvec2d* mfv = static_cast<mfvec2d*>(getFieldValue(env, obj));
    sfvec2d* sfv = static_cast<sfvec2d*>(getFieldValue(env, sfvec2dObj));
    if (!mfv || !sfv) return;
    try {
        mfv->value.insert(mfv->value.begin() + index, sfv->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec2d_insertValue__ILvrml_field_SFVec2d_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2d)
{
  Java_vrml_field_MFVec2d_insertValue__ILvrml_field_ConstSFVec2d_2
    (env, obj, index, sfvec2d);
}

/**
 * @brief JNI implementation of MFVec2f::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec2f object
 * @return String representation of MFVec2f.
 */
jstring JNICALL Java_vrml_field_MFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstMFVec3f_CreateObject___3_3F(JNIEnv * env,
                                                              jobject obj,
                                                              jobjectArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfvec3f>
            mfvec3f(new openvrml::mfvec3f(env->GetArrayLength(jarr)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfvec3f.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
    Java_vrml_field_MFVec3f_setValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_ConstMFVec3f_CreateObject__I_3F(JNIEnv * env,
                                                             jobject obj,
                                                             jint size,
                                                             jfloatArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfvec3f> mfvec3f(new openvrml::mfvec3f(size));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfvec3f.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
    Java_vrml_field_MFVec3f_setValue__I_3F(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_ConstMFVec3f_getSize(JNIEnv * env, jobject obj)
{
    mfvec3f * mfv = static_cast<mfvec3f *>(getFieldValue(env, obj));
    if (!mfv) return 0;
    return mfv->value.size();
}

void JNICALL Java_vrml_field_ConstMFVec3f_getValue___3_3F(JNIEnv * env,
                                                          jobject obj,
                                                          jobjectArray jarr)
{
    const mfvec3f * const mfv =
            static_cast<mfvec3f *>(getFieldValue(env, obj));
    if (!mfv) return;
    for (size_t i = 0; i < mfv->value.size(); ++i) {
        jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
        if (!element) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
        env->SetFloatArrayRegion(element, 0, 3,
                                 const_cast<jfloat *>(&mfv->value[i][0]));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

void JNICALL Java_vrml_field_ConstMFVec3f_getValue___3F(JNIEnv * env,
                                                        jobject obj,
                                                        jfloatArray jarr)
{
    const mfvec3f * const mfv =
            static_cast<mfvec3f *>(getFieldValue(env, obj));
    if (!mfv) return;
    for (size_t i = 0; i < mfv->value.size(); ++i) {
        env->SetFloatArrayRegion(jarr, i * 3, 3,
                                 const_cast<jfloat *>(&mfv->value[i][0]));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

/**
 * @brief JNI implementation of ConstMFVec3f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec3f object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y,z value
 */
void JNICALL Java_vrml_field_ConstMFVec3f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
    mfvec3f* mfv = static_cast<mfvec3f*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        jfloat * const arrayRegion =
                const_cast<jfloat*>(&mfv->value.at(element)[0]);
        env->SetFloatArrayRegion(jarr, 0, 3, arrayRegion);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstMFVec3f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec3f object
 * @param[in] element Position of object to retrieve
 * @param[out] sfvec3fObj SFVec3f to store retrieved object in
 */
void JNICALL
Java_vrml_field_ConstMFVec3f_get1Value__ILvrml_field_SFVec3f_2(
        JNIEnv * env,
        jobject obj,
        jint element,
        jobject sfvec3fObj)
{
    const mfvec3f * const mfv =
            static_cast<mfvec3f *>(getFieldValue(env, obj));
    if (!mfv) return;
    sfvec3f * const sfv =
            static_cast<sfvec3f *>(getFieldValue(env, sfvec3fObj));
    if (!sfv) return;
    try {
        sfv->value = mfv->value.at(element);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstMFVec3f::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec3f object
 * @return String representation of ConstMFVec3f.
 */
jstring JNICALL Java_vrml_field_ConstMFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFVec3f_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec3f_CreateObject___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec3f_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec3f_CreateObject__I_3F(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_MFVec3f_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFVec3f_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFVec3f_clear(JNIEnv * env, jobject obj)
{
    mfvec3f * const mfv = static_cast<mfvec3f *>(getFieldValue(env, obj));
    if (!mfv) return;
    mfv->value.clear();
}

void JNICALL Java_vrml_field_MFVec3f_delete
  (JNIEnv *env, jobject obj, jint n)
{
    mfvec3f* mfv = static_cast<mfvec3f*>(getFieldValue(env, obj));
    if (!mfv) return;
    if (!(size_t(n) < mfv->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mfv->value.erase(mfv->value.begin() + n);
}

void JNICALL Java_vrml_field_MFVec3f_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec3f_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec3f_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec3f_getValue___3F(env, obj, jarr);
}

/**
 * @brief JNI implementation of MFVec3f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec3f object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y,z value
 */
void JNICALL Java_vrml_field_MFVec3f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec3f_get1Value__I_3F(env, obj, element, jarr);
}

/**
 * @brief JNI implementation of MFVec3f::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec3f object
 * @param[in] element Position of object to retrieve
 * @param[out] sfvec3f SFVec3f to store retrieved object in
 */
void JNICALL
Java_vrml_field_MFVec3f_get1Value__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec3f)
{
  Java_vrml_field_ConstMFVec3f_get1Value__ILvrml_field_SFVec3f_2
    (env, obj, element, sfvec3f);
}

void JNICALL Java_vrml_field_MFVec3f_setValue___3_3F(JNIEnv * env,
                                                     jobject obj,
                                                     jobjectArray value)
{
    try {
        mfvec3f * const mfv =
                static_cast<mfvec3f *>(getFieldValue(env, obj));
        if (!mfv) return;
        mfv->value.resize(env->GetArrayLength(value)); // Throws bad_alloc.

        for (size_t i = 0; i < mfv->value.size(); ++i) {
            jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(value, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return;
            }
            jfloat * const v = env->GetFloatArrayElements(element, 0);
            if (!v) {
                // Presumably we raised an OutOfMemoryError.
                return;
            }
            const vec3f vec(v[0], v[1], v[2]);
            env->ReleaseFloatArrayElements(element, v, JNI_ABORT);
            mfv->value[i] = vec;
        }
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL Java_vrml_field_MFVec3f_setValue__I_3F(JNIEnv * env,
                                                    jobject obj,
                                                    jint size,
                                                    jfloatArray value)
{
    try {
        mfvec3f * const mfv =
                static_cast<mfvec3f *>(getFieldValue(env, obj));
        if (!mfv) return;
        mfv->value.resize(size); // Throws std::bad_alloc.
        jfloat * const vecs = env->GetFloatArrayElements(value, 0);
        if (!vecs) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (jint i = 0; i < size; ++i) {
            if (!(i * 3 + 2 < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                        env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than vec3s.length");
                break;
            }
            const vec3f vec(vecs[3 * i], vecs[3 * i + 1], vecs[3 * i + 2]);
            mfv->value[i] = vec;
        }
        env->ReleaseFloatArrayElements(value, vecs, JNI_ABORT);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec3f_setValue__Lvrml_field_MFVec3f_2(JNIEnv * env,
                                                        jobject obj,
                                                        jobject value)
{
    mfvec3f * const mfv = static_cast<mfvec3f *>(getFieldValue(env, obj));
    const mfvec3f * const newMFVec3f =
            static_cast<mfvec3f *>(getFieldValue(env, value));
    if (!mfv || !newMFVec3f) return;
    try {
        *mfv = *newMFVec3f; // Throws bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec3f_setValue__Lvrml_field_ConstMFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFVec3f_setValue__Lvrml_field_MFVec3f_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFVec3f_set1Value__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y, jfloat z)
{
    mfvec3f* mfv = static_cast<mfvec3f*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        mfv->value.at(index) = vec3f(x, y, z);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec3f_set1Value__ILvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3fObj)
{
    mfvec3f* mfv = static_cast<mfvec3f*>(getFieldValue(env, obj));
    sfvec3f* sfv = static_cast<sfvec3f*>(getFieldValue(env, sfvec3fObj));
    if (!mfv || !sfv) return;
    try {
        mfv->value.at(index) = sfv->value;
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec3f_set1Value__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  Java_vrml_field_MFVec3f_set1Value__ILvrml_field_ConstSFVec3f_2
    (env, obj, index, sfvec3f);
}

/**
 * @brief JNI implementation of MFVec3f::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec3f object
 * @param[in] x X-component of vector to add to end of list
 * @param[in] y Y-component of vector to add to end of list
 * @param[in] z Z-component of vector to add to end of list
 */
void JNICALL Java_vrml_field_MFVec3f_addValue__FFF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
    mfvec3f* mfv = static_cast<mfvec3f*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        mfv->value.push_back(vec3f(x, y, z));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFVec3f::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec3f object
 * @param[in] sfvec3fObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFVec3f_addValue__Lvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jobject sfvec3fObj)
{
    mfvec3f* mfv = static_cast<mfvec3f*>(getFieldValue(env, obj));
    sfvec3f* sfv = static_cast<sfvec3f*>(getFieldValue(env, sfvec3fObj));
    if (!mfv || !sfv) return;
    try {
        mfv->value.push_back(sfv->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFVec3f::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec3f object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFVec3f_addValue__Lvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFVec3f_addValue__Lvrml_field_ConstSFVec3f_2(env, obj,
                                   value);
}

void JNICALL Java_vrml_field_MFVec3f_insertValue__IFFF(JNIEnv * env,
                                                       jobject obj,
                                                       jint index,
                                                       jfloat x,
                                                       jfloat y,
                                                       jfloat z)
{
    mfvec3f* mfv = static_cast<mfvec3f*>(getFieldValue(env, obj));
    if (!mfv) { return; }
    if (!(size_t(index) < mfv->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
    }
    try {
        mfv->value.insert(mfv->value.begin() + index, vec3f(x, y, z));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec3f_insertValue__ILvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3fObj)
{
    sfvec3f* sfv = static_cast<sfvec3f*>(getFieldValue(env, sfvec3fObj));
    if (!sfv) { return; }
    Java_vrml_field_MFVec3f_insertValue__IFFF(env, obj, index,
                                              sfv->value.x(),
                                              sfv->value.y(),
                                              sfv->value.z());
}

void JNICALL
Java_vrml_field_MFVec3f_insertValue__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  Java_vrml_field_MFVec3f_insertValue__ILvrml_field_ConstSFVec3f_2
    (env, obj, index, sfvec3f);
}

/**
 * @brief JNI implementation of MFVec3f::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec3f object
 * @return String representation of MFVec3f.
 */
jstring JNICALL Java_vrml_field_MFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}


void JNICALL Java_vrml_field_ConstMFVec3d_CreateObject___3_3F(JNIEnv * env,
                                                              jobject obj,
                                                              jobjectArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfvec3d>
            mfvec3d(new openvrml::mfvec3d(env->GetArrayLength(jarr)));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfvec3d.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
    Java_vrml_field_MFVec3d_setValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_ConstMFVec3d_CreateObject__I_3F(JNIEnv * env,
                                                             jobject obj,
                                                             jint size,
                                                             jdoubleArray jarr)
{
    try {
        std::auto_ptr<openvrml::mfvec3d> mfvec3d(new openvrml::mfvec3d(size));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
        env->SetIntField(obj, fid, reinterpret_cast<int>(mfvec3d.release()));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
    Java_vrml_field_MFVec3d_setValue__I_3F(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_ConstMFVec3d_getSize(JNIEnv * env, jobject obj)
{
    mfvec3d * mfv = static_cast<mfvec3d *>(getFieldValue(env, obj));
    if (!mfv) return 0;
    return mfv->value.size();
}

void JNICALL Java_vrml_field_ConstMFVec3d_getValue___3_3F(JNIEnv * env,
                                                          jobject obj,
                                                          jobjectArray jarr)
{
    const mfvec3d * const mfv =
            static_cast<mfvec3d *>(getFieldValue(env, obj));
    if (!mfv) return;
    for (size_t i = 0; i < mfv->value.size(); ++i) {
        jdoubleArray element =
                static_cast<jdoubleArray>(env->GetObjectArrayElement(jarr, i));
        if (!element) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
        env->SetFloatArrayRegion(element, 0, 3,
                                 const_cast<jdouble *>(&mfv->value[i][0]));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

void JNICALL Java_vrml_field_ConstMFVec3d_getValue___3F(JNIEnv * env,
                                                        jobject obj,
                                                        jdoubleArray jarr)
{
    const mfvec3d * const mfv =
            static_cast<mfvec3d *>(getFieldValue(env, obj));
    if (!mfv) return;
    for (size_t i = 0; i < mfv->value.size(); ++i) {
        env->SetFloatArrayRegion(jarr, i * 3, 3,
                                 const_cast<jdouble *>(&mfv->value[i][0]));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

/**
 * @brief JNI implementation of ConstMFVec3d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec3d object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y,z value
 */
void JNICALL Java_vrml_field_ConstMFVec3d_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jdoubleArray jarr)
{
    mfvec3d* mfv = static_cast<mfvec3d*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        jdouble * const arrayRegion =
                const_cast<jdouble*>(&mfv->value.at(element)[0]);
        env->SetFloatArrayRegion(jarr, 0, 3, arrayRegion);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstMFVec3d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec3d object
 * @param[in] element Position of object to retrieve
 * @param[out] sfvec3dObj SFVec3d to store retrieved object in
 */
void JNICALL
Java_vrml_field_ConstMFVec3d_get1Value__ILvrml_field_SFVec3d_2(
        JNIEnv * env,
        jobject obj,
        jint element,
        jobject sfvec3dObj)
{
    const mfvec3d * const mfv =
            static_cast<mfvec3d *>(getFieldValue(env, obj));
    if (!mfv) return;
    sfvec3d * const sfv =
            static_cast<sfvec3d *>(getFieldValue(env, sfvec3dObj));
    if (!sfv) return;
    try {
        sfv->value = mfv->value.at(element);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of ConstMFVec3d::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj ConstMFVec3d object
 * @return String representation of ConstMFVec3d.
 */
jstring JNICALL Java_vrml_field_ConstMFVec3d_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_MFVec3d_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec3d_CreateObject___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec3d_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jdoubleArray jarr)
{
  Java_vrml_field_ConstMFVec3d_CreateObject__I_3F(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_MFVec3d_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFVec3d_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFVec3d_clear(JNIEnv * env, jobject obj)
{
    mfvec3d * const mfv = static_cast<mfvec3d *>(getFieldValue(env, obj));
    if (!mfv) return;
    mfv->value.clear();
}

void JNICALL Java_vrml_field_MFVec3d_delete
  (JNIEnv *env, jobject obj, jint n)
{
    mfvec3d* mfv = static_cast<mfvec3d*>(getFieldValue(env, obj));
    if (!mfv) return;
    if (!(size_t(n) < mfv->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
        return;
    }
    mfv->value.erase(mfv->value.begin() + n);
}

void JNICALL Java_vrml_field_MFVec3d_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec3d_getValue___3_3F(env, obj, jarr);
}

void JNICALL Java_vrml_field_MFVec3d_getValue___3F
  (JNIEnv *env, jobject obj, jdoubleArray jarr)
{
  Java_vrml_field_ConstMFVec3d_getValue___3F(env, obj, jarr);
}

/**
 * @brief JNI implementation of MFVec3d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec3d object
 * @param[in] element Position of object to retrieve
 * @param[out] jarr Array to store retrieved x,y,z value
 */
void JNICALL Java_vrml_field_MFVec3d_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jdoubleArray jarr)
{
  Java_vrml_field_ConstMFVec3d_get1Value__I_3F(env, obj, element, jarr);
}

/**
 * @brief JNI implementation of MFVec3d::get1Value.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec3d object
 * @param[in] element Position of object to retrieve
 * @param[out] sfvec3d SFVec3d to store retrieved object in
 */
void JNICALL
Java_vrml_field_MFVec3d_get1Value__ILvrml_field_SFVec3d_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec3d)
{
  Java_vrml_field_ConstMFVec3d_get1Value__ILvrml_field_SFVec3d_2
    (env, obj, element, sfvec3d);
}

void JNICALL Java_vrml_field_MFVec3d_setValue___3_3F(JNIEnv * env,
                                                     jobject obj,
                                                     jobjectArray value)
{
    try {
        mfvec3d * const mfv =
                static_cast<mfvec3d *>(getFieldValue(env, obj));
        if (!mfv) return;
        mfv->value.resize(env->GetArrayLength(value)); // Throws bad_alloc.

        for (size_t i = 0; i < mfv->value.size(); ++i) {
            jdoubleArray element =
                static_cast<jdoubleArray>(env->GetObjectArrayElement(value, i));
            if (!element) {
                // Presumably we raised an ArrayIndexOutOfBoundsException.
                return;
            }
            jdouble * const v = env->GetFloatArrayElements(element, 0);
            if (!v) {
                // Presumably we raised an OutOfMemoryError.
                return;
            }
            const vec3d vec(v[0], v[1], v[2]);
            env->ReleaseFloatArrayElements(element, v, JNI_ABORT);
            mfv->value[i] = vec;
        }
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL Java_vrml_field_MFVec3d_setValue__I_3F(JNIEnv * env,
                                                    jobject obj,
                                                    jint size,
                                                    jdoubleArray value)
{
    try {
        mfvec3d * const mfv =
                static_cast<mfvec3d *>(getFieldValue(env, obj));
        if (!mfv) return;
        mfv->value.resize(size); // Throws std::bad_alloc.
        jdouble * const vecs = env->GetFloatArrayElements(value, 0);
        if (!vecs) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (jint i = 0; i < size; ++i) {
            if (!(i * 3 + 2 < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                        env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than vec3s.length");
                break;
            }
            const vec3d vec(vecs[3 * i], vecs[3 * i + 1], vecs[3 * i + 2]);
            mfv->value[i] = vec;
        }
        env->ReleaseFloatArrayElements(value, vecs, JNI_ABORT);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec3d_setValue__Lvrml_field_MFVec3d_2(JNIEnv * env,
                                                        jobject obj,
                                                        jobject value)
{
    mfvec3d * const mfv = static_cast<mfvec3d *>(getFieldValue(env, obj));
    const mfvec3d * const newMFVec3d =
            static_cast<mfvec3d *>(getFieldValue(env, value));
    if (!mfv || !newMFVec3d) return;
    try {
        *mfv = *newMFVec3d; // Throws bad_alloc.
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec3d_setValue__Lvrml_field_ConstMFVec3d_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFVec3d_setValue__Lvrml_field_MFVec3d_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFVec3d_set1Value__IFFF
  (JNIEnv *env, jobject obj, jint index, jdouble x, jdouble y, jdouble z)
{
    mfvec3d* mfv = static_cast<mfvec3d*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        mfv->value.at(index) = vec3d(x, y, z);
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec3d_set1Value__ILvrml_field_ConstSFVec3d_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3dObj)
{
    mfvec3d* mfv = static_cast<mfvec3d*>(getFieldValue(env, obj));
    sfvec3d* sfv = static_cast<sfvec3d*>(getFieldValue(env, sfvec3dObj));
    if (!mfv || !sfv) return;
    try {
        mfv->value.at(index) = sfv->value;
    } catch (std::out_of_range & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec3d_set1Value__ILvrml_field_SFVec3d_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3d)
{
  Java_vrml_field_MFVec3d_set1Value__ILvrml_field_ConstSFVec3d_2
    (env, obj, index, sfvec3d);
}

/**
 * @brief JNI implementation of MFVec3d::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec3d object
 * @param[in] x X-component of vector to add to end of list
 * @param[in] y Y-component of vector to add to end of list
 * @param[in] z Z-component of vector to add to end of list
 */
void JNICALL Java_vrml_field_MFVec3d_addValue__FFF
  (JNIEnv *env, jobject obj, jdouble x, jdouble y, jdouble z)
{
    mfvec3d* mfv = static_cast<mfvec3d*>(getFieldValue(env, obj));
    if (!mfv) return;
    try {
        mfv->value.push_back(vec3d(x, y, z));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFVec3d::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec3d object
 * @param[in] sfvec3dObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFVec3d_addValue__Lvrml_field_ConstSFVec3d_2
  (JNIEnv *env, jobject obj, jobject sfvec3dObj)
{
    mfvec3d* mfv = static_cast<mfvec3d*>(getFieldValue(env, obj));
    sfvec3d* sfv = static_cast<sfvec3d*>(getFieldValue(env, sfvec3dObj));
    if (!mfv || !sfv) return;
    try {
        mfv->value.push_back(sfv->value);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of MFVec3d::addValue.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj MFVec3d object
 * @param[in] value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFVec3d_addValue__Lvrml_field_SFVec3d_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFVec3d_addValue__Lvrml_field_ConstSFVec3d_2(env, obj,
                                   value);
}

void JNICALL Java_vrml_field_MFVec3d_insertValue__IFFF(JNIEnv * env,
                                                       jobject obj,
                                                       jint index,
                                                       jdouble x,
                                                       jdouble y,
                                                       jdouble z)
{
    mfvec3d* mfv = static_cast<mfvec3d*>(getFieldValue(env, obj));
    if (!mfv) { return; }
    if (!(size_t(index) < mfv->value.size())) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass =
                env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, "index out of bounds");
    }
    try {
        mfv->value.insert(mfv->value.begin() + index, vec3d(x, y, z));
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

void JNICALL
Java_vrml_field_MFVec3d_insertValue__ILvrml_field_ConstSFVec3d_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3dObj)
{
    sfvec3d* sfv = static_cast<sfvec3d*>(getFieldValue(env, sfvec3dObj));
    if (!sfv) { return; }
    Java_vrml_field_MFVec3d_insertValue__IFFF(env, obj, index,
                                              sfv->value.x(),
                                              sfv->value.y(),
                                              sfv->value.z());
}

void JNICALL
Java_vrml_field_MFVec3d_insertValue__ILvrml_field_SFVec3d_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3d)
{
  Java_vrml_field_MFVec3d_insertValue__ILvrml_field_ConstSFVec3d_2
    (env, obj, index, sfvec3d);
}

/**
 * @brief JNI implementation of MFVec3d::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj MFVec3d object
 * @return String representation of MFVec3d.
 */
jstring JNICALL Java_vrml_field_MFVec3d_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of Script::getField.
 *
 * @param[in] env JNI environment
 * @param[in] obj Script object
 * @param[in] jstrFieldName Name of desired exposed field
 * @return Exposed field object
 */
jobject JNICALL Java_vrml_node_Script_getField(JNIEnv * env,
                                               jobject obj,
                                               jstring jstrFieldName)
{
    const char * charFieldName = env->GetStringUTFChars(jstrFieldName, 0);
    std::string fieldName(charFieldName);
    jfieldID fid = getFid(env, obj, "NodePtr", "I");
    if (!fid) { return 0; }
    script_node* script =
        reinterpret_cast<script_node*>(env->GetIntField(obj, fid));
    if (!script) { return 0; }
    jobject Field;

    script_node::field_value_map_t fieldMap = script->field_value_map();
    script_node::field_value_map_t::iterator iter = fieldMap.find(fieldName);

    if (iter != fieldMap.end()) {
        // Then we've found the field
        const field_value* fieldPtr = iter->second.get();
        assert(fieldPtr);
        std::ostringstream os;
        os << "vrml/field/" << fieldPtr->type() << std::ends;
        jclass clazz = env->FindClass(os.str().c_str());
        Field = env->AllocObject(clazz);
        fid = getFid(env, Field, "FieldPtr", "I");
        if (!fid) { return 0; }
        env->SetIntField(Field, fid, reinterpret_cast<int>(fieldPtr));
    } else {
        jclass excClazz = env->FindClass("vrml/InvalidFieldException");
        if (!excClazz) {
            // Can't find exception, just return
            return 0;
        }
        // throw an exception as the given field doesn't exist
        env->ThrowNew(excClazz, "Field not found");
        return 0;
    }

    fid = getFid(env, Field,"isExposedField", "Z");
    if (!fid) { return 0; }
    env->SetBooleanField(Field, fid, true);
    env->ReleaseStringUTFChars(jstrFieldName, charFieldName);
    return Field;
}

/**
 * @brief JNI implementation of Script::getEventOut.
 *
 * @param[in] env JNI environment
 * @param[in] obj Script object
 * @param[in] jstrEventOutName Name of desired eventOut
 * @return eventOut object
 */
jobject JNICALL Java_vrml_node_Script_getEventOut(JNIEnv * env,
                                                  jobject obj,
                                                  jstring jstrEventOutName)
{
    jobject eventOut;
    const char *charEventOut = env->GetStringUTFChars(jstrEventOutName , 0);
    std::string eventOutName(charEventOut);
    jfieldID fid = getFid(env, obj, "NodePtr", "I");
    if (!fid) return 0;
    script_node* script =
        reinterpret_cast<script_node*>(env->GetIntField(obj, fid));
    if (!script) return 0;

    const node_interface_set & interfaces = script->node::type().interfaces();
    const node_interface_set::const_iterator interface =
        find_if(interfaces.begin(), interfaces.end(),
                bind2nd(node_interface_matches_eventout(), eventOutName));

    field_value::type_id eventOutType = interface->field_type;

    if (eventOutType != field_value::invalid_type_id) {
        const script_node::eventout_map_t & eventOutMap =
            script->eventout_map();
        script_node::eventout_map_t::const_iterator iter =
            eventOutMap.find(eventOutName);

        if (iter != eventOutMap.end()) {
            // Found the eventOut
            std::ostringstream out;
            out << "vrml/field/" << iter->second->value().type();
            jclass clazz = env->FindClass(out.str().c_str());
            eventOut = env->AllocObject(clazz);
            fid = getFid(env, eventOut, "FieldPtr", "I");
            if (!fid) return 0;
            env->SetIntField(eventOut,
                             fid,
                             reinterpret_cast<int>(&iter->second->value()));
        }
    } else {
        jclass excClazz = env->FindClass("vrml/InvalidEventOutException");
        if (excClazz == 0) {
            // Can't find exception, just return
            return 0;
        }

        // throw an exception as the given eventOut doesn't exist
        env->ThrowNew(excClazz, "EventOut not found");
        return 0;
    }

    fid = getFid(env, eventOut,"isEventOut", "Z");
    if (!fid) return 0;
    env->SetBooleanField(eventOut, fid, true);
    env->ReleaseStringUTFChars(jstrEventOutName, charEventOut);

    return eventOut;
}

/**
 * @brief JNI implementation of Script::getEventIn.
 *
 * @param[in] env JNI environment
 * @param[in] obj Script object
 * @param[in] jstrEventInName EventIn name
 * @return EventIn field object.
 */
jobject JNICALL Java_vrml_node_Script_getEventIn(JNIEnv * env,
                                                 jobject obj,
                                                 jstring jstrEventInName)
{
    const char * charEventInName = env->GetStringUTFChars(jstrEventInName, 0);
    std::string eventInName(charEventInName);
    jfieldID fid = getFid(env, obj, "NodePtr", "I");
    if (!fid) return 0;
    script_node* script =
        reinterpret_cast<script_node*>(env->GetIntField(obj, fid));
    if (!script) return 0;
    jobject eventIn;

    const node_interface_set & interfaces = script->node::type().interfaces();
    const node_interface_set::const_iterator interface =
        find_if(interfaces.begin(), interfaces.end(),
                bind2nd(node_interface_matches_eventin(), eventInName));
    if (interface != interfaces.end()) {
        // Then we've found the eventIn
        field_value * fieldPtr = newField(interface->field_type);
        assert(fieldPtr);
        std::ostringstream os;
        os << "vrml/field/" << ftn[interface->field_type];
        jclass clazz = env->FindClass(os.str().c_str());
        eventIn = env->AllocObject(clazz);
        fid = getFid(env, eventIn, "FieldPtr", "I");
        if (!fid) { return 0; }
        env->SetIntField(eventIn, fid, reinterpret_cast<int>(fieldPtr));
    } else {
        jclass excClazz = env->FindClass("vrml/InvalidEventInException");
        if (excClazz == 0) {
            // Can't find exception, just return
            return 0;
        }
        // throw an exception as the given eventIn doesn't exist
        env->ThrowNew(excClazz, "EventIn not found");
        return 0;
    }

    fid = getFid(env, eventIn,"isEventIn", "Z");
    if (!fid) return 0;
    env->SetBooleanField(eventIn, fid, true);
    env->ReleaseStringUTFChars(jstrEventInName, charEventInName );
    return eventIn;
}

/**
 * @brief JNI implementation of Script::toString.
 *
 * @param[in] JNI environment.
 * @param[in] Script object.
 * @return String representation of Script object.
 */
jstring JNICALL Java_vrml_node_Script_toString(JNIEnv * env, jobject obj)
{
    std::ostringstream os;
    jfieldID fid = getFid(env, obj, "NodePtr", "I");
    if (!fid) { return 0; }
    script_node* node =
        reinterpret_cast<script_node*>(env->GetIntField(obj, fid));
    if (!node) { return 0; }
    os << *node;
    const char * str = os.str().c_str();
    jstring result = env->NewStringUTF(str);
    return result;
}

/**
 * @brief JNI implementation of BaseNode::getType.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java node object
 * @return Type of node
 */
jstring JNICALL Java_vrml_BaseNode_getType(JNIEnv * env, jobject obj)
{
    jfieldID fid = getFid(env, obj, "NodePtr", "I");
    if (!fid) return 0;
    node * const n = reinterpret_cast<node *>(env->GetIntField(obj, fid));
    if (!n) { return 0; }
    return env->NewStringUTF(n->type().id().c_str());
}

/**
 * @brief JNI implementation of BaseNode::getBrowser.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java node object
 * @return Browser object
 */
jobject JNICALL Java_vrml_BaseNode_getBrowser(JNIEnv * const env,
                                              const jobject obj)
{
    assert(env);
    jfieldID fid = getFid(env, obj, "isScript", "Z");
    if (!fid) return 0;
    bool isScript = static_cast<bool>(env->GetBooleanField(obj, fid));
    const jclass clazz = env->FindClass("vrml/Browser");
    const jobject jBrowser = env->AllocObject(clazz);
    fid = getFid(env, obj, "NodePtr", "I");
    if (!fid) { return 0; }

    if (isScript) {
        script_node * const node =
            reinterpret_cast<script_node *>(env->GetIntField(obj, fid));
        assert(node);
        assert(node->scene());
        fid = getFid(env, jBrowser, "BrowserPtr", "I");
        if (!fid) return 0;
        env->SetIntField(jBrowser, fid,
                         reinterpret_cast<int>(&node->scene()->browser()));
    } else {
        node * const n =
            reinterpret_cast<node *>(env->GetIntField(obj, fid));
        assert(n);
        assert(n->scene());
        fid = getFid(env, jBrowser, "BrowserPtr", "I");
        if (!fid) { return 0; }
        env->SetIntField(jBrowser, fid,
                         reinterpret_cast<int>(&n->scene()->browser()));
    }
    return jBrowser;
}

/**
 * @brief JNI implementation of Node::getEventIn.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java node object
 * @param[in] jstrEventInName Name of the eventIn
 * @return EventIn field object
 *
 * @todo Implement me! Need to throw InvalidEventInException if
 *       eventIn not present.
 */
jobject JNICALL Java_vrml_node_Node_getEventIn(JNIEnv * const env,
                                               jobject obj,
                                               jstring /* jstrEventInName */)
{
    jfieldID fid = getFid(env, obj, "NodePtr", "I");
    if (!fid) return 0;
    node * n = reinterpret_cast<node*>(env->GetIntField(obj, fid));
    if (!n) { return 0; }

    /* Uncomment when getEventIn implementation is added to Node
    const char *eventInName = env->GetStringUTFChars(jstrEventInName , 0);
    const field_value* field = pNode->getEventIn(eventInName);

    if (field == 0)
    {
       // throw exception as given eventIn name doesn't exist
       return 0;
    }

    char clazzName[256];
    sprintf(clazzName, "vrml/field/%s", field->typeName());
    jclass clazz = env->FindClass(clazzName);
    jobject eventIn = env->AllocObject(clazz);
    fid = env->GetFieldID(clazz, "FieldPtr", "I");;
    env->SetIntField(eventIn, fid, (int) field);
    env->ReleaseStringUTFChars(jstrEventInName, eventInName);
    */
    jobject eventIn;
    return eventIn;
}


/**
 * @brief JNI Implementation of Node::getEventOut.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java node object
 * @param[in] jstrEventOutName Name of the eventOut
 * @return EventOut field object
 *
 * @todo Implement me. Need to throw InvalidEventOutException
 * if eventOut not present.
 */
jobject JNICALL Java_vrml_node_Node_getEventOut(JNIEnv * /* env */,
                                                jobject /* obj */,
                                                jstring /* jstrEventOutName */)
{
  /*
  const char *eventOutName = env->GetStringUTFChars(jstrEventOutName , 0);
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  Node* pNode = (Node*) env->GetIntField(obj, fid);
  const field_value* field = pNode->getEventOut(eventOutName);

  if (field == 0)
  {
    // throw exception as given eventOut name doesn't exist
    return 0;
  }

  char clazzName[256];
  sprintf(clazzName, "vrml/field/%s", field->typeName());
  jclass clazz = env->FindClass(clazzName);
  jobject eventOut = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");;
  env->SetIntField(eventOut, fid, (int) field);
  env->ReleaseStringUTFChars(jstrEventOutName, eventOutName);

  return eventOut;
  */
  return 0;
}

/**
 * @brief JNI Implementation of Node::getExposedField
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java node object
 * @param[in] jstrEventOutName Name of the exposed field
 * @return Exposed field object
 *
 * @todo Implement me. Need to throw InvalidEventOutException
 * if eventOut not present.
 */
jobject JNICALL
Java_vrml_node_Node_getExposedField(JNIEnv * /* env */,
                                    jobject /* obj */,
                                    jstring /* jstrExposedFieldName */)
{
  /*
  const char *exposedFieldName =
    env->GetStringUTFChars(jstrExposedFieldName , 0);
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  Node* pNode = (Node*) env->GetIntField(obj, fid);
  const field_value* field = pNode->getField(exposedFieldName);

  if (field == 0)
  {
    // throw exception as given exposedField name doesn't exist
    return 0;
  }

  char clazzName[256];
  sprintf(clazzName, "vrml/field/%s", field->typeName());
  jclass clazz = env->FindClass(clazzName);
  jobject exposedField = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");;
  env->SetIntField(exposedField, fid, (int) field);
  env->ReleaseStringUTFChars(jstrExposedFieldName, exposedFieldName);

  return exposedField;

  */
  return 0;
}

/**
 * @brief JNI implementation of Node::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj Node object.
 * @return Stringified version of node.
 */
jstring JNICALL Java_vrml_node_Node_toString(JNIEnv * const env, jobject obj)
{
    std::ostringstream os;
    jfieldID fid = getFid(env, obj, "NodePtr", "I");
    if (!fid) { return 0; }
    node * n = reinterpret_cast<node*>(env->GetIntField(obj, fid));
    if (!n) { return 0; }
    os << *n;
    jstring result = env->NewStringUTF(os.str().c_str());
    return result;
}

/**
 * @brief JNI implementation of Event::getName.
 *
 * @param[in] env JNI environment
 * @param[in] obj Event object.
 * @return Name of the event.
 */
jstring JNICALL Java_vrml_Event_getName
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  if (!fid) return 0;
  VrmlEvent* event = reinterpret_cast<VrmlEvent*>(env->GetIntField(obj, fid));
  if (!event) return 0;
  return env->NewStringUTF(event->name().c_str());
}

/**
 * @brief JNI implementation of Event::getTimeStamp.
 *
 * @param[in] env JNI environment
 * @param[in] obj Event object.
 * @return Timestamp of event.
 */
jdouble JNICALL Java_vrml_Event_getTimeStamp(JNIEnv * env, jobject obj) {
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  if (!fid) return 0.0;
  VrmlEvent* event = reinterpret_cast<VrmlEvent*>(env->GetIntField(obj, fid));
  if (!event) return 0.0;
  return event->timeStamp();
}

/**
 * @brief JNI implementation of Event::getValue
 *
 * @param[in] env JNI environment
 * @param[in] obj Event object.
 * @return Changed field.
 */
jobject JNICALL Java_vrml_Event_getValue(JNIEnv * env, jobject obj)
{
    jfieldID fid = getFid(env, obj, "EventPtr", "I");
    if (!fid) { return 0; }
    VrmlEvent * event =
        reinterpret_cast<VrmlEvent *>(env->GetIntField(obj, fid));
    if (!event) { return 0; }
    const field_value * field = event->value();
    assert(field);
    std::ostringstream os;
    os << "vrml/field/Const" << field->type();
    jclass clazz = env->FindClass(os.str().c_str());
    jobject jField = env->AllocObject(clazz);
    fid = env->GetFieldID(clazz, "FieldPtr", "I");
    if (!fid) { return 0; }
    env->SetIntField(jField, fid, reinterpret_cast<int>(field));
    return jField;
}

/**
 * @brief JNI implementation of Event::clone.
 *
 * @param[in] env JNI environment
 * @param[in] obj Event object.
 * @return Clone of event.
 */
jobject JNICALL Java_vrml_Event_clone
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  if (!fid) return 0;
  VrmlEvent* event = reinterpret_cast<VrmlEvent*>(env->GetIntField(obj, fid));
  if (!event) return 0;
  jclass clazz = env->FindClass("vrml/Event");
  jobject jCloneEvent = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "EventPtr", "I");
  if (!fid) return 0;
  VrmlEvent* cloneEvent = event->clone();
  env->SetIntField(jCloneEvent, fid, reinterpret_cast<int>(cloneEvent));
  return jCloneEvent;
}

/**
 * @brief JNI implementation of Event::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj Event object.
 * @return Name of the event.
 */
jstring JNICALL Java_vrml_Event_toString
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  if (!fid) return 0;
  VrmlEvent* event = reinterpret_cast<VrmlEvent*>(env->GetIntField(obj, fid));
  if (!event) return 0;
  return env->NewStringUTF(event->name().c_str());
}

/**
 * @brief JNI implementation of Browser::toString.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 * @return Browser name and version.
 */
jstring JNICALL Java_vrml_Browser_toString(JNIEnv * const env,
                                           const jobject obj)
{
    assert(env);
    const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
    if (!fid) return 0;
    openvrml::browser * const browser =
      reinterpret_cast<openvrml::browser *>(env->GetIntField(obj, fid));
    assert(browser);
    std::ostringstream out;
    out << browser->name() << ' ' << browser->version() << std::ends;
    return env->NewStringUTF(out.str().c_str());
}

/**
 * @brief JNI implementation of Browser::getName.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 * @return Name of browser.
 */
jstring JNICALL Java_vrml_Browser_getName(JNIEnv * const env,
                                          const jobject obj)
{
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0;
  openvrml::browser * const browser =
    reinterpret_cast<openvrml::browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return env->NewStringUTF(browser->name());
}

/**
 * @brief JNI implementation of Browser::getVersion.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 * @return Version of OpenVRML being used
 */
jstring JNICALL Java_vrml_Browser_getVersion(JNIEnv * const env,
                                             const jobject obj)
{
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0;
  openvrml::browser * const browser =
    reinterpret_cast<openvrml::browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return env->NewStringUTF(browser->version());
}

/**
 * @brief JNI implementation of Browser::getCurrentSpeed.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 * @return Average navigation speed in m/s.
 *
 * @todo Fix me to properly return the current speed.
 */
jfloat JNICALL Java_vrml_Browser_getCurrentSpeed(JNIEnv * const env,
                                                 const jobject obj)
{
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0.0;
  openvrml::browser * const browser =
    reinterpret_cast<openvrml::browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return browser->current_speed();
}

/**
 * @brief JNI implementation of Browser::getCurrentFrameRate.
 *
 * Returns the current frame rate in frames per second.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 *
 * @return Average navigation speed.
 */
jfloat JNICALL Java_vrml_Browser_getCurrentFrameRate(JNIEnv * const env,
                                                     const jobject obj)
{
  assert(env);
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0.0;
  openvrml::browser * const browser =
    reinterpret_cast<openvrml::browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return browser->frame_rate();
}

/**
 * @brief JNI implementation of Browser::getWorldURL.
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 * @return URL of current world displayed in browser.
 */
jstring JNICALL Java_vrml_Browser_getWorldURL(JNIEnv * const env,
                                              const jobject obj)
{
  assert(env);
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0;
  openvrml::browser * const browser =
    reinterpret_cast<openvrml::browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return env->NewStringUTF(browser->world_url().c_str());
}

/**
 * @brief JNI implementation of Browser::replaceWorld.
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

/**
 * @brief JNI implementation of Browser::createVrmlFromString
 *
 * @param[in] env JNI environment
 * @param[in] obj JNI version of a Java Browser object.
 * @param[in] vrmlSyntax ...
 * @return ...
 *
 * @todo Implement me! This method should throw an
 *       InvalidVRMLSyntaxException if the syntax is invalid.
 */
jobjectArray JNICALL
Java_vrml_Browser_createVrmlFromString(JNIEnv * const env,
                                       const jobject obj,
                                       const jstring /* vrmlSyntax */)
{
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0;
  openvrml::browser * const browser =
    reinterpret_cast<openvrml::browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return 0;
}

/**
 * @brief JNI implementation of Browser::createVrmlFromURL.
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

/**
 * @brief JNI implementation of Browser::addRoute.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj JNI version of a Java Browser object.
 * @param[in] fromNode ...
 * @param[in] fromEventOut ...
 * @param[in] toNode ...
 * @param[in] toEventIn ...
 *
 * @todo This method should throw an InvalidRouteException if one of
 *       the arguments is invalid. This method should first check
 *       if directOutput is set, if it isn't, don't continue.
 */
void JNICALL Java_vrml_Browser_addRoute(JNIEnv * const env,
                                        jobject,
                                        jobject fromNodeObj,
                                        jstring fromEventOut,
                                        jobject toNodeObj,
                                        jstring toEventIn)
{
    jfieldID fid = getFid(env, fromNodeObj, "NodePtr", "I");
    if (!fid) { return; }
    node * fromNode =
        reinterpret_cast<node *>(env->GetIntField(fromNodeObj, fid));
    assert(fromNode);
    fid = getFid(env, toNodeObj, "NodePtr", "I");
    if (!fid) { return; }
    node * toNode = reinterpret_cast<node *>(env->GetIntField(toNodeObj, fid));
    assert(toNode);
    const char* str = env->GetStringUTFChars(fromEventOut, 0);
    std::string eventOut(str);
    env->ReleaseStringUTFChars(fromEventOut, str);
    str = env->GetStringUTFChars(toEventIn, 0);
    std::string eventIn(str);
    env->ReleaseStringUTFChars(toEventIn, str);
    add_route(*fromNode, eventOut, *toNode, eventIn);
}

/**
 * @brief JNI implementation of Browser::deleteRoute
 *
 * @param[in] env JNI environment
 * @param[in,out] obj JNI version of a Java Browser object.
 * @param[in] fromNode ...
 * @param[in] fromEventOut ...
 * @param[in] toNode ...
 * @param[in] toEventIn ...
 *
 * @todo This method should throw an InvalidRouteException if one of
 *       the arguments is invalid. This method should first check
 *       if directOutput is set, if it isn't, don't continue.
 */
void JNICALL Java_vrml_Browser_deleteRoute(JNIEnv * const env,
                                           jobject,
                                           jobject fromNodeObj,
                                           jstring fromEventOut,
                                           jobject toNodeObj,
                                           jstring toEventIn)
{
    jfieldID fid = getFid(env, fromNodeObj, "NodePtr", "I");
    if (!fid) { return; }
    node * fromNode =
        reinterpret_cast<node *>(env->GetIntField(fromNodeObj, fid));
    assert(fromNode);
    fid = getFid(env, toNodeObj, "NodePtr", "I");
    if (!fid) { return; }
    node * toNode = reinterpret_cast<node *>(env->GetIntField(toNodeObj, fid));
    assert(toNode);
    const char* str = env->GetStringUTFChars(fromEventOut, 0);
    std::string eventOut(str);
    env->ReleaseStringUTFChars(fromEventOut, str);
    str = env->GetStringUTFChars(toEventIn, 0);
    std::string eventIn(str);
    env->ReleaseStringUTFChars(toEventIn, str);
    delete_route(*fromNode, eventOut, *toNode, eventIn);
}

/**
 * @brief JNI implementation of Browser::loadURL.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj JNI version of a Java Browser object.
 * @param[in] jUrlArray URL to load with alternates.
 * @param[in] jParameterArray Array of parameters to load URL with
 *
 * @todo This method should throw an InvalidVRMLSyntaxException
 *       if the syntax is invalid. Also need to check if
 *       mustEvaluate flag is set, if not, don't continue.
 */
void JNICALL Java_vrml_Browser_loadURL(JNIEnv * const env,
                                       const jobject obj,
                                       const jobjectArray jUrlArray,
                                       const jobjectArray jParameterArray)
{
    using std::string;
    using std::vector;

    assert(env);
    assert(jUrlArray);

    try {
        size_t i;
        vector<string> url(env->GetArrayLength(jUrlArray)); // Throws std::bad_alloc.
        for (i = 0; i < url.size(); ++i) {
            jstring jstr =
                static_cast<jstring>(env->GetObjectArrayElement(jUrlArray, i));
            if (env->ExceptionCheck()) {
                return; // ArrayIndexOutOfBoundsException
            }
            const char * str = env->GetStringUTFChars(jstr, 0);
            if (!str) { return; } // OutOfMemoryError
            url[i] = str; // Throws std::bad_alloc.
            env->ReleaseStringUTFChars(jstr, str);
        }

        vector<string> parameters(env->GetArrayLength(jParameterArray)); // Throws std::bad_alloc.
        for (i = 0; i < parameters.size(); ++i) {
            jstring jstr = static_cast<jstring>
                            (env->GetObjectArrayElement(jParameterArray, i));
            if (env->ExceptionCheck()) {
                return; // ArrayIndexOutOfBoundsException
            }
            const char * str = env->GetStringUTFChars(jstr, 0);
            if (!str) { return; } // OutOfMemoryError
            parameters[i] = str; // Throws std::bad_alloc.
            env->ReleaseStringUTFChars(jstr, str);
        }

        const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
        if (!fid) { return; }
        openvrml::browser * const browser =
            reinterpret_cast<openvrml::browser *>(env->GetIntField(obj, fid));
        assert(browser);
        browser->load_url(url, parameters);
    } catch (std::bad_alloc & ex) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        jclass exceptionClass = env->FindClass("java/lang/OutOfMemoryError");
        if (!exceptionClass) {
            // Presumably FindClass raised an exception.
            return;
        }
        env->ThrowNew(exceptionClass, ex.what());
    }
}

/**
 * @brief JNI implementation of Browser::setDescription.
 *
 * @param[in] env JNI environment
 * @param[in,out] obj JNI version of a Java Browser object.
 * @param[in] jDescription Desired description of Browser.
 */
void JNICALL Java_vrml_Browser_setDescription
  (JNIEnv *env, jobject obj, jstring jDescription)
{
    const char *description = env->GetStringUTFChars(jDescription, 0);
    if (!description) { return; } // OutOfMemoryError
    const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
    if (!fid) { return; }
    openvrml::browser * const browser =
        reinterpret_cast<openvrml::browser *>(env->GetIntField(obj, fid));
    assert(browser);
    browser->description(description);
    env->ReleaseStringUTFChars(jDescription, description);
}

#endif // OPENVRML_ENABLE_SCRIPT_NODE_JAVA
