//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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

#include "doc2.hpp"
#include "MathUtils.h"
#include "System.h"
#include "VrmlNodeScript.h"
#include "VrmlNamespace.h"
#include "VrmlNodeType.h"
#include "VrmlScene.h"

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
      JDK1_1InitArgs vm_args;
      jint res;

      /* IMPORTANT: specify vm_args version # if you use JDK1.1.2 and beyond */
      vm_args.version = 0x00010001;

      // Append classDir to the end of default system class path
      JNI_GetDefaultJavaVMInitArgs(&vm_args);

      if (classDir)
	{
	  char fullpath[1024];
#ifndef macintosh
	  strncpy(fullpath, vm_args.classpath, sizeof(fullpath)-1);
	  fullpath[sizeof(fullpath)-1] = '\0';
	  int sleft = sizeof(fullpath) - strlen(fullpath) - 1;
	  strncat(fullpath, PATH_SEPARATOR, sleft);
	  sleft -= strlen(PATH_SEPARATOR);
	  strncat(fullpath, classDir, sleft);
#else /* Apple's MRJ always returns null for system class path */
      /* *** Needs the fullpath of the classDir here.... (this is not) *** */
	  strcpy(fullpath, classDir);
#endif
	  fullpath[sizeof(fullpath)-1] = '\0';
	  vm_args.classpath = fullpath;
	}

      /* Create the Java VM */
      res = JNI_CreateJavaVM(&d_jvm, (void**) &d_env, &vm_args);
      if (res < 0)
	theSystem->error("Can't create Java VM");
    }

  if (d_jvm && d_env)		// Per-object initialization
    {
      char fqClassName[1024];
      strcpy(fqClassName, "vrml/node/Script/"); // is this correct?...
      strcat(fqClassName, className);
      d_class = d_env->FindClass(fqClassName);
      if (d_class == 0) {
	theSystem->error( "Can't find Java class %s.\n", className);
	return;
      }

      // Create an object of this class
      //      d_object = ?

      // Call initialize()
      jmethodID initID = d_env->GetStaticMethodID(d_class, "initialize","()V");

      if (initID != 0)
	{
	  d_env->CallVoidMethod(d_object, initID);
	}

      // Cache other method IDs
      d_processEventsID =
	d_env->GetStaticMethodID(d_class, "processEvents",
				 "(I[Ljava/Cloneable/Event;)V");
      d_eventsProcessedID =
	d_env->GetStaticMethodID( d_class, "eventsProcessed",
				  "(I[Ljava/Cloneable/Event;)V");

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


// Run a specified script

void ScriptJDK::activate( double timeStamp,
			  const std::string & fname,
			  size_t argc,
			  const VrmlField * const argv[] )
{
  jstring jstr;
  jobjectArray args;

  if (! d_class) return;

  jstr = d_env->NewStringUTF(" from VRML!");
  if (jstr == 0) {
    theSystem->error( "Out of memory\n");
    return;
  }
  args = d_env->NewObjectArray( 1, d_env->FindClass("java/lang/String"), jstr);
  if (args == 0) {
    theSystem->error( "Out of memory\n");
    return;
  }
  d_env->CallStaticVoidMethod(d_class, d_processEventsID, args);

}

// Get a handle to the scene from a ScriptJDK

VrmlScene *ScriptJDK::browser() { return d_node->browser(); }

#endif // OPENVRML_HAVE_JAVA
