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
    
    options[0].optionString = "-Djava.class.path=.";
    options[1].optionString = "-verbose:class";
    options[2].optionString = "-verbose:jni";
    vm_args.version = JNI_VERSION_1_2;
    vm_args.options = options;
    vm_args.nOptions = 3;
    
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

    /* TODO Compile fails here */
    // Call constructor

    //    jmethodID ctorId = d_env->GetMethodId(d_class, "<init>", 
    //				  "()V");
    //if (ctorId != 0)
    //  d_object = d_env->NewObject(d_class, ctorID);

    jfieldID fid = d_env->GetFieldID(d_class, "NodePtr", "I");
    d_env->SetIntField(d_object, fid, (int) scriptNode());

    // Cache other method IDs
    d_processEventID =
      d_env->GetStaticMethodID(d_class, "processEvent",
			       "(Lvrml/Event;)V");

    if ((exception = d_env->ExceptionOccurred()) != NULL)
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

// Run a specified script

void ScriptJDK::activate( double timeStamp,
			  const std::string & fname,
			  size_t argc,
			  const VrmlField * const argv[] )
{
  /* Remove VrmlEvent stuff for now as it is not used by the library */

  if (argc == 2 && d_processEventID != 0)
  {
    jclass clazz;
    jfieldID fid;
    jobject jEvent;
    
    clazz = d_env->FindClass("vrml/Event");
    jEvent = d_env->AllocObject(clazz);
    fid = getFid(d_env, jEvent, "EventPtr", "I");
    d_env->CallVoidMethod(d_object, d_processEventID, jEvent);
    d_env->DeleteLocalRef(jEvent); 
  }
  else if (argc == 0)
  {
    // Call initialize()
    jmethodID initID = d_env->GetMethodID(d_class, fname.c_str(),"()V");
    
    if (initID != 0)
      d_env->CallVoidMethod(d_object, initID);
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

JNIEXPORT jstring JNICALL vrmlfield_toString(JNIEnv *env, jobject obj)
{
  ostrstream os;
  char *szString;
  VrmlField* pField;
  jfieldID fid;
  jstring result;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pField = (VrmlField*) env->GetIntField(obj, fid);
  pField->print(os);
  os << ends;
  szString = os.str();
  result = env->NewStringUTF(szString); 
  os.rdbuf()->freeze(0);
  return result;
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
  jfieldID fid;
  VrmlSFBool* pSFBool;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFBool = new VrmlSFBool(value ? true : false);
  env->SetIntField(obj, fid, (int) pSFBool);
}

JNIEXPORT jboolean JNICALL Java_vrml_field_ConstSFBool_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlSFBool* pSFBool;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFBool = (VrmlSFBool*) env->GetIntField(obj, fid);
  return (jboolean) pSFBool->get();
}


//
// SFBool
//

JNIEXPORT void JNICALL Java_vrml_field_SFBool_setValue__Z
  (JNIEnv *env, jobject obj, jboolean value)
{
  jfieldID fid;
  VrmlSFBool* pSFBool;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFBool = (VrmlSFBool*) env->GetIntField(obj, fid);
  pSFBool->set(value ? true : false);
}

JNIEXPORT void JNICALL Java_vrml_field_SFBool_setValue__Lvrml_field_ConstSFBool_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFBool* pSFBool;
  VrmlSFBool* pNewSFBool;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFBool = (VrmlSFBool*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFBool = (VrmlSFBool*) env->GetIntField(value, fid);
  pSFBool->set(pNewSFBool->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFBool_setValue__Lvrml_field_SFBool_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFBool* pSFBool;
  VrmlSFBool* pNewSFBool;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFBool = (VrmlSFBool*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFBool = (VrmlSFBool*) env->GetIntField(value, fid);
  pSFBool->set(pNewSFBool->get());
}

//
// ConstSFColor
//


JNIEXPORT void JNICALL Java_vrml_field_ConstSFColor_CreateObject
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  jfieldID fid;
  VrmlSFColor* pSFColor;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFColor = new VrmlSFColor(r, g, b);
  env->SetIntField(obj, fid, (int) pSFColor);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstSFColor_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid;
  VrmlSFColor* pSFColor;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 3, (jfloat*) pSFColor->get()) ;
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFColor_getRed
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlSFColor* pSFColor;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  return (jfloat) pSFColor->getR();
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFColor_getGreen
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlSFColor* pSFColor;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  return (jfloat) pSFColor->getG();
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFColor_getBlue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlSFColor* pSFColor;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  return (jfloat) pSFColor->getB();
}

//
// SFColor
//

JNIEXPORT void JNICALL Java_vrml_field_SFColor_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid;
  VrmlSFColor* pSFColor;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFColor->set((float*) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_SFColor_setValue__FFF
  (JNIEnv *env, jobject obj, jfloat r, jfloat g, jfloat b)
{
  jfieldID fid;
  VrmlSFColor* pSFColor;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  const float rgb[3] = { r, g, b };
  pSFColor->set(rgb);
}

JNIEXPORT void JNICALL Java_vrml_field_SFColor_setValue__Lvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFColor* pSFColor;
  VrmlSFColor* pNewSFColor;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFColor = (VrmlSFColor*) env->GetIntField(value, fid);
  pSFColor->set(pNewSFColor->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFColor_setValue__Lvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFColor* pSFColor;
  VrmlSFColor* pNewSFColor;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFColor = (VrmlSFColor*) env->GetIntField(value, fid);
  pSFColor->set(pNewSFColor->get());
}

//
// ConstSFFloat
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFFloat_CreateObject
  (JNIEnv *env, jobject obj, jfloat value)
{
  jfieldID fid;
  VrmlSFFloat* pSFFloat;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFFloat = new VrmlSFFloat((float) value);
  env->SetIntField(obj, fid, (int) pSFFloat);
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFFloat_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlSFFloat* pSFFloat;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFFloat = (VrmlSFFloat*) env->GetIntField(obj, fid);
  return (jfloat) pSFFloat->get();
}

//
// SFFloat
//

JNIEXPORT void JNICALL Java_vrml_field_SFFloat_setValue__F
  (JNIEnv *env, jobject obj, jfloat f)
{
  jfieldID fid;
  VrmlSFFloat* pSFFloat;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFFloat = (VrmlSFFloat*) env->GetIntField(obj, fid);
  pSFFloat->set((float) f);
}

JNIEXPORT void JNICALL Java_vrml_field_SFFloat_setValue__Lvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFFloat* pSFFloat;
  VrmlSFFloat* pNewSFFloat;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFFloat = (VrmlSFFloat*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFFloat = (VrmlSFFloat*) env->GetIntField(value, fid);
  pSFFloat->set(pNewSFFloat->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFFloat_setValue__Lvrml_field_SFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFFloat* pSFFloat;
  VrmlSFFloat* pNewSFFloat;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFFloat = (VrmlSFFloat*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFFloat = (VrmlSFFloat*) env->GetIntField(value, fid);
  pSFFloat->set(pNewSFFloat->get());
}

//
// ConstSFImage
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFImage_CreateObject
  (JNIEnv *env, jobject obj, jint width, jint height, jint components, jbyteArray pixels)
{
  jfieldID fid;
  VrmlSFImage* pConstSFImage;
  fid = getFid(env, obj, "FieldPtr", "I");
  jbyte *pjb = env->GetByteArrayElements(pixels, NULL); 
  pConstSFImage = new VrmlSFImage((int) width, (int) height, 
				  (int) components, (unsigned char *) pjb);
  env->ReleaseByteArrayElements(pixels, pjb, JNI_ABORT);
  env->SetIntField(obj, fid, (int) pConstSFImage);
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstSFImage_getWidth
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlSFImage* pConstSFImage;
  fid = getFid(env, obj, "FieldPtr", "I");
  pConstSFImage = (VrmlSFImage*) env->GetIntField(obj, fid);
  return (jint) pConstSFImage->getWidth();
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstSFImage_getHeight
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlSFImage* pConstSFImage;
  fid = getFid(env, obj, "FieldPtr", "I");
  pConstSFImage = (VrmlSFImage*) env->GetIntField(obj, fid);
  return (jint) pConstSFImage->getHeight();
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstSFImage_getComponents
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlSFImage* pConstSFImage;
  fid = getFid(env, obj, "FieldPtr", "I");
  pConstSFImage = (VrmlSFImage*) env->GetIntField(obj, fid);
  return (jint) pConstSFImage->getComponents();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstSFImage_getPixels
  (JNIEnv *env, jobject obj, jbyteArray pixels)
{
  jfieldID fid;
  VrmlSFImage* pConstSFImage;
  fid = getFid(env, obj, "FieldPtr", "I");
  pConstSFImage = (VrmlSFImage*) env->GetIntField(obj, fid);
  env->SetByteArrayRegion(pixels, 0, 
			  pConstSFImage->getWidth() * pConstSFImage->getHeight(),
			  (jbyte*) pConstSFImage->getPixels());
}

//
// SFImage
//

JNIEXPORT void JNICALL Java_vrml_field_SFImage_setValue__III_3B
  (JNIEnv *env, jobject obj, jint width, jint height, jint components, jbyteArray pixels)
{
  jfieldID fid;
  VrmlSFImage* pConstSFImage;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pConstSFImage = (VrmlSFImage*) env->GetIntField(obj, fid);
  jbyte *pjb = env->GetByteArrayElements(pixels, NULL); 
  pConstSFImage->set((int) width, (int) height, (int) components, 
		     (unsigned char *) pjb);
  env->ReleaseByteArrayElements(pixels, pjb, JNI_ABORT);
}


JNIEXPORT void JNICALL Java_vrml_field_SFImage_setValue__Lvrml_field_ConstSFImage_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFImage* pConstSFImage;
  const VrmlSFImage* pNewConstSFImage;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pConstSFImage = (VrmlSFImage*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewConstSFImage = (VrmlSFImage*) env->GetIntField(value, fid);
  *pConstSFImage = *pNewConstSFImage;
}

JNIEXPORT void JNICALL Java_vrml_field_SFImage_setValue__Lvrml_field_SFImage_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFImage* pConstSFImage;
  const VrmlSFImage* pNewConstSFImage;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pConstSFImage = (VrmlSFImage*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewConstSFImage = (VrmlSFImage*) env->GetIntField(value, fid);
  *pConstSFImage = *pNewConstSFImage;
}

//
// ConstSFInt32
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint value)
{
  jfieldID fid;
  VrmlSFInt32* pSFInt32;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFInt32 = new VrmlSFInt32((int) value);
  env->SetIntField(obj, fid, (int) pSFInt32);
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstSFInt32_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlSFInt32* pSFInt32;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFInt32 = (VrmlSFInt32*) env->GetIntField(obj, fid);
  return (int) pSFInt32->get();
}


//
// SFInt32
//

JNIEXPORT void JNICALL Java_vrml_field_SFInt32_setValue__I
  (JNIEnv *env, jobject obj, jint value)
{
  jfieldID fid;
  VrmlSFInt32* pSFInt32;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFInt32 = (VrmlSFInt32*) env->GetIntField(obj, fid);
  pSFInt32->set((int) value);
}

JNIEXPORT void JNICALL Java_vrml_field_SFInt32_setValue__Lvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jobject newValue)
{
  jfieldID fid;
  VrmlSFInt32* pSFInt32;
  VrmlSFInt32* pNewSFInt32;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFInt32 = (VrmlSFInt32*) env->GetIntField(obj, fid);
  fid = getFid(env, newValue, "FieldPtr", "I");
  pNewSFInt32 = (VrmlSFInt32*) env->GetIntField(newValue, fid);
  pSFInt32->set(pNewSFInt32->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFInt32_setValue__Lvrml_field_SFInt32_2
  (JNIEnv *env, jobject obj, jobject newValue)
{
  jfieldID fid;
  VrmlSFInt32* pSFInt32;
  VrmlSFInt32* pNewSFInt32;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFInt32 = (VrmlSFInt32*) env->GetIntField(obj, fid);
  fid = getFid(env, newValue, "FieldPtr", "I");
  pNewSFInt32 = (VrmlSFInt32*) env->GetIntField(newValue, fid);
  pSFInt32->set(pNewSFInt32->get());
}

//
// ConstSFNode
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFNode_CreateObject
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFNode* pSFNode;
  VrmlNode* pBaseNode;
  fid = getFid(env, value, "NodePtr", "I");
  pBaseNode = (VrmlNode*) env->GetIntField(value, fid);
  pSFNode = new VrmlSFNode(VrmlNodePtr(pBaseNode));
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

//
// SFNode
//

JNIEXPORT void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  cout << "TODO: Implement Java_vrml_field_SFNode_setValue__Lvrml_BaseNode_2" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFNode* pSFNode;
  VrmlSFNode* pNewSFNode;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFNode = (VrmlSFNode*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFNode = (VrmlSFNode*) env->GetIntField(value, fid);
  *pSFNode = *pNewSFNode;
}

JNIEXPORT void JNICALL Java_vrml_field_SFNode_setValue__Lvrml_field_SFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFNode* pSFNode;
  VrmlSFNode* pNewSFNode;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFNode = (VrmlSFNode*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFNode = (VrmlSFNode*) env->GetIntField(value, fid);
  *pSFNode = *pNewSFNode;
}

//
// ConstSFRotation
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFRotation_CreateObject
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY, jfloat axisZ, jfloat angle)
{
  jfieldID fid;
  VrmlSFRotation* pSFRotation;
  pSFRotation = new VrmlSFRotation(axisX, axisY, axisZ, angle);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFRotation);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstSFRotation_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid;
  VrmlSFRotation* pSFRotation;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFRotation = (VrmlSFRotation*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 4, (jfloat*) pSFRotation->get());
}

//
// SFRotation
//

JNIEXPORT void JNICALL Java_vrml_field_SFRotation_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid;
  VrmlSFRotation* pSFRotation;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFRotation = (VrmlSFRotation*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFRotation->set(pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_SFRotation_setValue__FFFF
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY, jfloat axisZ, jfloat angle)
{
  jfieldID fid;
  VrmlSFRotation* pSFRotation;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFRotation = (VrmlSFRotation*) env->GetIntField(obj, fid);
  const float rotation[] = { axisX, axisY, axisZ, angle };
  pSFRotation->set(rotation);
}

JNIEXPORT void JNICALL Java_vrml_field_SFRotation_setValue__Lvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFRotation* pSFRotation;
  VrmlSFRotation* pNewSFRotation;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFRotation = (VrmlSFRotation*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFRotation = (VrmlSFRotation*) env->GetIntField(value, fid);
  pSFRotation->set(pNewSFRotation->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFRotation_setValue__Lvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFRotation* pSFRotation;
  VrmlSFRotation* pNewSFRotation;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFRotation = (VrmlSFRotation*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFRotation = (VrmlSFRotation*) env->GetIntField(value, fid);
  pSFRotation->set(pNewSFRotation->get());
}

//
// ConstSFString
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFString_CreateObject
  (JNIEnv *env, jobject obj, jstring jstr)
{
  jfieldID fid;
  VrmlSFString* pSFString;
  const char *str = env->GetStringUTFChars(jstr, 0);
  pSFString = new VrmlSFString(str);
  env->ReleaseStringUTFChars(jstr, str);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFString);
}

JNIEXPORT jstring JNICALL Java_vrml_field_ConstSFString_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlSFString* pSFString;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFString = (VrmlSFString*) env->GetIntField(obj, fid);
  return env->NewStringUTF((pSFString->get()).c_str());
}

//
// SFString
//

JNIEXPORT void JNICALL Java_vrml_field_SFString_setValue__Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jstring jstr)
{
  jfieldID fid;
  VrmlSFString* pSFString;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFString = (VrmlSFString*) env->GetIntField(obj, fid);
  
  const char *str = env->GetStringUTFChars(jstr, 0);
  pSFString->set(str);
  env->ReleaseStringUTFChars(jstr, str);
}

JNIEXPORT void JNICALL Java_vrml_field_SFString_setValue__Lvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFString* pSFString;
  VrmlSFString* pNewSFString;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFString = (VrmlSFString*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFString = (VrmlSFString*) env->GetIntField(value, fid);
  pSFString->set(pNewSFString->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFString_setValue__Lvrml_field_SFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlSFString* pSFString;
  VrmlSFString* pNewSFString;
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFString = (VrmlSFString*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFString = (VrmlSFString*) env->GetIntField(value, fid);
  pSFString->set(pNewSFString->get());
}

//
// ConstSFTime
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFTime_CreateObject
  (JNIEnv *env, jobject obj, jdouble time)
{
  jfieldID fid;
  VrmlSFTime* pSFTime;
  pSFTime = new VrmlSFTime(time);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFTime);
}


JNIEXPORT jdouble JNICALL Java_vrml_field_ConstSFTime_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlSFTime* pSFTime;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFTime = (VrmlSFTime*) env->GetIntField(obj, fid);
  return (jdouble) pSFTime->get();
}

//
// SFTime
//

JNIEXPORT void JNICALL Java_vrml_field_SFTime_setValue__D
  (JNIEnv *env, jobject obj, jdouble time)
{
  jfieldID fid;
  VrmlSFTime* pSFTime;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFTime = (VrmlSFTime*) env->GetIntField(obj, fid);
  pSFTime->set((double) time);
}


JNIEXPORT void JNICALL Java_vrml_field_SFTime_setValue__Lvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFTime* pSFTime;
  VrmlSFTime* pNewSFTime;
  jfieldID fid;

  fid = getFid(env, obj, "FieldPtr", "I");
  pSFTime = (VrmlSFTime*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFTime = (VrmlSFTime*) env->GetIntField(value, fid);
  pSFTime->set(pNewSFTime->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFTime_setValue__Lvrml_field_SFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFTime* pSFTime;
  VrmlSFTime* pNewSFTime;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFTime = (VrmlSFTime*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFTime = (VrmlSFTime*) env->GetIntField(value, fid);
  pSFTime->set(pNewSFTime->get());
}

//
// ConstSFVec2f
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFVec2f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  jfieldID fid;
  VrmlSFVec2f* pSFVec2f;
  pSFVec2f = new VrmlSFVec2f(x, y);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFVec2f);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstSFVec2f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid;
  VrmlSFVec2f* pSFVec2f;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec2f = (VrmlSFVec2f*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 2, (jfloat*) pSFVec2f->get());
}


JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFVec2f_getX
  (JNIEnv *env, jobject obj)
{
  VrmlSFVec2f* pSFVec2f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec2f = (VrmlSFVec2f*) env->GetIntField(obj, fid);
  return (jfloat) pSFVec2f->getX();
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFVec2f_getY
  (JNIEnv *env, jobject obj)
{
  VrmlSFVec2f* pSFVec2f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec2f = (VrmlSFVec2f*) env->GetIntField(obj, fid);
  return (jfloat) pSFVec2f->getY();
}

//
// SFVec2f
//

JNIEXPORT void JNICALL Java_vrml_field_SFVec2f_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid;
  VrmlSFVec2f* pSFVec2f;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec2f = (VrmlSFVec2f*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFVec2f->set(pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec2f_setValue__FF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  VrmlSFVec2f* pSFVec2f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec2f = (VrmlSFVec2f*) env->GetIntField(obj, fid);
  const float vec2f[] = { x, y };
  pSFVec2f->set(vec2f);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec2f_setValue__Lvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFVec2f* pSFVec2f;
  VrmlSFVec2f* pNewSFVec2f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec2f = (VrmlSFVec2f*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFVec2f = (VrmlSFVec2f*) env->GetIntField(value, fid);
  pSFVec2f->set(pNewSFVec2f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec2f_setValue__Lvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFVec2f* pSFVec2f;
  VrmlSFVec2f* pNewSFVec2f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec2f = (VrmlSFVec2f*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFVec2f = (VrmlSFVec2f*) env->GetIntField(value, fid);
  pSFVec2f->set(pNewSFVec2f->get());
}


//
// ConstSFVec3f
//

JNIEXPORT void JNICALL Java_vrml_field_ConstSFVec3f_CreateObject
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  jfieldID fid;
  VrmlSFVec3f* pSFVec3f;
  pSFVec3f = new VrmlSFVec3f(x, y, z);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pSFVec3f);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstSFVec3f_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid;
  VrmlSFVec3f* pSFVec3f;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 3, (jfloat*) pSFVec3f->get());
}


JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFVec3f_getX
  (JNIEnv *env, jobject obj)
{
  VrmlSFVec3f* pSFVec3f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  return (jfloat) pSFVec3f->getX();
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFVec3f_getY
  (JNIEnv *env, jobject obj)
{
  VrmlSFVec3f* pSFVec3f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  return (jfloat) pSFVec3f->getY();
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstSFVec3f_getZ
  (JNIEnv *env, jobject obj)
{
  VrmlSFVec3f* pSFVec3f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  return (jfloat) pSFVec3f->getZ();
}

//
// SFVec3f
//

JNIEXPORT void JNICALL Java_vrml_field_SFVec3f_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  jfieldID fid;
  VrmlSFVec3f* pSFVec3f;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL); 
  pSFVec3f->set(pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec3f_setValue__FFF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  VrmlSFVec3f* pSFVec3f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  const float vec3f[] = { x, y, z };
  pSFVec3f->set(vec3f);
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec3f_setValue__Lvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFVec3f* pSFVec3f;
  VrmlSFVec3f* pNewSFVec3f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFVec3f = (VrmlSFVec3f*) env->GetIntField(value, fid);
  pSFVec3f->set(pNewSFVec3f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_SFVec3f_setValue__Lvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlSFVec3f* pSFVec3f;
  VrmlSFVec3f* pNewSFVec3f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewSFVec3f = (VrmlSFVec3f*) env->GetIntField(value, fid);
  pSFVec3f->set(pNewSFVec3f->get());
}

//
// MFField's
//

//
// MFColor
//

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFColor_getSize
  (JNIEnv *env, jobject obj)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  return (jint) pMFColor->getLength();
}


JNIEXPORT void JNICALL Java_vrml_field_ConstMFColor_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  jfieldID fid;
  VrmlMFColor* pMFColor;
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
  pMFColor = new VrmlMFColor(size / 3, (const float *) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  fid = getFid(env, obj, "FieldPtr", "I");
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
  jfieldID fid;
  
  VrmlMFColor* pMFColor;
  pMFColor = new VrmlMFColor(nRows, theArray);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFColor);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFColor_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  
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
  VrmlMFColor* pMFColor;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements, (jfloat*) pMFColor->get());
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFColor_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 3, 
			   (jfloat*) pMFColor->getElement(element));
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFColor_get1Value__ILvrml_field_SFColor_2
  (JNIEnv *env, jobject obj, jint element, jobject sfcolor)
{
  VrmlMFColor* pMFColor;
  VrmlSFColor* pSFColor;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  const float* value = pMFColor->getElement(element);
  fid = getFid(env, sfcolor, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(sfcolor, fid);
  pSFColor->set(value);
}

//
// MFColor
//

JNIEXPORT void JNICALL Java_vrml_field_MFColor_clear
  (JNIEnv *env, jobject obj)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  pMFColor->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_delete
  (JNIEnv *env, jobject obj, jint index)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  pMFColor->removeElement(index);
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
  VrmlMFColor* pMFColor;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  pMFColor->set(nRows, theArray);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray value)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  jsize nels = env->GetArrayLength(value);
  pMFColor->set(nels / 3, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFColor->set(size / 3, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_setValue__Lvrml_field_MFColor_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  VrmlMFColor* pNewMFColor;
  fid = getFid(env, value, "FieldPtr", "I");
  pNewMFColor = (VrmlMFColor*) env->GetIntField(value, fid);
  pMFColor->set(pNewMFColor->getLength(), pNewMFColor->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_set1Value__ILvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jint index, jobject sfcolor)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  VrmlSFColor* pSFColor;
  fid = getFid(env, sfcolor, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(sfcolor, fid);
  memcpy((void*) pMFColor->getElement(index), pSFColor->get(), 
	 3 * sizeof(float));
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_set1Value__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat red, jfloat green, jfloat blue)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  float *p = (float*) pMFColor->getElement(index);
  p[0] = red;
  p[1] = green;
  p[2] = blue;
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_addValue__Lvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jobject sfcolor)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  VrmlSFColor* pSFColor;
  fid = getFid(env, sfcolor, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(sfcolor, fid);
  pMFColor->insertElement(pMFColor->getLength(), pSFColor->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_addValue__FFF
  (JNIEnv *env, jobject obj, jfloat red, jfloat green, jfloat blue)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  float newData[3];
  
  newData[0] = red;
  newData[1] = green;
  newData[2] = blue;
  pMFColor->insertElement(pMFColor->getLength(), newData);
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_insertValue__ILvrml_field_ConstSFColor_2
  (JNIEnv *env, jobject obj, jint index, jobject sfcolor)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  VrmlSFColor* pSFColor;
  fid = getFid(env, sfcolor, "FieldPtr", "I");
  pSFColor = (VrmlSFColor*) env->GetIntField(sfcolor, fid);
  pMFColor->insertElement(index, pSFColor->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFColor_insertValue__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat red, jfloat green, jfloat blue)
{
  VrmlMFColor* pMFColor;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFColor = (VrmlMFColor*) env->GetIntField(obj, fid);
  float newData[3];
  
  newData[0] = red;
  newData[1] = green;
  newData[2] = blue;
  pMFColor->insertElement(index, newData);
}

//
// ConstMFFloat
//

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFFloat_getSize
  (JNIEnv *env, jobject obj)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  return (jint) pMFFloat->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFFloat_CreateObject
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  jfieldID fid;
  VrmlMFFloat* pMFFloat;
  jfloat *pjf = env->GetFloatArrayElements(value, NULL);
  pMFFloat = new VrmlMFFloat(size, (const float *) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFFloat);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFFloat_getValue
  (JNIEnv *env, jobject obj, jfloatArray jarr)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, pMFFloat->getLength(), 
			   (jfloat*) pMFFloat->get());
}

JNIEXPORT jfloat JNICALL Java_vrml_field_ConstMFFloat_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  return (jfloat) pMFFloat->getElement(index);
}

//
// MFFloat
//

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_clear
  (JNIEnv *env, jobject obj)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  pMFFloat->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_delete
  (JNIEnv *env, jobject obj, jint index)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  pMFFloat->removeElement(index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray value)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  jsize nels = env->GetArrayLength(value);
  pMFFloat->set(nels, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFFloat->set(size, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_setValue__Lvrml_field_MFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  VrmlMFFloat* pNewMFFloat;
  fid = getFid(env, value, "FieldPtr", "I");
  pNewMFFloat = (VrmlMFFloat*) env->GetIntField(value, fid);
  pMFFloat->set(pNewMFFloat->getLength(), pNewMFFloat->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_set1Value__IF
  (JNIEnv *env, jobject obj, jint index, jfloat value)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  pMFFloat->setElement(index, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_set1Value__ILvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject sffloat)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  VrmlSFFloat* pSFFloat;
  fid = getFid(env, sffloat, "FieldPtr", "I");
  pSFFloat = (VrmlSFFloat*) env->GetIntField(sffloat, fid);
  pMFFloat->setElement(index, pSFFloat->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_addValue__F
  (JNIEnv *env, jobject obj, jfloat value)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  pMFFloat->insertElement(pMFFloat->getLength(), (float) value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_addValue__Lvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  
  VrmlSFFloat* pSFFloat;
  fid = getFid(env, value, "FieldPtr", "I");
  pSFFloat = (VrmlSFFloat*) env->GetIntField(value, fid);
  pMFFloat->insertElement(pMFFloat->getLength(), pSFFloat->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_insertValue__IF
  (JNIEnv *env, jobject obj, jint index, jfloat value)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  pMFFloat->insertElement(index, (float) value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFFloat_insertValue__ILvrml_field_ConstSFFloat_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  VrmlMFFloat* pMFFloat;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFFloat = (VrmlMFFloat*) env->GetIntField(obj, fid);
  VrmlSFFloat* pSFFloat;
  fid = getFid(env, value, "FieldPtr", "I");
  pSFFloat = (VrmlSFFloat*) env->GetIntField(value, fid);
  pMFFloat->insertElement(index, pSFFloat->get());
}


//
// ConstMFInt32
//

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFInt32_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  return (jint) pMFInt32->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFInt32_CreateObject
  (JNIEnv *env, jobject obj, jint size, jlongArray value)
{
  jfieldID fid;
  VrmlMFInt32* pMFInt32;
  jlong *pjl = env->GetLongArrayElements(value, NULL);
  pMFInt32 = new VrmlMFInt32(size, (const long *) pjl);
  env->ReleaseLongArrayElements(value, pjl, JNI_ABORT);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFInt32);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFInt32_getValue
  (JNIEnv *env, jobject obj, jlongArray jarr)
{
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  env->SetLongArrayRegion(jarr, 0, pMFInt32->getLength(),
			  (jlong*) pMFInt32->get()) ;
}

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFInt32_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  return (jint) pMFInt32->getElement(index);
}

//
// MFInt32
//

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  pMFInt32->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_delete
  (JNIEnv *env, jobject obj, jint n)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  pMFInt32->removeElement(n);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_setValue___3I
  (JNIEnv *env, jobject obj, jlongArray value)
{
  jlong *pjl = env->GetLongArrayElements(value, NULL); 
  
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  int size = (int) env->GetArrayLength(value); 
  pMFInt32->set(size, (const long*) pjl);
  env->ReleaseLongArrayElements(value, pjl, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_setValue__I_3I
  (JNIEnv *env, jobject obj, jint size, jlongArray value)
{
  jlong *pjl = env->GetLongArrayElements(value, NULL); 
  
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFInt32* pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  pMFInt32->set(size, (const long*) pjl);
  env->ReleaseLongArrayElements(value, pjl, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_setValue__Lvrml_field_MFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFInt32* pMFInt32;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  VrmlMFInt32* pNewMFInt32;
  fid = getFid(env, value, "FieldPtr", "I");
  pNewMFInt32 = (VrmlMFInt32*) env->GetIntField(value, fid);
  pMFInt32->set(pNewMFInt32->getLength(), pNewMFInt32->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_set1Value__II
  (JNIEnv *env, jobject obj, jint index, jint value)
{
  VrmlMFInt32* pMFInt32;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  pMFInt32->setElement(index, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_set1Value__ILvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject sfint)
{
  VrmlMFInt32* pMFInt32;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  VrmlSFInt32* pSFInt32;
  fid = getFid(env, sfint, "FieldPtr", "I");
  pSFInt32 = (VrmlSFInt32*) env->GetIntField(sfint, fid);
  pMFInt32->setElement(index, pSFInt32->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_addValue__I
  (JNIEnv *env, jobject obj, jint value)
{
  VrmlMFInt32* pMFInt32;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  pMFInt32->insertElement(pMFInt32->getLength(), (int) value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_addValue__Lvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFInt32* pMFInt32;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  
  VrmlSFInt32* pSFInt32;
  fid = getFid(env, value, "FieldPtr", "I");
  pSFInt32 = (VrmlSFInt32*) env->GetIntField(value, fid);
  pMFInt32->insertElement(pMFInt32->getLength(), pSFInt32->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_insertValue__II
  (JNIEnv *env, jobject obj, jint index, jfloat value)
{
  VrmlMFInt32* pMFInt32;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  pMFInt32->insertElement(index, (int)value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFInt32_insertValue__ILvrml_field_ConstSFInt32_2
  (JNIEnv *env, jobject obj, jint index, jobject value)
{
  VrmlMFInt32* pMFInt32;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFInt32 = (VrmlMFInt32*) env->GetIntField(obj, fid);
  VrmlSFInt32* pSFInt32;
  fid = getFid(env, value, "FieldPtr", "I");
  pSFInt32 = (VrmlSFInt32*) env->GetIntField(value, fid);
  pMFInt32->insertElement(index, pSFInt32->get());
}

//
// ConstMFNode
//

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFNode_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFNode* pMFNode = (VrmlMFNode*) env->GetIntField(obj, fid);
  return (jint) pMFNode->getLength();
}

static VrmlMFNode* CreateMFNode(JNIEnv *env, jobjectArray basenode, jint size)
{
  cout << "TODO: Implement ScriptJDK::CreateMFNode" << endl;
  return 0;
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFNode_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray basenode)
{
  VrmlMFNode* pMFNode = CreateMFNode(env, basenode, size);
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFNode);
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

//
// MFNode
//

JNIEXPORT void JNICALL Java_vrml_field_MFNode_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlMFNode* pMFNode;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFNode = (VrmlMFNode*) env->GetIntField(obj, fid);
  pMFNode->clear();
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_delete
  (JNIEnv *env, jobject obj, jint index)
{
  jfieldID fid;
  VrmlMFNode* pMFNode;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFNode = (VrmlMFNode*) env->GetIntField(obj, fid);
  pMFNode->removeElement(index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_setValue__I_3Lvrml_BaseNode_2
  (JNIEnv *env, jobject obj, jint size, jobjectArray basenode)
{
  VrmlMFNode* pNewMFNode = CreateMFNode(env, basenode, size);
  
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFNode* pMFNode;
  pMFNode = (VrmlMFNode*) env->GetIntField(obj, fid);
  (*pMFNode) = (*pNewMFNode);
  delete pNewMFNode;
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_setValue__Lvrml_field_MFNode_2
  (JNIEnv *env, jobject obj, jobject value)
{
  jfieldID fid;
  VrmlMFNode* pMFNode;
  VrmlMFNode* pNewMFNode;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFNode = (VrmlMFNode*) env->GetIntField(obj, fid);
  fid = getFid(env, value, "FieldPtr", "I");
  pNewMFNode = (VrmlMFNode*) env->GetIntField(value, fid);
  *pMFNode = *pNewMFNode;
}

JNIEXPORT void JNICALL Java_vrml_field_MFNode_set1Value__ILvrml_field_ConstSFNode_2
  (JNIEnv *env, jobject obj, jint index, jobject sfnode)
{
  cout << "TODO: Implement Java_vrml_field_MFNode_set1Value__ILvrml_field_ConstSFNode_2" << endl;
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

//
// ConstMFRotation
//

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFRotation_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFRotation* pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  return (jint) pMFRotation->getLength();
}

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
  jfieldID fid;
  
  VrmlMFRotation* pMFRotation;
  pMFRotation = new VrmlMFRotation(nRows, theArray);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
  delete[] theArray;
}


JNIEXPORT void JNICALL Java_vrml_field_ConstMFRotation_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  jfieldID fid;
  VrmlMFRotation* pMFRotation;
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
  pMFRotation = new VrmlMFRotation(size / 4, (const float *) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFRotation_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  
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
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements, (jfloat*) pMFRotation->get());
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFRotation_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;

  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 4, 
			   (jfloat*) pMFRotation->getElement(element));
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFRotation_get1Value__ILvrml_field_SFRotation_2
  (JNIEnv *env, jobject obj, jint element, jobject sfrotation)
{
  VrmlMFRotation* pMFRotation;
  VrmlSFRotation* pSFRotation;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  const float* value = pMFRotation->getElement(element);
  fid = getFid(env, sfrotation, "FieldPtr", "I");
  pSFRotation = (VrmlSFRotation*) env->GetIntField(sfrotation, fid);
  pSFRotation->set(value);
}

//
// MFRotation
//

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_clear
  (JNIEnv *env, jobject obj)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  pMFRotation->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_delete
  (JNIEnv *env, jobject obj, jint index)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  pMFRotation->removeElement(index);
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
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
	
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  pMFRotation->set(nRows, theArray);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_setValue___3F
  (JNIEnv *env, jobject obj, jfloatArray value)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  jsize nels = env->GetArrayLength(value);
  pMFRotation->set(nels / 4, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFRotation->set(size / 4, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_setValue__Lvrml_field_MFRotation_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  VrmlMFRotation* pNewMFRotation;
  fid = getFid(env, value, "FieldPtr", "I");
  pNewMFRotation = (VrmlMFRotation*) env->GetIntField(value, fid);
  pMFRotation->set(pNewMFRotation->getLength(), pNewMFRotation->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_set1Value__ILvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jint index, jobject sfrotation)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  VrmlSFRotation* pSFRotation;
  fid = getFid(env, sfrotation, "FieldPtr", "I");
  pSFRotation = (VrmlSFRotation*) env->GetIntField(sfrotation, fid);
  memcpy((void*) pMFRotation->getElement(index), pSFRotation->get(),
	 4 * sizeof(float));
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_set1Value__IFFFF
  (JNIEnv *env, jobject obj, jint index, jfloat axisX, jfloat axisY, jfloat axisZ, jfloat angle)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  float *p = (float*) pMFRotation->getElement(index);
  p[0] = axisX;
  p[1] = axisY;
  p[2] = axisZ;
  p[3] = angle;
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_addValue__Lvrml_field_ConstSFRotation_2
  (JNIEnv *env, jobject obj, jobject sfrotation)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  VrmlSFRotation* pSFRotation;
  fid = getFid(env, sfrotation, "FieldPtr", "I");
  pSFRotation = (VrmlSFRotation*) env->GetIntField(sfrotation, fid);
  pMFRotation->insertElement(pMFRotation->getLength(), pSFRotation->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_addValue__FFFF
  (JNIEnv *env, jobject obj, jfloat axisX, jfloat axisY, jfloat axisZ, jfloat angle)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
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
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  VrmlSFRotation* pSFRotation;
  fid = getFid(env, sfrotation, "FieldPtr", "I");
  pSFRotation = (VrmlSFRotation*) env->GetIntField(sfrotation, fid);
  pMFRotation->insertElement(index, pSFRotation->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFRotation_insertValue__IFFFF
  (JNIEnv *env, jobject obj, jint index, jfloat axisX, jfloat axisY, jfloat axisZ, jfloat angle)
{
  VrmlMFRotation* pMFRotation;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFRotation = (VrmlMFRotation*) env->GetIntField(obj, fid);
  float newData[4];
  
  newData[0] = axisX;
  newData[1] = axisY;
  newData[2] = axisZ;
  newData[3] = angle;
  pMFRotation->insertElement(index, newData);
}

//
// ConstMFString
//

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFString_getSize
  (JNIEnv *env, jobject obj)
{
  VrmlMFString* pMFString;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  return (jint) pMFString->getLength();
}

static VrmlMFString* getMFString(JNIEnv *env, jint size, jobjectArray jarr)
{
  VrmlMFString* pMFString;
  
  if (size > 0)
  {
    const char** pStringArray = new const char*[size];
    int i;

    for (i = 0; i < size; i++)
    {
      jstring el = (jstring) env->GetObjectArrayElement(jarr, i);
      pStringArray[i] = el ? env->GetStringUTFChars(el, 0) : NULL;
    }
    pMFString = new VrmlMFString(size, (std::string*)pStringArray);
    for (i = 0; i < size; i++) 
    {
      jstring el = (jstring) env->GetObjectArrayElement(jarr, i);
      if (el)
	env->ReleaseStringUTFChars(el, pStringArray[i]);
    }
    delete[] pStringArray;
  }
  else
    pMFString = new VrmlMFString();
  return pMFString;
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFString_CreateObject
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
  VrmlMFString* pMFString = getMFString(env, size, jarr);
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFString);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFString_getValue
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
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
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFString* pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  jstring jstr;
  jstr = env->NewStringUTF((pMFString->getElement(index)).c_str()); 
  return jstr;
}

//
// MFString
//

JNIEXPORT void JNICALL Java_vrml_field_MFString_clear
  (JNIEnv *env, jobject obj)
{
  cout << "TODO: Implement Java_vrml_field_MFString_clear" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_delete
  (JNIEnv *env, jobject obj, jint index)
{
  VrmlMFString* pMFString;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  pMFString->removeElement(index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_setValue__I_3Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jint size, jobjectArray jarr)
{
  cout << "TODO: Implement Java_vrml_field_MFString_setValue__I_3Ljava_lang_String_2" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2
  (JNIEnv *env, jobject obj, jobject value)
{
  cout << "TODO: Implement Java_vrml_field_MFString_setValue__Lvrml_field_MFString_2" << endl;
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_set1Value__ILjava_lang_String_2
  (JNIEnv *env, jobject obj, jint index, jstring value)
{
  VrmlMFString* pMFString;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  pMFString->setElement(index, value ? env->GetStringUTFChars(value, 0) : NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_set1Value__ILvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  VrmlMFString* pMFString;
  VrmlSFString* pSFString;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  fid = getFid(env, sfstring, "FieldPtr", "I");
  pSFString = (VrmlSFString*) env->GetIntField(sfstring, fid);
  pMFString->setElement(index, pSFString->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_addValue__Ljava_lang_String_2
  (JNIEnv *env, jobject obj, jstring value)
{
  VrmlMFString* pMFString;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  pMFString->insertElement(pMFString->getLength(), 
			   value ? env->GetStringUTFChars(value, 0) : NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_addValue__Lvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jobject sfstring)
{
  VrmlMFString* pMFString;
  VrmlSFString* pSFString;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  fid = getFid(env, sfstring, "FieldPtr", "I");
  pSFString = (VrmlSFString*) env->GetIntField(sfstring, fid);
  pMFString->insertElement(pMFString->getLength(), (pSFString->get()).c_str());
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_insertValue__ILjava_lang_String_2
  (JNIEnv *env, jobject obj, jint index, jstring value)
{
  VrmlMFString* pMFString;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  pMFString->insertElement(index, 
			   value ? env->GetStringUTFChars(value, 0) : NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFString_insertValue__ILvrml_field_ConstSFString_2
  (JNIEnv *env, jobject obj, jint index, jobject sfstring)
{
  VrmlMFString* pMFString;
  VrmlSFString* pSFString;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFString = (VrmlMFString*) env->GetIntField(obj, fid);
  fid = getFid(env, sfstring, "FieldPtr", "I");
  pSFString = (VrmlSFString*) env->GetIntField(sfstring, fid);
  pMFString->insertElement(index, (pSFString->get()).c_str());
}

//
// ConstMFTime
//

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFTime_getSize
  (JNIEnv *env, jobject obj)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  return (jint) pMFTime->getLength();
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFTime_CreateObject
  (JNIEnv *env, jobject obj, jint size, jdoubleArray value)
{
  jfieldID fid;
  VrmlMFTime* pMFTime;
  jdouble *pjd = env->GetDoubleArrayElements(value, NULL);
  pMFTime = new VrmlMFTime(size, (const double *) pjd);
  env->ReleaseDoubleArrayElements(value, pjd, JNI_ABORT);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFTime);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFTime_getValue
  (JNIEnv *env, jobject obj, jdoubleArray jarr)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  env->SetDoubleArrayRegion(jarr, 0, pMFTime->getLength(), 
			    (jdouble*) pMFTime->get());
}


JNIEXPORT jdouble JNICALL Java_vrml_field_ConstMFTime_get1Value
  (JNIEnv *env, jobject obj, jint index)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  return (jdouble) pMFTime->getElement(index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_clear
  (JNIEnv *env, jobject obj)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  pMFTime->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_delete
  (JNIEnv *env, jobject obj, jint index)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  pMFTime->removeElement(index);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_setValue__I_3D
  (JNIEnv *env, jobject obj, jint size, jdoubleArray value)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  jdouble *pjd = env->GetDoubleArrayElements(value, NULL); 
  pMFTime->set(size, (const double*) pjd);
  env->ReleaseDoubleArrayElements(value, pjd, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_setValue__Lvrml_field_MFTime_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  VrmlMFTime* pNewMFTime;
  fid = getFid(env, value, "FieldPtr", "I");
  pNewMFTime = (VrmlMFTime*) env->GetIntField(value, fid);
  pMFTime->set(pNewMFTime->getLength(), pNewMFTime->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_set1Value__ID
  (JNIEnv *env, jobject obj, jint index, jdouble value)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  pMFTime->setElement(index, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_set1Value__ILvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  VrmlSFTime* pSFTime;
  fid = getFid(env, sftime, "FieldPtr", "I");
  pSFTime = (VrmlSFTime*) env->GetIntField(sftime, fid);
  pMFTime->setElement(index, pSFTime->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_addValue__D
  (JNIEnv *env, jobject obj, jdouble value)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  pMFTime->insertElement(pMFTime->getLength(), (double) value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_addValue__Lvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jobject sftime)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  VrmlSFTime* pSFTime;
  fid = getFid(env, sftime, "FieldPtr", "I");
  pSFTime = (VrmlSFTime*) env->GetIntField(sftime, fid);
  pMFTime->insertElement(pMFTime->getLength(), pSFTime->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_insertValue__ID
  (JNIEnv *env, jobject obj, jint index, jdouble value)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  pMFTime->insertElement(index, value);
}

JNIEXPORT void JNICALL Java_vrml_field_MFTime_insertValue__ILvrml_field_ConstSFTime_2
  (JNIEnv *env, jobject obj, jint index, jobject sftime)
{
  VrmlMFTime* pMFTime;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFTime = (VrmlMFTime*) env->GetIntField(obj, fid);
  VrmlSFTime* pSFTime;
  fid = getFid(env, sftime, "FieldPtr", "I");
  pSFTime = (VrmlSFTime*) env->GetIntField(sftime, fid);
  pMFTime->setElement(index, pSFTime->get());
}

//
// ConstMFVec2f
//

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFVec2f_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  return (jint) pMFVec2f->getLength();
}

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
  jfieldID fid;
  
  VrmlMFRotation* pMFRotation;
  pMFRotation = new VrmlMFRotation(nRows, theArray);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec2f_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  jfieldID fid;
  VrmlMFVec2f* pMFVec2f;
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
  pMFVec2f = new VrmlMFVec2f(size / 2, (const float *) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFVec2f);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec2f_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  VrmlMFVec2f* pMFVec2f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  
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
  VrmlMFVec2f* pMFVec2f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements, (jfloat*) pMFVec2f->get());
}


JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec2f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  VrmlMFVec2f* pMFVec2f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 2, 
			   (jfloat*) pMFVec2f->getElement(element));
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec2f_get1Value__ILvrml_field_SFVec2f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec2f)
{
  VrmlMFVec2f* pMFVec2f;
  VrmlSFVec2f* pSFVec2f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  const float* value = pMFVec2f->getElement(element);
  fid = getFid(env, sfvec2f, "FieldPtr", "I");
  pSFVec2f = (VrmlSFVec2f*) env->GetIntField(sfvec2f, fid);
  pSFVec2f->set(value);
}

//
// MFVec2f
//

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  pMFVec2f->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_delete
  (JNIEnv *env, jobject obj, jint n)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  pMFVec2f->removeElement(n);
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
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec2f* pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  pMFVec2f->set(nRows, theArray);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  VrmlMFVec2f* pMFVec2f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFVec2f->set(size / 2, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_setValue__Lvrml_field_MFVec2f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFVec2f* pMFVec2f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  VrmlMFVec2f* pNewMFVec2f;
  fid = getFid(env, value, "FieldPtr", "I");
  pNewMFVec2f = (VrmlMFVec2f*) env->GetIntField(value, fid);
  pMFVec2f->set(pNewMFVec2f->getLength(), pNewMFVec2f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_set1Value__IFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y)
{
  VrmlMFVec2f* pMFVec2f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  float *p = (float*) pMFVec2f->getElement(index);
  p[0] = x;
  p[1] = y;
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_set1Value__ILvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  VrmlMFVec2f* pMFVec2f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  VrmlSFVec2f* pSFVec2f;
  fid = getFid(env, sfvec2f, "FieldPtr", "I");
  pSFVec2f = (VrmlSFVec2f*) env->GetIntField(sfvec2f, fid);
  memcpy((void*) pMFVec2f->getElement(index), pSFVec2f->get(), 
	 2 * sizeof(float));
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_addValue__FF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y)
{
  VrmlMFVec2f* pMFVec2f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  float farr[2];
  farr[0] = x;
  farr[1] = y;
  pMFVec2f->insertElement(pMFVec2f->getLength(), farr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_addValue__Lvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jobject sfvec2f)
{
  VrmlMFVec2f* pMFVec2f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  VrmlSFVec2f* pSFVec2f;
  fid = getFid(env, sfvec2f, "FieldPtr", "I");
  pSFVec2f = (VrmlSFVec2f*) env->GetIntField(sfvec2f, fid);
  pMFVec2f->insertElement(pMFVec2f->getLength(), pSFVec2f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_insertValue__IFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y)
{
  VrmlMFVec2f* pMFVec2f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  float farr[2];
  farr[0] = x;
  farr[1] = y;
  pMFVec2f->insertElement(index, farr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec2f_insertValue__ILvrml_field_ConstSFVec2f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec2f)
{
  VrmlMFVec2f* pMFVec2f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec2f = (VrmlMFVec2f*) env->GetIntField(obj, fid);
  VrmlSFVec2f* pSFVec2f;
  fid = getFid(env, sfvec2f, "FieldPtr", "I");
  pSFVec2f = (VrmlSFVec2f*) env->GetIntField(sfvec2f, fid);
  pMFVec2f->insertElement(index, pSFVec2f->get());
}

//
// ConstMFVec3f
//

JNIEXPORT jint JNICALL Java_vrml_field_ConstMFVec3f_getSize
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  return (jint) pMFVec3f->getLength();
}

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
  jfieldID fid;
  
  VrmlMFRotation* pMFRotation;
  pMFRotation = new VrmlMFRotation(nRows, theArray);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFRotation);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec3f_CreateObject__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray jarr)
{
  jfieldID fid;
  VrmlMFVec3f* pMFVec3f;
  jfloat *pjf = env->GetFloatArrayElements(jarr, NULL);
  pMFVec3f = new VrmlMFVec3f(size / 3, (const float *) pjf);
  env->ReleaseFloatArrayElements(jarr, pjf, JNI_ABORT);
  fid = getFid(env, obj, "FieldPtr", "I");
  env->SetIntField(obj, fid, (int) pMFVec3f);
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec3f_getValue___3_3F
  (JNIEnv *env, jobject obj, jobjectArray jarr)
{
  VrmlMFVec3f* pMFVec3f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  
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
  VrmlMFVec3f* pMFVec3f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  jsize nElements = env->GetArrayLength(jarr);
  env->SetFloatArrayRegion(jarr, 0, nElements, (jfloat*) pMFVec3f->get());
}


JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec3f_get1Value__I_3F
  (JNIEnv *env, jobject obj, jint element, jfloatArray jarr)
{
  VrmlMFVec3f* pMFVec3f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  env->SetFloatArrayRegion(jarr, 0, 3, 
			   (jfloat*) pMFVec3f->getElement(element));
}

JNIEXPORT void JNICALL Java_vrml_field_ConstMFVec3f_get1Value__ILvrml_field_SFVec3f_2
  (JNIEnv *env, jobject obj, jint element, jobject sfvec3f)
{
  VrmlMFVec3f* pMFVec3f;
  VrmlSFVec3f* pSFVec3f;
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  const float* value = pMFVec3f->getElement(element);
  fid = getFid(env, sfvec3f, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(sfvec3f, fid);
  pSFVec3f->set(value);
}

//
// MFVec3f
//

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_clear
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  pMFVec3f->set(0, NULL);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_delete
  (JNIEnv *env, jobject obj, jint n)
{
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  pMFVec3f->removeElement(n);
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
  jfieldID fid;
  
  fid = getFid(env, obj, "FieldPtr", "I");
  VrmlMFVec3f* pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  pMFVec3f->set(nRows, theArray);
  delete[] theArray;
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_setValue__I_3F
  (JNIEnv *env, jobject obj, jint size, jfloatArray value)
{
  VrmlMFVec3f* pMFVec3f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  jfloat *pjf = env->GetFloatArrayElements(value, NULL); 
  pMFVec3f->set(size / 3, (float*) pjf);
  env->ReleaseFloatArrayElements(value, pjf, JNI_ABORT);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_setValue__Lvrml_field_MFVec3f_2
  (JNIEnv *env, jobject obj, jobject value)
{
  VrmlMFVec3f* pMFVec3f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  VrmlMFVec3f* pNewMFVec3f;
  fid = getFid(env, value, "FieldPtr", "I");
  pNewMFVec3f = (VrmlMFVec3f*) env->GetIntField(value, fid);
  pMFVec3f->set(pNewMFVec3f->getLength(), pNewMFVec3f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_set1Value__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y, jfloat z)
{
  VrmlMFVec3f* pMFVec3f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  float *p = (float*) pMFVec3f->getElement(index);
  p[0] = x;
  p[1] = y;
  p[2] = z;
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_set1Value__ILvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  VrmlMFVec3f* pMFVec3f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  VrmlSFVec3f* pSFVec3f;
  fid = getFid(env, sfvec3f, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(sfvec3f, fid);
  memcpy((void*) pMFVec3f->getElement(index), pSFVec3f->get(),
	 3 * sizeof(float));
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_addValue__FFF
  (JNIEnv *env, jobject obj, jfloat x, jfloat y, jfloat z)
{
  VrmlMFVec3f* pMFVec3f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  float farr[3];
  farr[0] = x;
  farr[1] = y;
  farr[2] = z;
  pMFVec3f->insertElement(pMFVec3f->getLength(), farr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_addValue__Lvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jobject sfvec3f)
{
  VrmlMFVec3f* pMFVec3f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  VrmlSFVec3f* pSFVec3f;
  fid = getFid(env, sfvec3f, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(sfvec3f, fid);
  pMFVec3f->insertElement(pMFVec3f->getLength(), pSFVec3f->get());
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_insertValue__IFFF
  (JNIEnv *env, jobject obj, jint index, jfloat x, jfloat y, jfloat z)
{
  VrmlMFVec3f* pMFVec3f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  float farr[3];
  farr[0] = x;
  farr[1] = y;
  farr[2] = z;
  pMFVec3f->insertElement(index, farr);
}

JNIEXPORT void JNICALL Java_vrml_field_MFVec3f_insertValue__ILvrml_field_ConstSFVec3f_2
  (JNIEnv *env, jobject obj, jint index, jobject sfvec3f)
{
  VrmlMFVec3f* pMFVec3f;
  jfieldID fid;
  fid = getFid(env, obj, "FieldPtr", "I");
  pMFVec3f = (VrmlMFVec3f*) env->GetIntField(obj, fid);
  VrmlSFVec3f* pSFVec3f;
  fid = getFid(env, sfvec3f, "FieldPtr", "I");
  pSFVec3f = (VrmlSFVec3f*) env->GetIntField(sfvec3f, fid);
  pMFVec3f->insertElement(index, pSFVec3f->get());
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
  jobject EventOut;

  cout << "TODO: Implement Java_vrml_node_Script_getEventOut" << endl;

  return EventOut;
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
  jfieldID fid;
  VrmlNode* pNode;
  jobject jBrowser;
  jclass clazz;
  
  fid = getFid(env, obj, "NodePtr", "I");
  pNode = (VrmlNode*) env->GetIntField(obj, fid);
  clazz = env->FindClass("vrml/Browser");
  jBrowser = env->AllocObject(clazz);
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
  jobject EventIn;

  cout << "TODO: Implement Java_vrml_node_Node_getEventIn" << endl;

  return EventIn;
}


JNIEXPORT jobject JNICALL Java_vrml_node_Node_getEventOut
  (JNIEnv *env, jobject obj, jstring jstrEventOutName)
{;
  jobject EventOut;

  cout << "TODO: Implement Java_vrml_node_Node_getEventOut" << endl;

  return EventOut;
}

JNIEXPORT jobject JNICALL Java_vrml_node_Node_getExposedField
  (JNIEnv *env, jobject obj, jstring jstrExposedFieldName)
{
  jobject ExposedField;

  cout << "TODO: Implement Java_vrml_node_Node_getExposedField" << endl;

  return ExposedField;
}

//
// Event
//

JNIEXPORT jstring JNICALL Java_vrml_Event_getName
  (JNIEnv *env, jobject obj)
{
  cout << "TODO: Implement Java_vrml_Event_getName" << endl;

  return env->NewStringUTF("blah");
}

JNIEXPORT jdouble JNICALL Java_vrml_Event_getTimeStamp
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlEvent* pEvent;
  fid = getFid(env, obj, "EventPtr", "I");
  pEvent = (VrmlEvent*) env->GetIntField(obj, fid);
  return (jdouble) pEvent->timeStamp(); 
}

JNIEXPORT jobject JNICALL Java_vrml_Event_getValue
  (JNIEnv *env, jobject obj)
{
  jfieldID fid;
  VrmlEvent* pEvent;
  const VrmlField* pField;
  jobject Field;
  char clazzName[256];
  jclass clazz;
  
  fid = getFid(env, obj, "EventPtr", "I");
  pEvent = (VrmlEvent*) env->GetIntField(obj, fid);
  pField = pEvent->value();
  
  sprintf(clazzName, "vrml/field/Const%s", pField->fieldTypeName());
  clazz = env->FindClass(clazzName);
  
  Field = env->AllocObject(clazz);
  fid = env->GetFieldID(clazz, "FieldPtr", "I");
  env->SetIntField(Field, fid, (int) pField);
  return Field;
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

} // extern "C"

#endif // OPENVRML_HAVE_JAVA
