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

#ifndef VRMLNODESCRIPT_H
#define VRMLNODESCRIPT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <list>
#include "common.h"
#include "vrml97node.h"

namespace OpenVRML {

    class Doc;
    class Script;
    class VrmlScene;

    class OPENVRML_SCOPE ScriptNode : public NodeChild {
    public:
        // Per-script field type
        struct ScriptField {
            std::string name;
            FieldValue * value;
            FieldValue::FieldType type;
            bool modified;
        };

        typedef std::list<ScriptField *> FieldList;

    private:
        // Fields
        SFBool d_directOutput;
        SFBool d_mustEvaluate;
        MFString d_url;

        SFString d_relativeUrl;

        // The script language-dependent part
        Script * script;

        // Fields and events defined for this Script
        FieldList d_eventIns;
        FieldList d_eventOuts;
        FieldList d_fields;

        int d_eventsReceived;

    public:
        // Define the fields of Script nodes
        static const NodeTypePtr
                defineType(NodeTypePtr nodeType = NodeTypePtr(0));

        ScriptNode(VrmlScene * scene = 0);
        ScriptNode(const ScriptNode &);
        virtual ~ScriptNode();

        void initialize(double timeStamp);
        void update(const SFTime & now);
        void shutdown(double timeStamp);
        void addEventIn(const std::string & name, FieldValue::FieldType type);
        void addEventOut(const std::string & name, FieldValue::FieldType type);
        void addField(const std::string & name, FieldValue::FieldType type,
		      const FieldValue * defaultVal = 0);
        FieldValue::FieldType hasEventIn(const std::string & id) const;
        FieldValue::FieldType hasEventOut(const std::string & id) const;
        FieldValue::FieldType hasField(const std::string & id) const;
        FieldValue::FieldType hasInterface(const std::string & id) const;
        void setEventIn(const std::string &, const FieldValue &);
        void setEventOut(const std::string &, const FieldValue &);
        FieldList & eventIns();
        FieldList & eventOuts() { return d_eventOuts; }
        FieldList & fields() { return d_fields; }
        VrmlScene * browser() { return d_scene; }

        virtual bool accept(NodeVisitor & visitor);
        virtual void resetVisitedFlag();
        virtual const FieldValue * getField(const std::string & fieldId) const;
        virtual void setField(const std::string & fieldId,
                              const FieldValue & fieldValue);
        virtual void eventIn(double timeStamp, const std::string & eventName,
		             const FieldValue & fieldValue);
        virtual const MFNode getChildren() const;
        virtual void addToScene(VrmlScene * scene,
                                const std::string & relativeUrl);

        virtual ScriptNode * toScript() const;

        virtual ostream& printFields(ostream& os, int indent);

    private:
        Script * createScript();

        // Generic field/event test/value methods
        FieldValue::FieldType has(const FieldList &, const std::string &) const;
        FieldValue * get(const FieldList &, const std::string &) const;
        void set(const FieldList &, const std::string &, const FieldValue &);
    };
}

#endif

