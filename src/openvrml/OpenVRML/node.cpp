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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <algorithm>
# include <sstream>
# include "node.h"
# include "scope.h"
# include "browser.h"
# include "MathUtils.h"
# include "bvolume.h"
# include "VrmlRenderContext.h"
# include "doc2.hpp"

namespace OpenVRML {

/**
 * @class UnsupportedInterface
 *
 * @brief Exception to indicate that a node interface is not supported.
 *
 * This exception is thrown by Node::setField and Node::processEvent to
 * indicate that the node doesn't support the interface through which the
 * caller is trying to modify the node. It is also thrown by
 * NodeClass::createType if the class object doesn't support an interface
 * specified in the NodeInterfaceSet given to that method.
 */

/**
 * @brief Constructor.
 *
 * @param message   An informative error message.
 */
UnsupportedInterface::UnsupportedInterface(const std::string & message):
    std::runtime_error(message)
{}

/**
 * @brief Constructor.
 *
 * @param nodeType      the node type.
 * @param interfaceId   the name of the interface that is not available.
 */
UnsupportedInterface::UnsupportedInterface(const NodeType & nodeType,
                                           const std::string & interfaceId):
    std::runtime_error(nodeType.id + " has no interface \"" + interfaceId + '"')
{}

namespace {
    const std::string toString(const NodeInterface::Type interfaceType)
    {
        std::ostringstream out;
        out << interfaceType;
        return out.str();
    }
}

/**
 * @brief Constructor.
 *
 * @param nodeType      the node type.
 * @param interfaceType the type of the interface that is not available.
 * @param interfaceId   the name of the interface that is not available.
 */
UnsupportedInterface::UnsupportedInterface(const NodeType & nodeType,
                                           const NodeInterface::Type interfaceType,
                                           const std::string & interfaceId):
    std::runtime_error(nodeType.id + " has no " + toString(interfaceType)
                       + " \"" + interfaceId + '"')
{}

/**
 * @brief Destructor.
 */
UnsupportedInterface::~UnsupportedInterface() throw ()
{}


/**
 * @struct NodeInterface
 *
 * @brief Type information for an interface of a node.
 */

/**
 * @enum NodeInterface::Type
 *
 * @brief Identify the type of interface.
 */

/**
 * @var NodeInterface::Type NodeInterface::invalidType
 *
 * @brief Used to indicate an invalid or unknown interface type.
 */

/**
 * @var NodeInterface::Type NodeInterface::eventIn
 *
 * @brief An eventIn.
 */

/**
 * @var NodeInterface::Type NodeInterface::eventOut
 *
 * @brief An eventOut.
 */

/**
 * @var NodeInterface::Type NodeInterface::exposedField
 *
 * @brief An exposedField.
 */

/**
 * @var NodeInterface::Type NodeInterface::field
 *
 * @brief A field.
 */

namespace {
    const char * const nodeInterfaceTypeId_[] = {
        "<invalid interface type>",
        "eventIn",
        "eventOut",
        "exposedField",
        "field"
    };
}

/**
 * @brief Stream inserter.
 *
 * @relates NodeInterface
 *
 * If @p type is NodeInterface::invalidType, @c failbit is set on @p out.
 *
 * @param out   an output stream.
 * @param type  a node interface type.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out, const NodeInterface::Type type)
{
    if (type == NodeInterface::invalidType) {
        out.setstate(std::ios_base::failbit);
    } else {
        out << nodeInterfaceTypeId_[type];
    }
    return out;
}

/**
 * @brief Stream extractor.
 *
 * @relates NodeInterface
 *
 * @param in    an input stream.
 * @param type  a node interface type.
 *
 * @return @p in.
 */
std::istream & operator>>(std::istream & in, NodeInterface::Type & type)
{
    using std::find;
    using std::string;

    string interfaceTypeId;
    in >> interfaceTypeId;

    static const char * const * const begin =
            nodeInterfaceTypeId_ + NodeInterface::eventIn;
    static const char * const * const end =
            nodeInterfaceTypeId_ + NodeInterface::field + 1;
    const char * const * const pos = find(begin, end, interfaceTypeId);
    if (pos != end) {
        type = NodeInterface::Type(pos - begin);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}


/**
 * @var NodeInterface::type
 *
 * @brief The interface type.
 */

/**
 * @var NodeInterface::fieldType
 *
 * @brief The field data type handled by this interface.
 */

/**
 * @var NodeInterface::id
 *
 * @brief The name of this interface.
 */

/**
 * @brief Constructor.
 *
 * @param type      the type of interface.
 * @param fieldType the field data type handled by the interface.
 * @param id        the name of the interface.
 */
NodeInterface::NodeInterface(const Type type, const FieldValue::Type fieldType,
                             const std::string & id):
        type(type), fieldType(fieldType), id(id) {}

/**
 * @fn bool operator==(const NodeInterface & lhs, const NodeInterface & rhs) throw ()
 *
 * @relates NodeInterface
 *
 * @brief Compare for equality.
 *
 * @param lhs   a NodeInterface.
 * @param rhs   a NodeInterface.
 *
 * @return @c true if the two NodeInterfaces are equal, @c false otherwise.
 */

/**
 * @fn bool operator!=(const NodeInterface & lhs, const NodeInterface & rhs) throw ()
 *
 * @relates NodeInterface
 *
 * @brief Compare for inequality.
 *
 * @param lhs   a NodeInterface.
 * @param rhs   a NodeInterface.
 *
 * @return @c true if the two NodeInterfaces are equal, @c false otherwise.
 */

/**
 * @class NodeInterfaceSet
 *
 * @brief A group of unique @link NodeInterface NodeInterfaces@endlink.
 *
 * NodeInterfaceSets are used to construct new @link NodeType NodeTypes@endlink.
 * NodeType objects also expose their interfaces as a NodeInterfaceSet.
 * The interfaces in a NodeInterfaceSet are guaranteed to be unique and
 * non-conflicting.
 */

/**
 * @internal
 *
 * @struct NodeInterfaceSet::IdLess
 *
 * @brief A functor for ordering @link NodeInterface NodeInterfaces@endlink in
 *      the set.
 *
 * Compares the @a id's of two @link NodeInterface NodeInterfaces@endlink using
 * @c operator<.
 */

/**
 * @fn bool NodeInterfaceSet::IdLess::operator()(const NodeInterface & lhs, const NodeInterface & rhs) const
 *
 * @brief Simple lexicographic comparison of the @a id's.
 *
 * @return @c true if @p lhs.id is lexicographically less than @p rhs.id,
 *      @c false otherwise.
 */

/**
 * @typedef NodeInterfaceSet::const_iterator
 *
 * @brief An STL const_iterator.
 */

/**
 * @brief Add an interface.
 *
 * @param nodeInterface a NodeInterface.
 *
 * @throw std::invalid argument if @p nodeInterface conflicts with an interface
 *                              already in the NodeInterfaceSet.
 * @throw std::bad_alloc        if memory allocation fails.
 */
void NodeInterfaceSet::add(const NodeInterface & nodeInterface)
        throw (std::invalid_argument, std::bad_alloc) {
    if (!this->nodeInterfaceSet.insert(nodeInterface).second) {
        throw std::invalid_argument("Interface conflicts with an interface already in this set.");
    }
}

/**
 * @fn NodeInterfaceSet::const_iterator NodeInterfaceSet::begin() const throw ()
 *
 * @brief Returns an iterator to the beginning of the NodeInterfaceSet.
 *
 * @return a const_iterator pointing to the first NodeInterface in the set.
 */

/**
 * @fn NodeInterfaceSet::const_iterator NodeInterfaceSet::end() const throw ()
 *
 * @brief Returns an iterator to the end of the NodeInterfaceSet.
 *
 * @return a const_iterator pointing to one increment past the last
 *       NodeInterface in the set.
 */

namespace {

    /**
     * @internal
     */
    struct InterfaceIdMatches_ :
            std::unary_function<OpenVRML::NodeInterface, bool> {
        explicit InterfaceIdMatches_(const std::string & interfaceId):
            interfaceId(&interfaceId)
        {}

        bool operator()(const OpenVRML::NodeInterface & interface) const
        {
            static const char eventInPrefix[] = "set_";
            static const char eventOutSuffix[] = "_changed";

            return interface.id == *this->interfaceId
                || (interface.type == NodeInterface::exposedField
                    && (eventInPrefix + interface.id == *this->interfaceId
                        || (interface.id + eventOutSuffix == *this->interfaceId)))
                || (interface.type == NodeInterface::eventIn
                    && interface.id == eventInPrefix + *this->interfaceId)
                || (interface.type == NodeInterface::eventOut
                    && interface.id == *this->interfaceId + eventOutSuffix);
        }

    private:
        const std::string * interfaceId;
    };
}

/**
 * @brief Find an interface matching @a interfaceId @p id.
 *
 * If no interface is found with an @a interfaceId that is an exact match for
 * @p id, this method will look for @c set_ and @c _changed variants.
 *
 * @param id    the interface id to look for.
 *
 * @return a const_iterator to the interface, or NodeInterfaceSet::end if no
 *      interface is found.
 */
NodeInterfaceSet::const_iterator
NodeInterfaceSet::findInterface(const std::string & id) const throw ()
{
    return std::find_if(this->begin(), this->end(), InterfaceIdMatches_(id));
}


/**
 * @class NodeClass
 *
 * @brief A class object for Node instances.
 *
 * NodeClass can be thought of as a "supertype" of sorts. A given node
 * implementation can support as many node types as there are unique
 * combinations of the interfaces it supports. The most readily apparent
 * role of the NodeClass object for a node implementation is to serve as
 * a factory for these @link NodeType NodeTypes@endlink.
 */

/**
 * @var Browser & NodeClass::browser
 *
 * @brief The Browser associated with this NodeClass.
 */

/**
 * @brief Constructor.
 *
 * A NodeClass is constructed using a Browser. All Node instances that share
 * a particular NodeClass "belong to" the Browser associated with the NodeClass.
 *
 * @param browser   the Browser to be associated with the NodeClass.
 */
NodeClass::NodeClass(Browser & browser) throw (): browser(browser) {}

/**
 * @brief Destructor.
 */
NodeClass::~NodeClass() throw () {}

/**
 * @brief NodeClass-specific initialization.
 *
 * This method is called during initialization of a Browser object with a new
 * root Scene. It is called after the individual node instances have been
 * initialized, and before the world starts running.
 *
 * @param time  the current time.
 */
void NodeClass::initialize(const double time) throw ()
{}

/**
 * @brief NodeClass-specific rendering.
 *
 * The default implementation of this method does nothing.
 *
 * @param viewer    the Viewer to render to.
 */
void NodeClass::render(Viewer & viewer) throw ()
{}

/**
 * @fn const NodeTypePtr NodeClass::createType(const std::string & id, const NodeInterfaceSet & interfaces) throw (std::invalid_argument, std::bad_alloc)
 *
 * @brief Create a new NodeType.
 *
 * @link NodeType NodeTypes@endlink can be said to subset the master type
 * provided by the NodeClass. Each NodeClass instance can support certain
 * node interfaces; the NodeInterfaceSet passed to createType must be a
 * subset of those supported interfaces.
 *
 * @param id            the name for the new NodeType.
 * @param interfaces    a NodeInterfaceSet containing the interfaces
 *                      for the new type.
 *
 * @return a NodeTypePtr to the newly created NodeType.
 *
 * @throw std::invalid_argument if the NodeClass cannot support one of the
 *                              @link NodeInterface NodeInterfaces@endlink in
 *                              @p interfaces.
 * @throw std::bad_alloc        if memory allocation fails.
 */


/**
 * @class NodeType
 *
 * @brief Type information object for @link Node Nodes@endlink.
 */

/**
 * @var NodeClass & NodeType::nodeClass
 *
 * @brief The class object associated with the NodeType.
 */

/**
 * @var const std::string NodeType::id
 *
 * @brief The name of the NodeType.
 */

/**
 * @brief Constructor.
 *
 * @param nodeClass the class object associated with the NodeType.
 * @param id        the name for the NodeType.
 *
 * @throw std::bad_alloc    if memory allocation fails.
 */
NodeType::NodeType(NodeClass & nodeClass, const std::string & id)
        throw (std::bad_alloc): nodeClass(nodeClass), id(id) {}

/**
 * @brief Destructor.
 */
NodeType::~NodeType() throw () {}

namespace {
    struct IsEventIn_ : public std::unary_function<NodeInterface, bool> {
    private:
        const std::string & id;

    public:
        IsEventIn_(const std::string & id): id(id) {}

        bool operator()(const NodeInterface & nodeInterface) const {
            return (nodeInterface.type == NodeInterface::eventIn
                        && (this->id == nodeInterface.id
                            || "set_" + this->id == nodeInterface.id))
                    || (nodeInterface.type == NodeInterface::exposedField
                        && (this->id == nodeInterface.id
                            || this->id == "set_" + nodeInterface.id));
        }
    };
}

/**
 * @brief Determine if the node type has an eventIn.
 *
 * If the node type supports an eventIn named @p id, this method will return
 * the type identifier corresponding to the data type handled by the eventIn.
 * Note that the actual interface type that supports the eventIn may be either
 * an eventIn or an exposedField.
 *
 * @param id    the name of the eventIn.
 *
 * @return the data type of the eventIn, or FieldValue::invalidType if no such
 *      eventIn exists.
 */
FieldValue::Type NodeType::hasEventIn(const std::string & id) const throw () {
    const NodeInterfaceSet & interfaces = this->getInterfaces();
    const NodeInterfaceSet::const_iterator end = interfaces.end();
    const NodeInterfaceSet::const_iterator pos =
            std::find_if(interfaces.begin(), end, IsEventIn_(id));
    if (pos == end) { return FieldValue::invalidType; }
    return pos->fieldType;
}

namespace {
    struct IsEventOut_ : public std::unary_function<NodeInterface, bool> {
    private:
        const std::string & id;

    public:
        IsEventOut_(const std::string & id): id(id) {}

        bool operator()(const NodeInterface & nodeInterface) const {
            return (nodeInterface.type == NodeInterface::eventOut
                        && (this->id == nodeInterface.id
                            || this->id + "_changed" == nodeInterface.id))
                    || (nodeInterface.type == NodeInterface::exposedField
                        && (this->id == nodeInterface.id
                            || this->id == nodeInterface.id + "_changed"));
        }
    };
}

/**
 * @brief Determine if the node type has an eventOut.
 *
 * If the node type supports an eventOut named @p id, this method will return
 * the type identifier corresponding to the data type handled by the eventOut.
 * Note that the actual interface type that supports the eventOut may be either
 * an eventOut or an exposedField.
 *
 * @param id    the name of the eventOut.
 *
 * @return the data type of the eventOut, or FieldValue::invalidType if no such
 *      eventOut exists.
 */
FieldValue::Type NodeType::hasEventOut(const std::string & id) const throw () {
    const NodeInterfaceSet & interfaces = this->getInterfaces();
    const NodeInterfaceSet::const_iterator end = interfaces.end();
    const NodeInterfaceSet::const_iterator pos =
            std::find_if(interfaces.begin(), end, IsEventOut_(id));
    if (pos == end) { return FieldValue::invalidType; }
    return pos->fieldType;
}

namespace {
    struct IsExposedField_ : public std::unary_function<NodeInterface, bool> {
    private:
        const std::string & id;

    public:
        IsExposedField_(const std::string & id): id(id) {}

        bool operator()(const NodeInterface & nodeInterface) const {
            return nodeInterface.type == NodeInterface::exposedField
                    && this->id == nodeInterface.id;
        }
    };
}

/**
 * @brief Determine if the node type has an exposedField.
 *
 * If the node type has an exposedField named @p id, this method will return
 * the type identifier corresponding to the data type handled by the
 * exposedField.
 *
 * @param id    the name of the exposedField.
 *
 * @return the data type of the exposedField, or FieldValue::invalidType if no
 *      such exposedField exists.
 */
FieldValue::Type NodeType::hasExposedField(const std::string & id) const
        throw () {
    const NodeInterfaceSet & interfaces = this->getInterfaces();
    const NodeInterfaceSet::const_iterator end = interfaces.end();
    const NodeInterfaceSet::const_iterator pos =
            std::find_if(interfaces.begin(), end, IsExposedField_(id));
    if (pos == end) { return FieldValue::invalidType; }
    return pos->fieldType;
}

namespace {
    struct IsField_ : public std::unary_function<NodeInterface, bool> {
    private:
        const std::string & id;

    public:
        IsField_(const std::string & id): id(id) {}

        bool operator()(const NodeInterface & nodeInterface) const {
            return nodeInterface.type == NodeInterface::field
                    && this->id == nodeInterface.id;
        }
    };
}

/**
 * @brief Determine if the node type has a field.
 *
 * If the node type has a field named @p id, this method will return the type
 * identifier corresponding to the data type accepted by the field.
 *
 * @param id    the name of the field.
 *
 * @return the data type of the field, or FieldValue::invalidType if no such
 *      field exists.
 */
FieldValue::Type NodeType::hasField(const std::string & id) const throw () {
    const NodeInterfaceSet & interfaces = this->getInterfaces();
    const NodeInterfaceSet::const_iterator end = interfaces.end();
    const NodeInterfaceSet::const_iterator pos =
            std::find_if(interfaces.begin(), end, IsField_(id));
    if (pos == end) { return FieldValue::invalidType; }
    return pos->fieldType;
}

/**
 * @fn const NodeInterfaceSet & NodeType::getInterfaces() const throw ()
 *
 * @brief Get the set of interfaces for the NodeType.
 *
 * @return the set of interfaces.
 */

/**
 * @fn const NodePtr NodeType::createNode(const ScopePtr & scope) const throw (std::bad_alloc)
 *
 * @brief Create a new Node with this NodeType.
 *
 * @param scope         the Scope that the new node should belong to.
 *
 * @return a NodePtr to a new Node.
 *
 * @excpetion std::bad_alloc    if memory allocation fails.
 */


/**
 * @class FieldValueTypeMismatch
 *
 * @brief Thrown when field value types do not match, generally in a @c ROUTE
 *      or @c IS.
 */

/**
 * @brief Constructor.
 */
FieldValueTypeMismatch::FieldValueTypeMismatch():
    std::runtime_error("Field value types do not match.")
{}

/**
 * @brief Destructor.
 */
FieldValueTypeMismatch::~FieldValueTypeMismatch() throw ()
{}


/**
 * @class Node
 *
 * @brief A node in the scene graph.
 */

/**
 * @struct Node::Route
 *
 * @brief A route from one node to another through which events propagate.
 */

/**
 * @var const std::string Node::Route::fromEventOut
 *
 * @brief The name of the eventOut the route is coming from.
 */

/**
 * @var const NodePtr Node::Route::toNode
 *
 * @brief The node the route is going to.
 */

/**
 * @var const std::string Node::Route::toEventIn
 *
 * @brief The name of the eventIn on @a toNode that the route is going to.
 */

/**
 * @brief Constructor.
 *
 * @param fromEventOut  the name of the eventOut the route is coming from.
 * @param toNode        the node the route is going to.
 * @param toEventIn     the name of an eventIn on @p toNode that the route is
 *                      going to.
 */
Node::Route::Route(const std::string & fromEventOut,
                   const NodePtr & toNode, const std::string & toEventIn):
        fromEventOut(fromEventOut), toNode(toNode),
	toEventIn(toEventIn) {}

/**
 * @brief Copy constructor.
 *
 * @param route the Route to copy.
 */
Node::Route::Route(const Route & route): fromEventOut(route.fromEventOut),
	toNode(route.toNode), toEventIn(route.toEventIn) {}

/**
 * @fn bool operator==(const Node::Route & lhs, const Node::Route & rhs)
 *
 * @relates Node::Route
 *
 * @brief Compare two @link Route Routes@endlink for equality.
 *
 * @param lhs   a Route.
 * @param rhs   a Route.
 *
 * @return @c true if the @link Route Routes@link are identical, @c false
 *      otherwise.
 */

/**
 * @fn bool operator!=(const Node::Route & lhs, const Node::Route & rhs)
 *
 * @relates Node::Route
 *
 * @brief Compare two @link Route Routes@endlink for inequality.
 *
 * @param lhs   a Route.
 * @param rhs   a Route.
 *
 * @return @c true if the @link Route Routes@link are not identical, @c false
 *      otherwise.
 */

/**
 * @struct Node::PolledEventOutValue
 *
 * @brief Simple struct for use in implementing nodes that are polled for
 *      pending events.
 */

/**
 * @var const FieldValuePtr Node::PolledEventOutValue::value
 *
 * @brief The value.
 */

/**
 * @var bool Node::PolledEventOutValue::modified
 *
 * @brief A flag to indicate whether the eventOut has been modified.
 */

/**
 * @brief Default constructor.
 */
Node::PolledEventOutValue::PolledEventOutValue():
    modified(false)
{}

/**
 * @brief Constructor.
 *
 * @param value     the value.
 * @param modified  a flag to indicate whether the eventOut has been modified.
 */
Node::PolledEventOutValue::PolledEventOutValue(const FieldValuePtr & value,
                                               const bool modified):
        value(value), modified(modified) {}

/**
 * @var Node::nodeType
 *
 * @brief The type information object for the node.
 */

/**
 * @brief Constructor.
 *
 * @param type  the NodeType associated with the instance.
 * @param scope the Scope associated with the instance.
 */
Node::Node(const NodeType & type, const ScopePtr & scope):
        scope(scope),
        scene(0),
        nodeType(type),
        d_modified(false),
        d_bvol_dirty(false),
        visited(false) {}

typedef std::map<std::string, Node *> NamedNodeMap;

namespace {

    struct NodeIs_ : std::unary_function<NamedNodeMap::value_type, bool> {
        NodeIs_(const Node & node): node(&node) {}

        bool operator()(const NamedNodeMap::value_type & value) const {
            return value.second == this->node;
        }

    private:
        const Node * node;
    };
}

/**
 * @brief Destructor.
 *
 * Remove node name (if any) from the scope.
 */
Node::~Node() throw () {
    //
    // If this is the primordial node in a prototype definition, this->scope
    // will be null.
    //
    if (this->scope) {
        using std::find_if;
        const NamedNodeMap::iterator end = this->scope->namedNodeMap.end();
        const NamedNodeMap::iterator pos =
                find_if(this->scope->namedNodeMap.begin(), end, NodeIs_(*this));
        if (pos != end) { this->scope->namedNodeMap.erase(pos); }
    }
}

/**
 * @brief Accept a visitor.
 *
 * If the node has not been visited, set the @a visited flag to @c true and
 * call NodeVisitor::visit for this object. Otherwise (if the @a visited flag
 * is already @c true), this method has no effect.
 *
 * <p>The fact that the @a visited flag is set <em>before</em> the
 * node is actually visited is an important detail. Even though scene
 * graphs should not have cycles, nodes can be self-referencing: a field
 * of a Script node can legally @c USE the Script node. (This does not pose
 * a problem for rendering since nodes in a Script node's fields are not part
 * of the transformation hierarchy.)
 *
 * @param visitor   a NodeVisitor.
 *
 * @return @c true if the visitor is accepted (the node
 *         <em>has not</em> been visited during this traversal),
 *         @c false otherwise (the node <em>has</em> been
 *         visited during this traversal).
 */
bool Node::accept(NodeVisitor & visitor) {
    if (!this->visited) {
        this->visited = true;
        visitor.visit(*this);
        return true;
    }
    return false;
}

/**
 * @brief Recursively set the @a visited flag to @c false for this node and
 *      its children.
 *
 * Typically used by a visitor (a class that implements NodeVisitor)
 * after traversal is complete.
 */
void Node::resetVisitedFlag() throw ()
{
    if (this->visited) {
        //
        // Set this node's visited flag to false.
        //
        this->visited = false;

        //
        // Recursively call this method on any child nodes.
        //
        const NodeInterfaceSet & interfaces = this->nodeType.getInterfaces();
        for (NodeInterfaceSet::const_iterator interface(interfaces.begin());
                interface != interfaces.end(); ++interface) {
            if (interface->type == NodeInterface::exposedField
                    || interface->type == NodeInterface::field) {
                if (interface->fieldType == FieldValue::sfnode) {
                    const SFNode & sfnode =
                        static_cast<const SFNode &>(this->getField(interface->id));
                    if (sfnode.get()) { sfnode.get()->resetVisitedFlag(); }
                } else if (interface->fieldType == FieldValue::mfnode) {
                    const MFNode & mfnode =
                        static_cast<const MFNode &>(this->getField(interface->id));
                    for (size_t i = 0; i < mfnode.getLength(); ++i) {
                        if (mfnode.getElement(i)) {
                            mfnode.getElement(i)->resetVisitedFlag();
                        }
                    }
                }
            }
        }
    }
}

/**
 * @brief Set the nodeId of the node.
 *
 * @param nodeId   the ID for the node.
 */
void Node::setId(const std::string & nodeId) {
    assert(this->scope);
    this->scope->namedNodeMap[nodeId] = this;
}

/**
 * @brief Retrieve the nodeId of this node.
 *
 * @return the nodeId
 */
const std::string Node::getId() const {
    using std::find_if;
    assert(this->scope);
    const NamedNodeMap::iterator end = this->scope->namedNodeMap.end();
    const NamedNodeMap::iterator pos =
            find_if(this->scope->namedNodeMap.begin(), end, NodeIs_(*this));
    return (pos != end) ? pos->first : std::string();
}

void Node::addEventOutIS(const std::string & eventOutId,
                         PolledEventOutValue * const eventOutValue)
        throw (UnsupportedInterface, std::bad_alloc) {
    if (!this->nodeType.hasEventOut(eventOutId)) {
        throw UnsupportedInterface(this->nodeType.id
                                   + " node has no eventOut \"" + eventOutId
                                   + "\"");
    }
    const EventOutISMap::value_type value(eventOutId, eventOutValue);
    this->eventOutISMap.insert(value);
}

/**
 * @brief Initialize the Node.
 *
 * This method works recursively, initializing any child nodes to the same
 * @p scene and @p timestamp. If the node has already been initialized, this
 * method has no effect.
 *
 * @param scene     the Scene to which the Node will belong.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void Node::initialize(Scene & scene, const double timestamp)
    throw (std::bad_alloc)
{
    if (!this->scene) {
        this->scene = &scene;
        this->initializeImpl(timestamp);

        const NodeInterfaceSet & interfaces = this->nodeType.getInterfaces();
        for (NodeInterfaceSet::const_iterator interface(interfaces.begin());
                interface != interfaces.end(); ++interface) {
            if (interface->type == NodeInterface::exposedField
                    || interface->type == NodeInterface::field) {
                if (interface->fieldType == FieldValue::sfnode) {
                    assert(dynamic_cast<const SFNode *>
                           (&this->getField(interface->id)));
                    const SFNode & sfnode = static_cast<const SFNode &>
                                            (this->getField(interface->id));
                    if (sfnode.get()) {
                        sfnode.get()->initialize(scene, timestamp);
                    }
                } else if (interface->fieldType == FieldValue::mfnode) {
                    assert(dynamic_cast<const MFNode *>
                           (&this->getField(interface->id)));
                    const MFNode & mfnode = static_cast<const MFNode &>
                                            (this->getField(interface->id));
                    for (size_t i = 0; i < mfnode.getLength(); ++i) {
                        if (mfnode.getElement(i)) {
                            mfnode.getElement(i)->initialize(scene, timestamp);
                        }
                    }
                }
            }
        }
    }
    assert(this->scene == &scene);
}

/**
 * @brief Generalized field mutator.
 *
 * @param id    the name of the field.
 * @param value the new value.
 *
 * @throw UnsupportedInterface  if the node has no field named @p id.
 * @throw std::bad_cast         if @p value is not the correct type.
 * @throw std::bad_alloc        if memory allocation fails.
 *
 * @pre @p value must be the appropriate type for the interface.
 */
void Node::setField(const std::string & id, const FieldValue & value)
        throw (UnsupportedInterface, std::bad_cast, std::bad_alloc) {
    this->setFieldImpl(id, value);
}

/**
 * @brief Generalized field accessor.
 *
 * @param id    the name of the field.
 *
 * @throw UnsupportedInterface  if the node has no field named @p id.
 */
const FieldValue & Node::getField(const std::string & id) const
        throw (UnsupportedInterface) {
    return this->getFieldImpl(id);
}

/**
 * @brief Process an event.
 *
 * @param id        the name of the eventIn to which the event is being sent.
 * @param value     the new value.
 * @param timestamp the current time.
 *
 * @throw UnsupportedInterface  if the node has no eventIn named @p id.
 * @throw std::bad_cast         if @p value is not the correct type.
 * @throw std::bad_alloc        if memory allocation fails.
 *
 * @pre @p value must be the appropriate type for the interface.
 */
void Node::processEvent(const std::string & id, const FieldValue & value,
                        const double timestamp)
        throw (UnsupportedInterface, std::bad_cast, std::bad_alloc) {
    this->processEventImpl(id, value, timestamp);
}


/**
 * @brief Generalized eventOut accessor.
 *
 * @param id    the name of the eventOut.
 *
 * @throw UnsupportedInterface  if the node has no eventOut named @p id.
 */
const FieldValue & Node::getEventOut(const std::string & id) const
        throw (UnsupportedInterface) {
    return this->getEventOutImpl(id);
}

/**
 * @brief Cast to a const ScriptNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const ScriptNode * Node::toScript() const throw () { return 0; }

/**
 * @brief Cast to a ScriptNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
ScriptNode * Node::toScript() throw () { return 0; }

/**
 * @brief Cast to a const AppearanceNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const AppearanceNode * Node::toAppearance() const throw () { return 0; }

/**
 * @brief Cast to an AppearanceNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
AppearanceNode * Node::toAppearance() throw () { return 0; }

/**
 * @brief Cast to a const ChildNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const ChildNode * Node::toChild() const throw () { return 0; }

/**
 * @brief Cast to a ChildNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
ChildNode * Node::toChild() throw () { return 0; }

/**
 * @brief Cast to a const ColorNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const ColorNode * Node::toColor() const throw () { return 0; }

/**
 * @brief Cast to a ColorNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
ColorNode * Node::toColor() throw () { return 0; }

/**
 * @brief Cast to a const CoordinateNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const CoordinateNode * Node::toCoordinate() const throw () { return 0; }

/**
 * @brief Cast to a CoordinateNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
CoordinateNode * Node::toCoordinate() throw () { return 0; }

/**
 * @brief Cast to a const FontStyleNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const FontStyleNode * Node::toFontStyle() const throw () { return 0; }

/**
 * @brief Cast to a FontStyleNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
FontStyleNode * Node::toFontStyle() throw () { return 0; }

/**
 * @brief Cast to a const GeometryNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const GeometryNode * Node::toGeometry() const throw () { return 0; }

/**
 * @brief Cast to a GeometryNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
GeometryNode * Node::toGeometry() throw () { return 0; }

/**
 * @brief Cast to a const MaterialNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const MaterialNode * Node::toMaterial() const throw () { return 0; }

/**
 * @brief Cast to a MaterialNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
MaterialNode * Node::toMaterial() throw () { return 0; }

/**
 * @brief Cast to a const NormalNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const NormalNode * Node::toNormal() const throw () { return 0; }

/**
 * @brief Cast to a NormalNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
NormalNode * Node::toNormal() throw () { return 0; }

/**
 * @brief Cast to a const SoundSourceNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const SoundSourceNode * Node::toSoundSource() const throw () { return 0; }

/**
 * @brief Cast to a SoundSourceNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
SoundSourceNode * Node::toSoundSource() throw () { return 0; }

/**
 * @brief Cast to a const TextureNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const TextureNode * Node::toTexture() const throw () { return 0; }

/**
 * @brief Cast to a TextureNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
TextureNode * Node::toTexture() throw () { return 0; }

/**
 * @brief Cast to a const TextureCoordinateNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const TextureCoordinateNode * Node::toTextureCoordinate() const throw () {
    return 0;
}

/**
 * @brief Cast to a TextureCoordinateNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
TextureCoordinateNode * Node::toTextureCoordinate() throw () { return 0; }

/**
 * @brief Cast to a const TextureTransformNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
const TextureTransformNode * Node::toTextureTransform() const throw () {
    return 0;
}

/**
 * @brief Cast to a TextureTransformNode.
 *
 * Default implementation returns 0.
 *
 * @return 0
 */
TextureTransformNode * Node::toTextureTransform() throw () { return 0; }


// Safe node downcasts. These avoid the dangerous casts of Node* (esp in
// presence of protos), but are ugly in that this class must know about all
// the subclasses. These return 0 if the typecast is invalid.
// Remember to also add new ones to NodeProto. Protos should
// return their first implementation node (except toProto()).

Vrml97Node::Anchor * Node::toAnchor() const { return 0; }

Vrml97Node::AudioClip * Node::toAudioClip() const { return 0; }

Vrml97Node::Group * Node::toGroup() const { return 0; }

Vrml97Node::AbstractLight * Node::toLight() const { return 0; }

Vrml97Node::MovieTexture * Node::toMovieTexture() const { return 0; }

Vrml97Node::NavigationInfo * Node::toNavigationInfo() const { return 0; }

Vrml97Node::PlaneSensor * Node::toPlaneSensor() const { return 0; }

Vrml97Node::SphereSensor * Node::toSphereSensor() const { return 0; }

Vrml97Node::CylinderSensor * Node::toCylinderSensor() const { return 0; }

Vrml97Node::PointLight * Node::toPointLight() const { return 0; }

Vrml97Node::SpotLight * Node::toSpotLight() const { return 0; }

Vrml97Node::TimeSensor * Node::toTimeSensor() const { return 0; }

Vrml97Node::TouchSensor * Node::toTouchSensor() const { return 0; }

Vrml97Node::Viewpoint * Node::toViewpoint() const { return 0; }


/**
 * @brief Add a route from an eventOut of this node to an eventIn of another
 *      node.
 *
 * If the route being added already exists, this method has no effect.
 *
 * @param fromEventOut  an eventOut of the node.
 * @param toNode        a node.
 * @param toEventIn     an eventIn of @p toNode.
 *
 * @exception UnsupportedInterface      if the node has no eventOut
 *                                      @p fromEventOut; or if @p toNode has no
 *                                      eventIn @p toEventIn.
 * @exception FieldValueTypeMismatch    if @p fromEventOut and @p toEventIn
 *                                      have different field value types.
 *
 * @pre @p toNode is not null.
 */
void Node::addRoute(const std::string & fromEventOut,
                    const NodePtr & toNode,
                    const std::string & toEventIn)
    throw (UnsupportedInterface, FieldValueTypeMismatch)
{
    assert(toNode);

    const FieldValue::Type fromInterfaceType =
            this->nodeType.hasEventOut(fromEventOut);
    if (fromInterfaceType == FieldValue::invalidType) {
        throw UnsupportedInterface(this->nodeType, NodeInterface::eventOut,
                                   fromEventOut);
    }

    const FieldValue::Type toInterfaceType =
            toNode->nodeType.hasEventIn(toEventIn);
    if (toInterfaceType == FieldValue::invalidType) {
        throw UnsupportedInterface(toNode->nodeType, NodeInterface::eventIn,
                                   toEventIn);
    }

    if (fromInterfaceType != toInterfaceType) {
        throw FieldValueTypeMismatch();
    }

    const Route route(fromEventOut, toNode, toEventIn);

    //
    // Is this route already here?
    //
    const RouteList::iterator pos =
            std::find(this->routes.begin(), this->routes.end(), route);

    //
    // If not, add it.
    //
    if (pos == this->routes.end()) { this->routes.push_back(route); }
}


/**
 * @brief Remove a route from an eventOut of this node to an eventIn of another
 *      node.
 *
 * If no such route exists, this method has no effect.
 *
 * @param fromEventOut  an eventOut of the node.
 * @param toNode        a node.
 * @param toEventIn     an eventIn of @p toNode.
 */
void Node::deleteRoute(const std::string & fromEventOut,
                       const NodePtr & toNode,
                       const std::string & toEventIn) throw ()
{
    const RouteList::iterator pos =
            std::find(this->routes.begin(), this->routes.end(),
                      Route(fromEventOut, toNode, toEventIn));
    if (pos != this->routes.end()) { this->routes.erase(pos); }
}

/**
 * @brief Get the routes from this node.
 *
 * @return an std::vector of Routes from this node.
 */
const Node::RouteList & Node::getRoutes() const {
    return this->routes;
}

/**
 * Dirty bit - indicates node needs to be revisited for rendering.
 */
void Node::setModified() {
    this->d_modified = true;
    this->nodeType.nodeClass.browser.setModified();
}

bool Node::isModified() const { return this->d_modified; }


/**
 * @brief Mark all the nodes in the path as (not) modified.
 *
 * Convenience function used by updateModified.
 *
 * @param path  stack of ancestor nodes.
 * @param mod   set modified flag to this value.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void Node::markPathModified(NodePath& path, bool mod, int flags) {
  NodePath::iterator i;
  NodePath::iterator end = path.end();
  if (flags & 0x001) {
    for (i = path.begin(); i != end; ++i) {
      Node *c = *i;
      if (mod) {
	// do the proof that our invarient shows that this short
	// circuit is legal...
	c->setModified();
      } else
	c->clearModified();
    }
  }
  if (flags & 0x002) {
    for (i = path.begin(); i != end; ++i) {
      Node *c = *i;
      if (mod) {
	c->setBVolumeDirty(true);
      } else
	c->setBVolumeDirty(false);
    }
  }
}

/**
 * @brief Propagate the bvolume dirty flag from children to parents.
 *
 * I don't like this at all, but it's not worth making it pretty
 * because the need for it will go away when parent pointers are
 * implemented.
 *
 * @param path  stack of ancestor nodes.
 * @param flags 1 indicates normal modified flag, 2 indicates the
 *              bvolume dirty flag, 3 indicates both.
 */
void Node::updateModified(NodePath & path, int flags)
{
    if (this->d_modified || this->d_bvol_dirty) {
        this->markPathModified(path, true, flags);
    }
}

// note not virtual
//
void Node::updateModified(int flags)
{
  NodePath path;
  updateModified(path, flags);
}


void Node::clearFlags()
{
  d_flag = false;
}

// Render

//bool Node::cull(Viewer *v, RenderContext* c)
//{
//if (c && c->getCullFlag()) {
//VrmlBVolume* bv = this->getBVolume();
//int r = v->isectViewVolume(*bv); // better not be null...
//if (r == VrmlBVolume::BV_OUTSIDE) {
//cout << "Node::render():OUTSIDE:culled" << endl;
//return true;
//} else if (r == VrmlBVolume::BV_INSIDE) {
//cout << "Node::render():INSIDE:no more cull tests" << endl;
//c->setCullFlag(false);
//return false;
//} else {
//cout << "Node::render():PARTIAL:continue cull tests" << endl;
//return false;
//}
//}
//return false;
//}


/**
 * @brief Get this node's bounding volume.
 *
 * Nodes that have no bounding volume, or have a difficult to calculate
 * bvolume (like, say, Extrusion or Billboard) can just return an infinite
 * bsphere. Note that returning an infinite bvolume means that all the node's
 * ancestors will also end up with an infinite bvolume, and will never be
 * culled.
 *
 * @return this node's bounding volume
 *
 * @todo Should this return a reference?
 */
const BVolume * Node::getBVolume() const {
    static BSphere infBSphere;
    static BSphere * infBSpherePtr = 0;
    if (!infBSpherePtr) {
        infBSpherePtr = &infBSphere;
        infBSpherePtr->setMAX();
    }
    const_cast<Node *>(this)->setBVolumeDirty(false);
    return infBSpherePtr;
}


/**
 * Override a node's calculated bounding volume. Not implemented.
 *
 * @todo Implement me!
 */
void Node::setBVolume(const BVolume & v) {
    // XXX Implement me!
}

/**
 * Indicate that a node's bounding volume needs to be recalculated
 * (or not). If a node's bvolume is invalid, then the bvolumes of
 * all that node's ancestors are also invalid. Normally, the node
 * itself will determine when its bvolume needs updating.
 */
void Node::setBVolumeDirty(bool f) {
    this->d_bvol_dirty = f;
    if (f) { // only if dirtying, not clearing
        this->nodeType.nodeClass.browser.d_flags_need_updating = true;
    }
}

/**
 * Return true if the node's bounding volume needs to be
 * recalculated.
 */
bool Node::isBVolumeDirty() const {
    if (this->nodeType.nodeClass.browser.d_flags_need_updating) {
        this->nodeType.nodeClass.browser.updateFlags();
        this->nodeType.nodeClass.browser.d_flags_need_updating = false;
    }
    return this->d_bvol_dirty;
}

/**
 * @brief Render this node.
 *
 * Actually, most of the rendering work is delegated to the viewer, but this
 * method is responsible for traversal to the node's renderable children,
 * including culling. Each node class needs to implement this routine
 * appropriately. It's not abstract since it doesn't make sense to call render
 * on some nodes. Alternative would be to break render out into a seperate
 * mixins class, but that's probably overkill.
 *
 * @param viewer    viewer implementation responsible for actually doing the
 *                  drawing.
 * @param context   generic context argument; holds things like the accumulated
 *                  modelview transform.
 */
void Node::render(Viewer & viewer, VrmlRenderContext context)
{
  //if (cull(v, c)) return;
  clearModified();
}

/**
 * @brief Accumulate transformations for proper rendering of bindable nodes.
 *
 * Cache a pointer to one of the parent transforms. The resulting
 * pointer is used by getParentTransform. Grouping nodes need to
 * redefine this, the default implementation does nothing.
 *
 * @param parent    the parent node.
 */
void Node::accumulateTransform(Node * parent) {}

/**
 * @brief Get the nearest ancestor node that affects the modelview transform.
 *
 * Doesn't work for nodes with more than one parent.
 *
 * @return the nearest ancestor node that affects the modelview
 *      transform.
 */
Node * Node::getParentTransform() { return 0; }

/**
 * Compute the inverse of the transform stack above a Viewpoint
 * node. This is safe since the behavior of multi-parented
 * Viewpoint nodes is undefined. May be called at any time.
 *
 * @param M return the accumulated inverse
 *
 * @see accumulateTransform
 * @see getParentTransform
 */
void Node::inverseTransform(VrmlMatrix & M) {
    Node * parentTransform = getParentTransform();
    if (parentTransform) {
        parentTransform->inverseTransform(M);
    } else {
        M = VrmlMatrix(); // Set to identity.
    }
}

/**
 * @brief Send an event from this node.
 */
void Node::emitEvent(const std::string & id, const FieldValue & value,
                     const double timestamp)
        throw (std::bad_cast, std::bad_alloc) {
    assert(this->getScene());
    EventOutISMap::const_iterator pos = this->eventOutISMap.find(id);
    if (pos != this->eventOutISMap.end()) {
        pos->second->value->assign(value);
        pos->second->modified = true;
    }

    for (RouteList::const_iterator itr = this->routes.begin();
            itr != this->routes.end(); ++itr) {
        if (id == itr->fromEventOut) {
            FieldValue * const eventValue = value.clone().release();
            this->getScene()->browser.queueEvent(timestamp, eventValue,
                                                 itr->toNode, itr->toEventIn);
        }
    }
}

namespace {
    const short indentIncrement_ = 4;

    class PrintField_ : public std::unary_function<NodeInterface, void> {
        const Node & node;
        std::ostream & out;
        const size_t indent;

    public:
        PrintField_(const Node & node, std::ostream & out, const size_t indent):
                node(node), out(out), indent(indent) {}

        void operator()(const NodeInterface & interface) const {
            if (interface.type == NodeInterface::exposedField
                    || interface.type == NodeInterface::field) {
                this->out << std::string(this->indent + indentIncrement_, ' ')
                          << interface.id << ' ' << node.getField(interface.id);
            }
        }
    };
}

std::ostream & Node::print(std::ostream & out, const size_t indent) const {
    for (size_t i = 0; i < indent; ++i) { out << ' '; }
    std::string nodeId = this->getId();
    if (!nodeId.empty()) { out << "DEF " << nodeId << " "; }
    out << this->nodeType.id << " { ";
    const NodeInterfaceSet & interfaces = this->nodeType.getInterfaces();
    std::for_each(interfaces.begin(), interfaces.end(),
                  PrintField_(*this, out, indent));
    return out << " }";
}

std::ostream & operator<<(std::ostream & out, const Node & node) {
    return node.print(out, 0);
}

/**
 * @brief Node subclass-specific initialization.
 *
 * This method is called by Node::initialize. Subclasses of Node should
 * override this method for any subclass-specific initialization. Note that
 * this method cannot throw.
 *
 * The default implementation of this method does nothing.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void Node::initializeImpl(const double timestamp) throw (std::bad_alloc)
{}


/**
 * @class AppearanceNode
 *
 * @brief Abstract base class for appearance nodes.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
AppearanceNode::AppearanceNode(const NodeType & nodeType,
                               const ScopePtr & scope):
        Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
AppearanceNode::~AppearanceNode() throw () {}

/**
 * @brief Cast to an AppearanceNode.
 *
 * @return a pointer to this AppearanceNode.
 */
const AppearanceNode * AppearanceNode::toAppearance() const throw () {
    return this;
}

/**
 * @brief Cast to an AppearanceNode.
 *
 * @return a pointer to this AppearanceNode.
 */
AppearanceNode * AppearanceNode::toAppearance() throw () { return this; }

/**
 * @fn const SFNode & AppearanceNode::getMaterial() const throw ()
 *
 * @brief Get the material node associated with this appearance node.
 *
 * @return the MaterialNode associated with this AppearanceNode.
 */

/**
 * @fn const SFNode & AppearanceNode::getTexture() const throw ()
 *
 * @brief Get the texture node associated with this appearance node.
 *
 * @return the TextureNode associated with this AppearanceNode.
 */

/**
 * @fn const SFNode & AppearanceNode::getTextureTransform() const throw ()
 *
 * @brief Get the texture transform node associated with this appearance node.
 *
 * @return the TextureTransformNode associated with this AppearanceNode.
 */


/**
 * @class ChildNode
 *
 * @brief Abstract base class for child nodes.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
ChildNode::ChildNode(const NodeType & nodeType, const ScopePtr & scope):
        Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
ChildNode::~ChildNode() throw () {}

/**
 * @brief Cast to a ChildNode.
 *
 * @return a pointer to this ChildNode.
 */
const ChildNode * ChildNode::toChild() const throw () { return this; }

/**
 * @brief Cast to a ChildNode.
 *
 * @return a pointer to this ChildNode.
 */
ChildNode * ChildNode::toChild() throw () { return this; }


/**
 * @class ColorNode
 *
 * @brief Abstract base class for color nodes.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
ColorNode::ColorNode(const NodeType & nodeType, const ScopePtr & scope):
        Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
ColorNode::~ColorNode() throw () {}

/**
 * @brief Cast to a ColorNode.
 *
 * @return a pointer to this ColorNode.
 */
const ColorNode * ColorNode::toColor() const throw () { return this; }

/**
 * @brief Cast to a ColorNode.
 *
 * @return a pointer to this ColorNode.
 */
ColorNode * ColorNode::toColor() throw () { return this; }

/**
 * @fn const MFColor & ColorNode::getColor() const
 *
 * @brief Get the color array encapsulated by this node.
 *
 * @return the MFColor for this node.
 */


/**
 * @class CoordinateNode
 *
 * @brief Abstract base class for coordinate nodes.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
CoordinateNode::CoordinateNode(const NodeType & nodeType,
                               const ScopePtr & scope)
        : Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
CoordinateNode::~CoordinateNode() throw () {}

/**
 * @brief Cast to a CoordinateNode.
 *
 * @return a pointer to this CoordinateNode.
 */
const CoordinateNode * CoordinateNode::toCoordinate() const throw () {
    return this;
}

/**
 * @brief Cast to a CoordinateNode.
 *
 * @return a pointer to this CoordinateNode.
 */
CoordinateNode * CoordinateNode::toCoordinate() throw () { return this; }

/**
 * @fn const MFVec3f & CoordinateNode::getPoint() const
 *
 * @brief Get the points encapsulated by this node.
 *
 * @return the MFVec3f array of points for this node.
 */


/**
 * @class FontStyleNode
 *
 * @brief Abstract base class for font style nodes.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
FontStyleNode::FontStyleNode(const NodeType & nodeType, const ScopePtr & scope):
        Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
FontStyleNode::~FontStyleNode() throw () {}

/**
 * @brief Cast to a FontStyleNode.
 *
 * @return a pointer to this FontStyleNode.
 */
const FontStyleNode * FontStyleNode::toFontStyle() const throw () {
    return this;
}

/**
 * @brief Cast to a FontStyleNode.
 *
 * @return a pointer to this FontStyleNode.
 */
FontStyleNode * FontStyleNode::toFontStyle() throw () { return this; }

/**
 * @fn const MFString & FontStyleNode::getFamily() const throw ()
 *
 * @brief Get the list of font families.
 *
 * @return an MFString containing the font families that may be used for this
 *      FontStyle.
 */

/**
 * @fn const SFBool & FontStyleNode::getHorizontal() const throw ()
 *
 * @brief Get the flag indicating whether the text should be horizontal or
 *      vertical.
 *
 * @return @c TRUE if the text should be horizontal, or @c FALSE if the text
 *      should be vertical.
 */

/**
 * @fn const MFString & FontStyleNode::getJustify() const throw ()
 *
 * @brief Get the descriptor for the text justification.
 *
 * @return an MFString value describing the characteristics of the text
 *      justification.
 */

/**
 * @fn const SFString & FontStyleNode::getLanguage() const throw ()
 *
 * @brief Get the language code.
 *
 * @return the language code.
 */

/**
 * @fn const SFBool & FontStyleNode::getLeftToRight() const throw ()
 *
 * @brief Get the flag indicating whether the text should be rendered
 *      left-to-right.
 *
 * @return @c TRUE if the text should be rendered left-to-right, or @c FALSE if
 *      the text should be rendered right-to-left.
 */

/**
 * @fn const SFFloat & FontStyleNode::getSize() const throw ()
 *
 * @brief Get the size of the text.
 *
 * @return the size of the text.
 */

/**
 * @fn const SFFloat & FontStyleNode::getSpacing() const throw ()
 *
 * @brief Get the spacing for the text.
 *
 * @return the spacing for the text.
 */

/**
 * @fn const SFString & FontStyleNode::getStyle() const throw ()
 *
 * @brief Get the style for the text.
 *
 * @return an SFString descriptor of the text style.
 */

/**
 * @fn const SFBool & FontStyleNode::getTopToBottom() const throw ()
 *
 * @brief Get the flag indicating whether the text should be rendered
 *      top-to-bottom.
 *
 * @return @c TRUE if the text should be rendered top-to-bottom, or @c FALSE if
 *      the text should be rendered bottom-to-top.
 */


/**
 * @class GeometryNode
 *
 * @brief Abstract base class for geometry nodes.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
GeometryNode::GeometryNode(const NodeType & nodeType, const ScopePtr & scope):
        Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
GeometryNode::~GeometryNode() throw () {}

/**
 * @brief Cast to a GeometryNode.
 *
 * @return a pointer to this GeometryNode.
 */
const GeometryNode * GeometryNode::toGeometry() const throw () { return this; }

/**
 * @brief Cast to a GeometryNode.
 *
 * @return a pointer to this GeometryNode.
 */
GeometryNode * GeometryNode::toGeometry() throw () { return this; }

/**
 * @brief Get the color node (if any) associated with this geometry.
 *
 * @return the ColorNode associated associated with this geometry, or 0 if
 *      there is no such node.
 *
 * @todo Reevaluate the way the renderer visits geometry nodes; potentially
 *      eliminate this method.
 */
const ColorNode * GeometryNode::getColor() const throw () { return 0; }


/**
 * @class MaterialNode
 *
 * @brief Abstract base class for material nodes.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
MaterialNode::MaterialNode(const NodeType & nodeType, const ScopePtr & scope):
        Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
MaterialNode::~MaterialNode() throw () {}

/**
 * @brief Cast to a MaterialNode.
 *
 * @return a pointer to this MaterialNode.
 */
const MaterialNode * MaterialNode::toMaterial() const throw () { return this; }

/**
 * @brief Cast to a MaterialNode.
 *
 * @return a pointer to this MaterialNode.
 */
MaterialNode * MaterialNode::toMaterial() throw () { return this; }

/**
 * @fn const SFFloat & MaterialNode::getAmbientIntensity() const throw ()
 *
 * @brief Get the ambient intensity.
 *
 * @return the ambient intensity.
 */

/**
 * @fn const SFColor & MaterialNode::getDiffuseColor() const throw ()
 *
 * @brief Get the diffuse color.
 *
 * @return the diffuse color.
 */

/**
 * @fn const SFColor & MaterialNode::getEmissiveColor() const throw ()
 *
 * @brief Get the emissive color.
 *
 * @return the emissive color.
 */

/**
 * @fn const SFFloat & MaterialNode::getShininess() const throw ()
 *
 * @brief Get the shininess.
 *
 * @return the shininess.
 */

/**
 * @fn const SFColor & MaterialNode::getSpecularColor() const throw ()
 *
 * @brief Get the specular color.
 *
 * @return the specular color.
 */

/**
 * @fn const SFFloat & MaterialNode::getTransparency() const throw ()
 *
 * @brief Get the transparency.
 *
 * @return the transparency.
 */


/**
 * @class NormalNode
 *
 * @brief Abstract base class for normal nodes.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
NormalNode::NormalNode(const NodeType & nodeType, const ScopePtr & scope):
        Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
NormalNode::~NormalNode() throw () {}

/**
 * @brief Cast to a NormalNode.
 *
 * @return a pointer to this NormalNode.
 */
const NormalNode * NormalNode::toNormal() const throw () { return this; }

/**
 * @brief Cast to a NormalNode.
 *
 * @return a pointer to this NormalNode.
 */
NormalNode * NormalNode::toNormal() throw () { return this; }

/**
 * @fn const MFVec3f & NormalNode::getVector() const throw ()
 *
 * @brief Get the array of normal vectors.
 *
 * @return the array of normal vectors.
 */


/**
 * @class SoundSourceNode
 *
 * @brief Abstract base class for sound source nodes.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
SoundSourceNode::SoundSourceNode(const NodeType & nodeType,
                                 const ScopePtr & scope):
        Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
SoundSourceNode::~SoundSourceNode() throw () {}

/**
 * @brief Cast to a SoundSourceNode.
 *
 * @return a pointer to this SoundSourceNode.
 */
const SoundSourceNode * SoundSourceNode::toSoundSource() const throw () {
    return this;
}

/**
 * @brief Cast to a SoundSourceNode.
 *
 * @return a pointer to this SoundSourceNode.
 */
SoundSourceNode * SoundSourceNode::toSoundSource() throw () { return this; }


/**
 * @class TextureNode
 *
 * @brief Abstract base class for texture nodes.
 *
 * @todo Clean this up. It would be better just to have an accessor for an
 *      SFImage along with the repeatS and repeatT accessors.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
TextureNode::TextureNode(const NodeType & nodeType, const ScopePtr & scope):
        Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
TextureNode::~TextureNode() throw () {}

/**
 * @brief Cast to a TextureNode.
 *
 * @return a pointer to this TextureNode.
 */
const TextureNode * TextureNode::toTexture() const throw () { return this; }

/**
 * @brief Cast to a TextureNode.
 *
 * @return a pointer to this TextureNode.
 */
TextureNode * TextureNode::toTexture() throw () { return this; }

/**
 * @fn size_t TextureNode::nComponents() const throw ()
 *
 * @brief Get the number of components for the image.
 *
 * @return the number of components for the image.
 */

/**
 * @fn size_t TextureNode::width() const throw ()
 *
 * @brief Get width of the image in pixels.
 *
 * @return the width of the image in pixels.
 */

/**
 * @fn size_t TextureNode::height() const throw ()
 *
 * @brief Get height of the image in pixels.
 *
 * @return the height of the image in pixels.
 */

/**
 * @fn size_t TextureNode::nFrames() const throw ()
 *
 * @brief Get the number of frames for a time-dependent texture.
 *
 * @return the number of frames.
 *
 * @todo This needs to go away. Time-dependent textures might not be
 *      frame-based. For instance, consider an SVG animation.
 */

/**
 * @fn const unsigned char * TextureNode::pixels() const throw ()
 *
 * @brief Get the pixels.
 *
 * @return the pixels.
 */

/**
 * @fn const SFBool & TextureNode::getRepeatS() const throw ()
 *
 * @brief Get the flag indicating whether the texture should repeat in the
 *      <var>S</var> direction.
 *
 * @return @c TRUE if the image should repeat in the <var>S</var> direction,
 *      @c FALSE otherwise.
 */

/**
 * @fn const SFBool & TextureNode::getRepeatT() const throw ()
 *
 * @brief Get the flag indicating whether the texture should repeat in the
 *      <var>T</var> direction.
 *
 * @return @c TRUE if the image should repeat in the <var>T</var> direction,
 *      @c FALSE otherwise.
 */


/**
 * @class TextureCoordinateNode
 *
 * @brief Abstract base class for texture coordinate nodes.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
TextureCoordinateNode::TextureCoordinateNode(const NodeType & nodeType,
                                             const ScopePtr & scope):
        Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
TextureCoordinateNode::~TextureCoordinateNode() throw () {}

/**
 * @brief Cast to a TextureCoordinateNode.
 *
 * @return a pointer to this TextureCoordinateNode.
 */
const TextureCoordinateNode *
        TextureCoordinateNode::toTextureCoordinate() const throw () {
    return this;
}

/**
 * @brief Cast to a TextureCoordinateNode.
 *
 * @return a pointer to this TextureCoordinateNode.
 */
TextureCoordinateNode * TextureCoordinateNode::toTextureCoordinate() throw () {
    return this;
}


/**
 * @class TextureTransformNode
 *
 * @brief Abstract base class for texture transform nodes.
 */

/**
 * @brief Constructor.
 *
 * @param nodeType  the NodeType associated with the node.
 * @param scope     the Scope the node belongs to.
 */
TextureTransformNode::TextureTransformNode(const NodeType & nodeType,
                                           const ScopePtr & scope):
        Node(nodeType, scope) {}

/**
 * @brief Destructor.
 */
TextureTransformNode::~TextureTransformNode() throw () {}

/**
 * @brief Cast to a TextureTransformNode.
 *
 * @return a pointer to this TextureTransformNode.
 */
const TextureTransformNode *
        TextureTransformNode::toTextureTransform() const throw () {
    return this;
}

/**
 * @brief Cast to a TextureTransformNode.
 *
 * @return a pointer to this TextureTransformNode.
 */
TextureTransformNode * TextureTransformNode::toTextureTransform() throw () {
    return this;
}


/**
 * @class NodeVisitor
 *
 * @brief An interface to be implemented by visitors for nodes in a
 *        scene graph.
 */

/**
 * @brief Destructor.
 */
NodeVisitor::~NodeVisitor() {}

/**
 * @fn void NodeVisitor::visit(Node & node)
 *
 * @brief Visit a Node.
 *
 * @param node  a node.
 */

} // namespace OpenVRML
