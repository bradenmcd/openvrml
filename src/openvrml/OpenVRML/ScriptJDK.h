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

#if OPENVRML_ENABLE_SCRIPT_NODE_JAVA
#include "script.h"

#include <jni.h>

#include "field.h"

namespace OpenVRML {

    class ScriptNode;

    class ScriptJDK : public Script {
        // Shared by all JDK Script objects
        static JavaVM * d_jvm;
        static JNIEnv * d_env;

        jclass d_class;
        jobject d_object;
        jmethodID d_processEventsID, d_processEventID, d_eventsProcessedID;

    public:
        ScriptJDK(ScriptNode & scriptNode,
                  const char * className, const char * classDir);
        virtual ~ScriptJDK();

        virtual void initialize(double timestamp);
        virtual void processEvent(const std::string & id,
                                  const FieldValue & value, double timestamp);
        virtual void eventsProcessed(double timestamp);
        virtual void shutdown(double timestamp);

    private:
        void activate(double timeStamp, const std::string & fname,
                      size_t argc, const FieldValue * const argv[]);
    };
}

#endif // OPENVRML_ENABLE_SCRIPT_NODE_JAVA
#endif // _SCRIPTJDK_
