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
ScriptNodeClass::ScriptNodeClass(Browser & browser): NodeClass(browser) {}

/**
 * @brief Destructor.
 */
ScriptNodeClass::~ScriptNodeClass() throw () {}

/**
 * @brief Not implemented.
 *
 * This method is not implemented because the Script node implementation,
 * unlike other node implementations, cannot provide the implementation of
 * an @c EXTERNPROTO. It is an error to call this method.
 */
const NodeTypePtr ScriptNodeClass::createType(const std::string &,
                                              const NodeInterfaceSet &)
        throw () {
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
        NodeType(nodeClass, "Script") {
    static const NodeInterface interfaces[] = {
        NodeInterface(NodeInterface::exposedField, FieldValue::mfstring, "url"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "directOutput"),
        NodeInterface(NodeInterface::field, FieldValue::sfbool, "mustEvaluate")
    };
    this->interfaces.add(interfaces[0]);
    this->interfaces.add(interfaces[1]);
    this->interfaces.add(interfaces[2]);
}

/**
 * @brief Destructor.
 */
ScriptNode::ScriptNodeType::~ScriptNodeType() throw () {}

/**
 * @brief Add an interface.
 */
void ScriptNode::ScriptNodeType::addInterface(const NodeInterface & interface)
        throw (std::invalid_argument) {
    this->interfaces.add(interface);
}

/**
 * @brief Get the interfaces for the node.
 */
const NodeInterfaceSet & ScriptNode::ScriptNodeType::getInterfaces() const
        throw () {
    return this->interfaces;
}

namespace {
    const FieldValuePtr defaultFieldValue(const FieldValue::Type fieldType) {
        switch (fieldType) {
        case FieldValue::sfbool:        return FieldValuePtr(new SFBool);
        case FieldValue::sfcolor:       return FieldValuePtr(new SFColor);
        case FieldValue::sffloat:       return FieldValuePtr(new SFFloat);
        case FieldValue::sfimage:       return FieldValuePtr(new SFImage);
        case FieldValue::sfint32:       return FieldValuePtr(new SFInt32);
        case FieldValue::sfnode:        return FieldValuePtr(new SFNode);
        case FieldValue::sfrotation:    return FieldValuePtr(new SFRotation);
        case FieldValue::sfstring:      return FieldValuePtr(new SFString);
        case FieldValue::sftime:        return FieldValuePtr(new SFTime);
        case FieldValue::sfvec2f:       return FieldValuePtr(new SFVec2f);
        case FieldValue::sfvec3f:       return FieldValuePtr(new SFVec3f);
        case FieldValue::mfcolor:       return FieldValuePtr(new MFColor);
        case FieldValue::mffloat:       return FieldValuePtr(new MFFloat);
        case FieldValue::mfint32:       return FieldValuePtr(new MFInt32);
        case FieldValue::mfnode:        return FieldValuePtr(new MFNode);
        case FieldValue::mfrotation:    return FieldValuePtr(new MFRotation);
        case FieldValue::mfstring:      return FieldValuePtr(new MFString);
        case FieldValue::mftime:        return FieldValuePtr(new MFTime);
        case FieldValue::mfvec2f:       return FieldValuePtr(new MFVec2f);
        case FieldValue::mfvec3f:       return FieldValuePtr(new MFVec3f);
        default:
            assert(false);
        }
        return FieldValuePtr(0);
    }
}

/**
 * @brief Clone the Script node that has this NodeType.
 *
 * Since the NodeType for a ScriptNode is only available once a ScriptNode
 * is instantiated, you have to have a ScriptNode instance in order to be able
 * to use this method. The "primordial" ScriptNode instance must be created with
 * a call to the ScriptNode constructor.
 */
const NodePtr
        ScriptNode::ScriptNodeType::createNode(const ScopePtr & scope) const
        throw (std::bad_alloc) {
    ScriptNodeClass & scriptNodeClass =
            static_cast<ScriptNodeClass &>(this->nodeClass);
    const NodePtr node(new ScriptNode(scriptNodeClass, scope));
    ScriptNode & scriptNode = dynamic_cast<ScriptNode &>(*node);

    //
    // Copy the interfaces.
    //
    scriptNode.scriptNodeType.interfaces = this->interfaces;

    //
    // Add the default field and eventOut values.
    //
    for (NodeInterfaceSet::const_iterator itr(this->interfaces.begin());
            itr != this->interfaces.end(); ++itr) {
        if (itr->type == NodeInterface::field) {
            const FieldValueMap::value_type
                    value(itr->id, defaultFieldValue(itr->fieldType));
            const bool succeeded =
                    scriptNode.fieldValueMap.insert(value).second;
            assert(succeeded);
        } else if (itr->type == NodeInterface::eventOut) {
            const PolledEventOutValue
                    eventOutValue(defaultFieldValue(itr->fieldType), false);
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
    Node(this->scriptNodeType, scope),
    ChildNode(this->scriptNodeType, scope),
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

void ScriptNode::setUrl(const MFString & value, const double timestamp)
{
    delete this->script;
    this->script = 0;
    this->url = value;
    this->do_initialize(timestamp);

    //
    // url is an exposedField.
    //
    this->emitEvent("url_changed", this->url, timestamp);
}

const MFString & ScriptNode::getUrl() const
{
    return this->url;
}

void ScriptNode::addEventIn(const FieldValue::Type type, const std::string & id)
    throw (std::invalid_argument, std::bad_alloc)
{
    const NodeInterface interface(NodeInterface::eventIn, type, id);
    this->scriptNodeType.addInterface(interface);
}

void ScriptNode::addEventOut(const FieldValue::Type type,
                             const std::string & id)
    throw (std::invalid_argument, std::bad_alloc)
{
    const NodeInterface interface(NodeInterface::eventOut, type, id);
    this->scriptNodeType.addInterface(interface);

    //
    // eventOut value.
    //
    const PolledEventOutValue eventOutValue(defaultFieldValue(type), 0.0);
    EventOutValueMap::value_type value(id, eventOutValue);
    const bool succeeded = this->eventOutValueMap.insert(value).second;
    assert(succeeded);
}

void ScriptNode::addField(const std::string & id,
                          const FieldValuePtr & defaultVal)
    throw (std::invalid_argument, std::bad_alloc)
{
    const NodeInterface interface(NodeInterface::field, defaultVal->type(), id);
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
const ScriptNode * ScriptNode::toScript() const throw ()
{
    return this;
}

/**
 * @brief Return a pointer to this ScriptNode.
 *
 * @return a pointer to this ScriptNode.
 */
ScriptNode * ScriptNode::toScript() throw ()
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
                this->emitEvent(itr->first, *itr->second.value, currentTime);
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

void ScriptNode::assignWithSelfRefCheck(const SFNode & inval,
                                        SFNode & retval) const
    throw ()
{
    const NodePtr & oldNode = static_cast<SFNode &>(retval).get();

    //
    // About to relinquish ownership of a SFNode value. If the
    // SFNode value is this Script node, then we need to
    // *increment* its refcount, since we previously
    // *decremented* it to accommodate creating a cycle between
    // refcounted objects.
    //
    if (oldNode && (dynamic_cast<ScriptNode *>(oldNode.countPtr->first) == this)) {
        ++oldNode.countPtr->second;
    }

    retval = inval;

    //
    // Now, check to see if the new SFNode value is a self-
    // reference. If it is, we need to *decrement* the refcount.
    // A self-reference creates a cycle. If a Script node with
    // a self-reference were completely removed from the world,
    // it still wouldn't be deleted (if we didn't do this)
    // because the reference it held to itself would prevent the
    // refcount from ever dropping to zero.
    //
    const NodePtr & newNode = static_cast<SFNode &>(retval).get();
    if (dynamic_cast<ScriptNode *>(newNode.countPtr->first) == this) {
        --(newNode.countPtr->second);
    }
}

void ScriptNode::assignWithSelfRefCheck(const MFNode & inval,
                                        MFNode & retval) const
    throw ()
{
    size_t i;
    for (i = 0; i < retval.getLength(); ++i) {
        const NodePtr & oldNode = retval.getElement(i);
        if (oldNode && (dynamic_cast<ScriptNode *>(oldNode.countPtr->first) == this)) {
            ++oldNode.countPtr->second;
        }
    }

    retval = inval;

    for (i = 0; i < retval.getLength(); ++i) {
        const NodePtr & newNode = retval.getElement(i);
        if (newNode && (dynamic_cast<ScriptNode *>(newNode.countPtr->first) == this)) {
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
    assert(this->getScene());
    this->getScene()->browser.addScript(*this);

    this->eventsReceived = 0;
    this->script = this->createScript();
    if (this->script) {
        this->script->initialize(timestamp);
    }

    //
    // For each modified eventOut, send an event.
    //
    for (EventOutValueMap::iterator itr(this->eventOutValueMap.begin());
            itr != this->eventOutValueMap.end(); ++itr) {
        if (itr->second.modified) {
            this->emitEvent(itr->first, *itr->second.value, timestamp);
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
 * @exception UnsupportedInterface  if the node has no field @p id.
 * @exception std::bad_cast         if @p value is the wrong type.
 * @exception std::bad_alloc        if memory allocation fails.
 */
void ScriptNode::do_setField(const std::string & id, const FieldValue & value)
    throw (UnsupportedInterface, std::bad_cast, std::bad_alloc)
{
    FieldValueMap::iterator itr;
    if (id == "url") {
        //
        // Don't call setUrl() here, as that will emit a "url_changed" event.
        //
        this->url = dynamic_cast<const MFString &>(value);
    } else if (id == "directOutput") {
        this->directOutput = dynamic_cast<const SFBool &>(value);
    } else if (id == "mustEvaluate") {
        this->mustEvaluate = dynamic_cast<const SFBool &>(value);
    } else if (!((itr = this->fieldValueMap.find(id))
            == this->fieldValueMap.end())) {
        if (itr->second->type() == FieldValue::sfnode) {
            this->assignWithSelfRefCheck(dynamic_cast<const SFNode &>(value),
                                         static_cast<SFNode &>(*itr->second));
        } else if (itr->second->type() == FieldValue::mfnode) {
            this->assignWithSelfRefCheck(dynamic_cast<const MFNode &>(value),
                                         static_cast<MFNode &>(*itr->second));
        } else {
            itr->second->assign(value); // Throws std::bad_cast.
        }
    } else {
        throw UnsupportedInterface("Script node has no field \"" + id + "\".");
    }
}

/**
 * @brief Get the value of a field.
 *
 * @param id    the name of the field to get.
 *
 * @return the value for field @p id.
 *
 * @exception UnsupportedInterface  if the node has no field @p id.
 */
const FieldValue & ScriptNode::do_getField(const std::string & id) const
    throw (UnsupportedInterface)
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
    throw UnsupportedInterface("Script node has no field \"" + id + "\".");
}

void ScriptNode::do_processEvent(const std::string & id,
                                 const FieldValue & value,
                                 const double timestamp)
    throw (UnsupportedInterface, std::bad_cast, std::bad_alloc)
{
    if (!this->nodeType.hasEventIn(id)) {
        throw UnsupportedInterface("Script node has no eventIn \"" + id
                                   + "\".");
    }

    if (!this->script) { return; }

    if (id == "url" || id == "set_url") {
        this->setUrl(dynamic_cast<const MFString &>(value), timestamp);
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
                this->emitEvent(itr->first, *itr->second.value, timestamp);
                itr->second.modified = false;
            }
        }
        ++this->eventsReceived;
    }

    //
    // Script nodes shouldn't generate redraws.
    //
    this->clearModified();
}

/**
 * @brief Set the value of one of the node's eventOuts.
 *
 * This method is intended to be used by scripting language bindings to
 * set the value of eventOuts in response to script code.
 */
void ScriptNode::setEventOut(const std::string & id, const FieldValue & value)
    throw (UnsupportedInterface, std::bad_cast, std::bad_alloc)
{
    const EventOutValueMap::iterator itr(this->eventOutValueMap.find(id));
    if (itr == this->eventOutValueMap.end()) {
        throw UnsupportedInterface("Script node has no eventOut \"" + id
                                    + "\".");
    }

    if (itr->second.value->type() == FieldValue::sfnode) {
        this->assignWithSelfRefCheck(dynamic_cast<const SFNode &>(value),
                                     static_cast<SFNode &>(*itr->second.value));
    } else if (itr->second.value->type() == FieldValue::mfnode) {
        this->assignWithSelfRefCheck(dynamic_cast<const MFNode &>(value),
                                     static_cast<MFNode &>(*itr->second.value));
    } else {
        itr->second.value->assign(value); // Throws std::bad_cast.
    }
    itr->second.modified = true;
}

const FieldValue & ScriptNode::do_getEventOut(const std::string & id) const
    throw (UnsupportedInterface)
{
    FieldValueMap::const_iterator itr;
    if (id == "url" || id == "url_changed") {
        return this->url;
    } else if ((itr = this->fieldValueMap.find(id)) != this->fieldValueMap.end()
            || (itr = this->fieldValueMap.find(id + "_changed"))
                != this->fieldValueMap.end()) {
        return *itr->second;
    }
    throw UnsupportedInterface("Script has no eventOut \"" + id + "\".");
}

void ScriptNode::do_shutdown(const double timestamp) throw ()
{
    if (this->script) { this->script->shutdown(timestamp); }
    this->getScene()->browser.removeScript(*this);
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
                              const OpenVRML::FieldValue & value,
                              double timestamp);
    virtual void eventsProcessed(double timeStamp);
    virtual void shutdown(double timeStamp);

    OpenVRML::ScriptNode & getScriptNode();

    jsval vrmlFieldToJSVal(const OpenVRML::FieldValue &) throw ();

private:
    void initVrmlClasses() throw (std::bad_alloc);
    void defineBrowserObject() throw (std::bad_alloc);
    void defineFields() throw (std::bad_alloc);
    void activate(double timeStamp, const std::string & fname,
                  size_t argc, const OpenVRML::FieldValue * const argv[]);
};

}
}
# endif

namespace OpenVRML {

Script * ScriptNode::createScript() {
// Try each url until we find one we like
for (size_t i = 0; i < this->url.getLength(); ++i) {
if (this->url.getElement(i).length() == 0) continue;

// Get the protocol & mimetype...
# if OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT
// Need to handle external .js files too...
const char * javascriptScheme = "javascript:";
const char * vrmlscriptScheme = "vrmlscript:";
if (std::equal(javascriptScheme, javascriptScheme + 11,
               this->url.getElement(i).begin())
        || std::equal(vrmlscriptScheme, vrmlscriptScheme + 11,
                      this->url.getElement(i).begin())) {
    return new JavaScript_::
                    Script(*this, this->url.getElement(i).substr(11));
}
# endif

#if OPENVRML_ENABLE_SCRIPT_NODE_JAVA
const char javaExtension1[] = ".class";
const char javaExtension2[] = ".CLASS";

int slen = this->url.getElement(i).length();

if (slen > 6
        && (std::equal(javaExtension1, javaExtension1 + 6,
                       this->url.getElement(i).end() - 6)
            || std::equal(javaExtension2, javaExtension2 + 6,
                          this->url.getElement(i).end() - 6))) {
    Doc2 base(this->nodeType.nodeClass.browser.getWorldURI());
    Doc2 doc(this->url.getElement(i), &base);
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
std::auto_ptr<FieldValue> createFieldValueFromJsval(JSContext * cx, jsval val,
                                                    FieldValue::Type fieldType)
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
        FieldValue * const fieldValue;

    public:
        explicit SFData(FieldValue * fieldValue);
        virtual ~SFData();

        FieldValue & getFieldValue() const;
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
    static JSBool toJsval(const OpenVRML::SFColor & sfcolor,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::SFColor>
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
    static JSBool toJsval(const OpenVRML::SFImage & sfcolor,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::SFImage>
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
    static JSBool toJsval(const OpenVRML::SFNode & sfnode,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::SFNode>
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
    static JSBool toJsval(const OpenVRML::SFRotation & sfnode,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::SFRotation>
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
    static JSBool toJsval(const OpenVRML::SFVec2f & sfvec2f,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::SFVec2f>
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
    static JSBool toJsval(const OpenVRML::SFVec3f & sfvec3f,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::SFVec3f>
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

    static JSBool toJsval(const OpenVRML::MFColor & mfcolor,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::MFColor>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
};

class MFFloat : public MFJSDouble<MFFloat> {
public:
    static JSClass jsclass;

    static JSBool toJsval(const OpenVRML::MFFloat & mffloat,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::MFFloat>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
};

class MFInt32 : public MField {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj);
    static JSBool toJsval(const OpenVRML::MFInt32 & mfint32,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::MFInt32>
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
    static JSBool toJsval(const OpenVRML::MFNode & mfnode,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::MFNode>
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

    static JSBool toJsval(const OpenVRML::MFRotation & mfrotation,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::MFRotation>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
};

class MFString : public MField {
public:
    static JSClass jsclass;

    static JSObject * initClass(JSContext * cx, JSObject * obj) throw ();
    static JSBool toJsval(const OpenVRML::MFString & mfstring,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::MFString>
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

    static JSBool toJsval(const OpenVRML::MFTime & mftime,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::MFTime>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
};

class MFVec2f : public MFJSObject<MFVec2f> {
public:
    static JSClass jsclass;
    static JSClass & sfjsclass;

    static JSBool toJsval(const OpenVRML::MFVec2f & mfvec2f,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::MFVec2f>
    createFromJSObject(JSContext * cx, JSObject * obj)
        throw (BadConversion, std::bad_alloc);
};

class MFVec3f : public MFJSObject<MFVec3f> {
public:
    static JSClass jsclass;
    static JSClass & sfjsclass;

    static JSBool toJsval(const OpenVRML::MFVec3f & mfvec3f,
                          JSContext * cx, JSObject * obj, jsval * rval)
        throw ();
    static std::auto_ptr<OpenVRML::MFVec3f>
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
    JS_DestroyContext(this->cx);
    if (--nInstances == 0) {
        JS_DestroyRuntime(rt);
        rt = 0;
    }
}

void Script::initialize(const double timestamp)
{
    const SFTime arg(timestamp);
    const FieldValue * argv[] = { &arg };
    this->activate(timestamp, "initialize", 1, argv);
}

void Script::processEvent(const std::string & id,
                          const FieldValue & value,
                          const double timestamp)
{
    const SFTime timestampArg(timestamp);
    const FieldValue * argv[] = { &value, &timestampArg };
    this->activate(timestamp, id, 2, argv);
}

void Script::eventsProcessed(const double timestamp)
{
    const SFTime arg(timestamp);
    const FieldValue * argv[] = { &arg };
    this->activate(timestamp, "eventsProcessed", 1, argv);
}

void Script::shutdown(const double timestamp)
{
    const SFTime arg(timestamp);
    const FieldValue * argv[] = { &arg };
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
                      const FieldValue * const argv[])
{
    assert(this->cx);

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
                    std::auto_ptr<FieldValue> fieldValue =
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
 * @brief Create a jsval from an OpenVRML::FieldValue.
 */
jsval Script::vrmlFieldToJSVal(const FieldValue & fieldValue) throw ()
{
    jsval rval;
    JSObject * const globalObj = JS_GetGlobalObject(this->cx);
    assert(globalObj);

    switch (fieldValue.type()) {
    case FieldValue::sfbool:
        {
            const OpenVRML::SFBool & sfbool =
                    static_cast<const OpenVRML::SFBool &>(fieldValue);
	    rval = BOOLEAN_TO_JSVAL(sfbool.get());
        }
        break;

    case FieldValue::sfcolor:
        {
            const OpenVRML::SFColor & sfcolor =
                    static_cast<const OpenVRML::SFColor &>(fieldValue);
            if (!SFColor::toJsval(sfcolor,
                                  this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::sffloat:
        {
	    const OpenVRML::SFFloat & sffloat =
                    static_cast<const OpenVRML::SFFloat &>(fieldValue);
            if (!JS_NewDoubleValue(cx, sffloat.get(), &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::sfimage:
        {
            const OpenVRML::SFImage & sfimage =
                    static_cast<const OpenVRML::SFImage &>(fieldValue);
            if (!SFImage::toJsval(sfimage, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::sfint32:
        {
            const OpenVRML::SFInt32 & sfint32 =
                    static_cast<const OpenVRML::SFInt32 &>(fieldValue);
            if (!JS_NewNumberValue(cx, jsdouble(sfint32.get()), &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::sfnode:
        {
            const OpenVRML::SFNode & sfnode =
                    static_cast<const OpenVRML::SFNode &>(fieldValue);
            if (!SFNode::toJsval(sfnode, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::sfrotation:
        {
            const OpenVRML::SFRotation & sfrotation =
                static_cast<const OpenVRML::SFRotation &>(fieldValue);
            if (!SFRotation::toJsval(sfrotation,
                                     this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::sfstring:
        {
            const OpenVRML::SFString & sfstring =
                    static_cast<const OpenVRML::SFString &>(fieldValue);
            rval = STRING_TO_JSVAL(
                        JS_NewStringCopyZ(cx, sfstring.get().c_str()));
        }
        break;

    case FieldValue::sftime:
        {
	    const OpenVRML::SFTime & sftime =
                    static_cast<const OpenVRML::SFTime &>(fieldValue);
            if (!JS_NewDoubleValue(cx, sftime.get(), &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::sfvec2f:
        {
            const OpenVRML::SFVec2f & sfvec2f =
                    static_cast<const OpenVRML::SFVec2f &>(fieldValue);
            if (!SFVec2f::toJsval(sfvec2f,
                                  this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::sfvec3f:
        {
            const OpenVRML::SFVec3f & sfvec3f =
                    static_cast<const OpenVRML::SFVec3f &>(fieldValue);
            if (!SFVec3f::toJsval(sfvec3f,
                                  this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::mfcolor:
        {
            const OpenVRML::MFColor & mfcolor =
                    static_cast<const OpenVRML::MFColor &>(fieldValue);
            if (!MFColor::toJsval(mfcolor,
                                  this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::mffloat:
        {
            const OpenVRML::MFFloat & mffloat =
                    static_cast<const OpenVRML::MFFloat &>(fieldValue);
            if (!MFFloat::toJsval(mffloat,
                                  this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::mfint32:
        {
            const OpenVRML::MFInt32 & mfint32 =
                    static_cast<const OpenVRML::MFInt32 &>(fieldValue);
            if (!MFInt32::toJsval(mfint32, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::mfnode:
        {
            const OpenVRML::MFNode & mfnode =
                    static_cast<const OpenVRML::MFNode &>(fieldValue);
            if (!MFNode::toJsval(mfnode, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::mfrotation:
        {
            const OpenVRML::MFRotation & mfrotation =
                static_cast<const OpenVRML::MFRotation &>(fieldValue);
            if (!MFRotation::toJsval(mfrotation,
                                     this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::mfstring:
        {
            const OpenVRML::MFString & mfstring =
                    static_cast<const OpenVRML::MFString &>(fieldValue);
            if (!MFString::toJsval(mfstring,
                                   this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::mftime:
        {
            const OpenVRML::MFTime & mftime =
                    static_cast<const OpenVRML::MFTime &>(fieldValue);
            if (!MFTime::toJsval(mftime, this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::mfvec2f:
        {
            const OpenVRML::MFVec2f & mfvec2f =
                    static_cast<const OpenVRML::MFVec2f &>(fieldValue);
            if (!MFVec2f::toJsval(mfvec2f,
                                  this->cx, globalObj, &rval)) {
                rval = JSVAL_NULL;
            }
        }
        break;

    case FieldValue::mfvec3f:
        {
            const OpenVRML::MFVec3f & mfvec3f =
                    static_cast<const OpenVRML::MFVec3f &>(fieldValue);
            if (!MFVec3f::toJsval(mfvec3f,
                                  this->cx, globalObj, &rval)) {
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

    const FieldValue::Type fieldType =
            scriptNode.nodeType.hasEventOut(eventId);
    //
    // If this assertion is false, then we accidentally gave this
    // setter to an object that doesn't correspond to an eventOut!
    //
    assert(fieldType != FieldValue::invalidType);

    //
    // Convert to an OpenVRML::FieldValue and set the eventOut value.
    //
    try {
        std::auto_ptr<FieldValue> fieldValue =
                createFieldValueFromJsval(cx, *val, fieldType);
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

    const FieldValue::Type fieldType =
            scriptNode.nodeType.hasField(fieldId);
    //
    // If this assertion is false, then we accidentally gave this
    // setter to an object that doesn't correspond to a field!
    //
    assert(fieldType != FieldValue::invalidType);

    //
    // Convert to an OpenVRML::FieldValue and set the eventOut value.
    //
    try {
        std::auto_ptr<FieldValue> fieldValue =
                createFieldValueFromJsval(cx, *val, fieldType);
        scriptNode.setField(fieldId, *fieldValue);
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

    OpenVRML::Browser & browser = script->getScriptNode().getScene()->browser;

    string nodeId = script->getScriptNode().getId();
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
std::auto_ptr<FieldValue>
createFieldValueFromJsval(JSContext * const cx, const jsval v,
                          const FieldValue::Type expectType)
    throw (BadConversion, std::bad_alloc)
{
    using std::auto_ptr;

    switch (expectType) {
    case FieldValue::sfbool:
        {
            if (!JSVAL_IS_BOOLEAN(v)) {
                throw BadConversion("Boolean value expected.");
            }
            return auto_ptr<FieldValue>
                    (new SFBool(JSVAL_TO_BOOLEAN(v)));
        }

    case FieldValue::sfcolor:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (SFColor::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::sffloat:
        {
            jsdouble d;
            if (!JS_ValueToNumber(cx, v, &d)) {
                throw BadConversion("Numeric value expected.");
            }
            return auto_ptr<FieldValue>(new SFFloat(d));
        }

    case FieldValue::sfint32:
        {
            int32 i;
            if (!JS_ValueToECMAInt32(cx, v, &i)) {
                throw BadConversion("Numeric value expected.");
            }
            return auto_ptr<FieldValue>(new SFInt32(i));
        }

    case FieldValue::sfimage:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (SFImage::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::sfnode:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (SFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::sfrotation:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (SFRotation::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::sfstring:
        {
            if (!JSVAL_IS_STRING(v)) {
                throw BadConversion("String value expected.");
            }
            JSString * const jsstring = JSVAL_TO_STRING(v);
            //
            // Is the null check here really necessary? Perhaps this
            // should be an assertion?
            //
            return auto_ptr<FieldValue>
                    (new SFString(jsstring
                                    ? JS_GetStringBytes(jsstring)
                                    : ""));
        }

    case FieldValue::sftime:
        {
            jsdouble d;
            if (!JS_ValueToNumber(cx, v, &d)) {
                throw BadConversion("Numeric value expected.");
            }
            return auto_ptr<FieldValue>(new SFTime(d));
        }

    case FieldValue::sfvec2f:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (SFVec2f::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::sfvec3f:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (SFVec3f::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::mfcolor:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (MFColor::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::mffloat:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (MFFloat::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::mfint32:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (MFInt32::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::mfnode:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (MFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::mfrotation:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (MFRotation::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::mfstring:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (MFString::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::mftime:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (MFTime::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::mfvec2f:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (MFVec2f::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    case FieldValue::mfvec3f:
        if (!JSVAL_IS_OBJECT(v)) {
            throw BadConversion("Object expected.");
        }
        return auto_ptr<FieldValue>
                (MFVec3f::createFromJSObject(cx, JSVAL_TO_OBJECT(v))
                    .release());

    default:
        assert(false);
        return auto_ptr<FieldValue>();
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
            script->getScriptNode().nodeType.nodeClass
                .browser.getName();
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
            script->getScriptNode().nodeType.nodeClass
                .browser.getVersion();
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

    float speed = script->getScriptNode().nodeType.nodeClass
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
                script->getScriptNode().nodeType.nodeClass
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
            script->getScriptNode().nodeType.nodeClass
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

    std::auto_ptr<OpenVRML::MFString> url =
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

    std::auto_ptr<OpenVRML::MFString> parameters =
            MFString::createFromJSObject(cx, JSVAL_TO_OBJECT(argv[1]));
    assert(parameters.get());

    script->getScriptNode().getScene()
            ->browser.loadURI(*url, *parameters);
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

    std::auto_ptr<OpenVRML::MFNode> nodes =
            MFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(argv[0]));
    assert(nodes.get());

    script->getScriptNode().getScene()
            ->browser.replaceWorld(*nodes);

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

        assert(script->getScriptNode().getScene());
        OpenVRML::Browser & browser =
                script->getScriptNode().getScene()->browser;
        OpenVRML::MFNode nodes =
                browser.createVrmlFromStream(in);

        if (nodes.getLength() == 0) {
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

    std::auto_ptr<OpenVRML::MFString>
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

    std::auto_ptr<OpenVRML::SFNode>
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
    std::auto_ptr<OpenVRML::MFNode>
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
    std::auto_ptr<OpenVRML::SFNode> fromNode =
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
    std::auto_ptr<OpenVRML::SFNode> toNode =
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
        fromNode->get()->addRoute(fromEventOut,
                                  toNode->get(), toEventIn);
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
    std::auto_ptr<OpenVRML::SFNode> fromNode =
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
    std::auto_ptr<OpenVRML::SFNode> toNode =
            SFNode::createFromJSObject(cx, JSVAL_TO_OBJECT(argv[0]));

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

SField::SFData::SFData(FieldValue * const fieldValue):
    fieldValue(fieldValue)
{
    assert(fieldValue);
}

SField::SFData::~SFData()
{
    delete this->fieldValue;
}

FieldValue & SField::SFData::getFieldValue() const
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

JSBool SFColor::toJsval(const OpenVRML::SFColor & sfcolor,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const sfcolorObj =
            JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfcolorObj) { return JS_FALSE; }

    const SFData * const sfdata =
            static_cast<SFData *>(JS_GetPrivate(cx, sfcolorObj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFColor *>(&sfdata->getFieldValue()));

    //
    // Assignment should not throw.
    //
    static_cast<OpenVRML::SFColor &>(sfdata->getFieldValue()) = sfcolor;

    *rval = OBJECT_TO_JSVAL(sfcolorObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::SFColor>
SFColor::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFColor::jsclass, 0)) {
        throw BadConversion("SFColor object expected.");
    }
    const SFData * const sfdata =
        static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFColor *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::SFColor>
            (static_cast<OpenVRML::SFColor *>
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
        std::auto_ptr<OpenVRML::SFColor>
                sfcolor(new OpenVRML::SFColor(rgb[0], rgb[1], rgb[2]));
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
    assert(dynamic_cast<OpenVRML::SFColor *>(&sfdata->getFieldValue()));
    const OpenVRML::SFColor & thisColor =
            static_cast<OpenVRML::SFColor &>(sfdata->getFieldValue());

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) { return JS_FALSE; }
        if (!JS_NewDoubleValue(cx, thisColor[JSVAL_TO_INT(id)], rval)) {
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
    assert(dynamic_cast<OpenVRML::SFColor *>(&sfdata->getFieldValue()));
    OpenVRML::SFColor & thisColor =
            static_cast<OpenVRML::SFColor &>(sfdata->getFieldValue());

    if (!JSVAL_IS_INT(id)
            || JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) {
        return JS_FALSE;
    }

    jsdouble d;
    if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

    thisColor[JSVAL_TO_INT(id)] = d;
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
    assert(dynamic_cast<OpenVRML::SFColor *>(&sfdata->getFieldValue()));
    OpenVRML::SFColor & thisColor =
            static_cast<OpenVRML::SFColor &>(sfdata->getFieldValue());

    jsdouble h, s, v;

    if (!JSVAL_IS_NUMBER(argv[0])
            || !JS_ValueToNumber(cx, argv[0], &h)
            || !JSVAL_IS_NUMBER(argv[1])
            || !JS_ValueToNumber(cx, argv[1], &s)
            || !JSVAL_IS_NUMBER(argv[2])
            || !JS_ValueToNumber(cx, argv[2], &v)) {
        return JS_FALSE;
    }

    thisColor.setHSV(h, s, v);
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
    assert(dynamic_cast<OpenVRML::SFColor *>(&sfdata->getFieldValue()));
    OpenVRML::SFColor & thisColor =
            static_cast<OpenVRML::SFColor &>(sfdata->getFieldValue());

    float hsv[3];

    thisColor.getHSV(hsv); // XXX throws? nah...

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

JSBool SFImage::toJsval(const OpenVRML::SFImage & sfimage,
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
        std::auto_ptr<OpenVRML::SFImage>
                sfimageClone(static_cast<OpenVRML::SFImage *>
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

std::auto_ptr<OpenVRML::SFImage>
SFImage::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFImage::jsclass, 0)) {
        throw BadConversion("SFImage object expected.");
    }
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFImage *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::SFImage>
            (static_cast<OpenVRML::SFImage *>
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
                int32 pixel;
                if (!JS_ValueToInt32(cx, mfdata->array[i], &pixel)) {
                    return JS_FALSE;
                }
                for (size_t j(0); j < comp; ++j) {
                    *(pixelPtr + j) = (pixel & byteMask[j]) >> (8 * j);
                }
            }
        }

        std::auto_ptr<OpenVRML::SFImage>
                sfimage(new OpenVRML::SFImage(x, y, comp, &pixels[0]));
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
    assert(dynamic_cast<OpenVRML::SFImage *>(&sfdata->getFieldValue()));
    const OpenVRML::SFImage & thisImage =
            static_cast<OpenVRML::SFImage &>(sfdata->getFieldValue());

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

JSBool SFNode::toJsval(const OpenVRML::SFNode & sfnode,
                       JSContext * const cx, JSObject * const obj,
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
        std::auto_ptr<OpenVRML::SFNode>
                sfnodeClone(static_cast<OpenVRML::SFNode *>
                                (sfnode.clone().release()));
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

std::auto_ptr<OpenVRML::SFNode>
SFNode::createFromJSObject(JSContext * const cx,
                           JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFNode::jsclass, 0)) {
        throw BadConversion("SFNode object expected.");
    }
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFNode *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::SFNode>
            (static_cast<OpenVRML::SFNode *>
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

    assert(script->getScriptNode().getScene());
    OpenVRML::Browser & browser = script->getScriptNode().getScene()->browser;
    OpenVRML::MFNode nodes;
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
    if (nodes.getLength() != 1) { return JS_FALSE; }

    try {
        std::auto_ptr<OpenVRML::SFNode>
            sfnode(new OpenVRML::SFNode(nodes.getElement(0)));
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
    assert(dynamic_cast<OpenVRML::SFNode *>(&sfdata->getFieldValue()));
    const OpenVRML::SFNode & thisNode =
            static_cast<OpenVRML::SFNode &>(sfdata->getFieldValue());

    if (!thisNode.get()) { return JS_TRUE; }

    Script * const script = static_cast<Script *>(JS_GetContextPrivate(cx));
    assert(script);

    try {
        JSString * str = 0;
        if (JSVAL_IS_STRING(id) && ((str = JSVAL_TO_STRING(id)))) {
            const char * eventOut = JS_GetStringBytes(str);
            const FieldValue & fieldVal = thisNode.get()->getEventOut(eventOut);

            // convert event out value to jsval...
            *vp = script->vrmlFieldToJSVal(fieldVal);

            // If JS_FALSE is returned, apparently the parent object is
            // not searched for the property, so stuff like toString()
            // will fail... don't know how to distinguish between a valid
            // prop and an invalid eventOut...
            return JS_TRUE;
        }
    } catch (UnsupportedInterface & ex) {}
    return JS_TRUE;
}

JSBool SFNode::setProperty(JSContext * const cx, JSObject * const obj,
                           const jsval id, jsval * const vp)
    throw ()
{
    SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFNode *>(&sfdata->getFieldValue()));
    OpenVRML::SFNode & thisNode =
            static_cast<OpenVRML::SFNode &>(sfdata->getFieldValue());

    NodePtr nodePtr = thisNode.get();

    if (!nodePtr || !JSVAL_IS_STRING(id)) { return JS_FALSE; }

    const char * const eventInId = JS_GetStringBytes(JSVAL_TO_STRING(id));
    if (!eventInId) { return JS_FALSE; }

    // convert vp to field, send eventIn to node
    FieldValue::Type expectType;
    std::auto_ptr<FieldValue> fieldValue;
    if ((expectType = nodePtr->nodeType.hasEventIn(eventInId))) {
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
	Script * const script = static_cast<Script *>(JS_GetContextPrivate(cx));
        assert(script);
        script->getScriptNode().getScene()->browser
                .queueEvent(s_timeStamp, fieldValue.release(), nodePtr,
                            eventInId);
    } else if ((expectType = nodePtr->nodeType.hasField(eventInId))) {
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
	Script * const script = static_cast<Script *>(JS_GetContextPrivate(cx));
	assert(script);
	script->getScriptNode().getScene()->browser
                .queueEvent(s_timeStamp, fieldValue.release(), nodePtr,
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

JSBool SFRotation::toJsval(const OpenVRML::SFRotation & sfrotation,
                           JSContext * const cx, JSObject * const obj,
                           jsval * const rval)
    throw ()
{
    JSObject * const sfrotationObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfrotationObj) { return JS_FALSE; }

    const SFData * const sfdata =
        static_cast<SFData *>(JS_GetPrivate(cx, sfrotationObj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));

    //
    // Assignment should not throw.
    //
    static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue()) = sfrotation;
    *rval = OBJECT_TO_JSVAL(sfrotationObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::SFRotation>
SFRotation::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFRotation::jsclass, 0)) {
        throw BadConversion("SFRotation object expected.");
    }
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::SFRotation>
            (static_cast<OpenVRML::SFRotation *>
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
            assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
            const OpenVRML::SFVec3f & argVec1 =
                    static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

            if (argc > 1) {
                if (JSVAL_IS_OBJECT(argv[1])) {
                    if (!JS_InstanceOf(cx, JSVAL_TO_OBJECT(argv[1]),
                                   &JavaScript_::SFVec3f::jsclass, 0)) {
                        return JS_FALSE;
                    }
                    sfdata = static_cast<SFData *>
                                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[1])));
                    assert(sfdata);
                    assert(dynamic_cast<OpenVRML::SFVec3f *>
                                (&sfdata->getFieldValue()));
                    const OpenVRML::SFVec3f & argVec2 =
                            (static_cast<OpenVRML::SFVec3f &>
                                (sfdata->getFieldValue()));

                    OpenVRML::SFVec3f axisVec(argVec1.cross(argVec2));
                    axisVec = axisVec.normalize();
                    x = axisVec.getX();
                    y = axisVec.getY();
                    z = axisVec.getZ();
                    angle = acos(argVec1.dot(argVec2)
                            / (argVec1.length() * argVec2.length()));

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
        std::auto_ptr<OpenVRML::SFRotation>
                sfrotation(new OpenVRML::SFRotation(x, y, z, angle));
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
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    const OpenVRML::SFRotation & thisRot =
        static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 3) { return JS_FALSE; }
        if (!JS_NewDoubleValue(cx, thisRot.get()[JSVAL_TO_INT(id)], rval)) {
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
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    OpenVRML::SFRotation & thisRot =
        static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());

    if (!JSVAL_IS_INT(id)) { return JS_FALSE; }

    if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 3) { return JS_FALSE; }

    jsdouble d;
    if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

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
    sfdata->changed = true;
    return JS_TRUE;
}

JSBool SFRotation::getAxis(JSContext * const cx, JSObject * const obj,
                           uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    const OpenVRML::SFRotation & thisRot =
        static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());
    sfdata = 0;

    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0, obj);
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & resultVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

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
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    const OpenVRML::SFRotation & thisRot =
            static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());
    sfdata = 0;

    JSObject * const robj = JS_ConstructObject(cx, &SFRotation::jsclass, 0,
			                       JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    OpenVRML::SFRotation & resultRot =
        static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());

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
                            jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    const OpenVRML::SFRotation & thisRot =
            static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    const OpenVRML::SFRotation & argRot =
            static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());

    //
    // Construct the result object.
    //
    JSObject * robj = JS_ConstructObject(cx, &SFRotation::jsclass, 0,
                                         JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    OpenVRML::SFRotation & resultRot =
        static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());

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
	                   jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    const OpenVRML::SFRotation & thisRot =
            static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & argVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                         JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & resultVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

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
                           jsval * const rval)
    throw ()
{
    assert(argc >= 1);
    SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    OpenVRML::SFRotation & thisRot =
            static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());

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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&argSFData->getFieldValue()));
    const OpenVRML::SFVec3f & argVec =
            static_cast<OpenVRML::SFVec3f &>(argSFData->getFieldValue());

    thisRot.setAxis(argVec); // XXX Throws? Nah ...

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
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    const OpenVRML::SFRotation & thisRot =
            static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    const OpenVRML::SFRotation & argRot =
            static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFRotation *>(&sfdata->getFieldValue()));
    OpenVRML::SFRotation & resultRot =
            static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());

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

JSBool SFVec2f::toJsval(const OpenVRML::SFVec2f & sfvec2f,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const sfvec2fObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfvec2fObj) { return JS_FALSE; }

    const SFData * const sfdata =
            static_cast<SFData *>(JS_GetPrivate(cx, sfvec2fObj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));

    //
    // Assignment should not throw.
    //
    static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue()) = sfvec2f;
    *rval = OBJECT_TO_JSVAL(sfvec2fObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::SFVec2f>
SFVec2f::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFVec2f::jsclass, 0)) {
        throw BadConversion("SFVec2f object expected.");
    }
    const SFData * const sfdata =
            static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::SFVec2f>
            (static_cast<OpenVRML::SFVec2f *>
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
        std::auto_ptr<OpenVRML::SFVec2f>
                sfvec2f(new OpenVRML::SFVec2f(x, y));
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
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & thisVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 1) { return JS_FALSE; }
        if (!JS_NewDoubleValue(cx, thisVec[JSVAL_TO_INT(id)], rval)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool SFVec2f::setProperty(JSContext * const cx, JSObject * const obj,
                            const jsval id, jsval * const vp) throw () {
    SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec2f & thisVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());

    if (!JSVAL_IS_INT(id)) { return JS_FALSE; }

    if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 1) { return JS_FALSE; }

    jsdouble d;
    if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }

    thisVec[JSVAL_TO_INT(id)] = d;
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
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & thisVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & argVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec2f & resultVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());

    resultVec = thisVec.add(argVec); // XXX Throws? Nah ...

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
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & thisVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec2f & resultVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());

    resultVec = thisVec.divide(divisor); // XXX throws? nah...

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
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & thisVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & argVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    *rval = DOUBLE_TO_JSVAL(thisVec.dot(argVec));
    return JS_TRUE;
}

JSBool SFVec2f::length(JSContext * const cx, JSObject * const obj,
                       uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & thisVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());

    if (!JS_NewDoubleValue(cx, thisVec.length(), rval)) { return JS_FALSE; }
    return JS_TRUE;
}

JSBool SFVec2f::multiply(JSContext * const cx, JSObject * const obj,
                         const uintN argc, jsval * const argv,
                         jsval * const rval) throw () {
    assert(argc >= 1);
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & thisVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec2f & resultVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());

    resultVec = thisVec.multiply(factor); // XXX throws? nah...

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::negate(JSContext * const cx, JSObject * const obj,
                       uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & thisVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec2f & resultVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());

    resultVec = thisVec.negate(); // XXX Throws? Nah ...

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec2f::normalize(JSContext * const cx, JSObject * const obj,
                          uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & thisVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec2f & resultVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());

    resultVec = thisVec.normalize(); // XXX Throws? Nah ...

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
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & thisVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec2f & argVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec2f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec2f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec2f & resultVec =
            static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());

    resultVec = thisVec.subtract(argVec); // XXX Throws? Nah ...

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

JSBool SFVec3f::toJsval(const OpenVRML::SFVec3f & sfvec3f,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const sfvec3fObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!sfvec3fObj) { return JS_FALSE; }

    const SFData * const sfdata =
        static_cast<SFData *>(JS_GetPrivate(cx, sfvec3fObj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));

    //
    // Assignment should not throw.
    //
    static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue()) = sfvec3f;

    *rval = OBJECT_TO_JSVAL(sfvec3fObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::SFVec3f>
SFVec3f::createFromJSObject(JSContext * const cx, JSObject * const obj)
    throw (BadConversion, std::bad_alloc)
{
    if (!JS_InstanceOf(cx, obj, &SFVec3f::jsclass, 0)) {
        throw BadConversion("SFVec3f object expected.");
    }
    const SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    return std::auto_ptr<OpenVRML::SFVec3f>
            (static_cast<OpenVRML::SFVec3f *>
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
        std::auto_ptr<OpenVRML::SFVec3f>
                sfvec3f(new OpenVRML::SFVec3f(vec[0], vec[1], vec[2]));
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & thisVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    if (JSVAL_IS_INT(id)) {
        if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) { return JS_FALSE; }
        if (!JS_NewDoubleValue(cx, thisVec[JSVAL_TO_INT(id)], vp)) {
            return JS_FALSE;
        }
    }
    return JS_TRUE;
}

JSBool SFVec3f::setProperty(JSContext * const cx, JSObject * const obj,
                            const jsval id, jsval * const vp) throw () {
    SFData * const sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & thisVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    if (!JSVAL_IS_INT(id)) { return JS_FALSE; }
    if (JSVAL_TO_INT(id) < 0 || JSVAL_TO_INT(id) > 2) { return JS_FALSE; }

    jsdouble d;
    if (!JS_ValueToNumber(cx, *vp, &d)) { return JS_FALSE; }
    thisVec[JSVAL_TO_INT(id)] = d;
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & thisVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & argVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & resultVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    resultVec = thisVec.add(argVec); // XXX Throws? Nah ...

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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & thisVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & argVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) {return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & resultVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    resultVec = thisVec.cross(argVec); // XXX Throws? Nah ...

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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & thisVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & resultVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    resultVec = thisVec.divide(divisor); // XXX Throws? Nah ...

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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & thisVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & argVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    *rval = DOUBLE_TO_JSVAL(thisVec.dot(argVec));
    return JS_TRUE;
}

JSBool SFVec3f::length(JSContext * const cx, JSObject * const obj,
                       uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & thisVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    if (!JS_NewDoubleValue(cx, thisVec.length(), rval)) { return JS_FALSE; }
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & thisVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & resultVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    resultVec = thisVec.multiply(factor); // XXX Throws? Nah ...

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::negate(JSContext * const cx, JSObject * const obj,
                       uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & thisVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & resultVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    resultVec = thisVec.negate(); // Throws? Nah ...

    *rval = OBJECT_TO_JSVAL(robj);
    return JS_TRUE;
}

JSBool SFVec3f::normalize(JSContext * const cx, JSObject * const obj,
                          uintN, jsval *, jsval * const rval)
    throw ()
{
    const SFData * sfdata = static_cast<SFData *>(JS_GetPrivate(cx, obj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & thisVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & resultVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    resultVec = thisVec.normalize(); // XXX Throws? Nah ...

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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & thisVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & argVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & resultVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    resultVec = thisVec.subtract(argVec); // XXX Throws? Nah ...

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

std::auto_ptr<OpenVRML::MFColor>
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
    std::auto_ptr<OpenVRML::MFColor>
            mfcolor(new OpenVRML::MFColor(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFColor::jsclass, 0));
        const SField::SFData * const sfdata =
                static_cast<SField::SFData *>
                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const OpenVRML::SFColor & sfcolor =
                static_cast<OpenVRML::SFColor &>(sfdata->getFieldValue());
        mfcolor->setElement(i, sfcolor.get());
    }
    return mfcolor;
}

JSBool MFColor::toJsval(const OpenVRML::MFColor & mfcolor,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const mfcolorObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfcolorObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(mfcolor.getLength());
    if (!setLength(cx, mfcolorObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfcolorObj));
    for (size_t i = 0; i < mfcolor.getLength(); ++i) {
        const OpenVRML::SFColor sfcolor(mfcolor.getElement(i));
        if (!SFColor::toJsval(sfcolor, cx, obj, &mfdata->array[i])) {
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

JSBool MFFloat::toJsval(const OpenVRML::MFFloat & mffloat,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    JSObject * const mffloatObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mffloatObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(mffloat.getLength());
    if (!setLength(cx, mffloatObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mffloatObj));
    for (size_t i = 0; i < mffloat.getLength(); ++i) {
        if (!JS_NewDoubleValue(cx, mffloat.getElement(i), &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mffloatObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::MFFloat>
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
    std::auto_ptr<OpenVRML::MFFloat>
            mffloat(new OpenVRML::MFFloat(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
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

JSBool MFInt32::toJsval(const OpenVRML::MFInt32 & mfint32,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);

    JSObject * const mfint32Obj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfint32Obj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(mfint32.getLength());
    if (!setLength(cx, mfint32Obj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfint32Obj));
    for (size_t i = 0; i < mfint32.getLength(); ++i) {
        mfdata->array[i] = INT_TO_JSVAL(mfint32.getElement(i));
    }
    *rval = OBJECT_TO_JSVAL(mfint32Obj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::MFInt32>
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
    std::auto_ptr<OpenVRML::MFInt32>
            mfint32(new OpenVRML::MFInt32(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_INT(mfdata->array[i]));
        mfint32->setElement(i, JSVAL_TO_INT(mfdata->array[i]));
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
            int32 integer;
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
    int32 i;
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

JSBool MFNode::toJsval(const OpenVRML::MFNode & mfnode,
                       JSContext * const cx, JSObject * const obj,
                       jsval * const rval)
    throw ()
{
    JSObject * const mfnodeObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfnodeObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(mfnode.getLength());
    if (!setLength(cx, mfnodeObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, mfnodeObj));
    for (size_t i = 0; i < mfnode.getLength(); ++i) {
        const OpenVRML::SFNode sfnode(mfnode.getElement(i));
        if (!SFNode::toJsval(sfnode, cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfnodeObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::MFNode>
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
    std::auto_ptr<OpenVRML::MFNode>
            mfnode(new OpenVRML::MFNode(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFNode::jsclass, 0));
        const SField::SFData * const sfdata =
            static_cast<SField::SFData *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const OpenVRML::SFNode & sfnode =
                static_cast<OpenVRML::SFNode &>(sfdata->getFieldValue());
        mfnode->setElement(i, sfnode.get());
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

JSBool MFRotation::toJsval(const OpenVRML::MFRotation & mfrotation,
                           JSContext * const cx, JSObject * const obj,
                           jsval * const rval)
    throw ()
{
    JSObject * const mfrotationObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfrotationObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(mfrotation.getLength());
    if (!setLength(cx, mfrotationObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfrotationObj));
    for (size_t i = 0; i < mfrotation.getLength(); ++i) {
        const OpenVRML::SFRotation sfrotation(mfrotation.getElement(i));
        if (!SFRotation::toJsval(sfrotation, cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfrotationObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::MFRotation>
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
    std::auto_ptr<OpenVRML::MFRotation>
            mfrotation(new OpenVRML::MFRotation(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFRotation::jsclass, 0));
        const SField::SFData * const sfdata =
            static_cast<SField::SFData *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const OpenVRML::SFRotation & sfrotation =
                static_cast<OpenVRML::SFRotation &>(sfdata->getFieldValue());
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

JSBool MFString::toJsval(const OpenVRML::MFString & mfstring,
                         JSContext * const cx, JSObject * const obj,
                         jsval * const rval)
    throw ()
{
    assert(cx);
    assert(obj);
    assert(rval);
    JSObject * const mfstringObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfstringObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(mfstring.getLength());
    if (!setLength(cx, mfstringObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfstringObj));
    for (size_t i = 0; i < mfstring.getLength(); ++i) {
        JSString * jsstring =
                JS_NewStringCopyZ(cx, mfstring.getElement(i).c_str());
        if (!jsstring) { return JS_FALSE; }
        mfdata->array[i] = STRING_TO_JSVAL(jsstring);
    }
    *rval = OBJECT_TO_JSVAL(mfstringObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::MFString>
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
    std::auto_ptr<OpenVRML::MFString>
            mfstring(new OpenVRML::MFString(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_STRING(mfdata->array[i]));
        const char * const str =
                JS_GetStringBytes(JSVAL_TO_STRING(mfdata->array[i]));
        mfstring->setElement(i, str);
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

JSBool MFTime::toJsval(const OpenVRML::MFTime & mftime,
                       JSContext * const cx, JSObject * const obj,
                       jsval * const rval)
    throw ()
{
    JSObject * const mftimeObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mftimeObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(mftime.getLength());
    if (!setLength(cx, mftimeObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata = static_cast<MFData *>(JS_GetPrivate(cx, mftimeObj));
    for (size_t i = 0; i < mftime.getLength(); ++i) {
        if (!JS_NewDoubleValue(cx, mftime.getElement(i), &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mftimeObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::MFTime>
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
    std::auto_ptr<OpenVRML::MFTime>
            mftime(new OpenVRML::MFTime(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
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

JSBool MFVec2f::toJsval(const OpenVRML::MFVec2f & mfvec2f,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const mfvec2fObj = JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfvec2fObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(mfvec2f.getLength());
    if (!setLength(cx, mfvec2fObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfvec2fObj));
    for (size_t i = 0; i < mfvec2f.getLength(); ++i) {
        const OpenVRML::SFVec2f sfvec2f(mfvec2f.getElement(i));
        if (!SFVec2f::toJsval(sfvec2f, cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfvec2fObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::MFVec2f>
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
    std::auto_ptr<OpenVRML::MFVec2f>
            mfvec2f(new OpenVRML::MFVec2f(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0; i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFVec2f::jsclass, 0));
        const SField::SFData * const sfdata =
                static_cast<SField::SFData *>
                    (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const OpenVRML::SFVec2f & sfvec2f =
                static_cast<OpenVRML::SFVec2f &>(sfdata->getFieldValue());
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

JSBool MFVec3f::toJsval(const OpenVRML::MFVec3f & mfvec3f,
                        JSContext * const cx, JSObject * const obj,
                        jsval * const rval)
    throw ()
{
    JSObject * const mfvec3fObj =
            JS_ConstructObject(cx, &jsclass, 0, obj);
    if (!mfvec3fObj) { return JS_FALSE; }

    jsval length = INT_TO_JSVAL(mfvec3f.getLength());
    if (!setLength(cx, mfvec3fObj, 0, &length)) { return JS_FALSE; }

    MFData * const mfdata =
            static_cast<MFData *>(JS_GetPrivate(cx, mfvec3fObj));
    for (size_t i = 0; i < mfvec3f.getLength(); ++i) {
        const OpenVRML::SFVec3f sfvec3f(mfvec3f.getElement(i));
        if (!SFVec3f::toJsval(sfvec3f, cx, obj, &mfdata->array[i])) {
            return JS_FALSE;
        }
    }
    *rval = OBJECT_TO_JSVAL(mfvec3fObj);
    return JS_TRUE;
}

std::auto_ptr<OpenVRML::MFVec3f>
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
    std::auto_ptr<OpenVRML::MFVec3f>
            mfvec3f(new OpenVRML::MFVec3f(mfdata->array.size()));
    for (MField::JsvalArray::size_type i = 0;
            i < mfdata->array.size(); ++i) {
        assert(JSVAL_IS_OBJECT(mfdata->array[i]));
        assert(JS_InstanceOf(cx, JSVAL_TO_OBJECT(mfdata->array[i]),
                             &SFVec3f::jsclass, 0));
        const SField::SFData * const sfdata =
            static_cast<SField::SFData *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(mfdata->array[i])));
        assert(sfdata);

        const OpenVRML::SFVec3f & sfvec3f =
                static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
        mfvec3f->setElement(i, sfvec3f.get());
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
              { "toString", toString, 0, 0, 0 } };

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
        std::auto_ptr<OpenVRML::VrmlMatrix> mat(new OpenVRML::VrmlMatrix);
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

        OpenVRML::VrmlMatrix * const thisMat =
                static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, obj));

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
    using OpenVRML::SFVec3f;
    using OpenVRML::SFRotation;

    assert(cx);
    assert(obj);
    
    SFVec3f translation(0.0, 0.0, 0.0);
    SFRotation rotation(0.0, 0.0, 1.0, 0.0);
    SFVec3f scale(1.0, 1.0, 1.0);
    SFRotation scaleOrientation(0.0, 0.0, 1.0, 0.0);
    SFVec3f center(0.0, 0.0, 0.0);
    
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
            assert(dynamic_cast<SFVec3f *>(&sfdata->getFieldValue()));
            translation = static_cast<SFVec3f &>(sfdata->getFieldValue());
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
            assert(dynamic_cast<SFRotation *>(&sfdata->getFieldValue()));
            rotation = static_cast<SFRotation &>(sfdata->getFieldValue());
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
            assert(dynamic_cast<SFVec3f *>(&sfdata->getFieldValue()));
            scale = static_cast<SFVec3f &>(sfdata->getFieldValue());
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
            assert(dynamic_cast<SFRotation *>(&sfdata->getFieldValue()));
            scaleOrientation =
                    static_cast<SFRotation &>(sfdata->getFieldValue());
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
            assert(dynamic_cast<SFVec3f *>(&sfdata->getFieldValue()));
            center = static_cast<SFVec3f &>(sfdata->getFieldValue());
            break;

        default:
            break;
        }
    }

    OpenVRML::VrmlMatrix * const thisMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    thisMat->setTransform(translation,
                          rotation,
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
    using OpenVRML::SFVec3f;
    using OpenVRML::SFRotation;

    assert(cx);
    assert(obj);
    
    SFVec3f translation;
    SFRotation rotation;
    SFVec3f scale;

    const OpenVRML::VrmlMatrix * const thisMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    thisMat->getTransform(translation,
                          rotation,
                          scale);
    
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
            assert(dynamic_cast<SFVec3f *>(&sfdata->getFieldValue()));
            static_cast<SFVec3f &>(sfdata->getFieldValue()) = translation;
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
            assert(dynamic_cast<SFRotation *>(&sfdata->getFieldValue()));
            static_cast<SFRotation &>(sfdata->getFieldValue()) = rotation;
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
            assert(dynamic_cast<SFVec3f *>(&sfdata->getFieldValue()));
            static_cast<SFVec3f &>(sfdata->getFieldValue()) = scale;
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

    const OpenVRML::VrmlMatrix * const thisMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    OpenVRML::VrmlMatrix * const newMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, robj));
    assert(newMat);
    *newMat = thisMat->affine_inverse();
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

    const OpenVRML::VrmlMatrix * const thisMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    OpenVRML::VrmlMatrix * const newMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, robj));
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

    const OpenVRML::VrmlMatrix * const argMat =
            static_cast<OpenVRML::VrmlMatrix *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(argMat);

    const OpenVRML::VrmlMatrix * const thisMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &VrmlMatrix::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    OpenVRML::VrmlMatrix * const resultMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, robj));
    assert(resultMat);

    *resultMat = thisMat->multLeft(*argMat);
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

    const OpenVRML::VrmlMatrix * const argMat =
            static_cast<OpenVRML::VrmlMatrix *>
                (JS_GetPrivate(cx, JSVAL_TO_OBJECT(argv[0])));
    assert(argMat);

    const OpenVRML::VrmlMatrix * const thisMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &VrmlMatrix::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    OpenVRML::VrmlMatrix * const resultMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, robj));
    assert(resultMat);

    *resultMat = thisMat->multRight(*argMat);
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & argVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    const OpenVRML::VrmlMatrix * const thisMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SField::SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & resultVec =
        static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    thisMat->multVecMatrix(argVec, resultVec);
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
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    const OpenVRML::SFVec3f & argVec =
            static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());
    sfdata = 0;

    const OpenVRML::VrmlMatrix * const thisMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, obj));
    assert(thisMat);

    //
    // Construct the result object.
    //
    JSObject * const robj = JS_ConstructObject(cx, &SFVec3f::jsclass, 0,
                                               JS_GetParent(cx, obj));
    if (!robj) { return JS_FALSE; }

    sfdata = static_cast<SField::SFData *>(JS_GetPrivate(cx, robj));
    assert(sfdata);
    assert(dynamic_cast<OpenVRML::SFVec3f *>(&sfdata->getFieldValue()));
    OpenVRML::SFVec3f & resultVec =
        static_cast<OpenVRML::SFVec3f &>(sfdata->getFieldValue());

    thisMat->multMatrixVec(argVec, resultVec);
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

    const OpenVRML::VrmlMatrix * const thisMat =
            static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, obj));
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
    delete static_cast<OpenVRML::VrmlMatrix *>(JS_GetPrivate(cx, obj));
    JS_SetPrivate(cx, obj, 0);
}

} // namespace JavaScript_
# endif // OPENVRML_ENABLE_SCRIPT_NODE_JAVASCRIPT
}
