//
// OpenVRML
//
// Copyright (C) 2000  Braden N. McDaniel
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

header "post_include_hpp" {
# include "VrmlField.h"
class VrmlNamespace;
class VrmlNode;
class VrmlNodeScript;
class VrmlNodeType;
class VrmlMFTime;
class VrmlMFNode;
class Doc2;
}

header "post_include_cpp" {
# include <memory>
# include <assert.h>
# include "doc2.hpp"
# include "VrmlNamespace.h"
# include "VrmlNodeType.h"
# include "VrmlNode.h"
# include "VrmlNodeScript.h"
# include "VrmlSFBool.h"
# include "VrmlSFColor.h"
# include "VrmlSFFloat.h"
# include "VrmlSFImage.h"
# include "VrmlSFInt32.h"
# include "VrmlSFNode.h"
# include "VrmlSFRotation.h"
# include "VrmlSFTime.h"
# include "VrmlSFVec2f.h"
# include "VrmlSFVec3f.h"
# include "VrmlMFColor.h"
# include "VrmlMFFloat.h"
# include "VrmlMFInt32.h"
# include "VrmlMFNode.h"
# include "VrmlMFRotation.h"
# include "VrmlMFString.h"
# include "VrmlMFTime.h"
# include "VrmlMFVec2f.h"
# include "VrmlMFVec3f.h"

namespace {

    template <typename T>
        class SimpleVector {
            public:
                static size_t const BUFFER_INCREMENT;
                
                SimpleVector();
                SimpleVector(SimpleVector<T> const &);
                ~SimpleVector();
                
                SimpleVector<T> & operator=(SimpleVector<T> const &);
                
                size_t size() const;
                void add(T const &);
                T const * data() const;
                
            private:
                size_t bufferSize_;
                size_t size_;
                T * data_;
        };
    
    template <typename T>
        size_t const SimpleVector<T>::BUFFER_INCREMENT = 64;
    
    template <typename T>
        SimpleVector<T>::SimpleVector()
          : bufferSize_(BUFFER_INCREMENT), size_(0), data_(new T[bufferSize_])
        {}
    
    template <typename T>
        SimpleVector<T>::SimpleVector(SimpleVector<T> const & c)
          : bufferSize_(c.bufferSize_), size_(c.size_), data_(new T[bufferSize_])
        {
            std::copy(c.data_, c.data_ + c.size_, this->data_);
        }
    
    template <typename T>
        SimpleVector<T>::~SimpleVector()
        {
            delete [] this->data_;
        }

    template <typename T>
        SimpleVector<T> & SimpleVector<T>::operator=(SimpleVector<T> const & c)
        {
            if (this != &c) {
                delete [] this->data_;
                bufferSize_ = c.bufferSize_;
                size_ = c.size_;
                this->data_ = new T[bufferSize_];
                std::copy(c.data_, c.data_ + c.size_, this->data_);
            }
            return *this;
        }
    
    template <typename T>
        size_t SimpleVector<T>::size() const
        {
            return size_;
        }
    
    template <typename T>
        void SimpleVector<T>::add(T const & val)
        {
            if (size_ == bufferSize_) {
                T * newData = new T[bufferSize_ += BUFFER_INCREMENT];
                std::copy(this->data_, this->data_ + size_, newData);
                delete [] this->data_;
                this->data_ = newData;
            }
            this->data_[size_++] = val;
        }
    
    template <typename T>
        T const * SimpleVector<T>::data() const
        {
            return this->data_;
        }
}
}

options {
    language="Cpp";
}

class Vrml97Parser extends Parser;
options {
    k=1;
    importVocab=Vrml97;
}
{
    public:
        enum NodeInterfaceType {
            INVALID_NODE_INTERFACE_TYPE, EVENTIN, EVENTOUT, FIELD, EXPOSEDFIELD
        };
}

vrmlScene[VrmlMFNode & mfNode, VrmlNamespace & vrmlNamespace, Doc2 const * doc]
    :   (statement[mfNode, vrmlNamespace, doc])*
    ;

statement[VrmlMFNode & mfNode, VrmlNamespace & vrmlNamespace, Doc2 const * doc]
        {
            VrmlNode * node = 0;
        }
    :   node=nodeStatement[vrmlNamespace, doc]
        {
            assert(node);
            mfNode.addNode(node);
        }
    |   protoStatement[vrmlNamespace, doc]
    |   routeStatement[vrmlNamespace]
    ;

nodeStatement[VrmlNamespace & vrmlNamespace, Doc2 const * doc] returns [VrmlNode * n = 0]
options {
    defaultErrorHandler=false;
}
    :   n=node[vrmlNamespace, doc, std::string()]
    |   KEYWORD_DEF id0:ID n=node[vrmlNamespace, doc, id0->getText()]
    |   KEYWORD_USE id1:ID
        {
            n = vrmlNamespace.findNode(id1->getText().c_str());
            if (!n) {
                throw antlr::SemanticException("Node \"" + id1->getText() + "\" has not been defined in this scope.");
            }
        }
    ;

protoStatement[VrmlNamespace & vrmlNamespace, Doc2 const * doc]
    :   externproto[vrmlNamespace, doc]
    |   proto[vrmlNamespace, doc]
    ;

proto[VrmlNamespace & vrmlNamespace, Doc2 const * doc]
    :   KEYWORD_PROTO id:ID
        {
            VrmlNodeType * nodeType = new VrmlNodeType(id->getText().c_str());
            nodeType->setScope(vrmlNamespace);
        }
        LBRACKET (
            protoInterfaceDeclaration[vrmlNamespace, doc, *nodeType]
        )* RBRACKET LBRACE protoBody[doc, *nodeType] RBRACE
        {
            vrmlNamespace.addNodeType(nodeType);
        }
    ;

protoInterfaceDeclaration[VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlNodeType & nodeType]
        {
            NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
            VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD;
        }
    :   it=eventInterfaceType ft=fieldType id0:ID
        {
            if (nodeType.hasInterface(id0->getText().c_str()) != VrmlField::NO_FIELD) {
                throw antlr::SemanticException("Interface \"" + id0->getText() + "\" already declared for " + std::string(nodeType.getName()) + " node type.");
            }
            
            switch (it) {
                case EVENTIN:
                    nodeType.addEventIn(id0->getText().c_str(), ft);
                    break;
                    
                case EVENTOUT:
                    nodeType.addEventOut(id0->getText().c_str(), ft);
                    break;
                    
                default:
                    assert(false);
            }
        }
    |   it=fieldInterfaceType ft=fieldType id1:ID
        {
            VrmlField * fv = 0;
        }
        fv=fieldValue[vrmlNamespace, doc, ft]
        {
            const std::auto_ptr<VrmlField> autofv(fv);
            
            if (nodeType.hasInterface(id1->getText().c_str()) != VrmlField::NO_FIELD) {
                throw antlr::SemanticException("Interface \"" + id1->getText() + "\" already declared for " + std::string(nodeType.getName()) + " node type.");
            }
            
            switch (it) {
                case FIELD:
                    nodeType.addField(id1->getText().c_str(), ft, autofv.get());
                    break;
                    
                case EXPOSEDFIELD:
                    nodeType.addExposedField(id1->getText().c_str(), ft, autofv.get());
                    break;
                    
                default:
                    assert(false);
            }
        }
    ;

eventInterfaceType returns [Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE]
    :   KEYWORD_EVENTIN  { it = EVENTIN; }
    |   KEYWORD_EVENTOUT { it = EVENTOUT; }
    ;

fieldInterfaceType returns [Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE]
    :   KEYWORD_FIELD        { it = FIELD; }
    |   KEYWORD_EXPOSEDFIELD { it = EXPOSEDFIELD; }
    ;

protoBody[Doc2 const * doc, VrmlNodeType & nodeType]
        {
            VrmlNode * n = 0;
        }
    :   (protoStatement[*nodeType.scope(), doc])* n=protoNodeStatement[doc, nodeType]
        {
            assert(n);
            nodeType.addNode(n);
        }
        (protoBodyStatement[doc, nodeType])*
    ;

protoBodyStatement[Doc2 const * doc, VrmlNodeType & nodeType]
        {
            VrmlNode * n = 0;
        }
    :   n=protoNodeStatement[doc, nodeType]
        {
            assert(n);
            nodeType.addNode(n);
        }
    |   protoStatement[*nodeType.scope(), doc]
    |   routeStatement[*nodeType.scope()]
    ;

protoNodeStatement[Doc2 const * doc, VrmlNodeType & protoNodeType] returns [VrmlNode * n = 0]
options {
    defaultErrorHandler=false;
}
    :   n=protoNode[doc, protoNodeType, std::string()]
    |   KEYWORD_DEF id0:ID n=protoNode[doc, protoNodeType, id0->getText()]
    |   KEYWORD_USE id1:ID
        {
            n = protoNodeType.scope()->findNode(id1->getText().c_str());
            if (!n) {
                throw antlr::SemanticException("Node \"" + id1->getText() + "\" has not been defined in this scope.");
            }
        }
    ;

externproto[VrmlNamespace & vrmlNamespace, Doc2 const * doc]
    :   KEYWORD_EXTERNPROTO id:ID
        {
            VrmlNodeType * nodeType = new VrmlNodeType(id->getText().c_str());
        }
        LBRACKET (externInterfaceDeclaration[*nodeType])* RBRACKET
        {
            VrmlMFString * urlList = 0;
        }
        urlList=externprotoUrlList
        {
            nodeType->setUrl(urlList, doc);
            
            vrmlNamespace.addNodeType(nodeType);
        }
    ;

externInterfaceDeclaration[VrmlNodeType & nodeType]
        {
            NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
            VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
        }
    :   it=interfaceType ft=fieldType id:ID
        {
            switch (it) {
                case EVENTIN:
                    nodeType.addEventIn(id->getText().c_str(), ft);
                    break;
                    
                case EVENTOUT:
                    nodeType.addEventOut(id->getText().c_str(), ft);
                    break;
                    
                case EXPOSEDFIELD:
                    nodeType.addExposedField(id->getText().c_str(), ft);
                    break;
                    
                case FIELD:
                    nodeType.addField(id->getText().c_str(), ft);
                    break;
                    
                default:
                    assert(false);
            }
        }
    ;

interfaceType returns [Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE]
    : it=eventInterfaceType
    | it=fieldInterfaceType
    ;

externprotoUrlList returns [VrmlMFString * msv = 0]
        {
            std::string s;
        }
    :   s=stringValue
        {
            const char * const sPtr = s.c_str();
            msv = new VrmlMFString(1, &sPtr);
        }
    |   LBRACKET
        {
            std::vector<std::string> stringVector;
        }
        (
            s=stringValue
            {
                stringVector.push_back(s);
            }
        )* RBRACKET
        {
            char const ** c_strs = new char const *[stringVector.size()];
            for (unsigned int i = 0; i < stringVector.size(); ++i) {
                c_strs[i] = stringVector[i].c_str();
            }
            
            msv = new VrmlMFString(stringVector.size(), c_strs);
            delete [] c_strs;
        }
    ;

//
// When adding a route, we should check whether the nodes in question actually
// have the interfaces given in the ROUTE. This code should probably reside in
// the implementation of VrmlNode::addRoute(), and there is a comment in that
// section of the code indicating that it is intended to be added.  But it's not
// there now, and adding it could be complicated. (That is, it probably means
// VrmlNode::addRoute() should throw an exception.) So for now, I'm just going
// to add that code here in the parser. It shouldn't be too difficult to move it
// when the time comes.
// -- Braden McDaniel <braden@endoframe.com>, 6 Apr, 2000
//
routeStatement[VrmlNamespace const & vrmlNamespace]
    :   KEYWORD_ROUTE fromNodeId:ID PERIOD fromInterfaceId:ID
        KEYWORD_TO toNodeId:ID PERIOD toInterfaceId:ID
        {
            VrmlNode * const fromNode = vrmlNamespace.findNode(fromNodeId->getText().c_str());
            if (!fromNode) {
                throw antlr::SemanticException("Node \"" + fromNodeId->getText() + "\" has not been defined in this scope.");
            }
            
            VrmlNodeType const & fromNodeType = fromNode->nodeType();
            VrmlField::VrmlFieldType fromInterfaceType = VrmlField::NO_FIELD;
            
            if (   ((fromInterfaceType = fromNodeType.hasEventOut(fromInterfaceId->getText().c_str())) == VrmlField::NO_FIELD)
                && ((fromInterfaceType = fromNodeType.hasExposedField(fromInterfaceId->getText().c_str())) == VrmlField::NO_FIELD)) {
                
                VrmlNodeScript * fromScriptNode = 0;
                if ((fromScriptNode = dynamic_cast<VrmlNodeScript *>(fromNode))) {
                    fromInterfaceType = fromScriptNode->hasEventOut(fromInterfaceId->getText().c_str());
                }
                
                if (fromInterfaceType == VrmlField::NO_FIELD) {
                    throw antlr::SemanticException(std::string(fromNodeType.getName()) + " has no eventOut or exposedField \"" + fromInterfaceId->getText() + "\".");
                }
            }
            
            VrmlNode * const toNode = vrmlNamespace.findNode(toNodeId->getText().c_str());
            if (!toNode) {
                throw antlr::SemanticException("Node \"" + toNodeId->getText() + "\" has not been defined in this scope.");
            }
            
            VrmlNodeType const & toNodeType = toNode->nodeType();
            VrmlField::VrmlFieldType toInterfaceType = VrmlField::NO_FIELD;
            
            if (   ((toInterfaceType = toNodeType.hasEventIn(toInterfaceId->getText().c_str())) == VrmlField::NO_FIELD)
                && ((toInterfaceType = toNodeType.hasExposedField(toInterfaceId->getText().c_str())) == VrmlField::NO_FIELD)) {
                
                VrmlNodeScript * toScriptNode = 0;
                if ((toScriptNode = dynamic_cast<VrmlNodeScript *>(toNode))) {
                    toInterfaceType = toScriptNode->hasEventIn(toInterfaceId->getText().c_str());
                }
                
                if (toInterfaceType == VrmlField::NO_FIELD) {
                    throw antlr::SemanticException(std::string(toNodeType.getName()) + " has no eventIn or exposedField \"" + toInterfaceId->getText() + "\".");
                }
            }
            
            if (fromInterfaceType != toInterfaceType) {
                throw antlr::SemanticException("Routed interface types must match.");
            }
            
            fromNode->addRoute(fromInterfaceId->getText().c_str(), toNode, toInterfaceId->getText().c_str());
        }
    ;

node[VrmlNamespace & vrmlNamespace, Doc2 const * doc, std::string const & nodeId] returns [VrmlNode * n = 0]
options {
    defaultErrorHandler = false;
}
        {
            VrmlNodeType const * nodeType = 0;
        }
    :
	{ !LT(1)->getText().compare("Script") }?
	scriptId:ID 
        {
            nodeType = vrmlNamespace.findType("Script");
            assert(nodeType);
            
            n = nodeType->newNode();
            
            if (nodeId.size() > 0) {
                n->setName(nodeId.c_str(), &vrmlNamespace);
            }
            
            VrmlNodeScript * const scriptNode = dynamic_cast<VrmlNodeScript *>(n);
            assert(scriptNode);
        }
        LBRACE (
            nodeBodyElement[vrmlNamespace, doc, *n]
            | scriptInterfaceDeclaration[vrmlNamespace, doc, *scriptNode]
        )* RBRACE
        
    |  	nodeTypeId:ID 
        {
            nodeType = vrmlNamespace.findType(nodeTypeId->getText().c_str());
            if (!nodeType) {
                throw antlr::SemanticException("Unknown node type \"" + nodeTypeId->getText() + "\".");
            }
            
            n = nodeType->newNode();
            
            if (nodeId.size() > 0) {
                n->setName(nodeId.c_str(), &vrmlNamespace);
            }
        }
        LBRACE (nodeBodyElement[vrmlNamespace, doc, *n])* RBRACE
    ;

nodeBodyElement[VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlNode & node]
        {
            VrmlNodeType const & nodeType = node.nodeType();
            VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
        }
    :   id:ID
        {
            if (   ((ft = nodeType.hasField(id->getText().c_str())) == VrmlField::NO_FIELD)
                && ((ft = nodeType.hasExposedField(id->getText().c_str())) == VrmlField::NO_FIELD)) {
                
                throw antlr::SemanticException(std::string(nodeType.getName()) + " node has no field or exposedField \"" + id->getText() + "\" (nodeBodyEl).");
            }
            
            VrmlField * fv = 0;
        }
        fv=fieldValue[vrmlNamespace, doc, ft]
        {
            assert(fv);
            node.setField(id->getText().c_str(), *fv);
            delete fv;
        }
    |   routeStatement[vrmlNamespace]
    |   protoStatement[vrmlNamespace, doc]
    ;

scriptInterfaceDeclaration[VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlNodeScript & node]
        {
            NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
            VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
        }
    :   it=eventInterfaceType ft=fieldType id:ID
        {
            if (   (node.hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)
                || (node.nodeType().hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)) {
                
                throw antlr::SemanticException("Interface \"" + id->getText() + "\" already declared for Script node.");
            }
            
            switch (it) {
                case EVENTIN:
                    node.addEventIn(id->getText().c_str(), ft);
                    break;
                case EVENTOUT:
                    node.addEventOut(id->getText().c_str(), ft);
                    break;
                default:
                    assert(false);
            }
        }
    |   scriptFieldInterfaceDeclaration[vrmlNamespace, doc, node]
    ;

scriptFieldInterfaceDeclaration[VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlNodeScript & node]
        {
            VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD;
            VrmlField * fv = 0;
        }
    :   KEYWORD_FIELD ft=fieldType id:ID fv=fieldValue[vrmlNamespace, doc, ft]
        {
            assert(fv);
            
            //
            // Show this code some auto_ptr lovin' to make it exception-safe.
            //
            const std::auto_ptr<VrmlField> autofv(fv);
            
            if (   (node.hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)
                || (node.nodeType().hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)) {
                
                throw antlr::SemanticException("Interface \"" + id->getText() + "\" already declared for Script node.");
            }
            
            node.addField(id->getText().c_str(), ft, autofv.get());
        }
    ;

protoNode[Doc2 const * doc, VrmlNodeType & protoNodeType, std::string const & nodeId] returns [VrmlNode * n = 0]
options {
    defaultErrorHandler=false;
}
        {
            VrmlNodeType const * nodeType = 0;
        }
    : 
	{ !LT(1)->getText().compare("Script") }?
	  scriptId:ID
        {
            nodeType = protoNodeType.scope()->findType("Script");
            assert(nodeType);
            
            n = nodeType->newNode();
            
            if (nodeId.size() > 0) {
                n->setName(nodeId.c_str(), protoNodeType.scope());
            }
            
            VrmlNodeScript * const scriptNode = dynamic_cast<VrmlNodeScript *>(n);
            assert(scriptNode);
        }
        LBRACE (
            protoNodeBodyElement[doc, protoNodeType, *n]
            | protoScriptInterfaceDeclaration[doc, protoNodeType, *scriptNode]
        )* RBRACE

    | 	  nodeTypeId:ID
        {
            nodeType = protoNodeType.scope()->findType(nodeTypeId->getText().c_str());
            if (!nodeType) {
                throw antlr::SemanticException("Unknown node type \"" + nodeTypeId->getText() + "\".");
            }
            
            n = nodeType->newNode();
            
            if (nodeId.size() > 0) {
                n->setName(nodeId.c_str(), protoNodeType.scope());
            }
        }
        LBRACE (protoNodeBodyElement[doc, protoNodeType, *n])* RBRACE
    ;

protoNodeBodyElement[Doc2 const * doc, VrmlNodeType & protoNodeType, VrmlNode & node]
        {
            VrmlNodeType const & nodeType = node.nodeType();
            VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
        }
    :   { nodeType.hasEventIn(LT(1)->getText().c_str()) != VrmlField::NO_FIELD ||
	  nodeType.hasEventOut(LT(1)->getText().c_str()) != VrmlField::NO_FIELD }?
      eventId:ID isStatement[protoNodeType, node, eventId->getText()]

    | id:ID 
        {
            if (   ((ft = nodeType.hasField(id->getText().c_str())) == VrmlField::NO_FIELD)
                && ((ft = nodeType.hasExposedField(id->getText().c_str())) == VrmlField::NO_FIELD)) {
                
                throw antlr::SemanticException(std::string(nodeType.getName()) + " node has no field or exposedField \"" + id->getText() + "\" (protoNodeBodyEl).");
            }
            
            VrmlField * fv = 0;
        }
        (
            (
                fv=protoFieldValue[doc, protoNodeType, ft]
                {
                    assert(fv);
                    node.setField(id->getText().c_str(), *fv);
                    delete fv;
                }
            )
            | isStatement[protoNodeType, node, id->getText()]
        )
    |   routeStatement[*protoNodeType.scope()]
    |   protoStatement[*protoNodeType.scope(), doc]
    ;

//
// As with the situation with ROUTEs, we probably want to move the code that 
// checks to see if the interface is valid and the types match into the
// implementation of VrmlNodeType.
// -- Braden McDaniel <braden@endoframe.com>, 8 Apr, 2000
//
isStatement[VrmlNodeType & protoNodeType, VrmlNode & node, std::string const & nodeInterfaceId]
    :   KEYWORD_IS id:ID
        {
            //
            // Okay, I was going to write some code to make sure the IS
            // statement is valid before trying to add it, but VrmlNodeType has
            // poor facilities for determining the interface type from a given
            // interface id. Also, it does not appear that VrmlNodeType
            // differentiates between an exposedField zzz and field/eventIn/
            // eventOut triplet zzz/set_zzz/zzz_changed. This differentiation
            // probably needs to happen before IS statements can be checked
            // appropriately.
            // -- Braden McDaniel <braden@endoframe.com>, 8 Apr, 2000
            //
            
            protoNodeType.addIS(id->getText().c_str(), &node, nodeInterfaceId.c_str());
        }
    ;

protoScriptInterfaceDeclaration[Doc2 const * doc, VrmlNodeType & protoNodeType, VrmlNodeScript & node]
        {
            NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
            VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
        }
    :   it=eventInterfaceType ft=fieldType id:ID
        {
            if (   (node.hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)
                || (node.nodeType().hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)) {
                
                throw antlr::SemanticException("Interface \"" + id->getText() + "\" already declared for Script node.");
            }
            
            switch (it) {
                case EVENTIN:
                    node.addEventIn(id->getText().c_str(), ft);
                    break;
                case EVENTOUT:
                    node.addEventOut(id->getText().c_str(), ft);
                    break;
                default:
                    assert(false);
            }
        }
        (isStatement[protoNodeType, node, id->getText()])?
    |   protoScriptFieldInterfaceDeclaration[doc, protoNodeType, node]
    ;

protoScriptFieldInterfaceDeclaration[Doc2 const * doc, VrmlNodeType & protoNodeType, VrmlNodeScript & node]
        {
            VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
            VrmlField * fv = 0;
        }
    :   KEYWORD_FIELD ft=fieldType id:ID
        {
            if (   (node.hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)
                || (node.nodeType().hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)) {
                
                throw antlr::SemanticException("Interface \"" + id->getText() + "\" already declared for Script node.");
            }
            
        }
        (
            (
                fv=protoFieldValue[doc, protoNodeType, ft]
                {
                    assert(fv);
                    node.addField(id->getText().c_str(), ft, fv);
                    delete fv;
                }
            )
            |
		{
                    node.addField(id->getText().c_str(), ft);
		}
		isStatement[protoNodeType, node, id->getText()]
        )
    ;

fieldType returns [VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD]
    :   FIELDTYPE_MFCOLOR { ft = VrmlField::MFCOLOR; }
    |   FIELDTYPE_MFFLOAT { ft = VrmlField::MFFLOAT; }
    |   FIELDTYPE_MFINT32 { ft = VrmlField::MFINT32; }
    |   FIELDTYPE_MFNODE { ft = VrmlField::MFNODE; }
    |   FIELDTYPE_MFROTATION { ft = VrmlField::MFROTATION; }
    |   FIELDTYPE_MFSTRING { ft = VrmlField::MFSTRING; }
    |   FIELDTYPE_MFTIME { ft = VrmlField::MFTIME; }
    |   FIELDTYPE_MFVEC2F { ft = VrmlField::MFVEC2F; }
    |   FIELDTYPE_MFVEC3F { ft = VrmlField::MFVEC3F; }
    |   FIELDTYPE_SFBOOL { ft = VrmlField::SFBOOL; }
    |   FIELDTYPE_SFCOLOR { ft = VrmlField::SFCOLOR; }
    |   FIELDTYPE_SFFLOAT { ft = VrmlField::SFFLOAT; }
    |   FIELDTYPE_SFIMAGE { ft = VrmlField::SFIMAGE; }
    |   FIELDTYPE_SFINT32 { ft = VrmlField::SFINT32; }
    |   FIELDTYPE_SFNODE { ft = VrmlField::SFNODE; }
    |   FIELDTYPE_SFROTATION { ft = VrmlField::SFROTATION; }
    |   FIELDTYPE_SFSTRING { ft = VrmlField::SFSTRING; }
    |   FIELDTYPE_SFTIME { ft = VrmlField::SFTIME; }
    |   FIELDTYPE_SFVEC2F { ft = VrmlField::SFVEC2F; }
    |   FIELDTYPE_SFVEC3F { ft = VrmlField::SFVEC3F; }
    ;

fieldValue[VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlField::VrmlFieldType ft] returns [VrmlField * fv = 0]
    :   {    (ft == VrmlField::SFNODE)
          || (ft == VrmlField::MFNODE) }? fv=nodeFieldValue[vrmlNamespace, doc, ft]
    |   fv=nonNodeFieldValue[ft]
    ;

protoFieldValue[Doc2 const * doc, VrmlNodeType & protoNodeType, VrmlField::VrmlFieldType ft] returns [VrmlField * fv = 0]
    :   {    (ft == VrmlField::SFNODE)
          || (ft == VrmlField::MFNODE) }? fv=protoNodeFieldValue[doc, protoNodeType, ft]
        {
            assert(fv);
        }
    |   fv=nonNodeFieldValue[ft]
        {
            assert(fv);
        }
    ;

nonNodeFieldValue[VrmlField::VrmlFieldType ft] returns [VrmlField * fv = 0]
    :   { ft == VrmlField::SFBOOL }? fv=sfBoolValue
    |   { ft == VrmlField::SFCOLOR }? fv=sfColorValue
    |   { ft == VrmlField::SFFLOAT }? fv=sfFloatValue
    |   { ft == VrmlField::SFIMAGE }? fv=sfImageValue
    |   { ft == VrmlField::SFINT32 }? fv=sfInt32Value
    |   { ft == VrmlField::SFROTATION }? fv=sfRotationValue
    |   { ft == VrmlField::SFSTRING }? fv=sfStringValue
    |   { ft == VrmlField::SFTIME }? fv=sfTimeValue
    |   { ft == VrmlField::SFVEC2F }? fv=sfVec2fValue
    |   { ft == VrmlField::SFVEC3F }? fv=sfVec3fValue
    |   { ft == VrmlField::MFCOLOR }? fv=mfColorValue
    |   { ft == VrmlField::MFFLOAT }? fv=mfFloatValue
    |   { ft == VrmlField::MFINT32 }? fv=mfInt32Value
    |   { ft == VrmlField::MFROTATION }? fv=mfRotationValue
    |   { ft == VrmlField::MFSTRING }? fv=mfStringValue
    |   { ft == VrmlField::MFTIME }? fv=mfTimeValue
    |   { ft == VrmlField::MFVEC2F }? fv=mfVec2fValue
    |   fv=mfVec3fValue
    ;

nodeFieldValue[VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlField::VrmlFieldType ft] returns [VrmlField * fv = 0]
options {
    defaultErrorHandler=false;
}
    :   { ft == VrmlField::SFNODE }? fv=sfNodeValue[vrmlNamespace, doc]
    |   fv=mfNodeValue[vrmlNamespace, doc]
    ;

protoNodeFieldValue[Doc2 const * doc, VrmlNodeType & protoNodeType, VrmlField::VrmlFieldType ft] returns [VrmlField * fv = 0]
options {
    defaultErrorHandler=false;
}
    :   { ft == VrmlField::SFNODE }? fv=protoSfNodeValue[doc, protoNodeType]
        {
            assert(fv);
        }
    |   fv=protoMfNodeValue[doc, protoNodeType]
        {
            assert(fv);
        }
    ;

sfBoolValue returns [VrmlSFBool * sbv = new VrmlSFBool()]
        {
            bool val(false);
        }
    :   val=boolValue { *sbv = VrmlSFBool(val); }
    ;

boolValue returns [bool val]
        {
            val = false;
        }
    :   KEYWORD_TRUE { val = true; }
    |   KEYWORD_FALSE { val = false; }
    ;

sfColorValue returns [VrmlSFColor * scv = new VrmlSFColor()]
        {
            float c[3];
        }
    :   colorValue[c]
        {
            *scv = VrmlSFColor(c[0], c[1], c[2]);
        }
    ;

mfColorValue returns [VrmlMFColor * mcv = new VrmlMFColor()]
        {
            float c[3];
        }
    :   colorValue[c]
        {
            *mcv = VrmlMFColor(1, c);
        }
    |   LBRACKET
        {
            SimpleVector<float> colorVector;
        }
        (
            colorValue[c]
            {
                colorVector.add(c[0]);
                colorVector.add(c[1]);
                colorVector.add(c[2]);
            }
        )* RBRACKET
        {
            *mcv = VrmlMFColor(colorVector.size() / 3L, colorVector.data());
        }
    ;

colorValue[float c[3]]
        {
            float r(0.0f), g(0.0f), b(0.0f);
        }
    :   r=colorComponent g=colorComponent b=colorComponent
        {
            c[0] = r;
            c[1] = g;
            c[2] = b;
        }
    ;

//
// colorComponent is defined separately from floatValue since, at some point, it
// might be useful to issue a warning if a color component is not in [0, 1].
//
colorComponent returns [float val = 0.0f]
    :   val=floatValue
    ;

sfFloatValue returns [VrmlSFFloat * sfv = new VrmlSFFloat(0.0f)]
        {
            float f(0.0f);
        }
    :   f=floatValue
        {
            *sfv = VrmlSFFloat(f);
        }
    ;

mfFloatValue returns [VrmlMFFloat * mfv = new VrmlMFFloat()]
        {
            float f(0.0f);
        }
    :   f=floatValue
        {
            *mfv = VrmlMFFloat(1, &f);
        }
    |   LBRACKET
        {
            SimpleVector<float> floatVector;
        }
        (
            f=floatValue
            {
                floatVector.add(f);
            }
        )* RBRACKET
        {
            *mfv = VrmlMFFloat(floatVector.size(), floatVector.data());
        }
    ;

floatValue returns [float val = 0.0f]
    :   f0:REAL     { val = atof(f0->getText().c_str()); }
    |   f1:INTEGER  { val = atof(f1->getText().c_str()); }
    ;

sfImageValue returns [VrmlSFImage * siv = new VrmlSFImage()]
        {
            unsigned long w(0L), h(0L), com(0L), pixel(0L);
        }
    :   w=intValue h=intValue com=intValue
        {
            SimpleVector<unsigned char> pixelVector;
        }
        (
            pixel=intValue
            {
                // need to confirm the cross-platform-ness of this, it
                // looks kind of ugly but might in fact be ok. basically,
                // we read the value as an integer, then strip off the
                // bytes one by one.
		for (int i = com - 1; i >= 0; i--) {
                    pixelVector.add(static_cast<unsigned char>(pixel >> (8 * i) & 0xff));
                }
            }
        )*
        {
            // if somebody gives us a really, really, really big
            // pixeltexture, then we will crash. in the age of dos
            // attacks, we have to assume that someone will feed us a
	    // too-big texture to see if we barf. good behavior
	    // would be to detect outsized w/h and bail. casting away
	    // the compiler warning is not helpful. there are other 
            // bigger bugs to fry, so I guess it's ok for now.
            //
            if (pixelVector.size() != (w * h * com)) {
                throw antlr::SemanticException("Wrong number of pixel values for SFImage.");
            }
            *siv = VrmlSFImage(w, h, com, pixelVector.data()); // hmmmm...
        }
    ;

sfInt32Value returns [VrmlSFInt32 * siv = new VrmlSFInt32(0L)]
        {
            long i(0L);
        }
    :   i=intValue
        {
            *siv = VrmlSFInt32(i);
        }
    ;

mfInt32Value returns [VrmlMFInt32 * miv = new VrmlMFInt32()]
        {
            long i(0L);
        }
    :   i=intValue
        {
            *miv = VrmlMFInt32(1, &i);
        }
    |   LBRACKET
        {
            SimpleVector<long> longVector;
        }
        (
            i=intValue
            {
                longVector.add(i);
            }
        )* RBRACKET
        {
            *miv = VrmlMFInt32(longVector.size(), longVector.data());
        }
    ;

intValue returns [long val = 0]
    :   i0:INTEGER { val = atol(i0->getText().c_str()); }
    |  	i1:HEX_INTEGER { val = strtol(i1->getText().c_str(), 0, 16); }
    ;

sfNodeValue[VrmlNamespace & vrmlNamespace, Doc2 const * doc] returns [VrmlSFNode * snv = new VrmlSFNode(0)]
        {
            VrmlNode * n = 0;
        }
    :   n=nodeStatement[vrmlNamespace, doc]
        {
            *snv = VrmlSFNode(n);
        }
    |   KEYWORD_NULL
    ;

protoSfNodeValue[Doc2 const * doc, VrmlNodeType & protoNodeType] returns [VrmlSFNode * snv = new VrmlSFNode(0)]
        {
            VrmlNode * n = 0;
        }
    :   n=protoNodeStatement[doc, protoNodeType]
        {
            *snv = VrmlSFNode(n);
        }
    |   KEYWORD_NULL
    ;

mfNodeValue[VrmlNamespace & vrmlNamespace, Doc2 const * doc] returns [VrmlMFNode * mnv = new VrmlMFNode()]
        {
            VrmlNode * n = 0;
        }
    :   n=nodeStatement[vrmlNamespace, doc]
        {
            *mnv = VrmlMFNode(1, &n);
        }
        
    |   LBRACKET
        (
            n=nodeStatement[vrmlNamespace, doc]
            {
                mnv->addNode(n);
            }
        )* RBRACKET
    ;

protoMfNodeValue[Doc2 const * doc, VrmlNodeType & protoNodeType] returns [VrmlMFNode * mnv = new VrmlMFNode()]
        {
            VrmlNode * n = 0;
        }
    :   n=protoNodeStatement[doc, protoNodeType]
        {
            *mnv = VrmlMFNode(1, &n);
        }
        
    |   LBRACKET
        (
            n=protoNodeStatement[doc, protoNodeType]
            {
                mnv->addNode(n);
            }
        )* RBRACKET
    ;

sfRotationValue returns [VrmlSFRotation * srv = new VrmlSFRotation()]
        {
            float r[4];
        }
    :   rotationValue[r]
        {
            *srv = VrmlSFRotation(r[0], r[1], r[2], r[3]);
        }
    ;

mfRotationValue returns [VrmlMFRotation * mrv = new VrmlMFRotation()]
        {
            float r[4];
        }
    :   rotationValue[r]
        {
            *mrv = VrmlMFRotation(1, r);
        }
    |   LBRACKET
        {
            SimpleVector<float> floatVector;
        }
        (
            rotationValue[r]
            {
                floatVector.add(r[0]);
                floatVector.add(r[1]);
                floatVector.add(r[2]);
                floatVector.add(r[3]);
            }
        )* RBRACKET
        {
            *mrv = VrmlMFRotation(floatVector.size() / 4L, floatVector.data());
        }
    ;

//
// Potentially issue a warning here if the vector isn't normalized.
//
rotationValue[float r[4]]
        {
            float x(0.0f), y(0.0f), z(0.0f), rot(0.0f);
        }
    :   x=floatValue y=floatValue z=floatValue rot=floatValue
        {
            r[0] = x;
            r[1] = y;
            r[2] = z;
            r[3] = rot;
        }
    ;

sfStringValue returns [VrmlSFString * ssv = new VrmlSFString()]
        {
            std::string s;
        }
    :   s=stringValue
        {
            *ssv = VrmlSFString(s.c_str());
        }
    ;

mfStringValue returns [VrmlMFString * msv = new VrmlMFString()]
        {
            std::string s;
        }
    :   s=stringValue
        {
            const char * const sPtr = s.c_str();
            *msv = VrmlMFString(1, &sPtr);
        }
    |   LBRACKET
        {
            std::vector<std::string> stringVector;
        }
        (
            s=stringValue
            {
                stringVector.push_back(s);
            }
        )* RBRACKET
        {
            char const ** c_strs = new char const *[stringVector.size()];
            for (unsigned int i = 0; i < stringVector.size(); ++i) {
                c_strs[i] = stringVector[i].c_str();
            }
            
            *msv = VrmlMFString(stringVector.size(), c_strs);
            delete [] c_strs;
        }
    ;

stringValue returns [std::string str]
    :   s:STRING
        {
            //
            // Why doesn't this work?
            //
            // str = std::string(s->getText().begin() + 1, s->getText().end() - 1);
            
            std::string temp(s->getText());
            str = std::string(temp.begin() + 1, temp.end() - 1);
        }
    ;

sfTimeValue returns [VrmlSFTime * stv = new VrmlSFTime(0.0)]
        {
            double t(0.0);
        }
    :   t=doubleValue
        {
            *stv = VrmlSFTime(t);
        }
    ;

mfTimeValue returns [VrmlMFTime * mtv = new VrmlMFTime()]
        {
            double t(0.0);
        }
    :   t=doubleValue
        {
            *mtv = VrmlMFTime(1, &t);
        }
    |   LBRACKET
        {
            SimpleVector<double> doubleVector;
        }
        (
            t=doubleValue
            {
                doubleVector.add(t);
            }
        )* RBRACKET
        {
            *mtv = VrmlMFTime(doubleVector.size(), doubleVector.data());
        }
    ;

doubleValue returns [double val = 0.0]
    :   d0:REAL     { val = atof(d0->getText().c_str()); }
    |   d1:INTEGER  { val = atof(d1->getText().c_str()); }
    ;

sfVec2fValue returns [VrmlSFVec2f * svv = new VrmlSFVec2f()]
        {
            float v[2];
        }
    :   vec2fValue[v]
        {
            *svv = VrmlSFVec2f(v[0], v[1]);
        }
    ;

mfVec2fValue returns [VrmlMFVec2f * mvv = new VrmlMFVec2f()]
        {
            float v[2];
        }
    :   vec2fValue[v]
        {
            *mvv = VrmlMFVec2f(1, v);
        }
    |   LBRACKET
        {
            SimpleVector<float> floatVector;
        }
        (
            vec2fValue[v]
            {
                floatVector.add(v[0]);
                floatVector.add(v[1]);
            }
        )* RBRACKET
        {
            *mvv = VrmlMFVec2f(floatVector.size() / 2L, floatVector.data());
        }
    ;

vec2fValue[float v[2]]
        {
            float x(0.0f), y(0.0f);
        }
    :   x=floatValue y=floatValue
        {
            v[0] = x;
            v[1] = y;
        }
    ;

sfVec3fValue returns [VrmlSFVec3f * svv = new VrmlSFVec3f()]
        {
            float v[3];
        }
    :   vec3fValue[v]
        {
            *svv = VrmlSFVec3f(v[0], v[1], v[2]);
        }
    ;

mfVec3fValue returns [VrmlMFVec3f * mvv = new VrmlMFVec3f()]
        {
            float v[3];
        }
    :   vec3fValue[v]
        {
            *mvv = VrmlMFVec3f(1, v);
        }
    |   LBRACKET
        {
            SimpleVector<float> floatVector;
        }
        (
            vec3fValue[v]
            {
                floatVector.add(v[0]);
                floatVector.add(v[1]);
                floatVector.add(v[2]);
            }
        )* RBRACKET
        {
            *mvv = VrmlMFVec3f(floatVector.size() / 3L, floatVector.data());
        }
    ;

vec3fValue[float v[3]]
        {
            float x(0.0f), y(0.0f), z(0.0f);
        }
    :   x=floatValue y=floatValue z=floatValue
        {
            v[0] = x;
            v[1] = y;
            v[2] = z;
        }
    ;
