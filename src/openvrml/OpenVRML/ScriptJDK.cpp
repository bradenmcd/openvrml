//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2001  Henri Manson
// Copyright (C) 2002  Thomas Flynn
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
#   include <stdio.h>
#   include <string.h>
#   include <strstream>
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
#   include <vrml_field_SFImage.h>
#   include <vrml_field_SFInt32.h>
#   include <vrml_field_SFNode.h>
#   include <vrml_field_SFRotation.h>
#   include <vrml_field_SFString.h>
#   include <vrml_field_SFTime.h>
#   include <vrml_field_SFVec2f.h>
#   include <vrml_field_SFVec3f.h>
#   include <vrml_field_MFColor.h>
#   include <vrml_field_MFFloat.h>
#   include <vrml_field_MFInt32.h>
#   include <vrml_field_MFNode.h>
#   include <vrml_field_MFRotation.h>
#   include <vrml_field_MFString.h>
#   include <vrml_field_MFTime.h>
#   include <vrml_field_MFVec2f.h>
#   include <vrml_field_MFVec3f.h>
#   include <vrml_field_ConstSFBool.h>
#   include <vrml_field_ConstSFColor.h>
#   include <vrml_field_ConstSFFloat.h>
#   include <vrml_field_ConstSFImage.h>
#   include <vrml_field_ConstSFInt32.h>
#   include <vrml_field_ConstSFNode.h>
#   include <vrml_field_ConstSFRotation.h>
#   include <vrml_field_ConstSFString.h>
#   include <vrml_field_ConstSFTime.h>
#   include <vrml_field_ConstSFVec2f.h>
#   include <vrml_field_ConstSFVec3f.h>
#   include <vrml_field_ConstMFColor.h>
#   include <vrml_field_ConstMFFloat.h>
#   include <vrml_field_ConstMFInt32.h>
#   include <vrml_field_ConstMFNode.h>
#   include <vrml_field_ConstMFRotation.h>
#   include <vrml_field_ConstMFString.h>
#   include <vrml_field_ConstMFTime.h>
#   include <vrml_field_ConstMFVec2f.h>
#   include <vrml_field_ConstMFVec3f.h>
#   include <vrml_node_Node.h>
#   include <vrml_node_Script.h>

#   include "doc2.hpp"
#   include "MathUtils.h"
#   include "System.h"
#   include "script.h"
#   include "browser.h"
#   include "nodeptr.h"
#   include "node.h"
#   include "field.h"

#   ifdef _WIN32
#     define PATH_SEPARATOR ";"
#   else /* UNIX and macintosh */
#     define PATH_SEPARATOR ":"
#   endif

using namespace OpenVRML;

namespace {

   /**
    * @class VrmlEvent
    *
    * @brief Used to store events to be processed by the JSAI code.
    */
    class VrmlEvent {
        double d_timeStamp;
        std::string d_eventName;
        FieldValue *d_value;

    public:
        VrmlEvent(double timeStamp, const std::string& eventName,
                  const FieldValue * value) :
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
        const FieldValue * value() const { return d_value; }
    };
}

const char * ftn[] = {
  "<invalid field type>",
  "SFBool",
  "SFColor",
  "SFFloat",
  "SFImage",
  "SFInt32",
  "SFNode",
  "SFRotation",
  "SFString",
  "SFTime",
  "SFVec2f",
  "SFVec3f",
  "MFColor",
  "MFFloat",
  "MFInt32",
  "MFNode",
  "MFRotation",
  "MFString",
  "MFTime",
  "MFVec2f",
  "MFVec3f"
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
 * @param scriptNode Reference to the ScriptNode that uses this Script.
 * @param className Name of the Java class.
 * @param classDir Location of Java class.
 */
ScriptJDK::ScriptJDK(ScriptNode & scriptNode, const char * className,
		     const char * classDir):
        Script(scriptNode), d_class(0), d_object(0), d_processEventsID(0),
        d_processEventID(0), d_eventsProcessedID(0) {

  if (! d_jvm)			// Initialize static members
  {
    JavaVMInitArgs vm_args;
    jint res;
    JavaVMOption options[4];

    /* get the currently defined CLASSPATH env variable */
    char* classPath = getenv("CLASSPATH");

    std::ostrstream appendedClassPath;

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

    options[0].optionString = appendedClassPath.str();
    options[1].optionString = "-verbose:class";
    options[2].optionString = "-verbose:jni";
#ifndef _WIN32
    options[3].optionString = "-Djava.library.path=" OPENVRML_LIBDIR_;
#endif
    vm_args.version = JNI_VERSION_1_2;
    vm_args.options = options;
#ifdef _WIN32
    vm_args.nOptions = 3;
#else
    vm_args.nOptions = 4;
#endif
    /* Create the Java VM */
    res = JNI_CreateJavaVM(&d_jvm, (void**) &d_env, &vm_args);
    appendedClassPath.rdbuf()->freeze(false);

    if (res < 0)
      theSystem->error("Can't create Java VM");
  }

  if (d_jvm && d_env)		// Per-object initialization
  {
    char fqClassName[1024];
    jthrowable exception;
    strcpy(fqClassName, "");
    strcat(fqClassName, className);

    d_class = d_env->FindClass(fqClassName);
    if (!d_class)
    {
      theSystem->error( "Can't find Java class %s.\n", className);
      return;
    }

    // Call constructor
    jmethodID ctorId = d_env->GetMethodID(d_class, "<init>", "()V");

    if (ctorId)
      d_object = d_env->NewObject(d_class, ctorId);

    jfieldID fid = d_env->GetFieldID(d_class, "NodePtr", "I");
    d_env->SetIntField(d_object, fid, reinterpret_cast<int>(&scriptNode));

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
     * @param env JNI environment
     * @param obj JNI object
     * @param membername Name of desired member of JNI object
     * @param sig Data type of member
     * @return JNI fieldID of member on the given object
     */
    jfieldID getFid(JNIEnv *env, jobject obj, char *membername, char *sig)
    {
        assert(obj);
        jclass clazz = env->GetObjectClass(obj);
        return env->GetFieldID(clazz, membername, sig);
    }

    /**
     * @brief Method to retrieve the corresponding field value pointer
     * in OpenVRML from a field in Java.
     *
     * @param env JNI environment
     * @param obj JNI version of a Java field object
     * @return OpenVRML field value pointer representing the passed in Java
     *         field object.
     *
     * @todo This method needs to be revisited. We immediately set the
     *       modified flag on eventOuts even though nothing has been set yet.
     */
    FieldValue* getFieldValue(JNIEnv *env, jobject obj)
    {
      FieldValue* fieldPtr;
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

      bool eventIn = static_cast<bool>(env->GetBooleanField(obj, fid));

      if ((fid = getFid(env, obj, "isExposedField", "Z")) == 0)
      {
        // Should never occur
        return 0;
      }

      bool exposedField = static_cast<bool>(env->GetBooleanField(obj, fid));

      if ((fid = getFid(env, obj, "FieldPtr", "I")) == 0)
      {
        // Should never occur
        return 0;
      }

      if (eventOut)
      {
        ScriptNode::PolledEventOutValue * const eventOutPtr =
          reinterpret_cast<ScriptNode::PolledEventOutValue *>
            (env->GetIntField(obj, fid));
        fieldPtr = eventOutPtr->value.get();
        eventOutPtr->modified = true;
      }
      else
      {
        fieldPtr = reinterpret_cast<FieldValue*>(env->GetIntField(obj, fid));
      }

      return fieldPtr;
    }

    /**
     * @brief This method converts a JNI VRML field object to its
     *        string representation.
     *
     * @param env JNI environment
     * @param obj JNI version of a Java field object
     * @return String representation of a VRML field
     */
    jstring fieldToString(JNIEnv *env, jobject obj)
    {
      std::ostrstream os;
      FieldValue* field = getFieldValue(env, obj);
      if (!field) return 0;
      os << *field << std::ends;
      char* szString = os.str();
      jstring result = env->NewStringUTF(szString);
      os.rdbuf()->freeze(0);
      return result;
    }
}

/**
 * @brief Initialize the Script object.
 *
 * @param timestamp Time at which script is being initialized.
 */
void ScriptJDK::initialize(const double timestamp)
{
  const SFTime arg(timestamp);
  const FieldValue * argv[] = { &arg };
  this->activate(timestamp, "initialize", 1, argv);
}

/**
 * @brief Called to process an event that has occurred.
 *
 * @param id Name of the event.
 * @param value Value of the event.
 * @param timestamp Time at which event occurred.
 */
void ScriptJDK::processEvent(const std::string & id, const FieldValue & value,
                             const double timestamp)
{
  const SFTime timestampArg(timestamp);
  const FieldValue * argv[] = { &value, &timestampArg };
  this->activate(timestamp, id, 2, argv);
}

/**
 * @brief Called to process one or more events that have occurred.
 *
 * @param timestamp Time at which last event occurred.
 */
void ScriptJDK::eventsProcessed(const double timestamp)
{
  const SFTime arg(timestamp);
  const FieldValue * argv[] = { &arg };
  this->activate(timestamp, "eventsProcessed", 1, argv);
}

/**
 * @brief Called when the script is deleted or the world containing
 * the script is unloaded or replaced
 *
 * @param timestamp Time at which script is being destroyed.
 */
void ScriptJDK::shutdown(const double timestamp)
{
  const SFTime arg(timestamp);
  const FieldValue * argv[] = { &arg };
  this->activate(timestamp, "shutdown", 1, argv);
}

/**
 * @brief Run a specified script.
 *
 * @param timeStamp Time at which script is being run.
 * @param fname Script name
 * @param argc Number of arguments to pass to script
 * @param argv Array of arguments
 */
void ScriptJDK::activate( double timeStamp,
			  const std::string& fname,
			  size_t argc,
			  const FieldValue* const argv[] )
{
  if (argc == 2 && d_processEventID)
  {
    jclass clazz = d_env->FindClass("vrml/Event");

    if (!clazz)
    {
      theSystem->error("Can't find Java class vrml/Event");
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
 * @brief Create a new FieldValue of the given type.
 *
 * @param fieldtype Type of field to create.
 * @return New FieldValue of given type
 */
static FieldValue* newField(FieldValue::Type fieldtype)
{
  switch (fieldtype)
  {
    case FieldValue::invalidType:
      return NULL;
    case FieldValue::sfbool:
      return new SFBool;
    case FieldValue::sfcolor:
      return new SFColor;
    case FieldValue::sffloat:
      return new SFFloat;
    case FieldValue::sfimage:
      return new SFImage;
    case FieldValue::sfint32:
      return new SFInt32;
    case FieldValue::sfnode:
      return new SFNode;
    case FieldValue::sfrotation:
      return new SFRotation;
    case FieldValue::sfstring:
      return new SFString;
    case FieldValue::sftime:
      return new SFTime;
    case FieldValue::sfvec2f:
      return new SFVec2f;
    case FieldValue::sfvec3f:
      return new SFVec3f;
    case FieldValue::mfcolor:
      return new MFColor;
    case FieldValue::mffloat:
      return new MFFloat;
    case FieldValue::mfint32:
      return new MFInt32;
    case FieldValue::mfnode:
      return new MFNode;
    case FieldValue::mfrotation:
      return new MFRotation;
    case FieldValue::mfstring:
      return new MFString;
    case FieldValue::mftime:
      return new MFTime;
    case FieldValue::mfvec2f:
      return new MFVec2f;
    case FieldValue::mfvec3f:
      return new MFVec3f;
    default:
      return NULL;
  }
}

/**
 * @brief Clone a JNI VRML field object.
 *
 * @param env JNI environment
 * @param obj JNI version of a VRML field object.
 * @return Cloned copy of JNI version of a VRML field object.
 *
 * @todo revisit this method
 */
jobject JNICALL Java_vrml_Field_clone
  (JNIEnv *env, jobject obj)
{
  // This method will fail if used on an eventOut (as the stored value is
  // not a FieldValue*. It'll all fail as soon as anyone tries to use the
  // field in any way as the flags aren't copied that indicate whether the
  // field is an eventIn, eventOut or exposedField.

  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return 0;
  FieldValue* field =
    reinterpret_cast<FieldValue*>(env->GetIntField(obj, fid));
  if (!field) return 0;
  std::ostrstream os;
  os << "vrml/field/" << field->type() << '\0';
  jclass clazz = env->FindClass(os.str());
  os.rdbuf()->freeze(false);
  jobject jCloneField = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");
  if (!fid) return 0;
  FieldValue* cloneField = field->clone().release();
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
jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
  return JNI_VERSION_1_2;
}

/**
 * @brief Called when the native library is unloaded.
 *
 * @param vm Java VM.
 * @param reserved unused.
 */
void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
}

/**
 * @brief JNI implementation of ConstSFBool::CreateObject.
 *
 * @param env JNI environment
 * @param obj ConstSFBool object
 * @param value Initial value of ConstSFBool field
 */
void JNICALL Java_vrml_field_ConstSFBool_CreateObject
  (JNIEnv *env, jobject obj, jboolean value)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  SFBool* sfbool = new SFBool(value ? true : false);
  env->SetIntField(obj, fid, reinterpret_cast<int>(sfbool));
}

/**
 * @brief JNI implementation of ConstSFBool::getValue.
 *
 * @param env JNI environment
 * @param obj ConstSFBool object
 * @return Value of ConstSFBool field
 */
jboolean JNICALL Java_vrml_field_ConstSFBool_getValue(JNIEnv * env,
                                                      jobject obj) {
  SFBool * sfbool = static_cast<SFBool *>(getFieldValue(env, obj));
  if (!sfbool) return false;
  return sfbool->get();
}

/**
 * @brief JNI implementation of ConstSFBool::toString.
 *
 * @param env JNI environment
 * @param obj ConstSFBool object
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
 * @param env JNI environment
 * @param obj SFBool object
 * @param value Initial value of SFBool object.
 */
void JNICALL Java_vrml_field_SFBool_CreateObject
  (JNIEnv *env, jobject obj, jboolean value)
{
  Java_vrml_field_ConstSFBool_CreateObject(env, obj, value);
}

/**
 * @brief JNI implementation of SFBool::getValue.
 *
 * @param env JNI environment
 * @param obj SFBool object
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
 * @param env JNI environment
 * @param obj SFBool object
 * @param value Desired value of SFBool object.
 */
void JNICALL Java_vrml_field_SFBool_setValue__Z(JNIEnv * env,
                                                jobject obj,
                                                jboolean value) {
  SFBool * sfbool = static_cast<SFBool *>(getFieldValue(env, obj));
  if (!sfbool) return;
  sfbool->set(value);
}

/**
 * @brief JNI implementation of SFBool::setValue.
 *
 * @param env JNI environment
 * @param obj SFBool object
 * @param value Desired value of SFBool object.
 */
void JNICALL
Java_vrml_field_SFBool_setValue__Lvrml_field_ConstSFBool_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFBool* newSFBool = static_cast<SFBool*>(getFieldValue(env, value));
  SFBool* sfbool = static_cast<SFBool*>(getFieldValue(env, obj));
  if ((!newSFBool) || (!sfbool)) return;
  sfbool->set(newSFBool->get());
}

/**
 * @brief JNI implementation of SFBool::setValue.
 *
 * @param env JNI environment
 * @param obj SFBool object
 * @param value Desired value of SFBool object.
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
 * @param env JNI environment
 * @param obj SFBool object
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
 * @param env JNI environment
 * @param obj ConstSFColor object.
 * @param r Red component of Color
 * @param g Green component of Color
 * @param b Blue component of Color
 */
void JNICALL Java_vrml_field_ConstSFColor_CreateObject
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  SFColor* sfcolor = new SFColor(r, g, b);
  env->SetIntField(obj, fid, reinterpret_cast<int>(sfcolor));
}

/**
 * @brief JNI implementation of ConstSFColor::getValue.
 *
 * @param env JNI environment
 * @param obj ConstSFColor object
 * @param jarr Array to fill with RGB values of SFColor.
 */
void JNICALL Java_vrml_field_ConstSFColor_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr) {
  SFColor * sfcolor = static_cast<SFColor*>(getFieldValue(env, obj));
  if (!sfcolor) return;
  env->SetFloatArrayRegion(jarr, 0, 3, const_cast<float *>(sfcolor->get()));
}

/**
 * @brief JNI implementation of ConstSFColor::getRed.
 *
 * @param env JNI environment
 * @param obj ConstSFColor object
 * @return Red component of SFColor
 */
jfloat JNICALL Java_vrml_field_ConstSFColor_getRed(JNIEnv * env, jobject obj) {
  SFColor * sfcolor = static_cast<SFColor *>(getFieldValue(env, obj));
  if (!sfcolor) return 0.0;
  return sfcolor->getR();
}

/**
 * @brief JNI implementation of ConstSFColor::getGreen.
 *
 * @param env JNI environment
 * @param obj ConstSFColor object
 * @return Green component of SFColor
 */
jfloat JNICALL Java_vrml_field_ConstSFColor_getGreen(JNIEnv * env,
                                                     jobject obj) {
  SFColor * sfcolor = static_cast<SFColor *>(getFieldValue(env, obj));
  if (!sfcolor) return 0.0;
  return sfcolor->getG();
}

/**
 * @brief JNI implementation of ConstSFColor::getBlue.
 *
 * @param env JNI environment
 * @param obj ConstSFColor object
 * @return Blue component of SFColor
 */
jfloat JNICALL Java_vrml_field_ConstSFColor_getBlue(JNIEnv * env, jobject obj) {
  SFColor * sfcolor = static_cast<SFColor *>(getFieldValue(env, obj));
  if (!sfcolor) return 0.0;
  return sfcolor->getB();
}

/**
 * @brief JNI implementation of ConstSFColor::toString.
 *
 * @param env JNI environment
 * @param obj ConstSFColor object.
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
 * @param env JNI environment
 * @param obj SFColor object
 * @param r Red component of Color
 * @param g Green component of Color
 * @param b Blue component of Color
 */
void JNICALL Java_vrml_field_SFColor_CreateObject
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  Java_vrml_field_ConstSFColor_CreateObject(env, obj, r, g, b);
}

/**
 * @brief JNI implementation of SFColor::getValue
 *
 * @param env JNI environment
 * @param obj SFColor object
 * @param jarr Array to populate with SFColor RGB components.
 */
void JNICALL Java_vrml_field_SFColor_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFColor_getValue(env, obj, jarr);
}

/**
 * @brief JNI implementation of SFColor::getRed.
 *
 * @param env JNI environment
 * @param obj SFColor object
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
 * @param env JNI environment
 * @param obj SFColor object
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
 * @param env JNI environment
 * @param obj SFColor object
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
 * @param env JNI environment
 * @param obj SFColor object
 * @param jarr Array containing RGB components of desired SFColor.
 */
void JNICALL Java_vrml_field_SFColor_setValue___3F(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr)
{
  SFColor * sfcolor = static_cast<SFColor *>(getFieldValue(env, obj));
  if (!sfcolor) return;
  jfloat * pjf = env->GetFloatArrayElements(jarr, NULL);
  float rgb[3] = { pjf[0], pjf[1], pjf[2] };
  sfcolor->set(rgb);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

/**
 * @brief JNI implementation of SFColor::setValue.
 *
 * @param env JNI environment
 * @param obj SFColor object
 * @param r Red component of desired color.
 * @param g Green component of desired color.
 * @param b Blue component of desired color.
 */
void JNICALL Java_vrml_field_SFColor_setValue__FFF
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  const float rgb[3] = { r, g, b };
  SFColor* sfcolor = static_cast<SFColor*>(getFieldValue(env, obj));
  if (!sfcolor) return;
  sfcolor->set(rgb);
}

/**
 * @brief JNI implementation of SFColor::setValue.
 *
 * @param env JNI environment
 * @param obj SFColor object
 * @param value ConstSFColor object to copy color from.
 */
void JNICALL
Java_vrml_field_SFColor_setValue__Lvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFColor* newSFColor = static_cast<SFColor*>(getFieldValue(env, value));
  SFColor* sfcolor = static_cast<SFColor*>(getFieldValue(env, obj));
  if (!newSFColor || !sfcolor) return;
  sfcolor->set(newSFColor->get());
}

/**
 * @brief JNI implementation of SFColor::setValue.
 *
 * @param env JNI environment
 * @param obj SFColor object
 * @param value SFColor object to copy color from.
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
 * @param env JNI environment
 * @param obj SFColor object
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
 * @param env JNI environment
 * @param obj ConstSFFloat object
 * @param value Initial value for object
 */
void JNICALL Java_vrml_field_ConstSFFloat_CreateObject
  (JNIEnv *env, jobject obj, jfloat value)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  SFFloat* sffloat = new SFFloat(static_cast<float>(value));
  env->SetIntField(obj, fid, reinterpret_cast<int>(sffloat));
}

/**
 * @brief JNI implementation of ConstSFFloat::getValue.
 *
 * @param env JNI environment
 * @param obj ConstSFFloat object
 * @return Value of field.
 */
jfloat JNICALL Java_vrml_field_ConstSFFloat_getValue(JNIEnv * env,
                                                     jobject obj) {
  SFFloat * sffloat = static_cast<SFFloat *>(getFieldValue(env, obj));
  if (!sffloat) return 0.0;
  return sffloat->get();
}

/**
 * @brief JNI implementation of ConstSFFloat::toString.
 *
 * @param env JNI environment
 * @param obj ConstSFFloat object
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
 * @param env JNI environment
 * @param obj SFFloat object
 * @param value Initial value for object
 */
void JNICALL Java_vrml_field_SFFloat_CreateObject
  (JNIEnv *env, jobject obj, jfloat value)
{
  Java_vrml_field_ConstSFFloat_CreateObject(env, obj, value);
}

/**
 * @brief JNI implementation of SFFloat::getValue.
 *
 * @param env JNI environment
 * @param obj SFFloat object
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
 * @param env JNI environment
 * @param obj SFFloat object
 * @param f Desired value
 */
void JNICALL Java_vrml_field_SFFloat_setValue__F(JNIEnv * env,
                                                 jobject obj,
                                                 jfloat f) {
  SFFloat * sffloat = static_cast<SFFloat *>(getFieldValue(env, obj));
  if (!sffloat) return;
  sffloat->set(f);
}

/**
 * @brief JNI implementation of SFFloat::setValue.
 *
 * @param env JNI environment
 * @param obj SFFloat object
 * @param value Desired value
 */
void JNICALL
Java_vrml_field_SFFloat_setValue__Lvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFFloat* sffloat = static_cast<SFFloat*>(getFieldValue(env, obj));
  SFFloat* newSFFloat = static_cast<SFFloat*>(getFieldValue(env, value));
  if (!sffloat || !newSFFloat) return;
  sffloat->set(newSFFloat->get());
}

/**
 * @brief JNI implementation of SFFloat::setValue.
 *
 * @param env JNI environment
 * @param obj SFFloat object
 * @param value Desired value
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
 * @param env JNI environment
 * @param obj SFFloat object
 * @return String representation of SFFloat.
 */
jstring JNICALL Java_vrml_field_SFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of ConstSFImage::CreateObject.
 *
 * @param env JNI environment
 * @param obj ConstSFImage object
 * @param width Initial width
 * @param height Initial height
 * @param components Initial number of components
 * @param pixels Initial set of pixels
 */
void JNICALL Java_vrml_field_ConstSFImage_CreateObject
  (JNIEnv *env, jobject obj, jint width, jint height,
   jint components, jbyteArray pixels)
{
  SFImage* sfimage;
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;

  if (pixels)
  {
    jbyte *pjb = env->GetByteArrayElements(pixels, NULL);
    sfimage = new SFImage(static_cast<int>(width),
                          static_cast<int>(height),
                          static_cast<int>(components),
                          (unsigned char *)pjb);
    env->ReleaseByteArrayElements(pixels, pjb, JNI_ABORT);
  }
  else
  {
    sfimage = new SFImage(static_cast<int>(width),
                          static_cast<int>(height),
                          static_cast<int>(components), 0);
  }

  env->SetIntField(obj, fid, reinterpret_cast<int>(sfimage));
}

/**
 * @brief JNI implementation of ConstSFImage::getWidth.
 *
 * @param env JNI environment
 * @param obj ConstSFImage object
 * @return Width of ConstSFImage
 */
jint JNICALL Java_vrml_field_ConstSFImage_getWidth
  (JNIEnv *env, jobject obj)
{
  SFImage* sfimage = static_cast<SFImage*>(getFieldValue(env, obj));
  if (!sfimage) return 0;
  return static_cast<jint>(sfimage->getWidth());
}

/**
 * @brief JNI implementation of ConstSFImage::getHeight.
 *
 * @param env JNI environment
 * @param obj ConstSFImage object
 * @return Height of ConstSFImage
 */
jint JNICALL Java_vrml_field_ConstSFImage_getHeight
  (JNIEnv *env, jobject obj)
{
  SFImage* sfimage = static_cast<SFImage*>(getFieldValue(env, obj));
  if (!sfimage) return 0;
  return static_cast<jint>(sfimage->getHeight());
}

/**
 * @brief JNI implementation of ConstSFImage::getComponents.
 *
 * @param env JNI environment
 * @param obj ConstSFImage object
 * @return Number of components in ConstSFImage
 */
jint JNICALL Java_vrml_field_ConstSFImage_getComponents
  (JNIEnv *env, jobject obj)
{
  SFImage* sfimage = static_cast<SFImage*>(getFieldValue(env, obj));
  if (!sfimage) return 0;
  return static_cast<jint>(sfimage->getComponents());
}

/**
 * @brief JNI implementation of ConstSFImage::getPixels.
 *
 * @param env JNI environment
 * @param obj ConstSFImage object
 * @param pixels Array to fill with ConstSFImage pixels
 */
void JNICALL Java_vrml_field_ConstSFImage_getPixels
  (JNIEnv *env, jobject obj, jbyteArray pixels)
{
  SFImage* sfimage = static_cast<SFImage*>(getFieldValue(env, obj));
  if (!sfimage) return;
  env->SetByteArrayRegion(pixels, 0,
                          sfimage->getWidth() * sfimage->getHeight(),
                          (jbyte*)sfimage->getPixels());
}

/**
 * @brief JNI implementation of ConstSFImage::toString.
 *
 * @param env JNI environment
 * @param obj ConstSFImage object
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
 * @param env JNI environment
 * @param obj SFImage object
 * @param width Initial width
 * @param height Initial height
 * @param components Initial number of components
 * @param pixels Initial set of pixels
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
 * @param env JNI environment
 * @param obj SFImage object
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
 * @param env JNI environment
 * @param obj SFImage object
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
 * @param env JNI environment
 * @param obj SFImage object
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
 * @param env JNI environment
 * @param obj SFImage object
 * @param pixels Array to fill with SFImage pixels
 */
void JNICALL Java_vrml_field_SFImage_getPixels
  (JNIEnv *env, jobject obj, jbyteArray pixels)
{
  Java_vrml_field_ConstSFImage_getPixels(env, obj, pixels);
}

/**
 * @brief JNI implementation of SFImage::setValue.
 *
 * @param env JNI environment
 * @param obj SFImage object
 * @param width New width
 * @param height New height
 * @param components Number of components
 * @param pixels New set of pixels
 */
void JNICALL Java_vrml_field_SFImage_setValue__III_3B
  (JNIEnv *env, jobject obj, jint width, jint height,
   jint components, jbyteArray pixels)
{
  SFImage* sfimage = static_cast<SFImage*>(getFieldValue(env, obj));
  if (!sfimage) return;
  jbyte *pjb = env->GetByteArrayElements(pixels, NULL);
  sfimage->set(static_cast<int>(width), static_cast<int>(height),
                static_cast<int>(components),
                (unsigned char *)pjb);
  env->ReleaseByteArrayElements(pixels, pjb, JNI_ABORT);
}

/**
 * @brief JNI implementation of SFImage::setValue.
 *
 * @param env JNI environment
 * @param obj SFImage object
 * @param value ConstSFImage to copy value from
 */
void JNICALL
Java_vrml_field_SFImage_setValue__Lvrml_field_ConstSFImage_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFImage* sfimage = static_cast<SFImage*>(getFieldValue(env, obj));
  if (!sfimage) return;
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  if (!fid) return;
  SFImage* newSFImage =
    reinterpret_cast<SFImage*>(env->GetIntField(value, fid));
  if (!newSFImage) return;
  *sfimage = *newSFImage;
}

/**
 * @brief JNI implementation of SFImage::setValue.
 *
 * @param env JNI environment
 * @param obj SFImage object
 * @param value SFImage to copy value from
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
 * @param env JNI environment
 * @param obj SFImage object
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
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  SFInt32* sfint32 = new SFInt32(static_cast<int>(value));
  if (!sfint32) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(sfint32));
}

/**
 * @brief JNI implementation of ConstSFInt32::getValue.
 *
 * @param env JNI environment
 * @param obj ConstSFInt32 object
 * @return Value of field
 */
jint JNICALL Java_vrml_field_ConstSFInt32_getValue
  (JNIEnv *env, jobject obj)
{
  SFInt32* sfint32 = static_cast<SFInt32*>(getFieldValue(env, obj));
  if (!sfint32) return 0;
  return static_cast<int>(sfint32->get());
}

/**
 * @brief JNI implementation of ConstSFInt32::toString.
 *
 * @param env JNI environment
 * @param obj ConstSFInt32 object
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
 * @param env JNI environment
 * @param obj SFInt32 object
 * @return Value of field
 */
jint JNICALL Java_vrml_field_SFInt32_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFInt32_getValue(env, obj);
}

void JNICALL Java_vrml_field_SFInt32_setValue__I
  (JNIEnv *env, jobject obj, jint value)
{
  SFInt32* sfint32 = static_cast<SFInt32*>(getFieldValue(env, obj));
  if (!sfint32) return;
  sfint32->set(static_cast<int>(value));
}

void JNICALL
Java_vrml_field_SFInt32_setValue__Lvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFInt32* sfint32 = static_cast<SFInt32*>(getFieldValue(env, obj));
  SFInt32* newSFInt32 = static_cast<SFInt32*>(getFieldValue(env, value));
  if (!sfint32 || !newSFInt32) return;
  sfint32->set(newSFInt32->get());
}

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
 * @param env JNI environment
 * @param obj SFInt32 object
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
 * @param env JNI environment
 * @param obj ConstSFNode object
 * @param value BaseNode object to initialize field with.
 */
void JNICALL Java_vrml_field_ConstSFNode_CreateObject
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  SFNode* sfnode;

  if (value == 0)
  {
    // default constructor was called
    sfnode = new SFNode(NodePtr());
  }
  else
  {
    fid = getFid(env, value, "NodePtr", "I");
    if (!fid) return;
    Node* baseNode = reinterpret_cast<Node*>(env->GetIntField(value, fid));
    if (!baseNode) return;
    sfnode = new SFNode(NodePtr(baseNode));
  }

  fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(sfnode));
}

/**
 * @brief JNI implementation of ConstSFNode::getValue.
 *
 * @param env JNI environment
 * @param obj ConstSFNode object
 * @return BaseNode contained in SFNode.
 */
jobject JNICALL Java_vrml_field_ConstSFNode_getValue
  (JNIEnv *env, jobject obj)
{
  SFNode* sfnode = static_cast<SFNode*>(getFieldValue(env, obj));
  if (!sfnode) return 0;
  jclass clazz = env->FindClass("vrml/node/Node");
  jobject jNode = env->AllocObject(clazz);
  jfieldID fid = env->GetFieldID(clazz, "NodePtr", "I");
  if (!fid) return 0;
  env->SetIntField(jNode, fid, reinterpret_cast<int>(sfnode->get().get()));
  return jNode;
}

/**
 * @brief JNI implementation of ConstSFNode::toString.
 *
 * @param env JNI environment
 * @param obj ConstSFNode object
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
 * @param env JNI environment
 * @param obj SFNode object
 * @param value BaseNode object to initialize field with.
 */
void JNICALL Java_vrml_field_SFNode_CreateObject
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_ConstSFNode_CreateObject(env, obj, value);
}

/**
 * @brief JNI implementation of SFNode::getValue.
 *
 * @param env JNI environment
 * @param obj SFNode object
 * @return BaseNode contained in SFNode.
 */
jobject JNICALL Java_vrml_field_SFNode_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFNode_getValue(env, obj);
}

void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFNode* sfnode = static_cast<SFNode*>(getFieldValue(env, obj));
  if (!sfnode) return;
  jfieldID fid = getFid(env, value, "NodePtr", "I");
  if (!fid) return;
  Node* node = reinterpret_cast<Node*>(env->GetIntField(value, fid));
  sfnode->set(NodePtr(node));
}

void JNICALL
Java_vrml_field_SFNode_setValue__Lvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFNode* sfnode = static_cast<SFNode*>(getFieldValue(env, obj));
  SFNode* newSFNode = static_cast<SFNode*>(getFieldValue(env, value));
  if (!sfnode || !newSFNode) return;
  *sfnode = *newSFNode;
}

void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFNode_setValue__Lvrml_field_ConstSFNode_2(env, obj, value);
}

/**
 * @brief JNI implementation of SFNode::toString.
 *
 * @param env JNI environment
 * @param obj SFNode object
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
  SFRotation* sfrotation = new SFRotation(axisX, axisY,
                                          axisZ, angle);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(sfrotation));
}

void JNICALL Java_vrml_field_ConstSFRotation_getValue(JNIEnv * env,
                                                      jobject obj,
                                                      jfloatArray jarr) {
  SFRotation * sfrotation =
    static_cast<SFRotation*>(getFieldValue(env, obj));
  if (!sfrotation) return;
  env->SetFloatArrayRegion(jarr, 0, 4,
                           const_cast<float *>(sfrotation->get()));
}

/**
 * @brief JNI implementation of ConstSFRotation::toString.
 *
 * @param env JNI environment
 * @param obj ConstSFRotation object
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
  SFRotation * sfrotation =
    static_cast<SFRotation*>(getFieldValue(env, obj));
  if (!sfrotation) return;
  jfloat * pjf = env->GetFloatArrayElements(jarr, NULL);
  float rot[4] = { pjf[0], pjf[1], pjf[2], pjf[3] };
  sfrotation->set(rot);
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
void JNICALL Java_vrml_field_SFRotation_setValue__FFFF
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY,
   jfloat axisZ, jfloat angle)
{
  SFRotation* sfrotation = static_cast<SFRotation*>(getFieldValue(env, obj));
  if (!sfrotation) return;
  const float rotation[] = { axisX, axisY, axisZ, angle };
  sfrotation->set(rotation);
}

/**
 * JNI implementation of SFRotation::setValue.
 *
 * @brief env JNI environment
 * @brief obj SFRotation object
 * @brief value New value for field
 */
void JNICALL
Java_vrml_field_SFRotation_setValue__Lvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFRotation* sfrotation = static_cast<SFRotation*>(getFieldValue(env, obj));
  SFRotation* newSFRotation =
    static_cast<SFRotation*>(getFieldValue(env, value));
  if (!sfrotation || !newSFRotation) return;
  sfrotation->set(newSFRotation->get());
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
 * @param env JNI environment
 * @param obj SFRotation object
 * @return String representation of SFRotation.
 */
jstring JNICALL Java_vrml_field_SFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstSFString_CreateObject
  (JNIEnv *env, jobject obj, jstring jstr)
{
  SFString* sfstring;

  // Need to handle null strings
  if (jstr)
  {
    const char* str = env->GetStringUTFChars(jstr, 0);
    sfstring = new SFString(std::string(str));
    env->ReleaseStringUTFChars(jstr, str);
  }
  else
  {
    sfstring = new SFString(std::string());
  }

  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(sfstring));
}

/**
 * @brief JNI implementation of ConstSFString::getValue.
 *
 * @param env JNI environment
 * @param obj ConstSFString object
 * @return Value of field
 */
jstring JNICALL Java_vrml_field_ConstSFString_getValue
  (JNIEnv *env, jobject obj)
{
  SFString* sfstring = static_cast<SFString*>(getFieldValue(env, obj));
  if (!sfstring) return 0;
  return env->NewStringUTF((sfstring->get()).c_str());
}

/**
 * @brief JNI implementation of ConstSFString::toString.
 *
 * @param env JNI environment
 * @param obj ConstSFString object
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
 * @param env JNI environment
 * @param obj SFString object
 * @return Value of field
 */
jstring JNICALL Java_vrml_field_SFString_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFString_getValue(env, obj);
}

void JNICALL Java_vrml_field_SFString_setValue__Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jstring jstr)
{
  SFString* sfstring = static_cast<SFString*>(getFieldValue(env, obj));
  if (!sfstring) return;
  const char *str = env->GetStringUTFChars(jstr, 0);
  sfstring->set(str);
  env->ReleaseStringUTFChars(jstr, str);
}

void JNICALL
Java_vrml_field_SFString_setValue__Lvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFString* sfstring = static_cast<SFString*>(getFieldValue(env, obj));
  SFString* newSFString = static_cast<SFString*>(getFieldValue(env, value));
  if (!sfstring || !newSFString) return;
  sfstring->set(newSFString->get());
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
 * @param env JNI environment
 * @param obj SFString object
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
  SFTime* sftime = new SFTime(time);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(sftime));
}

/**
 * @brief JNI implementation of ConstSFTime::getValue.
 *
 * @param env JNI environment
 * @param obj ConstSFTime object
 * @return Value of field
 */
jdouble JNICALL Java_vrml_field_ConstSFTime_getValue
  (JNIEnv *env, jobject obj)
{
  SFTime* sftime = static_cast<SFTime*>(getFieldValue(env, obj));
  if (!sftime) return 0.0;
  return static_cast<jdouble>(sftime->get());
}

/**
 * @brief JNI implementation of ConstSFTime::toString.
 *
 * @param env JNI environment
 * @param obj ConstSFTime object
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
 * @param env JNI environment
 * @param obj SFTime object
 * @return Value of field
 */
jdouble JNICALL Java_vrml_field_SFTime_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFTime_getValue(env, obj);
}

void JNICALL Java_vrml_field_SFTime_setValue__D
  (JNIEnv *env, jobject obj, jdouble time)
{
  SFTime* sftime = static_cast<SFTime*>(getFieldValue(env, obj));
  if (!sftime) return;
  sftime->set(static_cast<double>(time));
}

void JNICALL
Java_vrml_field_SFTime_setValue__Lvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFTime* sftime = static_cast<SFTime*>(getFieldValue(env, obj));
  SFTime* newSFTime = static_cast<SFTime*>(getFieldValue(env, value));
  if (!sftime || !newSFTime) return;
  sftime->set(newSFTime->get());
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
 * @param env JNI environment
 * @param obj SFTime object
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
  SFVec2f* sfvec2f = new SFVec2f(x, y);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(sfvec2f));
}

/**
 * @brief JNI implementation of ConstSFVec2f::getValue.
 *
 * @param env JNI environment
 * @param obj ConstSFVec2f object
 * @param jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_ConstSFVec2f_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr) {
  SFVec2f * sfvec2f = static_cast<SFVec2f *>(getFieldValue(env, obj));
  if (!sfvec2f) return;
  env->SetFloatArrayRegion(jarr, 0, 2, const_cast<float *>(sfvec2f->get()));
}

/**
 * @brief JNI implementation of ConstSFVec2f::getX.
 *
 * @param env JNI environment
 * @param obj ConstSFVec2f object
 * @return X-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec2f_getX
  (JNIEnv *env, jobject obj)
{
  SFVec2f* sfvec2f = static_cast<SFVec2f*>(getFieldValue(env, obj));
  if (!sfvec2f) return 0.0;
  return static_cast<jfloat>(sfvec2f->getX());
}

/**
 * @brief JNI implementation of ConstSFVec2f::getY.
 *
 * @param env JNI environment
 * @param obj ConstSFVec2f object
 * @return Y-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec2f_getY
  (JNIEnv *env, jobject obj)
{
  SFVec2f* sfvec2f = static_cast<SFVec2f*>(getFieldValue(env, obj));
  if (!sfvec2f) return 0.0;
  return static_cast<jfloat>(sfvec2f->getY());
}

/**
 * @brief JNI implementation of ConstSFVec2f::toString.
 *
 * @param env JNI environment
 * @param obj ConstSFVec2f object
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
 * @param env JNI environment
 * @param obj SFVec2f object
 * @param jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_SFVec2f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFVec2f_getValue(env, obj, jarr);
}

/**
 * @brief JNI implementation of SFVec2f::getX.
 *
 * @param env JNI environment
 * @param obj SFVec2f object
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
 * @param env JNI environment
 * @param obj SFVec2f object
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
  SFVec2f * sfvec2f = static_cast<SFVec2f*>(getFieldValue(env, obj));
  if (!sfvec2f) return;
  jfloat * pjf = env->GetFloatArrayElements(jarr, NULL);
  float vec[2] = { pjf[0], pjf[1] };
  sfvec2f->set(vec);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_SFVec2f_setValue__FF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  SFVec2f* sfvec2f = static_cast<SFVec2f*>(getFieldValue(env, obj));
  if (!sfvec2f) return;
  const float vec2f[] = { x, y };
  sfvec2f->set(vec2f);
}

void JNICALL
Java_vrml_field_SFVec2f_setValue__Lvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFVec2f* sfvec2f = static_cast<SFVec2f*>(getFieldValue(env, obj));
  SFVec2f* newSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, value));
  if (!sfvec2f || !newSFVec2f) return;
  sfvec2f->set(newSFVec2f->get());
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
 * @param env JNI environment
 * @param obj SFVec2f object
 * @return String representation of SFVec2f.
 */
jstring JNICALL Java_vrml_field_SFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstSFVec3f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  SFVec3f* sfvec3f = new SFVec3f(x, y, z);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(sfvec3f));
}

/**
 * @brief JNI implementation of ConstSFVec3f::getValue.
 *
 * @param env JNI environment
 * @param obj ConstSFVec3f object
 * @param jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_ConstSFVec3f_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr) {
  SFVec3f * sfvec3f = static_cast<SFVec3f*>(getFieldValue(env, obj));
  if (!sfvec3f) return;
  env->SetFloatArrayRegion(jarr, 0, 3, const_cast<float *>(sfvec3f->get()));
}

/**
 * @brief JNI implementation of ConstSFVec3f::getX.
 *
 * @param env JNI environment
 * @param obj ConstSFVec3f object
 * @return X-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec3f_getX(JNIEnv * env, jobject obj) {
  SFVec3f * sfvec3f = static_cast<SFVec3f *>(getFieldValue(env, obj));
  if (!sfvec3f) return 0.0;
  return sfvec3f->getX();
}

/**
 * @brief JNI implementation of ConstSFVec3f::getY.
 *
 * @param env JNI environment
 * @param obj ConstSFVec3f object
 * @return Y-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec3f_getY(JNIEnv * env, jobject obj) {
  SFVec3f * sfvec3f = static_cast<SFVec3f *>(getFieldValue(env, obj));
  if (!sfvec3f) return 0.0;
  return sfvec3f->getY();
}

/**
 * @brief JNI implementation of ConstSFVec3f::getZ.
 *
 * @param env JNI environment
 * @param obj ConstSFVec3f object
 * @return Z-component of vector
 */
jfloat JNICALL Java_vrml_field_ConstSFVec3f_getZ(JNIEnv * env, jobject obj) {
  SFVec3f * sfvec3f = static_cast<SFVec3f *>(getFieldValue(env, obj));
  if (!sfvec3f) return 0.0;
  return sfvec3f->getZ();
}

/**
 * @brief JNI implementation of ConstSFVec3f::toString.
 *
 * @param env JNI environment
 * @param obj ConstSFVec3f object
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
 * @param env JNI environment
 * @param obj SFVec3f object
 * @param jarr Value of field as a float array
 */
void JNICALL Java_vrml_field_SFVec3f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFVec3f_getValue(env, obj, jarr);
}

/**
 * @brief JNI implementation of SFVec3f::getX.
 *
 * @param env JNI environment
 * @param obj SFVec3f object
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
 * @param env JNI environment
 * @param obj SFVec3f object
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
 * @param env JNI environment
 * @param obj SFVec3f object
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
  SFVec3f * sfvec3f = static_cast<SFVec3f *>(getFieldValue(env, obj));
  if (!sfvec3f) return;
  jfloat * pjf = env->GetFloatArrayElements(jarr, NULL);
  float vec[3] = { pjf[0], pjf[1], pjf[2] };
  sfvec3f->set(vec);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_SFVec3f_setValue__FFF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  SFVec3f* sfvec3f = static_cast<SFVec3f*>(getFieldValue(env, obj));
  if (!sfvec3f) return;
  const float vec3f[] = { x, y, z };
  sfvec3f->set(vec3f);
}

void JNICALL
Java_vrml_field_SFVec3f_setValue__Lvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFVec3f* sfvec3f = static_cast<SFVec3f*>(getFieldValue(env, obj));
  SFVec3f* newSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, value));
  if (!sfvec3f || !newSFVec3f) return;
  sfvec3f->set(newSFVec3f->get());
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
 * @param env JNI environment
 * @param obj SFVec3f object
 * @return String representation of SFVec3f.
 */
jstring JNICALL Java_vrml_field_SFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstMFColor_CreateObject___3_3F(JNIEnv * env,
                                                              jobject obj,
                                                              jobjectArray jarr)
{
    try {
        std::auto_ptr<MFColor> mfcolor(new MFColor(env->GetArrayLength(jarr)));
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
        std::auto_ptr<MFColor> mfcolor(new MFColor(size));
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
  MFColor * mfcolor = static_cast<MFColor *>(getFieldValue(env, obj));
  if (!mfcolor) return 0;
  return mfcolor->getLength();
}

void JNICALL Java_vrml_field_ConstMFColor_getValue___3_3F(JNIEnv * env,
                                                          jobject obj,
                                                          jobjectArray jarr)
{
    const MFColor * const mfcolor =
            static_cast<MFColor *>(getFieldValue(env, obj));
    if (!mfcolor) return;
    for (size_t i = 0; i < mfcolor->getLength(); ++i) {
        jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
        if (!element) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
        env->SetFloatArrayRegion(element, 0, 3,
                                 const_cast<jfloat *>(mfcolor->getElement(i)));
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
    const MFColor * const mfcolor =
            static_cast<MFColor *>(getFieldValue(env, obj));
    if (!mfcolor) return;
    for (size_t i = 0; i < mfcolor->getLength(); ++i) {
        env->SetFloatArrayRegion(jarr, i * 3, 3,
                                 const_cast<jfloat *>(mfcolor->getElement(i)));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

void JNICALL Java_vrml_field_ConstMFColor_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  MFColor* mfcolor = static_cast<MFColor*>(getFieldValue(env, obj));
  if (!mfcolor) return;
  env->SetFloatArrayRegion(jarr, 0, 3, const_cast<jfloat*>(
                                         mfcolor->getElement(element)));
}

void JNICALL
Java_vrml_field_ConstMFColor_get1Value__ILvrml_field_SFColor_2(
        JNIEnv * env,
        jobject obj,
        jint element,
        jobject sfcolorObj)
{
    const MFColor * const mfcolor =
            static_cast<MFColor *>(getFieldValue(env, obj));
    if (!mfcolor) return;
    if (element < 0 || element >= mfcolor->getLength()) {
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
    SFColor * const sfcolor =
            static_cast<SFColor *>(getFieldValue(env, sfcolorObj));
    if (!sfcolor) return;
    sfcolor->set(mfcolor->getElement(element));
}

/**
 * @brief JNI implementation of ConstMFColor::toString.
 *
 * @param env JNI environment
 * @param obj ConstMFColor object
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
  MFColor * const mfcolor = static_cast<MFColor *>(getFieldValue(env, obj));
  if (!mfcolor) return;
  mfcolor->setLength(0); // Shouldn't throw bad_alloc.
}

void JNICALL Java_vrml_field_MFColor_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFColor* const mfcolor = static_cast<MFColor*>(getFieldValue(env, obj));
  if (!mfcolor) return;
  mfcolor->removeElement(index);
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
        MFColor * const mfcolor =
                static_cast<MFColor *>(getFieldValue(env, obj));
        if (!mfcolor) return;
        mfcolor->setLength(env->GetArrayLength(value)); // throws bad_alloc
        for (size_t i = 0; i < mfcolor->getLength(); ++i) {
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
            const float color[3] = { c[0], c[1], c[2] };
            env->ReleaseFloatArrayElements(element, c, JNI_ABORT);
            mfcolor->setElement(i, color);
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
        MFColor * const mfcolor =
                static_cast<MFColor *>(getFieldValue(env, obj));
        if (!mfcolor) return;
        mfcolor->setLength(size); // throws bad_alloc
        jfloat * const colors = env->GetFloatArrayElements(value, 0);
        if (!colors) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (size_t i = 0; i < mfcolor->getLength(); ++i) {
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
            const float color[3] = { colors[3 * i],
                                     colors[3 * i + 1],
                                     colors[3 * i + 2] };
            mfcolor->setElement(i, color);
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
    MFColor * const mfcolor = static_cast<MFColor *>(getFieldValue(env, obj));
    const MFColor * const newMFColor =
      static_cast<MFColor *>(getFieldValue(env, value));
    if (!mfcolor || !newMFColor) return;
    try {
        *mfcolor = *newMFColor; // Throws bad_alloc.
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
Java_vrml_field_MFColor_set1Value__ILvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jint index, jobject sfcolor)
{
  MFColor* mfcolor = static_cast<MFColor*>(getFieldValue(env, obj));
  SFColor* sfcolorptr = static_cast<SFColor*>(getFieldValue(env, sfcolor));
  if (!mfcolor || !sfcolorptr) return;
  mfcolor->setElement(index, sfcolorptr->get());
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
  MFColor* mfcolor = static_cast<MFColor*>(getFieldValue(env, obj));
  if (!mfcolor) return;
  float *p = const_cast<float*>(mfcolor->getElement(index));
  p[0] = red;
  p[1] = green;
  p[2] = blue;
}

/**
 * @brief JNI implementation of MFColor::addValue.
 *
 * @param env JNI environment
 * @param obj MFColor object
 * @param sfcolorObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFColor_addValue__Lvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jobject sfcolorObj)
{
  MFColor* mfcolor = static_cast<MFColor*>(getFieldValue(env, obj));
  SFColor* sfcolor = static_cast<SFColor*>(getFieldValue(env, sfcolorObj));
  if (!mfcolor || !sfcolor) return;
  size_t curLength = mfcolor->getLength();
  mfcolor->setLength(curLength + 1);
  mfcolor->setElement(curLength, sfcolor->get());
}

/**
 * @brief JNI implementation of MFColor::addValue.
 *
 * @param env JNI environment
 * @param obj MFColor object
 * @param sfcolor Value to add to end of list
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
 * @param env JNI environment
 * @param obj MFColor object
 * @param red Red component of color to add to end of list
 * @param green Green component of color to add to end of list
 * @param blue Blue component of color to add to end of list
 */
void JNICALL Java_vrml_field_MFColor_addValue__FFF
  (JNIEnv *env, jobject obj, jfloat red, jfloat green, jfloat blue)
{
  MFColor* mfcolor = static_cast<MFColor*>(getFieldValue(env, obj));
  if (!mfcolor) return;
  float newData[] = { red, green, blue };
  size_t curLength = mfcolor->getLength();
  mfcolor->setLength(curLength + 1);
  mfcolor->setElement(curLength, newData);
}

void JNICALL
Java_vrml_field_MFColor_insertValue__ILvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jint index, jobject sfcolor)
{
  MFColor* mfcolor = static_cast<MFColor*>(getFieldValue(env, obj));
  SFColor* sfcolorptr = static_cast<SFColor*>(getFieldValue(env, sfcolor));
  if (!mfcolor || !sfcolorptr) return;
  mfcolor->insertElement(index, sfcolorptr->get());
}

void JNICALL
Java_vrml_field_MFColor_insertValue__ILvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jint index, jobject sfcolor)
{
  Java_vrml_field_MFColor_insertValue__ILvrml_field_ConstSFColor_2
    (env, obj, index, sfcolor);
}

void JNICALL Java_vrml_field_MFColor_insertValue__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat red, jfloat green,
   jfloat blue)
{
  MFColor* mfcolor = static_cast<MFColor*>(getFieldValue(env, obj));
  if (!mfcolor) return;
  float newData[3];
  newData[0] = red;
  newData[1] = green;
  newData[2] = blue;
  mfcolor->insertElement(index, newData);
}

/**
 * @brief JNI implementation of MFColor::toString.
 *
 * @param env JNI environment
 * @param obj MFColor object
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
  jfloat *pjf = env->GetFloatArrayElements(value, NULL);
  MFFloat* mffloat = new MFFloat(size, const_cast<float const*>(pjf));
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(mffloat));
}

jint JNICALL Java_vrml_field_ConstMFFloat_getSize(JNIEnv * env, jobject obj) {
  MFFloat * mffloat = static_cast<MFFloat *>(getFieldValue(env, obj));
  if (!mffloat) return 0;
  return mffloat->getLength();
}

void JNICALL Java_vrml_field_ConstMFFloat_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jfloatArray jarr)
{
  const MFFloat * const mffloat =
    static_cast<MFFloat *>(getFieldValue(env, obj));
  if (!mffloat) return;
  const size_t size = mffloat->getLength();
  if (size > 0) {
    env->SetFloatArrayRegion(jarr, 0, size,
                             const_cast<jfloat *>(&mffloat->getElement(0)));
  }
}

jfloat JNICALL Java_vrml_field_ConstMFFloat_get1Value(JNIEnv * env,
                                                      jobject obj,
                                                      jint index)
{
  MFFloat * mffloat = static_cast<MFFloat *>(getFieldValue(env, obj));
  if (!mffloat) return 0.0;
  return mffloat->getElement(index);
}

/**
 * @brief JNI implementation of ConstMFFloat::toString.
 *
 * @param env JNI environment
 * @param obj ConstMFFloat object
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
  MFFloat * const mffloat = static_cast<MFFloat *>(getFieldValue(env, obj));
  if (!mffloat) return;
  mffloat->setLength(0); // Shouldn't throw bad_alloc.
}

void JNICALL Java_vrml_field_MFFloat_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFFloat* mffloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  if (!mffloat) return;
  mffloat->removeElement(index);
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
        MFFloat * const mffloat =
                static_cast<MFFloat *>(getFieldValue(env, obj));
        if (!mffloat) return;
        mffloat->setLength(size); // Throws bad_alloc.
        jfloat * const floats = env->GetFloatArrayElements(value, 0);
        if (!floats) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (size_t i = 0; i < mffloat->getLength(); ++i) {
            if (!(i < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                        env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than values.length");
                break;
            }
            mffloat->setElement(i, floats[i]);
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
    MFFloat * const mffloat = static_cast<MFFloat *>(getFieldValue(env, obj));
    const MFFloat * const newMFFloat =
      static_cast<MFFloat*>(getFieldValue(env, value));
    if (!mffloat || !newMFFloat) return;
    try {
        *mffloat = *newMFFloat; // Throws bad_alloc.
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
  MFFloat * mffloat = static_cast<MFFloat *>(getFieldValue(env, obj));
  mffloat->setElement(index, value);
}

void JNICALL
Java_vrml_field_MFFloat_set1Value__ILvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject sffloatObj)
{
  MFFloat* mffloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  SFFloat* sffloat = static_cast<SFFloat*>(getFieldValue(env, sffloatObj));
  if (!mffloat || !sffloat) return;
  mffloat->setElement(index, sffloat->get());
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
 * @param env JNI environment
 * @param obj MFFloat object
 * @param value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFFloat_addValue__F
  (JNIEnv *env, jobject obj, jfloat value)
{
  MFFloat* mffloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  if (!mffloat) return;
  size_t curLength = mffloat->getLength();
  mffloat->setLength(curLength + 1);
  mffloat->setElement(curLength, value);
}

/**
 * @brief JNI implementation of MFFloat::addValue.
 *
 * @param env JNI environment
 * @param obj MFFloat object
 * @param value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFFloat_addValue__Lvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFFloat* mffloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  SFFloat* sffloat = static_cast<SFFloat*>(getFieldValue(env, value));
  if (!mffloat || !sffloat) return;
  size_t curLength = mffloat->getLength();
  mffloat->setLength(curLength + 1);
  mffloat->setElement(curLength, sffloat->get());
}

/**
 * @brief JNI implementation of MFFloat::addValue.
 *
 * @param env JNI environment
 * @param obj MFFloat object
 * @param value Value to add to end of list
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
  MFFloat * mffloat = static_cast<MFFloat *>(getFieldValue(env, obj));
  if (!mffloat) return;
  mffloat->insertElement(index, value);
}

void JNICALL
Java_vrml_field_MFFloat_insertValue__ILvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  MFFloat* mffloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  SFFloat* sffloat = static_cast<SFFloat*>(getFieldValue(env, value));
  if (!mffloat || !sffloat) return;
  mffloat->insertElement(index, sffloat->get());
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
 * @param env JNI environment
 * @param obj MFFloat object
 * @return String representation of MFFloat.
 */
jstring JNICALL Java_vrml_field_MFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

void JNICALL Java_vrml_field_ConstMFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint size, jintArray value)
{
  jint * pji = env->GetIntArrayElements(value, NULL);
  std::vector<long> longVec(size);
  std::copy(pji, pji + size, longVec.begin());
  env->ReleaseIntArrayElements(value, pji, JNI_ABORT);
  MFInt32* pMFInt32 = new MFInt32(size, &longVec[0]);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(pMFInt32));
}

jint JNICALL Java_vrml_field_ConstMFInt32_getSize(JNIEnv * env, jobject obj) {
  MFInt32 * mfint32 = static_cast<MFInt32 *>(getFieldValue(env, obj));
  if (!mfint32) return 0;
  return mfint32->getLength();
}

void JNICALL Java_vrml_field_ConstMFInt32_getValue(JNIEnv * env,
                                                   jobject obj,
                                                   jintArray jarr)
{
    const MFInt32 * const mfint32 =
            static_cast<MFInt32*>(getFieldValue(env, obj));
    if (!mfint32) return;
    const size_t size = mfint32->getLength();
    if (size > 0) {
        try {
            std::vector<jint> jintVec(&mfint32->getElement(0),
                                      &mfint32->getElement(0) + size); // Throws bad_alloc.
            env->SetIntArrayRegion(jarr, 0, size, &jintVec[0]);
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
}

jint JNICALL Java_vrml_field_ConstMFInt32_get1Value(JNIEnv * env,
                                                    jobject obj,
                                                    jint index)
{
  MFInt32 * mfint32 = static_cast<MFInt32 *>(getFieldValue(env, obj));
  if (!mfint32) return 0;
  return mfint32->getElement(index);
}

/**
 * @brief JNI implementation of ConstMFInt32::toString.
 *
 * @param env JNI environment
 * @param obj ConstMFInt32 object
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
  MFInt32 * const mfint32 = static_cast<MFInt32 *>(getFieldValue(env, obj));
  if (!mfint32) return;
  mfint32->setLength(0); // Shouldn't throw bad_alloc
}

void JNICALL Java_vrml_field_MFInt32_delete
  (JNIEnv *env, jobject obj, jint n)
{
  MFInt32* mfint32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  if (!mfint32) return;
  mfint32->removeElement(n);
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
        MFInt32 * const mfint32 =
                static_cast<MFInt32 *>(getFieldValue(env, obj));
        if (!mfint32) return;
        mfint32->setLength(size); // Throws bad_alloc.
        jint * ints = env->GetIntArrayElements(value, 0);
        if (!ints) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (size_t i = 0; i < size; ++i) {
            if (!(i < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                        env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than values.length");
                break;
            }
            mfint32->setElement(i, ints[i]);
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
    MFInt32 * const mfint32 = static_cast<MFInt32 *>(getFieldValue(env, obj));
    const MFInt32 * const newMFInt32 =
      static_cast<MFInt32 *>(getFieldValue(env, value));
    if (!mfint32 || !newMFInt32) return;
    try {
        *mfint32 = *newMFInt32; // Throws bad_alloc.
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

void JNICALL Java_vrml_field_MFInt32_set1Value__II
  (JNIEnv *env, jobject obj, jint index, jint value)
{
  MFInt32* mfint32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  if (!mfint32) return;
  mfint32->setElement(index, value);
}

void JNICALL
Java_vrml_field_MFInt32_set1Value__ILvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject sfint)
{
  MFInt32* mfint32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  SFInt32* sfint32 = static_cast<SFInt32*>(getFieldValue(env, sfint));
  if (!mfint32 || !sfint32) return;
  mfint32->setElement(index, sfint32->get());
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
 * @param env JNI environment
 * @param obj MFInt32 object
 * @param value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFInt32_addValue__I(JNIEnv * env,
                                                 jobject obj,
                                                 jint value)
{
  MFInt32 * mfint32 = static_cast<MFInt32 *>(getFieldValue(env, obj));
  if (!mfint32) return;
  size_t curLength = mfint32->getLength();
  mfint32->setLength(curLength + 1);
  mfint32->setElement(curLength, value);
}

/**
 * @brief JNI implementation of MFInt32::addValue.
 *
 * @param env JNI environment
 * @param obj MFInt32 object
 * @param value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFInt32_addValue__Lvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFInt32* mfint32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  SFInt32* sfint32 = static_cast<SFInt32*>(getFieldValue(env, value));
  if (!mfint32 || !sfint32) return;
  size_t curLength = mfint32->getLength();
  mfint32->setLength(curLength + 1);
  mfint32->setElement(curLength, sfint32->get());
}

/**
 * @brief JNI implementation of MFInt32::addValue.
 *
 * @param env JNI environment
 * @param obj MFInt32 object
 * @param value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFInt32_addValue__Lvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFInt32_addValue__Lvrml_field_ConstSFInt32_2(env, obj,
                                                               value);
}

void JNICALL Java_vrml_field_MFInt32_insertValue__II
  (JNIEnv *env, jobject obj, jint index, jint value)
{
  MFInt32* mfint32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  if (!mfint32) return;
  mfint32->insertElement(index, value);
}

void JNICALL
Java_vrml_field_MFInt32_insertValue__ILvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  MFInt32* mfint32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  SFInt32* sfint32 = static_cast<SFInt32*>(getFieldValue(env, value));
  if (!mfint32 || !sfint32) return;
  mfint32->insertElement(index, sfint32->get());
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
 * @param env JNI environment
 * @param obj MFInt32 object
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
  MFNode* mfnode;
  jfieldID fid;

  if (size > 0)
  {
    NodePtr* pNodePtrArray = new NodePtr[size];

    for (int i = 0; i < size; i++)
    {
      jobject jNode = env->GetObjectArrayElement(jarr, i);
      fid = getFid(env, jNode, "NodePtr", "I");
      if (!fid) return;
      Node* node = reinterpret_cast<Node*>(env->GetIntField(jNode, fid));
      if (!node) return;
      pNodePtrArray[i] = NodePtr(node);
    }

    mfnode = new MFNode(size, pNodePtrArray);
    delete[] pNodePtrArray;
  }
  else
  {
    mfnode = new MFNode();
  }

  fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(mfnode));
}

jint JNICALL Java_vrml_field_ConstMFNode_getSize(JNIEnv * env, jobject obj) {
  MFNode * mfnode = static_cast<MFNode *>(getFieldValue(env, obj));
  if (!mfnode) return 0;
  return mfnode->getLength();
}

void JNICALL Java_vrml_field_ConstMFNode_getValue
  (JNIEnv *env, jobject obj, jobjectArray basenode)
{
  MFNode* mfnode = static_cast<MFNode*>(getFieldValue(env, obj));
  if (!mfnode) return;
  int arraySize = mfnode->getLength();
  jclass clazz = env->FindClass("vrml/node/Node");
  jobject jNode;
  jfieldID fid = env->GetFieldID(clazz, "NodePtr", "I");
  if (!fid) return;

  for (int pos = 0; pos < arraySize; pos++)
  {
    jNode = env->AllocObject(clazz);
    env->SetIntField(jNode, fid,
                     reinterpret_cast<int>(mfnode->getElement(pos).get()));
    env->SetObjectArrayElement(basenode, pos, jNode);
  }
}

/**
 * @brief JNI implementation of ConstMFNode::get1Value.
 *
 * @param env JNI environment
 * @param obj ConstMFNode object
 * @param index Position of desired element
 * @return Element at specified position
 */
jobject JNICALL Java_vrml_field_ConstMFNode_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  MFNode* mfnode = static_cast<MFNode*>(getFieldValue(env, obj));
  if (!mfnode) return 0;
  jclass clazz = env->FindClass("vrml/node/Node");
  jobject jNode = env->AllocObject(clazz);
  jfieldID fid = env->GetFieldID(clazz, "NodePtr", "I");
  if (!fid) return 0;
  env->SetIntField(jNode, fid,
                   reinterpret_cast<int>(mfnode->getElement(index).get()));
  return jNode;
}

/**
 * @brief JNI implementation of ConstMFNode::toString.
 *
 * @param env JNI environment
 * @param obj ConstMFNode object
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

void JNICALL Java_vrml_field_MFNode_clear
  (JNIEnv *env, jobject obj)
{
  MFNode* mfnode = static_cast<MFNode*>(getFieldValue(env, obj));
  if (!mfnode) return;
  mfnode->clear();
}

void JNICALL Java_vrml_field_MFNode_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFNode* mfnode = static_cast<MFNode*>(getFieldValue(env, obj));
  if (!mfnode) return;
  mfnode->removeElement(index);
}

void JNICALL Java_vrml_field_MFNode_getValue
  (JNIEnv *env, jobject obj, jobjectArray basenode)
{
  Java_vrml_field_ConstMFNode_getValue(env, obj, basenode);
}

/**
 * @brief JNI implementation of MFNode::get1Value.
 *
 * @param env JNI environment
 * @param obj MFNode object
 * @param index Position of desired element
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
 * @param env JNI environment.
 * @param obj MFNode object.
 * @param size Number of elements to set
 * @param basenode Array of BaseNodes to set value of field to
 *
 * @todo Implement me!
 */
void JNICALL Java_vrml_field_MFNode_setValue__I_3Lvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jint size, jobjectArray basenode)
{
}

void JNICALL Java_vrml_field_MFNode_setValue__Lvrml_field_MFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFNode* mfnode = static_cast<MFNode*>(getFieldValue(env, obj));
  MFNode* newMFNode = static_cast<MFNode*>(getFieldValue(env, value));
  if (!mfnode || !newMFNode) return;
  *mfnode = *newMFNode;
}

void JNICALL
Java_vrml_field_MFNode_setValue__Lvrml_field_ConstMFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFNode_setValue__Lvrml_field_MFNode_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFNode_set1Value__ILvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jint pos, jobject value)
{
  MFNode* mfnode = static_cast<MFNode*>(getFieldValue(env, obj));
  if (!mfnode) return;
  jfieldID fid = getFid(env, value, "NodePtr", "I");
  if (!fid) return;
  Node* newNode = reinterpret_cast<Node*>(env->GetIntField(value, fid));
  if (!newNode) return;
  mfnode->setElement(pos, NodePtr(newNode));
}

void JNICALL
Java_vrml_field_MFNode_set1Value__ILvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject sfnodeObj)
{
  MFNode* mfnode = static_cast<MFNode*>(getFieldValue(env, obj));
  SFNode* sfnode = static_cast<SFNode*>(getFieldValue(env, sfnodeObj));
  if (!mfnode || !sfnode) return;
  mfnode->setElement(index, sfnode->get());
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
 * @param env JNI environment
 * @param obj MFNode object
 * @param value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFNode_addValue__Lvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFNode* mfnode = static_cast<MFNode*>(getFieldValue(env, obj));
  if (!mfnode) return;
  jfieldID fid = getFid(env, value, "NodePtr", "I");
  if (!fid) return;
  Node* newNode = reinterpret_cast<Node*>(env->GetIntField(value, fid));
  if (!newNode) return;
  mfnode->addNode(NodePtr(newNode));
}

/**
 * @brief JNI implementation of MFNode::addValue.
 *
 * @param env JNI environment
 * @param obj MFNode object
 * @param value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFNode_addValue__Lvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFNode* mfnode = static_cast<MFNode*>(getFieldValue(env, obj));
  SFNode* sfnode = static_cast<SFNode*>(getFieldValue(env, value));
  if (!mfnode || !sfnode) return;
  mfnode->addNode(sfnode->get());
}

/**
 * @brief JNI implementation of MFNode::addValue.
 *
 * @param env JNI environment
 * @param obj MFNode object
 * @param value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFNode_addValue__Lvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFNode_addValue__Lvrml_field_ConstSFNode_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFNode_insertValue__ILvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  MFNode* mfnode = static_cast<MFNode*>(getFieldValue(env, obj));
  if (!mfnode) return;
  jfieldID fid = getFid(env, value, "NodePtr", "I");
  if (!fid) return;
  Node* newNode = reinterpret_cast<Node*>(env->GetIntField(value, fid));
  if (!newNode) return;
  mfnode->insertElement(index, NodePtr(newNode));
}

void JNICALL
Java_vrml_field_MFNode_insertValue__ILvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  MFNode* mfnode = static_cast<MFNode*>(getFieldValue(env, obj));
  SFNode* sfnode = static_cast<SFNode*>(getFieldValue(env, value));
  if (!mfnode || !sfnode) return;
  mfnode->insertElement(index, sfnode->get());
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
 * @param env JNI environment
 * @param obj MFNode object
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
        std::auto_ptr<MFRotation>
                mfrotation(new MFRotation(env->GetArrayLength(jarr)));
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
        std::auto_ptr<MFRotation> mfrotation(new MFRotation(size));
        jfieldID fid = getFid(env, obj, "FieldPtr", "I");
        if (!fid) return;
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
                                                     jobject obj) {
    MFRotation * mfrotation =
      static_cast<MFRotation *>(getFieldValue(env, obj));
    if (!mfrotation) return 0;
    return mfrotation->getLength();
}

void JNICALL Java_vrml_field_ConstMFRotation_getValue___3_3F(JNIEnv * env,
                                                             jobject obj,
                                                             jobjectArray jarr)
{
    const MFRotation * const mfrotation =
            static_cast<MFRotation *>(getFieldValue(env, obj));
    for (size_t i = 0; i < mfrotation->getLength(); ++i) {
        jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
        if (!element) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
        env->SetFloatArrayRegion(element, 0, 4,
                                 const_cast<jfloat *>(mfrotation->getElement(i)));
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
    const MFRotation * const mfrotation =
            static_cast<MFRotation *>(getFieldValue(env, obj));
    for (size_t i = 0; i < mfrotation->getLength(); ++i) {
        env->SetFloatArrayRegion(jarr, i * 4, 4,
                                 const_cast<jfloat *>(mfrotation->getElement(i)));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

void JNICALL Java_vrml_field_ConstMFRotation_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  MFRotation* mfrotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  env->SetFloatArrayRegion(jarr, 0, 4, const_cast<jfloat*>(
                                         mfrotation->getElement(element)));
}

void JNICALL
Java_vrml_field_ConstMFRotation_get1Value__ILvrml_field_SFRotation_2(
        JNIEnv * env,
        jobject obj,
        jint element,
        jobject sfrotationObj)
{
    const MFRotation * const mfrotation =
            static_cast<MFRotation *>(getFieldValue(env, obj));
    if (!mfrotation) return;
    if (element < 0 || element >= mfrotation->getLength()) {
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
    SFRotation * const sfrotation =
            static_cast<SFRotation *>(getFieldValue(env, sfrotationObj));
    if (!sfrotation) return;
    sfrotation->set(mfrotation->getElement(element));
}

/**
 * @brief JNI implementation of ConstMFRotation::toString.
 *
 * @param env JNI environment
 * @param obj ConstMFRotation object
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
  MFRotation * const mfrotation =
    static_cast<MFRotation *>(getFieldValue(env, obj));
  if (!mfrotation) return;
  mfrotation->setLength(0); // Shouldn't throw bad_alloc.
}

void JNICALL Java_vrml_field_MFRotation_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFRotation* mfrotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  if (!mfrotation) return;
  mfrotation->removeElement(index);
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
        MFRotation * const mfrotation =
                static_cast<MFRotation *>(getFieldValue(env, obj));
        if (!mfrotation) return;
        mfrotation->setLength(env->GetArrayLength(value));
        for (size_t i = 0; i < mfrotation->getLength(); ++i) {
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
            const float rot[4] = { r[0], r[1], r[2], r[3] };
            env->ReleaseFloatArrayElements(element, r, JNI_ABORT);
            mfrotation->setElement(i, rot);
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
        MFRotation * const mfrotation =
                static_cast<MFRotation *>(getFieldValue(env, obj));
        if (!mfrotation) return;
        mfrotation->setLength(size);
        jfloat * const r = env->GetFloatArrayElements(value, 0);
        if (!r) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (size_t i = 0; i < size; ++i) {
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
            const float rot[4] = { r[i * 4],
                                   r[i * 4 + 1],
                                   r[i * 4 + 2],
                                   r[i * 4 + 3] };
            mfrotation->setElement(i, rot);
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
    MFRotation * const mfrotation =
            static_cast<MFRotation *>(getFieldValue(env, obj));
    const MFRotation * const newMFRotation =
            static_cast<MFRotation *>(getFieldValue(env, value));
    if (!mfrotation || !newMFRotation) return;
    try {
        *mfrotation = *newMFRotation; // Throws bad_alloc.
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
Java_vrml_field_MFRotation_set1Value__ILvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotationObj)
{
  MFRotation* mfrotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  SFRotation* sfrotation =
    static_cast<SFRotation*>(getFieldValue(env, sfrotationObj));
  if (!mfrotation || !sfrotation) return;
  mfrotation->setElement(index, sfrotation->get());
}

void JNICALL
Java_vrml_field_MFRotation_set1Value__ILvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotation)
{
  Java_vrml_field_MFRotation_set1Value__ILvrml_field_ConstSFRotation_2
    (env, obj, index, sfrotation);
}

void JNICALL Java_vrml_field_MFRotation_set1Value__IFFFF
  (JNIEnv *env, jobject obj, jint index, jfloat axisX, jfloat axisY,
   jfloat axisZ, jfloat angle)
{
  MFRotation* mfrotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  if (!mfrotation) return;
  float *p = const_cast<float*>(mfrotation->getElement(index));
  p[0] = axisX;
  p[1] = axisY;
  p[2] = axisZ;
  p[3] = angle;
}

/**
 * @brief JNI implementation of MFRotation::addValue.
 *
 * @param env JNI environment
 * @param obj MFRotation object
 * @param sfrotationObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFRotation_addValue__Lvrml_field_ConstSFRotation_2(
        JNIEnv * env,
        jobject obj,
        jobject sfrotationObj)
{
  MFRotation * mfrotation = static_cast<MFRotation *>(getFieldValue(env, obj));
  SFRotation * sfrotation =
    static_cast<SFRotation *>(getFieldValue(env, sfrotationObj));
  if (!mfrotation || !sfrotation) return;
  size_t curLength = mfrotation->getLength();
  mfrotation->setLength(curLength + 1);
  mfrotation->setElement(curLength, sfrotation->get());
}

/**
 * @brief JNI implementation of MFRotation::addValue.
 *
 * @param env JNI environment
 * @param obj MFRotation object
 * @param sfrotation Value to add to end of list
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
 * @param env JNI environment
 * @param obj MFRotation object
 * @param axisX X-component of value to add to end of list
 * @param axisY Y-component of value to add to end of list
 * @param axisZ Z-component of value to add to end of list
 * @param angle Angle of value to add to end of list
 */
void JNICALL Java_vrml_field_MFRotation_addValue__FFFF
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY,
   jfloat axisZ, jfloat angle)
{
  MFRotation* mfrotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  if (!mfrotation) return;
  float newData[] = { axisX, axisY, axisZ, angle };
  size_t curLength = mfrotation->getLength();
  mfrotation->setLength(curLength + 1);
  mfrotation->setElement(curLength, newData);
}

void JNICALL
Java_vrml_field_MFRotation_insertValue__ILvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotationObj)
{
  MFRotation* mfrotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  SFRotation* sfrotation =
    static_cast<SFRotation*>(getFieldValue(env, sfrotationObj));
  if (!mfrotation || !sfrotation) return;
  mfrotation->insertElement(index, sfrotation->get());
}

void JNICALL
Java_vrml_field_MFRotation_insertValue__ILvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotation)
{
  Java_vrml_field_MFRotation_insertValue__ILvrml_field_ConstSFRotation_2
    (env, obj, index, sfrotation);
}

void JNICALL Java_vrml_field_MFRotation_insertValue__IFFFF
  (JNIEnv *env, jobject obj, jint index, jfloat axisX, jfloat axisY,
   jfloat axisZ, jfloat angle)
{
  MFRotation* mfrotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  if (!mfrotation) return;
  float newData[4];
  newData[0] = axisX;
  newData[1] = axisY;
  newData[2] = axisZ;
  newData[3] = angle;
  mfrotation->insertElement(index, newData);
}

/**
 * @brief JNI implementation of MFRotation::toString.
 *
 * @param env JNI environment
 * @param obj MFRotation object
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
  MFString* mfstring;

  if (size > 0)
  {
    std::string* stringArray = new std::string[size];

    for (int i = 0; i < size; i++)
    {
      jstring el = (jstring) env->GetObjectArrayElement(jarr, i);
      const char* temp = env->GetStringUTFChars(el, 0);
      stringArray[i] = std::string(temp);
      env->ReleaseStringUTFChars(el, temp);
    }

    mfstring = new MFString(size, stringArray);

    delete[] stringArray;
  }
  else
  {
    mfstring = new MFString();
  }

  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(mfstring));
}

jint JNICALL Java_vrml_field_ConstMFString_getSize(JNIEnv * env, jobject obj) {
  MFString * mfstring = static_cast<MFString *>(getFieldValue(env, obj));
  if (!mfstring) return 0;
  return mfstring->getLength();
}

void JNICALL Java_vrml_field_ConstMFString_getValue
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  if (!mfstring) return;
  for (int i = 0; i < mfstring->getLength(); i++)
  {
    jstring jstr;
    const char* str = (mfstring->getElement(i)).c_str();
    if (str)
    {
      jstr = env->NewStringUTF(str);
      env->SetObjectArrayElement(jarr, i, jstr);
    }
  }
}

jstring JNICALL Java_vrml_field_ConstMFString_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  if (!mfstring) return 0;
  jstring jstr = env->NewStringUTF((mfstring->getElement(index)).c_str());
  return jstr;
}

/**
 * @brief JNI implementation of ConstMFString::toString.
 *
 * @param env JNI environment
 * @param obj ConstMFString object
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

void JNICALL Java_vrml_field_MFString_clear
  (JNIEnv *env, jobject obj)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  if (!mfstring) return;
  mfstring->setLength(0);
}

void JNICALL Java_vrml_field_MFString_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  if (!mfstring) return;
  mfstring->removeElement(index);
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
Java_vrml_field_MFString_setValue__I_3Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  if (!mfstring) return;

  jsize entries = env->GetArrayLength(jarr);
  mfstring->setLength(entries);

  for (int pos = 0; pos < entries; pos++)
  {
    jstring jstr = static_cast<jstring>(env->GetObjectArrayElement(jarr, pos));
    const char *str = env->GetStringUTFChars(jstr, 0);
    mfstring->setElement(pos, std::string(str));
    env->ReleaseStringUTFChars(jstr, str);
  }
}

void JNICALL
Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  MFString* newMFString = static_cast<MFString*>(getFieldValue(env, value));
  if (!mfstring || !newMFString) return;

  size_t newStringLength = newMFString->getLength();
  mfstring->setLength(newStringLength);

  for (int pos = 0; pos < newStringLength; pos++)
  {
    mfstring->setElement(pos, newMFString->getElement(pos));
  }
}

void JNICALL
Java_vrml_field_MFString_setValue__Lvrml_field_ConstMFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2(env, obj, value);
}

void JNICALL
Java_vrml_field_MFString_set1Value__ILjava_lang_String_2
  (JNIEnv *env, jobject obj, jint index, jstring value)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  if (!mfstring) return;
  mfstring->setElement(index,
		       value ? env->GetStringUTFChars(value, 0) : NULL);
}

void JNICALL
Java_vrml_field_MFString_set1Value__ILvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstringObj)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  SFString* sfstring = static_cast<SFString*>(getFieldValue(env, sfstringObj));
  if (!mfstring || !sfstring) return;
  mfstring->setElement(index, sfstring->get());
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
 * @param env JNI environment
 * @param obj MFString object
 * @param value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFString_addValue__Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jstring value)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  if (!mfstring) return;
  size_t curLength = mfstring->getLength();
  mfstring->setLength(curLength + 1);
  mfstring->setElement(curLength,
                       value ? env->GetStringUTFChars(value, 0) : NULL);
}

/**
 * @brief JNI implementation of MFString::addValue.
 *
 * @param env JNI environment
 * @param obj MFString object
 * @param sfstringObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFString_addValue__Lvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jobject sfstringObj)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  SFString* sfstring = static_cast<SFString*>(getFieldValue(env, sfstringObj));
  if (!mfstring || !sfstring) return;
  size_t curLength = mfstring->getLength();
  mfstring->setLength(curLength + 1);
  mfstring->setElement(curLength,
                       (sfstring->get()).c_str());
}

/**
 * @brief JNI implementation of MFString::addValue.
 *
 * @param env JNI environment
 * @param obj MFString object
 * @param sfstring Value to add to end of list
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
 * @param env JNI environment
 * @param obj MFString object
 * @param index Position to insert value at
 * @param value Value to insert.
 */
void JNICALL
Java_vrml_field_MFString_insertValue__ILjava_lang_String_2
  (JNIEnv *env, jobject obj, jint index, jstring value)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  if (!mfstring) return;
  mfstring->insertElement(index,
                          value ? env->GetStringUTFChars(value, 0) : NULL);
}

/**
 * @brief JNI implementation of MFString::insertValue.
 *
 * @param env JNI environment
 * @param obj MFString object
 * @param index Position to insert value at
 * @param sfstringObj Value to insert.
 */
void JNICALL
Java_vrml_field_MFString_insertValue__ILvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstringObj)
{
  MFString* mfstring = static_cast<MFString*>(getFieldValue(env, obj));
  SFString* sfstring = static_cast<SFString*>(getFieldValue(env, sfstringObj));
  if (!mfstring || !sfstring) return;
  mfstring->insertElement(index, (sfstring->get()).c_str());
}

/**
 * @brief JNI implementation of MFString::insertValue.
 *
 * @param env JNI environment
 * @param obj MFString object
 * @param index Position to insert value at
 * @param sfstring Value to insert.
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
 * @param env JNI environment
 * @param obj MFString object
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
  jdouble *pjd = env->GetDoubleArrayElements(value, NULL);
  MFTime* mftime = new MFTime(size, const_cast<const double *>(pjd));
  env->ReleaseDoubleArrayElements(value, pjd, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  if (!fid) return;
  env->SetIntField(obj, fid, reinterpret_cast<int>(mftime));
}

jint JNICALL Java_vrml_field_ConstMFTime_getSize(JNIEnv * env, jobject obj)
{
  MFTime * mftime = static_cast<MFTime *>(getFieldValue(env, obj));
  if (!mftime) return 0;
  return mftime->getLength();
}

void JNICALL Java_vrml_field_ConstMFTime_getValue(JNIEnv * env,
                                                  jobject obj,
                                                  jdoubleArray jarr)
{
  MFTime * const mftime = static_cast<MFTime*>(getFieldValue(env, obj));
  if (!mftime) return;
  const size_t size = mftime->getLength();
  if (size > 0) {
      env->SetDoubleArrayRegion(jarr, 0, size,
                                const_cast<jdouble *>(&mftime->getElement(0)));
  }
}

jdouble JNICALL Java_vrml_field_ConstMFTime_get1Value(JNIEnv * env,
                                                      jobject obj,
                                                      jint index) {
  MFTime * mftime = static_cast<MFTime *>(getFieldValue(env, obj));
  if (!mftime) return 0.0;
  return mftime->getElement(index);
}

/**
 * @brief JNI implementation of ConstMFTime::toString.
 *
 * @param env JNI environment
 * @param obj ConstMFTime object
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
  MFTime * const mftime = static_cast<MFTime *>(getFieldValue(env, obj));
  if (!mftime) return;
  mftime->setLength(0); // Shouldn't throw bad_alloc.
}

void JNICALL Java_vrml_field_MFTime_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFTime* mftime = static_cast<MFTime*>(getFieldValue(env, obj));
  if (!mftime) return;
  mftime->removeElement(index);
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

void JNICALL Java_vrml_field_MFTime_setValue__I_3D(JNIEnv * env,
                                                   jobject obj,
                                                   jint size,
                                                   jdoubleArray value)
{
    try {
        MFTime * const mftime =
                static_cast<MFTime *>(getFieldValue(env, obj));
        if (!mftime) return;
        mftime->setLength(size); // Throws bad_alloc.
        jdouble * const times = env->GetDoubleArrayElements(value, 0);
        if (!times) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (size_t i = 0; i < mftime->getLength(); ++i) {
            if (!(i < env->GetArrayLength(value))) {
                env->ExceptionDescribe();
                env->ExceptionClear();
                jclass exceptionClass =
                        env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                if (!exceptionClass) {
                    // Presumably FindClass raised an exception.
                    break;
                }
                env->ThrowNew(exceptionClass, "size larger than values.length");
                break;
            }
            mftime->setElement(i, times[i]);
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
    MFTime * const mftime = static_cast<MFTime *>(getFieldValue(env, obj));
    const MFTime * const newMFTime =
            static_cast<MFTime *>(getFieldValue(env, value));
    if (!mftime || !newMFTime) return;
    try {
        *mftime = *newMFTime; // Throws bad_alloc.
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

void JNICALL Java_vrml_field_MFTime_set1Value__ID
  (JNIEnv *env, jobject obj, jint index, jdouble value)
{
  MFTime* mftime = static_cast<MFTime*>(getFieldValue(env, obj));
  if (!mftime) return;
  mftime->setElement(index, value);
}

void JNICALL
Java_vrml_field_MFTime_set1Value__ILvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftimeObj)
{
  MFTime* mftime = static_cast<MFTime*>(getFieldValue(env, obj));
  SFTime* sftime = static_cast<SFTime*>(getFieldValue(env, sftimeObj));
  if (!mftime || !sftime) return;
  mftime->setElement(index, sftime->get());
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
 * @param env JNI environment
 * @param obj MFTime object
 * @param value Value to add to end of list
 */
void JNICALL Java_vrml_field_MFTime_addValue__D(JNIEnv * env,
                                                jobject obj,
                                                jdouble value) {
  MFTime * mftime = static_cast<MFTime *>(getFieldValue(env, obj));
  if (!mftime) return;
  size_t curLength = mftime->getLength();
  mftime->setLength(curLength + 1);
  mftime->setElement(curLength, value);
}

/**
 * @brief JNI implementation of MFTime::addValue.
 *
 * @param env JNI environment
 * @param obj MFTime object
 * @param sftimeObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFTime_addValue__Lvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jobject sftimeObj)
{
  MFTime* mftime = static_cast<MFTime*>(getFieldValue(env, obj));
  SFTime* sftime = static_cast<SFTime*>(getFieldValue(env, sftimeObj));
  if (!mftime || !sftime) return;
  size_t curLength = mftime->getLength();
  mftime->setLength(curLength + 1);
  mftime->setElement(curLength, sftime->get());
}

/**
 * @brief JNI implementation of MFTime::addValue.
 *
 * @param env JNI environment
 * @param obj MFTime object
 * @param sftime Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFTime_addValue__Lvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jobject sftime)
{
  Java_vrml_field_MFTime_addValue__Lvrml_field_ConstSFTime_2
    (env, obj, sftime);
}

void JNICALL Java_vrml_field_MFTime_insertValue__ID
  (JNIEnv *env, jobject obj, jint index, jdouble value)
{
  MFTime* mftime = static_cast<MFTime*>(getFieldValue(env, obj));
  if (!mftime) return;
  mftime->insertElement(index, value);
}

void JNICALL
Java_vrml_field_MFTime_insertValue__ILvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftimeObj)
{
  MFTime* mftime = static_cast<MFTime*>(getFieldValue(env, obj));
  SFTime* sftime = static_cast<SFTime*>(getFieldValue(env, sftimeObj));
  if (!mftime || !sftime) return;
  mftime->insertElement(index, sftime->get());
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
 * @param env JNI environment
 * @param obj MFTime object
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
        std::auto_ptr<MFVec2f> mfvec2f(new MFVec2f(env->GetArrayLength(jarr)));
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
        std::auto_ptr<MFVec2f> mfvec2f(new MFVec2f(size));
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

jint JNICALL Java_vrml_field_ConstMFVec2f_getSize(JNIEnv * env, jobject obj) {
  MFVec2f * mfvec2f = static_cast<MFVec2f *>(getFieldValue(env, obj));
  if (!mfvec2f) return 0;
  return mfvec2f->getLength();
}

void JNICALL Java_vrml_field_ConstMFVec2f_getValue___3_3F(JNIEnv * env,
                                                          jobject obj,
                                                          jobjectArray jarr)
{
    const MFVec2f * const mfvec2f =
            static_cast<MFVec2f *>(getFieldValue(env, obj));
    if (!mfvec2f) return;
    for (size_t i = 0; i < mfvec2f->getLength(); ++i) {
        jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
        if (!element) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
        env->SetFloatArrayRegion(element, 0, 2,
                                 const_cast<jfloat *>(mfvec2f->getElement(i)));
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
    const MFVec2f * const mfvec2f =
            static_cast<MFVec2f *>(getFieldValue(env, obj));
    if (!mfvec2f) return;
    for (size_t i = 0; i < mfvec2f->getLength(); ++i) {
        env->SetFloatArrayRegion(jarr, i * 2, 2,
                                 const_cast<jfloat *>(mfvec2f->getElement(i)));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

/**
 * @brief JNI implementation of ConstMFVec2f::get1Value.
 *
 * @param env JNI environment
 * @param obj ConstMFVec2f object
 * @param element Position of object to retrieve
 * @param jarr Array to store retrieved x,y value
 */
void JNICALL Java_vrml_field_ConstMFVec2f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  MFVec2f* mfvec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  if (!mfvec2f) return;
  env->SetFloatArrayRegion(jarr, 0, 2,const_cast<jfloat*>(
                                        mfvec2f->getElement(element)));
}

/**
 * @brief JNI implementation of ConstMFVec2f::get1Value.
 *
 * @param env JNI environment
 * @param obj ConstMFVec2f object
 * @param element Position of object to retrieve
 * @param sfvec2fObj SFVec2f to store retrieved object in
 */
void JNICALL
Java_vrml_field_ConstMFVec2f_get1Value__ILvrml_field_SFVec2f_2(
        JNIEnv * env,
        jobject obj,
        jint element,
        jobject sfvec2fObj)
{
    const MFVec2f * const mfvec2f =
            static_cast<MFVec2f *>(getFieldValue(env, obj));
    if (!mfvec2f) return;
    if (element < 0 || element >= mfvec2f->getLength()) {
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
    SFVec2f * const sfvec2f =
            static_cast<SFVec2f *>(getFieldValue(env, sfvec2fObj));
    if (!sfvec2f) return;
    sfvec2f->set(mfvec2f->getElement(element));
}

/**
 * @brief JNI implementation of ConstMFVec2f::toString.
 *
 * @param env JNI environment
 * @param obj ConstMFVec2f object
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
  MFVec2f * const mfvec2f = static_cast<MFVec2f *>(getFieldValue(env, obj));
  if (!mfvec2f) return;
  mfvec2f->setLength(0); // Shouldn't throw bad_alloc.
}

void JNICALL Java_vrml_field_MFVec2f_delete
  (JNIEnv *env, jobject obj, jint n)
{
  MFVec2f* mfvec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  if (!mfvec2f) return;
  mfvec2f->removeElement(n);
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
 * @param env JNI environment
 * @param obj MFVec2f object
 * @param element Position of object to retrieve
 * @param jarr Array to store retrieved x,y value
 */
void JNICALL Java_vrml_field_MFVec2f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec2f_get1Value__I_3F(env, obj, element, jarr);
}

/**
 * @brief JNI implementation of MFVec2f::get1Value.
 *
 * @param env JNI environment
 * @param obj MFVec2f object
 * @param element Position of object to retrieve
 * @param sfvec2f SFVec2f to store retrieved object in
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
        MFVec2f * const mfvec2f =
                static_cast<MFVec2f *>(getFieldValue(env, obj));
        if (!mfvec2f) return;
        mfvec2f->setLength(env->GetArrayLength(value)); // throws bad_alloc
        
        for (size_t i = 0; i < mfvec2f->getLength(); ++i) {
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
            const float vec[2] = { v[0], v[1] };
            env->ReleaseFloatArrayElements(element, v, JNI_ABORT);
            mfvec2f->setElement(i, vec);
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
        MFVec2f * const mfvec2f =
                static_cast<MFVec2f *>(getFieldValue(env, obj));
        if (!mfvec2f) return;
        mfvec2f->setLength(size); // throws bad_alloc
        jfloat * const vecs = env->GetFloatArrayElements(value, 0);
        if (!vecs) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (size_t i = 0; i < mfvec2f->getLength(); ++i) {
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
            const float vec[2] = { vecs[2 * i],
                                   vecs[2 * i + 1] };
            mfvec2f->setElement(i, vec);
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
    MFVec2f * const mfvec2f = static_cast<MFVec2f *>(getFieldValue(env, obj));
    const MFVec2f * const newMFVec2f =
            static_cast<MFVec2f *>(getFieldValue(env, value));
    if (!mfvec2f || !newMFVec2f) return;
    try {
        *mfvec2f = *newMFVec2f; // Throws bad_alloc.
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
  MFVec2f* mfvec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  if (!mfvec2f) return;
  float *p = const_cast<float*>(mfvec2f->getElement(index));
  p[0] = x;
  p[1] = y;
}

void JNICALL
Java_vrml_field_MFVec2f_set1Value__ILvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2fObj)
{
  MFVec2f* mfvec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  SFVec2f* sfvec2f = static_cast<SFVec2f*>(getFieldValue(env, sfvec2fObj));
  if (!mfvec2f || !sfvec2f) return;
  mfvec2f->setElement(index, sfvec2f->get());
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
 * @param env JNI environment
 * @param obj MFVec2f object
 * @param x X-component of vector to add to end of list
 * @param y Y-component of vector to add to end of list
 */
void JNICALL Java_vrml_field_MFVec2f_addValue__FF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  MFVec2f* mfvec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  if (!mfvec2f) return;
  float farr[] = { x, y };
  size_t curLength = mfvec2f->getLength();
  mfvec2f->setLength(curLength + 1);
  mfvec2f->setElement(curLength, farr);
}

/**
 * @brief JNI implementation of MFVec2f::addValue.
 *
 * @param env JNI environment
 * @param obj MFVec2f object
 * @param sfvec2fObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFVec2f_addValue__Lvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jobject sfvec2fObj)
{
  MFVec2f* mfvec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  SFVec2f* sfvec2f = static_cast<SFVec2f*>(getFieldValue(env, sfvec2fObj));
  if (!mfvec2f || !sfvec2f) return;
  size_t curLength = mfvec2f->getLength();
  mfvec2f->setLength(curLength + 1);
  mfvec2f->setElement(curLength, sfvec2f->get());
}

/**
 * @brief JNI implementation of MFVec2f::addValue.
 *
 * @param env JNI environment
 * @param obj MFVec2f object
 * @param sfvec2f Value to add to end of list
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
  MFVec2f* mfvec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  if (!mfvec2f) return;
  float farr[] = { x, y };
  mfvec2f->insertElement(index, farr);
}

void JNICALL
Java_vrml_field_MFVec2f_insertValue__ILvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2fObj)
{
  MFVec2f* mfvec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  SFVec2f* sfvec2f = static_cast<SFVec2f*>(getFieldValue(env, sfvec2fObj));
  if (!mfvec2f || !sfvec2f) return;
  mfvec2f->insertElement(index, sfvec2f->get());
}

void JNICALL
Java_vrml_field_MFVec2f_insertValue__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  Java_vrml_field_MFVec2f_insertValue__ILvrml_field_ConstSFVec2f_2
    (env, obj, index, sfvec2f);
}

/**
 * @brief JNI implementation of MFVec2f::toString.
 *
 * @param env JNI environment
 * @param obj MFVec2f object
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
        std::auto_ptr<MFVec3f> mfvec3f(new MFVec3f(env->GetArrayLength(jarr)));
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
        std::auto_ptr<MFVec3f> mfvec3f(new MFVec3f(size));
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

jint JNICALL Java_vrml_field_ConstMFVec3f_getSize(JNIEnv * env, jobject obj) {
  MFVec3f * mfvec3f = static_cast<MFVec3f *>(getFieldValue(env, obj));
  if (!mfvec3f) return 0;
  return mfvec3f->getLength();
}

void JNICALL Java_vrml_field_ConstMFVec3f_getValue___3_3F(JNIEnv * env,
                                                          jobject obj,
                                                          jobjectArray jarr)
{
    const MFVec3f * const mfvec3f =
            static_cast<MFVec3f *>(getFieldValue(env, obj));
    if (!mfvec3f) return;
    for (size_t i = 0; i < mfvec3f->getLength(); ++i) {
        jfloatArray element =
                static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, i));
        if (!element) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
        env->SetFloatArrayRegion(element, 0, 3,
                                 const_cast<jfloat *>(mfvec3f->getElement(i)));
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
    const MFVec3f * const mfvec3f =
            static_cast<MFVec3f *>(getFieldValue(env, obj));
    if (!mfvec3f) return;
    for (size_t i = 0; i < mfvec3f->getLength(); ++i) {
        env->SetFloatArrayRegion(jarr, i * 3, 3,
                                 const_cast<jfloat *>(mfvec3f->getElement(i)));
        if (env->ExceptionOccurred()) {
            // Presumably we raised an ArrayIndexOutOfBoundsException.
            return;
        }
    }
}

/**
 * @brief JNI implementation of ConstMFVec3f::get1Value.
 *
 * @param env JNI environment
 * @param obj ConstMFVec3f object
 * @param element Position of object to retrieve
 * @param jarr Array to store retrieved x,y,z value
 */
void JNICALL Java_vrml_field_ConstMFVec3f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  MFVec3f* mfvec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  if (!mfvec3f) return;
  env->SetFloatArrayRegion(jarr, 0, 3,
                           const_cast<jfloat*>(mfvec3f->getElement(element)));
}

/**
 * @brief JNI implementation of ConstMFVec3f::get1Value.
 *
 * @param env JNI environment
 * @param obj ConstMFVec3f object
 * @param element Position of object to retrieve
 * @param sfvec3fObj SFVec3f to store retrieved object in
 */
void JNICALL
Java_vrml_field_ConstMFVec3f_get1Value__ILvrml_field_SFVec3f_2(
        JNIEnv * env,
        jobject obj,
        jint element,
        jobject sfvec3fObj)
{
    const MFVec3f * const mfvec3f =
            static_cast<MFVec3f *>(getFieldValue(env, obj));
    if (!mfvec3f) return;
    if (element < 0 || element >= mfvec3f->getLength()) {
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
    SFVec3f * const sfvec3f =
            static_cast<SFVec3f *>(getFieldValue(env, sfvec3fObj));
    if (!sfvec3f) return;
    sfvec3f->set(mfvec3f->getElement(element));
}

/**
 * @brief JNI implementation of ConstMFVec3f::toString.
 *
 * @param env JNI environment
 * @param obj ConstMFVec3f object
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
  MFVec3f * const mfvec3f = static_cast<MFVec3f *>(getFieldValue(env, obj));
  if (!mfvec3f) return;
  mfvec3f->setLength(0); // Shouldn't throw bad_alloc.
}

void JNICALL Java_vrml_field_MFVec3f_delete
  (JNIEnv *env, jobject obj, jint n)
{
  MFVec3f* mfvec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  if (!mfvec3f) return;
  mfvec3f->removeElement(n);
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
 * @param env JNI environment
 * @param obj MFVec3f object
 * @param element Position of object to retrieve
 * @param jarr Array to store retrieved x,y,z value
 */
void JNICALL Java_vrml_field_MFVec3f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec3f_get1Value__I_3F(env, obj, element, jarr);
}

/**
 * @brief JNI implementation of MFVec3f::get1Value.
 *
 * @param env JNI environment
 * @param obj MFVec3f object
 * @param element Position of object to retrieve
 * @param sfvec3f SFVec3f to store retrieved object in
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
        MFVec3f * const mfvec3f =
                static_cast<MFVec3f *>(getFieldValue(env, obj));
        if (!mfvec3f) return;
        mfvec3f->setLength(env->GetArrayLength(value)); // throws bad_alloc
        
        for (size_t i = 0; i < mfvec3f->getLength(); ++i) {
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
            const float vec[3] = { v[0], v[1], v[2] };
            env->ReleaseFloatArrayElements(element, v, JNI_ABORT);
            mfvec3f->setElement(i, vec);
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
        MFVec3f * const mfvec3f =
                static_cast<MFVec3f *>(getFieldValue(env, obj));
        if (!mfvec3f) return;
        mfvec3f->setLength(size); // throws bad_alloc
        jfloat * const vecs = env->GetFloatArrayElements(value, 0);
        if (!vecs) {
            // Presumably we raised an OutOfMemoryError.
            return;
        }
        for (size_t i = 0; i < mfvec3f->getLength(); ++i) {
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
            const float vec[3] = { vecs[3 * i],
                                     vecs[3 * i + 1],
                                     vecs[3 * i + 2] };
            mfvec3f->setElement(i, vec);
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
    MFVec3f * const mfvec3f = static_cast<MFVec3f *>(getFieldValue(env, obj));
    const MFVec3f * const newMFVec3f =
            static_cast<MFVec3f *>(getFieldValue(env, value));
    if (!mfvec3f || !newMFVec3f) return;
    try {
        *mfvec3f = *newMFVec3f; // Throws bad_alloc.
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
  MFVec3f* mfvec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  if (!mfvec3f) return;
  float p[3];
  p[0] = x;
  p[1] = y;
  p[2] = z;
  mfvec3f->setElement(index, p);

}

void JNICALL
Java_vrml_field_MFVec3f_set1Value__ILvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3fObj)
{
  MFVec3f* mfvec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  SFVec3f* sfvec3f = static_cast<SFVec3f*>(getFieldValue(env, sfvec3fObj));
  if (!mfvec3f || !sfvec3f) return;
  mfvec3f->setElement(index, sfvec3f->get());
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
 * @param env JNI environment
 * @param obj MFVec3f object
 * @param x X-component of vector to add to end of list
 * @param y Y-component of vector to add to end of list
 * @param z Z-component of vector to add to end of list
 */
void JNICALL Java_vrml_field_MFVec3f_addValue__FFF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  MFVec3f* mfvec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  if (!mfvec3f) return;
  float farr[] = { x, y, z };
  size_t curLength = mfvec3f->getLength();
  mfvec3f->setLength(curLength + 1);
  mfvec3f->setElement(curLength, farr);
}

/**
 * @brief JNI implementation of MFVec3f::addValue.
 *
 * @param env JNI environment
 * @param obj MFVec3f object
 * @param sfvec3fObj Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFVec3f_addValue__Lvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jobject sfvec3fObj)
{
  MFVec3f* mfvec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  SFVec3f* sfvec3f = static_cast<SFVec3f*>(getFieldValue(env, sfvec3fObj));
  if (!mfvec3f || !sfvec3f) return;
  size_t curLength = mfvec3f->getLength();
  mfvec3f->setLength(curLength + 1);
  mfvec3f->setElement(curLength, sfvec3f->get());
}

/**
 * @brief JNI implementation of MFVec3f::addValue.
 *
 * @param env JNI environment
 * @param obj MFVec3f object
 * @param value Value to add to end of list
 */
void JNICALL
Java_vrml_field_MFVec3f_addValue__Lvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFVec3f_addValue__Lvrml_field_ConstSFVec3f_2(env, obj,
							       value);
}

void JNICALL Java_vrml_field_MFVec3f_insertValue__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y, jfloat z)
{
  MFVec3f* mfvec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  if (!mfvec3f) return;
  float farr[] = { x, y, z };
  mfvec3f->insertElement(index, farr);
}

void JNICALL
Java_vrml_field_MFVec3f_insertValue__ILvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3fObj)
{
  MFVec3f* mfvec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  SFVec3f* sfvec3f = static_cast<SFVec3f*>(getFieldValue(env, sfvec3fObj));
  if (!mfvec3f || !sfvec3f) return;
  mfvec3f->insertElement(index, sfvec3f->get());
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
 * @param env JNI environment
 * @param obj MFVec3f object
 * @return String representation of MFVec3f.
 */
jstring JNICALL Java_vrml_field_MFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

/**
 * @brief JNI implementation of Script::getField.
 *
 * @param env JNI environment
 * @param obj Script object
 * @param jstrFieldName Name of desired exposed field
 * @return Exposed field object
 */
jobject JNICALL Java_vrml_node_Script_getField
  (JNIEnv *env, jobject obj, jstring jstrFieldName)
{
  const char *charFieldName = env->GetStringUTFChars(jstrFieldName, 0);
  std::string fieldName(charFieldName);
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  if (!fid) return 0;
  ScriptNode* script =
    reinterpret_cast<ScriptNode*>(env->GetIntField(obj, fid));
  if (!script) return 0;
  jobject Field;

  ScriptNode::FieldValueMap fieldMap = script->getFieldValueMap();
  ScriptNode::FieldValueMap::iterator iter = fieldMap.find(fieldName);

  if (iter != fieldMap.end())
  {
    // Then we've found the field
    const FieldValue* fieldPtr = iter->second.get();
    assert(fieldPtr);
    std::ostrstream os;
    os << "vrml/field/" << fieldPtr->type() << '\0';
    jclass clazz = env->FindClass(os.str());
    os.rdbuf()->freeze(false);
    Field = env->AllocObject(clazz);
    fid = getFid(env, Field, "FieldPtr", "I");
    if (!fid) return 0;
    env->SetIntField(Field, fid, reinterpret_cast<int>(fieldPtr));
  }
  else
  {
    jclass excClazz = env->FindClass("vrml/InvalidFieldException");
    if (excClazz == 0)
    {
      // Can't find exception, just return
      return 0;
    }
    // throw an exception as the given field doesn't exist
    env->ThrowNew(excClazz, "Field not found");
    return 0;
  }

  fid = getFid(env, Field,"isExposedField", "Z");
  if (!fid) return 0;
  env->SetBooleanField(Field, fid, true);
  env->ReleaseStringUTFChars(jstrFieldName, charFieldName );
  return Field;
}

/**
 * @brief JNI implementation of Script::getEventOut.
 *
 * @param env JNI environment
 * @param obj Script object
 * @param jstrEventOutName Name of desired eventOut
 * @return eventOut object
 */
jobject JNICALL Java_vrml_node_Script_getEventOut
  (JNIEnv *env, jobject obj, jstring jstrEventOutName)
{
  jobject eventOut;
  const char *charEventOut = env->GetStringUTFChars(jstrEventOutName , 0);
  std::string eventOutName(charEventOut);
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  if (!fid) return 0;
  ScriptNode* script =
    reinterpret_cast<ScriptNode*>(env->GetIntField(obj, fid));
  if (!script) return 0;

  FieldValue::Type eventOutType = script->nodeType.hasEventOut(eventOutName);

  if (eventOutType != FieldValue::invalidType)
  {
    const ScriptNode::EventOutValueMap& eventOutMap =
      script->getEventOutValueMap();
    ScriptNode::EventOutValueMap::const_iterator iter =
      eventOutMap.find(eventOutName);

    if (iter != eventOutMap.end())
    {
      const ScriptNode::PolledEventOutValue& eventOutValue = iter->second;
      // Found the eventOut
      std::ostrstream os;
      os << "vrml/field/" << iter->second.value->type() << '\0';
      jclass clazz = env->FindClass(os.str());
      os.rdbuf()->freeze(false);
      eventOut = env->AllocObject(clazz);
      fid = getFid(env, eventOut, "FieldPtr", "I");
      if (!fid) return 0;
      env->SetIntField(eventOut, fid, reinterpret_cast<int>(&eventOutValue));
    }
  }
  else
  {
    jclass excClazz = env->FindClass("vrml/InvalidEventOutException");
    if (excClazz == 0)
    {
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
 * @param env JNI environment
 * @param obj Script object
 * @param jstrEventInName EventIn name
 * @return EventIn field object.
 */
jobject JNICALL Java_vrml_node_Script_getEventIn
  (JNIEnv *env, jobject obj, jstring jstrEventInName)
{
  const char *charEventInName = env->GetStringUTFChars(jstrEventInName, 0);
  std::string eventInName(charEventInName);
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  if (!fid) return 0;
  ScriptNode* script =
    reinterpret_cast<ScriptNode*>(env->GetIntField(obj, fid));
  if (!script) return 0;
  jobject eventIn;
  FieldValue::Type eventInType =
    script->nodeType.hasEventIn(eventInName);

  if (eventInType != FieldValue::invalidType)
  {
    // Then we've found the eventIn
    FieldValue* fieldPtr = newField(eventInType);
    assert(fieldPtr);
    std::ostrstream os;
    os << "vrml/field/" << ftn[eventInType] << '\0';
    jclass clazz = env->FindClass(os.str());
    os.rdbuf()->freeze(false);
    eventIn = env->AllocObject(clazz);
    fid = getFid(env, eventIn, "FieldPtr", "I");
    if (!fid) return 0;
    env->SetIntField(eventIn, fid, reinterpret_cast<int>(fieldPtr));
  }
  else
  {
    // look for eventIn in exposed field list
    eventInType = script->nodeType.hasField(eventInName);

    if (eventInType != FieldValue::invalidType)
    {
      // Then we've found the eventIn
      FieldValue* fieldPtr = newField(eventInType);
      assert(fieldPtr);
      std::ostrstream os;
      os << "vrml/field/" << ftn[eventInType] << '\0';
      jclass clazz = env->FindClass(os.str());
      os.rdbuf()->freeze(false);
      eventIn = env->AllocObject(clazz);
      fid = getFid(env, eventIn, "FieldPtr", "I");
      if (!fid) return 0;
      env->SetIntField(eventIn, fid, reinterpret_cast<int>(fieldPtr));
    }
    else
    {
      jclass excClazz = env->FindClass("vrml/InvalidEventInException");
      if (excClazz == 0)
      {
        // Can't find exception, just return
        return 0;
      }
      // throw an exception as the given eventIn doesn't exist
      env->ThrowNew(excClazz, "EventIn not found");
      return 0;
    }
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
 * @param JNI environment.
 * @param Script object.
 * @return String representation of Script object.
 */
jstring JNICALL Java_vrml_node_Script_toString
  (JNIEnv *env, jobject obj)
{
  std::ostrstream os;
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  if (!fid) return 0;
  ScriptNode* node = reinterpret_cast<ScriptNode*>(env->GetIntField(obj, fid));
  if (!node) return 0;
  os << *node << std::ends;
  char* szString = os.str();
  jstring result = env->NewStringUTF(szString);
  os.rdbuf()->freeze(0);
  return result;
}

/**
 * @brief JNI implementation of BaseNode::getType.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java node object
 * @return Type of node
 */
jstring JNICALL Java_vrml_BaseNode_getType
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  if (!fid) return 0;
  Node* node = (Node*)env->GetIntField(obj, fid);
  if (!node) return 0;
  return env->NewStringUTF(node->nodeType.id.c_str());
}

/**
 * @brief JNI implementation of BaseNode::getBrowser.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java node object
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
  if (!fid) return 0;

  if (isScript)
  {
    ScriptNode * const node =
      reinterpret_cast<ScriptNode *>(env->GetIntField(obj, fid));
    assert(node);
    assert(node->getScene());
    fid = getFid(env, jBrowser, "BrowserPtr", "I");
    if (!fid) return 0;
    env->SetIntField(jBrowser, fid,
                     reinterpret_cast<int>(&node->getScene()->browser));
  }
  else
  {
    Node * const node =
      reinterpret_cast<Node *>(env->GetIntField(obj, fid));
    assert(node);
    assert(node->getScene());
    fid = getFid(env, jBrowser, "BrowserPtr", "I");
    if (!fid) return 0;
    env->SetIntField(jBrowser, fid,
                     reinterpret_cast<int>(&node->getScene()->browser));
  }

  return jBrowser;
}

/**
 * @brief JNI implementation of Node::getEventIn.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java node object
 * @param jstrEventInName Name of the eventIn
 * @return EventIn field object
 *
 * @todo Implement me! Need to throw InvalidEventInException if
 *       eventIn not present.
 */
jobject JNICALL Java_vrml_node_Node_getEventIn
  (JNIEnv *env, jobject obj, jstring jstrEventInName)
{
  const char *eventInName = env->GetStringUTFChars(jstrEventInName , 0);
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  if (!fid) return 0;
  Node* node = (Node*) env->GetIntField(obj, fid);
  if (!node) return 0;

  /* Uncomment when getEventIn implementation is added to Node
  const FieldValue* field = pNode->getEventIn(eventInName);

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
 * @param env JNI environment
 * @param obj JNI version of a Java node object
 * @param jstrEventOutName Name of the eventOut
 * @return EventOut field object
 *
 * @todo Implement me. Need to throw InvalidEventOutException 
 * if eventOut not present.
 */
jobject JNICALL Java_vrml_node_Node_getEventOut
  (JNIEnv *env, jobject obj, jstring jstrEventOutName)
{
  /*
  const char *eventOutName = env->GetStringUTFChars(jstrEventOutName , 0);
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  Node* pNode = (Node*) env->GetIntField(obj, fid);
  const FieldValue* field = pNode->getEventOut(eventOutName);

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
 * @param env JNI environment
 * @param obj JNI version of a Java node object
 * @param jstrEventOutName Name of the exposed field
 * @return Exposed field object
 *
 * @todo Implement me. Need to throw InvalidEventOutException 
 * if eventOut not present.
 */
jobject JNICALL Java_vrml_node_Node_getExposedField
  (JNIEnv *env, jobject obj, jstring jstrExposedFieldName)
{
  /*
  const char *exposedFieldName =
    env->GetStringUTFChars(jstrExposedFieldName , 0);
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  Node* pNode = (Node*) env->GetIntField(obj, fid);
  const FieldValue* field = pNode->getField(exposedFieldName);

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
 * @param env JNI environment
 * @param obj Node object.
 * @return Stringified version of node.
 */
jstring JNICALL Java_vrml_node_Node_toString
  (JNIEnv *env, jobject obj)
{
  std::ostrstream os;
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  if (!fid) return 0;
  Node* node = reinterpret_cast<Node*>(env->GetIntField(obj, fid));
  if (!node) return 0;
  os << *node << std::ends;
  jstring result = env->NewStringUTF(os.str());
  os.rdbuf()->freeze(0);
  return result;
}

/**
 * @brief JNI implementation of Event::getName.
 *
 * @param env JNI environment
 * @param obj Event object.
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
 * @param env JNI environment
 * @param obj Event object.
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
 * @param env JNI environment
 * @param obj Event object.
 * @return Changed field.
 */
jobject JNICALL Java_vrml_Event_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  if (!fid) return 0;
  VrmlEvent* event = reinterpret_cast<VrmlEvent*>(env->GetIntField(obj, fid));
  if (!event) return 0;
  const FieldValue* field = event->value();
  assert(field);
  std::ostrstream os;
  os << "vrml/field/Const" << field->type() << '\0';
  jclass clazz = env->FindClass(os.str());
  os.rdbuf()->freeze(false);
  jobject jField = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");
  if (!fid) return 0;
  env->SetIntField(jField, fid, reinterpret_cast<int>(field));
  return jField;
}

/**
 * @brief JNI implementation of Event::clone.
 *
 * @param env JNI environment
 * @param obj Event object.
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
 * @param env JNI environment
 * @param obj Event object.
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
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @return Browser name and version.
 */
jstring JNICALL Java_vrml_Browser_toString(JNIEnv * const env,
                                           const jobject obj)
{
    assert(env);
    const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
    if (!fid) return 0;
    Browser * const browser =
      reinterpret_cast<Browser *>(env->GetIntField(obj, fid));
    assert(browser);
    std::ostringstream out;
    out << browser->getName() << ' ' << browser->getVersion() << std::ends;
    return env->NewStringUTF(out.str().c_str());
}

/**
 * @brief JNI implementation of Browser::getName.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @return Name of browser.
 */
jstring JNICALL Java_vrml_Browser_getName(JNIEnv * const env,
                                          const jobject obj)
{
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0;
  Browser * const browser =
    reinterpret_cast<Browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return env->NewStringUTF(browser->getName());
}

/**
 * @brief JNI implementation of Browser::getVersion.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @return Version of OpenVRML being used
 */
jstring JNICALL Java_vrml_Browser_getVersion(JNIEnv * const env,
                                             const jobject obj)
{
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0;
  Browser * const browser =
    reinterpret_cast<Browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return env->NewStringUTF(browser->getVersion());
}

/**
 * @brief JNI implementation of Browser::getCurrentSpeed.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @return Average navigation speed in m/s.
 *
 * @todo Fix me to properly return the current speed.
 */
jfloat JNICALL Java_vrml_Browser_getCurrentSpeed(JNIEnv * const env,
                                                 const jobject obj)
{
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0.0;
  Browser * const browser =
    reinterpret_cast<Browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return 0.0;
}

/**
 * @brief JNI implementation of Browser::getCurrentFrameRate.
 *
 * Returns the current frame rate in frames per second.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 *
 * @return Average navigation speed.
 */
jfloat JNICALL Java_vrml_Browser_getCurrentFrameRate(JNIEnv * const env,
                                                     const jobject obj)
{
  assert(env);
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0.0;
  Browser * const browser =
    reinterpret_cast<Browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return browser->getFrameRate();
}

/**
 * @brief JNI implementation of Browser::getWorldURL.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @return URL of current world displayed in browser.
 */
jstring JNICALL Java_vrml_Browser_getWorldURL(JNIEnv * const env,
                                              const jobject obj)
{
  assert(env);
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0;
  Browser * const browser =
    reinterpret_cast<Browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return env->NewStringUTF(browser->getWorldURI().c_str());
}

/**
 * @brief JNI implementation of Browser::replaceWorld.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @param basenodes ...
 *
 * @todo Implement me!
 */
void JNICALL Java_vrml_Browser_replaceWorld
  (JNIEnv *env, jobject obj, jobjectArray basenodes)
{
  // First check if mustEvaluate is set, if not then method
  // should not continue.
}

/**
 * @brief JNI implementation of Browser::createVrmlFromString
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @param vrmlSyntax ...
 * @return ...
 *
 * @todo Implement me! This method should throw an
 *       InvalidVRMLSyntaxException if the syntax is invalid.
 */
jobjectArray JNICALL
Java_vrml_Browser_createVrmlFromString(JNIEnv * const env,
                                       const jobject obj,
                                       const jstring vrmlSyntax)
{
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return 0;
  Browser * const browser =
    reinterpret_cast<Browser *>(env->GetIntField(obj, fid));
  assert(browser);
  return 0;
}

/**
 * @brief JNI implementation of Browser::createVrmlFromURL.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @param urls ...
 * @param basenode ...
 * @param event ...
 *
 * @todo Implement me!  This method should throw an
 *       InvalidVRMLSyntaxException if the syntax is invalid.
 */
void JNICALL Java_vrml_Browser_createVrmlFromURL
  (JNIEnv *env, jobject obj, jobjectArray urls,
   jobject basenode, jstring event)
{
}

/**
 * @brief JNI implementation of Browser::addRoute.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @param fromNode ...
 * @param fromEventOut ...
 * @param toNode ...
 * @param toEventIn ...
 *
 * @todo This method should throw an InvalidRouteException if one of
 *       the arguments is invalid. This method should first check
 *       if directOutput is set, if it isn't, don't continue.
 */
void JNICALL Java_vrml_Browser_addRoute
  (JNIEnv *env, jobject obj, jobject fromNodeObj, jstring fromEventOut,
   jobject toNodeObj, jstring toEventIn)
{
  jfieldID fid = getFid(env, fromNodeObj, "NodePtr", "I");
  if (!fid) return;
  Node* fromNode = reinterpret_cast<Node*>(env->GetIntField(fromNodeObj, fid));
  assert(fromNode);
  fid = getFid(env, toNodeObj, "NodePtr", "I");
  if (!fid) return;
  Node* toNode = reinterpret_cast<Node*>(env->GetIntField(toNodeObj, fid));
  assert(toNode);
  const char* str = env->GetStringUTFChars(fromEventOut, 0);
  std::string eventOut(str);
  env->ReleaseStringUTFChars(fromEventOut, str);
  str = env->GetStringUTFChars(toEventIn, 0);
  std::string eventIn(str);
  env->ReleaseStringUTFChars(toEventIn, str);
  fromNode->addRoute(eventOut, NodePtr(toNode), eventIn);
}

/**
 * @brief JNI implementation of Browser::deleteRoute
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @param fromNode ...
 * @param fromEventOut ...
 * @param toNode ...
 * @param toEventIn ...
 *
 * @todo This method should throw an InvalidRouteException if one of
 *       the arguments is invalid. This method should first check
 *       if directOutput is set, if it isn't, don't continue.
 */
void JNICALL Java_vrml_Browser_deleteRoute
  (JNIEnv *env, jobject obj, jobject fromNodeObj, jstring fromEventOut,
   jobject toNodeObj, jstring toEventIn)
{
  jfieldID fid = getFid(env, fromNodeObj, "NodePtr", "I");
  if (!fid) return;
  Node* fromNode = reinterpret_cast<Node*>(env->GetIntField(fromNodeObj, fid));
  assert(fromNode);
  fid = getFid(env, toNodeObj, "NodePtr", "I");
  if (!fid) return;
  Node* toNode = reinterpret_cast<Node*>(env->GetIntField(toNodeObj, fid));
  assert(toNode);
  const char* str = env->GetStringUTFChars(fromEventOut, 0);
  std::string eventOut(str);
  env->ReleaseStringUTFChars(fromEventOut, str);
  str = env->GetStringUTFChars(toEventIn, 0);
  std::string eventIn(str);
  env->ReleaseStringUTFChars(toEventIn, str);
  fromNode->deleteRoute(eventOut, NodePtr(toNode), eventIn);
}

/**
 * @brief JNI implementation of Browser::loadURL.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @param jUrlArray URL to load with alternates.
 * @param jParameterArray Array of parameters to load URL with
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
  assert(env);
    
  size_t i;
  MFString url(env->GetArrayLength(jUrlArray));
  for (i = 0; i < url.getLength(); ++i)
  {
    jstring jstr =
      static_cast<jstring>(env->GetObjectArrayElement(jUrlArray, i));
    const char * str = env->GetStringUTFChars(jstr, 0);
    url.setElement(i, str);
    env->ReleaseStringUTFChars(jstr, str);
  }

  MFString parameters(env->GetArrayLength(jParameterArray));
  for (i = 0; i < parameters.getLength(); ++i)
  {
    jstring jstr =
      static_cast<jstring>(env->GetObjectArrayElement(jParameterArray, i));
    const char * str = env->GetStringUTFChars(jstr, 0);
    parameters.setElement(i, str);
    env->ReleaseStringUTFChars(jstr, str);
  }

  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return;
  Browser * const browser =
    reinterpret_cast<Browser *>(env->GetIntField(obj, fid));
  assert(browser);

  browser->loadURI(url, parameters);
}

/**
 * @brief JNI implementation of Browser::setDescription.
 *
 * @param env JNI environment
 * @param obj JNI version of a Java Browser object.
 * @param jDescription Desired description of Browser.
 */
void JNICALL Java_vrml_Browser_setDescription
  (JNIEnv *env, jobject obj, jstring jDescription)
{
  const char *description = env->GetStringUTFChars(jDescription, 0);
  const jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  if (!fid) return;
  Browser * const browser =
    reinterpret_cast<Browser *>(env->GetIntField(obj, fid));
  assert(browser);
  browser->setDescription(std::string(description));
  env->ReleaseStringUTFChars(jDescription, description);
}

#endif // OPENVRML_ENABLE_SCRIPT_NODE_JAVA
