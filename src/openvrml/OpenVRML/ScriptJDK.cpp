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

#ifdef OPENVRML_HAVE_JAVA
#include "ScriptJDK.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "doc2.hpp"
#include "MathUtils.h"
#include "System.h"
#include "VrmlNodeScript.h"
#include "VrmlNamespace.h"
#include "nodetype.h"
#include "VrmlScene.h"
#include "VrmlEvent.h"
#include "VrmlNodePtr.h"

#include "field.h"

#ifdef _WIN32
#define PATH_SEPARATOR ";"
#else /* UNIX and macintosh */
#define PATH_SEPARATOR ":"
#endif


// Static members
JavaVM *ScriptJDK::d_jvm = 0;
JNIEnv *ScriptJDK::d_env = 0;

// Construct from classname. Assumes className.class exists in classDir

ScriptJDK::ScriptJDK( VrmlNodeScript *node,
		      const char *className,
		      const char *classDir ) :
  d_node(node),
  d_class(0),
  d_object(0),
  d_processEventsID(0),
  d_processEventID(0),
  d_eventsProcessedID(0),
  ScriptObject(*node)
{
  if (! d_jvm)			// Initialize static members
  {
    JavaVMInitArgs vm_args;
    jint res;
    JavaVMOption options[4];
    
    /* get the currently defined CLASSPATH env variable */
    char* classPath = getenv("CLASSPATH");

    ostrstream appendedClassPath;

    appendedClassPath << "-Djava.class.path=." 
		      << PATH_SEPARATOR << classDir
      		      << PATH_SEPARATOR << classPath 
#ifndef _WIN32
		      << PATH_SEPARATOR << "/usr/local/share/java/vrml.jar" 
#endif
		      << ends;
#ifdef _WIN32
    appendedClassPath.rdbuf()->freeze(false);
#else
    appendedClassPath.freeze(false);
#endif

    options[0].optionString = appendedClassPath.str();
    options[1].optionString = "-verbose:class";
    options[2].optionString = "-verbose:jni";
    options[3].optionString = "-Djava.library.path=/usr/local/lib";

    vm_args.version = JNI_VERSION_1_2;
    vm_args.options = options;
    vm_args.nOptions = 4;
    
    /* Create the Java VM */
    res = JNI_CreateJavaVM(&d_jvm, (void**) &d_env, &vm_args);
    
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
    d_env->SetIntField(d_object, fid, (int) scriptNode());

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


ScriptJDK::~ScriptJDK()
{
  if (d_jvm)
    {
      d_jvm->DestroyJavaVM();
      d_jvm = 0;
    }
}

static jfieldID getFid(JNIEnv *env, jobject obj, char *membername, char *sig)
{
  jclass clazz;
  clazz = env->GetObjectClass(obj);
  return clazz == 0 ? NULL : env->GetFieldID(clazz, membername, sig);
}

static VrmlField* getVrmlField(JNIEnv *env, jobject obj)
{
  VrmlField* fieldPtr;
  jfieldID fid;

  fid = getFid(env, obj, "isEventOut", "Z");
  bool eventOut = (bool) env->GetBooleanField(obj, fid);
  fid = getFid(env, obj, "FieldPtr", "I");

  if (eventOut)
  {
    VrmlNodeScript::ScriptField* pScriptField =
      (VrmlNodeScript::ScriptField*) env->GetIntField(obj, fid);
    fieldPtr = (VrmlField*)pScriptField->value;
    pScriptField->modified = true;
  }
  else
  {
    fieldPtr = (VrmlField*) env->GetIntField(obj, fid);
  }

  return fieldPtr;
}

static jstring fieldToString(JNIEnv *env, jobject obj)
{
  ostrstream os;
  VrmlField* pField = getVrmlField(env, obj);
  pField->print(os);
  os << ends;
  char* szString = os.str();
  jstring result = env->NewStringUTF(szString); 
  os.rdbuf()->freeze(0);
  return result;
}

// Run a specified script

void ScriptJDK::activate( double timeStamp,
			  const std::string & fname,
			  size_t argc,
			  const VrmlField * const argv[] )
{
  if (argc == 2 && d_processEventID != 0)
  {
    jclass clazz;
    jfieldID fid;
    jobject jEvent;
    VrmlEvent* pEvent;    

    clazz = d_env->FindClass("vrml/Event");

    if (clazz == 0)
    {
      theSystem->error( "Can't find Java class vrml/Event");
      return;
    }

    jEvent = d_env->AllocObject(clazz);
    fid = getFid(d_env, jEvent, "EventPtr", "I");
    pEvent = new VrmlEvent(timeStamp, fname.c_str(), argv[0]);
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
  else if (argc == 0)
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

// Get a handle to the scene from a ScriptJDK

VrmlScene *ScriptJDK::browser() { return d_node->browser(); }

extern "C" {

static VrmlField* newField(VrmlField::VrmlFieldType fieldtype)
{
  switch (fieldtype)
  {
    case VrmlField::NO_FIELD:
      return NULL;
    case VrmlField::SFBOOL:
      return new VrmlSFBool;
    case VrmlField::SFCOLOR:
      return new VrmlSFColor;
    case VrmlField::SFFLOAT:
      return new VrmlSFFloat;
    case VrmlField::SFIMAGE:
      return new VrmlSFImage;
    case VrmlField::SFINT32:
      return new VrmlSFInt32;
    case VrmlField::SFNODE:
      return new VrmlSFNode;
    case VrmlField::SFROTATION:
      return new VrmlSFRotation;
    case VrmlField::SFSTRING:
      return new VrmlSFString;
    case VrmlField::SFTIME:
      return new VrmlSFTime;
    case VrmlField::SFVEC2F:
      return new VrmlSFVec2f;
    case VrmlField::SFVEC3F:
      return new VrmlSFVec3f;
    case VrmlField::MFCOLOR:
      return new VrmlMFColor;
    case VrmlField::MFFLOAT:
      return new VrmlMFFloat;
    case VrmlField::MFINT32:
      return new VrmlMFInt32;
    case VrmlField::MFNODE:
      return new VrmlMFNode;
    case VrmlField::MFROTATION:
      return new VrmlMFRotation;
    case VrmlField::MFSTRING:
      return new VrmlMFString;
    case VrmlField::MFTIME:
      return new VrmlMFTime;
    case VrmlField::MFVEC2F:
      return new VrmlMFVec2f;
    case VrmlField::MFVEC3F:
      return new VrmlMFVec3f;
    default:
      return NULL;
  }
}

JNIEXPORT jobject JNICALL Java_vrml_Field_clone
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  jobject jCloneField;
  jclass clazz;
  VrmlField* pField;
  VrmlField* pCloneField;
  char clazzName[256];
  const char *szFieldType;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pField = (VrmlField*) env->GetIntField(obj, fid);
  szFieldType = pField->fieldTypeName();
  sprintf(clazzName, "vrml/field/%s", szFieldType);
  clazz = env->FindClass(clazzName);
  jCloneField = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");
  pCloneField = pField->clone();
  env->SetIntField(jCloneField, fid, (int) pCloneField);	
  return jCloneField;
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved)
{
  return JNI_VERSION_1_2;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved)
{
}

//
// ConstSFBool
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFBool_CreateObject
  (JNIEnv *env, jobject obj, jboolean value)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFBool* pSFBool = new VrmlSFBool(value ? true : false);
  env->SetIntField(obj, fid, (int) pSFBool);
}

JNIEXPORT jboolean JNICALL Java_vrml_field_ConstSFBool_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFBool* pSFBool = (VrmlSFBool*) env->GetIntField(obj, fid);
  return (jboolean) pSFBool->get();
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFBool_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFBool
//

JNIEXPORT void JNICALL Java_vrml_field_SFBool_CreateObject
  (JNIEnv *env, jobject obj, jboolean value)
{
  Java_vrml_field_ConstSFBool_CreateObject(env, obj, value);
}

JNIEXPORT jboolean JNICALL Java_vrml_field_SFBool_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFBool_getValue(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_SFBool_setValue__Z
  (JNIEnv *env, jobject obj, jboolean value)
{
  VrmlSFBool* pSFBool = (VrmlSFBool*)getVrmlField(env, obj);
  pSFBool->set(value ? true : false);
}

JNIEXPORT void JNICALL Java_vrml_field_SFBool_setValue__Lvrml_field_ConstSFBool_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFBool* pNewSFBool = (VrmlSFBool*) env->GetIntField(value, fid);
  VrmlSFBool* pSFBool = (VrmlSFBool*)getVrmlField(env, obj);
  pSFBool->set(pNewSFBool->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFBool_setValue__Lvrml_field_SFBool_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFBool_setValue__Lvrml_field_ConstSFBool_2(env, obj, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFBool_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFColor
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFColor_CreateObject
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFColor* pSFColor = new VrmlSFColor(r, g, b);
  env->SetIntField(obj, fid, (int) pSFColor);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstSFColor_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFColor* pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 3, (jfloat*) pSFColor->get()) ;
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFColor_getRed
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFColor* pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  return (jfloat) pSFColor->getR();
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFColor_getGreen
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFColor* pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  return (jfloat) pSFColor->getG();
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFColor_getBlue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFColor* pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  return (jfloat) pSFColor->getB();
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFColor
//

JNIEXPORT void JNICALL Java_vrml_field_SFColor_CreateObject
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  Java_vrml_field_ConstSFColor_CreateObject(env, obj, r, g, b);
}

JNIEXPORT void JNICALL Java_vrml_field_SFColor_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFColor_getValue(env, obj, jarr);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_SFColor_getRed
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFColor_getRed(env, obj);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_SFColor_getGreen
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFColor_getGreen(env, obj);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_SFColor_getBlue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFColor_getBlue(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_SFColor_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  VrmlSFColor* pSFColor = (VrmlSFColor*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFColor->set((float*) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_SFColor_setValue__FFF
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  const float rgb[3] = { r, g, b };
  VrmlSFColor* pSFColor = (VrmlSFColor*)getVrmlField(env, obj);
  pSFColor->set(rgb);
}

JNIEXPORT void JNICALL Java_vrml_field_SFColor_setValue__Lvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFColor* pNewSFColor = (VrmlSFColor*) env->GetIntField(value, fid);
  VrmlSFColor* pSFColor = (VrmlSFColor*)getVrmlField(env, obj);
  pSFColor->set(pNewSFColor->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFColor_setValue__Lvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFColor_setValue__Lvrml_field_ConstSFColor_2(env, obj, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFFloat
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFFloat_CreateObject
  (JNIEnv *env, jobject obj, jfloat value)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFFloat* pSFFloat = new VrmlSFFloat((float) value);
  env->SetIntField(obj, fid, (int) pSFFloat);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFFloat_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFFloat* pSFFloat = (VrmlSFFloat*) env->GetIntField(obj, fid);
  return (jfloat) pSFFloat->get();
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFFloat
//

JNIEXPORT void JNICALL Java_vrml_field_SFFloat_CreateObject
  (JNIEnv *env, jobject obj, jfloat value)
{
  Java_vrml_field_ConstSFFloat_CreateObject(env, obj, value);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_SFFloat_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFFloat_getValue(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_SFFloat_setValue__F
  (JNIEnv *env, jobject obj, jfloat f)
{
  VrmlSFFloat* pSFFloat = (VrmlSFFloat*)getVrmlField(env, obj);
  pSFFloat->set((float) f);
}

JNIEXPORT void JNICALL Java_vrml_field_SFFloat_setValue__Lvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFFloat* pSFFloat = (VrmlSFFloat*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFFloat* pNewSFFloat = (VrmlSFFloat*) env->GetIntField(value, fid);
  pSFFloat->set(pNewSFFloat->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFFloat_setValue__Lvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFFloat_setValue__Lvrml_field_ConstSFFloat_2(env, obj, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFImage
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFImage_CreateObject
  (JNIEnv *env, jobject obj, jint width, jint height, jint components, jbyteArray pixels)
{
  VrmlSFImage* pConstSFImage;
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  jbyte *pjb = env->GetByteArrayElements(pixels, NULL); 
  pConstSFImage = new VrmlSFImage((int) width, (int) height, 
				  (int) components, (unsigned char *) pjb);
  env->ReleaseByteArrayElements(pixels, pjb, JNI_ABORT);
  env->SetIntField(obj, fid, (int) pConstSFImage);
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstSFImage_getWidth
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFImage* pConstSFImage = (VrmlSFImage*) env->GetIntField(obj, fid);
  return (jint) pConstSFImage->getWidth();
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstSFImage_getHeight
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFImage* pConstSFImage = (VrmlSFImage*) env->GetIntField(obj, fid);
  return (jint) pConstSFImage->getHeight();
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstSFImage_getComponents
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFImage* pConstSFImage = (VrmlSFImage*) env->GetIntField(obj, fid);
  return (jint) pConstSFImage->getComponents();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstSFImage_getPixels
  (JNIEnv *env, jobject obj, jbyteArray pixels)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFImage* pConstSFImage = (VrmlSFImage*) env->GetIntField(obj, fid);
  env->SetByteArrayRegion(pixels, 0, 
			  pConstSFImage->getWidth() * pConstSFImage->getHeight(),
			  (jbyte*) pConstSFImage->getPixels());
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFImage_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFImage
//

JNIEXPORT void JNICALL Java_vrml_field_SFImage_CreateObject
  (JNIEnv *env, jobject obj, jint width, jint height, jint components, jbyteArray pixels)
{
  Java_vrml_field_ConstSFImage_CreateObject(env, obj, width, height, components, pixels);
}

JNIEXPORT jint JNICALL Java_vrml_field_SFImage_getWidth
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFImage_getWidth(env, obj);
}

JNIEXPORT jint JNICALL Java_vrml_field_SFImage_getHeight
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFImage_getHeight(env, obj);
}

JNIEXPORT jint JNICALL Java_vrml_field_SFImage_getComponents
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFImage_getComponents(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_SFImage_getPixels
  (JNIEnv *env, jobject obj, jbyteArray pixels)
{
  Java_vrml_field_ConstSFImage_getPixels(env, obj, pixels);
}

JNIEXPORT void JNICALL Java_vrml_field_SFImage_setValue__III_3B
  (JNIEnv *env, jobject obj, jint width, jint height, jint components, jbyteArray pixels)
{
  VrmlSFImage* pSFImage = (VrmlSFImage*)getVrmlField(env, obj);
  jbyte *pjb = env->GetByteArrayElements(pixels, NULL); 
  pSFImage->set((int) width, (int) height, (int) components, 
		(unsigned char *) pjb);
  env->ReleaseByteArrayElements(pixels, pjb, JNI_ABORT);
}


JNIEXPORT void JNICALL Java_vrml_field_SFImage_setValue__Lvrml_field_ConstSFImage_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFImage* pSFImage = (VrmlSFImage*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFImage* pNewSFImage = (VrmlSFImage*) env->GetIntField(value, fid);
  *pSFImage = *pNewSFImage;
}

JNIEXPORT void JNICALL Java_vrml_field_SFImage_setValue__Lvrml_field_SFImage_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFImage_setValue__Lvrml_field_ConstSFImage_2(env, obj, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFImage_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFInt32
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint value)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFInt32* pSFInt32 = new VrmlSFInt32((int) value);
  env->SetIntField(obj, fid, (int) pSFInt32);
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstSFInt32_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFInt32* pSFInt32 = (VrmlSFInt32*) env->GetIntField(obj, fid);
  return (int) pSFInt32->get();
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFInt32
//

JNIEXPORT void JNICALL Java_vrml_field_SFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint value)
{
  Java_vrml_field_ConstSFInt32_CreateObject(env, obj, value);
}

JNIEXPORT jint JNICALL Java_vrml_field_SFInt32_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFInt32_getValue(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_SFInt32_setValue__I
  (JNIEnv *env, jobject obj, jint value)
{
  VrmlSFInt32* pSFInt32 = (VrmlSFInt32*)getVrmlField(env, obj);  
  pSFInt32->set((int) value);
}

JNIEXPORT void JNICALL Java_vrml_field_SFInt32_setValue__Lvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFInt32* pSFInt32 = (VrmlSFInt32*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFInt32* pNewSFInt32 = (VrmlSFInt32*) env->GetIntField(value, fid);
  pSFInt32->set(pNewSFInt32->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFInt32_setValue__Lvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFInt32_setValue__Lvrml_field_ConstSFInt32_2(env, obj, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFNode
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFNode_CreateObject
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid = getFid(env, value, "NodePtr", "I");
  VrmlNode* pBaseNode = (VrmlNode*) env->GetIntField(value, fid);
  VrmlSFNode* pSFNode = new VrmlSFNode(VrmlNodePtr(pBaseNode));
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFNode);
}

JNIEXPORT jobject JNICALL Java_vrml_field_ConstSFNode_getValue
  (JNIEnv *env, jobject obj)
{
  jobject jNode;

  cout << "TODO: Implement Java_vrml_field_ConstSFNode_getValue" << endl;

  return jNode;
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFNode
//

JNIEXPORT void JNICALL Java_vrml_field_SFNode_CreateObject
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_ConstSFNode_CreateObject(env, obj, value);
}

JNIEXPORT jobject JNICALL Java_vrml_field_SFNode_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFNode_getValue(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  cout << "TODO: Implement Java_vrml_field_SFNode_setValue__Lvrml_BaseNode_2" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFNode* pSFNode = (VrmlSFNode*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFNode* pNewSFNode = (VrmlSFNode*) env->GetIntField(value, fid);
  *pSFNode = *pNewSFNode;
}

JNIEXPORT void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFNode_setValue__Lvrml_field_ConstSFNode_2(env, obj, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFRotation
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFRotation_CreateObject
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY, jfloat axisZ, jfloat angle)
{
  VrmlSFRotation* pSFRotation = new VrmlSFRotation(axisX, axisY, axisZ, angle);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFRotation);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstSFRotation_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFRotation* pSFRotation = (VrmlSFRotation*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 4, (jfloat*) pSFRotation->get());
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFRotation
//

JNIEXPORT void JNICALL Java_vrml_field_SFRotation_CreateObject
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY, jfloat axisZ, jfloat angle)
{
  Java_vrml_field_ConstSFRotation_CreateObject(env, obj, axisX, axisY, axisZ, angle);
}

JNIEXPORT void JNICALL Java_vrml_field_SFRotation_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFRotation_getValue(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_SFRotation_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  VrmlSFRotation* pSFRotation = (VrmlSFRotation*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFRotation->set(pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_SFRotation_setValue__FFFF
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY, jfloat axisZ, jfloat angle)
{
  VrmlSFRotation* pSFRotation = (VrmlSFRotation*)getVrmlField(env, obj);
  const float rotation[] = { axisX, axisY, axisZ, angle };
  pSFRotation->set(rotation);
}

JNIEXPORT void JNICALL Java_vrml_field_SFRotation_setValue__Lvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFRotation* pSFRotation = (VrmlSFRotation*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFRotation* pNewSFRotation = 
    (VrmlSFRotation*) env->GetIntField(value, fid);
  pSFRotation->set(pNewSFRotation->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFRotation_setValue__Lvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFRotation_setValue__Lvrml_field_ConstSFRotation_2(env, obj, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}


//
// ConstSFString
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFString_CreateObject
  (JNIEnv *env, jobject obj, jstring jstr)
{
  const char *str = env->GetStringUTFChars(jstr, 0);
  VrmlSFString* pSFString = new VrmlSFString(str);
  env->ReleaseStringUTFChars(jstr, str);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFString);
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFString_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFString* pSFString = (VrmlSFString*) env->GetIntField(obj, fid);
  return env->NewStringUTF((pSFString->get()).c_str());
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFString
//

JNIEXPORT void JNICALL Java_vrml_field_SFString_CreateObject
  (JNIEnv *env, jobject obj, jstring jstr)
{
  Java_vrml_field_ConstSFString_CreateObject(env, obj, jstr);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFString_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFString_getValue(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_SFString_setValue__Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jstring jstr)
{
  VrmlSFString* pSFString = (VrmlSFString*)getVrmlField(env, obj);
  const char *str = env->GetStringUTFChars(jstr, 0);
  pSFString->set(str);
  env->ReleaseStringUTFChars(jstr, str);
}

JNIEXPORT void JNICALL Java_vrml_field_SFString_setValue__Lvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFString* pSFString = (VrmlSFString*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFString* pNewSFString = (VrmlSFString*) env->GetIntField(value, fid);
  pSFString->set(pNewSFString->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFString_setValue__Lvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFString_setValue__Lvrml_field_ConstSFString_2(env, obj, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFTime
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFTime_CreateObject
  (JNIEnv *env, jobject obj, jdouble time)
{
  VrmlSFTime* pSFTime = new VrmlSFTime(time);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFTime);
}


JNIEXPORT jdouble JNICALL Java_vrml_field_ConstSFTime_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFTime* pSFTime = (VrmlSFTime*) env->GetIntField(obj, fid);
  return (jdouble) pSFTime->get();
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFTime
//

JNIEXPORT void JNICALL Java_vrml_field_SFTime_CreateObject
  (JNIEnv *env, jobject obj, jdouble time)
{
  Java_vrml_field_ConstSFTime_CreateObject(env, obj, time);
}


JNIEXPORT jdouble JNICALL Java_vrml_field_SFTime_getValue
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFTime_getValue(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_SFTime_setValue__D
  (JNIEnv *env, jobject obj, jdouble time)
{
  VrmlSFTime* pSFTime = (VrmlSFTime*)getVrmlField(env, obj);
  pSFTime->set((double) time);
}


JNIEXPORT void JNICALL Java_vrml_field_SFTime_setValue__Lvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFTime* pSFTime = (VrmlSFTime*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFTime* pNewSFTime = (VrmlSFTime*) env->GetIntField(value, fid);
  pSFTime->set(pNewSFTime->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFTime_setValue__Lvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFTime_setValue__Lvrml_field_ConstSFTime_2(env, obj, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFVec2f
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFVec2f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  VrmlSFVec2f* pSFVec2f = new VrmlSFVec2f(x, y);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFVec2f);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstSFVec2f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFVec2f* pSFVec2f = (VrmlSFVec2f*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 2, (jfloat*) pSFVec2f->get());
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFVec2f_getX
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFVec2f* pSFVec2f = (VrmlSFVec2f*) env->GetIntField(obj, fid);
  return (jfloat) pSFVec2f->getX();
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFVec2f_getY
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFVec2f* pSFVec2f = (VrmlSFVec2f*) env->GetIntField(obj, fid);
  return (jfloat) pSFVec2f->getY();
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFVec2f
//

JNIEXPORT void JNICALL Java_vrml_field_SFVec2f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  Java_vrml_field_ConstSFVec2f_CreateObject(env, obj, x, y);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec2f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFVec2f_getValue(env, obj, jarr);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_SFVec2f_getX
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec2f_getX(env, obj);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_SFVec2f_getY
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec2f_getY(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec2f_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  VrmlSFVec2f* pSFVec2f = (VrmlSFVec2f*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFVec2f->set(pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec2f_setValue__FF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  VrmlSFVec2f* pSFVec2f = (VrmlSFVec2f*)getVrmlField(env, obj);
  const float vec2f[] = { x, y };
  pSFVec2f->set(vec2f);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec2f_setValue__Lvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFVec2f* pSFVec2f = (VrmlSFVec2f*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFVec2f* pNewSFVec2f = (VrmlSFVec2f*) env->GetIntField(value, fid);
  pSFVec2f->set(pNewSFVec2f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec2f_setValue__Lvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFVec2f_setValue__Lvrml_field_ConstSFVec2f_2(env, obj, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstSFVec3f
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFVec3f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  VrmlSFVec3f* pSFVec3f = new VrmlSFVec3f(x, y, z);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFVec3f);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstSFVec3f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFVec3f* pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 3, (jfloat*) pSFVec3f->get());
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFVec3f_getX
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFVec3f* pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  return (jfloat) pSFVec3f->getX();
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFVec3f_getY
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFVec3f* pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  return (jfloat) pSFVec3f->getY();
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFVec3f_getZ
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlSFVec3f* pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  return (jfloat) pSFVec3f->getZ();
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// SFVec3f
//

JNIEXPORT void JNICALL Java_vrml_field_SFVec3f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  Java_vrml_field_ConstSFVec3f_CreateObject(env, obj, x, y, z);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec3f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstSFVec3f_getValue(env, obj, jarr);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_SFVec3f_getX
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3f_getX(env, obj);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_SFVec3f_getY
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3f_getY(env, obj);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_SFVec3f_getZ
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstSFVec3f_getZ(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec3f_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  VrmlSFVec3f* pSFVec3f = (VrmlSFVec3f*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFVec3f->set(pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec3f_setValue__FFF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  VrmlSFVec3f* pSFVec3f = (VrmlSFVec3f*)getVrmlField(env, obj);
  const float vec3f[] = { x, y, z };
  pSFVec3f->set(vec3f);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec3f_setValue__Lvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFVec3f* pSFVec3f = (VrmlSFVec3f*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFVec3f* pNewSFVec3f = (VrmlSFVec3f*) env->GetIntField(value, fid);
  pSFVec3f->set(pNewSFVec3f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec3f_setValue__Lvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_SFVec3f_setValue__Lvrml_field_ConstSFVec3f_2(env, obj, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_SFVec3f_toString
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

JNIEXPORT void JNICALL Java_vrml_field_ConstMFColor_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
  VrmlMFColor* pMFColor = new VrmlMFColor(size / 3, (const float *) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFColor);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFColor_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
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
  
  VrmlMFColor* pMFColor = new VrmlMFColor(nRows, theArray);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFColor);
  delete[] theArray;
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFColor_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFColor* pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  return (jint) pMFColor->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFColor_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFColor* pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  
  int nRows = pMFColor->getLength();
  jfloat* p = (jfloat*) pMFColor->get();
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(jarr, row);
    
    env->SetFloatArrayRegion(el, 0, 3, p);
    p += 3;
  }
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFColor_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFColor* pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements, (jfloat*) pMFColor->get());
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFColor_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFColor* pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 3, 
			   (jfloat*) pMFColor->getElement(element));
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFColor_get1Value__ILvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jint element, jobject sfcolor)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFColor* pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  const float* value = pMFColor->getElement(element);
  fid = getFid(env, sfcolor, "FieldPtr", "I");
  VrmlSFColor* pSFColor = (VrmlSFColor*) env->GetIntField(sfcolor, fid);
  pSFColor->set(value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstMFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFColor
//

JNIEXPORT void JNICALL Java_vrml_field_MFColor_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  Java_vrml_field_ConstMFColor_CreateObject__I_3F(env, obj, size, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFColor_CreateObject___3_3F(env, obj, jarr);
}

JNIEXPORT jint JNICALL Java_vrml_field_MFColor_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFColor_getSize(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFColor* pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  pMFColor->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_delete
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFColor* pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  pMFColor->removeElement(index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFColor_getValue___3_3F(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstMFColor_getValue___3F(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{   
  Java_vrml_field_ConstMFColor_get1Value__I_3F(env, obj, element, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_get1Value__ILvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jint element, jobject sfcolor)
{
  Java_vrml_field_ConstMFColor_get1Value__ILvrml_field_SFColor_2(env, obj, element, sfcolor);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_setValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray value)
{
  jsize nRows = env->GetArrayLength(value);
  float* theArray = new float[nRows * 3];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(value, row);
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 3; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }

  VrmlMFColor* pMFColor = (VrmlMFColor*)getVrmlField(env, obj);
  pMFColor->set(nRows, theArray);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray value)
{
  VrmlMFColor* pMFColor = (VrmlMFColor*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  jsize nels = env->GetArrayLength(value);
  pMFColor->set(nels / 3, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  VrmlMFColor* pMFColor = (VrmlMFColor*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFColor->set(size / 3, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_setValue__Lvrml_field_MFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFColor* pMFColor = (VrmlMFColor*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlMFColor* pNewMFColor = (VrmlMFColor*) env->GetIntField(value, fid);
  pMFColor->set(pNewMFColor->getLength(), pNewMFColor->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_setValue__Lvrml_field_ConstMFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFColor_setValue__Lvrml_field_MFColor_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_set1Value__ILvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jint index, jobject sfcolor)
{
  VrmlMFColor* pMFColor = (VrmlMFColor*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfcolor, "FieldPtr", "I");
  VrmlSFColor* pSFColor = (VrmlSFColor*) env->GetIntField(sfcolor, fid);
  memcpy((void*) pMFColor->getElement(index), pSFColor->get(), 
	 3 * sizeof(float));
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_set1Value__ILvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jint index, jobject sfcolor)
{
  Java_vrml_field_MFColor_set1Value__ILvrml_field_ConstSFColor_2(env, obj, index, sfcolor);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_set1Value__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat red, jfloat green, jfloat blue)
{
  VrmlMFColor* pMFColor = (VrmlMFColor*)getVrmlField(env, obj);
  float *p = (float*) pMFColor->getElement(index);
  p[0] = red;
  p[1] = green;
  p[2] = blue;
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_addValue__Lvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jobject sfcolor)
{
  VrmlMFColor* pMFColor = (VrmlMFColor*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfcolor, "FieldPtr", "I");
  VrmlSFColor* pSFColor = (VrmlSFColor*) env->GetIntField(sfcolor, fid);
  pMFColor->insertElement(pMFColor->getLength(), pSFColor->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_addValue__Lvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jobject sfcolor)
{
  Java_vrml_field_MFColor_addValue__Lvrml_field_ConstSFColor_2(env, obj, sfcolor);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_addValue__FFF
  (JNIEnv *env, jobject obj, jfloat red, jfloat green, jfloat blue)
{
  VrmlMFColor* pMFColor = (VrmlMFColor*)getVrmlField(env, obj);
  float newData[3];
  newData[0] = red;
  newData[1] = green;
  newData[2] = blue;
  pMFColor->insertElement(pMFColor->getLength(), newData);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_insertValue__ILvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jint index, jobject sfcolor)
{
  VrmlMFColor* pMFColor = (VrmlMFColor*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfcolor, "FieldPtr", "I");
  VrmlSFColor* pSFColor = (VrmlSFColor*) env->GetIntField(sfcolor, fid);
  pMFColor->insertElement(index, pSFColor->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_insertValue__ILvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jint index, jobject sfcolor)
{
  Java_vrml_field_MFColor_insertValue__ILvrml_field_ConstSFColor_2(env, obj, index, sfcolor);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_insertValue__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat red, jfloat green, jfloat blue)
{
  VrmlMFColor* pMFColor = (VrmlMFColor*)getVrmlField(env, obj);
  float newData[3];
  newData[0] = red;
  newData[1] = green;
  newData[2] = blue;
  pMFColor->insertElement(index, newData);
}

JNIEXPORT jstring JNICALL Java_vrml_field_MFColor_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFFloat
//

JNIEXPORT void JNICALL Java_vrml_field_ConstMFFloat_CreateObject
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  jfloat *pjf = env->GetFloatArrayElements(value, NULL);
  VrmlMFFloat* pMFFloat = new VrmlMFFloat(size, (const float *) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFFloat);
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFFloat_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  return (jint) pMFFloat->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFFloat_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, pMFFloat->getLength(), 
			   (jfloat*) pMFFloat->get());
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstMFFloat_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  return (jfloat) pMFFloat->getElement(index);
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstMFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFFloat
//
JNIEXPORT void JNICALL Java_vrml_field_MFFloat_CreateObject
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  Java_vrml_field_ConstMFFloat_CreateObject(env, obj, size, value);
}

JNIEXPORT jint JNICALL Java_vrml_field_MFFloat_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFFloat_getSize(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  pMFFloat->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_delete
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  pMFFloat->removeElement(index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstMFFloat_getValue(env, obj, jarr);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_MFFloat_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFFloat_get1Value(env, obj, index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray value)
{
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  jsize nels = env->GetArrayLength(value);
  pMFFloat->set(nels, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFFloat->set(size, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_setValue__Lvrml_field_MFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlMFFloat* pNewMFFloat = (VrmlMFFloat*) env->GetIntField(value, fid);
  pMFFloat->set(pNewMFFloat->getLength(), pNewMFFloat->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_setValue__Lvrml_field_ConstMFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFFloat_setValue__Lvrml_field_MFFloat_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_set1Value__IF
  (JNIEnv *env, jobject obj, jint index, jfloat value)
{
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  pMFFloat->setElement(index, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_set1Value__ILvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject sffloat)
{
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sffloat, "FieldPtr", "I");
  VrmlSFFloat* pSFFloat = (VrmlSFFloat*) env->GetIntField(sffloat, fid);
  pMFFloat->setElement(index, pSFFloat->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_set1Value__ILvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject sffloat)
{
  Java_vrml_field_MFFloat_set1Value__ILvrml_field_ConstSFFloat_2(env, obj, index, sffloat);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_addValue__F
  (JNIEnv *env, jobject obj, jfloat value)
{
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*)getVrmlField(env, obj);
  pMFFloat->insertElement(pMFFloat->getLength(), (float) value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_addValue__Lvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFFloat* pSFFloat = (VrmlSFFloat*) env->GetIntField(value, fid);
  pMFFloat->insertElement(pMFFloat->getLength(), pSFFloat->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_addValue__Lvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFFloat_addValue__Lvrml_field_ConstSFFloat_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_insertValue__IF
  (JNIEnv *env, jobject obj, jint index, jfloat value)
{
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*)getVrmlField(env, obj);
  pMFFloat->insertElement(index, (float) value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_insertValue__ILvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  VrmlMFFloat* pMFFloat = (VrmlMFFloat*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFFloat* pSFFloat = (VrmlSFFloat*) env->GetIntField(value, fid);
  pMFFloat->insertElement(index, pSFFloat->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_insertValue__ILvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  Java_vrml_field_MFFloat_insertValue__ILvrml_field_ConstSFFloat_2(env, obj, index, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_MFFloat_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFInt32
//

JNIEXPORT void JNICALL Java_vrml_field_ConstMFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint size, jlongArray value)
{
  jlong *pjl = env->GetLongArrayElements(value, NULL);
  VrmlMFInt32* pMFInt32 = new VrmlMFInt32(size, (const long *) pjl);
  env->ReleaseLongArrayElements(value, pjl, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFInt32);
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFInt32_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  return (jint) pMFInt32->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFInt32_getValue
  (JNIEnv *env, jobject obj, jlongArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  env->SetLongArrayRegion(jarr, 0, pMFInt32->getLength(),
			  (jlong*) pMFInt32->get()) ;
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFInt32_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  return (jint) pMFInt32->getElement(index);
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstMFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFInt32
//

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint size, jlongArray value)
{
  Java_vrml_field_ConstMFInt32_CreateObject(env, obj, size, value);
}

JNIEXPORT jint JNICALL Java_vrml_field_MFInt32_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFInt32_getSize(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  pMFInt32->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_delete
  (JNIEnv *env, jobject obj, jint n)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  pMFInt32->removeElement(n);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_getValue
  (JNIEnv *env, jobject obj, jlongArray jarr)
{
  Java_vrml_field_MFInt32_getValue(env, obj, jarr);
}

JNIEXPORT jint JNICALL Java_vrml_field_MFInt32_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFInt32_get1Value(env, obj, index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_setValue___3I
  (JNIEnv *env, jobject obj, jlongArray value)
{
  jlong *pjl = env->GetLongArrayElements(value, NULL); 
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*)getVrmlField(env, obj);
  int size = (int) env->GetArrayLength(value); 
  pMFInt32->set(size, (const long*) pjl);
  env->ReleaseLongArrayElements(value, pjl, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_setValue__I_3I
  (JNIEnv *env, jobject obj, jint size, jlongArray value)
{
  jlong *pjl = env->GetLongArrayElements(value, NULL); 
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*)getVrmlField(env, obj);
  pMFInt32->set(size, (const long*) pjl);
  env->ReleaseLongArrayElements(value, pjl, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_setValue__Lvrml_field_MFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlMFInt32* pNewMFInt32 = (VrmlMFInt32*) env->GetIntField(value, fid);
  pMFInt32->set(pNewMFInt32->getLength(), pNewMFInt32->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_setValue__Lvrml_field_ConstMFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFInt32_setValue__Lvrml_field_MFInt32_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_set1Value__II
  (JNIEnv *env, jobject obj, jint index, jint value)
{
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*)getVrmlField(env, obj);
  pMFInt32->setElement(index, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_set1Value__ILvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject sfint)
{
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfint, "FieldPtr", "I");
  VrmlSFInt32* pSFInt32 = (VrmlSFInt32*) env->GetIntField(sfint, fid);
  pMFInt32->setElement(index, pSFInt32->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_set1Value__ILvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject sfint)
{
  Java_vrml_field_MFInt32_set1Value__ILvrml_field_SFInt32_2(env, obj, index, sfint);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_addValue__I
  (JNIEnv *env, jobject obj, jint value)
{
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*)getVrmlField(env, obj);
  pMFInt32->insertElement(pMFInt32->getLength(), (int) value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_addValue__Lvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFInt32* pSFInt32 = (VrmlSFInt32*) env->GetIntField(value, fid);
  pMFInt32->insertElement(pMFInt32->getLength(), pSFInt32->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_addValue__Lvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFInt32_addValue__Lvrml_field_ConstSFInt32_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_insertValue__II
  (JNIEnv *env, jobject obj, jint index, jfloat value)
{
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*)getVrmlField(env, obj);
  pMFInt32->insertElement(index, (int)value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_insertValue__ILvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlSFInt32* pSFInt32 = (VrmlSFInt32*) env->GetIntField(value, fid);
  pMFInt32->insertElement(index, pSFInt32->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_insertValue__ILvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  Java_vrml_field_MFInt32_insertValue__ILvrml_field_ConstSFInt32_2(env, obj, index, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_MFInt32_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFNode
//

JNIEXPORT void JNICALL Java_vrml_field_ConstMFNode_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray basenode)
{
  cout << "TODO: Implement Java_vrml_field_ConstMFNode_CreateObject" << endl;
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFNode_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFNode* pMFNode = (VrmlMFNode*) env->GetIntField(obj, fid);
  return (jint) pMFNode->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFNode_getValue
  (JNIEnv *env, jobject obj, jobjectArray basenode)
{
  cout << "TODO: Implement ScriptJDK::Java_vrml_field_ConstMFNode_getValue" << endl;
}

JNIEXPORT jobject JNICALL Java_vrml_field_ConstMFNode_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  cout << "TODO: Implement ScriptJDK::Java_vrml_field_ConstMFNode_get1Value" << endl;
  return 0;
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstMFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFNode
//

JNIEXPORT void JNICALL Java_vrml_field_MFNode_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray basenode)
{
  Java_vrml_field_ConstMFNode_CreateObject(env, obj, size, basenode);
}

JNIEXPORT jint JNICALL Java_vrml_field_MFNode_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFNode_getSize(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFNode* pMFNode = (VrmlMFNode*) env->GetIntField(obj, fid);
  pMFNode->clear();
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_delete
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFNode* pMFNode = (VrmlMFNode*) env->GetIntField(obj, fid);
  pMFNode->removeElement(index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_getValue
  (JNIEnv *env, jobject obj, jobjectArray basenode)
{
  Java_vrml_field_ConstMFNode_getValue(env, obj, basenode);
}

JNIEXPORT jobject JNICALL Java_vrml_field_MFNode_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFNode_get1Value(env, obj, index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_setValue__I_3Lvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jint size, jobjectArray basenode)
{
  cout << "TODO: Implement Java_vrml_field_MFNode_setValue__I_3Lvrml_BaseNode_2" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_setValue__Lvrml_field_MFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFNode* pMFNode = (VrmlMFNode*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlMFNode* pNewMFNode = (VrmlMFNode*) env->GetIntField(value, fid);
  *pMFNode = *pNewMFNode;
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_setValue__Lvrml_field_ConstMFNode_2
  (JNIEnv *, jobject, jobject)
{
  cout << "TODO: Implement Java_vrml_field_MFNode_setValue" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_set1Value__ILvrml_BaseNode_2
  (JNIEnv *, jobject, jint, jobject)
{
  cout << "TODO: Implement Java_vrml_field_MFNode_set1Value" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_set1Value__ILvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject sfnode)
{
  cout << "TODO: Implement Java_vrml_field_MFNode_set1Value__ILvrml_field_ConstSFNode_2" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_set1Value__ILvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject sfnode)
{
  Java_vrml_field_MFNode_set1Value__ILvrml_field_ConstSFNode_2(env, obj, index, sfnode);
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_addValue__Lvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  cout << "TODO: Implement Java_vrml_field_MFNode_addValue__Lvrml_BaseNode_2" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_addValue__Lvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  cout << "TODO: Implement Java_vrml_field_MFNode_addValue__Lvrml_field_ConstSFNode_2" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_addValue__Lvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFNode_addValue__Lvrml_field_ConstSFNode_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_insertValue__ILvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  cout << "TODO: Implement Java_vrml_field_MFNode_insertValue__ILvrml_BaseNode_2" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_insertValue__ILvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  cout << "TODO: Implement Java_vrml_field_MFNode_insertValue__ILvrml_field_ConstSFNode_2" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_insertValue__ILvrml_field_SFNode_2  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  Java_vrml_field_MFNode_insertValue__ILvrml_field_ConstSFNode_2(env, obj, index, value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_MFNode_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFRotation
//

JNIEXPORT void JNICALL Java_vrml_field_ConstMFRotation_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jsize nRows = env->GetArrayLength(jarr);
  float* theArray = new float[nRows * 4];
  float* p = theArray;

  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(jarr, row);
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 4; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }
  
  VrmlMFRotation* pMFRotation = new VrmlMFRotation(nRows, theArray);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFRotation_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
  VrmlMFRotation* pMFRotation = new VrmlMFRotation(size / 4, (const float *) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFRotation_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  return (jint) pMFRotation->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFRotation_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  
  int nRows = pMFRotation->getLength();
  jfloat* p = (jfloat*) pMFRotation->get();
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(jarr, row);
    env->SetFloatArrayRegion(el, 0, 4, p);
    p += 4;
  }
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFRotation_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements, (jfloat*) pMFRotation->get());
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFRotation_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 4, 
			   (jfloat*) pMFRotation->getElement(element));
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFRotation_get1Value__ILvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jint element, jobject sfrotation)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  const float* value = pMFRotation->getElement(element);
  fid = getFid(env, sfrotation, "FieldPtr", "I");
  VrmlSFRotation* pSFRotation = (VrmlSFRotation*) env->GetIntField(sfrotation, fid);
  pSFRotation->set(value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstMFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}


//
// MFRotation
//

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFRotation_CreateObject___3_3F(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  Java_vrml_field_ConstMFRotation_CreateObject__I_3F(env, obj, size, jarr);
}

JNIEXPORT jint JNICALL Java_vrml_field_MFRotation_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFRotation_getSize(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  pMFRotation->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_delete
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  pMFRotation->removeElement(index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFRotation_getValue___3_3F(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstMFRotation_getValue___3F(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  Java_vrml_field_ConstMFRotation_get1Value__I_3F(env, obj, element, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_get1Value__ILvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jint element, jobject sfrotation)
{
  Java_vrml_field_ConstMFRotation_get1Value__ILvrml_field_SFRotation_2(env, obj, element, sfrotation);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_setValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray value)
{
  jsize nRows = env->GetArrayLength(value);
  float* theArray = new float[nRows * 4];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(value, row);
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 4; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }

  VrmlMFRotation* pMFRotation = (VrmlMFRotation*)getVrmlField(env, obj);
  pMFRotation->set(nRows, theArray);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray value)
{
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  jsize nels = env->GetArrayLength(value);
  pMFRotation->set(nels / 4, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFRotation->set(size / 4, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_setValue__Lvrml_field_MFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlMFRotation* pNewMFRotation = (VrmlMFRotation*) env->GetIntField(value, fid);
  pMFRotation->set(pNewMFRotation->getLength(), pNewMFRotation->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_setValue__Lvrml_field_ConstMFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFRotation_setValue__Lvrml_field_MFRotation_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_set1Value__ILvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotation)
{
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfrotation, "FieldPtr", "I");
  VrmlSFRotation* pSFRotation = (VrmlSFRotation*) env->GetIntField(sfrotation, fid);
  memcpy((void*) pMFRotation->getElement(index), pSFRotation->get(),
	 4 * sizeof(float));
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_set1Value__ILvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotation)
{
  Java_vrml_field_MFRotation_set1Value__ILvrml_field_ConstSFRotation_2(env, obj, index, sfrotation);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_set1Value__IFFFF
  (JNIEnv *env, jobject obj, jint index, jfloat axisX, jfloat axisY, jfloat axisZ, jfloat angle)
{
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*)getVrmlField(env, obj);
  float *p = (float*) pMFRotation->getElement(index);
  p[0] = axisX;
  p[1] = axisY;
  p[2] = axisZ;
  p[3] = angle;
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_addValue__Lvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jobject sfrotation)
{
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfrotation, "FieldPtr", "I");
  VrmlSFRotation* pSFRotation = (VrmlSFRotation*) env->GetIntField(sfrotation, fid);
  pMFRotation->insertElement(pMFRotation->getLength(), pSFRotation->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_addValue__Lvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jobject sfrotation)
{
  Java_vrml_field_MFRotation_addValue__Lvrml_field_ConstSFRotation_2(env, obj, sfrotation);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_addValue__FFFF
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY, jfloat axisZ, jfloat angle)
{
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*)getVrmlField(env, obj);
  float newData[4];
  newData[0] = axisX;
  newData[1] = axisY;
  newData[2] = axisZ;
  newData[3] = angle;
  pMFRotation->insertElement(pMFRotation->getLength(), newData);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_insertValue__ILvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotation)
{
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfrotation, "FieldPtr", "I");
  VrmlSFRotation* pSFRotation = (VrmlSFRotation*) env->GetIntField(sfrotation, fid);
  pMFRotation->insertElement(index, pSFRotation->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_insertValue__ILvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotation)
{
  Java_vrml_field_MFRotation_insertValue__ILvrml_field_ConstSFRotation_2(env, obj, index, sfrotation);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_insertValue__IFFFF
  (JNIEnv *env, jobject obj, jint index, jfloat axisX, jfloat axisY, jfloat axisZ, jfloat angle)
{
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*)getVrmlField(env, obj);
  float newData[4];
  newData[0] = axisX;
  newData[1] = axisY;
  newData[2] = axisZ;
  newData[3] = angle;
  pMFRotation->insertElement(index, newData);
}

JNIEXPORT jstring JNICALL Java_vrml_field_MFRotation_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFString
//

JNIEXPORT void JNICALL Java_vrml_field_ConstMFString_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
  VrmlMFString* pMFString;
  
  if (size > 0)
  {
    std::string* pStringArray = new std::string[size];
    int i;

    for (i = 0; i < size; i++)
    {
      jstring el = (jstring) env->GetObjectArrayElement(jarr, i);
      pStringArray[i] = std::string(env->GetStringUTFChars(el, 0));
    }

    pMFString = new VrmlMFString(size, pStringArray);

    for (i = 0; i < size; i++) 
    {
      jstring el = (jstring) env->GetObjectArrayElement(jarr, i);
      if (el)
	env->ReleaseStringUTFChars(el, pStringArray[i].c_str());
    }
    delete[] pStringArray;
  }
  else
  {
    pMFString = new VrmlMFString();
  }

  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFString);
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFString_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFString* pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  return (jint) pMFString->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFString_getValue
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFString* pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
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

JNIEXPORT jstring JNICALL Java_vrml_field_ConstMFString_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFString* pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  jstring jstr = env->NewStringUTF((pMFString->getElement(index)).c_str()); 
  return jstr;
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstMFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFString
//

JNIEXPORT void JNICALL Java_vrml_field_MFString_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
  Java_vrml_field_ConstMFString_CreateObject(env, obj, size, jarr);
}

JNIEXPORT jint JNICALL Java_vrml_field_MFString_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_MFString_getSize(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFString* pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  pMFString->setLength(0);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_delete
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFString* pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  pMFString->removeElement(index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_getValue
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFString_getValue(env, obj, jarr);
}

JNIEXPORT jstring JNICALL Java_vrml_field_MFString_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFString_get1Value(env, obj, index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_setValue__I_3Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
  cout << "TODO: Implement Java_vrml_field_MFString_setValue__I_3Ljava_lang_String_2" << endl;

  VrmlMFString* pMFString = (VrmlMFString*)getVrmlField(env, obj);
  pMFString->setLength(0);


}

JNIEXPORT void JNICALL Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  cout << "TODO: Implement Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_setValue__Lvrml_field_ConstMFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_set1Value__ILjava_lang_String_2
  (JNIEnv *env, jobject obj, jint index, jstring value)
{
  VrmlMFString* pMFString = (VrmlMFString*)getVrmlField(env, obj);
  pMFString->setElement(index, value ? env->GetStringUTFChars(value, 0) : NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_set1Value__ILvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  VrmlMFString* pMFString = (VrmlMFString*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfstring, "FieldPtr", "I");
  VrmlSFString* pSFString = (VrmlSFString*) env->GetIntField(sfstring, fid);
  pMFString->setElement(index, pSFString->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_set1Value__ILvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  Java_vrml_field_MFString_set1Value__ILvrml_field_ConstSFString_2(env, obj, index, sfstring);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_addValue__Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jstring value)
{
  VrmlMFString* pMFString = (VrmlMFString*)getVrmlField(env, obj);
  pMFString->insertElement(pMFString->getLength(), 
			   value ? env->GetStringUTFChars(value, 0) : NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_addValue__Lvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jobject sfstring)
{
  VrmlMFString* pMFString = (VrmlMFString*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfstring, "FieldPtr", "I");
  VrmlSFString* pSFString = (VrmlSFString*) env->GetIntField(sfstring, fid);
  pMFString->insertElement(pMFString->getLength(), (pSFString->get()).c_str());
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_addValue__Lvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jobject sfstring)
{
  Java_vrml_field_MFString_addValue__Lvrml_field_ConstSFString_2(env, obj, sfstring);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_insertValue__ILjava_lang_String_2
  (JNIEnv *env, jobject obj, jint index, jstring value)
{
  VrmlMFString* pMFString = (VrmlMFString*)getVrmlField(env, obj);
  pMFString->insertElement(index, 
			   value ? env->GetStringUTFChars(value, 0) : NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_insertValue__ILvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  VrmlMFString* pMFString = (VrmlMFString*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfstring, "FieldPtr", "I");
  VrmlSFString* pSFString = (VrmlSFString*) env->GetIntField(sfstring, fid);
  pMFString->insertElement(index, (pSFString->get()).c_str());
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_insertValue__ILvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  Java_vrml_field_MFString_insertValue__ILvrml_field_ConstSFString_2(env, obj, index, sfstring);
}

JNIEXPORT jstring JNICALL Java_vrml_field_MFString_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFTime
//

JNIEXPORT void JNICALL Java_vrml_field_ConstMFTime_CreateObject
  (JNIEnv *env, jobject obj, jint size, jdoubleArray value)
{
  jdouble *pjd = env->GetDoubleArrayElements(value, NULL);
  VrmlMFTime* pMFTime = new VrmlMFTime(size, (const double *) pjd);
  env->ReleaseDoubleArrayElements(value, pjd, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFTime);
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFTime_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFTime* pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  return (jint) pMFTime->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFTime_getValue
  (JNIEnv *env, jobject obj, jdoubleArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFTime* pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  env->SetDoubleArrayRegion(jarr, 0, pMFTime->getLength(), 
			    (jdouble*) pMFTime->get());
}

JNIEXPORT jdouble JNICALL Java_vrml_field_ConstMFTime_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFTime* pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  return (jdouble) pMFTime->getElement(index);
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstMFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFTime
//

JNIEXPORT void JNICALL Java_vrml_field_MFTime_CreateObject
  (JNIEnv *env, jobject obj, jint size, jdoubleArray value)
{
  Java_vrml_field_ConstMFTime_CreateObject(env, obj, size, value);
}

JNIEXPORT jint JNICALL Java_vrml_field_MFTime_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFTime_getSize(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFTime* pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  pMFTime->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_delete
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFTime* pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  pMFTime->removeElement(index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_getValue
  (JNIEnv *env, jobject obj, jdoubleArray jarr)
{
  Java_vrml_field_ConstMFTime_getValue(env, obj, jarr);
}

JNIEXPORT jdouble JNICALL Java_vrml_field_MFTime_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  return Java_vrml_field_ConstMFTime_get1Value(env, obj, index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_setValue__I_3D
  (JNIEnv *env, jobject obj, jint size, jdoubleArray value)
{
  VrmlMFTime* pMFTime = (VrmlMFTime*)getVrmlField(env, obj);
  jdouble *pjd = env->GetDoubleArrayElements(value, NULL); 
  pMFTime->set(size, (const double*) pjd);
  env->ReleaseDoubleArrayElements(value, pjd, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_setValue__Lvrml_field_MFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFTime* pMFTime = (VrmlMFTime*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlMFTime* pNewMFTime = (VrmlMFTime*) env->GetIntField(value, fid);
  pMFTime->set(pNewMFTime->getLength(), pNewMFTime->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_setValue__Lvrml_field_ConstMFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFTime_setValue__Lvrml_field_MFTime_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_set1Value__ID
  (JNIEnv *env, jobject obj, jint index, jdouble value)
{
  VrmlMFTime* pMFTime = (VrmlMFTime*)getVrmlField(env, obj);
  pMFTime->setElement(index, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_set1Value__ILvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  VrmlMFTime* pMFTime = (VrmlMFTime*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sftime, "FieldPtr", "I");
  VrmlSFTime* pSFTime = (VrmlSFTime*) env->GetIntField(sftime, fid);
  pMFTime->setElement(index, pSFTime->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_set1Value__ILvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  Java_vrml_field_MFTime_set1Value__ILvrml_field_ConstSFTime_2(env, obj, index, sftime);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_addValue__D
  (JNIEnv *env, jobject obj, jdouble value)
{
  VrmlMFTime* pMFTime = (VrmlMFTime*)getVrmlField(env, obj);
  pMFTime->insertElement(pMFTime->getLength(), (double) value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_addValue__Lvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jobject sftime)
{
  VrmlMFTime* pMFTime = (VrmlMFTime*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sftime, "FieldPtr", "I");
  VrmlSFTime* pSFTime = (VrmlSFTime*) env->GetIntField(sftime, fid);
  pMFTime->insertElement(pMFTime->getLength(), pSFTime->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_addValue__Lvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jobject sftime)
{
  Java_vrml_field_MFTime_addValue__Lvrml_field_ConstSFTime_2(env, obj, sftime);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_insertValue__ID
  (JNIEnv *env, jobject obj, jint index, jdouble value)
{
  VrmlMFTime* pMFTime = (VrmlMFTime*)getVrmlField(env, obj);
  pMFTime->insertElement(index, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_insertValue__ILvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  VrmlMFTime* pMFTime = (VrmlMFTime*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sftime, "FieldPtr", "I");
  VrmlSFTime* pSFTime = (VrmlSFTime*) env->GetIntField(sftime, fid);
  pMFTime->setElement(index, pSFTime->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_insertValue__ILvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  Java_vrml_field_MFTime_insertValue__ILvrml_field_ConstSFTime_2(env, obj, index, sftime);
}

JNIEXPORT jstring JNICALL Java_vrml_field_MFTime_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFVec2f
//

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec2f_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jsize nRows = env->GetArrayLength(jarr);
  float* theArray = new float[nRows * 2];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(jarr, row);
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 2; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }
  
  VrmlMFRotation* pMFRotation = new VrmlMFRotation(nRows, theArray);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec2f_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
  VrmlMFVec2f* pMFVec2f = new VrmlMFVec2f(size / 2, (const float *) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFVec2f);
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFVec2f_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  return (jint) pMFVec2f->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec2f_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  
  int nRows = pMFVec2f->getLength();
  jfloat* p = (jfloat*) pMFVec2f->get();
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(jarr, row);
    env->SetFloatArrayRegion(el, 0, 2, p);
    p += 2;
  }
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec2f_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements, (jfloat*) pMFVec2f->get());
}


JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec2f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 2, 
			   (jfloat*) pMFVec2f->getElement(element));
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec2f_get1Value__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec2f)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  const float* value = pMFVec2f->getElement(element);
  fid = getFid(env, sfvec2f, "FieldPtr", "I");
  VrmlSFVec2f* pSFVec2f = (VrmlSFVec2f*) env->GetIntField(sfvec2f, fid);
  pSFVec2f->set(value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstMFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFVec2f
//

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec2f_CreateObject___3_3F(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec2f_CreateObject__I_3F(env, obj, size, jarr);
}

JNIEXPORT jint JNICALL Java_vrml_field_MFVec2f_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFVec2f_getSize(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  pMFVec2f->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_delete
  (JNIEnv *env, jobject obj, jint n)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  pMFVec2f->removeElement(n);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec2f_getValue___3_3F(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec2f_getValue___3F(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec2f_get1Value__I_3F(env, obj, element, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_get1Value__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec2f)
{
  Java_vrml_field_ConstMFVec2f_get1Value__ILvrml_field_SFVec2f_2(env, obj, element, sfvec2f);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_setValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray value)
{
  jsize nRows = env->GetArrayLength(value);
  float* theArray = new float[nRows * 2];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(value, row);
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 2; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }
  
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*)getVrmlField(env, obj);
  pMFVec2f->set(nRows, theArray);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFVec2f->set(size / 2, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_setValue__Lvrml_field_MFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlMFVec2f* pNewMFVec2f = (VrmlMFVec2f*) env->GetIntField(value, fid);
  pMFVec2f->set(pNewMFVec2f->getLength(), pNewMFVec2f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_setValue__Lvrml_field_ConstMFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFVec2f_setValue__Lvrml_field_MFVec2f_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_set1Value__IFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y)
{
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*)getVrmlField(env, obj);
  float *p = (float*) pMFVec2f->getElement(index);
  p[0] = x;
  p[1] = y;
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_set1Value__ILvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfvec2f, "FieldPtr", "I");
  VrmlSFVec2f* pSFVec2f = (VrmlSFVec2f*) env->GetIntField(sfvec2f, fid);
  memcpy((void*) pMFVec2f->getElement(index), pSFVec2f->get(), 
	 2 * sizeof(float));
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_set1Value__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  Java_vrml_field_MFVec2f_set1Value__ILvrml_field_ConstSFVec2f_2(env, obj, index, sfvec2f);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_addValue__FF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*)getVrmlField(env, obj);
  float farr[2];
  farr[0] = x;
  farr[1] = y;
  pMFVec2f->insertElement(pMFVec2f->getLength(), farr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_addValue__Lvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jobject sfvec2f)
{
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfvec2f, "FieldPtr", "I");
  VrmlSFVec2f* pSFVec2f = (VrmlSFVec2f*) env->GetIntField(sfvec2f, fid);
  pMFVec2f->insertElement(pMFVec2f->getLength(), pSFVec2f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_addValue__Lvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jobject sfvec2f)
{
  Java_vrml_field_MFVec2f_addValue__Lvrml_field_ConstSFVec2f_2(env, obj, sfvec2f);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_insertValue__IFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y)
{
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*)getVrmlField(env, obj);
  float farr[2];
  farr[0] = x;
  farr[1] = y;
  pMFVec2f->insertElement(index, farr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_insertValue__ILvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfvec2f, "FieldPtr", "I");
  VrmlSFVec2f* pSFVec2f = (VrmlSFVec2f*) env->GetIntField(sfvec2f, fid);
  pMFVec2f->insertElement(index, pSFVec2f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_insertValue__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  Java_vrml_field_MFVec2f_insertValue__ILvrml_field_ConstSFVec2f_2(env, obj, index, sfvec2f);
}

JNIEXPORT jstring JNICALL Java_vrml_field_MFVec2f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// ConstMFVec3f
//

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec3f_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
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
  
  VrmlMFRotation* pMFRotation = new VrmlMFRotation(nRows, theArray);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec3f_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
  VrmlMFVec3f* pMFVec3f = new VrmlMFVec3f(size / 3, (const float *) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFVec3f);
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFVec3f_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  return (jint) pMFVec3f->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec3f_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  
  int nRows = pMFVec3f->getLength();
  jfloat* p = (jfloat*) pMFVec3f->get();
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(jarr, row);
    env->SetFloatArrayRegion(el, 0, 3, p);
    p += 3;
  }
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec3f_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements, (jfloat*) pMFVec3f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec3f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 3, 
			   (jfloat*) pMFVec3f->getElement(element));
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec3f_get1Value__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec3f)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  const float* value = pMFVec3f->getElement(element);
  fid = getFid(env, sfvec3f, "FieldPtr", "I");
  VrmlSFVec3f* pSFVec3f = (VrmlSFVec3f*) env->GetIntField(sfvec3f, fid);
  pSFVec3f->set(value);
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstMFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// MFVec3f
//

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_CreateObject___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec3f_CreateObject___3_3F(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec3f_CreateObject__I_3F(env, obj, size, jarr);
}

JNIEXPORT jint JNICALL Java_vrml_field_MFVec3f_getSize
  (JNIEnv *env, jobject obj)
{
  return Java_vrml_field_ConstMFVec3f_getSize(env, obj);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  pMFVec3f->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_delete
  (JNIEnv *env, jobject obj, jint n)
{
  jfieldID fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  pMFVec3f->removeElement(n);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  Java_vrml_field_ConstMFVec3f_getValue___3_3F(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_getValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec3f_getValue___3F(env, obj, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  Java_vrml_field_ConstMFVec3f_get1Value__I_3F(env, obj, element, jarr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_get1Value__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec3f)
{
  Java_vrml_field_ConstMFVec3f_get1Value__ILvrml_field_SFVec3f_2(env, obj, element, sfvec3f);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_setValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray value)
{
  jsize nRows = env->GetArrayLength(value);
  float* theArray = new float[nRows * 3];
  float* p = theArray;
  
  for (int row = 0; row < nRows; row++)
  {
    jfloatArray el = (jfloatArray) env->GetObjectArrayElement(value, row);
    jfloat *pjf = env->GetFloatArrayElements(el, NULL); 
    for (int col = 0; col < 3; col++)
    {
      *p++ = pjf[col];
    }
    env->ReleaseFloatArrayElements(el, pjf, JNI_ABORT);
  }
  
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*)getVrmlField(env, obj);
  pMFVec3f->set(nRows, theArray);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*)getVrmlField(env, obj);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFVec3f->set(size / 3, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_setValue__Lvrml_field_MFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, value, "FieldPtr", "I");
  VrmlMFVec3f* pNewMFVec3f = (VrmlMFVec3f*) env->GetIntField(value, fid);
  pMFVec3f->set(pNewMFVec3f->getLength(), pNewMFVec3f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_setValue__Lvrml_field_ConstMFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFVec3f_setValue__Lvrml_field_MFVec3f_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_set1Value__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y, jfloat z)
{
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*)getVrmlField(env, obj);
  float *p = (float*) pMFVec3f->getElement(index);
  p[0] = x;
  p[1] = y;
  p[2] = z;
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_set1Value__ILvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfvec3f, "FieldPtr", "I");
  VrmlSFVec3f* pSFVec3f = (VrmlSFVec3f*) env->GetIntField(sfvec3f, fid);
  memcpy((void*) pMFVec3f->getElement(index), pSFVec3f->get(),
	 3 * sizeof(float));
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_set1Value__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  Java_vrml_field_MFVec3f_set1Value__ILvrml_field_ConstSFVec3f_2(env, obj, index, sfvec3f);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_addValue__FFF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*)getVrmlField(env, obj);
  float farr[3];
  farr[0] = x;
  farr[1] = y;
  farr[2] = z;
  pMFVec3f->insertElement(pMFVec3f->getLength(), farr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_addValue__Lvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jobject sfvec3f)
{
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfvec3f, "FieldPtr", "I");
  VrmlSFVec3f* pSFVec3f = (VrmlSFVec3f*) env->GetIntField(sfvec3f, fid);
  pMFVec3f->insertElement(pMFVec3f->getLength(), pSFVec3f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_addValue__Lvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  Java_vrml_field_MFVec3f_addValue__Lvrml_field_ConstSFVec3f_2(env, obj, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_insertValue__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y, jfloat z)
{
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*)getVrmlField(env, obj);
  float farr[3];
  farr[0] = x;
  farr[1] = y;
  farr[2] = z;
  pMFVec3f->insertElement(index, farr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_insertValue__ILvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*)getVrmlField(env, obj);
  jfieldID fid = getFid(env, sfvec3f, "FieldPtr", "I");
  VrmlSFVec3f* pSFVec3f = (VrmlSFVec3f*) env->GetIntField(sfvec3f, fid);
  pMFVec3f->insertElement(index, pSFVec3f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_insertValue__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  Java_vrml_field_MFVec3f_insertValue__ILvrml_field_ConstSFVec3f_2(env, obj, index, sfvec3f);
}

JNIEXPORT jstring JNICALL Java_vrml_field_MFVec3f_toString
  (JNIEnv *env, jobject obj)
{
  return fieldToString(env, obj);
}

//
// Script
//

JNIEXPORT jobject JNICALL Java_vrml_node_Script_getField
  (JNIEnv *env, jobject obj, jstring jstrFieldName)
{
  jfieldID fid;
  VrmlNodeScript* pScript;
  jobject Field;
  jclass clazz;
  const VrmlField* FieldPtr;
  char clazzName[256];
  
  const char *fieldName = env->GetStringUTFChars(jstrFieldName, 0);
  fid = getFid(env, obj, "NodePtr", "I");
  pScript = (VrmlNodeScript*) env->GetIntField(obj, fid);
  FieldPtr = pScript->getField(fieldName);
  sprintf(clazzName, "vrml/field/%s", FieldPtr->fieldTypeName());
  clazz = env->FindClass(clazzName);
  env->ReleaseStringUTFChars(jstrFieldName, fieldName );
  Field = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");
  env->SetIntField(Field, fid, (int) FieldPtr);
  return Field;
}

JNIEXPORT jobject JNICALL Java_vrml_node_Script_getEventOut
  (JNIEnv *env, jobject obj, jstring jstrEventOutName)
{
  jfieldID fid;
  VrmlNodeScript* pScript;
  jobject eventOut;
  jclass clazz;
  VrmlField::VrmlFieldType eventOutType;
  const VrmlNodeScript::ScriptField* fieldPtr;
  char clazzName[256];

  const char *eventOutName = env->GetStringUTFChars(jstrEventOutName , 0);
  fid = getFid(env, obj, "NodePtr", "I");
  pScript = (VrmlNodeScript*) env->GetIntField(obj, fid);
  eventOutType = pScript->hasEventOut(eventOutName);
  sprintf(clazzName, "vrml/field/%s", VrmlField::getFieldName(eventOutType));
  
  clazz = env->FindClass(clazzName);
  eventOut = env->AllocObject(clazz);
  fid = getFid(env, eventOut, "FieldPtr", "I");

  // What you really want to do here is get a pointer to a ScriptField in the
  // FieldList array
  VrmlNodeScript::FieldList& eventOuts = pScript->eventOuts();

  for (VrmlNodeScript::FieldList::const_iterator i(eventOuts.begin()); 
        i != eventOuts.end(); ++i) {
    if ((*i)->name == eventOutName) {
      fieldPtr = (*i);
    }
  }

  env->SetIntField(eventOut, fid, (int) fieldPtr);

  fid = getFid(env, eventOut,"isEventOut", "Z");
  env->SetBooleanField(eventOut, fid, true);

  env->ReleaseStringUTFChars(jstrEventOutName, eventOutName);
  
  return eventOut;
}

JNIEXPORT jobject JNICALL Java_vrml_node_Script_getEventIn
  (JNIEnv *, jobject, jstring)
{
  cout << "TODO: Implement Java_vrml_node_Script_getEventIn" << endl;

  return 0;
}

JNIEXPORT jstring JNICALL Java_vrml_node_Script_toString
  (JNIEnv *env, jobject obj)
{
  cout << "TODO: Implement Java_vrml_node_Script_toString" << endl;

  return 0;
}

//
// BaseNode
//

JNIEXPORT jstring JNICALL Java_vrml_BaseNode_getType
  (JNIEnv *env, jobject obj)
{
  cout << "TODO: Implement Java_vrml_BaseNode_getType" << endl;

  return env->NewStringUTF("blah");
}

JNIEXPORT jobject JNICALL Java_vrml_BaseNode_getBrowser
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "NodePtr", "I");
  VrmlNode* pNode = (VrmlNode*) env->GetIntField(obj, fid);
  jclass clazz = env->FindClass("vrml/Browser");
  jobject jBrowser = env->AllocObject(clazz);
  fid = getFid(env, jBrowser, "BrowserPtr", "I");
  env->SetIntField(jBrowser, fid, (int) pNode->scene());	
  return jBrowser;
}

//
// Node
//

JNIEXPORT jobject JNICALL Java_vrml_node_Node_getEventIn
  (JNIEnv *env, jobject obj, jstring jstrEventInName)
{
  jobject eventIn;
  jfieldID fid;
  VrmlNode* pNode;
  char clazzName[256];
  jclass clazz;

  const char *eventInName = env->GetStringUTFChars(jstrEventInName , 0);
  fid = getFid(env, obj, "NodePtr", "I");
  pNode = (VrmlNode*) env->GetIntField(obj, fid);

  cout << "TODO: Implement Java_vrml_node_Node_getEventIn" << endl;

  return eventIn;
}


JNIEXPORT jobject JNICALL Java_vrml_node_Node_getEventOut
  (JNIEnv *env, jobject obj, jstring jstrEventOutName)
{
  jobject eventOut;
  jfieldID fid;
  VrmlNode* pNode;
  char clazzName[256];
  jclass clazz;

  const char *eventOutName = env->GetStringUTFChars(jstrEventOutName , 0);
  fid = getFid(env, obj, "NodePtr", "I");
  pNode = (VrmlNode*) env->GetIntField(obj, fid);
  const VrmlField* field = pNode->getEventOut(eventOutName);

  if (field == 0)
  {
    // throw exception as given eventOut name doesn't exist
    return 0;
  }

  sprintf(clazzName, "vrml/field/%s", field->fieldTypeName());
  clazz = env->FindClass(clazzName);
  eventOut = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");;
  env->SetIntField(eventOut, fid, (int) field);
  env->ReleaseStringUTFChars(jstrEventOutName, eventOutName);

  return eventOut;
}

JNIEXPORT jobject JNICALL Java_vrml_node_Node_getExposedField
  (JNIEnv *env, jobject obj, jstring jstrExposedFieldName)
{
  jobject exposedField;
  jfieldID fid;
  VrmlNode* pNode;
  char clazzName[256];
  jclass clazz;

  const char *exposedFieldName = 
    env->GetStringUTFChars(jstrExposedFieldName , 0);
  fid = getFid(env, obj, "NodePtr", "I");
  pNode = (VrmlNode*) env->GetIntField(obj, fid);
  const VrmlField* field = pNode->getField(exposedFieldName);

  if (field == 0)
  {
    // throw exception as given exposedField name doesn't exist
    return 0;
  }

  sprintf(clazzName, "vrml/field/%s", field->fieldTypeName());
  clazz = env->FindClass(clazzName);
  exposedField = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");;
  env->SetIntField(exposedField, fid, (int) field);
  env->ReleaseStringUTFChars(jstrExposedFieldName, exposedFieldName);

  return exposedField;
}

JNIEXPORT jstring JNICALL Java_vrml_node_Node_toString
  (JNIEnv *, jobject)
{
  cout << "TODO: Implement Java_vrml_node_Node_toString" << endl;

  return 0;
}

//
// Event
//

JNIEXPORT jstring JNICALL Java_vrml_Event_getName
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  VrmlEvent* pEvent = (VrmlEvent*) env->GetIntField(obj, fid);
  return env->NewStringUTF(pEvent->name());
}

JNIEXPORT jdouble JNICALL Java_vrml_Event_getTimeStamp
  (JNIEnv *env, jobject obj)
{
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  VrmlEvent* pEvent = (VrmlEvent*) env->GetIntField(obj, fid);
  return (jdouble) pEvent->timeStamp(); 
}

JNIEXPORT jobject JNICALL Java_vrml_Event_getValue
  (JNIEnv *env, jobject obj)
{  
  jfieldID fid = getFid(env, obj, "EventPtr", "I");
  VrmlEvent* pEvent = (VrmlEvent*) env->GetIntField(obj, fid);
  const VrmlField* pField = pEvent->value();
  char clazzName[256];
  sprintf(clazzName, "vrml/field/Const%s", pField->fieldTypeName());
  jclass clazz = env->FindClass(clazzName);
  jobject Field = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");
  env->SetIntField(Field, fid, (int) pField);
  return Field;
}

JNIEXPORT jobject JNICALL Java_vrml_Event_clone
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

JNIEXPORT jstring JNICALL Java_vrml_Event_toString
  (JNIEnv *env, jobject obj)
{
  cout << "TODO: Implement Java_vrml_Event_toString" << endl;

  return 0;
}

//
// Browser
//

JNIEXPORT jstring JNICALL Java_vrml_Browser_toString
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  
  // What should this return?

  return env->NewStringUTF("OpenVRML");
}

JNIEXPORT jstring JNICALL Java_vrml_Browser_getName
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  return env->NewStringUTF(pBrowser->getName());
}

JNIEXPORT jstring JNICALL Java_vrml_Browser_getVersion
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  return env->NewStringUTF(pBrowser->getVersion());
}

JNIEXPORT jfloat JNICALL Java_vrml_Browser_getCurrentSpeed
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  return (jfloat) 0.0;
}

JNIEXPORT jfloat JNICALL Java_vrml_Browser_getCurrentFrameRate
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  return (jfloat) pBrowser->getFrameRate();
}

JNIEXPORT jstring JNICALL Java_vrml_Browser_getWorldURL
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  return env->NewStringUTF(pBrowser->urlDoc()->url());
}

JNIEXPORT void JNICALL Java_vrml_Browser_replaceWorld
  (JNIEnv *env, jobject obj, jobjectArray basenodes)
{
  // First check if mustEvaluate is set, if not then method
  // should not continue.

  cout << "TODO: Implement Java_vrml_Browser_replaceWorld" << endl;
}

JNIEXPORT jobjectArray JNICALL Java_vrml_Browser_createVrmlFromString
  (JNIEnv *env, jobject obj, jstring vrmlSyntax)
{
  jfieldID fid;
  VrmlScene* pBrowser;
  
  fid = getFid(env, obj, "BrowserPtr", "I");
  pBrowser = (VrmlScene*) env->GetIntField(obj, fid);
  return 0;
}

JNIEXPORT void JNICALL Java_vrml_Browser_createVrmlFromURL
  (JNIEnv *env, jobject obj, jobjectArray urls, jobject basenode, jstring event)
{
  cout << "TODO: Implement Java_vrml_Browser_createVrmlFromURL" << endl;
}

JNIEXPORT void JNICALL Java_vrml_Browser_addRoute
  (JNIEnv *, jobject, jobject, jstring, jobject, jstring)
{
  // First check if directOutput is set, if not then method
  // should not continue

  cout << "TODO: Implement Java_vrml_Browser_addRoute" << endl;
}

JNIEXPORT void JNICALL Java_vrml_Browser_deleteRoute
  (JNIEnv *, jobject, jobject, jstring, jobject, jstring)
{
  // First check if directOutput is set, if not then method
  // should not continue

  cout << "TODO: Implement Java_vrml_Browser_deleteRoute" << endl;
}

JNIEXPORT void JNICALL Java_vrml_Browser_loadURL
  (JNIEnv *, jobject, jobjectArray, jobjectArray)
{
  // First check if mustEvaluate is set, if not then method
  // should not continue.

  cout << "TODO: Implement Java_vrml_Browser_loadURL" << endl;
}

JNIEXPORT void JNICALL Java_vrml_Browser_setDescription
  (JNIEnv *, jobject, jstring)
{
  cout << "TODO: Implement Java_vrml_Browser_setDescription" << endl;
}

} // extern "C"

#endif // OPENVRML_HAVE_JAVA
