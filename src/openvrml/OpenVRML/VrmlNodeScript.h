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
#include "vrml97node.h"

class Doc;
class ScriptObject;
class VrmlScene;


class VrmlNodeScript : public VrmlNodeChild {

public:

  // Define the fields of Script nodes
  static VrmlNodeType *defineType(VrmlNodeType *t = 0);
  virtual VrmlNodeType & nodeType() const;

  VrmlNodeScript( VrmlScene *scene = 0 );
  VrmlNodeScript( const VrmlNodeScript& );
  virtual ~VrmlNodeScript();

  virtual bool accept(VrmlNodeVisitor & visitor);
  virtual void resetVisitedFlag();
  
  virtual VrmlNodeScript* toScript() const;

  virtual void addToScene( VrmlScene *s, const char *relUrl );

  virtual ostream& printFields(ostream& os, int indent);

  virtual void eventIn(double timeStamp,
		       const char *eventName,
		       const VrmlField & fieldValue);

  virtual const VrmlField *getField(const char *fieldName) const;
  virtual void setField(const char *fieldName, const VrmlField &fieldValue);

  // Script processing methods
  void initialize( double timeStamp );
  void update( VrmlSFTime &now );
  void shutdown( double timeStamp );

  // Methods for adding eventIns/Outs/fields to this script
  void addEventIn(const char *name, VrmlField::VrmlFieldType type);
  void addEventOut(const char *name, VrmlField::VrmlFieldType type);
  void addField(const char *name, VrmlField::VrmlFieldType type,
		const VrmlField * defaultVal = 0);

  // Access to eventIns/Outs/fields for ScriptObjects

  // Per-script field type
  struct ScriptField {
    char *name;
    VrmlField *value;
    VrmlField::VrmlFieldType type;
    bool modified;
  };

  typedef std::list< ScriptField* > FieldList;

  // Tests for specific fields/events
  VrmlField::VrmlFieldType hasEventIn(const char *name) const;
  VrmlField::VrmlFieldType hasEventOut(const char *name) const;
  VrmlField::VrmlFieldType hasField(const char *name) const;
  VrmlField::VrmlFieldType hasInterface(char const *) const;

  // Set field/event values
  void setEventIn(const char *, const VrmlField &);
  void setEventOut(const char *, const VrmlField &);
  // setField declared above as virtual

  // Fields and events defined for this Script
  FieldList &eventIns() { return d_eventIns; }
  FieldList &eventOuts() { return d_eventOuts; }
  FieldList &fields() { return d_fields; }

  // Access to browser functions for ScriptObjects
  VrmlScene *browser() { return d_scene; }

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

  // Generic field/event add/test/value methods
  VrmlField::VrmlFieldType has(const FieldList &, const char *) const;
  VrmlField* get(const FieldList &, const char *) const;
  void set(const FieldList &, const char *, const VrmlField &);

  int d_eventsReceived;
};

#endif

