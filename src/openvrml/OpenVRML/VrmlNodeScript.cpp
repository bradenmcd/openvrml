//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
// Copyright (C) 2001  Braden McDaniel
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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# if defined(_WIN32) && !defined(__CYGWIN__)
#   include <winconfig.h>
# endif

# include <math.h>
# include <assert.h>
# include <memory>
# include <vector>
# include <algorithm>
# if defined(_WIN32) && !defined(__CYGWIN__)
#   include <strstrea.h>
# else
#   include <strstream.h>
# endif
# include "private.h"
# include "VrmlNodeScript.h"
# include "nodetype.h"
# include "VrmlNodeVisitor.h"
# include "VrmlNamespace.h"
# include "ScriptObject.h"
# include "VrmlScene.h"
# include "doc2.hpp"
# include "ScriptJDK.h"

#if defined(WIN32)
// Disable warning messages about forcing value to bool 'true' or 'false'
#pragma warning (disable:4800)
#endif

/**
 * @class VrmlNodeScript
 *
 * @brief Represents a VRML Script node.
 */

namespace {
    /**
     * Script factory. Add each Script to the scene for fast access.
     */
    const VrmlNodePtr creator(VrmlScene * const scene) {
        return VrmlNodePtr(new VrmlNodeScript(scene));
    }
}

/**
 * Define the built in NodeType:: "Script" fields
 */
const NodeTypePtr VrmlNodeScript::defineType(NodeTypePtr nodeType) {
    static NodeTypePtr st(0);

    if (!nodeType) {
        if (st) {
            return st; // Only define the type once.
        }
        st.reset(new NodeType("Script", creator));
        nodeType = st;
    }

    nodeType->addExposedField("url", VrmlField::MFSTRING);
    nodeType->addField("directOutput", VrmlField::SFBOOL);
    nodeType->addField("mustEvaluate", VrmlField::SFBOOL);

    return nodeType;
}

VrmlNodeScript::VrmlNodeScript(VrmlScene * const scene):
        VrmlNodeChild(*defineType(), scene), d_directOutput(false),
        d_mustEvaluate(false), d_script(0), d_eventsReceived(0) {
    if (this->d_scene) {
        this->d_scene->addScript(*this);
    }
}

VrmlNodeScript::VrmlNodeScript(const VrmlNodeScript & node):
        VrmlNodeChild(node), d_directOutput(node.d_directOutput),
        d_mustEvaluate(node.d_mustEvaluate), d_url(node.d_url), d_script(0),
        d_eventIns(0), d_eventOuts(0), d_fields(0), d_eventsReceived(0) {
  // add eventIn/eventOut/fields from source Script
  FieldList::const_iterator i;

  for (i = node.d_eventIns.begin(); i != node.d_eventIns.end(); ++i)
    addEventIn( (*i)->name, (*i)->type );
  for (i = node.d_eventOuts.begin(); i != node.d_eventOuts.end(); ++i)
    addEventOut( (*i)->name, (*i)->type );
  for (i = node.d_fields.begin(); i != node.d_fields.end(); ++i)
    addField(  (*i)->name, (*i)->type, (*i)->value );
}


VrmlNodeScript::~VrmlNodeScript()
{
  shutdown( theSystem->time() );

  // removeScript ought to call shutdown...
  if (d_scene) d_scene->removeScript(*this);

  delete d_script;

  // delete eventIn/eventOut/field ScriptField list contents
  FieldList::iterator i;
  for (i = d_eventIns.begin(); i != d_eventIns.end(); ++i)
    {
      ScriptField *r = *i;
      delete r->value;
      delete r;
    }
  for (i = d_eventOuts.begin(); i != d_eventOuts.end(); ++i)
    {
      ScriptField *r = *i;
      delete r->value;
      delete r;
    }
  for (i = d_fields.begin(); i != d_fields.end(); ++i)
    {
      ScriptField *r = *i;
      delete r->value;
      delete r;
    }
}

bool VrmlNodeScript::accept(VrmlNodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    
    return false;
}

void VrmlNodeScript::resetVisitedFlag() {
    if (this->visited) {
        this->visited = false;
        for (FieldList::const_iterator itr = this->d_fields.begin();
                itr != this->d_fields.end(); ++itr) {
            assert((*itr)->value);
            if ((*itr)->type == VrmlField::SFNODE) {
                assert(dynamic_cast<VrmlSFNode *>((*itr)->value));
                static_cast<VrmlSFNode *>((*itr)->value)
                        ->get()->resetVisitedFlag();
            } else if ((*itr)->type == VrmlField::MFNODE) {
                assert(dynamic_cast<VrmlMFNode *>((*itr)->value));
                VrmlMFNode & mfnode = static_cast<VrmlMFNode &>(*(*itr)->value);
                for (size_t i = 0; i < mfnode.getLength(); ++i) {
                    mfnode.getElement(i)->resetVisitedFlag();
                }
            }
        }
    }
}

namespace {
    struct AccumulateNodes_ :
            public std::unary_function<VrmlNodeScript::FieldList::value_type, void> {
        explicit AccumulateNodes_(VrmlMFNode & children): children(children) {}
        
        result_type operator()(argument_type scriptField) {
            if (scriptField->type == VrmlField::SFNODE) {
                assert(dynamic_cast<VrmlSFNode *>(scriptField->value));
                this->children.setLength(this->children.getLength() + 1);
                this->children.setElement(this->children.getLength() - 1,
                        static_cast<VrmlSFNode *>(scriptField->value)->get());
            } else if (scriptField->type == VrmlField::MFNODE) {
                assert(dynamic_cast<VrmlMFNode *>(scriptField->value));
                VrmlMFNode &
                        nodes = *static_cast<VrmlMFNode *>(scriptField->value);
                this->children.setLength(children.getLength()
                                            + nodes.getLength());
                for (size_t i = 0; i < nodes.getLength(); ++i) {
                    this->children.setElement(this->children.getLength() + i,
                                              nodes.getElement(i));
                }
            }
        }
    
    private:
        VrmlMFNode & children;
    };
}

const VrmlMFNode VrmlNodeScript::getChildren() const {
    VrmlMFNode children;
    std::for_each(this->d_fields.begin(), this->d_fields.end(),
                  AccumulateNodes_(children));
    return children;
}

VrmlNodeScript* VrmlNodeScript::toScript() const
{ return (VrmlNodeScript*) this; }

void VrmlNodeScript::addToScene(VrmlScene * const scene,
                                const char * const relUrl) {
    this->d_relativeUrl.set(relUrl);
    if (this->d_scene == scene) {
        return;
    }
    if ((this->d_scene = scene)) {
        this->initialize(theSystem->time());
        this->d_scene->addScript(*this);
    }
}


ostream& VrmlNodeScript::printFields(ostream& os, int indent)
{
  if (d_url.getLength() > 0) PRINT_FIELD(url);
  if (d_directOutput.get()) PRINT_FIELD(directOutput);
  if (d_mustEvaluate.get()) PRINT_FIELD(mustEvaluate);

  return os;
}


void VrmlNodeScript::initialize(const double timestamp) {
    assert(!this->d_script);

    this->d_eventsReceived = 0;
    this->d_script = this->createScript();
    if (this->d_script) {
        this->d_script->activate(timestamp, "initialize", 0, 0);
    }
    
    //
    // For each modified eventOut, send an event.
    //
    for (FieldList::const_iterator i = this->d_eventOuts.begin();
            i != this->d_eventOuts.end(); ++i) {
        if ((*i)->modified) {
            this->eventOut(timestamp, (*i)->name, *((*i)->value));
        }
    }
}

void VrmlNodeScript::shutdown(const double timestamp) {
    if (this->d_script) {
        this->d_script->activate(timestamp, "shutdown", 0, 0);
    }
}

void VrmlNodeScript::update(const VrmlSFTime & timeNow) {
    if (this->d_eventsReceived > 0) {
        if (this->d_script) {
            this->d_script->activate(timeNow.get(), "eventsProcessed", 0, 0);
        }
        this->d_eventsReceived = 0;
    }
}


void VrmlNodeScript::eventIn(double timeStamp,
			     const std::string & eventName,
			     const VrmlField & fieldValue) {
    if (!d_script) {
        this->initialize(timeStamp);
    }
    
    if (!d_script) {
        return;
    }
    
    static const char * eventInPrefix = "set_";
    std::string basicEventName;
    if (std::equal(eventInPrefix, eventInPrefix + 4, eventName.begin())) {
        basicEventName = eventName.substr(4);
    } else {
        basicEventName = eventName;
    }
    bool valid = (this->hasEventIn(eventName)
                || this->hasEventIn(basicEventName));
    if (valid) {
        this->setEventIn(eventName, fieldValue);

        VrmlSFTime ts( timeStamp );
        const VrmlField *args[] = { &fieldValue, &ts };

        FieldList::const_iterator i;
        for (i = d_eventOuts.begin(); i != d_eventOuts.end(); ++i)
	  (*i)->modified = false;

        d_script->activate( timeStamp, eventName, 2, args );

        // For each modified eventOut, send an event
        for (i = d_eventOuts.begin(); i != d_eventOuts.end(); ++i) {
	    if ((*i)->modified) {
                eventOut(timeStamp, (*i)->name, *((*i)->value));
            }
        }

        ++d_eventsReceived;	// call eventsProcessed later
    }

    // Let the generic code handle the rest.
    else
      VrmlNode::eventIn(timeStamp, eventName, fieldValue);

    // Scripts shouldn't generate redraws.
    clearModified();
}



// add events/fields

namespace {
    void add(VrmlNodeScript::FieldList & recs, const std::string & ename,
             VrmlField::VrmlFieldType type) {
        VrmlNodeScript::ScriptField * const scriptField =
                new VrmlNodeScript::ScriptField;
        scriptField->name = ename;
        scriptField->type = type;
        switch (type) {
        case VrmlField::SFBOOL:
            scriptField->value = new VrmlSFBool();
            break;
        case VrmlField::SFCOLOR:
            scriptField->value = new VrmlSFColor();
            break;
        case VrmlField::SFFLOAT:
            scriptField->value = new VrmlSFFloat();
            break;
        case VrmlField::SFIMAGE:
            scriptField->value = new VrmlSFImage();
            break;
        case VrmlField::SFINT32:
            scriptField->value = new VrmlSFInt32();
            break;
        case VrmlField::SFNODE:
            scriptField->value = new VrmlSFNode();
            break;
        case VrmlField::SFROTATION:
            scriptField->value = new VrmlSFRotation();
            break;
        case VrmlField::SFSTRING:
            scriptField->value = new VrmlSFString();
            break;
        case VrmlField::SFTIME:
            scriptField->value = new VrmlSFTime();
            break;
        case VrmlField::SFVEC2F:
            scriptField->value = new VrmlSFVec2f();
            break;
        case VrmlField::SFVEC3F:
            scriptField->value = new VrmlSFVec3f();
            break;
        case VrmlField::MFCOLOR:
            scriptField->value = new VrmlMFColor();
            break;
        case VrmlField::MFFLOAT:
            scriptField->value = new VrmlMFFloat();
            break;
        case VrmlField::MFINT32:
            scriptField->value = new VrmlMFInt32();
            break;
        case VrmlField::MFNODE:
            scriptField->value = new VrmlMFNode();
            break;
        case VrmlField::MFROTATION:
            scriptField->value = new VrmlMFRotation();
            break;
        case VrmlField::MFSTRING:
            scriptField->value = new VrmlMFString();
            break;
        case VrmlField::MFTIME:
            scriptField->value = new VrmlMFTime();
            break;
        case VrmlField::MFVEC2F:
            scriptField->value = new VrmlMFVec2f();
            break;
        case VrmlField::MFVEC3F:
            scriptField->value = new VrmlMFVec3f();
            break;
        default:
            assert(false);
            break;
        }
        
        recs.push_front(scriptField);
    }
}

void VrmlNodeScript::addEventIn(const std::string & ename,
                                VrmlField::VrmlFieldType t) {
    add(this->d_eventIns, ename, t);
}

void VrmlNodeScript::addEventOut(const std::string & ename,
                                 VrmlField::VrmlFieldType t) {
    add(this->d_eventOuts, ename, t);
}

void VrmlNodeScript::addField(const std::string & ename,
                              VrmlField::VrmlFieldType t,
			      const VrmlField * val) {
    add(this->d_fields, ename, t);
    if (val) {
        this->set(this->d_fields, ename, *val);
    }
}

VrmlField * VrmlNodeScript::get(const FieldList & recs,
                                const std::string & fieldId) const {
    for (FieldList::const_iterator i(recs.begin()); i != recs.end(); ++i) {
        if ((*i)->name == fieldId) {
            return (*i)->value;
        }
    }
    return 0;
}

// has

VrmlField::VrmlFieldType VrmlNodeScript::hasEventIn(const std::string & id)
        const {
    return has(d_eventIns, id);
}

VrmlField::VrmlFieldType VrmlNodeScript::hasEventOut(const std::string & id)
        const {
    return has(d_eventOuts, id);
}

VrmlField::VrmlFieldType VrmlNodeScript::hasField(const std::string & id)
        const {
    return has(d_fields, id);
}

VrmlField::VrmlFieldType VrmlNodeScript::hasInterface(const std::string & id)
        const {
    VrmlField::VrmlFieldType fieldType = VrmlField::NO_FIELD;
    
    if ((fieldType = this->hasField(id)) != VrmlField::NO_FIELD) {
        return fieldType;
    }
    
    if ((fieldType = this->hasEventIn(id)) != VrmlField::NO_FIELD) {
        return fieldType;
    }
    
    if ((fieldType = this->hasEventOut(id)) != VrmlField::NO_FIELD) {
        return fieldType;
    }
    
    return fieldType;
}

VrmlField::VrmlFieldType VrmlNodeScript::has(const FieldList & recs,
                                             const std::string & id) const {
    for (FieldList::const_iterator i(recs.begin()); i != recs.end(); ++i) {
        if ((*i)->name == id) {
            return (*i)->type;
        }
    }
    return VrmlField::NO_FIELD;
}

// Get the value of a field or eventOut.

const VrmlField * VrmlNodeScript::getField(const std::string & fieldId) const
{
    // exposedFields
    if (fieldId == "url") {
        return &d_url;
    }

    // look up 
    else if (this->hasField(fieldId)) {
        return get(d_fields, fieldId);
    }

    // look up event outs
    else if (this->hasEventOut(fieldId)) {
        return get(d_eventOuts, fieldId);
    }

    return VrmlNodeChild::getField(fieldId);
}


// Set the value of one of the node fields/events.
// setField is public so the parser can access it.

void VrmlNodeScript::setField(const std::string & fieldId,
			      const VrmlField & fieldValue) {
  VrmlField::VrmlFieldType ft;

  if TRY_FIELD(url, MFString)	// need to re-initialize() if url changes...
  else if TRY_FIELD(directOutput, SFBool)
  else if TRY_FIELD(mustEvaluate, SFBool)
  else if ( (ft = hasField(fieldId)) != 0 )
    {
      if (ft == VrmlField::fieldType( fieldValue.fieldTypeName() ))
	set(d_fields, fieldId, fieldValue);
      else
	theSystem->error("Invalid type (%s) for %s field of Script node.\n",
                         fieldValue.fieldTypeName(), fieldId.c_str());
    }
  else
    VrmlNodeChild::setField(fieldId, fieldValue);
}

VrmlNodeScript::FieldList & VrmlNodeScript::eventIns() {
    return this->d_eventIns;
}

void VrmlNodeScript::setEventIn(const std::string & fname,
                                const VrmlField & value) {
    this->set(d_eventIns, fname, value);
}

void VrmlNodeScript::setEventOut(const std::string & fname,
                                 const VrmlField & value) {
    this->set(d_eventOuts, fname, value);
}

void VrmlNodeScript::set(const FieldList & recs, const std::string & fieldId,
                         const VrmlField & value) {
    for (FieldList::const_iterator itr = recs.begin(); itr != recs.end();
            ++itr) {
        if ((*itr)->name == fieldId) {
            //
            // Script nodes can be self referential! Check this condition,
            // and "undo" the refcounting: decrement the refcount on any
            // self-references we acquire ownership of, and increment the
            // refcount on any self-references for which we relinquish
            // ownership.
            //
            const VrmlField::VrmlFieldType fieldType(value.fieldType());
            if (fieldType == VrmlField::SFNODE) {
                const VrmlNodePtr & oldNode =
                        static_cast<VrmlSFNode *>((*itr)->value)->get();
                //
                // About to relinquish ownership of a SFNode value. If the
                // SFNode value is this Script node, then we need to
                // *increment* its refcount, since we previously
                // *decremented* it to accommodate creating a cycle between
                // refcounted objects.
                //
                if (oldNode && (oldNode.countPtr->first == this)) {
                    ++(oldNode.countPtr->second);
                }
                
	        delete (*itr)->value;
	        (*itr)->value = value.clone();
                
                //
                // Now, check to see if the new SFNode value is a self-
                // reference. If it is, we need to *decrement* the refcount.
                // A self-reference creates a cycle. If a Script node with
                // a self-reference were completely removed from the scene,
                // it still wouldn't be deleted (if we didn't do this)
                // because the reference it held to itself would prevent the
                // refcount from ever dropping to zero.
                //
                const VrmlNodePtr & newNode =
                        static_cast<VrmlSFNode *>((*itr)->value)->get();
                if (newNode && (newNode.countPtr->first == this)) {
                    --(newNode.countPtr->second);
                }
            } else if (fieldType == VrmlField::MFNODE) {
                size_t i;
                const VrmlMFNode & oldNodes =
                        static_cast<VrmlMFNode &>(*(*itr)->value);
                for (i = 0; i < oldNodes.getLength(); ++i) {
                    const VrmlNodePtr & node = oldNodes.getElement(i);
                    if (node && (node.countPtr->first == this)) {
                        ++(node.countPtr->second);
                    }
                }
                
                delete (*itr)->value;
                (*itr)->value = value.clone();
                
                const VrmlMFNode & newNodes =
                        static_cast<VrmlMFNode &>(*(*itr)->value);
                for (i = 0; i < newNodes.getLength(); ++i) {
                    const VrmlNodePtr & node = newNodes.getElement(i);
                    if (node && (node.countPtr->first == this)) {
                        --(node.countPtr->second);
                    }
                }
            } else {
	        delete (*itr)->value;
	        (*itr)->value = value.clone();
            }
            
	    (*itr)->modified = true;
	    return;
        }
    }
}

# ifdef OPENVRML_HAVE_JS
#   include <mozilla/jsapi.h>
namespace {

    namespace JavaScript_ {
        
        class Script : public ScriptObject {
            static JSRuntime * rt;
            static size_t nInstances;

            double d_timeStamp;

            JSContext * cx;

        public:
            Script(VrmlNodeScript & scriptNode, const std::string & source)
                throw (std::bad_alloc);
            virtual ~Script();

            virtual void activate(double timeStamp, const std::string & fname,
                                  size_t argc, const VrmlField * const argv[]);

            VrmlNodeScript & getScriptNode();

            jsval vrmlFieldToJSVal(const VrmlField & f, bool protect);

        private:
            bool initVrmlClasses() throw ();
            bool defineBrowserObject() throw ();
            bool defineFields() throw ();
        };
    }
}
# endif

ScriptObject * VrmlNodeScript::createScript() {
    // Try each url until we find one we like
    for (size_t i = 0; i < this->d_url.getLength(); ++i) {
        if (this->d_url.getElement(i).length() == 0) continue;

        // Get the protocol & mimetype...
# ifdef OPENVRML_HAVE_JS
        // Need to handle external .js files too...
        const char javascriptScheme[] = "javascript:";
        const char vrmlscriptScheme[] = "vrmlscript:";
        if (std::equal(javascriptScheme, javascriptScheme + 11,
                       this->d_url.getElement(i).begin())
                || std::equal(vrmlscriptScheme, vrmlscriptScheme + 11,
                              this->d_url.getElement(i).begin())) {
            return new JavaScript_::Script(*this,
                                        this->d_url.getElement(i).substr(11));
	}
# endif

#ifdef OPENVRML_HAVE_JAVA
	const char javaExtension1[] = ".class";
	const char javaExtension2[] = ".CLASS";

        int slen = this->d_url.getElement(i).length();

        if (slen > 6  &&
	    (std::equal(javaExtension1, javaExtension1 + 6, 
			this->d_url.getElement(i).end() - 6) ||
	     std::equal(javaExtension2, javaExtension2 + 6, 
			this->d_url.getElement(i).end() - 6)))
	{
	  Doc2 *relative = 0;
	  if ( browser() ) {
	    relative = browser()->urlDoc();
	  }
	  Doc2 doc(this->d_url.getElement(i), relative);
	  if ( doc.localName() ) {
	    return new ScriptJDK(this, doc.urlBase(), doc.localPath() );
	  }
	}
#endif
    }

    return 0;
}

namespace {

# ifdef OPENVRML_HAVE_JS
    namespace JavaScript_ {

        const long MAX_HEAP_BYTES = 4L * 1024L * 1024L;
        const long STACK_CHUNK_BYTES = 4024L;

        void initVrmlClasses(JSContext * cx, JSObject * obj);
        JSBool floatsToJSArray(size_t numFloats, const float * floats,
                               JSContext * cx, jsval * rval);
        VrmlField * createVrmlFieldFromJsval(JSContext * cx, jsval val,
				             VrmlField::VrmlFieldType fieldType);
        char * objToEventOut(JSContext * cx, JSObject * obj);
        void checkEventOut(JSContext * cx, JSObject * obj,
                           const VrmlField & val);

        namespace Global {
            JSClass jsclass = {
                "global", 0,
                JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
            };
            JSBool print(JSContext *, JSObject *, uintN, jsval *, jsval *);

        }

        namespace Browser {
            JSClass jsclass = {
                "Browser", 0,
                JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
                JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub
            };
            JSBool getName(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
            JSBool getVersion(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval) throw ();
            JSBool getCurrentSpeed(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval)
                    throw ();
            JSBool getCurrentFrameRate(JSContext * cx, JSObject * obj,
                                       uintN argc, jsval * argv, jsval * rval)
                    throw ();
            JSBool getWorldURL(JSContext * cx, JSObject * obj,
                               uintN argc, jsval * argv, jsval * rval) throw ();
            JSBool replaceWorld(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval)
                    throw ();
            JSBool createVrmlFromString(JSContext * cx, JSObject * obj,
                                        uintN argc, jsval * argv, jsval * rval)
                    throw ();
            JSBool createVrmlFromURL(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv, jsval * rval)
                    throw ();
            JSBool addRoute(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval) throw ();
            JSBool deleteRoute(JSContext * cx, JSObject * obj,
                               uintN argc, jsval * argv, jsval * rval) throw ();
            JSBool loadURL(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
            JSBool setDescription(JSContext * cx, JSObject * obj,
                                  uintN argc, jsval * argv, jsval * rval)
                    throw ();
        }

        //
        // A base class for a field value "holder" that gets owned by the
        // JavaScript objects corresponding to VRML data values. The holder
        // includes essential metadata: the corresponding interface ID for
        // the object, if any, and whether or not the object corresponds to
        // an eventOut.
        //
        class FieldData {
        public:
            const std::string * scriptFieldId;
            bool isEventOut;
        
            virtual ~FieldData() = 0;
            
            bool isScriptField() const;
            
        protected:
            FieldData(const std::string * scriptFieldId, bool isEventOut);
        };
        
        class SField {
        public:
            /**
             * The SFData class takes ownership of the VrmlField value and
             * deletes it upon destruction.
             */
            class SFData : public FieldData {
                VrmlField * const fieldValue;
                
            public:
                explicit SFData(VrmlField * fieldValue,
                                const std::string * scriptFieldId = 0,
                                bool isEventOut = false);
                virtual ~SFData();
                
                VrmlField & getFieldValue() const;
            
            private:
                // non-copyable
                SFData(const SFData &);
                SFData & operator=(const SFData &);
            };
            
        protected:
            static void finalize(JSContext * cx, JSObject * obj) throw ();
            static JSBool toString(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval)
                    throw ();
        private:
            SField();
        };

        class SFColor : public SField {
        public:
            static JSClass jsclass;
            
            static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
            static JSBool toJsval(const VrmlSFColor & sfcolor, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlSFColor * createFromJSObject(JSContext * cx,
                                                    JSObject * obj);
            
        private:
            static JSBool construct(JSContext * cx, JSObject * obj,
                                    uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv) throw ();
            static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                      jsval * vp) throw ();
            static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                      jsval * vp) throw ();
            static JSBool setHSV(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool getHSV(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval * rval)
                    throw ();
        };

        class SFImage : public SField {
        public:
            static JSClass jsclass;
        
            static JSObject * initClass(JSContext * cx, JSObject * obj)
                    throw ();
            static JSBool toJsval(const VrmlSFImage & sfcolor, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlSFImage * createFromJSObject(JSContext * cx,
                                                    JSObject * obj);
            
        private:
            static JSBool construct(JSContext * cx, JSObject * obj,
	                            uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv) throw ();
            static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                      jsval * vp) throw ();
            static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                      jsval * vp) throw ();
        };
        
        class SFNode : public SField {
        public:
            static JSClass jsclass;
        
            static JSObject * initClass(JSContext * cx, JSObject * obj)
                    throw ();
            static JSBool toJsval(const VrmlSFNode & sfnode, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlSFNode * createFromJSObject(JSContext * cx,
                                                   JSObject * obj);
            static JSBool construct(JSContext * cx, JSObject * obj,
                                    uintN argc, jsval * argv, jsval *) throw ();
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv) throw ();
            static JSBool getProperty(JSContext * cx, JSObject * obj,
                                      jsval id, jsval * vp) throw ();
            static JSBool setProperty(JSContext * cx, JSObject * obj,
                                      jsval id, jsval * vp) throw ();
        };

        class SFRotation : public SField {
        public:
            static JSClass jsclass;
        
            static JSObject * initClass(JSContext * cx, JSObject * obj)
                    throw ();
            static JSBool toJsval(const VrmlSFRotation & sfnode, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlSFRotation * createFromJSObject(JSContext * cx,
                                                       JSObject * obj);
        
        private:
            static JSBool construct(JSContext * cx, JSObject * obj,
	                            uintN argc, jsval * argv, jsval *) throw ();
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv) throw ();
            static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                      jsval * vp) throw ();
            static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                      jsval * vp) throw ();
            static JSBool getAxis(JSContext * cx, JSObject * obj,
                                  uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool inverse(JSContext * cx, JSObject * obj,
                                  uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool multiply(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool multVec(JSContext * cx, JSObject * obj,
                                  uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool setAxis(JSContext * cx, JSObject * obj,
                                  uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool slerp(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval)
                    throw ();
        };

        class SFVec2f : public SField {
        public:
            static JSClass jsclass;
        
            static JSObject * initClass(JSContext * cx, JSObject * obj)
                    throw ();
            static JSBool toJsval(const VrmlSFVec2f & sfvec2f, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlSFVec2f * createFromJSObject(JSContext * cx,
                                                    JSObject * obj);
        
        private:
            static JSBool constructor(JSContext * cx, JSObject * obj,
	                              uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv) throw ();
            static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                     jsval * rval) throw ();
            static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                      jsval * vp) throw ();
            static JSBool add(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval) throw ();
            static JSBool divide(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool dot(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval) throw ();
            static JSBool length(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool multiply(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool negate(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool normalize(JSContext * cx, JSObject * obj,
                                    uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool subtract(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval)
                    throw ();
        };
        
        class SFVec3f : public SField {
        public:
            static JSClass jsclass;
            
            static JSObject * initClass(JSContext * cx, JSObject * obj)
                    throw ();
            static JSBool toJsval(const VrmlSFVec3f & sfvec3f, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlSFVec3f * createFromJSObject(JSContext * cx,
                                                    JSObject * obj);
        
        private:
            static JSBool constructor(JSContext * cx, JSObject * obj,
                                      uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv) throw ();
            static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                                      jsval * vp) throw ();
            static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                                      jsval * vp) throw ();
            static JSBool add(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval) throw ();
            static JSBool cross(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool divide(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool dot(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval) throw ();
            static JSBool length(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool multiply(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool negate(JSContext * cx, JSObject * obj,
                                 uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool normalize(JSContext * cx, JSObject * obj,
                                    uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static JSBool subtract(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval)
                    throw ();
        };

        class MField {
        public:
            typedef std::vector<jsval> JsvalArray;
            
            class MFData : public FieldData {
            public:
                JsvalArray array;

                MFData(JsvalArray::size_type size,
                       const std::string * scriptFieldId = 0, bool isEventOut = false);
                virtual ~MFData();

            private:
                // Not copyable.
                MFData(const MFData &);
                MFData & operator=(const MFData &);
            };
            
        protected:
            static JSBool getElement(JSContext * cx, JSObject * obj,
                                     jsval id, jsval * vp) throw ();
            static JSBool getLength(JSContext * cx, JSObject * obj,
                                    jsval id, jsval * vp) throw ();

        private:
            MField();
        };

        /**
         * @class Base class template for SFColor, SFNode, SFRotation,
         *      SFVec2f, SFVec3f.
         */
        template <typename Subclass>
            class MFJSObject : public MField {
            public:
                static JSObject * initClass(JSContext * cx, JSObject * obj)
                        throw ();
            
            protected:
                static JSBool construct(JSContext * cx, JSObject * obj,
	                                uintN argc, jsval * argv, jsval * vp)
                        throw ();
                static JSBool setElement(JSContext * cx, JSObject * obj,
                                         jsval id, jsval * vp) throw ();
                static JSBool setLength(JSContext * cx, JSObject * obj,
                                        jsval id, jsval * vp) throw ();
                static JSBool toString(JSContext * cx, JSObject * obj,
                                       uintN argc, jsval * argv, jsval * rval)
                        throw ();
                static void finalize(JSContext * cx, JSObject * obj) throw ();
            
            private:
                static JSBool initObject(JSContext * cx, JSObject * obj,
                                         uintN argc, jsval * argv) throw ();
            };

        /**
         * @class Base class template for MFFloat, MFTime.
         *
         * In JavaScript, all floating point values are treated as doubles.
         */
        template <typename Subclass>
            class MFJSDouble : public MField {
            public:
                static JSObject * initClass(JSContext * cx, JSObject * obj)
                        throw ();
            
            protected:
                static JSBool construct(JSContext * cx, JSObject * obj,
	                                uintN argc, jsval * argv, jsval * vp)
                        throw ();
                static JSBool setElement(JSContext * cx, JSObject * obj,
                                         jsval id, jsval * vp) throw ();
                static JSBool setLength(JSContext * cx, JSObject * obj,
                                        jsval id, jsval * vp) throw ();
                static JSBool toString(JSContext * cx, JSObject * obj,
                                       uintN argc, jsval * argv, jsval * rval)
                        throw ();
                static void finalize(JSContext * cx, JSObject * obj) throw ();
            
            private:
                static JSBool initObject(JSContext * cx, JSObject * obj,
                                         uintN argc, jsval * argv) throw ();
            };

        class MFColor : public MFJSObject<MFColor> {
        public:
            static JSClass jsclass;
            static JSClass & sfjsclass;
            
            static JSBool toJsval(const VrmlMFColor & mfcolor, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlMFColor * createFromJSObject(JSContext * cx,
                                                    JSObject * obj);
        };
        
        class MFFloat : public MFJSDouble<MFFloat> {
        public:
            static JSClass jsclass;
            
            static JSBool toJsval(const VrmlMFFloat & mffloat, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlMFFloat * createFromJSObject(JSContext * cx,
                                                    JSObject * obj);
        };
        
        class MFInt32 : public MField {
        public:
            static JSClass jsclass;
            
            static JSObject * initClass(JSContext * cx, JSObject * obj);
            static JSBool toJsval(const VrmlMFInt32 & mfint32, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlMFInt32 * createFromJSObject(JSContext * cx,
                                                    JSObject * obj);

        private:
            static JSBool construct(JSContext * cx, JSObject * obj,
	                            uintN argc, jsval * argv,
                                    jsval * vp);
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv);
            static JSBool setElement(JSContext * cx, JSObject * obj,
                                     jsval id, jsval * vp);
            static JSBool setLength(JSContext * cx, JSObject * obj,
                                    jsval id, jsval * vp);
            static JSBool toString(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval);
            static void finalize(JSContext * cx, JSObject * obj);
        };
        
        //
        // We can't use the MFJSObject<> template to implement MFNode, since
        // the SFNode constructor requires an argument.
        //
        class MFNode : public MField {
        public:
            static JSClass jsclass;
            
            static JSObject * initClass(JSContext * cx, JSObject * obj)
                    throw ();
            static JSBool toJsval(const VrmlMFNode & mfnode, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlMFNode * createFromJSObject(JSContext * cx,
                                                     JSObject * obj);

        private:
            static JSBool construct(JSContext * cx, JSObject * obj,
	                            uintN argc, jsval * argv,
                                    jsval * vp) throw ();
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv) throw ();
            static JSBool setElement(JSContext * cx, JSObject * obj,
                                     jsval id, jsval * vp) throw ();
            static JSBool setLength(JSContext * cx, JSObject * obj,
                                    jsval id, jsval * vp) throw ();
            static JSBool toString(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static void finalize(JSContext * cx, JSObject * obj) throw ();
        };
        
        class MFRotation : public MFJSObject<MFRotation> {
        public:
            static JSClass jsclass;
            static JSClass & sfjsclass;
            
            static JSBool toJsval(const VrmlMFRotation & mfrotation, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlMFRotation * createFromJSObject(JSContext * cx,
                                                       JSObject * obj);
        };
        
        class MFString : public MField {
        public:
            static JSClass jsclass;
            
            static JSObject * initClass(JSContext * cx, JSObject * obj)
                    throw ();
            static JSBool toJsval(const VrmlMFString & mfstring, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlMFString * createFromJSObject(JSContext * cx,
                                                     JSObject * obj);

        private:
            static JSBool construct(JSContext * cx, JSObject * obj,
	                            uintN argc, jsval * argv,
                                    jsval * vp) throw ();
            static JSBool initObject(JSContext * cx, JSObject * obj,
                                     uintN argc, jsval * argv) throw ();
            static JSBool setElement(JSContext * cx, JSObject * obj,
                                     jsval id, jsval * vp) throw ();
            static JSBool setLength(JSContext * cx, JSObject * obj,
                                    jsval id, jsval * vp) throw ();
            static JSBool toString(JSContext * cx, JSObject * obj,
                                   uintN argc, jsval * argv, jsval * rval)
                    throw ();
            static void finalize(JSContext * cx, JSObject * obj) throw ();
        };
        
        class MFTime : public MFJSDouble<MFTime> {
        public:
            static JSClass jsclass;
            
            static JSBool toJsval(const VrmlMFTime & mftime, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlMFTime * createFromJSObject(JSContext * cx,
                                                   JSObject * obj);
        };
        
        class MFVec2f : public MFJSObject<MFVec2f> {
        public:
            static JSClass jsclass;
            static JSClass & sfjsclass;
            
            static JSBool toJsval(const VrmlMFVec2f & mfvec2f, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlMFVec2f * createFromJSObject(JSContext * cx,
                                                    JSObject * obj);
        };
        
        class MFVec3f : public MFJSObject<MFVec3f> {
        public:
            static JSClass jsclass;
            static JSClass & sfjsclass;
            
            static JSBool toJsval(const VrmlMFVec3f & mfvec3f, bool protect,
                                  JSContext * cx, JSObject * obj, jsval * rval)
                    throw ();
            static VrmlMFVec3f * createFromJSObject(JSContext * cx,
                                                    JSObject * obj);
        };

        JSRuntime * Script::rt = 0; // Javascript runtime singleton object
        size_t Script::nInstances = 0; // Number of distinct script objects
 
        JSBool eventOut_setProperty(JSContext * cx, JSObject * obj,
                                    jsval id, jsval * val) throw ();

        void ErrorReporter(JSContext *, const char *, JSErrorReport *);


        // Construct from inline script

        Script::Script(VrmlNodeScript & scriptNode, const std::string & source)
                throw (std::bad_alloc):
                ScriptObject(scriptNode), cx(0) {

            //
            // Initialize the runtime.
            //
            if (!rt) {
                if (!(rt = JS_NewRuntime(MAX_HEAP_BYTES))) {
                    throw std::bad_alloc();
                }
            }

            //
            // Initialize the context for this Script object.
            //
            if (!(this->cx = JS_NewContext(rt, STACK_CHUNK_BYTES))) {
                throw std::bad_alloc();
            }

            //
            // Store a pointer to this Script object in the context.
            //
            JS_SetContextPrivate(this->cx, this);

            JS_SetErrorReporter(cx, ErrorReporter);

            // Define the global objects (builtins, Browser, SF*, MF*) ...
            JSObject * const globalObj =
                    JS_NewObject(this->cx, &Global::jsclass, 0, 0);
            if (!globalObj) {
                throw std::bad_alloc();
            }
            
            if (!JS_InitStandardClasses(this->cx, globalObj)) {
                throw std::bad_alloc();
            }

            static JSFunctionSpec globalFunctions[] = {
                { "print", Global::print, 0 },
                { 0, 0, 0 }
            };

            JS_DefineFunctions( cx, globalObj, globalFunctions );

            // VRML-like TRUE, FALSE syntax
            if (! JS_DefineProperty( cx, globalObj, "FALSE",
			             BOOLEAN_TO_JSVAL(false), 0, 0,
			             JSPROP_READONLY | JSPROP_PERMANENT ))
	      theSystem->error("JS_DefineProp FALSE failed\n");
            if (! JS_DefineProperty( cx, globalObj, "TRUE",
			             BOOLEAN_TO_JSVAL(true), 0, 0,
			             JSPROP_READONLY | JSPROP_PERMANENT ))
	      theSystem->error("JS_DefineProp TRUE failed\n");

            // Browser object
            if (!this->defineBrowserObject()) {
                throw std::bad_alloc();
            }
                
            //
            // Define SF*/MF* classes.
            //
            if (!initVrmlClasses()) {
                throw std::bad_alloc();
            }

            // Define field/eventOut vars for this script
            defineFields();

            /* These should indicate source location for diagnostics. */
            char *filename = 0;
            uintN lineno = 0;

            jsval rval;
            if (!JS_EvaluateScript(cx, globalObj,
                                   source.c_str(), source.length(),
                                   filename, lineno, &rval))
	      theSystem->error("JS_EvaluateScript failed\n");
            
            ++nInstances;
        }

        Script::~Script() {
            JS_DestroyContext(this->cx);
            if (--nInstances == 0) {
                JS_DestroyRuntime(rt);
                rt = 0;
            }
        }

        static double s_timeStamp;	// go away...

        /**
         * @brief Run a specified script.
         *
         * @todo Decide what to do if a call to a JavaScript function fails
         *      (probably due to an error in the script).
         */
        void Script::activate(const double timeStamp, const std::string & fname,
                              const size_t argc,
                              const VrmlField * const argv[]) {
            assert(this->cx);

            jsval fval, rval;
            JSObject * const globalObj = JS_GetGlobalObject(this->cx);
            assert(globalObj);
            
            try {
                if (!JS_LookupProperty(cx, globalObj, fname.c_str(), &fval)) {
                    throw std::bad_alloc();
                }

                //
                // The function may not be defined, in which case we should do nothing.
                //
                if (JSVAL_IS_VOID(fval)) {
                    return;
                }

                jsval * const jsargv = new jsval[argc];

                d_timeStamp = timeStamp;
                s_timeStamp = timeStamp;	// this won't work for long...

                // convert VrmlField*'s to (gc-protected) jsvals
                size_t i;
                for (i = 0; i < argc; ++i) {
                    jsargv[i] = argv[i] ? vrmlFieldToJSVal(*argv[i], true) : JSVAL_NULL;
                }

                JSBool ok;
                ok = JS_CallFunctionValue(cx, globalObj, fval, argc, jsargv, &rval);
                //
                // What should we do at this point if a function call fails? For now,
                // just print a message for a debug build.
                //
# ifndef NDEBUG
                if (!ok) {
                    cerr << "Call to " << fname << " in Script node "
                         << this->scriptNode.getId() << " failed." << endl;
                }
# endif

                // Free up args
                for (i = 0; i < argc; ++i) {
                    if (JSVAL_IS_GCTHING(jsargv[i])) {
                        ok = JS_RemoveRoot(cx, JSVAL_TO_GCTHING(jsargv[i]));
                        assert(ok);
                    }
                }

                delete [] jsargv;

            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
            } catch (...) {
                assert(false);
            }
        }

        VrmlNodeScript & Script::getScriptNode() {
            return this->scriptNode;
        }

        // Convert a VrmlField value to a jsval, optionally protect from gc.

        jsval Script::vrmlFieldToJSVal(const VrmlField & f, const bool protect) {

            jsval rval;
            JSObject * const globalObj = JS_GetGlobalObject(this->cx);
            assert(globalObj);

            switch (f.fieldType()) {
            case VrmlField::SFBOOL:
	        rval = BOOLEAN_TO_JSVAL(static_cast<const VrmlSFBool &>(f).get());
                break;

            case VrmlField::SFCOLOR:
                if (!SFColor::toJsval(static_cast<const VrmlSFColor &>(f), protect,
                                      this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::SFFLOAT:
                {
	            jsdouble * const d =
                            JS_NewDouble(cx, static_cast<const VrmlSFFloat &>(f).get());
	            if (protect) JS_AddRoot( cx, d );
	            rval = DOUBLE_TO_JSVAL( d );
                    break;
                }

            case VrmlField::SFIMAGE:
                if (!SFImage::toJsval(static_cast<const VrmlSFImage &>(f), protect,
                                      this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::SFINT32:
	        return INT_TO_JSVAL(static_cast<const VrmlSFInt32 &>(f).get());

            case VrmlField::SFNODE:
                if (!SFNode::toJsval(static_cast<const VrmlSFNode &>(f), protect,
                                     this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::SFROTATION:
                if (!SFRotation::toJsval(static_cast<const VrmlSFRotation &>(f), protect,
                                         this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::SFSTRING:
                {
	            JSString * s = JS_NewStringCopyZ(cx,
                            static_cast<const VrmlSFString &>(f).get().c_str());
	            if (protect) JS_AddRoot( cx, s );
	            rval = STRING_TO_JSVAL(s);
                    break;
                }

            case VrmlField::SFTIME:
                {
	            jsdouble *d = JS_NewDouble(cx,
                                    static_cast<const VrmlSFTime &>(f).get());
	            if (protect) JS_AddRoot( cx, d );
	            rval = DOUBLE_TO_JSVAL(d);
                    break;
                }

            case VrmlField::SFVEC2F:
                if (!SFVec2f::toJsval(static_cast<const VrmlSFVec2f &>(f), protect,
                                      this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::SFVEC3F:
                if (!SFVec3f::toJsval(static_cast<const VrmlSFVec3f &>(f), protect,
                                      this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::MFCOLOR:
                if (!MFColor::toJsval(static_cast<const VrmlMFColor &>(f), protect,
                                      this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::MFFLOAT:
                if (!MFFloat::toJsval(static_cast<const VrmlMFFloat &>(f), protect,
                                      this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::MFINT32:
                if (!MFInt32::toJsval(static_cast<const VrmlMFInt32 &>(f), protect,
                                      this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::MFNODE:
                if (!MFNode::toJsval(static_cast<const VrmlMFNode &>(f), protect,
                                     this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::MFROTATION:
                if (!MFRotation::toJsval(static_cast<const VrmlMFRotation &>(f), protect,
                                         this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::MFSTRING:
                if (!MFString::toJsval(static_cast<const VrmlMFString &>(f), protect,
                                       this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::MFTIME:
                if (!MFTime::toJsval(static_cast<const VrmlMFTime &>(f), protect,
                                     this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::MFVEC2F:
                if (!MFVec2f::toJsval(static_cast<const VrmlMFVec2f &>(f), protect,
                                      this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            case VrmlField::MFVEC3F:
                if (!MFVec3f::toJsval(static_cast<const VrmlMFVec3f &>(f), protect,
                                      this->cx, globalObj, &rval)) {
                    rval = JSVAL_NULL;
                }
                break;

            default:
                assert(false);
                rval = BOOLEAN_TO_JSVAL(false);
            }

            return rval;
        }


        // Must assign the proper type to eventOuts

        JSBool eventOut_setProperty(JSContext * const cx, JSObject * const obj,
                                    const jsval id, jsval * const val) throw () {
            JSString *str = JS_ValueToString(cx, id);
            if (! str) return JS_FALSE;
            const char *eventName = JS_GetStringBytes(str);

            //
            // The Script object pointer is stored as private data in the
            // context.
            //
            Script * const script =
                    static_cast<Script *>(JS_GetContextPrivate(cx));
            assert(script);

            VrmlNodeScript & scriptNode = script->getScriptNode();

            const VrmlField::VrmlFieldType fieldType =
                    scriptNode.hasEventOut(eventName);
            //
            // If this assertion is false, the we accidentally gave an object
            // that doesn't correspond to an eventOut this setter!
            //
            assert(fieldType != VrmlField::NO_FIELD);

            // Convert to a vrmlField and set the eventOut value
            try {
                std::auto_ptr<VrmlField>
                        fieldValue(createVrmlFieldFromJsval(cx, *val, fieldType));
                scriptNode.setEventOut(eventName, *fieldValue);
            } catch (std::exception & ex) { // should be bad_alloc
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

/* Why is this code here if it's also in             
            // Don't overwrite the property value.
            if (JSVAL_IS_OBJECT(*val) &&
                JSVAL_TO_OBJECT(*val) != 0 &&
                ! JS_DefineProperty( cx, JSVAL_TO_OBJECT(*val), "_eventOut",
			             PRIVATE_TO_JSVAL((long int)eventName),
			             0, 0, JSPROP_READONLY | JSPROP_PERMANENT ))
              theSystem->error("JS_DefineProp _eventOut failed\n");
*/

            return JS_TRUE;
        }
        
        //
        // Initialize SF*/MF* types.
        //
        bool Script::initVrmlClasses() throw () {
            JSObject * const globalObj = JS_GetGlobalObject(this->cx);
            assert(globalObj);
            return (SFColor::initClass(this->cx, globalObj)
                    && SFImage::initClass(this->cx, globalObj)
                    && SFNode::initClass(this->cx, globalObj)
                    && SFRotation::initClass(this->cx, globalObj)
                    && SFVec2f::initClass(this->cx, globalObj)
                    && SFVec3f::initClass(this->cx, globalObj)
                    && MFColor::initClass(this->cx, globalObj)
                    && MFFloat::initClass(this->cx, globalObj)
                    && MFInt32::initClass(this->cx, globalObj)
                    && MFNode::initClass(this->cx, globalObj)
                    && MFRotation::initClass(this->cx, globalObj)
                    && MFString::initClass(this->cx, globalObj)
                    && MFTime::initClass(this->cx, globalObj)
                    && MFVec2f::initClass(this->cx, globalObj)
                    && MFVec3f::initClass(this->cx, globalObj));
        }
        
        //
        // Define the Browser object.
        //
        bool Script::defineBrowserObject() throw () {
            
            JSObject * const globalObj = JS_GetGlobalObject(this->cx);
            assert(globalObj);
            
            static JSFunctionSpec methods[] =
                    { { "getName", Browser::getName, 0 },
                      { "getVersion", Browser::getVersion, 0 },
                      { "getCurrentSpeed", Browser::getCurrentSpeed, 0 },
                      { "getCurrentFrameRate", Browser::getCurrentFrameRate, 0 },
                      { "getWorldURL", Browser::getWorldURL, 0 },
                      { "replaceWorld", Browser::replaceWorld, 1 },
                      { "createVrmlFromString", Browser::createVrmlFromString, 1 },
                      { "createVrmlFromURL", Browser::createVrmlFromURL, 3 },
                      { "addRoute", Browser::addRoute, 4 },
                      { "deleteRoute", Browser::deleteRoute, 4 },
                      { "loadURL", Browser::loadURL, 2 },
                      { "setDescription", Browser::setDescription, 1 },
                      { 0 } };
            
            JSObject * const browserObj =
                    JS_DefineObject(this->cx, globalObj,
                                    "Browser", &Browser::jsclass, 0, 0);
            if (!browserObj) {
                return false;
            }

            if (!JS_DefineFunctions(this->cx, browserObj, methods)) {
                return false;
            }
        }
        
        //
        // Define objects corresponding to fields/eventOuts
        //
        bool Script::defineFields() throw () {
            JSObject * const globalObj = JS_GetGlobalObject(this->cx);
            assert(globalObj);
            
            VrmlNodeScript::FieldList::iterator i;
            for (i = this->scriptNode.fields().begin();
                    i != this->scriptNode.fields().end(); ++i) {
                jsval val = vrmlFieldToJSVal(*(*i)->value, false);
                if (JSVAL_IS_OBJECT(val)) {
                    FieldData * const fieldData =
                            static_cast<FieldData *>
                                (JS_GetPrivate(this->cx, JSVAL_TO_OBJECT(val)));
                    fieldData->scriptFieldId = &(*i)->name;
                }
                if (!JS_DefineProperty(this->cx, globalObj,
                                       (*i)->name.c_str(), val,
                                       0, 0, // getter, setter, ...
                                       JSPROP_PERMANENT)) {
                    return false;
                }
            }

            for (i = this->scriptNode.eventOuts().begin();
                    i != this->scriptNode.eventOuts().end(); ++i) {
                assert((*i)->value);
                jsval val = vrmlFieldToJSVal(*(*i)->value, false);
                if (JSVAL_IS_OBJECT(val)) {
                    FieldData * const fieldData =
                            static_cast<FieldData *>
                                (JS_GetPrivate(this->cx, JSVAL_TO_OBJECT(val)));
                    fieldData->scriptFieldId = &(*i)->name;
                    fieldData->isEventOut = true;
                }
                
                if (!JS_DefineProperty(this->cx, globalObj,
                                       (*i)->name.c_str(), val,
			               0, eventOut_setProperty, //getter, setter
			               JSPROP_PERMANENT)) {
# ifndef NDEBUG
                    cerr << "Attempt to define \"" << (*i)->name
                         << "\" on global object failed." << endl;
# endif
                    return false;
                }
            }
        }


        void
        ErrorReporter(JSContext *, const char *message, JSErrorReport *report)
        {
            int i, j, k, n;

            theSystem->error("javascript: ");
            if (!report) {
	        theSystem->error("%s\n", message);
	        return;
            }

            if (report->filename)
	        theSystem->error("%s, ", report->filename);
            if (report->lineno)
	        theSystem->error("line %u: ", report->lineno);
            theSystem->error(message);
            if (!report->linebuf) {
	        theSystem->error("\n");
	        return;
            }

            theSystem->error(":\n%s\n", report->linebuf);
            n = report->tokenptr - report->linebuf;
            for (i = j = 0; i < n; i++) {
	        if (report->linebuf[i] == '\t') {
	            for (k = (j + 8) & ~7; j < k; j++)
	              theSystem->error(".");
	            continue;
	        }
	        theSystem->error(".");
	        j++;
            }
            theSystem->error("\n");
        }

        JSBool floatsToJSArray(const size_t numFloats, const float * floats,
                               JSContext * const cx, jsval * const rval) {
            static const size_t MAX_FIXED = 20;
            jsval jsfixed[MAX_FIXED];
            jsval * jsvec = (numFloats > MAX_FIXED)
                            ? new jsval[numFloats]
                            : jsfixed;

            size_t i;
            for (i = 0; i < numFloats; ++i) {
                jsdouble * d = JS_NewDouble(cx, floats[i]);
                if (d && JS_AddRoot(cx, d)) {
                    jsvec[i] = DOUBLE_TO_JSVAL(d);
                } else {
                    break;
                }
            }

            if (i == numFloats) {
                JSObject * arr = JS_NewArrayObject(cx, numFloats, jsvec);
                if (arr) {
                    *rval = OBJECT_TO_JSVAL(arr);
                }
            }

            for (size_t j = 0; j < i; ++j) {
                JS_RemoveRoot(cx, JSVAL_TO_GCTHING(jsvec[j]));
            }
            if (numFloats > MAX_FIXED) {
                delete [] jsvec;
            }
            return (i == numFloats) ? JS_TRUE : JS_FALSE;
        }

        /**
         * @brief Convert a jsval to a (new) VrmlField.
         */
        VrmlField * createVrmlFieldFromJsval(JSContext * cx, jsval v,
                                        VrmlField::VrmlFieldType expectType) {
            switch (expectType) {
            case VrmlField::SFBOOL:
                {
                    assert(JSVAL_IS_BOOLEAN(v));
                    return new VrmlSFBool(JSVAL_TO_BOOLEAN(v));
                }
                
            case VrmlField::SFCOLOR:
                assert(JSVAL_IS_OBJECT(v));
                return SFColor::createFromJSObject(cx, JSVAL_TO_OBJECT(v));
            
            case VrmlField::SFFLOAT:
                {
                    assert(JSVAL_IS_NUMBER(v));
                    jsdouble sffloatDouble;
                    JS_ValueToNumber(cx, v, &sffloatDouble);
                    return new VrmlSFFloat(sffloatDouble);
                }
            
            case VrmlField::SFINT32:
                {
                    assert(JSVAL_IS_INT(v));
                    return new VrmlSFInt32(JSVAL_TO_INT(v));
                }
            
            case VrmlField::SFIMAGE:
                assert(JSVAL_IS_OBJECT(v));
                return SFImage::createFromJSObject(cx, JSVAL_TO_OBJECT(v));
            
            case VrmlField::SFNODE:
                assert(JSVAL_IS_OBJECT(v));
                return SFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(v));
            
            case VrmlField::SFROTATION:
                assert(JSVAL_IS_OBJECT(v));
                return SFRotation::createFromJSObject(cx, JSVAL_TO_OBJECT(v));
            
            case VrmlField::SFSTRING:
                {
                    assert(JSVAL_IS_STRING(v));
                    JSString * const jsstring = JSVAL_TO_STRING(v);
                    //
                    // Is the null check here really necessary? Perhaps this
                    // should be an assertion?
                    //
                    return new VrmlSFString(jsstring
                                            ? JS_GetStringBytes(jsstring)
                                            : "");
                }
            
            case VrmlField::SFTIME:
                {
                    assert(JSVAL_IS_NUMBER(v));
                    jsdouble sftimeDouble;
                    JS_ValueToNumber(cx, v, &sftimeDouble);
                    return new VrmlSFTime(sftimeDouble);
                }
            
            case VrmlField::SFVEC2F:
                assert(JSVAL_IS_OBJECT(v));
                return SFVec2f::createFromJSObject(cx, JSVAL_TO_OBJECT(v));
            
            case VrmlField::SFVEC3F:
                assert(JSVAL_IS_OBJECT(v));
                return SFVec3f::createFromJSObject(cx, JSVAL_TO_OBJECT(v));
            
            case VrmlField::MFCOLOR:
                assert(JSVAL_IS_OBJECT(v));
                return MFColor::createFromJSObject(cx, JSVAL_TO_OBJECT(v));
                
            case VrmlField::MFFLOAT:
                assert(JSVAL_IS_OBJECT(v));
                return MFFloat::createFromJSObject(cx, JSVAL_TO_OBJECT(v));
            
            case VrmlField::MFINT32:
                assert(JSVAL_IS_OBJECT(v));
                return MFInt32::createFromJSObject(cx, JSVAL_TO_OBJECT(v));
            
            case VrmlField::MFNODE:
                assert(JSVAL_IS_OBJECT(v));
                return MFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(v));

            case VrmlField::MFROTATION:
                assert(JSVAL_IS_OBJECT(v));
                return MFRotation::createFromJSObject(cx, JSVAL_TO_OBJECT(v));

            case VrmlField::MFSTRING:
                assert(JSVAL_IS_OBJECT(v));
                return MFString::createFromJSObject(cx, JSVAL_TO_OBJECT(v));
            
            case VrmlField::MFTIME:
                assert(JSVAL_IS_OBJECT(v));
                return MFTime::createFromJSObject(cx, JSVAL_TO_OBJECT(v));
            
            case VrmlField::MFVEC2F:
                assert(JSVAL_IS_OBJECT(v));
                return MFVec2f::createFromJSObject(cx, JSVAL_TO_OBJECT(v));

            case VrmlField::MFVEC3F:
                assert(JSVAL_IS_OBJECT(v));
                return MFVec3f::createFromJSObject(cx, JSVAL_TO_OBJECT(v));

            default:
                assert(false);
                return 0;
            }
        }

        // Get a pointer to an eventOut from an object
        char * objToEventOut(JSContext * const cx, JSObject * const obj) {
            jsval val;
            if (JS_LookupProperty(cx, obj, "_eventOut", &val)
                    && JSVAL_IS_INT(val)) {
                return (char*) JSVAL_TO_PRIVATE(val);
            }
            return 0;
        }

        //
        // Check whether we are modifying a prop on an eventOut object, and if so,
        // notify the script.
        //
        void checkEventOut(JSContext * const cx, JSObject * const obj,
                           const VrmlField & val) {
            const FieldData * const fieldData =
                    static_cast<FieldData *>(JS_GetPrivate(cx, obj));
            assert(fieldData);
            
            if (fieldData->isEventOut) {
                assert(fieldData->scriptFieldId);
                Script * const script =
                        static_cast<Script *>(JS_GetContextPrivate(cx));
                assert(script);
                script->getScriptNode().setEventOut(*fieldData->scriptFieldId,
                                                    val);
            }
        }

        namespace Global {

            JSBool print(JSContext * const cx, JSObject *,
                         const uintN argc, jsval * const argv, jsval *) {
                for (uintN i = 0; i < argc; i++) {
                    JSString * const str = JS_ValueToString(cx, argv[i]);
                    if (!str) {
                        return JS_FALSE;
                    }
                    cout << JS_GetStringBytes(str) << "\n";
                }
                cout << flush;
                return JS_TRUE;
            }
        }

        namespace Browser {

            JSBool getName(JSContext * const cx, JSObject *,
		           uintN, jsval *, jsval * const rval) throw () {
                Script * const script =
                        static_cast<Script *>(JS_GetContextPrivate(cx));
                assert(script);
                if (!script->getScriptNode().scene()) {
                    return JS_FALSE;
                }

                const char * const name =
                        script->getScriptNode().scene()->getName();
                *rval = STRING_TO_JSVAL(JS_InternString(cx, name));
                return JS_TRUE;
            }

            JSBool getVersion(JSContext * const cx, JSObject *,
		              uintN, jsval *, jsval * const rval) throw () {
                Script * const script =
                        static_cast<Script *>(JS_GetContextPrivate(cx));
                assert(script);
                if (!script->getScriptNode().scene()) {
                    return JS_FALSE;
                }

                const char * const version =
                        script->getScriptNode().scene()->getVersion();
                *rval = STRING_TO_JSVAL(JS_InternString(cx, version));
                return JS_TRUE;
            }

            JSBool getCurrentSpeed(JSContext * const cx, JSObject *,
                                   uintN, jsval *, jsval * const rval) throw () {
                *rval = DOUBLE_TO_JSVAL(JS_NewDouble( cx, 0.0 )); //...
                return JS_TRUE;
            }

            JSBool getCurrentFrameRate(JSContext * const cx, JSObject *,
                                       uintN, jsval *, jsval * const rval)
                    throw () {
                Script * const script =
                        static_cast<Script *>(JS_GetContextPrivate(cx));
                assert(script);
                if (!script->getScriptNode().scene()) {
                    return JS_FALSE;
                }

                *rval = DOUBLE_TO_JSVAL(JS_NewDouble(cx,
                            script->getScriptNode().scene()->getFrameRate()));
                return JS_TRUE;
            }

            JSBool getWorldURL(JSContext * const cx, JSObject *,
                               uintN, jsval *, jsval * const rval) throw () {
                Script * const script =
                        static_cast<Script *>(JS_GetContextPrivate(cx));
                assert(script);
                if (!script->getScriptNode().scene()) {
                    return JS_FALSE;
                }

                const char * url = 0;
                if (script->getScriptNode().scene()->urlDoc()) {
                    url = script->getScriptNode().scene()->urlDoc()->url();
                }
                if (!url) {
                    url = "";
                }
                *rval = STRING_TO_JSVAL(JS_InternString(cx, url));
                return JS_TRUE;
            }


            // No events will be processed after loadURL.

            JSBool loadURL(JSContext * const cx, JSObject *,
                           const uintN argc, jsval * const argv,
                           jsval * const rval) throw () {
                assert(argc >= 2);
                
                Script * const script =
                        static_cast<Script *>(JS_GetContextPrivate(cx));
                assert(script);
                if (!script->getScriptNode().scene()) {
                    return JS_FALSE;
                }

                //
                // Make sure our first argument (the URL) is an MFString.
                //
                if (!JSVAL_IS_OBJECT(argv[0])
                        || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                          &MFString::jsclass, 0)) {
                    return JS_FALSE;
                }
                
                std::auto_ptr<VrmlMFString>
                        url(MFString::createFromJSObject(cx,
                                                    JSVAL_TO_OBJECT(argv[0])));
                assert(url.get());
                
                //
                // Make sure our second argument is an MFString
                //
                if (!JSVAL_IS_OBJECT(argv[1])
                        || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[1]),
                                          &MFString::jsclass, 0)) {
                    return JS_FALSE;
                }
                
                std::auto_ptr<VrmlMFString>
                        parameters(MFString::createFromJSObject(cx,
                                                    JSVAL_TO_OBJECT(argv[1])));
                assert(parameters.get());
                
                script->getScriptNode().scene()->queueLoadUrl(url.get(),
                                                              parameters.get());
                return JS_TRUE;
            }


            // This does return, but no events will be processed after it is called.

            JSBool replaceWorld(JSContext * const cx, JSObject *,
                                const uintN argc, jsval * const argv,
                                jsval * const rval) throw () {
                assert(argc >= 1);
                
                Script * const script =
                        static_cast<Script *>(JS_GetContextPrivate(cx));
                assert(script);
                if (!script->getScriptNode().scene()) {
                    return JS_FALSE;
                }
                
                //
                // Make sure our argument is an MFNode.
                //
                if (!JSVAL_IS_OBJECT(argv[0])
                        || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                          &MFNode::jsclass, 0)) {
                    return JS_FALSE;
                }
                
                std::auto_ptr<VrmlMFNode>
                        nodes(MFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(argv[0])));
                assert(nodes.get());
                
                VrmlNamespace * ns = new VrmlNamespace(); // should be stored with nodes...
                script->getScriptNode().scene()->queueReplaceNodes(nodes.get(), ns);
                
                *rval = JSVAL_VOID;
                return JS_TRUE;
            }

            JSBool createVrmlFromString(JSContext * const cx,
                                        JSObject * const obj,
				        const uintN argc, jsval * const argv,
                                        jsval * const rval) throw () {
                assert(argc >= 1);
                
                //
                // Make sure our argument is a string.
                //
                if (!JSVAL_IS_STRING(argv[0])) {
                    return JS_FALSE;
                }
                
                JSString * str = JSVAL_TO_STRING(argv[0]);
                assert(str);

                char *vrmlString = JS_GetStringBytes(str);
                VrmlNamespace ns;
                VrmlMFNode kids(VrmlScene::readString(vrmlString, &ns));

                if (kids.getLength() == 0) {
                    *rval = JSVAL_NULL;
                } else {
                    // Put the children from g into an MFNode and return in rval.
                    // should store the namespace as well...
                    if (!MFNode::toJsval(kids, false, cx, obj, rval)) {
                        return JS_FALSE;
                    }
                }
                return JS_TRUE;
            }


            // createVrmlFromURL( MFString url, SFNode node, SFString event )

            JSBool createVrmlFromURL(JSContext * const cx, JSObject *,
				     const uintN argc, jsval * const argv,
                                     jsval * const rval) throw () {
                assert(argc >= 3);

                Script * const script =
                        static_cast<Script *>(JS_GetContextPrivate(cx));
                assert(script);
                if (!script->getScriptNode().scene()) {
                    return JS_FALSE;
                }
                
                Doc2 * relative = script->getScriptNode().scene()->urlDoc();
                
                //
                // Make sure our first argument (the URL) is an MFString.
                //
                if (!JSVAL_IS_OBJECT(argv[0])
                        || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                          &MFString::jsclass, 0)) {
                    return JS_FALSE;
                }
                
                std::auto_ptr<VrmlMFString>
                        url(MFString::createFromJSObject(cx,
                                                    JSVAL_TO_OBJECT(argv[0])));
                assert(url.get());
                
                //
                // Make sure our second arument is an SFNode.
                //
                if (!JSVAL_IS_OBJECT(argv[1])
                        || ! JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[1]),
                                           &SFNode::jsclass, 0)) {
                    return JS_FALSE;
                }
                
                std::auto_ptr<VrmlSFNode>
                        sfnode(SFNode::createFromJSObject(cx,
                                                    JSVAL_TO_OBJECT(argv[1])));
                assert(sfnode.get());
                VrmlNodePtr node(sfnode->get());
                if (!node) {
                    return JS_FALSE;
                }
                
                //
                // Make sure our third argument is a string.
                //
                if (!JSVAL_IS_STRING(argv[2])) {
                    return JS_FALSE;
                }
                
                const char * const eventInId =
                        JS_GetStringBytes(JSVAL_TO_STRING(argv[2]));
                
	        VrmlNamespace ns;	// this is a problem...
	        std::auto_ptr<VrmlMFNode>
                        kids(VrmlScene::readWrl(*url, relative, &ns));
                
                if (!kids.get()) {
                    return JS_FALSE;
                }
                
	        node->eventIn(s_timeStamp, // fix me...
			      eventInId, *kids);
                
                *rval = JSVAL_VOID;
                return JS_TRUE;
            }

            // addRoute(SFNode fromNode, String fromEventOut, SFNode toNode, String toEvent)

            JSBool addRoute(JSContext * const cx, JSObject *,
                            const uintN argc, jsval * const argv,
                            jsval * const rval) throw () {
                assert(argc >= 4);
                
                //
                // Make sure our first argument (fromNode) is a SFNode.
                //
                if (!JSVAL_IS_OBJECT(argv[0])
                        || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                          &SFNode::jsclass, 0)) {
                    return JS_FALSE;
                }
                std::auto_ptr<VrmlSFNode>
                        fromNode(SFNode::createFromJSObject(cx,
                                                    JSVAL_TO_OBJECT(argv[0])));
                
                //
                // Makes sure our second argument (fromEventOut) is a string.
                //
                if (!JSVAL_IS_STRING(argv[1])) {
                    return JS_FALSE;
                }
                const char * const fromEventOut =
                        JS_GetStringBytes(JSVAL_TO_STRING(argv[1]));
                
                //
                // Make sure our third argument (toNode) is a SFNode.
                //
                if (!JSVAL_IS_OBJECT(argv[2])
                        || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[2]),
                                          &SFNode::jsclass, 0)) {
                    return JS_FALSE;
                }
                std::auto_ptr<VrmlSFNode>
                        toNode(SFNode::createFromJSObject(cx,
                                                    JSVAL_TO_OBJECT(argv[2])));
                
                //
                // Makes sure our fourth argument (toEventIn) is a string.
                //
                if (!JSVAL_IS_STRING(argv[3])) {
                    return JS_FALSE;
                }
                const char * const toEventIn =
                        JS_GetStringBytes(JSVAL_TO_STRING(argv[3]));
                
                fromNode->get()->addRoute(fromEventOut,
                                          toNode->get(), toEventIn);
                
                *rval = JSVAL_VOID;
                return JS_TRUE;
            }

            // deleteRoute(SFNode fromNode, String fromEventOut, SFNode toNode, String toEvent)

            JSBool deleteRoute(JSContext * const cx, JSObject *,
                               const uintN argc, jsval * const argv,
                               jsval * const rval) throw () {
                assert(argc >= 4);
                
                //
                // Make sure our first argument (fromNode) is a SFNode.
                //
                if (!JSVAL_IS_OBJECT(argv[0])
                        || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                          &SFNode::jsclass, 0)) {
                    return JS_FALSE;
                }
                std::auto_ptr<VrmlSFNode>
                        fromNode(SFNode::createFromJSObject(cx,
                                                    JSVAL_TO_OBJECT(argv[0])));
                
                //
                // Makes sure our second argument (fromEventOut) is a string.
                //
                if (!JSVAL_IS_STRING(argv[1])) {
                    return JS_FALSE;
                }
                const char * const fromEventOut =
                        JS_GetStringBytes(JSVAL_TO_STRING(argv[1]));
                
                //
                // Make sure our third argument (toNode) is a SFNode.
                //
                if (!JSVAL_IS_OBJECT(argv[2])
                        || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[2]),
                                          &SFNode::jsclass, 0)) {
                    return JS_FALSE;
                }
                std::auto_ptr<VrmlSFNode>
                        toNode(SFNode::createFromJSObject(cx,
                                                    JSVAL_TO_OBJECT(argv[0])));
                
                //
                // Makes sure our fourth argument (toEventIn) is a string.
                //
                if (!JSVAL_IS_STRING(argv[3])) {
                    return JS_FALSE;
                }
                const char * const toEventIn =
                        JS_GetStringBytes(JSVAL_TO_STRING(argv[3]));
                
                fromNode->get()->deleteRoute(fromEventOut,
                                             toNode->get(), toEventIn);
                
                *rval = JSVAL_VOID;
                return JS_TRUE;
            }

            JSBool setDescription(JSContext * const cx, JSObject *,
			          const uintN argc, jsval * const argv,
                                  jsval * const rval) throw () {
                assert(argc >= 1);
                if (!JSVAL_IS_STRING(argv[0])) {
                    return JS_FALSE;
                }
                theSystem->inform("%s",
                                  JS_GetStringBytes(JSVAL_TO_STRING(argv[0])));
                *rval = JSVAL_VOID;
                return JS_TRUE;
            }
        }


        //
        // FieldData
        //
        
        FieldData::FieldData(const std::string * const scriptFieldId,
                             const bool isEventOut):
                scriptFieldId(scriptFieldId), isEventOut(isEventOut) {}
        
        FieldData::~FieldData() {}
        
        bool FieldData::isScriptField() const {
            return this->scriptFieldId;
        }
        
        
        //
        // SField
        //
        
        SField::SFData::SFData(VrmlField * const fieldValue,
                               const std::string * const scriptFieldId,
                               const bool isEventOut):
                FieldData(scriptFieldId, isEventOut), fieldValue(fieldValue) {
            assert(fieldValue);
        }
        
        SField::SFData::~SFData() {
            delete fieldValue;
        }
        
        VrmlField & SField::SFData::getFieldValue() const {
            return *this->fieldValue;
        }
        
        void SField::finalize(JSContext * const cx, JSObject * const obj)
                throw () {
            delete static_cast<FieldData *>(JS_GetPrivate(cx, obj));
            JS_SetPrivate(cx, obj, 0);
        }

        JSBool SField::toString(JSContext * const cx, JSObject * const obj,
                                uintN, jsval *, jsval * const rval) throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);

            ostrstream os;
            os << sfdata->getFieldValue() << '\0';
            const char * ss = os.str();
            JSString * s = JS_NewStringCopyZ(cx, ss);
            os.rdbuf()->freeze(0);
            if (!s) {
                return JS_FALSE;
            }

            *rval = STRING_TO_JSVAL(s);
            return JS_TRUE;
        }

        JSClass SFColor::jsclass =
                { "SFColor", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
                  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };
            
        JSObject * SFColor::initClass(JSContext * const cx,
                                      JSObject * const obj) throw () {
            static JSPropertySpec properties[] =
                    { { "r", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { "g", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { "b", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            static JSFunctionSpec methods[] =
                    // name, func ptr, argc
                    { { "setHSV", setHSV, 3, 0, 0 },
                      { "getHSV", getHSV, 0, 0, 0 },
                      { "toString", toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            return JS_InitClass(cx, obj, 0, &jsclass,
                                construct, 0, // constructor function, min arg count
                                properties, methods,
                                0, 0); // static properties and methods
        }

        JSBool SFColor::toJsval(const VrmlSFColor & sfcolor, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            JSObject * const sfcolorObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!sfcolorObj) {
                return JS_FALSE;
            }
            if (protect) {
                if (!JS_AddRoot(cx, sfcolorObj)) {
                    return JS_FALSE;
                }
            }

            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, sfcolorObj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFColor *>(&sfdata->getFieldValue()));
            
            //
            // Assignment should not throw.
            //
            static_cast<VrmlSFColor &>(sfdata->getFieldValue()) = sfcolor;

            *rval = OBJECT_TO_JSVAL(sfcolorObj);
            return JS_TRUE;
        }

        VrmlSFColor * SFColor::createFromJSObject(JSContext * const cx,
                                                  JSObject * const obj) {
            assert(JS_InstanceOf(cx, obj, &SFColor::jsclass, 0));
            const SFData * const sfdata =
                static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFColor *>(&sfdata->getFieldValue()));
            return static_cast<VrmlSFColor *>(sfdata->getFieldValue().clone());
        }
        
        JSBool SFColor::construct(JSContext * const cx, JSObject * obj,
	                          const uintN argc, jsval * const argv,
                                  jsval * const rval) throw () {
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }

        JSBool SFColor::initObject(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv)
                throw () {
            jsdouble rgb[] = { 0.0, 0.0, 0.0 };
            for (uintN i = 0; i < ((argc < 3) ? argc : 3); ++i) {
                if (!JSVAL_IS_NUMBER(argv[i])
                        || !JS_ValueToNumber(cx, argv[i], rgb + i)) {
                    return JS_FALSE;
                }
            }

            try {
                std::auto_ptr<VrmlSFColor>
                        sfcolor(new VrmlSFColor(rgb[0], rgb[1], rgb[2]));
                std::auto_ptr<SFData> sfdata(new SFData(sfcolor.get()));
                sfcolor.release();
                if (!JS_SetPrivate(cx, obj, sfdata.get())) {
                    return JS_FALSE;
                }
                sfdata.release();
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }
        
        JSBool SFColor::getProperty(JSContext * const cx, JSObject * const obj,
                                    const jsval id, jsval * const rval)
                throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFColor *>(&sfdata->getFieldValue()));
            const VrmlSFColor & thisColor
                    (static_cast<VrmlSFColor &>(sfdata->getFieldValue()));

            if (JSVAL_IS_INT(id)) {
                if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) {
                    return JS_FALSE;
                }
                if (!JS_NewDoubleValue(cx, thisColor[JSVAL_TO_INT(id)], rval)) {
                    return JS_FALSE;
                }
            }

            return JS_TRUE;
        }

        JSBool SFColor::setProperty(JSContext * const cx, JSObject * const obj,
                                    const jsval id, jsval * const vp) throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFColor *>(&sfdata->getFieldValue()));
            VrmlSFColor & thisColor
                    (static_cast<VrmlSFColor &>(sfdata->getFieldValue()));

            if (!JSVAL_IS_INT(id)
                    || JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) {
                return JS_FALSE;
            }

            jsdouble d;
            if (!JS_ValueToNumber(cx, *vp, &d)) {
                return JS_FALSE;
            }

            thisColor[JSVAL_TO_INT(id)] = d;
            checkEventOut(cx, obj, thisColor);

            return JS_TRUE;
        }

        JSBool SFColor::setHSV(JSContext * const cx, JSObject * const obj,
                               const uintN argc, jsval * const argv,
                               jsval * const rval) throw () {
            assert(argc >= 3);
            
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFColor *>(&sfdata->getFieldValue()));
            VrmlSFColor & thisColor
                    (static_cast<VrmlSFColor &>(sfdata->getFieldValue()));
            
            jsdouble h, s, v;
            
            if (!JSVAL_IS_NUMBER(argv[0])
                    || !JS_ValueToNumber(cx, argv[0], &h)
                    || !JSVAL_IS_NUMBER(argv[1])
                    || !JS_ValueToNumber(cx, argv[1], &s)
                    || !JSVAL_IS_NUMBER(argv[2])
                    || !JS_ValueToNumber(cx, argv[2], &s)) {
                return JS_FALSE;
            }
            
            thisColor.setHSV(h, s, v);
            checkEventOut(cx, obj, thisColor);
            *rval = JSVAL_VOID;
            return JS_TRUE;
        }

        JSBool SFColor::getHSV(JSContext * const cx, JSObject * const obj,
                               uintN, jsval *, jsval * rval) throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFColor *>(&sfdata->getFieldValue()));
            VrmlSFColor & thisColor
                    (static_cast<VrmlSFColor &>(sfdata->getFieldValue()));
            
            float hsv[3];
            
            thisColor.getHSV(hsv); // XXX throws? nah...
            
            return floatsToJSArray(3, hsv, cx, rval);
        }

        
        JSClass SFImage::jsclass =
                { "SFImage", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
                  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };
        
        JSObject * SFImage::initClass(JSContext * const cx,
                                      JSObject * const obj) throw () {
            //
            // The properties are read-only for now; this can be made smarter
            // once the underlying OpenVRML SFImage type uses an integer array
            // to represent the pixel data (rather than a char array).
            //
            static JSPropertySpec properties[] =
                    { { "x", 0, JSPROP_PERMANENT | JSPROP_READONLY, 0, 0 },
                      { "y", 1, JSPROP_PERMANENT | JSPROP_READONLY, 0, 0 },
                      { "comp", 2, JSPROP_PERMANENT | JSPROP_READONLY, 0, 0 },
                      { "array", 3, JSPROP_PERMANENT |JSPROP_READONLY, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            static JSFunctionSpec methods[] =
                    // name, func ptr, argc
                    { { "toString", toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            return JS_InitClass(cx, obj, 0, &jsclass,
                                construct, 4, // constructor function, min arg count
                                properties, methods,
                                0, 0); // static properties and methods
        }

        JSBool SFImage::toJsval(const VrmlSFImage & sfimage, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            //
            // Can't call JS_ConstructObject() here since SFImage's ctor
            // requires 4 arguments.
            //
	    JSObject * const sfimageObj =
                    JS_NewObject(cx, &jsclass, 0, obj);
	    if (!sfimageObj) {
                return JS_FALSE;
            }
	    if (protect) {
                if (!JS_AddRoot(cx, sfimageObj)) {
                    return JS_FALSE;
                }
            }

            try {
                std::auto_ptr<VrmlSFImage>
                        sfimageClone(static_cast<VrmlSFImage *>(sfimage.clone()));
                std::auto_ptr<SFData> sfdata(new SFData(sfimageClone.get()));
                sfimageClone.release();
                if (!JS_SetPrivate(cx, sfimageObj, sfdata.get())) {
                    return JS_FALSE;
                }
                sfdata.release();
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            *rval = OBJECT_TO_JSVAL(sfimageObj);
            return JS_TRUE;
        }
        
        VrmlSFImage * SFImage::createFromJSObject(JSContext * const cx,
                                                  JSObject * const obj) {
            assert(JS_InstanceOf(cx, obj, &SFImage::jsclass, 0));
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFImage *>(&sfdata->getFieldValue()));
            return static_cast<VrmlSFImage *>(sfdata->getFieldValue().clone());
        }
        
        JSBool SFImage::construct(JSContext * const cx, JSObject * obj,
	                          const uintN argc, jsval * const argv,
                                  jsval * const rval) throw () {
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }

        //
        // This is ugly because the spec requires an MFInt array for the pixel data
        // and because giving write access to the image size parameters can cause 
        // the library code to crash unless they are validated somehow...
        //
        JSBool SFImage::initObject(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv)
                throw () {
            assert(argc >= 4);
            
            jsdouble d;
            
            //
            // x dimension.
            //
            if (!JSVAL_IS_NUMBER(argv[0])
                    || !JS_ValueToNumber(cx, argv[0], &d)) {
                return JS_FALSE;
            }
            const long x = static_cast<long>(d);
            
            //
            // y dimension.
            //
            if (!JSVAL_IS_NUMBER(argv[1])
                    || !JS_ValueToNumber(cx, argv[1], &d)) {
                return JS_FALSE;
            }
            const long y = static_cast<long>(d);
            
            //
            // components
            //
            if (!JSVAL_IS_NUMBER(argv[2])
                    || !JS_ValueToNumber(cx, argv[2], &d)) {
                return JS_FALSE;
            }
            const long comp = static_cast<long>(d);
            
            //
            // pixel data array
            //
            if (!JSVAL_IS_OBJECT(argv[3])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[3]),
                                      &MFInt32::jsclass, 0)) {
                return JS_FALSE;
            }
            
            MField::MFData * const mfdata =
                    static_cast<MField::MFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[3])));
            
            if (mfdata->array.size() != (x * y)) {
                return JS_FALSE;
            }
            
            try {
                typedef std::vector<unsigned char> Pixels;
                
                Pixels pixels(x * y * comp);
                
                Pixels::iterator pixelPtr = pixels.begin();
                for (MField::JsvalArray::size_type i(0);
                        i < mfdata->array.size(); ++i, pixelPtr += comp) {
                    assert(JSVAL_IS_INT(mfdata->array[i]));
	            static const long byteMask[] =
                            { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
                    int32 pixel;
                    if (!JS_ValueToInt32(cx, mfdata->array[i], &pixel)) {
                        return JS_FALSE;
                    }
                    for (size_t j(0); j < comp; ++j) {
                        *(pixelPtr + j) = (pixel & byteMask[j]) >> (8 * j);
                    }
                }

                std::auto_ptr<VrmlSFImage>
                        sfimage(new VrmlSFImage(x, y, comp, &pixels[0]));
                std::auto_ptr<SFData> sfdata(new SFData(sfimage.get()));
                sfimage.release();
                if (!JS_SetPrivate(cx, obj, sfdata.get())) {
                    return JS_FALSE;
                }
                sfdata.release();
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                return JS_FALSE;
            }
            
            return JS_TRUE;
        }            

        /**
         * @brief JavaScript SFImage getter
         *
         * @todo Return the pixel array as an MFInt32.
         */
        JSBool SFImage::getProperty(JSContext * const cx, JSObject * const obj,
                                    const jsval id, jsval * const vp) throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFImage *>(&sfdata->getFieldValue()));
            const VrmlSFImage & thisImage =
                        (static_cast<VrmlSFImage &>(sfdata->getFieldValue()));

            if (JSVAL_IS_INT(id)) {
                switch (JSVAL_TO_INT(id)) {
                    case 0:
                        *vp = INT_TO_JSVAL(thisImage.getWidth());
                        break;
                    case 1:
                        *vp = INT_TO_JSVAL(thisImage.getHeight());
                        break;
                    case 2:
                        *vp = INT_TO_JSVAL(thisImage.getComponents());
                        break;
                    case 3: // *vp = convert pixels to MFInt...
                    default: return JS_FALSE;
                }
            }

            return JS_TRUE;
        }


        /**
         * @brief JavaScript SFImage getter
         *
         * @todo Implement me!
         */
        JSBool SFImage::setProperty(JSContext *, JSObject *, jsval , jsval *)
                throw () {
          // ...
          return JS_FALSE;
        }

        
        JSClass SFNode::jsclass =
                { "SFNode", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub,  JS_PropertyStub, getProperty, setProperty,
                  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };
        
        JSObject * SFNode::initClass(JSContext * const cx, JSObject * const obj)
                throw () {
            static JSFunctionSpec methods[] =
                    { { "toString", SFNode::toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            return JS_InitClass(cx, obj, 0, &jsclass,
                                SFNode::construct, 1, // constructor function, min arg count
                                0, methods, // instance properties, methods
                                0, 0); // static properties and methods
        }

        JSBool SFNode::toJsval(const VrmlSFNode & sfnode, const bool protect,
                               JSContext * const cx, JSObject * const obj,
                               jsval * const rval) throw () {
            //
            // The SFNode constructor requires one arg (a vrmlstring),
            // so we can't use JS_ConstructObject here.
            //
            JSObject * const sfnodeObj =
                    JS_NewObject(cx, &jsclass, 0, obj);
	    if (!sfnodeObj) {
                return JS_FALSE;
            }

	    if (protect) {
                if (!JS_AddRoot(cx, sfnodeObj)) {
                    return JS_FALSE;
                }
            }
            
            try {
                std::auto_ptr<VrmlSFNode>
                        sfnodeClone(static_cast<VrmlSFNode *>(sfnode.clone()));
                std::auto_ptr<SFData> sfdata(new SFData(sfnodeClone.get()));
                sfnodeClone.release();
                if (!JS_SetPrivate(cx, sfnodeObj, sfdata.get())) {
                    return JS_FALSE;
                }
                sfdata.release();
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            *rval = OBJECT_TO_JSVAL(sfnodeObj);
            return JS_TRUE;
        }

        VrmlSFNode * SFNode::createFromJSObject(JSContext * const cx,
                                                JSObject * const obj) {
            assert(JS_InstanceOf(cx, obj, &SFNode::jsclass, 0));
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFNode *>(&sfdata->getFieldValue()));
            return static_cast<VrmlSFNode *>(sfdata->getFieldValue().clone());
        }

        JSBool SFNode::construct(JSContext * const cx, JSObject * obj,
                                 const uintN argc, jsval * const argv,
                                 jsval * rval) throw () {
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }

        JSBool SFNode::initObject(JSContext * const cx, JSObject * const obj,
                                  const uintN argc, jsval * const argv)
                throw () {
            assert(argc >= 1);

            //
            // Make sure our argument is a string.
            //
            if (!JSVAL_IS_STRING(argv[0])) {
                return JS_FALSE;
            }

            JSString * const str = JSVAL_TO_STRING(argv[0]);
            assert(str);

            VrmlNamespace vrmlNamespace;
            const VrmlMFNode nodes = VrmlScene::readString(JS_GetStringBytes(str),
                                                           &vrmlNamespace);
            //
            // Fail if the string does not produce exactly one node.
            //
            if (nodes.getLength() != 1) {
                return JS_FALSE;
            }

            try {
                std::auto_ptr<VrmlSFNode>
                    sfnode(new VrmlSFNode(nodes.getElement(0)));
                std::auto_ptr<SFData> sfdata(new SFData(sfnode.get()));
                sfnode.release();
                if (!JS_SetPrivate(cx, obj, sfdata.get())) {
                    return JS_FALSE;
                }
                sfdata.release();
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        //
        // SFNode getProperty reads eventOut values, setProperty sends eventIns.
        //
        JSBool SFNode::getProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFNode *>(&sfdata->getFieldValue()));
            const VrmlSFNode & thisNode
                    (static_cast<VrmlSFNode &>(sfdata->getFieldValue()));
            
            if (!thisNode.get()) {
                return JS_FALSE;
            }
            
            Script * const script =
                    static_cast<Script *>(JS_GetContextPrivate(cx));
            assert(script);

            JSString * str = 0;
            if (JSVAL_IS_STRING(id) && ((str = JSVAL_TO_STRING(id)))) {
                char * eventOut = JS_GetStringBytes(str);
                const VrmlField * const fieldVal =
                        thisNode.get()->getEventOut(eventOut);

                // convert event out value to jsval...
                if (fieldVal) {
	            *vp = script->vrmlFieldToJSVal(*fieldVal, false);
	        }

                // If JS_FALSE is returned, apparently the parent object is
                // not searched for the property, so stuff like toString()
                // will fail... don't know how to distinguish between a valid
                // prop and an invalid eventOut...
                return JS_TRUE;
            }

            return JS_FALSE;
        }

        JSBool SFNode::setProperty(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFNode *>(&sfdata->getFieldValue()));
            VrmlSFNode & thisNode
                    (static_cast<VrmlSFNode &>(sfdata->getFieldValue()));
            
            VrmlNodePtr nodePtr = thisNode.get();
            
            if (!nodePtr || !JSVAL_IS_STRING(id)) {
                return JS_FALSE;
            }
            
            const char * const eventInId = JS_GetStringBytes(JSVAL_TO_STRING(id));
            if (!eventInId) {
                return JS_FALSE;
            }
            
	    // convert vp to field, send eventIn to node
            VrmlField::VrmlFieldType expectType;
            if ((expectType = nodePtr->type.hasEventIn(eventInId))) {
                VrmlField * const fieldValue =
                        createVrmlFieldFromJsval(cx, *vp, expectType);
                if (!fieldValue) {
                    return JS_FALSE;
                }
                // This should only happen if directOutput is set...

	        // the timestamp should be stored as a global property and
	        // looked up via obj somehow...
	        Script * const script = 
                        static_cast<Script *>(JS_GetContextPrivate(cx));
                assert(script);
	        if (script->getScriptNode().scene()) {
	            script->getScriptNode().scene()
                            ->queueEvent(s_timeStamp, fieldValue, nodePtr,
                                         eventInId);
	        }
            }
            
            checkEventOut(cx, obj, thisNode);
            return JS_TRUE;
        }


        JSClass SFRotation::jsclass =
                { "SFRotation", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
                  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };
        
        JSObject * SFRotation::initClass(JSContext * const cx,
                                         JSObject * const obj) throw () {
            static JSPropertySpec properties[] =
                    { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { "z", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { "angle", 3, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            static JSFunctionSpec methods[] =
                    // name, func ptr, argc
                    { { "getAxis", getAxis, 0, 0, 0 },
                      { "inverse", inverse, 0, 0, 0 },
                      { "multiply", multiply, 1, 0, 0 },
                      { "multVec", multVec, 1, 0, 0 },
                      { "setAxis", setAxis, 1, 0, 0 },
                      { "slerp", slerp, 2, 0, 0 },
                      { "toString", toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            return JS_InitClass(cx, obj, 0, &jsclass,
                                construct, 0, // constructor function, min arg count
                                properties, methods, // instance properties, methods
                                0, 0); // static properties and methods
        }

        JSBool SFRotation::toJsval(const VrmlSFRotation & sfrotation,
                                   const bool protect,
                                   JSContext * const cx, JSObject * const obj,
                                   jsval * const rval) throw () {
	    JSObject * const sfrotationObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
	    if (!sfrotationObj) {
                return JS_FALSE;
            }
	    if (protect) {
                if (!JS_AddRoot(cx, sfrotationObj)) {
                    return JS_FALSE;
                }
            }

            const SFData * const sfdata =
                static_cast<SFData *>(JS_GetPrivate(cx, sfrotationObj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            
            //
            // Assignment should not throw.
            //
            static_cast<VrmlSFRotation &>(sfdata->getFieldValue()) = sfrotation;
            
            *rval = OBJECT_TO_JSVAL(sfrotationObj);
            return JS_TRUE;
        }

        VrmlSFRotation * SFRotation::createFromJSObject(JSContext * const cx,
                                                        JSObject * const obj) {
            assert(JS_InstanceOf(cx, obj, &SFRotation::jsclass, 0));
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            return static_cast<VrmlSFRotation *>
                    (sfdata->getFieldValue().clone());
        }

        JSBool SFRotation::construct(JSContext * const cx, JSObject * obj,
                                     const uintN argc, jsval * const argv,
                                     jsval * const rval) throw () {
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }
        
        JSBool SFRotation::initObject(JSContext * const cx,
                                      JSObject * const obj,
                                      const uintN argc, jsval * const argv)
                throw () {

            jsdouble x = 0.0, y = 1.0, z = 0.0, angle = 0.0;

            if (argc > 0) {
                if (JSVAL_IS_NUMBER(argv[0])) {
	            if (!JS_ValueToNumber(cx, argv[0], &x)) {
                        return JS_FALSE;
                    }
	            if (argc > 1) {
                        if (!JSVAL_IS_NUMBER(argv[1])
                                || !JS_ValueToNumber(cx, argv[1], &y)) {
                            return JS_FALSE;
                        }

	                if (argc > 2) {
                            if (!JSVAL_IS_NUMBER(argv[2])
                                    || !JS_ValueToNumber(cx,
                                                         argv[2], &z)) {
                                return JS_FALSE;
                            }
                            if (argc > 3) {
                                if (!JSVAL_IS_NUMBER(argv[3])
                                        || !JS_ValueToNumber(cx,
                                                    argv[3], &angle)) {
                                    return JS_FALSE;
                                }
                            }
                        }
                    }
                } else if (JSVAL_IS_OBJECT(argv[0])) {
                    if (!JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                       &JavaScript_::SFVec3f::jsclass, 0)) {
                        return JS_FALSE;
                    }
                    const SFData * sfdata =
                            static_cast<SFData *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
                    assert(sfdata);
                    assert(dynamic_cast<VrmlSFVec3f *>
                                (&sfdata->getFieldValue()));
                    const VrmlSFVec3f & argVec1 =
                        (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));

                    if (argc > 1) {
                        if (JSVAL_IS_OBJECT(argv[1])) {
                            if (!JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[1]),
                                           &JavaScript_::SFVec3f::jsclass, 0)) {
                                return JS_FALSE;
                            }
                            sfdata = static_cast<SFData *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
                            assert(sfdata);
                            assert(dynamic_cast<VrmlSFVec3f *>
                                        (&sfdata->getFieldValue()));
                            const VrmlSFVec3f & argVec2 =
                                    (static_cast<VrmlSFVec3f &>
                                        (sfdata->getFieldValue()));
                            
                            VrmlSFVec3f axisVec(argVec1.cross(argVec2));
                            x = axisVec.getX();
                            y = axisVec.getY();
                            z = axisVec.getZ();
                            angle = acos(argVec1.dot(argVec2) /
                                    (argVec1.length() * argVec2.length()));

                        } else if (JSVAL_IS_NUMBER(argv[1])) {
                            if (!JS_ValueToNumber(cx, argv[1], &angle)) {
                                return JS_FALSE;
                            }
                            x = argVec1.getX();
                            y = argVec1.getY();
                            z = argVec1.getZ();

                        } else {
                            return JS_FALSE;
                        }
                    }
                } else {
                    return JS_FALSE;
                }
            }

            try {
                std::auto_ptr<VrmlSFRotation>
                        sfrotation(new VrmlSFRotation(x, y, z, angle));
                std::auto_ptr<SFData> sfdata(new SFData(sfrotation.get()));
                sfrotation.release();
                if (!JS_SetPrivate(cx, obj, sfdata.get())) {
                    return JS_FALSE;
                }
                sfdata.release();
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        JSBool SFRotation::getProperty(JSContext * const cx,
                                       JSObject * const obj,
                                       const jsval id, jsval * const rval)
                throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            const VrmlSFRotation & thisRot
                    (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));

            if (JSVAL_IS_INT(id)) {
                if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 3) {
                    return JS_FALSE;
                }
                if (!JS_NewDoubleValue(cx, thisRot.get()[JSVAL_TO_INT(id)],
                                       rval)) {
                    return JS_FALSE;
                }
            }

            return JS_TRUE;
        }

        JSBool SFRotation::setProperty(JSContext * const cx,
                                       JSObject * const obj,
                                       const jsval id, jsval * const vp)
                throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            VrmlSFRotation & thisRot
                    (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));

            if (!JSVAL_IS_INT(id)) {
                return JS_FALSE;
            }
            
            if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 3) {
                return JS_FALSE;
            }

            jsdouble d;
            if (!JS_ValueToNumber(cx, *vp, &d)) {
                return JS_FALSE;
            }
            
            switch (JSVAL_TO_INT(id)) {
            case 0:
                thisRot.setX(d);
                break;

            case 1:
                thisRot.setY(d);
                break;

            case 2:
                thisRot.setZ(d);
                break;

            case 3:
                thisRot.setAngle(d);
                break;

            default:
                return JS_FALSE;
            }
            
            checkEventOut(cx, obj, thisRot);
            return JS_TRUE;
        }

        JSBool SFRotation::getAxis(JSContext * const cx, JSObject * const obj,
                                   uintN, jsval *, jsval * const rval)
                throw () {
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            const VrmlSFRotation & thisRot
                    (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            JSObject * const robj =
                    JS_ConstructObject(cx, &SFVec3f::jsclass, 0, robj);
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            VrmlSFVec3f & resultVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            try {
                resultVec = thisRot.getAxis();
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                return JS_FALSE;
            }
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFRotation::inverse(JSContext * const cx, JSObject * const obj,
                                   uintN, jsval *, jsval * const rval)
                throw () {
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            const VrmlSFRotation & thisRot
                    (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));
            sfdata = 0;

            JSObject * const robj =
                    JS_ConstructObject(cx, &SFRotation::jsclass, 0,
			               JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }

            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>
                        (&sfdata->getFieldValue()));
            VrmlSFRotation & resultRot
                (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));

            try {
                resultRot = thisRot.inverse();
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }
        
        JSBool SFRotation::multiply(JSContext * const cx, JSObject * const obj,
                                    const uintN argc, jsval * const argv,
                                    jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            const VrmlSFRotation & thisRot
                    (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));
            sfdata = 0;

            //
            // Make sure our argument is an SFRotation.
            //
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &SFRotation::jsclass, 0)) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            const VrmlSFRotation & argRot
                    (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));
            
            //
            // Construct the result object.
            //
            JSObject * robj = JS_ConstructObject(cx, &SFRotation::jsclass, 0,
                                                 JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }

            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>
                        (&sfdata->getFieldValue()));
            VrmlSFRotation & resultRot
                (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));
            
            try {
                resultRot = thisRot.multiply(argRot);
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }
                
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFRotation::multVec(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv,
	                           jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            const VrmlSFRotation & thisRot
                    (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is an SFVec3f.
            //
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &SFVec3f::jsclass, 0)) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & argVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Construct the result object.
            //
            JSObject * robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                                 JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }

            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            VrmlSFVec3f & resultVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            try {
                resultVec = thisRot.multVec(argVec);
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }
                
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFRotation::setAxis(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv,
                                   jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            VrmlSFRotation & thisRot
                    (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is an SFVec3f.
            //
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &SFVec3f::jsclass, 0)) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & argVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            thisRot.setAxis(argVec); // XXX Throws? Nah ...
            
            checkEventOut(cx, obj, thisRot);
            *rval = JSVAL_VOID;
            return JS_TRUE;
        }

        JSBool SFRotation::slerp(JSContext * const cx, JSObject * const obj,
                                 const uintN argc, jsval * const argv,
                                 jsval * const rval) throw () {
            assert(argc >= 2);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            const VrmlSFRotation & thisRot
                    (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our first argument is an SFRotation.
            //
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &SFRotation::jsclass, 0)) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            const VrmlSFRotation & argRot
                    (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our second argument is a number.
            //
            jsdouble factor;
            if (!JSVAL_IS_NUMBER(argv[1])
                    || !JS_ValueToNumber(cx, argv[1], &factor)) {
                return JS_FALSE;
            }
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFRotation::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }

            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFRotation *>(&sfdata->getFieldValue()));
            VrmlSFRotation & resultRot
                    (static_cast<VrmlSFRotation &>(sfdata->getFieldValue()));
            
            try {
                resultRot = thisRot.slerp(argRot, factor);
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }


        JSClass SFVec2f::jsclass =
                { "SFVec2f", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
                  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };
        
        JSObject * SFVec2f::initClass(JSContext * const cx,
                                      JSObject * const obj) throw () {
            static JSPropertySpec properties[] =
                    { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            static JSFunctionSpec methods[] =
              /*    name          native          nargs    */
                    { { "add", add, 1, 0, 0 },
                      { "divide", divide, 1, 0, 0 },
                      { "dot", dot, 1, 0, 0 },
                      { "length", length, 0, 0, 0 },
                      { "multiply", multiply, 1, 0, 0 },
                      { "negate", negate, 0, 0, 0 },
                      { "normalize", normalize, 0, 0, 0 },
                      { "subtract", subtract, 1, 0, 0 },
                      { "toString", toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            return JS_InitClass(cx, obj, 0, &jsclass,
                                constructor, 0, // constructor function, min arg count
                                properties, methods,
                                0, 0); // static properties and methods
        }

        JSBool SFVec2f::toJsval(const VrmlSFVec2f & sfvec2f, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
	    JSObject * const sfvec2fObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
	    if (!sfvec2fObj) {
                return JS_FALSE;
            }
	    if (protect) {
                if (!JS_AddRoot(cx, sfvec2fObj)) {
                    return JS_FALSE;
                }
            }

            const SFData * const sfdata =
                static_cast<SFData *>(JS_GetPrivate(cx, sfvec2fObj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            
            //
            // Assignment should not throw.
            //
            static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()) = sfvec2f;
            
            *rval = OBJECT_TO_JSVAL(sfvec2fObj);
            return JS_TRUE;
        }

        VrmlSFVec2f * SFVec2f::createFromJSObject(JSContext * const cx,
                                                  JSObject * const obj) {
            assert(JS_InstanceOf(cx, obj, &SFVec2f::jsclass, 0));
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            return static_cast<VrmlSFVec2f *>(sfdata->getFieldValue().clone());
        }

        JSBool SFVec2f::constructor(JSContext * const cx, JSObject * obj,
                                    const uintN argc, jsval * const argv,
                                    jsval * const rval) throw () {
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }
        
        JSBool SFVec2f::initObject(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv)
                throw () {
            jsdouble x = 0.0, y = 0.0;
            if (argc > 0) {
                if (!JSVAL_IS_NUMBER(argv[0])
                        || !JS_ValueToNumber(cx, argv[0], &x)) {
                    return JS_FALSE;
                }
            }
            if (argc > 1) {
                if (!JSVAL_IS_NUMBER(argv[1])
                        || !JS_ValueToNumber(cx, argv[1], &y)) {
                    return JS_FALSE;
                }
            }
            
            try {
                std::auto_ptr<VrmlSFVec2f> sfvec2f(new VrmlSFVec2f(x, y));
                std::auto_ptr<SFData> sfdata(new SFData(sfvec2f.get()));
                sfvec2f.release();
                if (!JS_SetPrivate(cx, obj, sfdata.get())) {
                    return JS_FALSE;
                }
                sfdata.release();
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }
            
            return JS_TRUE;
        }

        JSBool SFVec2f::getProperty(JSContext * const cx, JSObject * const obj,
                                    const jsval id, jsval * const rval)
                throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & thisVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));

            if (JSVAL_IS_INT(id)) {
                if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 1) {
                    return JS_FALSE;
                }
                if (!JS_NewDoubleValue(cx, thisVec[JSVAL_TO_INT(id)], rval)) {
                    return JS_FALSE;
                }
            }
            
            return JS_TRUE;
        }

        JSBool SFVec2f::setProperty(JSContext * const cx, JSObject * const obj,
                                    const jsval id, jsval * const vp) throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            VrmlSFVec2f & thisVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            
            if (!JSVAL_IS_INT(id)) {
                return JS_FALSE;
            }
            
            if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 1) {
                return JS_FALSE;
            }

            jsdouble d;
            if (!JS_ValueToNumber(cx, *vp, &d)) {
                return JS_FALSE;
            }

            thisVec[JSVAL_TO_INT(id)] = d;
            checkEventOut(cx, obj, thisVec);
            return JS_TRUE;
        }

        JSBool SFVec2f::add(JSContext * const cx, JSObject * const obj,
                            const uintN argc, jsval * const argv,
                            jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & thisVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is an SFVec2f.
            //
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &SFVec2f::jsclass, 0)) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & argVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }

            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            VrmlSFVec2f & resultVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));

            resultVec = thisVec.add(argVec); // XXX Throws? Nah ...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFVec2f::divide(JSContext * const cx, JSObject * const obj,
                               const uintN argc, jsval * const argv,
                               jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & thisVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is a number.
            //
            jsdouble divisor = 0.0;
            if (!JSVAL_IS_NUMBER(argv[0])
                    || !JS_ValueToNumber(cx, argv[0], &divisor)) {
                return JS_FALSE;
            }
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            
            if (!robj) {
                return JS_FALSE;
            }

            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            VrmlSFVec2f & resultVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));

            resultVec = thisVec.divide(divisor); // XXX throws? nah...

            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFVec2f::dot(JSContext * const cx, JSObject * const obj,
                            const uintN argc, jsval * const argv,
                            jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & thisVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is an SFVec2f.
            //
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &SFVec2f::jsclass, 0)) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & argVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            sfdata = 0;

            *rval = DOUBLE_TO_JSVAL(thisVec.dot(argVec));
            return JS_TRUE;
        }

        JSBool SFVec2f::length(JSContext * const cx, JSObject * const obj,
                               uintN, jsval *, jsval * const rval) throw () {
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & thisVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            
            if (!JS_NewDoubleValue(cx, thisVec.length(), rval)) {
                return JS_FALSE;
            }
            
            return JS_TRUE;
        }

        JSBool SFVec2f::multiply(JSContext * const cx, JSObject * const obj,
                                 const uintN argc, jsval * const argv,
                                 jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & thisVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is a number.
            //
            jsdouble factor = 0.0;
            if (!JSVAL_IS_NUMBER(argv[0])
                    || !JS_ValueToNumber(cx, argv[0], &factor)) {
                return JS_FALSE;
            }
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            VrmlSFVec2f & resultVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            
            resultVec = thisVec.multiply(factor); // XXX throws? nah...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFVec2f::negate(JSContext * const cx, JSObject * const obj,
                               uintN, jsval *, jsval * const rval) throw () {
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & thisVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            VrmlSFVec2f & resultVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            
            resultVec = thisVec.negate(); // XXX Throws? Nah ...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFVec2f::normalize(JSContext * const cx, JSObject * const obj,
                                  uintN, jsval *, jsval * const rval) throw () {
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & thisVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            VrmlSFVec2f & resultVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            
            resultVec = thisVec.normalize(); // XXX Throws? Nah ...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFVec2f::subtract(JSContext * const cx, JSObject * const obj,
                                 const uintN argc, jsval * const argv,
                                 jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & thisVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is an SFVec2f.
            //
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &SFVec2f::jsclass, 0)) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            const VrmlSFVec2f & argVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec2f *>(&sfdata->getFieldValue()));
            VrmlSFVec2f & resultVec
                    (static_cast<VrmlSFVec2f &>(sfdata->getFieldValue()));
            
            resultVec = thisVec.subtract(argVec); // XXX Throws? Nah ...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSClass SFVec3f::jsclass =
                { "SFVec3f", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
                  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };
        
        JSObject * SFVec3f::initClass(JSContext * const cx,
                                      JSObject * const obj) throw () {
            static JSPropertySpec properties[] =
                    { { "x", 0, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { "y", 1, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { "z", 2, JSPROP_ENUMERATE | JSPROP_PERMANENT, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            static JSFunctionSpec methods[] =
              /*    name          native          nargs    */
                    { { "add", add, 1, 0, 0 },
                      { "cross", cross, 1, 0, 0 },
                      { "divide", divide, 1, 0, 0 },
                      { "dot", dot, 1, 0, 0 },
                      { "length", length, 0, 0, 0 },
                      { "multiply", multiply, 1, 0, 0 },
                      { "negate", negate, 0, 0, 0 },
                      { "normalize", normalize, 0, 0, 0 },
                      { "subtract", subtract, 1, 0, 0 },
                      { "toString", toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            return JS_InitClass(cx, obj, 0, &jsclass,
                                constructor, 0, // constructor function, min arg count
                                properties, methods,
                                0, 0); // static properties and methods
        }

        JSBool SFVec3f::toJsval(const VrmlSFVec3f & sfvec3f, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
	    JSObject * const sfvec3fObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
	    if (!sfvec3fObj) {
                return JS_FALSE;
            }
	    if (protect) {
                if (!JS_AddRoot(cx, sfvec3fObj)) {
                    return JS_FALSE;
                }
            }

            const SFData * const sfdata =
                static_cast<SFData *>(JS_GetPrivate(cx, sfvec3fObj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            
            //
            // Assignment should not throw.
            //
            static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()) = sfvec3f;

            *rval = OBJECT_TO_JSVAL(sfvec3fObj);
            return JS_TRUE;
        }

        VrmlSFVec3f * SFVec3f::createFromJSObject(JSContext * const cx,
                                                  JSObject * const obj) {
            assert(JS_InstanceOf(cx, obj, &SFVec3f::jsclass, 0));
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            return static_cast<VrmlSFVec3f *>(sfdata->getFieldValue().clone());
        }

        JSBool SFVec3f::constructor(JSContext * const cx, JSObject * obj,
                                    const uintN argc, jsval * const argv,
                                    jsval * const rval) throw () {
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }

        JSBool SFVec3f::initObject(JSContext * const cx, JSObject * obj,
                                   const uintN argc, jsval * const argv)
                throw () {
            jsdouble vec[] = { 0.0, 0.0, 0.0 };
            for (uintN i(0); i < ((argc < 3) ? argc : 3); ++i) {
                if (!JSVAL_IS_NUMBER(argv[i])) {
                    return JS_FALSE;
                }
                if (!JS_ValueToNumber(cx, argv[0], vec + i)) {
                    return JS_FALSE;
                }
            }
            
            try {
                std::auto_ptr<VrmlSFVec3f>
                        sfvec3f(new VrmlSFVec3f(vec[0], vec[1], vec[2]));
                std::auto_ptr<SFData> sfdata(new SFData(sfvec3f.get()));
                sfvec3f.release();
                if (!JS_SetPrivate(cx, obj, sfdata.get())) {
                    return JS_FALSE;
                }
                sfdata.release();
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }
            
            return JS_TRUE;
        }
        
        JSBool SFVec3f::getProperty(JSContext * const cx, JSObject * const obj,
                                    const jsval id, jsval * const vp) throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & thisVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            if (JSVAL_IS_INT(id)) {
                if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) {
                    return JS_FALSE;
                }
                if (!JS_NewDoubleValue(cx, thisVec[JSVAL_TO_INT(id)], vp)) {
                    return JS_FALSE;
                }
            }
            
            return JS_TRUE;
        }

        JSBool SFVec3f::setProperty(JSContext * const cx, JSObject * const obj,
                                    const jsval id, jsval * const vp) throw () {
            const SFData * const sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            VrmlSFVec3f & thisVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));

            if (!JSVAL_IS_INT(id)) {
                return JS_FALSE;
            }
            if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) {
                return JS_FALSE;
            }

            jsdouble d;
            if (!JS_ValueToNumber(cx, *vp, &d)) {
                return JS_FALSE;
            }
            thisVec[JSVAL_TO_INT(id)] = d;
            checkEventOut(cx, obj, thisVec);
            return JS_TRUE;
        }

        JSBool SFVec3f::add(JSContext * const cx, JSObject * const obj,
                            const uintN argc, jsval * const argv,
                            jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & thisVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is an SFVec2f.
            //
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &SFVec3f::jsclass, 0)) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & argVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            VrmlSFVec3f & resultVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            resultVec = thisVec.add(argVec); // XXX Throws? Nah ...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFVec3f::cross(JSContext * const cx, JSObject * const obj,
                              const uintN argc, jsval * const argv,
                              jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & thisVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is an SFVec3f.
            //
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &SFVec3f::jsclass, 0)) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & argVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            VrmlSFVec3f & resultVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            resultVec = thisVec.cross(argVec); // XXX Throws? Nah ...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFVec3f::divide(JSContext * const cx, JSObject * const obj,
                               const uintN argc, jsval * const argv,
                               jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & thisVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is a number.
            //
            jsdouble divisor = 0.0;
            if (!JSVAL_IS_NUMBER(argv[0])
                    || !JS_ValueToNumber(cx, argv[0], &divisor)) {
                return JS_FALSE;
            }
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            VrmlSFVec3f & resultVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            resultVec = thisVec.divide(divisor); // XXX Throws? Nah ...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFVec3f::dot(JSContext * const cx, JSObject * const obj,
                            const uintN argc, jsval * const argv,
                            jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & thisVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is an SFVec3f.
            //
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &SFVec3f::jsclass, 0)) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>
                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & argVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));

            *rval = DOUBLE_TO_JSVAL(thisVec.dot(argVec));
            return JS_TRUE;
        }

        JSBool SFVec3f::length(JSContext * const cx, JSObject * const obj,
                               uintN, jsval *, jsval * const rval) throw () {
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & thisVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            if (!JS_NewDoubleValue(cx, thisVec.length(), rval)) {
                return JS_FALSE;
            }
            
            return JS_TRUE;
        }

        JSBool SFVec3f::multiply(JSContext * const cx, JSObject * const obj,
                               const uintN argc, jsval * const argv,
                               jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & thisVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is a number.
            //
            jsdouble factor = 0.0;
            if (!JSVAL_IS_NUMBER(argv[0])
                    || !JS_ValueToNumber(cx, argv[0], &factor)) {
                return JS_FALSE;
            }
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            VrmlSFVec3f & resultVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            resultVec = thisVec.multiply(factor); // XXX Throws? Nah ...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFVec3f::negate(JSContext * const cx, JSObject * const obj,
                               uintN, jsval *, jsval * const rval) throw () {
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & thisVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            VrmlSFVec3f & resultVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            resultVec = thisVec.negate(); // Throws? Nah ...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFVec3f::normalize(JSContext * const cx, JSObject * const obj,
                                  uintN, jsval *, jsval * const rval) throw () {
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & thisVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            VrmlSFVec3f & resultVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            resultVec = thisVec.normalize(); // XXX Throws? Nah ...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }

        JSBool SFVec3f::subtract(JSContext * const cx, JSObject * const obj,
                                 const uintN argc, jsval * const argv,
                                 jsval * const rval) throw () {
            assert(argc >= 1);
            const SFData * sfdata =
                    static_cast<SFData *>(JS_GetPrivate(cx, obj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & thisVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Make sure our argument is an SFVec2f.
            //
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &SFVec3f::jsclass, 0)) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>
                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            const VrmlSFVec3f & argVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            sfdata = 0;
            
            //
            // Construct the result object.
            //
            JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                                       JS_GetParent(cx, obj));
            if (!robj) {
                return JS_FALSE;
            }
            
            sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
            assert(sfdata);
            assert(dynamic_cast<VrmlSFVec3f *>(&sfdata->getFieldValue()));
            VrmlSFVec3f & resultVec
                    (static_cast<VrmlSFVec3f &>(sfdata->getFieldValue()));
            
            resultVec = thisVec.subtract(argVec); // XXX Throws? Nah ...
            
            *rval = OBJECT_TO_JSVAL(robj);
            return JS_TRUE;
        }
        
        
        MField::MFData::MFData(JsvalArray::size_type size,
                               const std::string * const scriptFieldId,
                               const bool isEventOut):
                FieldData(scriptFieldId, isEventOut), array(size) {}
        
        MField::MFData::~MFData() {}
        
        JSBool MField::getElement(JSContext * const cx, JSObject * const obj,
                                  const jsval id, jsval * const vp) throw () {
            assert(cx);
            assert(obj);
            assert(vp);
            MFData * const mfdata = static_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            if (JSVAL_IS_INT(id)) {
                if (JSVAL_TO_INT(id) < 0
                        || JSVAL_TO_INT(id) >= mfdata->array.size()) {
                    return JS_FALSE;
                }

                *vp = mfdata->array[JSVAL_TO_INT(id)];
            }

            return JS_TRUE;
        }

        JSBool MField::getLength(JSContext * const cx, JSObject * const obj,
                                 const jsval id, jsval * const vp) throw () {
            assert(cx);
            assert(obj);
            assert(vp);
            const MFData * const mfdata = static_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            *vp = INT_TO_JSVAL(mfdata->array.size());
            return JS_TRUE;
        }

        
        template <typename Subclass>
            JSObject * MFJSObject<Subclass>::initClass(JSContext * const cx,
                                                       JSObject * const obj)
                    throw () {
                assert(cx);
                assert(obj);
                
                static JSPropertySpec properties[] =
                        { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
                          { 0, 0, 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                        { { "toString", toString, 0, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                JSObject * proto = JS_InitClass(cx, obj, 0, &Subclass::jsclass,
                                                construct, 0, // constructor function, min arg count
                                                properties, methods,
                                                0, 0); // static properties and methods
                if (!proto || !initObject(cx, proto, 0, 0)) {
                    return 0;
                }

                return proto;
            }

        template <typename Subclass>
            JSBool MFJSObject<Subclass>::construct(JSContext * const cx,
                                                   JSObject * obj,
	                                           const uintN argc,
                                                   jsval * const argv,
                                                   jsval * const rval) throw () {
                assert(cx);
                assert(obj);
                assert(rval);
                
                //
                // If called without new, replace obj with a new object.
                //
                if (!JS_IsConstructing(cx)) {
                    obj = JS_NewObject(cx, &Subclass::jsclass, 0, 0);
                    if (!obj) {
                        return JS_FALSE;
                    }
                    *rval = OBJECT_TO_JSVAL(obj);
                }
                
                return initObject(cx, obj, argc, argv);
            }
        
        template <typename Subclass>
            JSBool MFJSObject<Subclass>::initObject(JSContext * const cx,
                                                    JSObject * const obj,
                                                    const uintN argc,
                                                    jsval * const argv)
                    throw () {
                assert(cx);
                assert(obj);
                
                try {
                    MFData * const mfdata = new MFData(argc);
                    if (!JS_SetPrivate(cx, obj, mfdata)) {
                        return JS_FALSE;
                    }


                    for (uintN i = 0; i < argc; ++i) {
                        if (!JSVAL_IS_OBJECT(argv[i])
                                || !JS_InstanceOf(cx,
                                                  JSVAL_TO_OBJECT(argv[i]),
                                                  &Subclass::sfjsclass, 0)) {
                            return JS_FALSE;
                        }

                        assert(JSVAL_IS_GCTHING(argv[i]));
                        JSBool ok =
                                JS_AddRoot(cx, JSVAL_TO_GCTHING(argv[i]));
                        assert(ok);

                        mfdata->array[i] = argv[i];
                    }

                } catch (std::exception & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                    return JS_FALSE;
                } catch (...) {
                    assert(false);
                    return JS_FALSE;
                }

                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSObject<Subclass>::setElement(JSContext * const cx,
                                                    JSObject * const obj,
                                                    const jsval id,
                                                    jsval * const vp) throw () {
                assert(cx);
                assert(obj);
                assert(vp);
                
                MFData * const mfdata = static_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                //
                // Make sure new value is of the correct type.
                //
                if (!JSVAL_IS_OBJECT(*vp)
                        || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(*vp),
                                          &Subclass::sfjsclass, 0)) {
                    return JS_FALSE;
                }

                if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) {
                    return JS_FALSE;
                }

                //
                // Grow array if necessary.
                //
                if (JSVAL_TO_INT(id) >= mfdata->array.size()) {
                    jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
                    if (!setLength(cx, obj, 0, &newLength)) {
                        return JS_FALSE;
                    }
                }

                //
                // Protect the new element from garbage collection.
                //
                if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(*vp))) {
                    return JS_FALSE;
                }

                //
                // Allow the old element to be garbage collected.
                //
                JSBool ok = JS_RemoveRoot(cx,
                            JSVAL_TO_GCTHING(mfdata->array[JSVAL_TO_INT(id)]));
                assert(ok);

                //
                // Put the new element in the array.
                //
                mfdata->array[JSVAL_TO_INT(id)] = *vp;

                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSObject<Subclass>::setLength(JSContext * const cx,
                                                   JSObject * const obj,
                                                   const jsval id,
                                                   jsval * const vp) throw () {
                assert(cx);
                assert(obj);
                assert(vp);
                
                MFData * const mfdata = static_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) {
                    return JS_FALSE;
                }

                try {
                    if (JSVAL_TO_INT(*vp) > mfdata->array.size()) {
                        //
                        // Grow the array; create new elements and protect
                        // them from garbage collection.
                        //
                        const JsvalArray::size_type oldSize =
                                mfdata->array.size();
                        mfdata->array.resize(JSVAL_TO_INT(*vp));
                        for (JsvalArray::size_type i = oldSize;
                                i < mfdata->array.size(); ++i) {
                            JSObject * const element =
                                JS_ConstructObject(cx, &Subclass::sfjsclass,
                                                   0, 0);
                            if (!element) {
                                throw std::bad_alloc();
                            }
                            jsval elementVal = OBJECT_TO_JSVAL(element);
                            if (!JS_AddRoot(cx,
                                    JSVAL_TO_GCTHING(elementVal))) {
                                throw std::bad_alloc();
                            }
                            mfdata->array[i] = elementVal;
                        }
                    } else if (JSVAL_TO_INT(*vp) < mfdata->array.size()) {
                        //
                        // Shrink the array; allow the truncated elements to
                        // be garbage collected.
                        //
                        for (JsvalArray::size_type i = JSVAL_TO_INT(*vp);
                                i < mfdata->array.size(); ++i) {
                            JSBool ok = JS_RemoveRoot(cx,
                                JSVAL_TO_GCTHING(mfdata->array[i]));
                            assert(ok);
                        }
                        mfdata->array.resize(JSVAL_TO_INT(*vp));
                    }
                } catch (std::exception & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                    return JS_FALSE;
                } catch (...) {
                    assert(false);
                    return JS_FALSE;
                }

                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSObject<Subclass>::toString(JSContext * const cx,
                                                  JSObject * const obj,
                                                  uintN, jsval *,
                                                  jsval * const rval) throw () {
                assert(cx);
                assert(obj);
                assert(rval);
                
                MFData * const mfdata = static_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
                assert(mfdata);

                ostrstream os;
                os << "[";
                for (JsvalArray::size_type i = 0; i < mfdata->array.size();
                        ++i) {
                    assert(JSVAL_IS_OBJECT(mfdata->array[i]));
                    const SField::SFData * const sfdata =
                            static_cast<SField::SFData *>
                                (JS_GetPrivate(cx,
                                    JSVAL_TO_OBJECT(mfdata->array[i])));
                    assert(sfdata);
                    os << sfdata->getFieldValue();
                    if ((i + 1) < mfdata->array.size()) {
                        os << ", ";
                    }
                }
                os << "]";

                JSString * jsstr = JS_NewStringCopyZ(cx, os.str());
                
                //
                // Return ownership of the buffer to the ostrstream.
                //
                os.rdbuf()->freeze(0);
                
                if (!jsstr) {
                    return JS_FALSE;
                }

                *rval = STRING_TO_JSVAL(jsstr);
                return JS_TRUE;
            }

        template <typename Subclass>
            void MFJSObject<Subclass>::finalize(JSContext * const cx,
                                                JSObject * const obj) throw () {
                assert(cx);
                assert(obj);
                
                MFData * const mfdata = static_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                for (JsvalArray::const_iterator i(mfdata->array.begin());
                        i != mfdata->array.end(); ++i) {
                    JSBool ok = JS_RemoveRoot(cx, JSVAL_TO_GCTHING(*i));
                    assert(ok);
                }
                delete mfdata;
            }


        template <typename Subclass>
            JSObject * MFJSDouble<Subclass>::initClass(JSContext * const cx,
                                                       JSObject * const obj)
                    throw () {
                static JSPropertySpec properties[] =
                        { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
                          { 0, 0, 0, 0, 0 } };

                static JSFunctionSpec methods[] =
                        { { "toString", toString, 0, 0, 0 },
                          { 0, 0, 0, 0, 0 } };

                JSObject * proto = JS_InitClass(cx, obj, 0, &Subclass::jsclass,
                                                construct, 0, // constructor function, min arg count
                                                properties, methods,
                                                0, 0); // static properties and methods
                if (!proto || !initObject(cx, proto, 0, 0)) {
                    return 0;
                }

                return proto;
            }

        template <typename Subclass>
            JSBool MFJSDouble<Subclass>::construct(JSContext * const cx,
                                                   JSObject * obj,
	                                           const uintN argc,
                                                   jsval * const argv,
                                                   jsval * rval) throw () {
                //
                // If called without new, replace obj with a new object.
                //
                if (!JS_IsConstructing(cx)) {
                    obj = JS_NewObject(cx, &Subclass::jsclass, 0, 0);
                    if (!obj) {
                        return JS_FALSE;
                    }
                    *rval = OBJECT_TO_JSVAL(obj);
                }
                
                return initObject(cx, obj, argc, argv);
            }
        
        template <typename Subclass>
            JSBool MFJSDouble<Subclass>::initObject(JSContext * const cx,
                                                    JSObject * const obj,
                                                    const uintN argc,
                                                    jsval * const argv)
                    throw () {
                try {
                    MFData * const mfdata = new MFData(argc);
                    if (!JS_SetPrivate(cx, obj, mfdata)) {
                        return JS_FALSE;
                    }


                    for (uintN i = 0; i < argc; ++i) {
                        if (!JSVAL_IS_NUMBER(argv[i])) {
                            return JS_FALSE;
                        }

                        //
                        // If the value is an int, convert it to a double.
                        //
                        jsval doubleVal;
                        if (JSVAL_IS_INT(argv[i])) {
                            if (!JS_NewDoubleValue(cx, JSVAL_TO_INT(argv[i]),
                                                   &doubleVal)) {
                                return JS_FALSE;
                            }
                        } else {
                            assert(JSVAL_IS_DOUBLE(argv[i]));
                            doubleVal = argv[i];
                        }
                        
                        //
                        // Protect from garbage collection.
                        //
                        assert(JSVAL_IS_GCTHING(doubleVal));
                        JSBool ok =
                                JS_AddRoot(cx, JSVAL_TO_GCTHING(doubleVal));
                        assert(ok);

                        mfdata->array[i] = doubleVal;
                    }

                } catch (std::exception & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                    return JS_FALSE;
                } catch (...) {
                    assert(false);
                    return JS_FALSE;
                }
                
                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSDouble<Subclass>::setElement(JSContext * const cx,
                                                    JSObject * const obj,
                                                    const jsval id,
                                                    jsval * const vp) throw () {
                MFData * const mfdata = static_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                //
                // Make sure new value is a number.
                //
                if (!JSVAL_IS_NUMBER(*vp)) {
                    return JS_FALSE;
                }

                //
                // If the new value is an int, convert it to a double.
                //
                jsval doubleVal;
                if (JSVAL_IS_INT(*vp)) {
                    if (!JS_NewDoubleValue(cx, JSVAL_TO_INT(*vp),
                                           &doubleVal)) {
                        return JS_FALSE;
                    }
                } else {
                    assert(JSVAL_IS_DOUBLE(*vp));
                    doubleVal = *vp;
                }
                
                if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) {
                    return JS_FALSE;
                }

                //
                // Grow array if necessary.
                //
                if (JSVAL_TO_INT(id) >= mfdata->array.size()) {
                    jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
                    if (!setLength(cx, obj, 0, &newLength)) {
                        return JS_FALSE;
                    }
                }

                //
                // Protect the new element from garbage collection.
                //
                if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(doubleVal))) {
                    return JS_FALSE;
                }

                //
                // Allow the old element to be garbage collected.
                //
                JSBool ok = JS_RemoveRoot(cx,
                        JSVAL_TO_GCTHING(mfdata->array[JSVAL_TO_INT(id)]));
                assert(ok);

                //
                // Put the new element in the array.
                //
                mfdata->array[JSVAL_TO_INT(id)] = doubleVal;

                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSDouble<Subclass>::setLength(JSContext * const cx,
                                                   JSObject * const obj,
                                                   const jsval id,
                                                   jsval * const vp) throw () {
                MFData * const mfdata = static_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) {
                    return JS_FALSE;
                }

                try {
                    if (JSVAL_TO_INT(*vp) > mfdata->array.size()) {
                        //
                        // Grow the array; create new elements and protect
                        // them from garbage collection.
                        //
                        const JsvalArray::size_type oldSize =
                                mfdata->array.size();
                        mfdata->array.resize(JSVAL_TO_INT(*vp));
                        for (JsvalArray::size_type i = oldSize;
                                i < mfdata->array.size(); ++i) {
                            jsval doubleVal;
                            if (!JS_NewDoubleValue(cx, 0.0, &doubleVal)) {
                                throw std::bad_alloc();
                            }
                            if (!JS_AddRoot(cx,
                                    JSVAL_TO_GCTHING(doubleVal))) {
                                throw std::bad_alloc();
                            }
                            mfdata->array[i] = doubleVal;
                        }
                    } else if (JSVAL_TO_INT(*vp) < mfdata->array.size()) {
                        //
                        // Shrink the array; allow the truncated elements to
                        // be garbage collected.
                        //
                        for (JsvalArray::size_type i = JSVAL_TO_INT(*vp);
                                i < mfdata->array.size(); ++i) {
                            JSBool ok = JS_RemoveRoot(cx,
                                JSVAL_TO_GCTHING(mfdata->array[i]));
                            assert(ok);
                        }
                        mfdata->array.resize(JSVAL_TO_INT(*vp));
                    }
                } catch (std::exception & ex) {
                    OPENVRML_PRINT_EXCEPTION_(ex);
                    return JS_FALSE;
                }

                return JS_TRUE;
            }

        template <typename Subclass>
            JSBool MFJSDouble<Subclass>::toString(JSContext * const cx,
                                                  JSObject * const obj,
                                                  uintN, jsval *,
                                                  jsval * const rval) throw () {
                MFData * const mfdata = static_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
                assert(mfdata);

                ostrstream os;
                os << "[";
                for (JsvalArray::size_type i = 0; i < mfdata->array.size();
                        ++i) {
                    assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
                    os << *JSVAL_TO_DOUBLE(mfdata->array[i]);
                    if ((i + 1) < mfdata->array.size()) {
                        os << ", ";
                    }
                }
                os << "]";

                JSString * jsstr = JS_NewStringCopyZ(cx, os.str());
                
                //
                // Return ownership of the buffer to the ostrstream.
                //
                os.rdbuf()->freeze(0);
                
                if (!jsstr) {
                    return JS_FALSE;
                }

                *rval = STRING_TO_JSVAL(jsstr);
                return JS_TRUE;
            }

        template <typename Subclass>
            void MFJSDouble<Subclass>::finalize(JSContext * const cx,
                                                JSObject * const obj) throw () {
                MFData * const mfdata = static_cast<MFData *>
                                                (JS_GetPrivate(cx, obj));
                assert(mfdata);

                for (JsvalArray::const_iterator i(mfdata->array.begin());
                        i != mfdata->array.end(); ++i) {
                    JSBool ok = JS_RemoveRoot(cx, JSVAL_TO_GCTHING(*i));
                    assert(ok);
                }
                delete mfdata;
            }

        JSClass MFColor::jsclass =
                { "MFColor", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSClass & MFColor::sfjsclass(SFColor::jsclass);
        
        VrmlMFColor * MFColor::createFromJSObject(JSContext * const cx,
                                                  JSObject * const obj) {
            assert(cx);
            assert(obj);
            assert(JS_InstanceOf(cx, obj, &MFColor::jsclass, 0));
            
            MField::MFData * const mfdata =
                    static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);
            VrmlMFColor * const mfcolor =
                    new VrmlMFColor(mfdata->array.size());
            for (MField::JsvalArray::size_type i = 0;
                    i < mfdata->array.size(); ++i) {
                assert(JSVAL_IS_OBJECT(mfdata->array[i]));
                assert(JS_InstanceOf(cx,
                                     JSVAL_TO_OBJECT(mfdata->array[i]),
                                     &SFColor::jsclass, 0));
                const SField::SFData * const sfdata =
                        static_cast<SField::SFData *>
                            (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
                assert(sfdata);
                
                const VrmlSFColor & sfcolor =
                        static_cast<VrmlSFColor &>(sfdata->getFieldValue());
                mfcolor->setElement(i, sfcolor.get());
            }

            return mfcolor;
        }
        
        JSBool MFColor::toJsval(const VrmlMFColor & mfcolor, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            JSObject * const mfcolorObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfcolorObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = static_cast<MFData *>
                                    (JS_GetPrivate(cx, mfcolorObj));

            mfdata->array.resize(mfcolor.getLength());

            for (size_t i = 0; i < mfcolor.getLength(); ++i) {
                const VrmlSFColor sfcolor(mfcolor.getElement(i));
                if (!SFColor::toJsval(sfcolor, true, cx, obj,
                                      &mfdata->array[i])) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfcolorObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfcolorObj);
            return JS_TRUE;
        }

        JSClass MFFloat::jsclass =
                { "MFFloat", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSBool MFFloat::toJsval(const VrmlMFFloat & mffloat, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            assert(cx);
            assert(obj);
            assert(rval);
            
            JSObject * const mffloatObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mffloatObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = static_cast<MFData *>
                                    (JS_GetPrivate(cx, mffloatObj));

            mfdata->array.resize(mffloat.getLength());

            for (size_t i = 0; i < mffloat.getLength(); ++i) {
                if (!JS_NewDoubleValue(cx, mffloat.getElement(i),
                                       &mfdata->array[i])) {
                    return JS_FALSE;
                }
                if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(mfdata->array[i]))) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mffloatObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mffloatObj);
            return JS_TRUE;
        }

        VrmlMFFloat * MFFloat::createFromJSObject(JSContext * const cx,
                                                  JSObject * const obj) {
            assert(cx);
            assert(obj);
            assert(JS_InstanceOf(cx, obj, &MFFloat::jsclass, 0));
            MField::MFData * const mfdata =
                    static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);
            VrmlMFFloat * mffloat =
                    new VrmlMFFloat(mfdata->array.size());
            for (MField::JsvalArray::size_type i = 0;
                    i < mfdata->array.size(); ++i) {
                assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
                mffloat->setElement(i, *JSVAL_TO_DOUBLE(mfdata->array[i]));
            }

            return mffloat;
        }
        
        JSClass MFInt32::jsclass =
                { "MFFloat", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSObject * MFInt32::initClass(JSContext * const cx,
                                      JSObject * const obj) {
            assert(cx);
            assert(obj);
            
            static JSPropertySpec properties[] =
                    { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
                      { 0, 0, 0, 0, 0 } };

            static JSFunctionSpec methods[] =
                    { { "toString", toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            JSObject * proto = JS_InitClass(cx, obj, 0, &jsclass,
                                            construct, 0, // constructor function, min arg count
                                            properties, methods,
                                            0, 0); // static properties and methods
            if (!proto || !initObject(cx, proto, 0, 0)) {
                return 0;
            }

            return proto;
        }

        JSBool MFInt32::toJsval(const VrmlMFInt32 & mfint32, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            assert(cx);
            assert(obj);
            assert(rval);
            
            JSObject * const mfint32Obj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfint32Obj) {
                return JS_FALSE;
            }

            MFData * const mfdata = static_cast<MFData *>
                                    (JS_GetPrivate(cx, mfint32Obj));

            mfdata->array.resize(mfint32.getLength());

            for (size_t i = 0; i < mfint32.getLength(); ++i) {
                mfdata->array[i] = INT_TO_JSVAL(mfint32.getElement(i));
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfint32Obj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfint32Obj);
            return JS_TRUE;
        }

        VrmlMFInt32 * MFInt32::createFromJSObject(JSContext * const cx,
                                                  JSObject * const obj) {
            assert(cx);
            assert(obj);
            assert(JS_InstanceOf(cx, obj, &MFInt32::jsclass, 0));
            MField::MFData * const mfdata =
                    static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);
            VrmlMFInt32 * const mfint32 = new VrmlMFInt32(mfdata->array.size());
            for (MField::JsvalArray::size_type i = 0;
                    i < mfdata->array.size(); ++i) {
                assert(JSVAL_IS_INT(mfdata->array[i]));
                mfint32->setElement(i, JSVAL_TO_INT(mfdata->array[i]));
            }

            return mfint32;
        }
        
        JSBool MFInt32::construct(JSContext * const cx, JSObject * obj,
	                          const uintN argc, jsval * const argv,
                                  jsval * const rval) {
            assert(cx);
            assert(obj);
            assert(rval);
            
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }

        JSBool MFInt32::initObject(JSContext * const cx, JSObject * const obj,
                                   const uintN argc, jsval * const argv) {
            assert(cx);
            assert(obj);
            
            try {
                MFData * const mfdata =
                        new MFData(argc);
                if (!JS_SetPrivate(cx, obj, mfdata)) {
                    return JS_FALSE;
                }

                for (uintN i = 0; i < argc; ++i) {
                    if (!JSVAL_IS_INT(argv[i])) {
                        return JS_FALSE;
                    }

                    mfdata->array[i] = argv[i];
                }

            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        JSBool MFInt32::setElement(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) {
            assert(cx);
            assert(obj);
            assert(vp);
            
            MFData * const mfdata =
                    static_cast<MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);

            //
            // Make sure new value is an integer.
            //
            if (!JSVAL_IS_INT(*vp)) {
                return JS_FALSE;
            }

            if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) {
                return JS_FALSE;
            }

            //
            // Grow array if necessary.
            //
            if (JSVAL_TO_INT(id) >= mfdata->array.size()) {
                jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
                if (!setLength(cx, obj, 0, &newLength)) {
                    return JS_FALSE;
                }
            }

            //
            // Put the new element in the array.
            //
            mfdata->array[JSVAL_TO_INT(id)] = *vp;

            return JS_TRUE;
        }

        JSBool MFInt32::setLength(JSContext * const cx, JSObject * const obj,
                                  const jsval id, jsval * const vp) {
            assert(cx);
            assert(obj);
            assert(vp);
            
            MFData * const mfdata =
                    static_cast<MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);

            if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) {
                return JS_FALSE;
            }

            try {
                if (JSVAL_TO_INT(*vp) != mfdata->array.size()) {
                    mfdata->array.resize(JSVAL_TO_INT(*vp));
                }
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        JSBool MFInt32::toString(JSContext * const cx, JSObject * const obj,
                                 uintN, jsval *, jsval * const rval) {
            assert(cx);
            assert(obj);
            
            MFData * const mfdata = static_cast<MFData *>
                                        (JS_GetPrivate(cx, obj));
            assert(mfdata);

            ostrstream os;
            os << "[";
            for (JsvalArray::size_type i = 0; i < mfdata->array.size();
                    ++i) {
                os << JSVAL_TO_INT(mfdata->array[i]);
                if ((i + 1) < mfdata->array.size()) {
                    os << ", ";
                }
            }
            os << "]";

            JSString * jsstr = JS_NewStringCopyZ(cx, os.str());

            //
            // Return ownership of the buffer to the ostrstream.
            //
            os.rdbuf()->freeze(0);

            if (!jsstr) {
                return JS_FALSE;
            }

            *rval = STRING_TO_JSVAL(jsstr);
            return JS_TRUE;
        }

        void MFInt32::finalize(JSContext * const cx, JSObject * const obj) {
            delete static_cast<MFData *>(JS_GetPrivate(cx, obj));
        }

        JSClass MFNode::jsclass =
                { "MFNode", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSObject * MFNode::initClass(JSContext * const cx, JSObject * const obj)
                throw () {
            assert(cx);
            assert(obj);
            
            static JSPropertySpec properties[] =
                    { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
                      { 0, 0, 0, 0, 0 } };

            static JSFunctionSpec methods[] =
                    { { "toString", toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            JSObject * proto = JS_InitClass(cx, obj, 0, &jsclass,
                                            construct, 0, // constructor function, min arg count
                                            properties, methods,
                                            0, 0); // static properties and methods
            if (!proto || !initObject(cx, proto, 0, 0)) {
                return 0;
            }

            return proto;
        }
        
        JSBool MFNode::initObject(JSContext * const cx, JSObject * const obj,
                                  const uintN argc, jsval * const argv)
                throw () {
            assert(cx);
            assert(obj);
            
            try {
                MFData * const mfdata = new MFData(argc);
                if (!JS_SetPrivate(cx, obj, mfdata)) {
                    return JS_FALSE;
                }
                
                for (uintN i = 0; i < argc; ++i) {
                    //
                    // Make sure all args are SFNodes.
                    //
                    if (!JSVAL_IS_OBJECT(argv[i])
                            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[i]),
                                              &SFNode::jsclass, 0)) {
                        return JS_FALSE;
                    }

                    mfdata->array[i] = argv[i];
                }

            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }
        
        JSBool MFNode::toJsval(const VrmlMFNode & mfnode, const bool protect,
                               JSContext * const cx, JSObject * const obj,
                               jsval * const rval) throw () {
            JSObject * const mfnodeObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfnodeObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = static_cast<MFData *>
                                    (JS_GetPrivate(cx, mfnodeObj));

            mfdata->array.resize(mfnode.getLength());

            for (size_t i = 0; i < mfnode.getLength(); ++i) {
                const VrmlSFNode sfnode(mfnode.getElement(i));
                if (!SFNode::toJsval(sfnode, true, cx, obj,
                                     &mfdata->array[i])) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfnodeObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfnodeObj);
            return JS_TRUE;
        }

        VrmlMFNode * MFNode::createFromJSObject(JSContext * const cx,
                                                JSObject * const obj) {
            assert(JS_InstanceOf(cx, obj, &MFNode::jsclass, 0));
            MField::MFData * const mfdata =
                    static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);
            VrmlMFNode * const mfnode =
                    new VrmlMFNode(mfdata->array.size());
            for (MField::JsvalArray::size_type i = 0;
                    i < mfdata->array.size(); ++i) {
                assert(JSVAL_IS_OBJECT(mfdata->array[i]));
                assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                                     &SFNode::jsclass, 0));
                const SField::SFData * const sfdata =
                    static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
                assert(sfdata);
                
                const VrmlSFNode & sfnode =
                        static_cast<VrmlSFNode &>(sfdata->getFieldValue());
                mfnode->setElement(i, sfnode.get());
            }

            return mfnode;
        }
        
        JSBool MFNode::construct(JSContext * const cx, JSObject * obj,
                                 const uintN argc, jsval * const argv,
                                 jsval * const rval) throw () {
            assert(cx);
            assert(obj);
            assert(rval);
            
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }
        
        JSBool MFNode::setElement(JSContext * const cx, JSObject * const obj,
                                  const jsval id, jsval * const vp) throw () {
            assert(cx);
            assert(obj);
            assert(vp);
            
            MFData * const mfdata =
                    static_cast<MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);
            
            //
            // Make sure new value is an SFNode.
            //
            if (!JSVAL_IS_OBJECT(*vp)
                    || !JS_InstanceOf(cx, obj, &SFNode::jsclass, 0)) {
                return JS_FALSE;
            }
            
            if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) {
                return JS_FALSE;
            }
            
            //
            // Grow array if necessary.
            //
            if (JSVAL_TO_INT(id) >= mfdata->array.size()) {
                jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
                if (!setLength(cx, obj, 0, &newLength)) {
                    return JS_FALSE;
                }
            }
            
            //
            // Put the new element in the array.
            //
            mfdata->array[JSVAL_TO_INT(id)] = *vp;
            
            return JS_TRUE;
        }
        
        JSBool MFNode::setLength(JSContext * const cx, JSObject * const obj,
                                 const jsval id, jsval * const vp) throw () {
            assert(cx);
            assert(obj);
            assert(vp);

            MFData * const mfdata = static_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) {
                return JS_FALSE;
            }

            try {
                if (JSVAL_TO_INT(*vp) > mfdata->array.size()) {
                    //
                    // Grow the array; create new elements and protect
                    // them from garbage collection.
                    //
                    const JsvalArray::size_type oldSize =
                            mfdata->array.size();
                    mfdata->array.resize(JSVAL_TO_INT(*vp));

                    jsval arg = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "NULL"));

                    for (JsvalArray::size_type i = oldSize;
                            i < mfdata->array.size(); ++i) {
                        JSObject * const element =
                            JS_NewObject(cx, &SFNode::jsclass, 0, 0);
                        if (!element) {
                            return JS_FALSE;
                        }
                        if (!SFNode::initObject(cx, element, 1, &arg)) {
                            return JS_FALSE;
                        }
                        jsval elementVal = OBJECT_TO_JSVAL(element);
                        if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(elementVal))) {
                            return JS_FALSE;
                        }
                        mfdata->array[i] = elementVal;
                    }
                } else if (JSVAL_TO_INT(*vp) < mfdata->array.size()) {
                    //
                    // Shrink the array; allow the truncated elements to
                    // be garbage collected.
                    //
                    for (JsvalArray::size_type i = JSVAL_TO_INT(*vp);
                            i < mfdata->array.size(); ++i) {
                        JSBool ok = JS_RemoveRoot(cx,
                            JSVAL_TO_GCTHING(mfdata->array[i]));
                        assert(ok);
                    }
                    mfdata->array.resize(JSVAL_TO_INT(*vp));
                }
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }
        
        JSBool MFNode::toString(JSContext * const cx, JSObject * const obj,
                                const uintN argc, jsval * const argv,
                                jsval * const rval) throw () {
            assert(cx);
            assert(obj);
            
            MFData * const mfdata = static_cast<MFData *>
                                        (JS_GetPrivate(cx, obj));
            assert(mfdata);

            ostrstream os;
            os << "[ ";
            for (JsvalArray::size_type i = 0; i < mfdata->array.size();
                    ++i) {
                const SField::SFData * const sfdata =
                    static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
                assert(sfdata);
                os << sfdata->getFieldValue();
                if ((i + 1) < mfdata->array.size()) {
                    os << ", ";
                }
            }
            os << " ]";

            JSString * jsstr = JS_NewStringCopyZ(cx, os.str());

            //
            // Return ownership of the buffer to the ostrstream.
            //
            os.rdbuf()->freeze(0);

            if (!jsstr) {
                return JS_FALSE;
            }

            *rval = STRING_TO_JSVAL(jsstr);
            return JS_TRUE;
        }
        
        void MFNode::finalize(JSContext * const cx, JSObject * const obj)
                throw () {
            MFData * const mfdata =
                    static_cast<MFData *>(JS_GetPrivate(cx, obj));
            
            //
            // Allow individual SFNodes to be garbage collected.
            //
            for (JsvalArray::iterator i(mfdata->array.begin());
                    i != mfdata->array.end(); ++i) {
                JS_RemoveRoot(cx, JSVAL_TO_GCTHING(*i));
            }
            
            delete mfdata;
        }
        
        JSClass MFRotation::jsclass =
                { "MFRotation", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSClass & MFRotation::sfjsclass = SFRotation::jsclass;

        JSBool MFRotation::toJsval(const VrmlMFRotation & mfrotation,
                                   const bool protect,
                                   JSContext * const cx, JSObject * const obj,
                                   jsval * const rval) throw () {
            JSObject * const mfrotationObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfrotationObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = static_cast<MFData *>
                                    (JS_GetPrivate(cx, mfrotationObj));

            mfdata->array.resize(mfrotation.getLength());

            for (size_t i = 0; i < mfrotation.getLength(); ++i) {
                const VrmlSFRotation sfrotation(mfrotation.getElement(i));
                if (!SFRotation::toJsval(sfrotation, true, cx, obj,
                                         &mfdata->array[i])) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfrotationObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfrotationObj);
            return JS_TRUE;
        }

        VrmlMFRotation * MFRotation::createFromJSObject(JSContext * const cx,
                                                        JSObject * const obj) {
            assert(JS_InstanceOf(cx, obj, &MFRotation::jsclass, 0));
            MField::MFData * const mfdata =
                    static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);
            VrmlMFRotation * const mfrotation =
                    new VrmlMFRotation(mfdata->array.size());
            for (MField::JsvalArray::size_type i = 0;
                    i < mfdata->array.size(); ++i) {
                assert(JSVAL_IS_OBJECT(mfdata->array[i]));
                assert(JS_InstanceOf(cx,
                                     JSVAL_TO_OBJECT(mfdata->array[i]),
                                     &SFRotation::jsclass, 0));
                const SField::SFData * const sfdata =
                    static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
                assert(sfdata);
                
                const VrmlSFRotation & sfrotation =
                        static_cast<VrmlSFRotation &>(sfdata->getFieldValue());
                
                mfrotation->setElement(i, sfrotation.get());
            }

            return mfrotation;
        }
        
        JSClass MFString::jsclass =
                { "MFString", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSObject * MFString::initClass(JSContext * const cx,
                                       JSObject * const obj) throw () {
            static JSPropertySpec properties[] =
                    { { "length", 0, JSPROP_PERMANENT, getLength, setLength },
                      { 0, 0, 0, 0, 0 } };

            static JSFunctionSpec methods[] =
                    { { "toString", toString, 0, 0, 0 },
                      { 0, 0, 0, 0, 0 } };

            JSObject * proto = JS_InitClass(cx, obj, 0, &jsclass,
                                            construct, 0, // constructor function, min arg count
                                            properties, methods,
                                            0, 0); // static properties and methods
            if (!proto || !initObject(cx, proto, 0, 0)) {
                return 0;
            }

            return proto;
        }

        JSBool MFString::toJsval(const VrmlMFString & mfstring,
                                 const bool protect,
                                 JSContext * const cx, JSObject * const obj,
                                 jsval * const rval) throw () {
            assert(cx);
            assert(obj);
            assert(rval);
            JSObject * const mfstringObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfstringObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = static_cast<MFData *>
                                    (JS_GetPrivate(cx, mfstringObj));

            mfdata->array.resize(mfstring.getLength());

            for (size_t i = 0; i < mfstring.getLength(); ++i) {
                JSString * jsstring =
                        JS_NewStringCopyZ(cx, mfstring.getElement(i).c_str());
                if (!jsstring) {
                    return JS_FALSE;
                }
                if (!JS_AddRoot(cx, jsstring)) {
                    return JS_FALSE;
                }
                mfdata->array[i] = STRING_TO_JSVAL(jsstring);
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfstringObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfstringObj);
            return JS_TRUE;
        }

        VrmlMFString * MFString::createFromJSObject(JSContext * const cx,
                                                    JSObject * const obj) {
            assert(JS_InstanceOf(cx, obj, &MFString::jsclass, 0));
            MField::MFData * const mfdata =
                    static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);
            VrmlMFString * const mfstring =
                    new VrmlMFString(mfdata->array.size());
            for (MField::JsvalArray::size_type i = 0;
                    i < mfdata->array.size(); ++i) {
                assert(JSVAL_IS_STRING(mfdata->array[i]));
                const char * const str =
                        JS_GetStringBytes(
                            JSVAL_TO_STRING(mfdata->array[i]));
                mfstring->setElement(i, str);
            }

            return mfstring;
        }

        JSBool MFString::construct(JSContext * const cx, JSObject * obj,
	                           const uintN argc, jsval * const argv,
                                   jsval * const rval) throw () {
            assert(cx);
            assert(obj);
            assert(rval);
            
            //
            // If called without new, replace obj with a new object.
            //
            if (!JS_IsConstructing(cx)) {
                obj = JS_NewObject(cx, &jsclass, 0, 0);
                if (!obj) {
                    return JS_FALSE;
                }
                *rval = OBJECT_TO_JSVAL(obj);
            }

            return initObject(cx, obj, argc, argv);
        }

        JSBool MFString::initObject(JSContext * const cx, JSObject * const obj,
                                    const uintN argc, jsval * const argv)
                throw () {
            assert(cx);
            assert(obj);
            
            try {
                MFData * const mfdata =
                        new MFData(argc);
                if (!JS_SetPrivate(cx, obj, mfdata)) {
                    return JS_FALSE;
                }

                for (uintN i = 0; i < argc; ++i) {
                    if (!JSVAL_IS_STRING(argv[i])) {
                        return JS_FALSE;
                    }

                    //
                    // Protect from garbage collection.
                    //
                    assert(JSVAL_IS_GCTHING(argv[i]));
                    JSBool ok = JS_AddRoot(cx, JSVAL_TO_GCTHING(argv[i]));
                    assert(ok);

                    mfdata->array[i] = argv[i];
                }

            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        JSBool MFString::setElement(JSContext * const cx, JSObject * const obj,
                                    const jsval id, jsval * const vp) throw () {
            MFData * const mfdata = static_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            //
            // Make sure new value is a string.
            //
            if (!JSVAL_IS_STRING(*vp)) {
                return JS_FALSE;
            }

            if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) {
                return JS_FALSE;
            }

            //
            // Grow array if necessary.
            //
            if (JSVAL_TO_INT(id) >= mfdata->array.size()) {
                jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
                if (!setLength(cx, obj, 0, &newLength)) {
                    return JS_FALSE;
                }
            }

            //
            // Protect the new element from garbage collection.
            //
            if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(*vp))) {
                return JS_FALSE;
            }

            //
            // Allow the old element to be garbage collected.
            //
            JSBool ok = JS_RemoveRoot(cx,
                    JSVAL_TO_GCTHING(mfdata->array[JSVAL_TO_INT(id)]));
            assert(ok);

            //
            // Put the new element in the array.
            //
            mfdata->array[JSVAL_TO_INT(id)] = *vp;

            return JS_TRUE;
        }

        JSBool MFString::setLength(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp) throw () {
            MFData * const mfdata = static_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) {
                return JS_FALSE;
            }

            try {
                if (JSVAL_TO_INT(*vp) > mfdata->array.size()) {
                    //
                    // Grow the array; create new elements and protect
                    // them from garbage collection.
                    //
                    const JsvalArray::size_type oldSize =
                            mfdata->array.size();
                    mfdata->array.resize(JSVAL_TO_INT(*vp));
                    for (JsvalArray::size_type i = oldSize;
                            i < mfdata->array.size(); ++i) {
                        JSString * jsstring = JS_NewStringCopyZ(cx, "");
                        if (!jsstring) {
                            throw std::bad_alloc();
                        }
                        if (!JS_AddRoot(cx, jsstring)) {
                            throw std::bad_alloc();
                        }
                        mfdata->array[i] = STRING_TO_JSVAL(jsstring);
                    }
                } else if (JSVAL_TO_INT(*vp) < mfdata->array.size()) {
                    //
                    // Shrink the array; allow the truncated elements to
                    // be garbage collected.
                    //
                    for (JsvalArray::size_type i = JSVAL_TO_INT(*vp);
                            i < mfdata->array.size(); ++i) {
                        JSBool ok = JS_RemoveRoot(cx,
                            JSVAL_TO_GCTHING(mfdata->array[i]));
                        assert(ok);
                    }
                    mfdata->array.resize(JSVAL_TO_INT(*vp));
                }
            } catch (std::exception & ex) {
                OPENVRML_PRINT_EXCEPTION_(ex);
                return JS_FALSE;
            } catch (...) {
                assert(false);
                return JS_FALSE;
            }

            return JS_TRUE;
        }

        JSBool MFString::toString(JSContext * const cx, JSObject * const obj,
                                  uintN, jsval *, jsval * const rval) throw () {
            MFData * const mfdata = static_cast<MFData *>
                                        (JS_GetPrivate(cx, obj));
            assert(mfdata);

            ostrstream os;
            os << "[";
            for (JsvalArray::size_type i = 0; i < mfdata->array.size();
                    ++i) {
                assert(JSVAL_IS_STRING(mfdata->array[i]));
                os << "\""
                   << JS_GetStringBytes(JSVAL_TO_STRING(mfdata->array[i]))
                   << "\"";
                if ((i + 1) < mfdata->array.size()) {
                    os << ", ";
                }
            }
            os << "]";

            JSString * jsstr = JS_NewStringCopyZ(cx, os.str());

            //
            // Return ownership of the buffer to the ostrstream.
            //
            os.rdbuf()->freeze(0);

            if (!jsstr) {
                return JS_FALSE;
            }

            *rval = STRING_TO_JSVAL(jsstr);
            return JS_TRUE;
        }

        void MFString::finalize(JSContext * const cx, JSObject * const obj)
                throw () {
            MFData * const mfdata = static_cast<MFData *>
                                            (JS_GetPrivate(cx, obj));
            assert(mfdata);

            for (JsvalArray::const_iterator i(mfdata->array.begin());
                    i != mfdata->array.end(); ++i) {
                JSBool ok = JS_RemoveRoot(cx, JSVAL_TO_GCTHING(*i));
                assert(ok);
            }
            delete mfdata;
        }

        JSClass MFTime::jsclass =
                { "MFTime", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSBool MFTime::toJsval(const VrmlMFTime & mftime, const bool protect,
                               JSContext * const cx, JSObject * const obj,
                               jsval * const rval) throw () {
            JSObject * const mftimeObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mftimeObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = static_cast<MFData *>
                                    (JS_GetPrivate(cx, mftimeObj));

            mfdata->array.resize(mftime.getLength());

            for (size_t i = 0; i < mftime.getLength(); ++i) {
                if (!JS_NewDoubleValue(cx, mftime.getElement(i),
                                       &mfdata->array[i])) {
                    return JS_FALSE;
                }
                if (!JS_AddRoot(cx, JSVAL_TO_GCTHING(mfdata->array[i]))) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mftimeObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mftimeObj);
            return JS_TRUE;
        }

        VrmlMFTime * MFTime::createFromJSObject(JSContext * const cx,
                                                JSObject * const obj) {
            assert(JS_InstanceOf(cx, obj, &MFTime::jsclass, 0));
            MField::MFData * const mfdata =
                    static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);
            VrmlMFTime * mftime =
                    new VrmlMFTime(mfdata->array.size());
            for (MField::JsvalArray::size_type i = 0;
                    i < mfdata->array.size(); ++i) {
                assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
                mftime->setElement(i, *JSVAL_TO_DOUBLE(mfdata->array[i]));
            }

            return mftime;
        }
        
        JSClass MFVec2f::jsclass =
                { "MFVec2f", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSClass & MFVec2f::sfjsclass = SFVec2f::jsclass;

        JSBool MFVec2f::toJsval(const VrmlMFVec2f & mfvec2f, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            JSObject * const mfvec2fObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfvec2fObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = static_cast<MFData *>
                                    (JS_GetPrivate(cx, mfvec2fObj));

            mfdata->array.resize(mfvec2f.getLength());

            for (size_t i = 0; i < mfvec2f.getLength(); ++i) {
                const VrmlSFVec2f sfvec2f(mfvec2f.getElement(i));
                if (!SFVec2f::toJsval(sfvec2f, true, cx, obj,
                                      &mfdata->array[i])) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfvec2fObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfvec2fObj);
            return JS_TRUE;
        }

        VrmlMFVec2f * MFVec2f::createFromJSObject(JSContext * const cx,
                                                  JSObject * const obj) {
            assert(JS_InstanceOf(cx, obj, &MFVec2f::jsclass, 0));
            MField::MFData * const mfdata =
                    static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);
            VrmlMFVec2f * mfvec2f =
                    new VrmlMFVec2f(mfdata->array.size());
            for (MField::JsvalArray::size_type i = 0;
                    i < mfdata->array.size(); ++i) {
                assert(JSVAL_IS_OBJECT(mfdata->array[i]));
                assert(JS_InstanceOf(cx,
                                     JSVAL_TO_OBJECT(mfdata->array[i]),
                                     &SFVec2f::jsclass, 0));
                const SField::SFData * const sfdata =
                        static_cast<SField::SFData *>
                            (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
                assert(sfdata);
                
                const VrmlSFVec2f & sfvec2f =
                        static_cast<VrmlSFVec2f &>(sfdata->getFieldValue());
                mfvec2f->setElement(i, sfvec2f.get());
            }

            return mfvec2f;
        }
        
        JSClass MFVec3f::jsclass =
                { "MFVec3f", JSCLASS_HAS_PRIVATE,
                  JS_PropertyStub, JS_PropertyStub,
                  getElement, setElement,  
                  JS_EnumerateStub, JS_ResolveStub,
                  JS_ConvertStub, finalize };
        
        JSClass & MFVec3f::sfjsclass = SFVec3f::jsclass;

        JSBool MFVec3f::toJsval(const VrmlMFVec3f & mfvec3f, const bool protect,
                                JSContext * const cx, JSObject * const obj,
                                jsval * const rval) throw () {
            JSObject * const mfvec3fObj =
                    JS_ConstructObject(cx, &jsclass, 0, obj);
            if (!mfvec3fObj) {
                return JS_FALSE;
            }

            MFData * const mfdata = static_cast<MFData *>
                                    (JS_GetPrivate(cx, mfvec3fObj));

            mfdata->array.resize(mfvec3f.getLength());

            for (size_t i = 0; i < mfvec3f.getLength(); ++i) {
                const VrmlSFVec3f sfvec3f(mfvec3f.getElement(i));
                if (!SFVec3f::toJsval(sfvec3f, true, cx, obj,
                                      &mfdata->array[i])) {
                    return JS_FALSE;
                }
            }

            if (protect) {
                if (!JS_AddRoot(cx, mfvec3fObj)) {
                    return JS_FALSE;
                }
            }

            *rval = OBJECT_TO_JSVAL(mfvec3fObj);
            return JS_TRUE;
        }
        
        VrmlMFVec3f * MFVec3f::createFromJSObject(JSContext * const cx,
                                                  JSObject * const obj) {
            assert(cx);
            assert(obj);
            assert(JS_InstanceOf(cx, obj, &MFVec3f::jsclass, 0));
            MField::MFData * const mfdata =
                    static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
            assert(mfdata);
            VrmlMFVec3f * mfvec3f = new VrmlMFVec3f(mfdata->array.size());
            for (MField::JsvalArray::size_type i = 0;
                    i < mfdata->array.size(); ++i) {
                assert(JSVAL_IS_OBJECT(mfdata->array[i]));
                assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                                     &SFVec3f::jsclass, 0));
                const SField::SFData * const sfdata =
                    static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
                assert(sfdata);
                
                const VrmlSFVec3f & sfvec3f =
                        static_cast<VrmlSFVec3f &>(sfdata->getFieldValue());
                mfvec3f->setElement(i, sfvec3f.get());
            }

            return mfvec3f;
        }
    }
# endif // OPENVRML_HAVE_JS
}
