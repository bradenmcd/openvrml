//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2001  Henri Manson
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

# if defined(_WIN32) && !defined(__CYGWIN__)
#   include <winconfig.h>
# endif

#ifdef OPENVRML_HAVE_JNI
#include "ScriptJDK.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <strstrea.h>
#else
#include <strstream.h>
#endif

# include <vrml_Browser.h>
# include <vrml_Event.h>
# include <vrml_Field.h>
# include <vrml_MField.h>
# include <vrml_ConstField.h>
# include <vrml_ConstMField.h>
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
# include <vrml_field_SFVec3f.h>
# include <vrml_field_MFColor.h>
# include <vrml_field_MFFloat.h>
# include <vrml_field_MFInt32.h>
# include <vrml_field_MFNode.h>
# include <vrml_field_MFRotation.h>
# include <vrml_field_MFString.h>
# include <vrml_field_MFTime.h>
# include <vrml_field_MFVec2f.h>
# include <vrml_field_MFVec3f.h>
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
# include <vrml_field_ConstSFVec3f.h>
# include <vrml_field_ConstMFColor.h>
# include <vrml_field_ConstMFFloat.h>
# include <vrml_field_ConstMFInt32.h>
# include <vrml_field_ConstMFNode.h>
# include <vrml_field_ConstMFRotation.h>
# include <vrml_field_ConstMFString.h>
# include <vrml_field_ConstMFTime.h>
# include <vrml_field_ConstMFVec2f.h>
# include <vrml_field_ConstMFVec3f.h>
# include <vrml_node_Node.h>
# include <vrml_node_Script.h>

#include "doc2.hpp"
#include "MathUtils.h"
#include "System.h"
#include "script.h"
#include "VrmlNamespace.h"
#include "VrmlScene.h"
#include "nodeptr.h"

#include "field.h"

#ifdef _WIN32
#define PATH_SEPARATOR ";"
#else /* UNIX and macintosh */
#define PATH_SEPARATOR ":"
#endif

using namespace OpenVRML;

namespace {

    class OPENVRML_SCOPE VrmlEvent {
        double d_timeStamp;
        char *d_eventName;
        FieldValue *d_value;
        int d_refCount;

    public:
        VrmlEvent(double timeStamp, const char *eventName,
                  const FieldValue * value) :
          d_timeStamp(timeStamp),
          d_eventName(new char[strlen(eventName)+1]),
          d_value(value->clone()),
          d_refCount(1)
        { strcpy(d_eventName, eventName); }

        ~VrmlEvent() { delete [] d_eventName; delete d_value; }

        VrmlEvent * clone() const {
          return new VrmlEvent(this->d_timeStamp, this->d_eventName, this->d_value);
        }

        // VrmlEvents are reference counted.
        // The reference counting is manual (that is, each user of a
        // VrmlEvent, such as the VrmlNode class, calls reference()
        // and dereference() explicitly). Should make it internal...

        // Add/remove references to a VrmlEvent. This is silly, as it
        // requires the users of VrmlEvent to do the reference/derefs...
        VrmlEvent *reference() { ++d_refCount; return this; }
        void dereference() { if (--d_refCount == 0) delete this; }

        double timeStamp() const { return d_timeStamp; }
        const char * name() const { return d_eventName; }
        const FieldValue * value() const { return d_value; }
    };
}

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
 * @param scriptNode ...
 * @param className ...
 * @param classDir ...
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

    ostrstream appendedClassPath;

    appendedClassPath << "-Djava.class.path=." 
		      << PATH_SEPARATOR << classDir;

    // Only append CLASSPATH if non-null
    if (classPath != 0)
    {
      appendedClassPath << PATH_SEPARATOR << classPath;
    }

#ifndef _WIN32
    appendedClassPath << PATH_SEPARATOR
                      << OPENVRML_PKGDATADIR_ << "/java/script.jar"; 
#endif
	appendedClassPath << ends;

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
    if (d_class == 0) 
    {
      theSystem->error( "Can't find Java class %s.\n", className);
      return;
    }

    // Call constructor
    jmethodID ctorId = d_env->GetMethodID(d_class, "<init>", "()V");

    if (ctorId != 0)
      d_object = d_env->NewObject(d_class, ctorId);

    jfieldID fid = d_env->GetFieldID(d_class, "NodePtr", "I");
    d_env->SetIntField(d_object, fid, reinterpret_cast<int>(&this->scriptNode));

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

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @param membername ...
 * @param sig ...
 * @return ...
 */
static jfieldID getFid(JNIEnv *env, jobject obj, char *membername, char *sig)
{
  jclass clazz;
  clazz = env->GetObjectClass(obj);
  return clazz == 0 ? NULL : env->GetFieldID(clazz, membername, sig);
}

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @return ...
 */
static FieldValue* getFieldValue(JNIEnv *env, jobject obj)
{
  FieldValue* fieldPtr;
  jfieldID fid;

  fid = getFid(env, obj, "isEventOut", "Z");
  bool eventOut = static_cast<bool>(env->GetBooleanField(obj, fid));
  fid = getFid(env, obj, "isEventIn", "Z");
  bool eventIn = static_cast<bool>(env->GetBooleanField(obj, fid));
  fid = getFid(env, obj, "isExposedField", "Z");
  bool exposedField = static_cast<bool>(env->GetBooleanField(obj, fid));
  fid = getFid(env, obj, "FieldPtr", "I");

  cout << "TODO: Implement getFieldValue" << endl;

  if (eventOut)
  {
    ScriptNode::PolledEventOutValue * const eventOutPtr =
      reinterpret_cast<ScriptNode::PolledEventOutValue *>
        (env->GetIntField(obj, fid));
    fieldPtr = static_cast<FieldValue*>(eventOutPtr->value.get());
    eventOutPtr->modified = true;
  }
  else
  {
    fieldPtr = (FieldValue*) env->GetIntField(obj, fid);
  }

  return fieldPtr;
}

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @return ...
 */
static jstring fieldToString(JNIEnv *env, jobject obj)
{
  ostrstream os;
  FieldValue* pField = getFieldValue(env, obj);
  os << *pField << ends;
  char* szString = os.str();
  jstring result = env->NewStringUTF(szString);
  os.rdbuf()->freeze(0);
  return result;
}

/**
 * @brief ...
 *
 * ...
 *
 * @param timestamp ...
 */
void ScriptJDK::initialize(const double timestamp)
{
  const SFTime arg(timestamp);
  const FieldValue * argv[] = { &arg };
  this->activate(timestamp, "initialize", 1, argv);
}

/**
 * @brief ...
 *
 * ...
 *
 * @param id ...
 * @param value ...
 * @param timestamp ...
 */
void ScriptJDK::processEvent(const std::string & id, const FieldValue & value,
                             const double timestamp)
{
  const SFTime timestampArg(timestamp);
  const FieldValue * argv[] = { &value, &timestampArg };
  this->activate(timestamp, id, 2, argv);
}

/**
 * @brief ...
 *
 * ...
 *
 * @param timestamp ...
 */
void ScriptJDK::eventsProcessed(const double timestamp)
{
  const SFTime arg(timestamp);
  const FieldValue * argv[] = { &arg };
  this->activate(timestamp, "eventsProcessed", 1, argv);
}

/**
 * @brief ...
 *
 * ...
 *
 * @param timestamp ...
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
 * ...
 *
 * @param timeStamp ...
 * @param fname ...
 * @param argc ...
 * @param argv ...
 */
void ScriptJDK::activate( double timeStamp,
			  const std::string & fname,
			  size_t argc,
			  const FieldValue * const argv[] )
{
  if (argc == 2 && d_processEventID != 0)
  {
    jclass clazz = d_env->FindClass("vrml/Event");

    if (clazz == 0)
    {
      theSystem->error( "Can't find Java class vrml/Event");
      return;
    }

    jobject jEvent = d_env->AllocObject(clazz);
    jfieldID fid = getFid(d_env, jEvent, "EventPtr", "I");
    VrmlEvent* pEvent = new VrmlEvent(timeStamp, fname.c_str(), argv[0]);
    d_env->SetIntField(jEvent, fid, (int) pEvent);
    d_env->CallVoidMethod(d_object, d_processEventID, jEvent);
    d_env->DeleteLocalRef(jEvent);
    delete pEvent;

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
    if (initID != 0)
      d_env->CallVoidMethod(d_object, initID);
    if (d_env->ExceptionOccurred())
    {
      d_env->ExceptionDescribe();
      d_env->ExceptionClear();
    }
  }
}

/**
 * @brief ...
 *
 * ...
 *
 * @param fieldtype ...
 * @return ...
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
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @return ...
 */
jobject JNICALL Java_vrml_Field_clone
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  jobject jCloneField;
  jclass clazz;
  FieldValue* pField;
  FieldValue* pCloneField;

  // TODO: This method needs to be revisited  
  fid = getFid(env, obj, "FieldPtr", "I");
  pField = (FieldValue*) env->GetIntField(obj, fid);
  ostrstream os;
  os << "vrml/field/" << pField->type() << '\0';
  clazz = env->FindClass(os.str());
  os.rdbuf()->freeze(false);
  jCloneField = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");
  pCloneField = pField->clone();
  env->SetIntField(jCloneField, fid, (int) pCloneField);	
  return jCloneField;
}

/**
 * @brief ...
 *
 * ...
 *
 * @param vm ...
 * @param reserved ...
 * @return ...
 */
jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
  return JNI_VERSION_1_2;
}

/**
 * @brief ...
 *
 * ...
 *
 * @param vm ...
 * @param reserved ...
 */
void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
}

//
// ConstSFBool
//

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @param value ...
 */
void JNICALL Java_vrml_field_ConstSFBool_CreateObject
  (JNIEnv *env, jobject obj, jboolean value)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  SFBool* pSFBool = new SFBool(value ? true : false);
  env->SetIntField(obj, fid, (int) pSFBool);
}

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @return ...
 */
jboolean JNICALL Java_vrml_field_ConstSFBool_getValue
  (JNIEnv *env, jobject obj)
{
  SFBool* pSFBool = static_cast<SFBool*>(getFieldValue(env, obj));
  return static_cast<jboolean>(pSFBool->get());
}

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @return ...
 */
jstring JNICALL Java_vrml_field_ConstSFBool_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFBool
//

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @param value ...
 */
void JNICALL Java_vrml_field_SFBool_CreateObject
  (JNIEnv *env, jobject obj, jboolean value)
{
  Java_vrml_field_ConstSFBool_CreateObject(env, obj, value);
}

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @return ...
 */
jboolean JNICALL Java_vrml_field_SFBool_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFBool_getValue(env, obj);
}

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @param value ...
 */
void JNICALL Java_vrml_field_SFBool_setValue__Z
  (JNIEnv *env, jobject obj, jboolean value)
{
  SFBool* pSFBool = static_cast<SFBool*>(getFieldValue(env, obj));
  pSFBool->set(value ? true : false);
}

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @param value ...
 */
void JNICALL 
Java_vrml_field_SFBool_setValue__Lvrml_field_ConstSFBool_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFBool* pNewSFBool = static_cast<SFBool*>(getFieldValue(env, value));
  SFBool* pSFBool = static_cast<SFBool*>(getFieldValue(env, obj));
  pSFBool->set(pNewSFBool->get());
}

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @param value ...
 */
void JNICALL 
Java_vrml_field_SFBool_setValue__Lvrml_field_SFBool_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFBool_setValue__Lvrml_field_ConstSFBool_2(env, obj, value);
}

/**
 * @brief ...
 *
 * ...
 *
 * @param env ...
 * @param obj ...
 * @return ...
 */
jstring JNICALL Java_vrml_field_SFBool_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFColor
//

void JNICALL Java_vrml_field_ConstSFColor_CreateObject
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  SFColor* pSFColor = new SFColor(r, g, b);
  env->SetIntField(obj, fid, (int)pSFColor);
}

void JNICALL Java_vrml_field_ConstSFColor_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  SFColor* pSFColor = static_cast<SFColor*>(getFieldValue(env, obj));
  env->SetFloatArrayRegion(jarr, 0, 3, static_cast<jfloat*>(pSFColor->get()));
}

jfloat JNICALL Java_vrml_field_ConstSFColor_getRed
  (JNIEnv *env, jobject obj)
{
  SFColor* pSFColor = static_cast<SFColor*>(getFieldValue(env, obj));
  return static_cast<jfloat>(pSFColor->getR());
}

jfloat JNICALL Java_vrml_field_ConstSFColor_getGreen
  (JNIEnv *env, jobject obj)
{
  SFColor* pSFColor = static_cast<SFColor*>(getFieldValue(env, obj));
  return static_cast<jfloat>(pSFColor->getG());
}

jfloat JNICALL Java_vrml_field_ConstSFColor_getBlue
  (JNIEnv *env, jobject obj)
{
  SFColor* pSFColor = static_cast<SFColor*>(getFieldValue(env, obj));
  return static_cast<jfloat>(pSFColor->getB());
}

jstring JNICALL Java_vrml_field_ConstSFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFColor
//

void JNICALL Java_vrml_field_SFColor_CreateObject
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  Java_vrml_field_ConstSFColor_CreateObject(env, obj, r, g, b);
}

void JNICALL Java_vrml_field_SFColor_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFColor_getValue(env, obj, jarr);
}

jfloat JNICALL Java_vrml_field_SFColor_getRed
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFColor_getRed(env, obj);
}

jfloat JNICALL Java_vrml_field_SFColor_getGreen
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFColor_getGreen(env, obj);
}

jfloat JNICALL Java_vrml_field_SFColor_getBlue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFColor_getBlue(env, obj);
}

void JNICALL Java_vrml_field_SFColor_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  SFColor* pSFColor = static_cast<SFColor*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFColor->set(static_cast<const float*>(pjf));
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_SFColor_setValue__FFF
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  const float rgb[3] = { r, g, b };
  SFColor* pSFColor = static_cast<SFColor*>(getFieldValue(env, obj));
  pSFColor->set(rgb);
}

void JNICALL 
Java_vrml_field_SFColor_setValue__Lvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFColor* pNewSFColor = static_cast<SFColor*>(getFieldValue(env, value));
  SFColor* pSFColor = static_cast<SFColor*>(getFieldValue(env, obj));
  pSFColor->set(pNewSFColor->get());
}

void JNICALL 
Java_vrml_field_SFColor_setValue__Lvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFColor_setValue__Lvrml_field_ConstSFColor_2
    (env, obj, value);
}

jstring JNICALL Java_vrml_field_SFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFFloat
//

void JNICALL Java_vrml_field_ConstSFFloat_CreateObject
  (JNIEnv *env, jobject obj, jfloat value)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  SFFloat* pSFFloat = new SFFloat((float) value);
  env->SetIntField(obj, fid, (int) pSFFloat);
}

jfloat JNICALL Java_vrml_field_ConstSFFloat_getValue
  (JNIEnv *env, jobject obj)
{
  SFFloat* pSFFloat = (SFFloat*) getFieldValue(env, obj);
  return static_cast<jfloat>(pSFFloat->get());
}

jstring JNICALL Java_vrml_field_ConstSFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFFloat
//

void JNICALL Java_vrml_field_SFFloat_CreateObject
  (JNIEnv *env, jobject obj, jfloat value)
{
  Java_vrml_field_ConstSFFloat_CreateObject(env, obj, value);
}

jfloat JNICALL Java_vrml_field_SFFloat_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFFloat_getValue(env, obj);
}

void JNICALL Java_vrml_field_SFFloat_setValue__F
  (JNIEnv *env, jobject obj, jfloat f)
{
  SFFloat* pSFFloat = static_cast<SFFloat*>(getFieldValue(env, obj));
  pSFFloat->set(static_cast<float>(f));
}

void JNICALL 
Java_vrml_field_SFFloat_setValue__Lvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFFloat* pSFFloat = static_cast<SFFloat*>(getFieldValue(env, obj));
  SFFloat* pNewSFFloat = static_cast<SFFloat*>(getFieldValue(env, value));
  pSFFloat->set(pNewSFFloat->get());
}

void JNICALL 
Java_vrml_field_SFFloat_setValue__Lvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFFloat_setValue__Lvrml_field_ConstSFFloat_2
    (env, obj, value);
}

jstring JNICALL Java_vrml_field_SFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFImage
//

void JNICALL Java_vrml_field_ConstSFImage_CreateObject
  (JNIEnv *env, jobject obj, jint width, jint height, 
   jint components, jbyteArray pixels)
{
  SFImage* pConstSFImage;
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");

  if (pixels != 0)
  {
    jbyte *pjb = env->GetByteArrayElements(pixels, NULL);
    pConstSFImage = new SFImage(static_cast<int>(width),
                                static_cast<int>(height),
                                static_cast<int>(components),
                                (unsigned char *)pjb);
    env->ReleaseByteArrayElements(pixels, pjb, JNI_ABORT);
  }
  else
  {
    pConstSFImage = new SFImage(static_cast<int>(width),
                                static_cast<int>(height),
                                static_cast<int>(components), 0);
  }

  env->SetIntField(obj, fid, (int) pConstSFImage);
}

jint JNICALL Java_vrml_field_ConstSFImage_getWidth
  (JNIEnv *env, jobject obj)
{
  SFImage* pConstSFImage = static_cast<SFImage*>(getFieldValue(env, obj));
  return static_cast<jint>(pConstSFImage->getWidth());
}

jint JNICALL Java_vrml_field_ConstSFImage_getHeight
  (JNIEnv *env, jobject obj)
{
  SFImage* pConstSFImage = static_cast<SFImage*>(getFieldValue(env, obj));
  return static_cast<jint>(pConstSFImage->getHeight());
}

jint JNICALL Java_vrml_field_ConstSFImage_getComponents
  (JNIEnv *env, jobject obj)
{
  SFImage* pConstSFImage = static_cast<SFImage*>(getFieldValue(env, obj));
  return static_cast<jint>(pConstSFImage->getComponents());
}

void JNICALL Java_vrml_field_ConstSFImage_getPixels
  (JNIEnv *env, jobject obj, jbyteArray pixels)
{
  SFImage* pSFImage = static_cast<SFImage*>(getFieldValue(env, obj));
  env->SetByteArrayRegion(pixels, 0, 
                          pSFImage->getWidth() * pSFImage->getHeight(),
                          (jbyte*)pSFImage->getPixels());
}

jstring JNICALL Java_vrml_field_ConstSFImage_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFImage
//

void JNICALL Java_vrml_field_SFImage_CreateObject
  (JNIEnv *env, jobject obj, jint width, jint height, 
   jint components, jbyteArray pixels)
{
  Java_vrml_field_ConstSFImage_CreateObject(env, obj, width, height, 
                                            components, pixels);
}

jint JNICALL Java_vrml_field_SFImage_getWidth
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFImage_getWidth(env, obj);
}

jint JNICALL Java_vrml_field_SFImage_getHeight
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFImage_getHeight(env, obj);
}

jint JNICALL Java_vrml_field_SFImage_getComponents
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFImage_getComponents(env, obj);
}

void JNICALL Java_vrml_field_SFImage_getPixels
  (JNIEnv *env, jobject obj, jbyteArray pixels)
{
  Java_vrml_field_ConstSFImage_getPixels(env, obj, pixels);
}

void JNICALL Java_vrml_field_SFImage_setValue__III_3B
  (JNIEnv *env, jobject obj, jint width, jint height, 
   jint components, jbyteArray pixels)
{
  SFImage* pSFImage = static_cast<SFImage*>(getFieldValue(env, obj));
  jbyte *pjb = env->GetByteArrayElements(pixels, NULL); 
  pSFImage->set(static_cast<int>(width), static_cast<int>(height),
                static_cast<int>(components),
                (unsigned char *)pjb);
  env->ReleaseByteArrayElements(pixels, pjb, JNI_ABORT);
}

void JNICALL 
Java_vrml_field_SFImage_setValue__Lvrml_field_ConstSFImage_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFImage* pSFImage = static_cast<SFImage*>(getFieldValue(env, obj));
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  SFImage* pNewSFImage = (SFImage*)env->GetIntField(value, fid);
  *pSFImage = *pNewSFImage;
}

void JNICALL 
Java_vrml_field_SFImage_setValue__Lvrml_field_SFImage_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFImage_setValue__Lvrml_field_ConstSFImage_2(env, obj, 
                                                               value);
}

jstring JNICALL Java_vrml_field_SFImage_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFInt32
//

void JNICALL Java_vrml_field_ConstSFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint value)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  SFInt32* pSFInt32 = new SFInt32(static_cast<int>(value));
  env->SetIntField(obj, fid, (int) pSFInt32);
}

jint JNICALL Java_vrml_field_ConstSFInt32_getValue
  (JNIEnv *env, jobject obj)
{
  SFInt32* pSFInt32 = static_cast<SFInt32*>(getFieldValue(env, obj));
  return static_cast<int>(pSFInt32->get());
}

jstring JNICALL Java_vrml_field_ConstSFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFInt32
//

void JNICALL Java_vrml_field_SFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint value)
{
  Java_vrml_field_ConstSFInt32_CreateObject(env, obj, value);
}

jint JNICALL Java_vrml_field_SFInt32_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFInt32_getValue(env, obj);
}

void JNICALL Java_vrml_field_SFInt32_setValue__I
  (JNIEnv *env, jobject obj, jint value)
{
  SFInt32* pSFInt32 = static_cast<SFInt32*>(getFieldValue(env, obj));
  pSFInt32->set(static_cast<int>(value));
}

void JNICALL 
Java_vrml_field_SFInt32_setValue__Lvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFInt32* pSFInt32 = static_cast<SFInt32*>(getFieldValue(env, obj));
  SFInt32* pNewSFInt32 = static_cast<SFInt32*>(getFieldValue(env, value));
  pSFInt32->set(pNewSFInt32->get());
}

void JNICALL 
Java_vrml_field_SFInt32_setValue__Lvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFInt32_setValue__Lvrml_field_ConstSFInt32_2
    (env, obj, value);
}

jstring JNICALL Java_vrml_field_SFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFNode
//

void JNICALL Java_vrml_field_ConstSFNode_CreateObject
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid = getFid(env, value, "NodePtr", "I");
  Node* pBaseNode = (Node*) env->GetIntField(value, fid);
  SFNode* pSFNode = new SFNode(NodePtr(pBaseNode));
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFNode);
}

jobject JNICALL Java_vrml_field_ConstSFNode_getValue
  (JNIEnv *env, jobject obj)
{
  SFNode* pSFNode = static_cast<SFNode*>(getFieldValue(env, obj));
  jclass clazz = env->FindClass("vrml/node/Node");
  jobject jNode = env->AllocObject(clazz);
  jfieldID fid = env->GetFieldID(clazz, "NodePtr", "I");
  env->SetIntField(jNode, fid, (int)pSFNode->get().get());
  return jNode;
}

jstring JNICALL Java_vrml_field_ConstSFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFNode
//

void JNICALL Java_vrml_field_SFNode_CreateObject
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_ConstSFNode_CreateObject(env, obj, value);
}

jobject JNICALL Java_vrml_field_SFNode_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFNode_getValue(env, obj);
}

void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFNode* pSFNode = static_cast<SFNode*>(getFieldValue(env, obj));
  jfieldID fid = getFid(env, value, "NodePtr", "I");
  Node* pNode = (Node*)env->GetIntField(value, fid);
  pSFNode->set(NodePtr(pNode));
}

void JNICALL 
Java_vrml_field_SFNode_setValue__Lvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFNode* pSFNode = static_cast<SFNode*>(getFieldValue(env, obj));
  SFNode* pNewSFNode = static_cast<SFNode*>(getFieldValue(env, value));
  *pSFNode = *pNewSFNode;
}

void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFNode_setValue__Lvrml_field_ConstSFNode_2(env, obj, value);
}

jstring JNICALL Java_vrml_field_SFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFRotation
//

void JNICALL Java_vrml_field_ConstSFRotation_CreateObject
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY, 
   jfloat axisZ, jfloat angle)
{
  SFRotation* pSFRotation = new SFRotation(axisX, axisY, 
                                           axisZ, angle);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFRotation);
}

void JNICALL Java_vrml_field_ConstSFRotation_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  SFRotation* pSFRotation = static_cast<SFRotation*>(getFieldValue(env, obj));
  env->SetFloatArrayRegion(jarr, 0, 4,
                           static_cast<jfloat*>(pSFRotation->get()));
}

jstring JNICALL Java_vrml_field_ConstSFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFRotation
//

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

void JNICALL Java_vrml_field_SFRotation_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  SFRotation* pSFRotation = static_cast<SFRotation*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFRotation->set(pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_SFRotation_setValue__FFFF
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY, 
   jfloat axisZ, jfloat angle)
{
  SFRotation* pSFRotation = static_cast<SFRotation*>(getFieldValue(env, obj));
  const float rotation[] = { axisX, axisY, axisZ, angle };
  pSFRotation->set(rotation);
}

void JNICALL 
Java_vrml_field_SFRotation_setValue__Lvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFRotation* pSFRotation = static_cast<SFRotation*>(getFieldValue(env, obj));
  SFRotation* pNewSFRotation =
    static_cast<SFRotation*>(getFieldValue(env, value));
  pSFRotation->set(pNewSFRotation->get());
}

void JNICALL 
Java_vrml_field_SFRotation_setValue__Lvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFRotation_setValue__Lvrml_field_ConstSFRotation_2
    (env, obj, value);
}

jstring JNICALL Java_vrml_field_SFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}


//
// ConstSFString
//

void JNICALL Java_vrml_field_ConstSFString_CreateObject
  (JNIEnv *env, jobject obj, jstring jstr)
{
  SFString* pSFString;

  // Need to handle null strings
  if (jstr != 0)
  {
    const char* str = env->GetStringUTFChars(jstr, 0);
    pSFString = new SFString(std::string(str));
    env->ReleaseStringUTFChars(jstr, str);
  }
  else
  {
    std::string str;
    pSFString = new SFString(std::string());
  }

  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFString);
}

jstring JNICALL Java_vrml_field_ConstSFString_getValue
  (JNIEnv *env, jobject obj)
{
  SFString* pSFString = (SFString*) getFieldValue(env, obj);
  return env->NewStringUTF((pSFString->get()).c_str());
}

jstring JNICALL Java_vrml_field_ConstSFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFString
//

void JNICALL Java_vrml_field_SFString_CreateObject
  (JNIEnv *env, jobject obj, jstring jstr)
{
  Java_vrml_field_ConstSFString_CreateObject(env, obj, jstr);
}

jstring JNICALL Java_vrml_field_SFString_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFString_getValue(env, obj);
}

void JNICALL Java_vrml_field_SFString_setValue__Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jstring jstr)
{
  SFString* pSFString = static_cast<SFString*>(getFieldValue(env, obj));
  const char *str = env->GetStringUTFChars(jstr, 0);
  pSFString->set(str);
  env->ReleaseStringUTFChars(jstr, str);
}

void JNICALL 
Java_vrml_field_SFString_setValue__Lvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFString* pSFString = static_cast<SFString*>(getFieldValue(env, obj));
  SFString* pNewSFString = static_cast<SFString*>(getFieldValue(env, value));
  pSFString->set(pNewSFString->get());
}

void JNICALL 
Java_vrml_field_SFString_setValue__Lvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFString_setValue__Lvrml_field_ConstSFString_2(env, obj, 
                                                                 value);
}

jstring JNICALL Java_vrml_field_SFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFTime
//

void JNICALL Java_vrml_field_ConstSFTime_CreateObject
  (JNIEnv *env, jobject obj, jdouble time)
{
  SFTime* pSFTime = new SFTime(time);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFTime);
}


jdouble JNICALL Java_vrml_field_ConstSFTime_getValue
  (JNIEnv *env, jobject obj)
{
  SFTime* pSFTime = static_cast<SFTime*>(getFieldValue(env, obj));
  return static_cast<jdouble>(pSFTime->get());
}

jstring JNICALL Java_vrml_field_ConstSFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFTime
//

void JNICALL Java_vrml_field_SFTime_CreateObject
  (JNIEnv *env, jobject obj, jdouble time)
{
  Java_vrml_field_ConstSFTime_CreateObject(env, obj, time);
}


jdouble JNICALL Java_vrml_field_SFTime_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFTime_getValue(env, obj);
}

void JNICALL Java_vrml_field_SFTime_setValue__D
  (JNIEnv *env, jobject obj, jdouble time)
{
  SFTime* pSFTime = static_cast<SFTime*>(getFieldValue(env, obj));
  pSFTime->set(static_cast<double>(time));
}


void JNICALL 
Java_vrml_field_SFTime_setValue__Lvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFTime* pSFTime = static_cast<SFTime*>(getFieldValue(env, obj));
  SFTime* pNewSFTime = static_cast<SFTime*>(getFieldValue(env, value));
  pSFTime->set(pNewSFTime->get());
}

void JNICALL Java_vrml_field_SFTime_setValue__Lvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFTime_setValue__Lvrml_field_ConstSFTime_2(env, obj, 
                                                             value);
}

jstring JNICALL Java_vrml_field_SFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFVec2f
//

void JNICALL Java_vrml_field_ConstSFVec2f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  SFVec2f* pSFVec2f = new SFVec2f(x, y);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFVec2f);
}

void JNICALL Java_vrml_field_ConstSFVec2f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  SFVec2f* pSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, obj));
  env->SetFloatArrayRegion(jarr, 0, 2, static_cast<jfloat*>(pSFVec2f->get()));
}

jfloat JNICALL Java_vrml_field_ConstSFVec2f_getX
  (JNIEnv *env, jobject obj)
{
  SFVec2f* pSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, obj));
  return static_cast<jfloat>(pSFVec2f->getX());
}

jfloat JNICALL Java_vrml_field_ConstSFVec2f_getY
  (JNIEnv *env, jobject obj)
{
  SFVec2f* pSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, obj));
  return static_cast<jfloat>(pSFVec2f->getY());
}

jstring JNICALL Java_vrml_field_ConstSFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFVec2f
//

void JNICALL Java_vrml_field_SFVec2f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  Java_vrml_field_ConstSFVec2f_CreateObject(env, obj, x, y);
}

void JNICALL Java_vrml_field_SFVec2f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFVec2f_getValue(env, obj, jarr);
}

jfloat JNICALL Java_vrml_field_SFVec2f_getX
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec2f_getX(env, obj);
}

jfloat JNICALL Java_vrml_field_SFVec2f_getY
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec2f_getY(env, obj);
}

void JNICALL Java_vrml_field_SFVec2f_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  SFVec2f* pSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFVec2f->set(pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_SFVec2f_setValue__FF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  SFVec2f* pSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, obj));
  const float vec2f[] = { x, y };
  pSFVec2f->set(vec2f);
}

void JNICALL 
Java_vrml_field_SFVec2f_setValue__Lvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFVec2f* pSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, obj));
  SFVec2f* pNewSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, value));
  pSFVec2f->set(pNewSFVec2f->get());
}

void JNICALL 
Java_vrml_field_SFVec2f_setValue__Lvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFVec2f_setValue__Lvrml_field_ConstSFVec2f_2(env, obj, 
                                                               value);
}

jstring JNICALL Java_vrml_field_SFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFVec3f
//

void JNICALL Java_vrml_field_ConstSFVec3f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  SFVec3f* pSFVec3f = new SFVec3f(x, y, z);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFVec3f);
}

void JNICALL Java_vrml_field_ConstSFVec3f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  SFVec3f* pSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, obj));
  env->SetFloatArrayRegion(jarr, 0, 3, static_cast<jfloat*>(pSFVec3f->get()));
}

jfloat JNICALL Java_vrml_field_ConstSFVec3f_getX
  (JNIEnv *env, jobject obj)
{
  SFVec3f* pSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, obj));
  return static_cast<jfloat>(pSFVec3f->getX());
}

jfloat JNICALL Java_vrml_field_ConstSFVec3f_getY
  (JNIEnv *env, jobject obj)
{
  SFVec3f* pSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, obj));
  return static_cast<jfloat>(pSFVec3f->getY());
}

jfloat JNICALL Java_vrml_field_ConstSFVec3f_getZ
  (JNIEnv *env, jobject obj)
{
  SFVec3f* pSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, obj));
  return static_cast<jfloat>(pSFVec3f->getZ());
}

jstring JNICALL Java_vrml_field_ConstSFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFVec3f
//

void JNICALL Java_vrml_field_SFVec3f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  Java_vrml_field_ConstSFVec3f_CreateObject(env, obj, x, y, z);
}

void JNICALL Java_vrml_field_SFVec3f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFVec3f_getValue(env, obj, jarr);
}

jfloat JNICALL Java_vrml_field_SFVec3f_getX
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3f_getX(env, obj);
}

jfloat JNICALL Java_vrml_field_SFVec3f_getY
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3f_getY(env, obj);
}

jfloat JNICALL Java_vrml_field_SFVec3f_getZ
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3f_getZ(env, obj);
}

void JNICALL Java_vrml_field_SFVec3f_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  SFVec3f* pSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFVec3f->set(pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_SFVec3f_setValue__FFF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  SFVec3f* pSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, obj));
  const float vec3f[] = { x, y, z };
  pSFVec3f->set(vec3f);
}

void JNICALL 
Java_vrml_field_SFVec3f_setValue__Lvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  SFVec3f* pSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, obj));
  SFVec3f* pNewSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, value));
  pSFVec3f->set(pNewSFVec3f->get());
}

void JNICALL 
Java_vrml_field_SFVec3f_setValue__Lvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFVec3f_setValue__Lvrml_field_ConstSFVec3f_2(env, obj, 
                                                               value);
}

jstring JNICALL Java_vrml_field_SFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFField's
//

//
// ConstMFColor
//

void JNICALL Java_vrml_field_ConstMFColor_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  MFColor* pMFColor;

  if (size > 0)
  {
    jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
    pMFColor = new MFColor(size / 3, (const float *) pjf);
    env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  }
  else
  {
    pMFColor = new MFColor();
  }

  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFColor);
}

void JNICALL Java_vrml_field_ConstMFColor_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");

  if (jarr == 0)
  {
    MFColor* pMFColor = new MFColor();
    env->SetIntField(obj, fid, (int) pMFColor);
    return;
  }
    
  jsize nRows = env->GetArrayLength(jarr);
  float* theArray = new float[nRows * 3];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(jarr, row);
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 3; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }
  
  MFColor* pMFColor = new MFColor(nRows, theArray);
  env->SetIntField(obj, fid, (int) pMFColor);
  delete[] theArray;
}

jint JNICALL Java_vrml_field_ConstMFColor_getSize
  (JNIEnv *env, jobject obj)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  return static_cast<jint>(pMFColor->getLength());
}

void JNICALL Java_vrml_field_ConstMFColor_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  int nRows = pMFColor->getLength();
  jfloat* p = const_cast<jfloat*>(pMFColor->get());
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el =
      static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, row));
    
    env->SetFloatArrayRegion(el, 0, 3, p);
    p += 3;
  }
}

void JNICALL Java_vrml_field_ConstMFColor_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements,
                           const_cast<jfloat*>(pMFColor->get()));
}

void JNICALL Java_vrml_field_ConstMFColor_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  env->SetFloatArrayRegion(jarr, 0, 3, const_cast<jfloat*>(
                                         pMFColor->getElement(element)));
}

void JNICALL 
Java_vrml_field_ConstMFColor_get1Value__ILvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jint element, jobject sfcolor)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  const float* value = pMFColor->getElement(element);
  SFColor* pSFColor = static_cast<SFColor*>(getFieldValue(env, sfcolor));
  pSFColor->set(value);
}

jstring JNICALL Java_vrml_field_ConstMFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFColor
//

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

void JNICALL Java_vrml_field_MFColor_clear
  (JNIEnv *env, jobject obj)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  pMFColor->set(0, NULL);
}

void JNICALL Java_vrml_field_MFColor_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  pMFColor->removeElement(index);
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

void JNICALL Java_vrml_field_MFColor_setValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray value)
{
  jsize nRows = env->GetArrayLength(value);
  float* theArray = new float[nRows * 3];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el =
      static_cast<jfloatArray>(env->GetObjectArrayElement(value, row));
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 3; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }

  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  pMFColor->set(nRows, theArray);
  delete[] theArray;
}

void JNICALL Java_vrml_field_MFColor_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray value)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  jsize nels = env->GetArrayLength(value);
  pMFColor->set(nels / 3, static_cast<float*>(pjf));
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_MFColor_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFColor->set(size / 3, static_cast<float*>(pjf));
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

void JNICALL 
Java_vrml_field_MFColor_setValue__Lvrml_field_MFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  MFColor* pNewMFColor = static_cast<MFColor*>(getFieldValue(env, value));
  pMFColor->set(pNewMFColor->getLength(), pNewMFColor->get());
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
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  SFColor* pSFColor = static_cast<SFColor*>(getFieldValue(env, sfcolor));
  memcpy((void*) pMFColor->getElement(index), pSFColor->get(), 
         3 * sizeof(float));
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
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  float *p = const_cast<float*>(pMFColor->getElement(index));
  p[0] = red;
  p[1] = green;
  p[2] = blue;
}

void JNICALL 
Java_vrml_field_MFColor_addValue__Lvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jobject sfcolor)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  SFColor* pSFColor = static_cast<SFColor*>(getFieldValue(env, sfcolor));
  pMFColor->insertElement(pMFColor->getLength(), pSFColor->get());
}

void JNICALL 
Java_vrml_field_MFColor_addValue__Lvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jobject sfcolor)
{
  Java_vrml_field_MFColor_addValue__Lvrml_field_ConstSFColor_2(env, obj, 
                                                               sfcolor);
}

void JNICALL Java_vrml_field_MFColor_addValue__FFF
  (JNIEnv *env, jobject obj, jfloat red, jfloat green, jfloat blue)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  float newData[3];
  newData[0] = red;
  newData[1] = green;
  newData[2] = blue;
  pMFColor->insertElement(pMFColor->getLength(), newData);
}

void JNICALL 
Java_vrml_field_MFColor_insertValue__ILvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jint index, jobject sfcolor)
{
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  SFColor* pSFColor = static_cast<SFColor*>(getFieldValue(env, sfcolor));
  pMFColor->insertElement(index, pSFColor->get());
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
  MFColor* pMFColor = static_cast<MFColor*>(getFieldValue(env, obj));
  float newData[3];
  newData[0] = red;
  newData[1] = green;
  newData[2] = blue;
  pMFColor->insertElement(index, newData);
}

jstring JNICALL Java_vrml_field_MFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFFloat
//

void JNICALL Java_vrml_field_ConstMFFloat_CreateObject
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  jfloat *pjf = env->GetFloatArrayElements(value, NULL);
  MFFloat* pMFFloat = new MFFloat(size, const_cast<float const*>(pjf));
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFFloat);
}

jint JNICALL Java_vrml_field_ConstMFFloat_getSize
  (JNIEnv *env, jobject obj)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  return static_cast<jint>(pMFFloat->getLength());
}

void JNICALL Java_vrml_field_ConstMFFloat_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  env->SetFloatArrayRegion(jarr, 0, pMFFloat->getLength(), 
                           const_cast<jfloat*>(pMFFloat->get()));
}

jfloat JNICALL Java_vrml_field_ConstMFFloat_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  return static_cast<jfloat>(pMFFloat->getElement(index));
}

jstring JNICALL Java_vrml_field_ConstMFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFFloat
//
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

void JNICALL Java_vrml_field_MFFloat_clear
  (JNIEnv *env, jobject obj)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  pMFFloat->set(0, NULL);
}

void JNICALL Java_vrml_field_MFFloat_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  pMFFloat->removeElement(index);
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

void JNICALL Java_vrml_field_MFFloat_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray value)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  jsize nels = env->GetArrayLength(value);
  pMFFloat->set(nels, const_cast<const float*>(pjf));
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_MFFloat_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFFloat->set(size, const_cast<const float*>(pjf));
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_MFFloat_setValue__Lvrml_field_MFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  MFFloat* pNewMFFloat = static_cast<MFFloat*>(getFieldValue(env, value));
  pMFFloat->set(pNewMFFloat->getLength(), pNewMFFloat->get());
}

void JNICALL 
Java_vrml_field_MFFloat_setValue__Lvrml_field_ConstMFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFFloat_setValue__Lvrml_field_MFFloat_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFFloat_set1Value__IF
  (JNIEnv *env, jobject obj, jint index, jfloat value)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  pMFFloat->setElement(index, value);
}

void JNICALL 
Java_vrml_field_MFFloat_set1Value__ILvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject sffloat)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  SFFloat* pSFFloat = static_cast<SFFloat*>(getFieldValue(env, sffloat));
  pMFFloat->setElement(index, pSFFloat->get());
}

void JNICALL 
Java_vrml_field_MFFloat_set1Value__ILvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject sffloat)
{
  Java_vrml_field_MFFloat_set1Value__ILvrml_field_ConstSFFloat_2
    (env, obj, index, sffloat);
}

void JNICALL Java_vrml_field_MFFloat_addValue__F
  (JNIEnv *env, jobject obj, jfloat value)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  pMFFloat->insertElement(pMFFloat->getLength(), static_cast<float>(value));
}

void JNICALL 
Java_vrml_field_MFFloat_addValue__Lvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  SFFloat* pSFFloat = static_cast<SFFloat*>(getFieldValue(env, value));
  pMFFloat->insertElement(pMFFloat->getLength(), pSFFloat->get());
}

void JNICALL 
Java_vrml_field_MFFloat_addValue__Lvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFFloat_addValue__Lvrml_field_ConstSFFloat_2(env, obj, 
                                                               value);
}

void JNICALL Java_vrml_field_MFFloat_insertValue__IF
  (JNIEnv *env, jobject obj, jint index, jfloat value)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  pMFFloat->insertElement(index, static_cast<float>(value));
}

void JNICALL 
Java_vrml_field_MFFloat_insertValue__ILvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  MFFloat* pMFFloat = static_cast<MFFloat*>(getFieldValue(env, obj));
  SFFloat* pSFFloat = static_cast<SFFloat*>(getFieldValue(env, value));
  pMFFloat->insertElement(index, pSFFloat->get());
}

void JNICALL 
Java_vrml_field_MFFloat_insertValue__ILvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  Java_vrml_field_MFFloat_insertValue__ILvrml_field_ConstSFFloat_2
    (env, obj, index, value);
}

jstring JNICALL Java_vrml_field_MFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFInt32
//

void JNICALL Java_vrml_field_ConstMFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint size, jintArray value)
{
  jint * pji = env->GetIntArrayElements(value, NULL);
  std::vector<long> longVec(size);
  std::copy(pji, pji + size, longVec.begin());
  env->ReleaseIntArrayElements(value, pji, JNI_ABORT);
  MFInt32* pMFInt32 = new MFInt32(size, &longVec[0]);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFInt32);
}

jint JNICALL Java_vrml_field_ConstMFInt32_getSize
  (JNIEnv *env, jobject obj)
{
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  return static_cast<jint>(pMFInt32->getLength());
}

void JNICALL Java_vrml_field_ConstMFInt32_getValue
  (JNIEnv *env, jobject obj, jintArray jarr)
{
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  std::vector<jint> jintVec(pMFInt32->getLength());
  std::copy(pMFInt32->get(), pMFInt32->get() + pMFInt32->getLength(),
            jintVec.begin());
  env->SetIntArrayRegion(jarr, 0, jintVec.size(), &jintVec[0]);
}

jint JNICALL Java_vrml_field_ConstMFInt32_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  return static_cast<jint>(pMFInt32->getElement(index));
}

jstring JNICALL Java_vrml_field_ConstMFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFInt32
//

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

void JNICALL Java_vrml_field_MFInt32_clear
  (JNIEnv *env, jobject obj)
{
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  pMFInt32->set(0, NULL);
}

void JNICALL Java_vrml_field_MFInt32_delete
  (JNIEnv *env, jobject obj, jint n)
{
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  pMFInt32->removeElement(n);
}

void JNICALL Java_vrml_field_MFInt32_getValue
  (JNIEnv *env, jobject obj, jintArray jarr)
{
  Java_vrml_field_MFInt32_getValue(env, obj, jarr);
}

jint JNICALL Java_vrml_field_MFInt32_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFInt32_get1Value(env, obj, index);
}

void JNICALL Java_vrml_field_MFInt32_setValue___3I
  (JNIEnv *env, jobject obj, jintArray value)
{
  jint * pji = env->GetIntArrayElements(value, NULL); 
  const size_t size = env->GetArrayLength(value);
  std::vector<long> longVec(size);
  std::copy(pji, pji + size, longVec.begin());
  MFInt32 * pMFInt32 = static_cast<MFInt32 *>(getFieldValue(env, obj));
  pMFInt32->set(longVec.size(), &longVec[0]);
  env->ReleaseIntArrayElements(value, pji, JNI_ABORT);
}

void JNICALL Java_vrml_field_MFInt32_setValue__I_3I
  (JNIEnv *env, jobject obj, jint size, jintArray value)
{
  jint * pji = env->GetIntArrayElements(value, NULL); 
  MFInt32 * pMFInt32 = static_cast<MFInt32 *>(getFieldValue(env, obj));
  std::vector<long> longVector(size);
  std::copy(pji, pji + size, longVector.begin());
  pMFInt32->set(size, &longVector[0]);
  env->ReleaseIntArrayElements(value, pji, JNI_ABORT);
}

void JNICALL 
Java_vrml_field_MFInt32_setValue__Lvrml_field_MFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  MFInt32* pNewMFInt32 = static_cast<MFInt32*>(getFieldValue(env, value));
  pMFInt32->set(pNewMFInt32->getLength(), pNewMFInt32->get());
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
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  pMFInt32->setElement(index, value);
}

void JNICALL 
Java_vrml_field_MFInt32_set1Value__ILvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject sfint)
{
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  SFInt32* pSFInt32 = static_cast<SFInt32*>(getFieldValue(env, sfint));
  pMFInt32->setElement(index, pSFInt32->get());
}

void JNICALL 
Java_vrml_field_MFInt32_set1Value__ILvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject sfint)
{
  Java_vrml_field_MFInt32_set1Value__ILvrml_field_SFInt32_2(env, obj, 
                                                            index, sfint);
}

void JNICALL Java_vrml_field_MFInt32_addValue__I
  (JNIEnv *env, jobject obj, jint value)
{
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  pMFInt32->insertElement(pMFInt32->getLength(), static_cast<int>(value));
}

void JNICALL 
Java_vrml_field_MFInt32_addValue__Lvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  SFInt32* pSFInt32 = static_cast<SFInt32*>(getFieldValue(env, value));
  pMFInt32->insertElement(pMFInt32->getLength(), pSFInt32->get());
}

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
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  pMFInt32->insertElement(index, value);
}

void JNICALL 
Java_vrml_field_MFInt32_insertValue__ILvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  MFInt32* pMFInt32 = static_cast<MFInt32*>(getFieldValue(env, obj));
  SFInt32* pSFInt32 = static_cast<SFInt32*>(getFieldValue(env, value));
  pMFInt32->insertElement(index, pSFInt32->get());
}

void JNICALL 
Java_vrml_field_MFInt32_insertValue__ILvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  Java_vrml_field_MFInt32_insertValue__ILvrml_field_ConstSFInt32_2
    (env, obj, index, value);
}

jstring JNICALL Java_vrml_field_MFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFNode
//

void JNICALL Java_vrml_field_ConstMFNode_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
  MFNode* pMFNode;
  jfieldID fid;
  
  if (size > 0)
  {
    NodePtr* pNodePtrArray = new NodePtr[size];

    for (int i = 0; i < size; i++)
    {
      jobject jNode = env->GetObjectArrayElement(jarr, i);
      fid = getFid(env, jNode, "NodePtr", "I");
      Node* pNode = (Node*) env->GetIntField(jNode, fid);
      pNodePtrArray[i] = NodePtr(pNode);
    }

    pMFNode = new MFNode(size, pNodePtrArray);
    delete[] pNodePtrArray;
  }
  else
  {
    pMFNode = new MFNode();
  }

  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFNode);
}

jint JNICALL Java_vrml_field_ConstMFNode_getSize
  (JNIEnv *env, jobject obj)
{
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  return static_cast<jint>(pMFNode->getLength());
}

void JNICALL Java_vrml_field_ConstMFNode_getValue
  (JNIEnv *env, jobject obj, jobjectArray basenode)
{
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  int arraySize = pMFNode->getLength();
  jclass clazz = env->FindClass("vrml/node/Node");
  jobject jNode;
  jfieldID fid = env->GetFieldID(clazz, "NodePtr", "I");

  for (int pos = 0; pos < arraySize; pos++)
  {
    jNode = env->AllocObject(clazz);
    env->SetIntField(jNode, fid, (int) pMFNode->getElement(pos).get());    
    env->SetObjectArrayElement(basenode, pos, jNode);
  }
}

jobject JNICALL Java_vrml_field_ConstMFNode_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  jclass clazz = env->FindClass("vrml/node/Node");
  jobject jNode = env->AllocObject(clazz);
  jfieldID fid = env->GetFieldID(clazz, "NodePtr", "I");
  env->SetIntField(jNode, fid, (int) pMFNode->getElement(index).get());
  return jNode;
}

jstring JNICALL Java_vrml_field_ConstMFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFNode
//

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
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  pMFNode->clear();
}

void JNICALL Java_vrml_field_MFNode_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  pMFNode->removeElement(index);
}

void JNICALL Java_vrml_field_MFNode_getValue
  (JNIEnv *env, jobject obj, jobjectArray basenode)
{
  Java_vrml_field_ConstMFNode_getValue(env, obj, basenode);
}

jobject JNICALL Java_vrml_field_MFNode_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFNode_get1Value(env, obj, index);
}

void JNICALL Java_vrml_field_MFNode_setValue__I_3Lvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jint size, jobjectArray basenode)
{
  cout << "TODO: Implement Java_vrml_field_MFNode_setValue__I_3Lvrml_BaseNode_2" << endl;
}

void JNICALL Java_vrml_field_MFNode_setValue__Lvrml_field_MFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  MFNode* pNewMFNode = static_cast<MFNode*>(getFieldValue(env, value));
  *pMFNode = *pNewMFNode;
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
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  jfieldID fid = getFid(env, value, "NodePtr", "I");
  Node* pNewNode = (Node*) env->GetIntField(value, fid);
  pMFNode->setElement(pos, NodePtr(pNewNode)); 
}

void JNICALL 
Java_vrml_field_MFNode_set1Value__ILvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject sfnode)
{
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  SFNode* pSFNode = static_cast<SFNode*>(getFieldValue(env, sfnode));
  pMFNode->setElement(index, pSFNode->get()); 
}

void JNICALL 
Java_vrml_field_MFNode_set1Value__ILvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject sfnode)
{
  Java_vrml_field_MFNode_set1Value__ILvrml_field_ConstSFNode_2(env, obj, index, sfnode);
}

void JNICALL Java_vrml_field_MFNode_addValue__Lvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  jfieldID fid = getFid(env, value, "NodePtr", "I");
  Node* pNewNode = (Node*) env->GetIntField(value, fid);
  pMFNode->addNode(NodePtr(pNewNode));
}

void JNICALL 
Java_vrml_field_MFNode_addValue__Lvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  SFNode* pSFNode = static_cast<SFNode*>(getFieldValue(env, value));
  pMFNode->addNode(pSFNode->get());
}

void JNICALL Java_vrml_field_MFNode_addValue__Lvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFNode_addValue__Lvrml_field_ConstSFNode_2(env, obj, value);
}

void JNICALL Java_vrml_field_MFNode_insertValue__ILvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  jfieldID fid = getFid(env, value, "NodePtr", "I");
  Node* pNewNode = (Node*) env->GetIntField(value, fid);
  pMFNode->insertElement(index, NodePtr(pNewNode)); 
}

void JNICALL 
Java_vrml_field_MFNode_insertValue__ILvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  MFNode* pMFNode = static_cast<MFNode*>(getFieldValue(env, obj));
  SFNode* pSFNode = static_cast<SFNode*>(getFieldValue(env, value));
  pMFNode->insertElement(index, pSFNode->get()); 
}

void JNICALL 
Java_vrml_field_MFNode_insertValue__ILvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  Java_vrml_field_MFNode_insertValue__ILvrml_field_ConstSFNode_2
    (env, obj, index, value);
}

jstring JNICALL Java_vrml_field_MFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFRotation
//

void JNICALL Java_vrml_field_ConstMFRotation_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jsize nRows = env->GetArrayLength(jarr);
  float* theArray = new float[nRows * 4];
  float* p = theArray;

  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el =
      static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, row));
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 4; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }
  
  MFRotation* pMFRotation = new MFRotation(nRows, theArray);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
  delete[] theArray;
}

void JNICALL Java_vrml_field_ConstMFRotation_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
  MFRotation* pMFRotation =
    new MFRotation(size / 4, (const float *) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
}

jint JNICALL Java_vrml_field_ConstMFRotation_getSize
  (JNIEnv *env, jobject obj)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  return static_cast<jint>(pMFRotation->getLength());
}

void JNICALL Java_vrml_field_ConstMFRotation_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  
  int nRows = pMFRotation->getLength();
  jfloat* p = const_cast<jfloat*>(pMFRotation->get());
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el =
      static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, row));
    env->SetFloatArrayRegion(el, 0, 4, p);
    p += 4;
  }
}

void JNICALL Java_vrml_field_ConstMFRotation_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements,
                           const_cast<jfloat*>(pMFRotation->get()));
}

void JNICALL Java_vrml_field_ConstMFRotation_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  env->SetFloatArrayRegion(jarr, 0, 4, const_cast<jfloat*>(
                                         pMFRotation->getElement(element)));
}

void JNICALL 
Java_vrml_field_ConstMFRotation_get1Value__ILvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jint element, jobject sfrotation)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  const float* value = pMFRotation->getElement(element);
  SFRotation* pSFRotation =
    static_cast<SFRotation*>(getFieldValue(env, sfrotation));
  pSFRotation->set(value);
}

jstring JNICALL Java_vrml_field_ConstMFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}


//
// MFRotation
//

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

void JNICALL Java_vrml_field_MFRotation_clear
  (JNIEnv *env, jobject obj)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  pMFRotation->set(0, NULL);
}

void JNICALL Java_vrml_field_MFRotation_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  pMFRotation->removeElement(index);
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

void JNICALL Java_vrml_field_MFRotation_setValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray value)
{
  jsize nRows = env->GetArrayLength(value);
  float* theArray = new float[nRows * 4];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el =
      static_cast<jfloatArray>(env->GetObjectArrayElement(value, row));
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 4; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }

  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  pMFRotation->set(nRows, theArray);
  delete[] theArray;
}

void JNICALL Java_vrml_field_MFRotation_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray value)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  jsize nels = env->GetArrayLength(value);
  pMFRotation->set(nels / 4, const_cast<const float*>(pjf));
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

void JNICALL Java_vrml_field_MFRotation_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFRotation->set(size / 4, const_cast<const float*>(pjf));
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

void JNICALL 
Java_vrml_field_MFRotation_setValue__Lvrml_field_MFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  MFRotation* pNewMFRotation =
    static_cast<MFRotation*>(getFieldValue(env, value));
  pMFRotation->set(pNewMFRotation->getLength(), pNewMFRotation->get());
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
  (JNIEnv *env, jobject obj, jint index, jobject sfrotation)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  SFRotation* pSFRotation =
    static_cast<SFRotation*>(getFieldValue(env, sfrotation));
  memcpy((void*) pMFRotation->getElement(index), pSFRotation->get(),
         4 * sizeof(float));
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
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  float *p = const_cast<float*>(pMFRotation->getElement(index));
  p[0] = axisX;
  p[1] = axisY;
  p[2] = axisZ;
  p[3] = angle;
}

void JNICALL 
Java_vrml_field_MFRotation_addValue__Lvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jobject sfrotation)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  SFRotation* pSFRotation =
    static_cast<SFRotation*>(getFieldValue(env, sfrotation));
  pMFRotation->insertElement(pMFRotation->getLength(), 
                             const_cast<float*>(pSFRotation->get()));
}

void JNICALL 
Java_vrml_field_MFRotation_addValue__Lvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jobject sfrotation)
{
  Java_vrml_field_MFRotation_addValue__Lvrml_field_ConstSFRotation_2
    (env, obj, sfrotation);
}

void JNICALL Java_vrml_field_MFRotation_addValue__FFFF
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY, 
   jfloat axisZ, jfloat angle)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  float newData[4];
  newData[0] = axisX;
  newData[1] = axisY;
  newData[2] = axisZ;
  newData[3] = angle;
  pMFRotation->insertElement(pMFRotation->getLength(), newData);
}

void JNICALL 
Java_vrml_field_MFRotation_insertValue__ILvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotation)
{
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  SFRotation* pSFRotation =
    static_cast<SFRotation*>(getFieldValue(env, sfrotation));
  pMFRotation->insertElement(index, pSFRotation->get());
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
  MFRotation* pMFRotation = static_cast<MFRotation*>(getFieldValue(env, obj));
  float newData[4];
  newData[0] = axisX;
  newData[1] = axisY;
  newData[2] = axisZ;
  newData[3] = angle;
  pMFRotation->insertElement(index, newData);
}

jstring JNICALL Java_vrml_field_MFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFString
//

void JNICALL Java_vrml_field_ConstMFString_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
  MFString* pMFString;
  
  if (size > 0)
  {
    std::string* pStringArray = new std::string[size];

    for (int i = 0; i < size; i++)
    {
      jstring el = (jstring) env->GetObjectArrayElement(jarr, i);
      const char* temp = env->GetStringUTFChars(el, 0);
      pStringArray[i] = std::string(temp);
      env->ReleaseStringUTFChars(el, temp);
    }

    pMFString = new MFString(size, pStringArray);

    delete[] pStringArray;
  }
  else
  {
    pMFString = new MFString();
  }

  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFString);
}

jint JNICALL Java_vrml_field_ConstMFString_getSize
  (JNIEnv *env, jobject obj)
{
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  return static_cast<jint>(pMFString->getLength());
}

void JNICALL Java_vrml_field_ConstMFString_getValue
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  for (int i = 0; i < pMFString->getLength(); i++)
  {
    jstring jstr;
    const char* str = (pMFString->getElement(i)).c_str();
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
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  jstring jstr = env->NewStringUTF((pMFString->getElement(index)).c_str()); 
  return jstr;
}

jstring JNICALL Java_vrml_field_ConstMFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFString
//

void JNICALL Java_vrml_field_MFString_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
  Java_vrml_field_ConstMFString_CreateObject(env, obj, size, jarr);
}

jint JNICALL Java_vrml_field_MFString_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_MFString_getSize(env, obj);
}

void JNICALL Java_vrml_field_MFString_clear
  (JNIEnv *env, jobject obj)
{
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  pMFString->setLength(0);
}

void JNICALL Java_vrml_field_MFString_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  pMFString->removeElement(index);
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
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));

  jsize entries = env->GetArrayLength(jarr);
  pMFString->setLength(entries);

  for (int pos = 0; pos < entries; pos++)
  {
    jstring jstr = static_cast<jstring>(env->GetObjectArrayElement(jarr, pos));
    const char *str = env->GetStringUTFChars(jstr, 0);
    pMFString->setElement(pos, std::string(str));
    env->ReleaseStringUTFChars(jstr, str);
  }
}

void JNICALL 
Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2
  (JNIEnv *env, jobject obj, jobject value)
{ 
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  MFString* pNewMFString = static_cast<MFString*>(getFieldValue(env, value));

  short newStringLength = pNewMFString->getLength();
  pMFString->setLength(newStringLength);

  for (int pos = 0; pos < newStringLength; pos++)
  {
    pMFString->setElement(pos, pNewMFString->getElement(pos));
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
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  pMFString->setElement(index,
			value ? env->GetStringUTFChars(value, 0) : NULL);
}

void JNICALL 
Java_vrml_field_MFString_set1Value__ILvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  SFString* pSFString = static_cast<SFString*>(getFieldValue(env, sfstring));
  pMFString->setElement(index, pSFString->get());
}

void JNICALL 
Java_vrml_field_MFString_set1Value__ILvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  Java_vrml_field_MFString_set1Value__ILvrml_field_ConstSFString_2
    (env, obj, index, sfstring);
}

void JNICALL 
Java_vrml_field_MFString_addValue__Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jstring value)
{
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  pMFString->insertElement(pMFString->getLength(), 
                           value ? env->GetStringUTFChars(value, 0) : NULL);
}

void JNICALL 
Java_vrml_field_MFString_addValue__Lvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jobject sfstring)
{
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  SFString* pSFString = static_cast<SFString*>(getFieldValue(env, sfstring));
  pMFString->insertElement(pMFString->getLength(), 
                           (pSFString->get()).c_str());
}

void JNICALL 
Java_vrml_field_MFString_addValue__Lvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jobject sfstring)
{
  Java_vrml_field_MFString_addValue__Lvrml_field_ConstSFString_2
    (env, obj, sfstring);
}

void JNICALL 
Java_vrml_field_MFString_insertValue__ILjava_lang_String_2
  (JNIEnv *env, jobject obj, jint index, jstring value)
{
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  pMFString->insertElement(index, 
                           value ? env->GetStringUTFChars(value, 0) : NULL);
}

void JNICALL 
Java_vrml_field_MFString_insertValue__ILvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  MFString* pMFString = static_cast<MFString*>(getFieldValue(env, obj));
  SFString* pSFString = static_cast<SFString*>(getFieldValue(env, sfstring));
  pMFString->insertElement(index, (pSFString->get()).c_str());
}

void JNICALL 
Java_vrml_field_MFString_insertValue__ILvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  Java_vrml_field_MFString_insertValue__ILvrml_field_ConstSFString_2
    (env, obj, index, sfstring);
}

jstring JNICALL Java_vrml_field_MFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFTime
//

void JNICALL Java_vrml_field_ConstMFTime_CreateObject
  (JNIEnv *env, jobject obj, jint size, jdoubleArray value)
{
  jdouble *pjd = env->GetDoubleArrayElements(value, NULL);
  MFTime* pMFTime = new MFTime(size, const_cast<const double *>(pjd));
  env->ReleaseDoubleArrayElements(value, pjd, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFTime);
}

jint JNICALL Java_vrml_field_ConstMFTime_getSize
  (JNIEnv *env, jobject obj)
{
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  return static_cast<jint>(pMFTime->getLength());
}

void JNICALL Java_vrml_field_ConstMFTime_getValue
  (JNIEnv *env, jobject obj, jdoubleArray jarr)
{
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  env->SetDoubleArrayRegion(jarr, 0, pMFTime->getLength(), 
                            const_cast<jdouble*>(pMFTime->get()));
}

jdouble JNICALL Java_vrml_field_ConstMFTime_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  return static_cast<jdouble>(pMFTime->getElement(index));
}

jstring JNICALL Java_vrml_field_ConstMFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFTime
//

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

void JNICALL Java_vrml_field_MFTime_clear
  (JNIEnv *env, jobject obj)
{
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  pMFTime->set(0, NULL);
}

void JNICALL Java_vrml_field_MFTime_delete
  (JNIEnv *env, jobject obj, jint index)
{
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  pMFTime->removeElement(index);
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

void JNICALL Java_vrml_field_MFTime_setValue__I_3D
  (JNIEnv *env, jobject obj, jint size, jdoubleArray value)
{
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  jdouble *pjd = env->GetDoubleArrayElements(value, NULL); 
  pMFTime->set(size, const_cast<const double*>(pjd));
  env->ReleaseDoubleArrayElements(value, pjd, JNI_ABORT);
}

void JNICALL 
Java_vrml_field_MFTime_setValue__Lvrml_field_MFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  MFTime* pNewMFTime = static_cast<MFTime*>(getFieldValue(env, value));
  pMFTime->set(pNewMFTime->getLength(), pNewMFTime->get());
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
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  pMFTime->setElement(index, value);
}

void JNICALL 
Java_vrml_field_MFTime_set1Value__ILvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  SFTime* pSFTime = static_cast<SFTime*>(getFieldValue(env, sftime));
  pMFTime->setElement(index, pSFTime->get());
}

void JNICALL 
Java_vrml_field_MFTime_set1Value__ILvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  Java_vrml_field_MFTime_set1Value__ILvrml_field_ConstSFTime_2
    (env, obj, index, sftime);
}

void JNICALL Java_vrml_field_MFTime_addValue__D
  (JNIEnv *env, jobject obj, jdouble value)
{
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  pMFTime->insertElement(pMFTime->getLength(), static_cast<double>(value));
}

void JNICALL 
Java_vrml_field_MFTime_addValue__Lvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jobject sftime)
{
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  SFTime* pSFTime = static_cast<SFTime*>(getFieldValue(env, sftime));
  pMFTime->insertElement(pMFTime->getLength(), pSFTime->get());
}

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
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  pMFTime->insertElement(index, value);
}

void JNICALL 
Java_vrml_field_MFTime_insertValue__ILvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  MFTime* pMFTime = static_cast<MFTime*>(getFieldValue(env, obj));
  SFTime* pSFTime = static_cast<SFTime*>(getFieldValue(env, sftime));
  pMFTime->setElement(index, pSFTime->get());
}

void JNICALL 
Java_vrml_field_MFTime_insertValue__ILvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  Java_vrml_field_MFTime_insertValue__ILvrml_field_ConstSFTime_2
    (env, obj, index, sftime);
}

jstring JNICALL Java_vrml_field_MFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFVec2f
//

void JNICALL Java_vrml_field_ConstMFVec2f_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jsize nRows = env->GetArrayLength(jarr);
  float* theArray = new float[nRows * 2];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el =
      static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, row));
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 2; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }
  
  MFRotation* pMFRotation = new MFRotation(nRows, theArray);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
  delete[] theArray;
}

void JNICALL Java_vrml_field_ConstMFVec2f_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
  MFVec2f* pMFVec2f = new MFVec2f(size / 2, (const float *) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFVec2f);
}

jint JNICALL Java_vrml_field_ConstMFVec2f_getSize
  (JNIEnv *env, jobject obj)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  return static_cast<jint>(pMFVec2f->getLength());
}

void JNICALL Java_vrml_field_ConstMFVec2f_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  MFVec2f* pMFVec2f = (MFVec2f*)getFieldValue(env, obj);
  
  int nRows = pMFVec2f->getLength();
  jfloat* p = const_cast<jfloat*>(pMFVec2f->get());
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(jarr, row);
    env->SetFloatArrayRegion(el, 0, 2, p);
    p += 2;
  }
}

void JNICALL Java_vrml_field_ConstMFVec2f_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements,
                           const_cast<jfloat*>(pMFVec2f->get()));
}


void JNICALL Java_vrml_field_ConstMFVec2f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  env->SetFloatArrayRegion(jarr, 0, 2, const_cast<jfloat*>(
                                         pMFVec2f->getElement(element)));
}

void JNICALL 
Java_vrml_field_ConstMFVec2f_get1Value__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec2f)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  const float* value = pMFVec2f->getElement(element);
  SFVec2f* pSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, sfvec2f));
  pSFVec2f->set(value);
}

jstring JNICALL Java_vrml_field_ConstMFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFVec2f
//

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

void JNICALL Java_vrml_field_MFVec2f_clear
  (JNIEnv *env, jobject obj)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  pMFVec2f->set(0, NULL);
}

void JNICALL Java_vrml_field_MFVec2f_delete
  (JNIEnv *env, jobject obj, jint n)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  pMFVec2f->removeElement(n);
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

void JNICALL Java_vrml_field_MFVec2f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec2f_get1Value__I_3F(env, obj, element, jarr);
}

void JNICALL 
Java_vrml_field_MFVec2f_get1Value__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec2f)
{
  Java_vrml_field_ConstMFVec2f_get1Value__ILvrml_field_SFVec2f_2
    (env, obj, element, sfvec2f);
}

void JNICALL Java_vrml_field_MFVec2f_setValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray value)
{
  jsize nRows = env->GetArrayLength(value);
  float* theArray = new float[nRows * 2];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el =
      static_cast<jfloatArray>(env->GetObjectArrayElement(value, row));
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 2; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }
  
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  pMFVec2f->set(nRows, theArray);
  delete[] theArray;
}

void JNICALL Java_vrml_field_MFVec2f_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFVec2f->set(size / 2, const_cast<const float*>(pjf));
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

void JNICALL 
Java_vrml_field_MFVec2f_setValue__Lvrml_field_MFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  MFVec2f* pNewMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, value));
  pMFVec2f->set(pNewMFVec2f->getLength(), pNewMFVec2f->get());
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
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  float *p = const_cast<float*>(pMFVec2f->getElement(index));
  p[0] = x;
  p[1] = y;
}

void JNICALL 
Java_vrml_field_MFVec2f_set1Value__ILvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  SFVec2f* pSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, sfvec2f));
  memcpy((void*) pMFVec2f->getElement(index), pSFVec2f->get(), 
         2 * sizeof(float));
}

void JNICALL 
Java_vrml_field_MFVec2f_set1Value__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  Java_vrml_field_MFVec2f_set1Value__ILvrml_field_ConstSFVec2f_2
    (env, obj, index, sfvec2f);
}

void JNICALL Java_vrml_field_MFVec2f_addValue__FF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  float farr[2];
  farr[0] = x;
  farr[1] = y;
  pMFVec2f->insertElement(pMFVec2f->getLength(), farr);
}

void JNICALL 
Java_vrml_field_MFVec2f_addValue__Lvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jobject sfvec2f)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  SFVec2f* pSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, sfvec2f));
  pMFVec2f->insertElement(pMFVec2f->getLength(), pSFVec2f->get());
}

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
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  float farr[2];
  farr[0] = x;
  farr[1] = y;
  pMFVec2f->insertElement(index, farr);
}

void JNICALL 
Java_vrml_field_MFVec2f_insertValue__ILvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  MFVec2f* pMFVec2f = static_cast<MFVec2f*>(getFieldValue(env, obj));
  SFVec2f* pSFVec2f = static_cast<SFVec2f*>(getFieldValue(env, sfvec2f));
  pMFVec2f->insertElement(index, pSFVec2f->get());
}

void JNICALL 
Java_vrml_field_MFVec2f_insertValue__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  Java_vrml_field_MFVec2f_insertValue__ILvrml_field_ConstSFVec2f_2
    (env, obj, index, sfvec2f);
}

jstring JNICALL Java_vrml_field_MFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFVec3f
//

void JNICALL Java_vrml_field_ConstMFVec3f_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jsize nRows = env->GetArrayLength(jarr);
  float* theArray = new float[nRows * 3];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el =
      static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, row));
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 3; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }
  
  MFRotation* pMFRotation = new MFRotation(nRows, theArray);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
  delete[] theArray;
}

void JNICALL Java_vrml_field_ConstMFVec3f_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
  MFVec3f* pMFVec3f = new MFVec3f(size / 3, (const float *) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFVec3f);
}

jint JNICALL Java_vrml_field_ConstMFVec3f_getSize
  (JNIEnv *env, jobject obj)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  return static_cast<jint>(pMFVec3f->getLength());
}

void JNICALL Java_vrml_field_ConstMFVec3f_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  
  int nRows = pMFVec3f->getLength();
  jfloat* p = const_cast<jfloat*>(pMFVec3f->get());
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el =
      static_cast<jfloatArray>(env->GetObjectArrayElement(jarr, row));
    env->SetFloatArrayRegion(el, 0, 3, p);
    p += 3;
  }
}

void JNICALL Java_vrml_field_ConstMFVec3f_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements,
                           const_cast<jfloat*>(pMFVec3f->get()));
}

void JNICALL Java_vrml_field_ConstMFVec3f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  env->SetFloatArrayRegion(jarr, 0, 3, 
                           const_cast<jfloat*>(pMFVec3f->getElement(element)));
}

void JNICALL 
Java_vrml_field_ConstMFVec3f_get1Value__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec3f)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  const float* value = pMFVec3f->getElement(element);
  SFVec3f* pSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, sfvec3f));
  pSFVec3f->set(value);
}

jstring JNICALL Java_vrml_field_ConstMFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFVec3f
//

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

void JNICALL Java_vrml_field_MFVec3f_clear
  (JNIEnv *env, jobject obj)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  pMFVec3f->set(0, NULL);
}

void JNICALL Java_vrml_field_MFVec3f_delete
  (JNIEnv *env, jobject obj, jint n)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  pMFVec3f->removeElement(n);
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

void JNICALL Java_vrml_field_MFVec3f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec3f_get1Value__I_3F(env, obj, element, jarr);
}

void JNICALL 
Java_vrml_field_MFVec3f_get1Value__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec3f)
{
  Java_vrml_field_ConstMFVec3f_get1Value__ILvrml_field_SFVec3f_2
    (env, obj, element, sfvec3f);
}

void JNICALL Java_vrml_field_MFVec3f_setValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray value)
{
  jsize nRows = env->GetArrayLength(value);
  float* theArray = new float[nRows * 3];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el =
      static_cast<jfloatArray>(env->GetObjectArrayElement(value, row));
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 3; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }
  
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  pMFVec3f->set(nRows, theArray);
  delete[] theArray;
}

void JNICALL Java_vrml_field_MFVec3f_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFVec3f->set(size / 3, const_cast<const float*>(pjf));
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

void JNICALL 
Java_vrml_field_MFVec3f_setValue__Lvrml_field_MFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  MFVec3f* pNewMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, value));
  pMFVec3f->set(pNewMFVec3f->getLength(), pNewMFVec3f->get());
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
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  float *p = const_cast<float*>(pMFVec3f->getElement(index));
  p[0] = x;
  p[1] = y;
  p[2] = z;
}

void JNICALL 
Java_vrml_field_MFVec3f_set1Value__ILvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  SFVec3f* pSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, sfvec3f));
  memcpy((void*) pMFVec3f->getElement(index), pSFVec3f->get(),
	 3 * sizeof(float));
}

void JNICALL 
Java_vrml_field_MFVec3f_set1Value__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  Java_vrml_field_MFVec3f_set1Value__ILvrml_field_ConstSFVec3f_2
    (env, obj, index, sfvec3f);
}

void JNICALL Java_vrml_field_MFVec3f_addValue__FFF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  float farr[3];
  farr[0] = x;
  farr[1] = y;
  farr[2] = z;
  pMFVec3f->insertElement(pMFVec3f->getLength(), farr);
}

void JNICALL 
Java_vrml_field_MFVec3f_addValue__Lvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jobject sfvec3f)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  SFVec3f* pSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, sfvec3f));
  pMFVec3f->insertElement(pMFVec3f->getLength(), pSFVec3f->get());
}

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
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  float farr[3];
  farr[0] = x;
  farr[1] = y;
  farr[2] = z;
  pMFVec3f->insertElement(index, farr);
}

void JNICALL 
Java_vrml_field_MFVec3f_insertValue__ILvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  MFVec3f* pMFVec3f = static_cast<MFVec3f*>(getFieldValue(env, obj));
  SFVec3f* pSFVec3f = static_cast<SFVec3f*>(getFieldValue(env, sfvec3f));
  pMFVec3f->insertElement(index, pSFVec3f->get());
}

void JNICALL 
Java_vrml_field_MFVec3f_insertValue__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  Java_vrml_field_MFVec3f_insertValue__ILvrml_field_ConstSFVec3f_2
    (env, obj, index, sfvec3f);
}

jstring JNICALL Java_vrml_field_MFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// Script
//

jobject JNICALL Java_vrml_node_Script_getField
  (JNIEnv *env, jobject obj, jstring jstrFieldName)
{
  const char *charFieldName = env->GetStringUTFChars(jstrFieldName, 0);
  std::string fieldName(charFieldName);
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  ScriptNode* pScript = (ScriptNode*) env->GetIntField(obj, fid);
  jobject Field;

  ScriptNode::FieldValueMap fieldMap = pScript->getFieldValueMap();
  ScriptNode::FieldValueMap::iterator iter = fieldMap.find(fieldName);

  if (iter != fieldMap.end())
  {
    // Then we've found the field
    const FieldValue* fieldPtr = iter->second.get();
    ostrstream os;
    os << "vrml/field/" << fieldPtr->type() << '\0';
    jclass clazz = env->FindClass(os.str());
    os.rdbuf()->freeze(false);
    Field = env->AllocObject(clazz);
    fid = getFid(env, Field, "FieldPtr", "I");
    env->SetIntField(Field, fid, (int) fieldPtr);
  }
  else
  {
    // Field doesn't exist, throw an exception
    // TODO: Add exception
    return 0;  
  }

  fid = getFid(env, Field,"isExposedField", "Z");
  env->SetBooleanField(Field, fid, true);
  env->ReleaseStringUTFChars(jstrFieldName, charFieldName );
  return Field;
}

jobject JNICALL Java_vrml_node_Script_getEventOut
  (JNIEnv *env, jobject obj, jstring jstrEventOutName)
{
  jobject eventOut;
  jclass clazz;
  const char *charEventOut = env->GetStringUTFChars(jstrEventOutName , 0);
  std::string eventOutName(charEventOut);
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  ScriptNode* pScript = (ScriptNode*) env->GetIntField(obj, fid);

  ScriptNode::EventOutValueMap eventOutMap = pScript->getEventOutValueMap();
  ScriptNode::EventOutValueMap::iterator iter = eventOutMap.find(eventOutName);

  if (iter != eventOutMap.end())
  {
    ScriptNode::PolledEventOutValue eventOutValue(iter->second);
    // Found the eventOut
    ostrstream os;
    os << "vrml/field/" << iter->second.value->type() << '\0';
    clazz = env->FindClass(os.str());
    os.rdbuf()->freeze(false);
    eventOut = env->AllocObject(clazz);
    fid = getFid(env, eventOut, "FieldPtr", "I");
    env->SetIntField(eventOut, fid, (int) &eventOutValue);
  }
  else
  {
    // TODO: throw an exception as the given event out doesn't exist
    return 0;
  }

  fid = getFid(env, eventOut,"isEventOut", "Z");
  env->SetBooleanField(eventOut, fid, true);
  env->ReleaseStringUTFChars(jstrEventOutName, charEventOut);
  
  return eventOut;
}

jobject JNICALL Java_vrml_node_Script_getEventIn
  (JNIEnv *env, jobject obj, jstring jstrEventInName)
{
  // TODO: This method needs to be revisited
  /*
  FieldValue::Type eventInType;
  const ScriptNode::ScriptField* fieldPtr;
  jobject eventIn;
  jfieldID fid;

  const char* charEventIn = env->GetStringUTFChars(jstrEventInName , 0);
  std::string eventInName(charEventIn);

  fid = getFid(env, obj, "NodePtr", "I");
  ScriptNode* pScript = (ScriptNode*) env->GetIntField(obj, fid);
  if ((eventInType = pScript->hasEventIn(eventInName)) != 
       FieldValue::invalidType)
  {
    char clazzName[256];
    sprintf(clazzName, "vrml/field/%s", FieldValue::getFieldName(eventInType));
    jclass clazz = env->FindClass(clazzName);
    eventIn = env->AllocObject(clazz);
    fid = getFid(env, eventIn, "FieldPtr", "I");
    ScriptNode::FieldList& eventIns = pScript->eventIns();
    for (ScriptNode::FieldList::const_iterator i(eventIns.begin()); 
          i != eventIns.end(); ++i) 
    {
      if ((*i)->name == eventInName) 
      {
        fieldPtr = (*i);
      }
    }
  }

  else if ((eventInType = pScript->hasField(eventInName)) !=
            FieldValue::invalidType)
  {
    char clazzName[256];
    sprintf(clazzName, "vrml/field/%s", FieldValue::getFieldName(eventInType));
    jclass clazz = env->FindClass(clazzName);
    eventIn = env->AllocObject(clazz);
    fid = getFid(env, eventIn, "FieldPtr", "I");
    ScriptNode::FieldList& fields = pScript->fields();
    for (ScriptNode::FieldList::const_iterator i(fields.begin()); 
           i != fields.end(); ++i) 
    {
      if ((*i)->name == eventInName) 
      {
        fieldPtr = (*i);
      }
    }
  }
  else
  {
    // TODO: throw an exception as the given event in doesn't exist
    return 0;
  }

  env->SetIntField(eventIn, fid, (int) fieldPtr);
  fid = getFid(env, eventIn,"isEventIn", "Z");
  env->SetBooleanField(eventIn, fid, true);
  env->ReleaseStringUTFChars(jstrEventInName, charEventIn);
  
  return eventIn;
  */

  return 0;
}

jstring JNICALL Java_vrml_node_Script_toString
  (JNIEnv *env, jobject obj)
{
  cout << "TODO: Implement Java_vrml_node_Script_toString" << endl;

  return 0;
}

//
// BaseNode
//

/**
 * @brief JNI implementation of BaseNode::getType.
 *
 * @todo Figure out how to implement this.
 */
jstring JNICALL Java_vrml_BaseNode_getType
  (JNIEnv *env, jobject obj)
{
  cout << "TODO: Implement Java_vrml_BaseNode_getType" << endl;
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  Node* pNode = (Node*)env->GetIntField(obj, fid);
  // Haven't found a way to do this yet
  return env->NewStringUTF("unimplemented");
}

jobject JNICALL Java_vrml_BaseNode_getBrowser
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  Node* pNode = (Node*) env->GetIntField(obj, fid);
  jclass clazz = env->FindClass("vrml/Browser");
  jobject jBrowser = env->AllocObject(clazz);
  fid = getFid(env, jBrowser, "BrowserPtr", "I");
  env->SetIntField(jBrowser, fid,
                   (int) &(pNode->nodeType.nodeClass.scene));
  return jBrowser;
}

//
// Node
//

jobject JNICALL Java_vrml_node_Node_getEventIn
  (JNIEnv *env, jobject obj, jstring jstrEventInName)
{
  // TODO: This method needs to be revisited

  const char *eventInName = env->GetStringUTFChars(jstrEventInName , 0);
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  Node* pNode = (Node*) env->GetIntField(obj, fid);
  
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


jobject JNICALL Java_vrml_node_Node_getEventOut
  (JNIEnv *env, jobject obj, jstring jstrEventOutName)
{
  // TODO: This method needs to be revisited
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

jobject JNICALL Java_vrml_node_Node_getExposedField
  (JNIEnv *env, jobject obj, jstring jstrExposedFieldName)
{
  // TODO: This method needs to be revisited
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

jstring JNICALL Java_vrml_node_Node_toString
  (JNIEnv *env, jobject obj)
{
  ostrstream os;
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  Node* pNode = (Node*) env->GetIntField(obj, fid);
  pNode->printFields(os, 0);
  os << ends;
  char* szString = os.str();
  jstring result = env->NewStringUTF(szString); 
  os.rdbuf()->freeze(0);
  return result;
}

//
// Event
//

jstring JNICALL Java_vrml_Event_getName
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  VrmlEvent* pEvent = (VrmlEvent*) env->GetIntField(obj, fid);
  return env->NewStringUTF(pEvent->name());
}

jdouble JNICALL Java_vrml_Event_getTimeStamp
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  VrmlEvent* pEvent = (VrmlEvent*) env->GetIntField(obj, fid);
  return static_cast<jdouble>(pEvent->timeStamp());
}

jobject JNICALL Java_vrml_Event_getValue
  (JNIEnv *env, jobject obj)
{  
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  VrmlEvent* pEvent = (VrmlEvent*) env->GetIntField(obj, fid);
  const FieldValue* pField = pEvent->value();
  ostrstream os;
  os << "vrml/field/Const" << pField->type() << '\0';
  jclass clazz = env->FindClass(os.str());
  os.rdbuf()->freeze(false);
  jobject Field = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");
  env->SetIntField(Field, fid, (int) pField);
  return Field;
}

jobject JNICALL Java_vrml_Event_clone
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  VrmlEvent* pEvent = (VrmlEvent*) env->GetIntField(obj, fid);
  jclass clazz = env->FindClass("vrml/Event");
  jobject jCloneEvent = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "EventPtr", "I");
  VrmlEvent* pCloneEvent = pEvent->clone();
  env->SetIntField(jCloneEvent, fid, (int) pCloneEvent);
  return jCloneEvent;
}

jstring JNICALL Java_vrml_Event_toString
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  VrmlEvent* pEvent = (VrmlEvent*) env->GetIntField(obj, fid);
  return env->NewStringUTF(pEvent->name()); 
}

//
// Browser
//

jstring JNICALL Java_vrml_Browser_toString
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  
  // TODO: What should this return?

  return env->NewStringUTF("OpenVRML");
}

jstring JNICALL Java_vrml_Browser_getName
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  return env->NewStringUTF(pBrowser->getName());
}

jstring JNICALL Java_vrml_Browser_getVersion
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  return env->NewStringUTF(pBrowser->getVersion());
}

jfloat JNICALL Java_vrml_Browser_getCurrentSpeed
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  // TODO
  return static_cast<jfloat>(0.0);
}

jfloat JNICALL Java_vrml_Browser_getCurrentFrameRate
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  return static_cast<jfloat>(pBrowser->getFrameRate());
}

jstring JNICALL Java_vrml_Browser_getWorldURL
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  return env->NewStringUTF(pBrowser->urlDoc()->url());
}

void JNICALL Java_vrml_Browser_replaceWorld
  (JNIEnv *env, jobject obj, jobjectArray basenodes)
{
  // First check if mustEvaluate is set, if not then method
  // should not continue.

  cout << "TODO: Implement Java_vrml_Browser_replaceWorld" << endl;
}

jobjectArray JNICALL Java_vrml_Browser_createVrmlFromString
  (JNIEnv *env, jobject obj, jstring vrmlSyntax)
{
  jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  VrmlScene* pBrowser = (VrmlScene*) env->GetIntField(obj, fid);

  cout << "TODO: implement Java_vrml_Browser_createVrmlFromString" << endl;

  return 0;
}

void JNICALL Java_vrml_Browser_createVrmlFromURL
  (JNIEnv *env, jobject obj, jobjectArray urls, 
   jobject basenode, jstring event)
{
  cout << "TODO: Implement Java_vrml_Browser_createVrmlFromURL" << endl;
}

void JNICALL Java_vrml_Browser_addRoute
  (JNIEnv *env, jobject obj, jobject fromNode, jstring fromEventOut, 
   jobject toNode, jstring toEventIn)
{
  // TODO: First check if directOutput is set, if not then method
  // should not continue
  
  jfieldID fid = getFid(env, fromNode, "NodePtr", "I");
  Node* pFromNode = (Node*) env->GetIntField(fromNode, fid);
  fid = getFid(env, toNode, "NodePtr", "I");
  Node* pToNode = (Node*) env->GetIntField(toNode, fid);
  const char* str = env->GetStringUTFChars(fromEventOut, 0);
  std::string eventOut(str);
  env->ReleaseStringUTFChars(fromEventOut, str);
  str = env->GetStringUTFChars(toEventIn, 0);
  std::string eventIn(str);
  env->ReleaseStringUTFChars(toEventIn, str);
  pFromNode->addRoute(eventOut, NodePtr(pToNode), eventIn);
}

void JNICALL Java_vrml_Browser_deleteRoute
  (JNIEnv *env, jobject obj, jobject fromNode, jstring fromEventOut,
   jobject toNode, jstring toEventIn)
{
  // First check if directOutput is set, if not then method
  // should not continue

  jfieldID fid = getFid(env, fromNode, "NodePtr", "I");
  Node* pFromNode = (Node*) env->GetIntField(fromNode, fid);
  fid = getFid(env, toNode, "NodePtr", "I");
  Node* pToNode = (Node*) env->GetIntField(toNode, fid);
  const char* str = env->GetStringUTFChars(fromEventOut, 0);
  std::string eventOut(str);
  env->ReleaseStringUTFChars(fromEventOut, str);
  str = env->GetStringUTFChars(toEventIn, 0);
  std::string eventIn(str);
  env->ReleaseStringUTFChars(toEventIn, str);
  pFromNode->deleteRoute(eventOut, NodePtr(pToNode), eventIn);
}

void JNICALL Java_vrml_Browser_loadURL
  (JNIEnv *env, jobject obj, jobjectArray jUrlArray, 
   jobjectArray jParameterArray)
{
  int pos;

  // TODO: First check if mustEvaluate is set, if not then method
  // should not continue.

  jfieldID fid = getFid(env, obj, "BrowserPtr", "I");
  VrmlScene* browser = (VrmlScene*) env->GetIntField(obj, fid);

  jsize entries = env->GetArrayLength(jUrlArray);
  std::string *urlArray = new std::string[entries];
  for (pos = 0; pos < entries; pos++)
  {
    jstring jstr = (jstring) env->GetObjectArrayElement(jUrlArray, pos);
    const char *str = env->GetStringUTFChars(jstr, 0);
    std::string newString(str);
    urlArray[pos] = newString;
    env->ReleaseStringUTFChars(jstr, str);
  }
  MFString urls(entries, urlArray);

  entries = env->GetArrayLength(jParameterArray);
  std::string *parameterArray = new std::string[entries];
  for (pos = 0; pos < entries; pos++)
  {
    jstring jstr = (jstring) env->GetObjectArrayElement(jParameterArray, pos);
    const char *str = env->GetStringUTFChars(jstr, 0);
    std::string newString(str);
    parameterArray[pos] = newString;
    env->ReleaseStringUTFChars(jstr, str);
  }
  MFString parameters(entries, parameterArray);

  browser->loadUrl(urls, parameters);
}

void JNICALL Java_vrml_Browser_setDescription
  (JNIEnv *env, jobject obj, jstring jDescription)
{
  const char *description = env->GetStringUTFChars(jDescription, 0);
  theSystem->inform("%s", description);
  env->ReleaseStringUTFChars(jDescription, description);
}

#endif // OPENVRML_HAVE_JNI
