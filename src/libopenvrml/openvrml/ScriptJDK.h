// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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

#include "field_value.h"

namespace openvrml {

    class script_node;

    class ScriptJDK : public script {
        // Shared by all JDK script objects
        static JavaVM * d_jvm;
        static JNIEnv * d_env;

        jclass d_class;
        jobject d_object;
        jmethodID d_processEventsID, d_processEventID, d_eventsProcessedID;

    public:
        ScriptJDK(script_node & scriptNode,
                  const char * className, const char * classDir);
        virtual ~ScriptJDK();

    private:
        virtual void do_initialize(double timestamp);
        virtual void do_process_event(const std::string & id,
                                      const field_value & value,
                                      double timestamp);
        virtual void do_events_processed(double timestamp);
        virtual void do_shutdown(double timestamp);

        void activate(double timeStamp, const std::string & fname,
                      size_t argc, const field_value * const argv[]);
    };
}

#endif // OPENVRML_ENABLE_SCRIPT_NODE_JAVA
#endif // _SCRIPTJDK_
