// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
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

# include <assert.h>
# include "private.h"
# include "script.h"
# include "scope.h"
# include "browser.h"
# include "doc2.hpp"
# include "System.h"
# include "ScriptJDK.h"

namespace OpenVRML {

/**
 * @class Script
 *
 * @brief Abstract class implemented by scripting language bindings.
 *
 * The runtime instantiates subclasses of Script for each VRML97 Script node;
 * and calls its methods appropriately to execute script code.
 */

/**
 * @var ScriptNode & Script::scriptNode
 *
 * @brief a reference to the ScriptNode that uses this Script object.
 */

/**
 * @brief Constructor.
 *
 * @param scriptNode    a reference to the ScriptNode that uses this Script
 *                      object.
 */
Script::Script(ScriptNode & scriptNode): scriptNode(scriptNode) {}

/**
 * @fn Script::Script(const Script &)
 *
 * @brief Non-copyable. Copy constructor declared but not defined.
 */

/**
 * @brief Destructor.
 */
Script::~Script() {}

/**
 * @fn Script & Script::operator=(const Script &)
 *
 * @brief Non-copyable. Assignment operator declared but not defined.
 */

/**
 * @fn void Script::initialize(double timestamp)
 *
 * @brief Initialize the Script node.
 */

/**
 * @fn void Script::processEvent(const std::string & id, const FieldValue & value, double timestamp)
 *
 * @brief Process an event.
 */

/**
 * @fn void Script::eventsProcessed(double timestamp)
 *
 * @brief Execute script code after processing events.
 */

/**
 * @fn void Script::shutdown(double timestamp)
 *
 * @brief Shut down the Script node.
 */


/**
 * @class ScriptNodeClass
 *
 * @brief Class object for @link ScriptNode ScriptNodes@endlink.
 *
 * There is one ScripNodeClass per Browser instance.
 *
 * @see Browser::scriptNodeClass
 */

/**
 * @brief Constructor.
 *
 * @param browser   the Browser to be associated with the ScriptNodeClass.
 */
ScriptNodeClass::ScriptNodeClass(Browser & browser):
    node_class(browser)
{}

/**
 * @brief Destructor.
 */
ScriptNodeClass::~ScriptNodeClass() throw ()
{}

/**
 * @brief Not implemented.
 *
 * This method is not implemented because the Script node implementation,
 * unlike other node implementations, cannot provide the implementation of
 * an @c EXTERNPROTO. It is an error to call this method.
 */
const NodeTypePtr ScriptNodeClass::create_type(const std::string &,
                                               const node_interface_set &)
    throw ()
{
    assert(false);
    return NodeTypePtr(0);
}


/**
 * @class ScriptNode
 *
 * @brief Represents a VRML Script node.
 */

/**
 * @typedef ScriptNode::FieldValueMap
 *
 * @brief A std::map that keys field values on their field name.
 */

/**
 * @typedef ScriptNode::EventOutValueMap
 *
 * @brief A std::map that keys eventOut values on their eventOut name.
 */

/**
 * @class ScriptNode::ScriptNodeType
 *
 * @brief Type objects for @link ScriptNode ScriptNodes@endlink.
 *
 * @see ScriptNode::scriptNodeType
 */

/**
 * @brief Constructor.
 */
ScriptNode::ScriptNodeType::ScriptNodeType(ScriptNodeClass & nodeClass):
    node_type(nodeClass, "Script")
{
    static const node_interface interfaces[] = {
        node_interface(node_interface::exposedfield_id,
                      field_value::mfstring_id,
                      "url"),
        node_interface(node_interface::field_id,
                      field_value::sfbool_id,
                      "directOutput"),
        node_interface(node_interface::field_id,
                      field_value::sfbool_id,
                      "mustEvaluate")
    };
    this->interfaces_.add(interfaces[0]);
    this->interfaces_.add(interfaces[1]);
    this->interfaces_.add(interfaces[2]);
}

/**
 * @brief Destructor.
 */
ScriptNode::ScriptNodeType::~ScriptNodeType() throw ()
{}

/**
 * @brief Add an interface.
 */
void ScriptNode::ScriptNodeType::addInterface(const node_interface & interface)
    throw (std::invalid_argument)
{
    this->interfaces_.add(interface);
}

/**
 * @brief Get the interfaces for the node.
 */
const node_interface_set & ScriptNode::ScriptNodeType::interfaces() const
    throw ()
{
    return this->interfaces_;
}

namespace {
    const field_value_ptr
    defaultFieldValue(const field_value::type_id fieldType)
        throw (std::bad_alloc)
    {
        switch (fieldType) {
        case field_value::sfbool_id:
            return field_value_ptr(new sfbool);
        case field_value::sfcolor_id:
            return field_value_ptr(new sfcolor);
        case field_value::sffloat_id:
            return field_value_ptr(new sffloat);
        case field_value::sfimage_id:
            return field_value_ptr(new sfimage);
        case field_value::sfint32_id:
            return field_value_ptr(new sfint32);
        case field_value::sfnode_id:
            return field_value_ptr(new sfnode);
        case field_value::sfrotation_id:
            return field_value_ptr(new sfrotation);
        case field_value::sfstring_id:
            return field_value_ptr(new sfstring);
        case field_value::sftime_id:
            return field_value_ptr(new sftime);
        case field_value::sfvec2f_id:
            return field_value_ptr(new sfvec2f);
        case field_value::sfvec3f_id:
            return field_value_ptr(new sfvec3f);
        case field_value::mfcolor_id:
            return field_value_ptr(new mfcolor);
        case field_value::mffloat_id:
            return field_value_ptr(new mffloat);
        case field_value::mfint32_id:
            return field_value_ptr(new mfint32);
        case field_value::mfnode_id:
            return field_value_ptr(new mfnode);
        case field_value::mfrotation_id:
            return field_value_ptr(new mfrotation);
        case field_value::mfstring_id:
            return field_value_ptr(new mfstring);
        case field_value::mftime_id:
            return field_value_ptr(new mftime);
        case field_value::mfvec2f_id:
            return field_value_ptr(new mfvec2f);
        case field_value::mfvec3f_id:
            return field_value_ptr(new mfvec3f);
        default:
            assert(false);
        }
        return field_value_ptr(0);
    }
}

/**
 * @brief Clone the Script node that has this NodeType.
 *
 * Since the NodeType for a ScriptNode is only available once a ScriptNode
 * is instantiated, you have to have a ScriptNode instance in order to be able
 * to use this method. The "primordial" ScriptNode instance must be created
 * with a call to the ScriptNode constructor.
 */
const NodePtr
ScriptNode::ScriptNodeType::create_node(const ScopePtr & scope) const
    throw (std::bad_alloc)
{
    ScriptNodeClass & scriptNodeClass =
        static_cast<ScriptNodeClass &>(this->_class);
    const NodePtr node(new ScriptNode(scriptNodeClass, scope));
    ScriptNode & scriptNode = dynamic_cast<ScriptNode &>(*node);

    //
    // Copy the interfaces.
    //
    scriptNode.scriptNodeType.interfaces_ = this->interfaces_;

    //
    // Add the default field and eventOut values.
    //
    for (node_interface_set::const_iterator itr(this->interfaces_.begin());
            itr != this->interfaces_.end(); ++itr) {
        if (itr->type == node_interface::field_id) {
            const FieldValueMap::value_type
                    value(itr->id, defaultFieldValue(itr->field_type));
            const bool succeeded =
                    scriptNode.fieldValueMap.insert(value).second;
            assert(succeeded);
        } else if (itr->type == node_interface::eventout_id) {
            const polled_eventout_value
                    eventOutValue(defaultFieldValue(itr->field_type), false);
            const EventOutValueMap::value_type value(itr->id, eventOutValue);
            const bool succeeded =
                    scriptNode.eventOutValueMap.insert(value).second;
            assert(succeeded);
        }
    }

    return node;
}

/**
 * @var ScriptNode::ScriptNodeType ScriptNode::scriptNodeType
 *
 * @brief Type object for the ScriptNode instance.
 *
 * Script node @link NodeType NodeTypes@endlink are significantly different
 * from other @link NodeType NodeTypes@endlink. While most @link NodeType NodeTypes@endlink
 * are shared by the Node instances they spawn, the ScriptNodeType is unique
 * to a ScriptNode instance, and it shares the ScriptNodeInstance's lifetime.
 * This reflects the fact that Script nodes in VRML get their functionality
 * by the addition of fields, eventIns, and eventOuts on a per-instance basis.
 *
 * For ScriptNode instances, Node::nodeType is an alias for the scriptNodeType
 * object.
 */

/**
 * @var SFBool ScriptNode::directOutput
 *
 * @brief directOutput field.
 */

/**
 * @var SFBool ScriptNode::mustEvaluate
 *
 * @brief mustEvaluate field.
 */

/**
 * @var MFString ScriptNode::url
 *
 * @brief url exposedField.
 */

/**
 * @var ScriptNode::FieldValueMap ScriptNode::fieldValueMap
 *
 * @brief Maps user-defined field names to their values.
 */

/**
 * @var ScriptNode::EventOutValueMap ScriptNode::eventOutValueMap
 *
 * @brief Maps user-defined eventOut names to their values.
 */

/**
 * @var Script * ScriptNode::script
 *
 * @brief A pointer to a Script object.
 */

/**
 * @var int ScriptNode::eventsReceived
 *
 * @brief A count of the number of events received since ScriptNode::update
 *      was called.
 */

/**
 * @brief Constructor.
 */
ScriptNode::ScriptNode(ScriptNodeClass & nodeClass, const ScopePtr & scope):
    node(this->scriptNodeType, scope),
    child_node(this->scriptNodeType, scope),
    scriptNodeType(nodeClass),
    directOutput(false),
    mustEvaluate(false),
    script(0),
    eventsReceived(0)
{}

/**
 * @brief Destructor.
 */
ScriptNode::~ScriptNode() throw ()
{
    delete script;
}

void ScriptNode::setUrl(const mfstring & value, const double timestamp)
{
    delete this->script;
    this->script = 0;
    this->url = value;
    this->do_initialize(timestamp);

    //
    // url is an exposedField.
    //
    this->emit_event("url_changed", this->url, timestamp);
}

const mfstring & ScriptNode::getUrl() const
{
    return this->url;
}

void ScriptNode::addEventIn(const field_value::type_id type,
                            const std::string & id)
    throw (std::invalid_argument, std::bad_alloc)
{
    const node_interface interface(node_interface::eventin_id, type, id);
    this->scriptNodeType.addInterface(interface);
}

void ScriptNode::addEventOut(const field_value::type_id type,
                             const std::string & id)
    throw (std::invalid_argument, std::bad_alloc)
{
    const node_interface interface(node_interface::eventout_id, type, id);
    this->scriptNodeType.addInterface(interface);

    //
    // eventOut value.
    //
    const polled_eventout_value eventOutValue(defaultFieldValue(type), 0.0);
    EventOutValueMap::value_type value(id, eventOutValue);
    const bool succeeded = this->eventOutValueMap.insert(value).second;
    assert(succeeded);
}

void ScriptNode::addField(const std::string & id,
                          const field_value_ptr & defaultVal)
    throw (std::invalid_argument, std::bad_alloc)
{
    const node_interface interface(node_interface::field_id,
                                   defaultVal->type(),
                                   id);
    this->scriptNodeType.addInterface(interface);

    //
    // field value.
    //
    const FieldValueMap::value_type value(id, defaultVal);
    const bool succeeded = this->fieldValueMap.insert(value).second;
    assert(succeeded);
}

/**
 * @brief Return a pointer to this ScriptNode.
 *
 * @return a pointer to this ScriptNode.
 */
const ScriptNode * ScriptNode::to_script() const throw ()
{
    return this;
}

/**
 * @brief Return a pointer to this ScriptNode.
 *
 * @return a pointer to this ScriptNode.
 */
ScriptNode * ScriptNode::to_script() throw ()
{
    return this;
}

void ScriptNode::update(const double currentTime)
{
    if (this->eventsReceived > 0) {
        this->eventsReceived = 0;
        if (this->script) {
            this->script->eventsProcessed(currentTime);
        }

        //
        // For each modified eventOut, send an event.
        //
        for (EventOutValueMap::iterator itr = eventOutValueMap.begin();
                itr != eventOutValueMap.end(); ++itr) {
            if (itr->second.modified) {
                this->emit_event(itr->first, *itr->second.value, currentTime);
                itr->second.modified = false;
            }
        }
    }
}

//
// Script nodes can be self referential! Check this condition,
// and "undo" the refcounting: decrement the refcount on any
// self-references we acquire ownership of, and increment the
// refcount on any self-references for which we relinquish
// ownership.
//

void ScriptNode::assignWithSelfRefCheck(const sfnode & inval,
                                        sfnode & retval) const
    throw ()
{
    const NodePtr & oldNode = retval.value;

    //
    // About to relinquish ownership of a sfnode value. If the
    // sfnode value is this Script node, then we need to
    // *increment* its refcount, since we previously
    // *decremented* it to accommodate creating a cycle between
    // refcounted objects.
    //
    if (oldNode
            && (dynamic_cast<ScriptNode *>(oldNode.countPtr->first) == this)) {
        ++oldNode.countPtr->second;
    }

    retval = inval;

    //
    // Now, check to see if the new sfnode value is a self-
    // reference. If it is, we need to *decrement* the refcount.
    // A self-reference creates a cycle. If a Script node with
    // a self-reference were completely removed from the world,
    // it still wouldn't be deleted (if we didn't do this)
    // because the reference it held to itself would prevent the
    // refcount from ever dropping to zero.
    //
    const NodePtr & newNode = retval.value;
    if (dynamic_cast<ScriptNode *>(newNode.countPtr->first) == this) {
        --(newNode.countPtr->second);
    }
}

void ScriptNode::assignWithSelfRefCheck(const mfnode & inval,
                                        mfnode & retval) const
    throw ()
{
    std::vector<NodePtr>::size_type i;
    for (i = 0; i < retval.value.size(); ++i) {
        const NodePtr & oldNode = retval.value[i];
        if (oldNode
            && (dynamic_cast<ScriptNode *>(oldNode.countPtr->first) == this)) {
            ++oldNode.countPtr->second;
        }
    }

    retval = inval;

    for (i = 0; i < retval.value.size(); ++i) {
        const NodePtr & newNode = retval.value[i];
        if (newNode
            && (dynamic_cast<ScriptNode *>(newNode.countPtr->first) == this)) {
            --(newNode.countPtr->second);
        }
    }
}

/**
 * @brief Initialize.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void ScriptNode::do_initialize(const double timestamp) throw (std::bad_alloc)
{
    assert(this->scene());
    this->scene()->browser.addScript(*this);

    this->eventsReceived = 0;
    this->script = this->createScript();
    if (this->script) { this->script->initialize(timestamp); }

    //
    // For each modified eventOut, send an event.
    //
    for (EventOutValueMap::iterator itr(this->eventOutValueMap.begin());
            itr != this->eventOutValueMap.end(); ++itr) {
        if (itr->second.modified) {
            this->emit_event(itr->first, *itr->second.value, timestamp);
            itr->second.modified = false;
        }
    }
}

/**
 * @brief Set the value of one of the node's fields.
 *
 * @param id    the name of the field to set.
 * @param value the new value.
 *
 * @exception unsupported_interface if the node has no field @p id.
 * @exception std::bad_cast         if @p value is the wrong type.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void ScriptNode::do_field(const std::string & id, const field_value & value)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    FieldValueMap::iterator itr;
    if (id == "url") {
        //
        // Don't call setUrl() here, as that will emit a "url_changed" event.
        //
        this->url = dynamic_cast<const mfstring &>(value);
    } else if (id == "directOutput") {
        this->directOutput = dynamic_cast<const sfbool &>(value);
    } else if (id == "mustEvaluate") {
        this->mustEvaluate = dynamic_cast<const sfbool &>(value);
    } else if (!((itr = this->fieldValueMap.find(id))
            == this->fieldValueMap.end())) {
        if (itr->second->type() == field_value::sfnode_id) {
            this->assignWithSelfRefCheck(dynamic_cast<const sfnode &>(value),
                                         static_cast<sfnode &>(*itr->second));
        } else if (itr->second->type() == field_value::mfnode_id) {
            this->assignWithSelfRefCheck(dynamic_cast<const mfnode &>(value),
                                         static_cast<mfnode &>(*itr->second));
        } else {
            itr->second->assign(value); // Throws std::bad_cast.
        }
    } else {
        throw unsupported_interface("Script node has no field \"" + id
                                    + "\".");
    }
}

/**
 * @brief Get the value of a field.
 *
 * @param id    the name of the field to get.
 *
 * @return the value for field @p id.
 *
 * @exception unsupported_interface  if the node has no field @p id.
 */
const field_value & ScriptNode::do_field(const std::string & id) const
    throw (unsupported_interface)
{
    FieldValueMap::const_iterator itr;
    if (id == "url") {
        return this->url;
    } else if (id == "directOutput") {
        return this->directOutput;
    } else if (id == "mustEvaluate") {
        return this->mustEvaluate;
    } else if ((itr = this->fieldValueMap.find(id))
            != this->fieldValueMap.end()) {
        return *itr->second;
    }
    throw unsupported_interface("Script node has no field \"" + id + "\".");
}

void ScriptNode::do_process_event(const std::string & id,
                                  const field_value & value,
                                  const double timestamp)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    if (!this->type.has_eventin(id)) {
        throw unsupported_interface("Script node has no eventIn \"" + id
                                   + "\".");
    }

    if (!this->script) { return; }

    if (id == "url" || id == "set_url") {
        this->setUrl(dynamic_cast<const mfstring &>(value), timestamp);
    } else {
        //
        // Hand the event off to script code.
        //
        this->script->processEvent(id, value, timestamp);

        //
        // For each modified eventOut, emit an event.
        //
        for (EventOutValueMap::iterator itr(this->eventOutValueMap.begin());
                itr != this->eventOutValueMap.end(); ++itr) {
            if (itr->second.modified) {
                this->emit_event(itr->first, *itr->second.value, timestamp);
                itr->second.modified = false;
            }
        }
        ++this->eventsReceived;
    }

    //
    // Script nodes shouldn't generate redraws.
    //
    this->modified(false);
}

/**
 * @brief Set the value of one of the node's eventOuts.
 *
 * This method is intended to be used by scripting language bindings to
 * set the value of eventOuts in response to script code.
 */
void ScriptNode::setEventOut(const std::string & id, const field_value & value)
    throw (unsupported_interface, std::bad_cast, std::bad_alloc)
{
    const EventOutValueMap::iterator itr(this->eventOutValueMap.find(id));
    if (itr == this->eventOutValueMap.end()) {
        throw unsupported_interface("Script node has no eventOut \"" + id
                                    + "\".");
    }

    if (itr->second.value->type() == field_value::sfnode_id) {
        this->assignWithSelfRefCheck(dynamic_cast<const sfnode &>(value),
                                     static_cast<sfnode &>(*itr->second.value));
    } else if (itr->second.value->type() == field_value::mfnode_id) {
        this->assignWithSelfRefCheck(dynamic_cast<const mfnode &>(value),
                                     static_cast<mfnode &>(*itr->second.value));
    } else {
        itr->second.value->assign(value); // Throws std::bad_cast.
    }
    itr->second.modified = true;
}

const field_value & ScriptNode::do_eventout(const std::string & id) const
    throw (unsupported_interface)
{
    FieldValueMap::const_iterator itr;
    if (id == "url" || id == "url_changed") {
        return this->url;
    } else if ((itr = this->fieldValueMap.find(id))
               != this->fieldValueMap.end()
            || (itr = this->fieldValueMap.find(id + "_changed"))
                != this->fieldValueMap.end()) {
        return *itr->second;
    }
    throw unsupported_interface("Script has no eventOut \"" + id + "\".");
}

void ScriptNode::do_shutdown(const double timestamp) throw ()
{
    if (this->script) { this->script->shutdown(timestamp); }
    this->scene()->browser.removeScript(*this);
}

} // namespace OpenVRML

# if OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT
#   include <math.h>
#   include <algorithm>
#   include <iostream>
#   include <memory>
#   include <sstream>
#   include <vector>
#   include <jsapi.h>
namespace {

namespace JavaScript_ {

class Script : public OpenVRML::Script {
    static JSRuntime * rt;
    static size_t nInstances;

    double d_timeStamp;

    JSContext * cx;

public:
    Script(OpenVRML::ScriptNode & scriptNode,
           const std::string & source)
            throw (std::bad_alloc);
    virtual ~Script();

    virtual void initialize(double timeStamp);
    virtual void processEvent(const std::string & id,
                              const OpenVRML::field_value & value,
                              double timestamp);
    virtual void eventsProcessed(double timeStamp);
    virtual void shutdown(double timeStamp);

    OpenVRML::ScriptNode & getScriptNode();

    jsval vrmlFieldToJSVal(const OpenVRML::field_value & value) throw ();

private:
    void initVrmlClasses() throw (std::bad_alloc);
    void defineBrowserObject() throw (std::bad_alloc);
    void defineFields() throw (std::bad_alloc);
    void activate(double timeStamp, const std::string & fname,
                  size_t argc, const OpenVRML::field_value * const argv[]);
};

}
}
# endif

namespace OpenVRML {

Script * ScriptNode::createScript() {
    // Try each url until we find one we like
    for (size_t i = 0; i < this->url.value.size(); ++i) {
        if (this->url.value[i].empty()) { continue; }

        // Get the protocol & mimetype...
# if OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT
        // Need to handle external .js files too...
        const char * javascriptScheme = "javascript:";
        const char * vrmlscriptScheme = "vrmlscript:";
        if (std::equal(javascriptScheme, javascriptScheme + 11,
                       this->url.value[i].begin())
                || std::equal(vrmlscriptScheme, vrmlscriptScheme + 11,
                              this->url.value[i].begin())) {
            return new JavaScript_::Script(*this,
                                           this->url.value[i].substr(11));
        }
# endif

#if OPENVRML_ENABLE_SCRIPT_NODE_JAVA
        const char javaExtension1[] = ".class";
        const char javaExtension2[] = ".CLASS";

        int slen = this->url.value[i].length();

        if (slen > 6
                && (std::equal(javaExtension1, javaExtension1 + 6,
                               this->url.value[i].end() - 6)
                    || std::equal(javaExtension2, javaExtension2 + 6,
                                  this->url.value[i].end() - 6))) {
            Doc2 base(this->type._class.browser.getWorldURI());
            Doc2 doc(this->url.value[i], &base);
            if (doc.localName()) {
                return new ScriptJDK(*this,
                                     doc.urlBase().c_str(),
                                     doc.localPath());
            }
        }
#endif
    }

    return 0;
}

} // namespace OpenVRML

namespace {

using namespace OpenVRML;

# if OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT
namespace JavaScript_ {

const long MAX_HEAP_BYTES = 4L * 1024L * 1024L;
const long STACK_CHUNK_BYTES = 4024L;

class BadConversion : public std::runtime_error {
public:
    BadConversion(const std::string & msg): runtime_error(msg) {}
    virtual ~BadConversion() throw () {}
};

JSBool floatsToJSArray(size_t numFloats, const float * floats,
                       JSContext * cx, jsval * rval);
std::auto_ptr<field_value>
createFieldValueFromJsval(JSContext * cx, jsval val,
                          field_value::type_id fieldType)
    throw (BadConversion, std::bad_alloc);

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
    bool changed;

    virtual ~FieldData() = 0;

protected:
    FieldData();

private:
    // Noncopyable.
    FieldData(const FieldData &);
    FieldData & operator=(const FieldData &);
};

class SField {
public:
    /**
     * The SFData class takes ownership of the FieldValue value and
     * deletes it upon destruction.
     */
    class SFData : public FieldData {
        field_value * const fieldValue;

    public:
        explicit SFData(field_value * fieldValue);
        virtual ~SFData();

        field_value & getFieldValue() const;
    };

protected:
    static void finalize(JSContext * cx, JSObject * obj) throw ();
    static JSBool toString(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
private:
    SField();
};

class SFColor : public SField {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const color & sfcolor,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::sfcolor>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);

private:
    static JSBool construct(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
    static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool setHSV(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool getHSV(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
};

class SFImage : public SField {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const OpenVRML::sfimage & sfcolor,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::sfimage>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);

private:
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv, jsval * rval) throw ();
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

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const NodePtr & node,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::sfnode>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
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

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const rotation & rotation,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::sfrotation>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);

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
                          uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool inverse(JSContext * cx, JSObject * obj,
                          uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multiply(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multVec(JSContext * cx, JSObject * obj,
                          uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool setAxis(JSContext * cx, JSObject * obj,
                          uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool slerp(JSContext * cx, JSObject * obj,
                        uintN argc, jsval * argv, jsval * rval) throw ();
};

class SFVec2f : public SField {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const vec2f & vec2f,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::sfvec2f>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);

private:
    static JSBool constructor(JSContext * cx, JSObject * obj,
	                      uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
    static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                             jsval * rval) throw ();
    static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool add(JSContext * cx, JSObject * obj,
                      uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool divide(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool dot(JSContext * cx, JSObject * obj,
                      uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool length(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multiply(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool negate(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool normalize(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool subtract(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
};

class SFVec3f : public SField {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const vec3f & vec3f,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::sfvec3f>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);

private:
    static JSBool constructor(JSContext * cx, JSObject * obj,
                              uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
    static JSBool getProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool setProperty(JSContext * cx, JSObject * obj, jsval id,
                              jsval * vp) throw ();
    static JSBool add(JSContext * cx, JSObject * obj,
                      uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool cross(JSContext * cx, JSObject * obj,
                        uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool divide(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool dot(JSContext * cx, JSObject * obj,
                      uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool length(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multiply(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool negate(JSContext * cx, JSObject * obj,
                         uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool normalize(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool subtract(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
};


class MField {
public:
    typedef std::vector<jsval> JsvalArray;

    class MFData : public FieldData {
    public:
        JsvalArray array;

        explicit MFData(JsvalArray::size_type size);
        virtual ~MFData();
    };

protected:
    static void AddRoots(JSContext * cx, JsvalArray & jsvalArray)
        throw (std::bad_alloc);
    static void RemoveRoots(JSContext * cx, JsvalArray & jsvalArray) throw ();

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
    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();

protected:
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv, jsval * vp) throw ();
    static JSBool setElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp) throw ();
    static JSBool setLength(JSContext * cx, JSObject * obj,
                            jsval id, jsval * vp) throw ();
    static JSBool toString(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
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
    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();

protected:
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv, jsval * vp) throw ();
    static JSBool setElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp) throw ();
    static JSBool setLength(JSContext * cx, JSObject * obj,
                            jsval id, jsval * vp) throw ();
    static JSBool toString(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static void finalize(JSContext * cx, JSObject * obj) throw ();

private:
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
};

class MFColor : public MFJSObject<MFColor> {
public:
    static JSClass jsclass;
    static JSClass & sfjsclass;

    static JSBool toJsval(const std::vector<color> & colors,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::mfcolor>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
};

class MFFloat : public MFJSDouble<MFFloat> {
public:
    static JSClass jsclass;

    static JSBool toJsval(const std::vector<float> & floats,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::mffloat>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
};

class MFInt32 : public MField {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj);
    static JSBool toJsval(const std::vector<OpenVRML::int32> & int32s,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::mfint32>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);

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

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const std::vector<NodePtr> & nodes,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::mfnode>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);

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
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static void finalize(JSContext * cx, JSObject * obj) throw ();
};

class MFRotation : public MFJSObject<MFRotation> {
public:
    static JSClass jsclass;
    static JSClass & sfjsclass;

    static JSBool toJsval(const std::vector<rotation> & rotations,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::mfrotation>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
};

class MFString : public MField {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const std::vector<std::string> & strings,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::mfstring>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);

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
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static void finalize(JSContext * cx, JSObject * obj) throw ();
};

class MFTime : public MFJSDouble<MFTime> {
public:
    static JSClass jsclass;

    static JSBool toJsval(const std::vector<double> & times,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::mftime>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
};

class MFVec2f : public MFJSObject<MFVec2f> {
public:
    static JSClass jsclass;
    static JSClass & sfjsclass;

    static JSBool toJsval(const std::vector<vec2f> & vec2fs,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::mfvec2f>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
};

class MFVec3f : public MFJSObject<MFVec3f> {
public:
    static JSClass jsclass;
    static JSClass & sfjsclass;

    static JSBool toJsval(const std::vector<vec3f> & vec3fs,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::mfvec3f>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
};

class VrmlMatrix {
public:
    //
    // The VrmlMatrixRow_ class is returned when using the [] operator on
    // a VrmlMatrix in order to facilitate matrix[i][j] notation. A
    // VrmlMatrixRow_ object holds a pointer to a row of the
    // OpenVRML::VrmlMatrix associate with the JavaScript VrmlMatrix that
    // spawned it. The only useful thing you can do with a VrmlMatrixRow_
    // object is use the [] operator to dereference a row element.
    //
    class Row {
    public:
        static JSClass jsclass;

        static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
        static JSBool construct(JSContext * cx, JSObject * obj,
	                        uintN argc, jsval * argv,
                                jsval * vp) throw ();
        static JSBool getElement(JSContext * cx, JSObject * obj,
                                 jsval id, jsval * vp) throw ();
        static JSBool setElement(JSContext * cx, JSObject * obj,
                                 jsval id, jsval * vp) throw ();
    };

    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool construct(JSContext * cx, JSObject * obj,
	                    uintN argc, jsval * argv,
                            jsval * vp) throw ();
    static JSBool initObject(JSContext * cx, JSObject * obj,
                             uintN argc, jsval * argv) throw ();
    static JSBool getElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp) throw ();
    static JSBool setElement(JSContext * cx, JSObject * obj,
                             jsval id, jsval * vp) throw ();
    static JSBool setTransform(JSContext * cx, JSObject * obj,
                               uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool getTransform(JSContext * cx, JSObject * obj,
                               uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool inverse(JSContext * cx, JSObject * obj,
                          uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool transpose(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multLeft(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multRight(JSContext * cx, JSObject * obj,
                            uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multVecMatrix(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool multMatrixVec(JSContext * cx, JSObject * obj,
                                uintN argc, jsval * argv, jsval * rval) throw ();
    static JSBool toString(JSContext * cx, JSObject * obj,
                           uintN argc, jsval * argv, jsval * rval) throw ();
    static void finalize(JSContext * cx, JSObject * obj) throw ();

private:
    VrmlMatrix();
};

JSRuntime * Script::rt = 0; // Javascript runtime singleton object
size_t Script::nInstances = 0; // Number of distinct script objects

JSBool eventOut_setProperty(JSContext * cx, JSObject * obj,
                            jsval id, jsval * val) throw ();

JSBool field_setProperty(JSContext * cx, JSObject * obj,
                         jsval id, jsval * val) throw ();

void errorReporter(JSContext * cx, const char * message,
                   JSErrorReport * report);


// Construct from inline script

Script::Script(ScriptNode & scriptNode, const std::string & source)
    throw (std::bad_alloc):
    OpenVRML::Script(scriptNode),
    cx(0)
{

    //
    // Initialize the runtime.
    //
    if (!rt) {
        if (!(rt = JS_NewRuntime(MAX_HEAP_BYTES))) { throw std::bad_alloc(); }
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

    JS_SetErrorReporter(cx, errorReporter);

    //
    // Define the global objects (builtins, Browser, SF*, MF*) ...
    //

    JSObject * const globalObj = JS_NewObject(this->cx, &Global::jsclass, 0, 0);
    if (!globalObj) { throw std::bad_alloc(); }

    if (!JS_InitStandardClasses(this->cx, globalObj)) {
        throw std::bad_alloc();
    }

    //
    // "print" function; non-standard, but widely used.
    //
    static JSFunctionSpec globalFunctions[] = {
        { "print", Global::print, 0 },
        { 0, 0, 0 }
    };
    JSBool ok = JS_DefineFunctions(cx, globalObj, globalFunctions);
    assert(ok);

    //
    // VRML-like TRUE, FALSE syntax
    //
    if (!(JS_DefineProperty(cx, globalObj, "FALSE",
                           BOOLEAN_TO_JSVAL(false), 0, 0,
                           JSPROP_READONLY | JSPROP_PERMANENT)
            && JS_DefineProperty(cx, globalObj, "TRUE",
                                 BOOLEAN_TO_JSVAL(true), 0, 0,
                                 JSPROP_READONLY | JSPROP_PERMANENT))) {
        throw std::bad_alloc();
    }

    //
    // Browser object.
    //
    this->defineBrowserObject();

    //
    // Define SF*/MF* classes.
    //
    this->initVrmlClasses();

    //
    // Define field/eventOut vars for this script.
    //
    this->defineFields();

    /* These should indicate source location for diagnostics. */
    char *filename = 0;
    uintN lineno = 0;

    jsval rval;
    ok = JS_EvaluateScript(cx, globalObj, source.c_str(), source.length(),
                           filename, lineno, &rval);
    OPENVRML_VERIFY_(ok);

    ++nInstances;
}

Script::~Script()
{
    using namespace std;
    cout << "Script::~Script" << endl;
    JS_DestroyContext(this->cx);
    if (--nInstances == 0) {
        JS_DestroyRuntime(rt);
        rt = 0;
    }
}

void Script::initialize(const double timestamp)
{
    const sftime arg(timestamp);
    const field_value * argv[] = { &arg };
    this->activate(timestamp, "initialize", 1, argv);
}

void Script::processEvent(const std::string & id,
                          const field_value & value,
                          const double timestamp)
{
    const sftime timestampArg(timestamp);
    const field_value * argv[] = { &value, &timestampArg };
    this->activate(timestamp, id, 2, argv);
}

void Script::eventsProcessed(const double timestamp)
{
    const sftime arg(timestamp);
    const field_value * argv[] = { &arg };
    this->activate(timestamp, "eventsProcessed", 1, argv);
}

void Script::shutdown(const double timestamp)
{
    const sftime arg(timestamp);
    const field_value * argv[] = { &arg };
    this->activate(timestamp, "shutdown", 1, argv);
}

double s_timeStamp; // XXX go away...

/**
 * @brief Run a specified script.
 *
 * @todo Decide what to do if a call to a JavaScript function fails
 *      (probably due to an error in the script).
 */
void Script::activate(const double timeStamp, const std::string & fname,
                      const size_t argc,
                      const field_value * const argv[])
{
    assert(this->cx);

    using namespace std;
    cout << "Script::activate: fname = " << fname << endl;

    jsval fval, rval;
    JSObject * const globalObj = JS_GetGlobalObject(this->cx);
    assert(globalObj);

    try {
        if (!JS_LookupProperty(this->cx, globalObj,
                               fname.c_str(), &fval)) {
            throw std::bad_alloc();
        }

        //
        // The function may not be defined, in which case we should do
        // nothing.
        //
        if (JSVAL_IS_VOID(fval)) { return; }

        d_timeStamp = timeStamp;
        s_timeStamp = timeStamp; // XXX this won't work for long...

        // convert FieldValue*'s to (gc-protected) jsvals
        std::vector<jsval> jsargv(argc);
        size_t i;
        for (i = 0; i < argc; ++i) {
            assert(argv[i]);
            jsargv[i] = vrmlFieldToJSVal(*argv[i]);
            if (!JS_AddRoot(this->cx, &jsargv[i])) {
                throw std::bad_alloc();
            }
        }

        JSBool ok = JS_CallFunctionValue(this->cx, globalObj,
                                         fval, argc, &jsargv[0], &rval);
        // XXX
        // XXX What should we do at this point if a function call fails?
        // XXX For now, just print a message for a debug build.
        // XXX
        OPENVRML_VERIFY_(ok);

        // Free up args
        for (i = 0; i < argc; ++i) {
            assert(jsargv[i] != JSVAL_NULL);
            if (JSVAL_IS_GCTHING(jsargv[i])) {
                ok = JS_RemoveRoot(cx, &jsargv[i]);
                assert(ok);
            }
        }

        //
        // Check to see if any eventOuts need to be sent.
        //
        for (ScriptNode::EventOutValueMap::const_iterator itr =
                this->scriptNode.getEventOutValueMap().begin();
                itr != this->scriptNode.getEventOutValueMap().end();
                ++itr) {
            assert(itr->second.value);
            jsval val;
            if (!JS_LookupProperty(this->cx, globalObj,
                                   itr->first.c_str(), &val)) {
                throw std::bad_alloc();
            }
            assert(val != JSVAL_VOID);
            if (JSVAL_IS_OBJECT(val)) {
                FieldData * fieldData =
                    static_cast<FieldData *>
                        (JS_GetPrivate(this->cx, JSVAL_TO_OBJECT(val)));
                if (fieldData->changed) {
                    std::auto_ptr<field_value> fieldValue =
                        createFieldValueFromJsval(this->cx, val,
                                                  itr->second.value->type());
                    itr->second.value->assign(*fieldValue);
                    this->scriptNode.setEventOut(itr->first,
                                                 *itr->second.value);
                    fieldData->changed = false;
                }
            }
        }
    } catch (std::exception & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
    } catch (...) {
        assert(false);
    }
}

ScriptNode & Script::getScriptNode()
{
    return this->scriptNode;
}

/**
 * @brief Create a jsval from an OpenVRML::field_value.
 */
jsval Script::vrmlFieldToJSVal(const field_value & fieldValue) throw ()
{
    jsval rval;
    JSObject * const globalObj = JS_GetGlobalObject(this->cx);
    assert(globalObj);

    switch (fieldValue.type()) {
    case field_value::sfbool_id:
        {
            using OpenVRML::sfbool;
            const sfbool & b = static_cast<const sfbool &>(fieldValue);
	    rval = BOOLEAN_TO_JSVAL(b.value);
        }
        break;

    case field_value::sfcolor_id:
        {
            using OpenVRML::sfcolor
            const sfcolor & c = static_cast<const sfcolor &>(fieldValue);
            if (!SFColor::toJsval(c.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sffloat_id:
        {
            using OpenVRML::sffloat;
	    const sffloat & f = static_cast<const sffloat &>(fieldValue);
            if (!JS_NewDoubleValue(cx, f.value, &rval)) { rval = JSVAL_NULL; }
        }
        break;

    case field_value::sfimage_id:
        {
            const OpenVRML::sfimage & sfimage =
                    static_cast<const OpenVRML::sfimage &>(fieldValue);
            if (!SFImage::toJsval(sfimage, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfint32_id:
        {
            const OpenVRML::sfint32 & sfint32 =
                    static_cast<const OpenVRML::sfint32 &>(fieldValue);
            if (!JS_NewNumberValue(cx, jsdouble(sfint32.value), &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfnode_id:
        {
            const OpenVRML::sfnode & sfnode =
                    static_cast<const OpenVRML::sfnode &>(fieldValue);
            if (!SFNode::toJsval(sfnode.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfrotation_id:
        {
            const OpenVRML::sfrotation & sfrotation =
                static_cast<const OpenVRML::sfrotation &>(fieldValue);
            if (!SFRotation::toJsval(sfrotation.value, this->cx, globalObj,
                                     &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfstring_id:
        {
            const OpenVRML::sfstring & sfstring =
                    static_cast<const OpenVRML::sfstring &>(fieldValue);
            rval = STRING_TO_JSVAL(
                        JS_NewStringCopyZ(cx, sfstring.value.c_str()));
        }
        break;

    case field_value::sftime_id:
        {
	    const OpenVRML::sftime & sftime =
                    static_cast<const OpenVRML::sftime &>(fieldValue);
            if (!JS_NewDoubleValue(cx, sftime.value, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfvec2f_id:
        {
            const OpenVRML::sfvec2f & sfvec2f =
                    static_cast<const OpenVRML::sfvec2f &>(fieldValue);
            if (!SFVec2f::toJsval(sfvec2f.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::sfvec3f_id:
        {
            const OpenVRML::sfvec3f & sfvec3f =
                    static_cast<const OpenVRML::sfvec3f &>(fieldValue);
            if (!SFVec3f::toJsval(sfvec3f.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfcolor_id:
        {
            const OpenVRML::mfcolor & mfcolor =
                    static_cast<const OpenVRML::mfcolor &>(fieldValue);
            if (!MFColor::toJsval(mfcolor.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mffloat_id:
        {
            const OpenVRML::mffloat & mffloat =
                    static_cast<const OpenVRML::mffloat &>(fieldValue);
            if (!MFFloat::toJsval(mffloat.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfint32_id:
        {
            const OpenVRML::mfint32 & mfint32 =
                    static_cast<const OpenVRML::mfint32 &>(fieldValue);
            if (!MFInt32::toJsval(mfint32.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfnode_id:
        {
            const OpenVRML::mfnode & mfnode =
                    static_cast<const OpenVRML::mfnode &>(fieldValue);
            if (!MFNode::toJsval(mfnode.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfrotation_id:
        {
            const OpenVRML::mfrotation & mfrotation =
                static_cast<const OpenVRML::mfrotation &>(fieldValue);
            if (!MFRotation::toJsval(mfrotation.value, this->cx, globalObj,
                                     &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfstring_id:
        {
            const OpenVRML::mfstring & mfstring =
                    static_cast<const OpenVRML::mfstring &>(fieldValue);
            if (!MFString::toJsval(mfstring.value, this->cx, globalObj,
                                   &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mftime_id:
        {
            const OpenVRML::mftime & mftime =
                    static_cast<const OpenVRML::mftime &>(fieldValue);
            if (!MFTime::toJsval(mftime.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfvec2f_id:
        {
            const OpenVRML::mfvec2f & mfvec2f =
                    static_cast<const OpenVRML::mfvec2f &>(fieldValue);
            if (!MFVec2f::toJsval(mfvec2f.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case field_value::mfvec3f_id:
        {
            const OpenVRML::mfvec3f & mfvec3f =
                    static_cast<const OpenVRML::mfvec3f &>(fieldValue);
            if (!MFVec3f::toJsval(mfvec3f.value, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    default:
        assert(false);
    }

    return rval;
}


// Must assign the proper type to eventOuts

JSBool eventOut_setProperty(JSContext * const cx, JSObject * const obj,
                            const jsval id, jsval * const val)
    throw ()
{
    JSString * const str = JS_ValueToString(cx, id);
    if (!str) { return JS_FALSE; }
    const char * const eventId = JS_GetStringBytes(str);

    //
    // The Script object pointer is stored as private data in the
    // context.
    //
    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    ScriptNode & scriptNode = script->getScriptNode();

    const field_value::type_id field_type_id =
        scriptNode.type.has_eventout(eventId);
    //
    // If this assertion is false, then we accidentally gave this
    // setter to an object that doesn't correspond to an eventOut!
    //
    assert(field_type_id != field_value::invalid_type_id);

    //
    // Convert to an OpenVRML::field_value and set the eventOut value.
    //
    try {
        std::auto_ptr<field_value> fieldValue =
                createFieldValueFromJsval(cx, *val, field_type_id);
        scriptNode.setEventOut(eventId, *fieldValue);
    } catch (BadConversion & ex) {
        std::cout << ex.what() << std::endl;
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }

    return JS_TRUE;
}

JSBool field_setProperty(JSContext * const cx, JSObject * const obj,
                         const jsval id, jsval * const val)
    throw ()
{
    JSString * const str = JS_ValueToString(cx, id);
    if (!str) { return JS_FALSE; }
    const char * const fieldId = JS_GetStringBytes(str);

    //
    // The Script object pointer is stored as private data in the
    // context.
    //
    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    ScriptNode & scriptNode = script->getScriptNode();

    const field_value::type_id field_type_id =
        scriptNode.type.has_field(fieldId);
    //
    // If this assertion is false, then we accidentally gave this
    // setter to an object that doesn't correspond to a field!
    //
    assert(field_type_id != field_value::invalid_type_id);

    //
    // Convert to an OpenVRML::FieldValue and set the eventOut value.
    //
    try {
        std::auto_ptr<field_value> fieldValue =
                createFieldValueFromJsval(cx, *val, field_type_id);
        scriptNode.field(fieldId, *fieldValue);
    } catch (BadConversion & ex) {
        std::cout << ex.what() << std::endl;
        return JS_FALSE;
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }

    return JS_TRUE;
}

//
// Initialize SF*/MF* types.
//
void Script::initVrmlClasses() throw (std::bad_alloc)
{
    JSObject * const globalObj = JS_GetGlobalObject(this->cx);
    assert(globalObj);
    if (!(SFColor::initClass(this->cx, globalObj)
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
            && MFVec3f::initClass(this->cx, globalObj)
            && VrmlMatrix::initClass(this->cx, globalObj)
            && VrmlMatrix::Row::initClass(this->cx, globalObj))) {
        throw std::bad_alloc();
    }
}

//
// Define the Browser object.
//
void Script::defineBrowserObject() throw (std::bad_alloc)
{
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
    if (!browserObj) { throw std::bad_alloc(); }

    if (!JS_DefineFunctions(this->cx, browserObj, methods)) {
        throw std::bad_alloc();
    }
}

//
// Define objects corresponding to fields/eventOuts
//
void Script::defineFields() throw (std::bad_alloc)
{
    JSObject * const globalObj = JS_GetGlobalObject(this->cx);
    assert(globalObj);

    {
        ScriptNode::FieldValueMap::const_iterator
                itr(this->scriptNode.getFieldValueMap().begin());
        for (; itr != this->scriptNode.getFieldValueMap().end();
                ++itr) {
            assert(itr->second);
            jsval val = vrmlFieldToJSVal(*itr->second);
            if (!JS_DefineProperty(this->cx, globalObj,
                                   itr->first.c_str(), val,
                                   0, field_setProperty, // getter, setter
                                   JSPROP_PERMANENT)) {
                throw std::bad_alloc();
            }
        }
    }

    {
        ScriptNode::EventOutValueMap::const_iterator itr;
        for (itr = this->scriptNode.getEventOutValueMap().begin();
                itr != this->scriptNode.getEventOutValueMap().end();
                ++itr) {
            assert(itr->second.value);
            jsval val = vrmlFieldToJSVal(*itr->second.value);
            if (!JS_DefineProperty(this->cx, globalObj,
                                   itr->first.c_str(), val,
			           0, eventOut_setProperty, // getter, setter
			           JSPROP_PERMANENT)) {
                throw std::bad_alloc();
            }
        }
    }
}


void errorReporter(JSContext * const cx,
                   const char * const message,
                   JSErrorReport * const errorReport)
{
    using std::endl;
    using std::string;

    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    OpenVRML::Browser & browser = script->getScriptNode().scene()->browser;

    string nodeId = script->getScriptNode().id();
    if (!nodeId.empty()) {
        browser.err << nodeId << ": ";
    }

    if (errorReport) {
        if (errorReport->filename) {
            browser.err << errorReport->filename << ": ";
        }

        browser.err << errorReport->lineno << ": ";
    }

    browser.err << message << endl;
}

JSBool floatsToJSArray(const size_t numFloats, const float * floats,
                       JSContext * const cx, jsval * const rval)
{
    std::vector<jsval> jsvec(numFloats);

    size_t i;
    for (i = 0; i < numFloats; ++i) {
        if (!JS_NewDoubleValue(cx, floats[i], &jsvec[i])) {
            return JS_FALSE;
        }
        if (!JS_AddRoot(cx, &jsvec[i])) { return JS_FALSE; }
    }

    JSObject * arr = 0;
    if (i == numFloats) {
        arr = JS_NewArrayObject(cx, numFloats, &jsvec[0]);
        if (arr) { *rval = OBJECT_TO_JSVAL(arr); }
    }

    for (size_t j = 0; j < i; ++j) { JS_RemoveRoot(cx, &jsvec[j]); }

    if (!arr) { return JS_FALSE; }

    *rval = OBJECT_TO_JSVAL(arr);
    return JS_TRUE;
}

/**
 * @brief Convert a jsval to a (new) FieldValue.
 */
std::auto_ptr<field_value>
createFieldValueFromJsval(JSContext * const cx, const jsval v,
                          const field_value::type_id expectType)
    throw (BadConversion, std::bad_alloc)
{
    using std::auto_ptr;

    switch (expectType) {
    case field_value::sfbool_id:
        {
            if (!JSVAL_IS_BOOLEAN(v)) {
                throw BadConversion("Boolean value expected.");
            }
            return auto_ptr<field_value>(new sfbool(JSVAL_TO_BOOLEAN(v)));
        }

    case field_value::sfcolor_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (SFColor::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::sffloat_id:
        {
            jsdouble d;
            if (!JS_ValueToNumber(cx, v, &d)) {
                throw BadConversion("Numeric value expected.");
            }
            return auto_ptr<field_value>(new sffloat(d));
        }

    case field_value::sfint32_id:
        {
            ::int32 i;
            if (!JS_ValueToECMAInt32(cx, v, &i)) {
                throw BadConversion("Numeric value expected.");
            }
            return auto_ptr<field_value>(new sfint32(i));
        }

    case field_value::sfimage_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (SFImage::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::sfnode_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (SFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::sfrotation_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (SFRotation::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::sfstring_id:
        {
            if (!JSVAL_IS_STRING(v)) {
                throw BadConversion("String value expected.");
            }
            JSString * const jsstring = JSVAL_TO_STRING(v);
            //
            // Is the null check here really necessary? Perhaps this
            // should be an assertion?
            //
            return auto_ptr<field_value>
                (new sfstring(jsstring
                              ? JS_GetStringBytes(jsstring)
                              : ""));
        }

    case field_value::sftime_id:
        {
            jsdouble d;
            if (!JS_ValueToNumber(cx, v, &d)) {
                throw BadConversion("Numeric value expected.");
            }
            return auto_ptr<field_value>(new sftime(d));
        }

    case field_value::sfvec2f_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (SFVec2f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::sfvec3f_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (SFVec3f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfcolor_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (MFColor::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mffloat_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (MFFloat::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfint32_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (MFInt32::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfnode_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (MFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfrotation_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (MFRotation::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfstring_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (MFString::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mftime_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (MFTime::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfvec2f_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (MFVec2f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    case field_value::mfvec3f_id:
        if (!JSVAL_IS_OBJECT(v)) { throw BadConversion("Object expected."); }
        return auto_ptr<field_value>
            (MFVec3f::createFromJSObject(cx, JSVAL_TO_OBJECT(v)).release());

    default:
        assert(false);
        return auto_ptr<field_value>();
    }
}

namespace Global {

JSBool print(JSContext * const cx, JSObject *,
             const uintN argc, jsval * const argv, jsval *)
{
    using std::cout;
    using std::flush;

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
	       uintN, jsval *, jsval * const rval)
    throw ()
{
    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    const char * const name =
            script->getScriptNode().type._class.browser.getName();
    *rval = STRING_TO_JSVAL(JS_InternString(cx, name));
    return JS_TRUE;
}

JSBool getVersion(JSContext * const cx, JSObject *,
		  uintN, jsval *, jsval * const rval)
    throw ()
{
    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    const char * const version =
            script->getScriptNode().type._class.browser.getVersion();
    *rval = STRING_TO_JSVAL(JS_InternString(cx, version));
    return JS_TRUE;
}

JSBool getCurrentSpeed(JSContext * const cx, JSObject *,
                       uintN, jsval *, jsval * const rval)
    throw ()
{
    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    float speed = script->getScriptNode().type._class
                  .browser.getCurrentSpeed();
    *rval = DOUBLE_TO_JSVAL(JS_NewDouble( cx, speed ));
    return JS_TRUE;
}

JSBool getCurrentFrameRate(JSContext * const cx, JSObject *,
                           uintN, jsval *, jsval * const rval)
    throw ()
{
    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    *rval = DOUBLE_TO_JSVAL(JS_NewDouble(cx,
                script->getScriptNode().type._class
                    .browser.getFrameRate()));
    return JS_TRUE;
}

JSBool getWorldURL(JSContext * const cx, JSObject *,
                   uintN, jsval *, jsval * const rval)
    throw ()
{
    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    const std::string url =
            script->getScriptNode().type._class
                .browser.getWorldURI();
    *rval = STRING_TO_JSVAL(JS_InternString(cx, url.c_str()));
    return JS_TRUE;
}


// No events will be processed after loadURL.

JSBool loadURL(JSContext * const cx, JSObject *,
               const uintN argc, jsval * const argv,
               jsval * const rval)
    throw ()
{
    assert(argc >= 2);

    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    //
    // Make sure our first argument (the URL) is an MFString.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &MFString::jsclass, 0)) {
        return JS_FALSE;
    }

    std::auto_ptr<OpenVRML::mfstring> url =
            MFString::createFromJSObject(cx, JSVAL_TO_OBJECT(argv[0]));
    assert(url.get());

    //
    // Make sure our second argument is an MFString
    //
    if (!JSVAL_IS_OBJECT(argv[1])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[1]),
                              &MFString::jsclass, 0)) {
        return JS_FALSE;
    }

    std::auto_ptr<OpenVRML::mfstring> parameters =
            MFString::createFromJSObject(cx, JSVAL_TO_OBJECT(argv[1]));
    assert(parameters.get());

    script->getScriptNode().scene()
            ->browser.loadURI(url->value, parameters->value);
    return JS_TRUE;
}


// This does return, but no events will be processed after it is called.

JSBool replaceWorld(JSContext * const cx, JSObject *,
                    const uintN argc, jsval * const argv,
                    jsval * const rval)
    throw ()
{
    assert(argc >= 1);

    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    //
    // Make sure our argument is an MFNode.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &MFNode::jsclass, 0)) {
        return JS_FALSE;
    }

    std::auto_ptr<OpenVRML::mfnode> nodes =
            MFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(argv[0]));
    assert(nodes.get());

    script->getScriptNode().scene()->browser.replaceWorld(nodes->value);

    *rval = JSVAL_VOID;
    return JS_TRUE;
}

JSBool createVrmlFromString(JSContext * const cx,
                            JSObject * const obj,
			    const uintN argc, jsval * const argv,
                            jsval * const rval)
    throw ()
{
    assert(argc >= 1);

    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    //
    // Make sure our argument is a string.
    //
    if (!JSVAL_IS_STRING(argv[0])) { return JS_FALSE; }

    JSString * str = JSVAL_TO_STRING(argv[0]);
    assert(str);

    try {
        std::istringstream in(JS_GetStringBytes(str));

        assert(script->getScriptNode().scene());
        OpenVRML::Browser & browser =
                script->getScriptNode().scene()->browser;
        const std::vector<NodePtr> nodes = browser.createVrmlFromStream(in);

        if (nodes.empty()) {
            *rval = JSVAL_NULL;
        } else {
            if (!MFNode::toJsval(nodes, cx, obj, rval)) {
                return JS_FALSE;
            }
        }
        return JS_TRUE;
    } catch (...) {
        return JS_FALSE;
    }
}


// createVrmlFromURL( MFString url, SFNode node, SFString event )

JSBool createVrmlFromURL(JSContext * const cx, JSObject *,
			 const uintN argc, jsval * const argv,
                         jsval * const rval)
    throw ()
{
# if 0
    assert(argc >= 3);

    Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    Doc2 * relative = script->getScriptNode().nodeType.nodeClass
            .browser.urlDoc();

    //
    // Make sure our first argument (the URL) is an MFString.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &MFString::jsclass, 0)) {
        return JS_FALSE;
    }

    std::auto_ptr<OpenVRML::mfstring>
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

    std::auto_ptr<OpenVRML::sfnode>
            sfnode(SFNode::createFromJSObject(cx,
                                        JSVAL_TO_OBJECT(argv[1])));
    assert(sfnode.get());
    NodePtr node(sfnode->get());
    if (!node) { return JS_FALSE; }

    //
    // Make sure our third argument is a string.
    //
    if (!JSVAL_IS_STRING(argv[2])) { return JS_FALSE; }

    const char * const eventInId =
            JS_GetStringBytes(JSVAL_TO_STRING(argv[2]));
    OpenVRML::Browser & browser =
            script->getScriptNode().nodeType.nodeClass.browser;
    std::auto_ptr<OpenVRML::mfnode>
            kids(browser.readWrl(*url, relative));

    if (!kids.get()) { return JS_FALSE; }

    node->processEvent(eventInId, *kids,
                       s_timeStamp); // fix me...

    *rval = JSVAL_VOID;
# endif
    return JS_TRUE;
}

// addRoute(SFNode fromNode, String fromEventOut, SFNode toNode, String toEvent)

JSBool addRoute(JSContext * const cx, JSObject *,
                const uintN argc, jsval * const argv,
                jsval * const rval)
    throw ()
{
    assert(argc >= 4);

    //
    // Make sure our first argument (fromNode) is a SFNode.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFNode::jsclass, 0)) {
        return JS_FALSE;
    }
    std::auto_ptr<OpenVRML::sfnode> fromNode =
            SFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(argv[0]));

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
    std::auto_ptr<OpenVRML::sfnode> toNode =
            SFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(argv[2]));
    //
    // Make sure the node is not null.
    //
    if (!toNode.get()) {
        JS_ReportError(cx, "NULL destination node.");
        return JS_FALSE;
    }

    //
    // Makes sure our fourth argument (toEventIn) is a string.
    //
    if (!JSVAL_IS_STRING(argv[3])) {
        return JS_FALSE;
    }
    const char * const toEventIn =
            JS_GetStringBytes(JSVAL_TO_STRING(argv[3]));

    try {
        fromNode->value->add_route(fromEventOut, toNode->value, toEventIn);
    } catch (std::runtime_error & ex) {
        JS_ReportError(cx, ex.what());
        return JS_FALSE;
    }

    *rval = JSVAL_VOID;
    return JS_TRUE;
}

// deleteRoute(SFNode fromNode, String fromEventOut, SFNode toNode, String toEvent)

JSBool deleteRoute(JSContext * const cx, JSObject *,
                   const uintN argc, jsval * const argv,
                   jsval * const rval)
    throw ()
{
    assert(argc >= 4);

    //
    // Make sure our first argument (fromNode) is a SFNode.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFNode::jsclass, 0)) {
        return JS_FALSE;
    }
    std::auto_ptr<OpenVRML::sfnode> fromNode =
            SFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(argv[0]));

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
    std::auto_ptr<OpenVRML::sfnode> toNode =
            SFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(argv[0]));

    //
    // Makes sure our fourth argument (toEventIn) is a string.
    //
    if (!JSVAL_IS_STRING(argv[3])) {
        return JS_FALSE;
    }
    const char * const toEventIn =
            JS_GetStringBytes(JSVAL_TO_STRING(argv[3]));

    fromNode->value->delete_route(fromEventOut, toNode->value, toEventIn);

    *rval = JSVAL_VOID;
    return JS_TRUE;
}

JSBool setDescription(JSContext * const cx, JSObject *,
		      const uintN argc, jsval * const argv,
                      jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    if (!JSVAL_IS_STRING(argv[0])) {
        return JS_FALSE;
    }
    theSystem->inform("%s",
                      JS_GetStringBytes(JSVAL_TO_STRING(argv[0])));
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

} // namespace Browser


//
// FieldData
//

FieldData::FieldData():
    changed(false)
{}

FieldData::~FieldData()
{}


//
// SField
//

SField::SFData::SFData(field_value * const fieldValue):
    fieldValue(fieldValue)
{
    assert(fieldValue);
}

SField::SFData::~SFData()
{
    delete this->fieldValue;
}

field_value & SField::SFData::getFieldValue() const
{
    return *this->fieldValue;
}

void SField::finalize(JSContext * const cx, JSObject * const obj)
    throw ()
{
    delete static_cast<FieldData *>(JS_GetPrivate(cx, obj));
    JS_SetPrivate(cx, obj, 0);
}

JSBool SField::toString(JSContext * const cx, JSObject * const obj,
                        uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * const sfdata =
            static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);

    std::ostringstream out;
    out << sfdata->getFieldValue();
    JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

JSClass SFColor::jsclass =
        { "SFColor", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
          JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };

JSObject * SFColor::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
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

    JSObject * const proto = JS_InitClass(cx, obj, 0, &jsclass,
                                          construct, 0, // constructor function, min arg count
                                          properties, methods,
                                          0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool SFColor::toJsval(const color & color,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const sfcolorObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfcolorObj) { return JS_FALSE; }

    const SFData * const sfdata =
            static_cast<SFData *>(JS_GetPrivate(cx, sfcolorObj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfcolor *>(&sfdata->getFieldValue()));

    //
    // Assignment does not throw.
    //
    static_cast<OpenVRML::sfcolor &>(sfdata->getFieldValue()).value = color;

    *rval = OBJECT_TO_JSVAL(sfcolorObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::sfcolor>
SFColor::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFColor::jsclass, 0)) {
        throw BadConversion("SFColor object expected.");
    }
    const SFData * const sfdata =
        static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfcolor *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::sfcolor>
            (static_cast<OpenVRML::sfcolor *>
                (sfdata->getFieldValue().clone().release()));
}

JSBool SFColor::construct(JSContext * const cx, JSObject * obj,
	                  const uintN argc, jsval * const argv,
                          jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool SFColor::initObject(JSContext * const cx, JSObject * const obj,
                           const uintN argc, jsval * const argv)
    throw ()
{
    jsdouble rgb[] = { 0.0, 0.0, 0.0 };
    for (uintN i = 0; i < ((argc < 3) ? argc : 3); ++i) {
        if (!JSVAL_IS_NUMBER(argv[i])
                || !JS_ValueToNumber(cx, argv[i], rgb + i)) {
            return JS_FALSE;
        }
    }

    try {
        std::auto_ptr<OpenVRML::sfcolor>
                sfcolor(new OpenVRML::sfcolor(color(rgb[0], rgb[1], rgb[2])));
        std::auto_ptr<SFData> sfdata(new SFData(sfcolor.get()));
        sfcolor.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
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
    throw ()
{
    const SFData * const sfdata =
            static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfcolor *>(&sfdata->getFieldValue()));
    const OpenVRML::sfcolor & thisColor =
            static_cast<OpenVRML::sfcolor &>(sfdata->getFieldValue());

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) { return JS_FALSE; }
        if (!JS_NewDoubleValue(cx, thisColor.value[JSVAL_TO_INT(id)], rval)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool SFColor::setProperty(JSContext * const cx, JSObject * const obj,
                            const jsval id, jsval * const vp)
    throw ()
{
    SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfcolor *>(&sfdata->getFieldValue()));
    OpenVRML::sfcolor & thisColor =
            static_cast<OpenVRML::sfcolor &>(sfdata->getFieldValue());

    if (!JSVAL_IS_INT(id)
            || JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) {
        return JS_FALSE;
    }

    jsdouble d;
    if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

    switch (JSVAL_TO_INT(id)) {
    case 0:
        thisColor.value.r(d);
        break;

    case 1:
        thisColor.value.g(d);
        break;

    case 2:
        thisColor.value.b(d);
        break;

    default:
        assert(false);
    }
    sfdata->changed = true;
    return JS_TRUE;
}

JSBool SFColor::setHSV(JSContext * const cx, JSObject * const obj,
                       const uintN argc, jsval * const argv,
                       jsval * const rval)
    throw ()
{
    assert(argc >= 3);

    SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfcolor *>(&sfdata->getFieldValue()));
    OpenVRML::sfcolor & thisColor =
            static_cast<OpenVRML::sfcolor &>(sfdata->getFieldValue());

    jsdouble h, s, v;

    if (!JSVAL_IS_NUMBER(argv[0])
            || !JS_ValueToNumber(cx, argv[0], &h)
            || !JSVAL_IS_NUMBER(argv[1])
            || !JS_ValueToNumber(cx, argv[1], &s)
            || !JSVAL_IS_NUMBER(argv[2])
            || !JS_ValueToNumber(cx, argv[2], &v)) {
        return JS_FALSE;
    }

    thisColor.value.hsv(h, s, v);
    *rval = JSVAL_VOID;
    sfdata->changed = true;
    return JS_TRUE;
}

JSBool SFColor::getHSV(JSContext * const cx, JSObject * const obj,
                       uintN, jsval *, jsval * rval)
    throw ()
{
    const SFData * const sfdata =
            static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfcolor *>(&sfdata->getFieldValue()));
    const OpenVRML::sfcolor & thisColor =
            static_cast<OpenVRML::sfcolor &>(sfdata->getFieldValue());

    float hsv[3];
    thisColor.value.hsv(hsv);

    return floatsToJSArray(3, hsv, cx, rval);
}


JSClass SFImage::jsclass =
        { "SFImage", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
          JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };

JSObject * SFImage::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
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

    JSObject * const proto = JS_InitClass(cx, obj, 0, &jsclass,
                                          construct, 3, // constructor function, min arg count
                                          properties, methods,
                                          0, 0); // static properties and methods
    jsval args[] = { INT_TO_JSVAL(0), INT_TO_JSVAL(0), INT_TO_JSVAL(0) };
    if (!proto || !initObject(cx, proto, 3, args)) { return 0; }
    return proto;
}

JSBool SFImage::toJsval(const OpenVRML::sfimage & sfimage,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    //
    // Can't call JS_ConstructObject() here since SFImage's ctor
    // requires 4 arguments.
    //
    JSObject * const sfimageObj = JS_NewObject(cx, &jsclass, 0, obj);
    if (!sfimageObj) { return JS_FALSE; }

    try {
        std::auto_ptr<OpenVRML::sfimage>
                sfimageClone(static_cast<OpenVRML::sfimage *>
                    (sfimage.clone().release()));
        std::auto_ptr<SFData> sfdata(new SFData(sfimageClone.get()));
        sfimageClone.release();
        if (!JS_SetPrivate(cx, sfimageObj, sfdata.get())) { return JS_FALSE; }
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

std::auto_ptr<OpenVRML::sfimage>
SFImage::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFImage::jsclass, 0)) {
        throw BadConversion("SFImage object expected.");
    }
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfimage *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::sfimage>
            (static_cast<OpenVRML::sfimage *>
                (sfdata->getFieldValue().clone().release()));
}

JSBool SFImage::construct(JSContext * const cx, JSObject * obj,
	                  const uintN argc, jsval * const argv,
                          jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
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
    throw ()
{
    assert(argc >= 3);

    //
    // x dimension.
    //
    if (!JSVAL_IS_INT(argv[0])) { return JS_FALSE; }
    const size_t x = JSVAL_TO_INT(argv[0]);

    //
    // y dimension.
    //
    if (!JSVAL_IS_INT(argv[1])) { return JS_FALSE; }
    const size_t y = JSVAL_TO_INT(argv[1]);

    //
    // components
    //
    if (!JSVAL_IS_INT(argv[2])) { return JS_FALSE; }
    const size_t comp = JSVAL_TO_INT(argv[2]);

    try {
        //
        // pixel data array
        //
        typedef std::vector<unsigned char> Pixels;

        Pixels pixels(x * y * comp, 0);

        if (argc > 3) {
            if (!JSVAL_IS_OBJECT(argv[3])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[3]),
                                      &MFInt32::jsclass, 0)) {
                return JS_FALSE;
            }

            MField::MFData * const mfdata =
                    static_cast<MField::MFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[3])));

            if (mfdata->array.size() != (x * y)) { return JS_FALSE; }

            Pixels::iterator pixelPtr = pixels.begin();
            for (MField::JsvalArray::size_type i(0);
                    i < mfdata->array.size(); ++i, pixelPtr += comp) {
                assert(JSVAL_IS_INT(mfdata->array[i]));
                static const long byteMask[] =
                        { 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000 };
                ::int32 pixel;
                if (!JS_ValueToInt32(cx, mfdata->array[i], &pixel)) {
                    return JS_FALSE;
                }
                for (size_t j(0); j < comp; ++j) {
                    *(pixelPtr + j) = (pixel & byteMask[j]) >> (8 * j);
                }
            }
        }

        std::auto_ptr<OpenVRML::sfimage>
                sfimage(new OpenVRML::sfimage(x, y, comp, &pixels[0]));
        std::auto_ptr<SFData> sfdata(new SFData(sfimage.get()));
        sfimage.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
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
                            const jsval id, jsval * const vp)
    throw ()
{
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfimage *>(&sfdata->getFieldValue()));
    const OpenVRML::sfimage & thisImage =
            static_cast<OpenVRML::sfimage &>(sfdata->getFieldValue());

    if (JSVAL_IS_INT(id)) {
        switch (JSVAL_TO_INT(id)) {
        case 0:
            *vp = INT_TO_JSVAL(thisImage.x());
            break;
        case 1:
            *vp = INT_TO_JSVAL(thisImage.y());
            break;
        case 2:
            *vp = INT_TO_JSVAL(thisImage.comp());
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
    throw ()
{
  // ...
  return JS_FALSE;
}


JSClass SFNode::jsclass =
        { "SFNode", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub,  JS_PropertyStub, getProperty, setProperty,
          JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };

JSObject * SFNode::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
    static JSFunctionSpec methods[] =
            { { "toString", SFNode::toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    jsval arg = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "Group {}"));
    JSObject * const proto = JS_InitClass(cx, obj, 0, &jsclass,
                                          SFNode::construct, 1, // constructor function, min arg count
                                          0, methods, // instance properties, methods
                                          0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 1, &arg)) { return 0; }
    return proto;
}

JSBool SFNode::toJsval(const NodePtr & node,
                       JSContext * const cx,
                       JSObject * const obj,
                       jsval * const rval)
    throw ()
{
    //
    // The SFNode constructor requires one arg (a vrmlstring),
    // so we can't use JS_ConstructObject here.
    //
    JSObject * const sfnodeObj = JS_NewObject(cx, &jsclass, 0, obj);
    if (!sfnodeObj) { return JS_FALSE; }

    try {
        std::auto_ptr<OpenVRML::sfnode> sfnodeClone(new OpenVRML::sfnode(node));
        std::auto_ptr<SFData> sfdata(new SFData(sfnodeClone.get()));
        sfnodeClone.release();
        if (!JS_SetPrivate(cx, sfnodeObj, sfdata.get())) { return JS_FALSE; }
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

std::auto_ptr<OpenVRML::sfnode>
SFNode::createFromJSObject(JSContext * const cx,
                           JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFNode::jsclass, 0)) {
        throw BadConversion("SFNode object expected.");
    }
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfnode *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::sfnode>
            (static_cast<OpenVRML::sfnode *>
                (sfdata->getFieldValue().clone().release()));
}

JSBool SFNode::construct(JSContext * const cx, JSObject * obj,
                         const uintN argc, jsval * const argv,
                         jsval * rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool SFNode::initObject(JSContext * const cx, JSObject * const obj,
                          const uintN argc, jsval * const argv)
    throw ()
{
    using std::istringstream;
    assert(argc >= 1);

    Script * const script = static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    //
    // Make sure our argument is a string.
    //
    if (!JSVAL_IS_STRING(argv[0])) { return JS_FALSE; }

    JSString * const str = JSVAL_TO_STRING(argv[0]);
    assert(str);

    istringstream in(JS_GetStringBytes(str));

    assert(script->getScriptNode().scene());
    OpenVRML::Browser & browser = script->getScriptNode().scene()->browser;
    std::vector<NodePtr> nodes;
    try {
        nodes = browser.createVrmlFromStream(in);
    } catch (InvalidVrml & ex) {
        JS_ReportError(cx, ex.what());
        return JS_FALSE;
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }

    //
    // Fail if the string does not produce exactly one node.
    //
    if (nodes.size() != 1) { return JS_FALSE; }

    try {
        std::auto_ptr<OpenVRML::sfnode>
                sfnode(new OpenVRML::sfnode(nodes[0]));
        std::auto_ptr<SFData> sfdata(new SFData(sfnode.get()));
        sfnode.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
        sfdata.release();
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    return JS_TRUE;
}

//
// SFNode getProperty reads eventOut values, setProperty sends eventIns.
//
JSBool SFNode::getProperty(JSContext * const cx, JSObject * const obj,
                           const jsval id, jsval * const vp)
    throw ()
{
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfnode *>(&sfdata->getFieldValue()));
    const OpenVRML::sfnode & thisNode =
            static_cast<OpenVRML::sfnode &>(sfdata->getFieldValue());

    if (!thisNode.value) { return JS_TRUE; }

    Script * const script = static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    try {
        JSString * str = 0;
        if (JSVAL_IS_STRING(id) && ((str = JSVAL_TO_STRING(id)))) {
            const char * eventOut = JS_GetStringBytes(str);
            const field_value & fieldVal = thisNode.value->eventout(eventOut);

            // convert event out value to jsval...
            *vp = script->vrmlFieldToJSVal(fieldVal);

            // If JS_FALSE is returned, apparently the parent object is
            // not searched for the property, so stuff like toString()
            // will fail... don't know how to distinguish between a valid
            // prop and an invalid eventOut...
            return JS_TRUE;
        }
    } catch (unsupported_interface & ex) {}
    return JS_TRUE;
}

JSBool SFNode::setProperty(JSContext * const cx, JSObject * const obj,
                           const jsval id, jsval * const vp)
    throw ()
{
    SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfnode *>(&sfdata->getFieldValue()));
    OpenVRML::sfnode & thisNode =
            static_cast<OpenVRML::sfnode &>(sfdata->getFieldValue());

    NodePtr nodePtr = thisNode.value;

    if (!nodePtr || !JSVAL_IS_STRING(id)) { return JS_FALSE; }

    const char * const eventInId = JS_GetStringBytes(JSVAL_TO_STRING(id));
    if (!eventInId) { return JS_FALSE; }

    // convert vp to field, send eventIn to node
    field_value::type_id expectType;
    std::auto_ptr<field_value> fieldValue;
    if ((expectType = nodePtr->type.has_eventin(eventInId))) {
        try {
            fieldValue =  createFieldValueFromJsval(cx, *vp, expectType);
        } catch (BadConversion & ex) {
            std::cout << ex.what() << std::endl;
            return JS_FALSE;
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return JS_FALSE;
        }
        // This should only happen if directOutput is set...

	// the timestamp should be stored as a global property and
	// looked up via obj somehow...
	Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
        assert(script);
        script->getScriptNode().scene()->browser
            .queueEvent(s_timeStamp,
                        fieldValue.release(),
                        nodePtr,
                        eventInId);
    } else if ((expectType = nodePtr->type.has_field(eventInId))) {
        try {
            fieldValue = createFieldValueFromJsval(cx, *vp, expectType);
        } catch (BadConversion & ex) {
            std::cout << ex.what() << std::endl;
            return JS_FALSE;
        } catch (std::bad_alloc & ex) {
            OPENVRML_PRINT_EXCEPTION_(ex);
            return JS_FALSE;
        }
	// This should only happen if directOutput is set...

	// the timestamp should be stored as a global property and
	// looked up via obj somehow...
	Script * const script =
            static_cast<Script *>(JS_GetContextPrivate(cx));
	assert(script);
	script->getScriptNode().scene()->browser
                .queueEvent(s_timeStamp,
                            fieldValue.release(),
                            nodePtr,
                            eventInId);
    }
    sfdata->changed = true;
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

    JSObject * const proto = JS_InitClass(cx, obj, 0, &jsclass,
                                          construct, 0, // constructor function, min arg count
                                          properties, methods, // instance properties, methods
                                          0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool SFRotation::toJsval(const rotation & rotation,
                           JSContext * const cx,
                           JSObject * const obj,
                           jsval * const rval)
    throw ()
{
    JSObject * const sfrotationObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfrotationObj) { return JS_FALSE; }

    const SFData * const sfdata =
        static_cast<SFData *>(JS_GetPrivate(cx, sfrotationObj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));

    //
    // Assignment does not throw.
    //
    static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue()).value =
            rotation;
    *rval = OBJECT_TO_JSVAL(sfrotationObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::sfrotation>
SFRotation::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFRotation::jsclass, 0)) {
        throw BadConversion("SFRotation object expected.");
    }
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::sfrotation>
            (static_cast<OpenVRML::sfrotation *>
                (sfdata->getFieldValue().clone().release()));
}

JSBool SFRotation::construct(JSContext * const cx, JSObject * obj,
                             const uintN argc, jsval * const argv,
                             jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool SFRotation::initObject(JSContext * const cx,
                              JSObject * const obj,
                              const uintN argc, jsval * const argv)
    throw ()
{
    jsdouble x = 0.0, y = 1.0, z = 0.0, angle = 0.0;
    if (argc > 0) {
        if (JSVAL_IS_NUMBER(argv[0])) {
	    if (!JS_ValueToNumber(cx, argv[0], &x)) { return JS_FALSE; }
	    if (argc > 1) {
                if (!JSVAL_IS_NUMBER(argv[1])
                        || !JS_ValueToNumber(cx, argv[1], &y)) {
                    return JS_FALSE;
                }

	        if (argc > 2) {
                    if (!JSVAL_IS_NUMBER(argv[2])
                            || !JS_ValueToNumber(cx, argv[2], &z)) {
                        return JS_FALSE;
                    }
                    if (argc > 3) {
                        if (!JSVAL_IS_NUMBER(argv[3])
                                || !JS_ValueToNumber(cx, argv[3], &angle)) {
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
            assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
            const OpenVRML::sfvec3f & argVec1 =
                    static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

            if (argc > 1) {
                if (JSVAL_IS_OBJECT(argv[1])) {
                    if (!JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[1]),
                                   &JavaScript_::SFVec3f::jsclass, 0)) {
                        return JS_FALSE;
                    }
                    sfdata = static_cast<SFData *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1])));
                    assert(sfdata);
                    assert(dynamic_cast<OpenVRML::sfvec3f *>
                                (&sfdata->getFieldValue()));
                    const OpenVRML::sfvec3f & argVec2 =
                            (static_cast<OpenVRML::sfvec3f &>
                                (sfdata->getFieldValue()));

                    vec3f axisVec = argVec1.value * argVec2.value;
                    axisVec = axisVec.normalize();
                    x = axisVec.x();
                    y = axisVec.y();
                    z = axisVec.z();
                    angle = acos(argVec1.value.dot(argVec2.value)
                            / (argVec1.value.length()
                                * argVec2.value.length()));

                } else if (JSVAL_IS_NUMBER(argv[1])) {
                    if (!JS_ValueToNumber(cx, argv[1], &angle)) {
                        return JS_FALSE;
                    }
                    x = argVec1.value.x();
                    y = argVec1.value.y();
                    z = argVec1.value.z();

                } else {
                    return JS_FALSE;
                }
            }
        } else {
            return JS_FALSE;
        }
    }

    try {
        std::auto_ptr<OpenVRML::sfrotation>
                sfrotation(new OpenVRML::sfrotation(rotation(x, y, z, angle)));
        std::auto_ptr<SFData> sfdata(new SFData(sfrotation.get()));
        sfrotation.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
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
    throw ()
{
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    const OpenVRML::sfrotation & thisRot =
        static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 3) { return JS_FALSE; }
        if (!JS_NewDoubleValue(cx, thisRot.value[JSVAL_TO_INT(id)], rval)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool SFRotation::setProperty(JSContext * const cx,
                               JSObject * const obj,
                               const jsval id, jsval * const vp)
    throw ()
{
    SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    OpenVRML::sfrotation & thisRot =
        static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());

    if (!JSVAL_IS_INT(id)) { return JS_FALSE; }

    if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 3) { return JS_FALSE; }

    jsdouble d;
    if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

    switch (JSVAL_TO_INT(id)) {
    case 0:
        thisRot.value.x(d);
        break;

    case 1:
        thisRot.value.y(d);
        break;

    case 2:
        thisRot.value.z(d);
        break;

    case 3:
        thisRot.value.angle(d);
        break;

    default:
        return JS_FALSE;
    }
    sfdata->changed = true;
    return JS_TRUE;
}

JSBool SFRotation::getAxis(JSContext * const cx, JSObject * const obj,
                           uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    const OpenVRML::sfrotation & thisRot =
        static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());
    sfdata = 0;

    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0, obj);
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & resultVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    try {
        resultVec.value = thisRot.value.axis();
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
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    const OpenVRML::sfrotation & thisRot =
            static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());
    sfdata = 0;

    JSObject * const robj = JS_ConstructObject(cx, &SFRotation::jsclass, 0,
			                       JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    OpenVRML::sfrotation & resultRot =
        static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());

    try {
        resultRot.value = thisRot.value.inverse();
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
                            jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    const OpenVRML::sfrotation & thisRot =
            static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Make sure our argument is an SFRotation.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFRotation::jsclass, 0)) {
        return JS_FALSE;
    }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    const OpenVRML::sfrotation & argRot =
            static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());

    //
    // Construct the result object.
    //
    JSObject * robj = JS_ConstructObject(cx, &SFRotation::jsclass, 0,
                                         JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    OpenVRML::sfrotation & resultRot =
        static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());

    try {
        resultRot.value = thisRot.value * argRot.value;
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
	                   jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    const OpenVRML::sfrotation & thisRot =
            static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & argVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                         JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & resultVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    resultVec.value = argVec.value * mat4f::rotation(thisRot.value);

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFRotation::setAxis(JSContext * const cx, JSObject * const obj,
                           const uintN argc, jsval * const argv,
                           jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    OpenVRML::sfrotation & thisRot =
            static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());

    //
    // Make sure our argument is an SFVec3f.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFVec3f::jsclass, 0)) {
        return JS_FALSE;
    }

    const SFData * const argSFData =
            static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(argSFData);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&argSFData->getFieldValue()));
    const OpenVRML::sfvec3f & argVec =
            static_cast<OpenVRML::sfvec3f &>(argSFData->getFieldValue());

    thisRot.value.axis(argVec.value);

    sfdata->changed = true;
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

JSBool SFRotation::slerp(JSContext * const cx, JSObject * const obj,
                         const uintN argc, jsval * const argv,
                         jsval * const rval)
    throw ()
{
    assert(argc >= 2);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    const OpenVRML::sfrotation & thisRot =
            static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Make sure our first argument is an SFRotation.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFRotation::jsclass, 0)) {
        return JS_FALSE;
    }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    const OpenVRML::sfrotation & argRot =
            static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());
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
    JSObject * const robj = JS_ConstructObject(cx, &SFRotation::jsclass,
                                               0, JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfrotation *>(&sfdata->getFieldValue()));
    OpenVRML::sfrotation & resultRot =
            static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());

    resultRot.value = thisRot.value.slerp(argRot.value, factor);

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}


JSClass SFVec2f::jsclass =
        { "SFVec2f", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
          JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };

JSObject * SFVec2f::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
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

    JSObject * const proto = JS_InitClass(cx, obj, 0, &jsclass,
                                          constructor, 0, // constructor function, min arg count
                                          properties, methods,
                                          0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool SFVec2f::toJsval(const vec2f & vec2f,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const sfvec2fObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfvec2fObj) { return JS_FALSE; }

    const SFData * const sfdata =
            static_cast<SFData *>(JS_GetPrivate(cx, sfvec2fObj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));

    //
    // Assignment should not throw.
    //
    static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue()).value = vec2f;
    *rval = OBJECT_TO_JSVAL(sfvec2fObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::sfvec2f>
SFVec2f::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFVec2f::jsclass, 0)) {
        throw BadConversion("SFVec2f object expected.");
    }
    const SFData * const sfdata =
            static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::sfvec2f>
            (static_cast<OpenVRML::sfvec2f *>
                (sfdata->getFieldValue().clone().release()));
}

JSBool SFVec2f::constructor(JSContext * const cx, JSObject * obj,
                            const uintN argc, jsval * const argv,
                            jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool SFVec2f::initObject(JSContext * const cx, JSObject * const obj,
                           const uintN argc, jsval * const argv)
    throw ()
{
    jsdouble x = 0.0, y = 0.0;
    if (argc > 0) {
        if (!JSVAL_IS_NUMBER(argv[0]) || !JS_ValueToNumber(cx, argv[0], &x)) {
            return JS_FALSE;
        }
    }
    if (argc > 1) {
        if (!JSVAL_IS_NUMBER(argv[1]) || !JS_ValueToNumber(cx, argv[1], &y)) {
            return JS_FALSE;
        }
    }

    try {
        std::auto_ptr<OpenVRML::sfvec2f>
                sfvec2f(new OpenVRML::sfvec2f(vec2f(x, y)));
        std::auto_ptr<SFData> sfdata(new SFData(sfvec2f.get()));
        sfvec2f.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
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
    throw ()
{
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & thisVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 1) { return JS_FALSE; }
        if (!JS_NewDoubleValue(cx, thisVec.value[JSVAL_TO_INT(id)], rval)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool SFVec2f::setProperty(JSContext * const cx, JSObject * const obj,
                            const jsval id, jsval * const vp) throw () {
    SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec2f & thisVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());

    if (!JSVAL_IS_INT(id)) { return JS_FALSE; }

    if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 1) { return JS_FALSE; }

    jsdouble d;
    if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

    thisVec.value[JSVAL_TO_INT(id)] = d;
    sfdata->changed = true;
    return JS_TRUE;
}

JSBool SFVec2f::add(JSContext * const cx, JSObject * const obj,
                    const uintN argc, jsval * const argv,
                    jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & thisVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Make sure our argument is an SFVec2f.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFVec2f::jsclass, 0)) {
        return JS_FALSE;
    }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & argVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec2f & resultVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());

    resultVec.value = thisVec.value + argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::divide(JSContext * const cx, JSObject * const obj,
                       const uintN argc, jsval * const argv,
                       jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & thisVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Make sure our argument is a number.
    //
    jsdouble divisor = 0.0;
    if (!JSVAL_IS_NUMBER(argv[0]) || !JS_ValueToNumber(cx, argv[0], &divisor)) {
        return JS_FALSE;
    }

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec2f & resultVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());

    resultVec.value = thisVec.value / divisor;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::dot(JSContext * const cx, JSObject * const obj,
                    const uintN argc, jsval * const argv,
                    jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & thisVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Make sure our argument is an SFVec2f.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFVec2f::jsclass, 0)) {
        return JS_FALSE;
    }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & argVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    *rval = DOUBLE_TO_JSVAL(thisVec.value.dot(argVec.value));
    return JS_TRUE;
}

JSBool SFVec2f::length(JSContext * const cx, JSObject * const obj,
                       uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & thisVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());

    if (!JS_NewDoubleValue(cx, thisVec.value.length(), rval)) {
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool SFVec2f::multiply(JSContext * const cx, JSObject * const obj,
                         const uintN argc, jsval * const argv,
                         jsval * const rval) throw () {
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & thisVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Make sure our argument is a number.
    //
    jsdouble factor = 0.0;
    if (!JSVAL_IS_NUMBER(argv[0]) || !JS_ValueToNumber(cx, argv[0], &factor)) {
        return JS_FALSE;
    }

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec2f & resultVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());

    resultVec.value = thisVec.value * factor;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::negate(JSContext * const cx, JSObject * const obj,
                       uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & thisVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec2f & resultVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());

    resultVec.value = -thisVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::normalize(JSContext * const cx, JSObject * const obj,
                          uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & thisVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec2f & resultVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());

    resultVec.value = thisVec.value.normalize();

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::subtract(JSContext * const cx, JSObject * const obj,
                         const uintN argc, jsval * const argv,
                         jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & thisVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Make sure our argument is an SFVec2f.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFVec2f::jsclass, 0)) {
        return JS_FALSE;
    }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec2f & argVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec2f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec2f & resultVec =
            static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());

    resultVec.value = thisVec.value - argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSClass SFVec3f::jsclass =
        { "SFVec3f", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub, getProperty, setProperty,
          JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, finalize };

JSObject * SFVec3f::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
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

    JSObject * const proto = JS_InitClass(cx, obj, 0, &jsclass,
                                          constructor, 0, // constructor function, min arg count
                                          properties, methods,
                                          0, 0); // static properties and methods
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool SFVec3f::toJsval(const vec3f & vec3f,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const sfvec3fObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfvec3fObj) { return JS_FALSE; }

    const SFData * const sfdata =
        static_cast<SFData *>(JS_GetPrivate(cx, sfvec3fObj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));

    //
    // Assignment should not throw.
    //
    static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue()).value = vec3f;

    *rval = OBJECT_TO_JSVAL(sfvec3fObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::sfvec3f>
SFVec3f::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFVec3f::jsclass, 0)) {
        throw BadConversion("SFVec3f object expected.");
    }
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::sfvec3f>
            (static_cast<OpenVRML::sfvec3f *>
                (sfdata->getFieldValue().clone().release()));
}

JSBool SFVec3f::constructor(JSContext * const cx, JSObject * obj,
                            const uintN argc, jsval * const argv,
                            jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool SFVec3f::initObject(JSContext * const cx, JSObject * obj,
                           const uintN argc, jsval * const argv)
        throw () {
    jsdouble vec[] = { 0.0, 0.0, 0.0 };
    for (uintN i(0); i < ((argc < 3) ? argc : 3); ++i) {
        if (!JSVAL_IS_NUMBER(argv[i])) { return JS_FALSE; }
        if (!JS_ValueToNumber(cx, argv[i], vec + i)) { return JS_FALSE; }
    }

    try {
        std::auto_ptr<OpenVRML::sfvec3f>
                sfvec3f(new OpenVRML::sfvec3f(vec3f(vec[0], vec[1], vec[2])));
        std::auto_ptr<SFData> sfdata(new SFData(sfvec3f.get()));
        sfvec3f.release();
        if (!JS_SetPrivate(cx, obj, sfdata.get())) { return JS_FALSE; }
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
                            const jsval id, jsval * const vp)
    throw ()
{
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & thisVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) { return JS_FALSE; }
        if (!JS_NewDoubleValue(cx, thisVec.value[JSVAL_TO_INT(id)], vp)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool SFVec3f::setProperty(JSContext * const cx, JSObject * const obj,
                            const jsval id, jsval * const vp) throw () {
    SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & thisVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    if (!JSVAL_IS_INT(id)) { return JS_FALSE; }
    if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) { return JS_FALSE; }

    jsdouble d;
    if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }
    thisVec.value[JSVAL_TO_INT(id)] = d;
    sfdata->changed = true;
    return JS_TRUE;
}

JSBool SFVec3f::add(JSContext * const cx, JSObject * const obj,
                    const uintN argc, jsval * const argv,
                    jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & thisVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Make sure our argument is an SFVec2f.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFVec3f::jsclass, 0)) {
        return JS_FALSE;
    }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & argVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & resultVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    resultVec.value = thisVec.value + argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::cross(JSContext * const cx, JSObject * const obj,
                      const uintN argc, jsval * const argv,
                      jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & thisVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Make sure our argument is an SFVec3f.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFVec3f::jsclass, 0)) {
        return JS_FALSE;
    }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & argVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) {return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & resultVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    resultVec.value = thisVec.value * argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::divide(JSContext * const cx, JSObject * const obj,
                       const uintN argc, jsval * const argv,
                       jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & thisVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
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
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & resultVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    resultVec.value = thisVec.value / divisor;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::dot(JSContext * const cx, JSObject * const obj,
                    const uintN argc, jsval * const argv,
                    jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & thisVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Make sure our argument is an SFVec3f.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFVec3f::jsclass, 0)) {
        return JS_FALSE;
    }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & argVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    *rval = DOUBLE_TO_JSVAL(thisVec.value.dot(argVec.value));
    return JS_TRUE;
}

JSBool SFVec3f::length(JSContext * const cx, JSObject * const obj,
                       uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & thisVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    if (!JS_NewDoubleValue(cx, thisVec.value.length(), rval)) {
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool SFVec3f::multiply(JSContext * const cx, JSObject * const obj,
                       const uintN argc, jsval * const argv,
                       jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & thisVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
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
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & resultVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    resultVec.value = thisVec.value * factor;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::negate(JSContext * const cx, JSObject * const obj,
                       uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & thisVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & resultVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    resultVec.value = -thisVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::normalize(JSContext * const cx, JSObject * const obj,
                          uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & thisVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & resultVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    resultVec.value = thisVec.value.normalize();

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::subtract(JSContext * const cx, JSObject * const obj,
                         const uintN argc, jsval * const argv,
                         jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & thisVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Make sure our argument is an SFVec2f.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFVec3f::jsclass, 0)) {
        return JS_FALSE;
    }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & argVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & resultVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    resultVec.value = thisVec.value - argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}


MField::MFData::MFData(JsvalArray::size_type size):
    array(size)
{}

MField::MFData::~MFData() {}

void MField::AddRoots(JSContext * const cx, JsvalArray & jsvalArray)
    throw (std::bad_alloc)
{
    size_t i;
    try {
        for (i = 0; i < jsvalArray.size(); ++i) {
            if (!JS_AddRoot(cx, &jsvalArray[i])) {
                throw std::bad_alloc();
            }
        }
    } catch (std::bad_alloc &) {
        for (size_t j = 0; j < i - 1; ++j) {
            JS_RemoveRoot(cx, &jsvalArray[j]);
        }
        throw;
    }
}

void MField::RemoveRoots(JSContext * const cx, JsvalArray & jsvalArray) throw ()
{
    for (size_t i = 0; i < jsvalArray.size(); ++i) {
        const JSBool ok = JS_RemoveRoot(cx, &jsvalArray[i]);
        assert(ok);
    }
}

JSBool MField::getElement(JSContext * const cx, JSObject * const obj,
                          const jsval id, jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(vp);
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0
                || size_t(JSVAL_TO_INT(id)) >= mfdata->array.size()) {
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
    const MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    *vp = INT_TO_JSVAL(mfdata->array.size());
    return JS_TRUE;
}


template <typename Subclass>
JSObject * MFJSObject<Subclass>::initClass(JSContext * const cx,
                                           JSObject * const obj)
    throw ()
{
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
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

template <typename Subclass>
JSBool MFJSObject<Subclass>::construct(JSContext * const cx,
                                       JSObject * obj,
	                               const uintN argc,
                                       jsval * const argv,
                                       jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &Subclass::jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

template <typename Subclass>
JSBool MFJSObject<Subclass>::initObject(JSContext * const cx,
                                        JSObject * const obj,
                                        const uintN argc,
                                        jsval * const argv)
    throw ()
{
    assert(cx);
    assert(obj);
    try {
        std::auto_ptr<MFData> mfdata(new MFData(argc));

        for (uintN i = 0; i < argc; ++i) {
            if (!JSVAL_IS_OBJECT(argv[i])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[i]),
                                      &Subclass::sfjsclass, 0)) {
                return JS_FALSE;
            }
            mfdata->array[i] = argv[i];
        }

        //
        // Protect array values from gc.
        //
        AddRoots(cx, mfdata->array);

        if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
        mfdata.release();
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSObject<Subclass>::setElement(JSContext * const cx,
                                        JSObject * const obj,
                                        const jsval id,
                                        jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(vp);
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    //
    // Make sure new value is of the correct type.
    //
    if (!JSVAL_IS_OBJECT(*vp)
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(*vp),
                              &Subclass::sfjsclass, 0)) {
        return JS_FALSE;
    }

    //
    // Make sure the index is valid.
    //
    if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) { return JS_FALSE; }

    //
    // Grow array if necessary.
    //
    if (size_t(JSVAL_TO_INT(id)) >= mfdata->array.size()) {
        jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
        if (!setLength(cx, obj, 0, &newLength)) { return JS_FALSE; }
    }

    //
    // Put the new element in the array.
    //
    mfdata->array[JSVAL_TO_INT(id)] = *vp;
    mfdata->changed = true;
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSObject<Subclass>::setLength(JSContext * const cx,
                                       JSObject * const obj,
                                       jsval, jsval * const vp)
        throw () {
    using std::copy;

    assert(cx);
    assert(obj);
    assert(vp);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) { return JS_FALSE; }

    if (size_t(JSVAL_TO_INT(*vp)) == mfdata->array.size()) {
        mfdata->changed = true;
        return JS_TRUE; // Nothing to do.
    }

    try {
        JsvalArray newArray(JSVAL_TO_INT(*vp));
        AddRoots(cx, newArray); // Protect from gc.

        //
        // Copy the values from the old array.
        //
        const size_t length = newArray.size() < mfdata->array.size()
                            ? newArray.size()
                            : mfdata->array.size();
        copy(mfdata->array.begin(), mfdata->array.begin() + length,
             newArray.begin());

        //
        // Initialize the rest of the array with new values.
        //
        try {
            for (size_t i = length; i < newArray.size(); ++i) {
                JSObject * const element =
                        JS_ConstructObject(cx, &Subclass::sfjsclass, 0, 0);
                if (!element) { throw std::bad_alloc(); }
                newArray[i] = OBJECT_TO_JSVAL(element);
            }
        } catch (std::bad_alloc &) {
            RemoveRoots(cx, newArray);
            throw;
        }

        //
        // Remove roots for the values in the old array.
        //
        RemoveRoots(cx, mfdata->array);

        //
        // Finally, swap the new vector with the old one.
        //
        std::swap(mfdata->array, newArray);
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    mfdata->changed = true;
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSObject<Subclass>::toString(JSContext * const cx,
                                      JSObject * const obj,
                                      uintN, jsval *,
                                      jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    std::ostringstream out;
    out << '[';
    for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        const SField::SFData * const sfdata =
                static_cast<SField::SFData *>
                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);
        out << sfdata->getFieldValue();
        if ((i + 1) < mfdata->array.size()) { out << ", "; }
    }
    out << ']';

    JSString * jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

template <typename Subclass>
void MFJSObject<Subclass>::finalize(JSContext * const cx, JSObject * const obj)
    throw ()
{
    assert(cx);
    assert(obj);
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    if (mfdata) {
        RemoveRoots(cx, mfdata->array);
        delete mfdata;
        JS_SetPrivate(cx, obj, 0);
    }
}


template <typename Subclass>
JSObject * MFJSDouble<Subclass>::initClass(JSContext * const cx,
                                           JSObject * const obj)
    throw ()
{
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
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

template <typename Subclass>
JSBool MFJSDouble<Subclass>::construct(JSContext * const cx,
                                       JSObject * obj,
	                               const uintN argc,
                                       jsval * const argv,
                                       jsval * rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &Subclass::jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

template <typename Subclass>
JSBool MFJSDouble<Subclass>::initObject(JSContext * const cx,
                                        JSObject * const obj,
                                        const uintN argc,
                                        jsval * const argv)
    throw ()
{
    try {
        std::auto_ptr<MFData> mfdata(new MFData(argc));
        for (uintN i = 0; i < argc; ++i) {
            if (!JSVAL_IS_NUMBER(argv[i])) { return JS_FALSE; }

            //
            // If the value is an int, convert it to a double.
            //
            if (JSVAL_IS_INT(argv[i])) {
                if (!JS_NewDoubleValue(cx, JSVAL_TO_INT(argv[i]),
                                       &mfdata->array[i])) {
                     throw std::bad_alloc();
                }
            } else {
                assert(JSVAL_IS_DOUBLE(argv[i]));
                mfdata->array[i] = argv[i];
            }
        }

        //
        // Protect array values from gc.
        //
        AddRoots(cx, mfdata->array);

        if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
        mfdata.release();
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSDouble<Subclass>::setElement(JSContext * const cx,
                                        JSObject * const obj,
                                        const jsval id,
                                        jsval * const vp) throw () {
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    //
    // Make sure new value is a number.
    //
    if (!JSVAL_IS_NUMBER(*vp)) { return JS_FALSE; }

    //
    // Make sure the index is valid.
    //
    if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) { return JS_FALSE; }

    //
    // Grow array if necessary.
    //
    if (size_t(JSVAL_TO_INT(id)) >= mfdata->array.size()) {
        jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
        if (!setLength(cx, obj, 0, &newLength)) { return JS_FALSE; }
    }

    //
    // Put the new element in the array.
    //
    if (JSVAL_IS_INT(*vp)) {
        if (!JS_NewDoubleValue(cx, JSVAL_TO_INT(*vp),
                               &mfdata->array[JSVAL_TO_INT(id)])) {
            return JS_FALSE;
        }
    } else {
        assert(JSVAL_IS_DOUBLE(*vp));
        mfdata->array[JSVAL_TO_INT(id)] = *vp;
    }
    mfdata->changed = true;
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSDouble<Subclass>::setLength(JSContext * const cx,
                                       JSObject * const obj,
                                       const jsval id,
                                       jsval * const vp)
    throw ()
{
    using std::copy;

    assert(cx);
    assert(obj);
    assert(vp);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) { return JS_FALSE; }

    if (size_t(JSVAL_TO_INT(*vp)) == mfdata->array.size()) {
        mfdata->changed = true;
        return JS_TRUE; // Nothing to do.
    }

    try {
        JsvalArray newArray(JSVAL_TO_INT(*vp));
        AddRoots(cx, newArray); // Protect from gc.

        //
        // Copy the values from the old array.
        //
        const size_t length = newArray.size() < mfdata->array.size()
                            ? newArray.size()
                            : mfdata->array.size();
        copy(mfdata->array.begin(), mfdata->array.begin() + length,
             newArray.begin());

        //
        // Initialize the rest of the array with new values.
        //
        try {
            for (size_t i = length; i < newArray.size(); ++i) {
                if (!JS_NewDoubleValue(cx, 0.0, &newArray[i])) {
                    throw std::bad_alloc();
                }
            }
        } catch (std::bad_alloc &) {
            RemoveRoots(cx, newArray);
            throw;
        }

        //
        // Remove roots for the values in the old array.
        //
        RemoveRoots(cx, mfdata->array);

        //
        // Finally, swap the new vector with the old one.
        //
        std::swap(mfdata->array, newArray);
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    mfdata->changed = true;
    return JS_TRUE;
}

template <typename Subclass>
JSBool MFJSDouble<Subclass>::toString(JSContext * const cx,
                                      JSObject * const obj,
                                      uintN, jsval *,
                                      jsval * const rval)
    throw ()
{
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    std::ostringstream out;
    out << '[';
    for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
        out << *JSVAL_TO_DOUBLE(mfdata->array[i]);
        if ((i + 1) < mfdata->array.size()) { out << ", "; }
    }
    out << ']';

    JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

template <typename Subclass>
void MFJSDouble<Subclass>::finalize(JSContext * const cx, JSObject * const obj)
    throw ()
{
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    if (mfdata) {
        RemoveRoots(cx, mfdata->array);
        delete mfdata;
        JS_SetPrivate(cx, obj, 0);
    }
}

JSClass MFColor::jsclass =
        { "MFColor", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub,
          getElement, setElement,
          JS_EnumerateStub, JS_ResolveStub,
          JS_ConvertStub, finalize };

JSClass & MFColor::sfjsclass = SFColor::jsclass;

std::auto_ptr<OpenVRML::mfcolor>
MFColor::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFColor::jsclass, 0)) {
        throw BadConversion("MFColor object expected.");
    }

    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<OpenVRML::mfcolor>
            mfcolor(new OpenVRML::mfcolor(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFColor::jsclass, 0));
        const SField::SFData * const sfdata =
                static_cast<SField::SFData *>
                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const OpenVRML::sfcolor & sfcolor =
                static_cast<OpenVRML::sfcolor &>(sfdata->getFieldValue());
        mfcolor->value[i] = sfcolor.value;
    }
    return mfcolor;
}

JSBool MFColor::toJsval(const std::vector<color> & colors,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const mfcolorObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfcolorObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(colors.size());
    if (!setLength(cx, mfcolorObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfcolorObj));
    for (size_t i = 0; i < colors.size(); ++i) {
        if (!SFColor::toJsval(colors[i], cx, obj, &mfdata->array[i])) {
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

JSBool MFFloat::toJsval(const std::vector<float> & floats,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    JSObject * const mffloatObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mffloatObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(floats.size());
    if (!setLength(cx, mffloatObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mffloatObj));
    for (size_t i = 0; i < floats.size(); ++i) {
        if (!JS_NewDoubleValue(cx, floats[i], &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mffloatObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::mffloat>
MFFloat::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFFloat::jsclass, 0)) {
        throw BadConversion("MFFloat object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<OpenVRML::mffloat>
            mffloat(new OpenVRML::mffloat(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
        mffloat->value[i] = *JSVAL_TO_DOUBLE(mfdata->array[i]);
    }
    return mffloat;
}

JSClass MFInt32::jsclass =
        { "MFFloat", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub,
          getElement, setElement,
          JS_EnumerateStub, JS_ResolveStub,
          JS_ConvertStub, finalize };

JSObject * MFInt32::initClass(JSContext * const cx, JSObject * const obj) {
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
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool MFInt32::toJsval(const std::vector<OpenVRML::int32> & int32s,
                        JSContext * const cx,
                        JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    JSObject * const mfint32Obj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfint32Obj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(int32s.size());
    if (!setLength(cx, mfint32Obj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfint32Obj));
    for (size_t i = 0; i < int32s.size(); ++i) {
        mfdata->array[i] = INT_TO_JSVAL(int32s[i]);
    }
    *rval = OBJECT_TO_JSVAL(mfint32Obj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::mfint32>
MFInt32::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFInt32::jsclass, 0)) {
        throw BadConversion("MFInt32 object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<OpenVRML::mfint32>
            mfint32(new OpenVRML::mfint32(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_INT(mfdata->array[i]));
        mfint32->value[i] = JSVAL_TO_INT(mfdata->array[i]);
    }
    return mfint32;
}

JSBool MFInt32::construct(JSContext * const cx, JSObject * obj,
	                  const uintN argc, jsval * const argv,
                          jsval * const rval)
{
    assert(cx);
    assert(obj);
    assert(rval);

    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool MFInt32::initObject(JSContext * const cx, JSObject * const obj,
                           const uintN argc, jsval * const argv)
{
    assert(cx);
    assert(obj);

    try {
        std::auto_ptr<MFData> mfdata(new MFData(argc));
        for (uintN i = 0; i < argc; ++i) {
            //
            // Convert the jsval to an int32 and back to a jsval in order
            // to remove any decimal part.
            //
            ::int32 integer;
            if (!JS_ValueToECMAInt32(cx, argv[i], &integer)) {
                return JS_FALSE;
            }
            if (!JS_NewNumberValue(cx, jsdouble(integer), &mfdata->array[i])) {
                return JS_FALSE;
            }
        }
        if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
        mfdata.release();
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool MFInt32::setElement(JSContext * const cx, JSObject * const obj,
                           const jsval id, jsval * const vp)
{
    assert(cx);
    assert(obj);
    assert(vp);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    //
    // Make sure the index is valid.
    //
    if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) { return JS_FALSE; }

    //
    // Grow array if necessary.
    //
    if (size_t(JSVAL_TO_INT(id)) >= mfdata->array.size()) {
        jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
        if (!setLength(cx, obj, 0, &newLength)) { return JS_FALSE; }
    }

    //
    // Convert the jsval to an int32 and back to a jsval in order
    // to remove any decimal part.
    //
    ::int32 i;
    if (!JS_ValueToECMAInt32(cx, *vp, &i)) { return JS_FALSE; }
    if (!JS_NewNumberValue(cx, jsdouble(i), &mfdata->array[JSVAL_TO_INT(id)])) {
        return JS_FALSE;
    }
    mfdata->changed = true;
    return JS_TRUE;
}

JSBool MFInt32::setLength(JSContext * const cx, JSObject * const obj,
                          const jsval id, jsval * const vp)
{
    assert(cx);
    assert(obj);
    assert(vp);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) { return JS_FALSE; }

    try {
        if (size_t(JSVAL_TO_INT(*vp)) != mfdata->array.size()) {
            mfdata->array.resize(JSVAL_TO_INT(*vp));
        }
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    mfdata->changed = true;
    return JS_TRUE;
}

JSBool MFInt32::toString(JSContext * const cx, JSObject * const obj,
                         uintN, jsval *, jsval * const rval) {
    assert(cx);
    assert(obj);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    std::ostringstream out;
    out << '[';
    for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        out << JSVAL_TO_INT(mfdata->array[i]);
        if ((i + 1) < mfdata->array.size()) { out << ", "; }
    }
    out << ']';

    JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

void MFInt32::finalize(JSContext * const cx, JSObject * const obj) {
    delete static_cast<MFData *>(JS_GetPrivate(cx, obj));
    JS_SetPrivate(cx, obj, 0);
}

JSClass MFNode::jsclass =
        { "MFNode", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub,
          getElement, setElement,
          JS_EnumerateStub, JS_ResolveStub,
          JS_ConvertStub, finalize };

JSObject * MFNode::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
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
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool MFNode::initObject(JSContext * const cx, JSObject * const obj,
                          const uintN argc, jsval * const argv)
    throw ()
{
    assert(cx);
    assert(obj);

    try {
        std::auto_ptr<MFData> mfdata(new MFData(argc));
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
        if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
        mfdata.release();
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool MFNode::toJsval(const std::vector<NodePtr> & nodes,
                       JSContext * const cx,
                       JSObject * const obj,
                       jsval * const rval)
    throw ()
{
    JSObject * const mfnodeObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfnodeObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(nodes.size());
    if (!setLength(cx, mfnodeObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, mfnodeObj));
    for (size_t i = 0; i < nodes.size(); ++i) {
        if (!SFNode::toJsval(nodes[i], cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfnodeObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::mfnode>
MFNode::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFNode::jsclass, 0)) {
        throw BadConversion("MFNode object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<OpenVRML::mfnode>
            mfnode(new OpenVRML::mfnode(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFNode::jsclass, 0));
        const SField::SFData * const sfdata =
            static_cast<SField::SFData *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const OpenVRML::sfnode & sfnode =
                static_cast<OpenVRML::sfnode &>(sfdata->getFieldValue());
        mfnode->value[i] = sfnode.value;
    }
    return mfnode;
}

JSBool MFNode::construct(JSContext * const cx, JSObject * obj,
                         const uintN argc, jsval * const argv,
                         jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool MFNode::setElement(JSContext * const cx, JSObject * const obj,
                          const jsval id, jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(vp);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    //
    // Make sure new value is an SFNode.
    //
    if (!JSVAL_IS_OBJECT(*vp)
            || !JS_InstanceOf(cx, obj, &SFNode::jsclass, 0)) {
        return JS_FALSE;
    }

    if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) { return JS_FALSE; }

    //
    // Grow array if necessary.
    //
    if (size_t(JSVAL_TO_INT(id)) >= mfdata->array.size()) {
        jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
        if (!setLength(cx, obj, 0, &newLength)) { return JS_FALSE; }
    }

    //
    // Put the new element in the array.
    //
    mfdata->array[JSVAL_TO_INT(id)] = *vp;
    mfdata->changed = true;
    return JS_TRUE;
}

JSBool MFNode::setLength(JSContext * const cx, JSObject * const obj,
                         const jsval id, jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(vp);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) { return JS_FALSE; }

    try {
        JsvalArray newArray(JSVAL_TO_INT(*vp));
        AddRoots(cx, newArray); // Protect from gc.

        //
        // Copy the values from the old array.
        //
        const size_t length = newArray.size() < mfdata->array.size()
                            ? newArray.size()
                            : mfdata->array.size();
        std::copy(mfdata->array.begin(), mfdata->array.begin() + length,
                  newArray.begin());

        //
        // Initialize the rest of the array with new values.
        //
        try {
            jsval arg = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "Group {}"));

            for (size_t i = length; i < newArray.size(); ++i) {
                JSObject * const element =
                    JS_ConstructObjectWithArguments(cx, &SFNode::jsclass, 0, 0,
                                                    1, &arg);
                if (!element) { throw std::bad_alloc(); }
                newArray[i] = OBJECT_TO_JSVAL(element);
            }
        } catch (std::bad_alloc &) {
            RemoveRoots(cx, newArray);
            throw;
        }

        //
        // Remove roots for the values in the old array.
        //
        RemoveRoots(cx, mfdata->array);

        //
        // Finally, swap the new vector with the old one.
        //
        std::swap(mfdata->array, newArray);
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    mfdata->changed = true;
    return JS_TRUE;
}

JSBool MFNode::toString(JSContext * const cx, JSObject * const obj,
                        const uintN argc, jsval * const argv,
                        jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    std::ostringstream out;
    out << '[';
    for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        const SField::SFData * const sfdata =
            static_cast<SField::SFData *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);
        out << sfdata->getFieldValue();
        if ((i + 1) < mfdata->array.size()) { out << ", "; }
    }
    out << ']';

    JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

void MFNode::finalize(JSContext * const cx, JSObject * const obj)
    throw ()
{
    assert(cx);
    assert(obj);
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    if (mfdata) {
        RemoveRoots(cx, mfdata->array);
        delete mfdata;
        JS_SetPrivate(cx, obj, 0);
    }
}

JSClass MFRotation::jsclass =
        { "MFRotation", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub,
          getElement, setElement,
          JS_EnumerateStub, JS_ResolveStub,
          JS_ConvertStub, finalize };

JSClass & MFRotation::sfjsclass = SFRotation::jsclass;

JSBool MFRotation::toJsval(const std::vector<rotation> & rotations,
                           JSContext * const cx, JSObject * const obj,
                           jsval * const rval)
    throw ()
{
    JSObject * const mfrotationObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfrotationObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(rotations.size());
    if (!setLength(cx, mfrotationObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfrotationObj));
    for (size_t i = 0; i < rotations.size(); ++i) {
        if (!SFRotation::toJsval(rotations[i], cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfrotationObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::mfrotation>
MFRotation::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFRotation::jsclass, 0)) {
        throw BadConversion("MFRotation object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<OpenVRML::mfrotation>
            mfrotation(new OpenVRML::mfrotation(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFRotation::jsclass, 0));
        const SField::SFData * const sfdata =
            static_cast<SField::SFData *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const OpenVRML::sfrotation & sfrotation =
                static_cast<OpenVRML::sfrotation &>(sfdata->getFieldValue());
        mfrotation->value[i] = sfrotation.value;
    }
    return mfrotation;
}

JSClass MFString::jsclass =
        { "MFString", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub,
          getElement, setElement,
          JS_EnumerateStub, JS_ResolveStub,
          JS_ConvertStub, finalize };

JSObject * MFString::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
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
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool MFString::toJsval(const std::vector<std::string> & strings,
                         JSContext * const cx, JSObject * const obj,
                         jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);
    JSObject * const mfstringObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfstringObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(strings.size());
    if (!setLength(cx, mfstringObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfstringObj));
    for (size_t i = 0; i < strings.size(); ++i) {
        JSString * jsstring = JS_NewStringCopyZ(cx, strings[i].c_str());
        if (!jsstring) { return JS_FALSE; }
        mfdata->array[i] = STRING_TO_JSVAL(jsstring);
    }
    *rval = OBJECT_TO_JSVAL(mfstringObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::mfstring>
MFString::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFString::jsclass, 0)) {
        throw BadConversion("MFString object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<OpenVRML::mfstring>
            mfstring(new OpenVRML::mfstring(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_STRING(mfdata->array[i]));
        const char * const str =
                JS_GetStringBytes(JSVAL_TO_STRING(mfdata->array[i]));
        mfstring->value[i] = str;
    }
    return mfstring;
}

JSBool MFString::construct(JSContext * const cx, JSObject * obj,
	                   const uintN argc, jsval * const argv,
                           jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool MFString::initObject(JSContext * const cx, JSObject * const obj,
                            const uintN argc, jsval * const argv)
    throw ()
{
    assert(cx);
    assert(obj);

    try {
        std::auto_ptr<MFData> mfdata(new MFData(argc));
        for (uintN i = 0; i < argc; ++i) {
            JSString * const str = JS_ValueToString(cx, argv[i]);
            if (!str) { return JS_FALSE; }
            mfdata->array[i] = STRING_TO_JSVAL(str);
        }

        //
        // Protect array values from gc.
        //
        AddRoots(cx, mfdata->array);

        if (!JS_SetPrivate(cx, obj, mfdata.get())) { return JS_FALSE; }
        mfdata.release();
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool MFString::setElement(JSContext * const cx, JSObject * const obj,
                            const jsval id, jsval * const vp)
    throw ()
{
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    //
    // Make sure the index is valid.
    //
    if (!JSVAL_IS_INT(id) || JSVAL_TO_INT(id) < 0) { return JS_FALSE; }

    //
    // Grow array if necessary.
    //
    if (size_t(JSVAL_TO_INT(id)) >= mfdata->array.size()) {
        jsval newLength = INT_TO_JSVAL(JSVAL_TO_INT(id) + 1);
        if (!setLength(cx, obj, 0, &newLength)) { return JS_FALSE; }
    }

    //
    // Convert the value to a JSString, if necessary.
    //
    JSString * const str = JS_ValueToString(cx, *vp);
    if (!str) { return JS_FALSE; }
    
    //
    // Put the new element in the array.
    //
    mfdata->array[JSVAL_TO_INT(id)] = STRING_TO_JSVAL(str);
    mfdata->changed = true;
    return JS_TRUE;
}

JSBool MFString::setLength(JSContext * const cx, JSObject * const obj,
                           const jsval id, jsval * const vp)
    throw ()
{
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    if (!JSVAL_IS_INT(*vp) || JSVAL_TO_INT(*vp) < 0) { return JS_FALSE; }

    try {
        JsvalArray newArray(JSVAL_TO_INT(*vp));
        AddRoots(cx, newArray); // Protect from gc.

        //
        // Copy the values from the old array.
        //
        const size_t length = newArray.size() < mfdata->array.size()
                            ? newArray.size()
                            : mfdata->array.size();
        std::copy(mfdata->array.begin(), mfdata->array.begin() + length,
                  newArray.begin());

        //
        // Initialize the rest of the array with new values.
        //
        try {
            for (size_t i = length; i < newArray.size(); ++i) {
                JSString * jsstring = JS_NewStringCopyZ(cx, "");
                if (!jsstring) { throw std::bad_alloc(); }
                newArray[i] = STRING_TO_JSVAL(jsstring);
            }
        } catch (std::bad_alloc &) {
            RemoveRoots(cx, newArray);
            throw;
        }

        //
        // Remove roots for the values in the old array.
        //
        RemoveRoots(cx, mfdata->array);

        //
        // Finally, swap the new vector with the old one.
        //
        std::swap(mfdata->array, newArray);
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    mfdata->changed = true;
    return JS_TRUE;
}

JSBool MFString::toString(JSContext * const cx, JSObject * const obj,
                          uintN, jsval *, jsval * const rval)
    throw ()
{
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);

    std::ostringstream out;
    out << '[';
    for (JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_STRING(mfdata->array[i]));
        out << '\"'
            << JS_GetStringBytes(JSVAL_TO_STRING(mfdata->array[i]))
            << '\"';
        if ((i + 1) < mfdata->array.size()) { out << ", "; }
    }
    out << ']';

    JSString * const jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
    if (!jsstr) { return JS_FALSE; }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

void MFString::finalize(JSContext * const cx, JSObject * const obj)
    throw ()
{
    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, obj));
    if (mfdata) {
        RemoveRoots(cx, mfdata->array);
        delete mfdata;
        JS_SetPrivate(cx, obj, 0);
    }
}

JSClass MFTime::jsclass =
        { "MFTime", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub,
          getElement, setElement,
          JS_EnumerateStub, JS_ResolveStub,
          JS_ConvertStub, finalize };

JSBool MFTime::toJsval(const std::vector<double> & times,
                       JSContext * const cx,
                       JSObject * const obj,
                       jsval * const rval)
    throw ()
{
    JSObject * const mftimeObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mftimeObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(times.size());
    if (!setLength(cx, mftimeObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, mftimeObj));
    for (size_t i = 0; i < times.size(); ++i) {
        if (!JS_NewDoubleValue(cx, times[i], &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mftimeObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::mftime>
MFTime::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);

    if (!JS_InstanceOf(cx, obj, &MFTime::jsclass, 0)) {
        throw BadConversion("MFTime object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<OpenVRML::mftime>
            mftime(new OpenVRML::mftime(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_DOUBLE(mfdata->array[i]));
        mftime->value[i] = *JSVAL_TO_DOUBLE(mfdata->array[i]);
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

JSBool MFVec2f::toJsval(const std::vector<vec2f> & vec2fs,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const mfvec2fObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfvec2fObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(vec2fs.size());
    if (!setLength(cx, mfvec2fObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfvec2fObj));
    for (size_t i = 0; i < vec2fs.size(); ++i) {
        if (!SFVec2f::toJsval(vec2fs[i], cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfvec2fObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::mfvec2f>
MFVec2f::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);
    if (!JS_InstanceOf(cx, obj, &MFVec2f::jsclass, 0)) {
        throw BadConversion("MFVec2f object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<OpenVRML::mfvec2f>
            mfvec2f(new OpenVRML::mfvec2f(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFVec2f::jsclass, 0));
        const SField::SFData * const sfdata =
                static_cast<SField::SFData *>
                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const OpenVRML::sfvec2f & sfvec2f =
                static_cast<OpenVRML::sfvec2f &>(sfdata->getFieldValue());
        mfvec2f->value[i] = sfvec2f.value;
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

JSBool MFVec3f::toJsval(const std::vector<vec3f> & vec3fs,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const mfvec3fObj =
            JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfvec3fObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(vec3fs.size());
    if (!setLength(cx, mfvec3fObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfvec3fObj));
    for (size_t i = 0; i < vec3fs.size(); ++i) {
        if (!SFVec3f::toJsval(vec3fs[i], cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfvec3fObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::mfvec3f>
MFVec3f::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    assert(cx);
    assert(obj);
    if (!JS_InstanceOf(cx, obj, &MFVec3f::jsclass, 0)) {
        throw BadConversion("MFVec3f object expected.");
    }
    MField::MFData * const mfdata =
            static_cast<MField::MFData *>(JS_GetPrivate(cx, obj));
    assert(mfdata);
    std::auto_ptr<OpenVRML::mfvec3f>
            mfvec3f(new OpenVRML::mfvec3f(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0;
            i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFVec3f::jsclass, 0));
        const SField::SFData * const sfdata =
            static_cast<SField::SFData *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const OpenVRML::sfvec3f & sfvec3f =
                static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
        mfvec3f->value[i] = sfvec3f.value;
    }

    return mfvec3f;
}

JSClass VrmlMatrix::Row::jsclass =
        { "VrmlMatrixRow_", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub,
          getElement, setElement,
          JS_EnumerateStub, JS_ResolveStub,
          JS_ConvertStub, JS_FinalizeStub };

JSObject * VrmlMatrix::Row::initClass(JSContext * const cx,
                                      JSObject * const obj)
    throw ()
{
    return JS_InitClass(cx, obj, 0, &jsclass,
                        construct, 0,
                        0, 0,
                        0, 0);
}

JSBool VrmlMatrix::Row::construct(JSContext * const cx, JSObject * obj,
                                  const uintN argc, jsval * const argv,
                                  jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return JS_TRUE;
}

JSBool VrmlMatrix::Row::getElement(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 3) {
            return JS_FALSE;
        }

        const float (*row)[4] =
                static_cast<float (*)[4]>(JS_GetPrivate(cx, obj));
        assert(row);

        if (!JS_NewDoubleValue(cx, (*row)[JSVAL_TO_INT(id)], vp)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool VrmlMatrix::Row::setElement(JSContext * const cx, JSObject * const obj,
                                   const jsval id, jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 3) {
            return JS_FALSE;
        }

        jsdouble d;
        if (!JSVAL_IS_NUMBER(*vp)
                || !JS_ValueToNumber(cx, *vp, &d)) {
            return JS_FALSE;
        }
        
        float (*row)[4] = static_cast<float (*)[4]>(JS_GetPrivate(cx, obj));
        assert(row);
        (*row)[JSVAL_TO_INT(id)] = d;
    }
    return JS_TRUE;
}

JSClass VrmlMatrix::jsclass =
        { "VrmlMatrix", JSCLASS_HAS_PRIVATE,
          JS_PropertyStub, JS_PropertyStub,
          getElement, setElement,
          JS_EnumerateStub, JS_ResolveStub,
          JS_ConvertStub, finalize };

JSObject * VrmlMatrix::initClass(JSContext * const cx, JSObject * const obj)
    throw ()
{
    static JSFunctionSpec methods[] =
            { { "setTransform", setTransform, 0, 0, 0 },
              { "getTransform", getTransform, 0, 0, 0 },
              { "inverse", inverse, 0, 0, 0 },
              { "transpose", transpose, 0, 0, 0 },
              { "multLeft", multLeft, 1, 0, 0 },
              { "multRight", multRight, 1, 0, 0 },
              { "multVecMatrix", multVecMatrix, 1, 0, 0 },
              { "multMatrixVec", multMatrixVec, 1, 0, 0 },
              { "toString", toString, 0, 0, 0 },
              { 0, 0, 0, 0, 0 } };

    JSObject * const proto = JS_InitClass(cx, obj, 0, &jsclass,
                                          construct, 0,
                                          0, methods,
                                          0, 0);
    if (!proto || !initObject(cx, proto, 0, 0)) { return 0; }
    return proto;
}

JSBool VrmlMatrix::construct(JSContext * const cx, JSObject * obj,
                             const uintN argc, jsval * const argv,
                             jsval * const rval)
    throw ()
{
    //
    // If called without new, replace obj with a new object.
    //
    if (!JS_IsConstructing(cx)) {
        obj = JS_NewObject(cx, &jsclass, 0, 0);
        if (!obj) { return JS_FALSE; }
        *rval = OBJECT_TO_JSVAL(obj);
    }
    return initObject(cx, obj, argc, argv);
}

JSBool VrmlMatrix::initObject(JSContext * const cx, JSObject * const obj,
                              uintN argc, jsval * const argv)
    throw ()
{
    assert(cx);
    assert(obj);

    argc = (argc < 16)
         ? argc
         : 16;
    try {
        std::auto_ptr<mat4f> mat(new mat4f);
        for (uintN i = 0; i < argc; ++i) {
            jsdouble d;
            if (!JSVAL_IS_NUMBER(argv[0])
                    || !JS_ValueToNumber(cx, argv[i], &d)) {
                return JS_FALSE;
            }
            (&(*mat)[0][0])[i] = d;
        }
        if (!JS_SetPrivate(cx, obj, mat.get())) { return JS_FALSE; }
        mat.release();
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    return JS_TRUE;
}

JSBool VrmlMatrix::getElement(JSContext * const cx, JSObject * const obj,
                              const jsval id, jsval * const vp)
    throw ()
{
    assert(cx);
    assert(obj);

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 3) {
            return JS_FALSE;
        }

        mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));

        //
        // Construct the result object.
        //
        JSObject * const robj = JS_ConstructObject(cx, &Row::jsclass, 0,
                                                   JS_GetParent(cx, obj));
        if (!robj) { return JS_FALSE; }
        
        float (&row)[4] = (*thisMat)[JSVAL_TO_INT(id)];
        if (!JS_SetPrivate(cx, robj, &row)) { return JS_FALSE; }   
        *vp = OBJECT_TO_JSVAL(robj);
    }
    return JS_TRUE;
}

JSBool VrmlMatrix::setElement(JSContext * const cx, JSObject * const obj,
                              const jsval id, jsval * const vp)
    throw ()
{
    return JS_TRUE;
}

JSBool VrmlMatrix::setTransform(JSContext * const cx, JSObject * const obj,
                                const uintN argc, jsval * const argv,
                                jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);

    using OpenVRML::sfvec3f;
    using OpenVRML::sfrotation;

    vec3f translation(0.0, 0.0, 0.0);
    rotation rot(0.0, 0.0, 1.0, 0.0);
    vec3f scale(1.0, 1.0, 1.0);
    rotation scaleOrientation(0.0, 0.0, 1.0, 0.0);
    vec3f center(0.0, 0.0, 0.0);
    
    for (uintN i = 0; i < argc; ++i) {
        const SField::SFData * sfdata = 0;
        switch (i) {
        case 0:
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &JavaScript_::SFVec3f::jsclass, 0)) {
                return JS_FALSE;
            }
            sfdata = static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<sfvec3f *>(&sfdata->getFieldValue()));
            translation = static_cast<sfvec3f &>(sfdata->getFieldValue()).value;
            break;

        case 1:
            if (!JSVAL_IS_OBJECT(argv[1])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[1]),
                                      &JavaScript_::SFRotation::jsclass, 0)) {
                return JS_FALSE;
            }
            sfdata = static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1])));
            assert(sfdata);
            assert(dynamic_cast<sfrotation *>(&sfdata->getFieldValue()));
            rot = static_cast<sfrotation &>(sfdata->getFieldValue()).value;
            break;

        case 2:
            if (!JSVAL_IS_OBJECT(argv[2])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[2]),
                                      &JavaScript_::SFVec3f::jsclass, 0)) {
                return JS_FALSE;
            }
            sfdata = static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[2])));
            assert(sfdata);
            assert(dynamic_cast<sfvec3f *>(&sfdata->getFieldValue()));
            scale = static_cast<sfvec3f &>(sfdata->getFieldValue()).value;
            break;

        case 3:
            if (!JSVAL_IS_OBJECT(argv[3])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[3]),
                                      &JavaScript_::SFRotation::jsclass, 0)) {
                return JS_FALSE;
            }
            sfdata = static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[3])));
            assert(sfdata);
            assert(dynamic_cast<sfrotation *>(&sfdata->getFieldValue()));
            scaleOrientation =
                    static_cast<sfrotation &>(sfdata->getFieldValue()).value;
            break;

        case 4:
            if (!JSVAL_IS_OBJECT(argv[4])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[4]),
                                      &JavaScript_::SFVec3f::jsclass, 0)) {
                return JS_FALSE;
            }
            sfdata = static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[4])));
            assert(sfdata);
            assert(dynamic_cast<sfvec3f *>(&sfdata->getFieldValue()));
            center = static_cast<sfvec3f &>(sfdata->getFieldValue()).value;
            break;

        default:
            break;
        }
    }

    mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    *thisMat = mat4f::transformation(translation,
                                     rot,
                                     scale,
                                     scaleOrientation,
                                     center);
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

JSBool VrmlMatrix::getTransform(JSContext * const cx, JSObject * const obj,
                                const uintN argc, jsval * const argv,
                                jsval * const rval)
    throw ()
{
    using OpenVRML::sfvec3f;
    using OpenVRML::sfrotation;

    assert(cx);
    assert(obj);
    
    vec3f translation;
    rotation rot;
    vec3f scale;

    const mat4f * const thisMat =
            static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    thisMat->transformation(translation, rot, scale);
    
    for (uintN i = 0; i < argc; ++i) {
        if (JSVAL_IS_NULL(argv[i])) { continue; }
        const SField::SFData * sfdata = 0;
        switch (i) {
        case 0:
            if (!JSVAL_IS_OBJECT(argv[0])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                                      &JavaScript_::SFVec3f::jsclass, 0)) {
                return JS_FALSE;
            }
            sfdata = static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
            assert(sfdata);
            assert(dynamic_cast<sfvec3f *>(&sfdata->getFieldValue()));
            static_cast<sfvec3f &>(sfdata->getFieldValue()).value = translation;
            break;

        case 1:
            if (!JSVAL_IS_OBJECT(argv[1])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[1]),
                                      &JavaScript_::SFRotation::jsclass, 0)) {
                return JS_FALSE;
            }
            sfdata = static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1])));
            assert(sfdata);
            assert(dynamic_cast<sfrotation *>(&sfdata->getFieldValue()));
            static_cast<sfrotation &>(sfdata->getFieldValue()).value = rot;
            break;

        case 2:
            if (!JSVAL_IS_OBJECT(argv[2])
                    || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[2]),
                                      &JavaScript_::SFVec3f::jsclass, 0)) {
                return JS_FALSE;
            }
            sfdata = static_cast<SField::SFData *>
                        (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[2])));
            assert(sfdata);
            assert(dynamic_cast<sfvec3f *>(&sfdata->getFieldValue()));
            static_cast<sfvec3f &>(sfdata->getFieldValue()).value = scale;
            break;

        default:
            break;
        }
    }
    *rval = JSVAL_VOID;
    return JS_TRUE;
}

JSBool VrmlMatrix::inverse(JSContext * const cx, JSObject * const obj,
                           const uintN argc, jsval * const argv,
                           jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    
    JSObject * robj = JS_ConstructObject(cx, &VrmlMatrix::jsclass, 0,
                                         JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    const mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    mat4f * const newMat =
            static_cast<mat4f *>(JS_GetPrivate(cx, robj));
    assert(newMat);
    *newMat = thisMat->inverse();
    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool VrmlMatrix::transpose(JSContext * const cx, JSObject * const obj,
                             const uintN argc, jsval * const argv,
                             jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    
    JSObject * robj = JS_ConstructObject(cx, &VrmlMatrix::jsclass, 0,
                                         JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    const mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    mat4f * const newMat = static_cast<mat4f *>(JS_GetPrivate(cx, robj));
    assert(newMat);
    *newMat = thisMat->transpose();
    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool VrmlMatrix::multLeft(JSContext * const cx, JSObject * const obj,
                            const uintN argc, jsval * const argv,
                            jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(argc >= 1);

    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &VrmlMatrix::jsclass, 0)) {
        return JS_FALSE;
    }

    const mat4f * const argMat =
            static_cast<mat4f *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(argMat);

    const mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &VrmlMatrix::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    mat4f * const resultMat = static_cast<mat4f *>(JS_GetPrivate(cx, robj));
    assert(resultMat);

    *resultMat = *argMat * *thisMat;
    return JS_TRUE;
}

JSBool VrmlMatrix::multRight(JSContext * const cx, JSObject * const obj,
                             const uintN argc, jsval * const argv,
                             jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(argc >= 1);

    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &VrmlMatrix::jsclass, 0)) {
        return JS_FALSE;
    }

    const mat4f * const argMat =
            static_cast<mat4f *> (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(argMat);

    const mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &VrmlMatrix::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    mat4f * const resultMat = static_cast<mat4f *>(JS_GetPrivate(cx, robj));
    assert(resultMat);

    *resultMat = *thisMat * *argMat;
    return JS_TRUE;
}

JSBool VrmlMatrix::multVecMatrix(JSContext * const cx, JSObject * const obj,
                                 const uintN argc, jsval * const argv,
                                 jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(argc >= 1);

    //
    // Make sure argument is an SFVec3f.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFVec3f::jsclass, 0)) {
        return JS_FALSE;
    }
    
    const SField::SFData * sfdata =
            static_cast<SField::SFData *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & argVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    const mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SField::SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & resultVec =
        static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    resultVec.value = argVec.value * *thisMat;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool VrmlMatrix::multMatrixVec(JSContext * const cx, JSObject * const obj,
                                 const uintN argc, jsval * const argv,
                                 jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(argc >= 1);

    //
    // Make sure argument is an SFVec3f.
    //
    if (!JSVAL_IS_OBJECT(argv[0])
            || !JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[0]),
                              &SFVec3f::jsclass, 0)) {
        return JS_FALSE;
    }

    const SField::SFData * sfdata =
            static_cast<SField::SFData *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::sfvec3f & argVec =
            static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    const mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SField::SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::sfvec3f *>(&sfdata->getFieldValue()));
    OpenVRML::sfvec3f & resultVec =
        static_cast<OpenVRML::sfvec3f &>(sfdata->getFieldValue());

    resultVec.value = *thisMat * argVec.value;

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool VrmlMatrix::toString(JSContext * const cx, JSObject * const obj,
                            const uintN argc, jsval * const argv,
                            jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    const mat4f * const thisMat = static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    JSString * jsstr = 0;
    try {
        std::ostringstream out;
        out << *thisMat;
        jsstr = JS_NewStringCopyZ(cx, out.str().c_str());
        if (!jsstr) { return JS_FALSE; }
    } catch (std::bad_alloc & ex) {
        OPENVRML_PRINT_EXCEPTION_(ex);
        return JS_FALSE;
    }
    *rval = STRING_TO_JSVAL(jsstr);
    return JS_TRUE;
}

void VrmlMatrix::finalize(JSContext * const cx, JSObject * const obj) throw ()
{
    delete static_cast<mat4f *>(JS_GetPrivate(cx, obj));
    JS_SetPrivate(cx, obj, 0);
}

} // namespace JavaScript_
# endif // OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT
}
