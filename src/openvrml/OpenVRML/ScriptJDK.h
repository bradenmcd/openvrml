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
#ifndef _SCRIPTJDK_
#define _SCRIPTJDK_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

# if defined(_WIN32) && !defined(__CYGWIN__)
#   include <winconfig.h>
# endif

#ifdef OPENVRML_HAVE_JAVA
//
//  Java (via Sun JDK) Script class
//
#include "ScriptObject.h"

#include <jni.h>

#include "field.h"

class VrmlNodeScript;
class VrmlScene;

class ScriptJDK: public ScriptObject {

public:

  ScriptJDK( VrmlNodeScript *, const char *className, const char *classDir );
  ~ScriptJDK();

  virtual void activate( double timeStamp,
			 const std::string & fname,
			 size_t argc,
			 const VrmlField * const argv[]);

  VrmlScene *browser();
  VrmlNodeScript *scriptNode() { return d_node; }

protected:

  // Shared by all JDK Script objects
  static JavaVM *d_jvm;
  static JNIEnv *d_env;

  // Per object data members
  VrmlNodeScript *d_node;

  jclass d_class;
  jobject d_object;
  jmethodID d_processEventsID, d_processEventID, d_eventsProcessedID;
};

#endif // OPENVRML_HAVE_JAVA
#endif // _SCRIPTJDK_

