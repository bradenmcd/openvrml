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

class Doc;
class ScriptObject;
class VrmlScene;

class OPENVRML_SCOPE VrmlNodeScript : public VrmlNodeChild {
public:
    // Per-script field type
    struct ScriptField {
        std::string name;
        VrmlField * value;
        VrmlField::VrmlFieldType type;
        bool modified;
    };
    
    typedef std::list<ScriptField *> FieldList;

private:
    // Fields
    VrmlSFBool d_directOutput;
    VrmlSFBool d_mustEvaluate;
    VrmlMFString d_url;

    VrmlSFString d_relativeUrl;

    // The script language-dependent part
    ScriptObject *d_script;

    // Fields and events defined for this Script
    FieldList d_eventIns;
    FieldList d_eventOuts;
    FieldList d_fields;

    int d_eventsReceived;

public:
    // Define the fields of Script nodes
    static const NodeTypePtr defineType(NodeTypePtr nodeType = NodeTypePtr(0));

    VrmlNodeScript(VrmlScene * scene = 0);
    VrmlNodeScript(const VrmlNodeScript &);
    virtual ~VrmlNodeScript();

    void initialize(double timeStamp);
    void update(const VrmlSFTime & now);
    void shutdown(double timeStamp);
    void addEventIn(const std::string & name, VrmlField::VrmlFieldType type);
    void addEventOut(const std::string & name, VrmlField::VrmlFieldType type);
    void addField(const std::string & name, VrmlField::VrmlFieldType type,
		  const VrmlField * defaultVal = 0);
    VrmlField::VrmlFieldType hasEventIn(const std::string & id) const;
    VrmlField::VrmlFieldType hasEventOut(const std::string & id) const;
    VrmlField::VrmlFieldType hasField(const std::string & id) const;
    VrmlField::VrmlFieldType hasInterface(const std::string & id) const;
    void setEventIn(const std::string &, const VrmlField &);
    void setEventOut(const std::string &, const VrmlField &);
    FieldList & eventIns();
    FieldList & eventOuts() { return d_eventOuts; }
    FieldList & fields() { return d_fields; }
    VrmlScene * browser() { return d_scene; }

    virtual bool accept(VrmlNodeVisitor & visitor);
    virtual void resetVisitedFlag();
    virtual const VrmlField * getField(const std::string & fieldId) const;
    virtual void setField(const std::string & fieldId,
                          const VrmlField & fieldValue);
    virtual void eventIn(double timeStamp,
		         const std::string & eventName,
		         const VrmlField & fieldValue);
    virtual const VrmlMFNode getChildren() const;
    virtual void addToScene(VrmlScene * scene, const char * relUrl);

    virtual VrmlNodeScript* toScript() const;

    virtual ostream& printFields(ostream& os, int indent);

private:
    // Generic field/event test/value methods
    VrmlField::VrmlFieldType has(const FieldList &, const std::string &) const;
    VrmlField * get(const FieldList &, const std::string &) const;
    void set(const FieldList &, const std::string &, const VrmlField &);
    ScriptObject * createScript();
};

#endif

