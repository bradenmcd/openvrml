/* $ANTLR 2.7.1: "Vrml97Parser.g" -> "Vrml97Parser.cpp"$ */
#include "Vrml97Parser.hpp"
#include "antlr/NoViableAltException.hpp"
#include "antlr/SemanticException.hpp"
#line 33 "Vrml97Parser.g"

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

#line 117 "Vrml97Parser.cpp"
#line 1 "Vrml97Parser.g"

#line 120 "Vrml97Parser.cpp"
Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
	setTokenNames(_tokenNames);
}

Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,1)
{
	setTokenNames(_tokenNames);
}

Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
	setTokenNames(_tokenNames);
}

Vrml97Parser::Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,1)
{
	setTokenNames(_tokenNames);
}

Vrml97Parser::Vrml97Parser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,1)
{
	setTokenNames(_tokenNames);
}

void Vrml97Parser::vrmlScene(
	VrmlMFNode & mfNode, VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
	
	try {      // for error handling
		{
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				statement(mfNode, vrmlNamespace, doc);
			}
			else {
				goto _loop3;
			}
			
		}
		_loop3:;
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
}

void Vrml97Parser::statement(
	VrmlMFNode & mfNode, VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
#line 164 "Vrml97Parser.g"
	
	VrmlNodePtr node;
	
#line 183 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			node=nodeStatement(vrmlNamespace, doc);
#line 169 "Vrml97Parser.g"
			
			assert(node);
			mfNode.addNode(*node);
			
#line 197 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(vrmlNamespace, doc);
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(vrmlNamespace);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_2);
	}
}

VrmlNodePtr  Vrml97Parser::nodeStatement(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
#line 177 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 229 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case ID:
	{
		n=node(vrmlNamespace, doc, std::string());
		break;
	}
	case KEYWORD_DEF:
	{
		match(KEYWORD_DEF);
		id0 = LT(1);
		match(ID);
		n=node(vrmlNamespace, doc, id0->getText());
		break;
	}
	case KEYWORD_USE:
	{
		match(KEYWORD_USE);
		id1 = LT(1);
		match(ID);
#line 185 "Vrml97Parser.g"
		
		n = vrmlNamespace.findNode(id1->getText().c_str());
		if (!n) {
		throw antlr::SemanticException("Node \"" + id1->getText() + "\" has not been defined in this scope.");
		}
		
#line 259 "Vrml97Parser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return n;
}

void Vrml97Parser::protoStatement(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EXTERNPROTO:
		{
			externproto(vrmlNamespace, doc);
			break;
		}
		case KEYWORD_PROTO:
		{
			proto(vrmlNamespace, doc);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::routeStatement(
	VrmlNamespace const & vrmlNamespace
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  fromNodeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  fromInterfaceId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  toNodeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  toInterfaceId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(KEYWORD_ROUTE);
		fromNodeId = LT(1);
		match(ID);
		match(PERIOD);
		fromInterfaceId = LT(1);
		match(ID);
		match(KEYWORD_TO);
		toNodeId = LT(1);
		match(ID);
		match(PERIOD);
		toInterfaceId = LT(1);
		match(ID);
#line 410 "Vrml97Parser.g"
		
		const VrmlNodePtr fromNode = vrmlNamespace.findNode(fromNodeId->getText().c_str());
		if (!fromNode) {
		throw antlr::SemanticException("Node \"" + fromNodeId->getText() + "\" has not been defined in this scope.");
		}
		
		VrmlNodeType const & fromNodeType = fromNode->nodeType();
		VrmlField::VrmlFieldType fromInterfaceType = VrmlField::NO_FIELD;
		
		if (   ((fromInterfaceType = fromNodeType.hasEventOut(fromInterfaceId->getText().c_str())) == VrmlField::NO_FIELD)
		&& ((fromInterfaceType = fromNodeType.hasExposedField(fromInterfaceId->getText().c_str())) == VrmlField::NO_FIELD)) {
		
		VrmlNodeScript * fromScriptNode = 0;
		if ((fromScriptNode = fromNode->toScript())) {
		fromInterfaceType = fromScriptNode->hasEventOut(fromInterfaceId->getText().c_str());
		}
		
		if (fromInterfaceType == VrmlField::NO_FIELD) {
		throw antlr::SemanticException(std::string(fromNodeType.getName()) + " has no eventOut or exposedField \"" + fromInterfaceId->getText() + "\".");
		}
		}
		
		const VrmlNodePtr toNode = vrmlNamespace.findNode(toNodeId->getText().c_str());
		if (!toNode) {
		throw antlr::SemanticException("Node \"" + toNodeId->getText() + "\" has not been defined in this scope.");
		}
		
		VrmlNodeType const & toNodeType = toNode->nodeType();
		VrmlField::VrmlFieldType toInterfaceType = VrmlField::NO_FIELD;
		
		if (   ((toInterfaceType = toNodeType.hasEventIn(toInterfaceId->getText().c_str())) == VrmlField::NO_FIELD)
		&& ((toInterfaceType = toNodeType.hasExposedField(toInterfaceId->getText().c_str())) == VrmlField::NO_FIELD)) {
		
		VrmlNodeScript * toScriptNode = 0;
		if ((toScriptNode = toNode->toScript())) {
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
		
#line 370 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

VrmlNodePtr  Vrml97Parser::node(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc, std::string const & nodeId
) {
#line 461 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 384 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 461 "Vrml97Parser.g"
	
	VrmlNodeType const * nodeType = 0;
	
#line 391 "Vrml97Parser.cpp"
	
	if (((LA(1)==ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 472 "Vrml97Parser.g"
		
		nodeType = vrmlNamespace.findType("Script");
		assert(nodeType);
		
		n = VrmlNodePtr(nodeType->newNode());
		
		if (nodeId.size() > 0) {
		n->setName(nodeId.c_str(), &vrmlNamespace);
		}
		
		VrmlNodeScript * const scriptNode = n->toScript();
		assert(scriptNode);
		
#line 410 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			switch ( LA(1)) {
			case ID:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				nodeBodyElement(vrmlNamespace, doc, *n);
				break;
			}
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_FIELD:
			{
				scriptInterfaceDeclaration(vrmlNamespace, doc, *scriptNode);
				break;
			}
			default:
			{
				goto _loop31;
			}
			}
		}
		_loop31:;
		}
		match(RBRACE);
	}
	else if ((LA(1)==ID)) {
		nodeTypeId = LT(1);
		match(ID);
#line 491 "Vrml97Parser.g"
		
		nodeType = vrmlNamespace.findType(nodeTypeId->getText().c_str());
		if (!nodeType) {
		throw antlr::SemanticException("Unknown node type \"" + nodeTypeId->getText() + "\".");
		}
		
		n = VrmlNodePtr(nodeType->newNode());
		
		if (nodeId.size() > 0) {
		n->setName(nodeId.c_str(), &vrmlNamespace);
		}
		
#line 456 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				nodeBodyElement(vrmlNamespace, doc, *n);
			}
			else {
				goto _loop33;
			}
			
		}
		_loop33:;
		}
		match(RBRACE);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return n;
}

void Vrml97Parser::externproto(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(KEYWORD_EXTERNPROTO);
		id = LT(1);
		match(ID);
#line 316 "Vrml97Parser.g"
		
		VrmlNodeType * nodeType = new VrmlNodeType(id->getText().c_str());
		
#line 492 "Vrml97Parser.cpp"
		match(LBRACKET);
		{
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
				externInterfaceDeclaration(*nodeType);
			}
			else {
				goto _loop22;
			}
			
		}
		_loop22:;
		}
		match(RBRACKET);
#line 320 "Vrml97Parser.g"
		
		VrmlMFString * urlList = 0;
		
#line 511 "Vrml97Parser.cpp"
		urlList=externprotoUrlList();
#line 324 "Vrml97Parser.g"
		
		nodeType->setUrl(urlList, doc);
		
		vrmlNamespace.addNodeType(nodeType);
		
#line 519 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::proto(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(KEYWORD_PROTO);
		id = LT(1);
		match(ID);
#line 200 "Vrml97Parser.g"
		
		VrmlNodeType * nodeType = new VrmlNodeType(id->getText().c_str());
		nodeType->setScope(vrmlNamespace);
		
#line 542 "Vrml97Parser.cpp"
		match(LBRACKET);
		{
		for (;;) {
			if ((_tokenSet_5.member(LA(1)))) {
				protoInterfaceDeclaration(vrmlNamespace, doc, *nodeType);
			}
			else {
				goto _loop9;
			}
			
		}
		_loop9:;
		}
		match(RBRACKET);
		match(LBRACE);
		protoBody(doc, *nodeType);
		match(RBRACE);
#line 207 "Vrml97Parser.g"
		
		vrmlNamespace.addNodeType(nodeType);
		
#line 564 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
}

void Vrml97Parser::protoInterfaceDeclaration(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlNodeType & nodeType
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 212 "Vrml97Parser.g"
	
	NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
	VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD;
	
#line 583 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id0 = LT(1);
			match(ID);
#line 218 "Vrml97Parser.g"
			
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
			
#line 613 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		case KEYWORD_FIELD:
		{
			it=fieldInterfaceType();
			ft=fieldType();
			id1 = LT(1);
			match(ID);
#line 237 "Vrml97Parser.g"
			
			VrmlField * fv = 0;
			
#line 627 "Vrml97Parser.cpp"
			fv=fieldValue(vrmlNamespace, doc, ft);
#line 241 "Vrml97Parser.g"
			
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
			
#line 650 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

void Vrml97Parser::protoBody(
	Doc2 const * doc, VrmlNodeType & nodeType
) {
#line 273 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 673 "Vrml97Parser.cpp"
	
	try {      // for error handling
		{
		for (;;) {
			if ((LA(1)==KEYWORD_EXTERNPROTO||LA(1)==KEYWORD_PROTO)) {
				protoStatement(*nodeType.scope(), doc);
			}
			else {
				goto _loop15;
			}
			
		}
		_loop15:;
		}
		n=protoNodeStatement(doc, nodeType);
#line 278 "Vrml97Parser.g"
		
		assert(n);
		nodeType.addNode(*n);
		
#line 694 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((_tokenSet_0.member(LA(1)))) {
				protoBodyStatement(doc, nodeType);
			}
			else {
				goto _loop17;
			}
			
		}
		_loop17:;
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_7);
	}
}

Vrml97Parser::NodeInterfaceType  Vrml97Parser::eventInterfaceType() {
#line 263 "Vrml97Parser.g"
	Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
#line 718 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		{
			match(KEYWORD_EVENTIN);
#line 264 "Vrml97Parser.g"
			it = EVENTIN;
#line 727 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EVENTOUT:
		{
			match(KEYWORD_EVENTOUT);
#line 265 "Vrml97Parser.g"
			it = EVENTOUT;
#line 735 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_8);
	}
	return it;
}

VrmlField::VrmlFieldType  Vrml97Parser::fieldType() {
#line 757 "Vrml97Parser.g"
	VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD;
#line 755 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case FIELDTYPE_MFCOLOR:
		{
			match(FIELDTYPE_MFCOLOR);
#line 758 "Vrml97Parser.g"
			ft = VrmlField::MFCOLOR;
#line 764 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFFLOAT:
		{
			match(FIELDTYPE_MFFLOAT);
#line 759 "Vrml97Parser.g"
			ft = VrmlField::MFFLOAT;
#line 772 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFINT32:
		{
			match(FIELDTYPE_MFINT32);
#line 760 "Vrml97Parser.g"
			ft = VrmlField::MFINT32;
#line 780 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFNODE:
		{
			match(FIELDTYPE_MFNODE);
#line 761 "Vrml97Parser.g"
			ft = VrmlField::MFNODE;
#line 788 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFROTATION:
		{
			match(FIELDTYPE_MFROTATION);
#line 762 "Vrml97Parser.g"
			ft = VrmlField::MFROTATION;
#line 796 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFSTRING:
		{
			match(FIELDTYPE_MFSTRING);
#line 763 "Vrml97Parser.g"
			ft = VrmlField::MFSTRING;
#line 804 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFTIME:
		{
			match(FIELDTYPE_MFTIME);
#line 764 "Vrml97Parser.g"
			ft = VrmlField::MFTIME;
#line 812 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC2F:
		{
			match(FIELDTYPE_MFVEC2F);
#line 765 "Vrml97Parser.g"
			ft = VrmlField::MFVEC2F;
#line 820 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC3F:
		{
			match(FIELDTYPE_MFVEC3F);
#line 766 "Vrml97Parser.g"
			ft = VrmlField::MFVEC3F;
#line 828 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFBOOL:
		{
			match(FIELDTYPE_SFBOOL);
#line 767 "Vrml97Parser.g"
			ft = VrmlField::SFBOOL;
#line 836 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFCOLOR:
		{
			match(FIELDTYPE_SFCOLOR);
#line 768 "Vrml97Parser.g"
			ft = VrmlField::SFCOLOR;
#line 844 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFFLOAT:
		{
			match(FIELDTYPE_SFFLOAT);
#line 769 "Vrml97Parser.g"
			ft = VrmlField::SFFLOAT;
#line 852 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFIMAGE:
		{
			match(FIELDTYPE_SFIMAGE);
#line 770 "Vrml97Parser.g"
			ft = VrmlField::SFIMAGE;
#line 860 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFINT32:
		{
			match(FIELDTYPE_SFINT32);
#line 771 "Vrml97Parser.g"
			ft = VrmlField::SFINT32;
#line 868 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFNODE:
		{
			match(FIELDTYPE_SFNODE);
#line 772 "Vrml97Parser.g"
			ft = VrmlField::SFNODE;
#line 876 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFROTATION:
		{
			match(FIELDTYPE_SFROTATION);
#line 773 "Vrml97Parser.g"
			ft = VrmlField::SFROTATION;
#line 884 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFSTRING:
		{
			match(FIELDTYPE_SFSTRING);
#line 774 "Vrml97Parser.g"
			ft = VrmlField::SFSTRING;
#line 892 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFTIME:
		{
			match(FIELDTYPE_SFTIME);
#line 775 "Vrml97Parser.g"
			ft = VrmlField::SFTIME;
#line 900 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC2F:
		{
			match(FIELDTYPE_SFVEC2F);
#line 776 "Vrml97Parser.g"
			ft = VrmlField::SFVEC2F;
#line 908 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC3F:
		{
			match(FIELDTYPE_SFVEC3F);
#line 777 "Vrml97Parser.g"
			ft = VrmlField::SFVEC3F;
#line 916 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_9);
	}
	return ft;
}

Vrml97Parser::NodeInterfaceType  Vrml97Parser::fieldInterfaceType() {
#line 268 "Vrml97Parser.g"
	Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
#line 936 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_FIELD:
		{
			match(KEYWORD_FIELD);
#line 269 "Vrml97Parser.g"
			it = FIELD;
#line 945 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		{
			match(KEYWORD_EXPOSEDFIELD);
#line 270 "Vrml97Parser.g"
			it = EXPOSEDFIELD;
#line 953 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_8);
	}
	return it;
}

VrmlField *  Vrml97Parser::fieldValue(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlField::VrmlFieldType ft
) {
#line 780 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 975 "Vrml97Parser.cpp"
	
	try {      // for error handling
		if (((_tokenSet_10.member(LA(1))))&&(    (ft == VrmlField::SFNODE)
          || (ft == VrmlField::MFNODE) )) {
			fv=nodeFieldValue(vrmlNamespace, doc, ft);
		}
		else if ((_tokenSet_11.member(LA(1)))) {
			fv=nonNodeFieldValue(ft);
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return fv;
}

VrmlNodePtr  Vrml97Parser::protoNodeStatement(
	Doc2 const * doc, VrmlNodeType & protoNodeType
) {
#line 298 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 1003 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case ID:
	{
		n=protoNode(doc, protoNodeType, std::string());
		break;
	}
	case KEYWORD_DEF:
	{
		match(KEYWORD_DEF);
		id0 = LT(1);
		match(ID);
		n=protoNode(doc, protoNodeType, id0->getText());
		break;
	}
	case KEYWORD_USE:
	{
		match(KEYWORD_USE);
		id1 = LT(1);
		match(ID);
#line 306 "Vrml97Parser.g"
		
		n = protoNodeType.scope()->findNode(id1->getText().c_str());
		if (!n) {
		throw antlr::SemanticException("Node \"" + id1->getText() + "\" has not been defined in this scope.");
		}
		
#line 1033 "Vrml97Parser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return n;
}

void Vrml97Parser::protoBodyStatement(
	Doc2 const * doc, VrmlNodeType & nodeType
) {
#line 285 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 1051 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, nodeType);
#line 290 "Vrml97Parser.g"
			
			assert(n);
			nodeType.addNode(*n);
			
#line 1065 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(*nodeType.scope(), doc);
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(*nodeType.scope());
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_13);
	}
}

VrmlNodePtr  Vrml97Parser::protoNode(
	Doc2 const * doc,
          VrmlNodeType & protoNodeType,
          std::string const & nodeId
) {
#line 588 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 1099 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 588 "Vrml97Parser.g"
	
	VrmlNodeType const * nodeType = 0;
	
#line 1106 "Vrml97Parser.cpp"
	
	if (((LA(1)==ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 601 "Vrml97Parser.g"
		
		nodeType = protoNodeType.scope()->findType("Script");
		assert(nodeType);
		
		n.reset(nodeType->newNode());
		
		if (nodeId.size() > 0) {
		n->setName(nodeId.c_str(), protoNodeType.scope());
		}
		
		VrmlNodeScript * const scriptNode = n->toScript();
		assert(scriptNode);
		
#line 1125 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			switch ( LA(1)) {
			case ID:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				protoNodeBodyElement(doc, protoNodeType, *n);
				break;
			}
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_FIELD:
			{
				protoScriptInterfaceDeclaration(doc, protoNodeType, *scriptNode);
				break;
			}
			default:
			{
				goto _loop39;
			}
			}
		}
		_loop39:;
		}
		match(RBRACE);
	}
	else if ((LA(1)==ID)) {
		nodeTypeId = LT(1);
		match(ID);
#line 620 "Vrml97Parser.g"
		
		nodeType = protoNodeType.scope()->findType(nodeTypeId->getText().c_str());
		if (!nodeType) {
		throw antlr::SemanticException("Unknown node type \"" + nodeTypeId->getText() + "\".");
		}
		
		n = VrmlNodePtr(nodeType->newNode());
		
		if (nodeId.size() > 0) {
		n->setName(nodeId.c_str(), protoNodeType.scope());
		}
		
#line 1171 "Vrml97Parser.cpp"
		match(LBRACE);
		{
		for (;;) {
			if ((_tokenSet_4.member(LA(1)))) {
				protoNodeBodyElement(doc, protoNodeType, *n);
			}
			else {
				goto _loop41;
			}
			
		}
		_loop41:;
		}
		match(RBRACE);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return n;
}

void Vrml97Parser::externInterfaceDeclaration(
	VrmlNodeType & nodeType
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 331 "Vrml97Parser.g"
	
	NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1203 "Vrml97Parser.cpp"
	
	try {      // for error handling
		it=interfaceType();
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 337 "Vrml97Parser.g"
		
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
		
#line 1233 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

VrmlMFString *  Vrml97Parser::externprotoUrlList() {
#line 366 "Vrml97Parser.g"
	VrmlMFString * msv = 0;
#line 1245 "Vrml97Parser.cpp"
#line 366 "Vrml97Parser.g"
	
	std::string s;
	
#line 1250 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case STRING:
		{
			s=stringValue();
#line 371 "Vrml97Parser.g"
			
			const char * const sPtr = s.c_str();
			msv = new VrmlMFString(1, &sPtr);
			
#line 1262 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 376 "Vrml97Parser.g"
			
			std::vector<std::string> stringVector;
			
#line 1272 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==STRING)) {
					s=stringValue();
#line 381 "Vrml97Parser.g"
					
					stringVector.push_back(s);
					
#line 1281 "Vrml97Parser.cpp"
				}
				else {
					goto _loop27;
				}
				
			}
			_loop27:;
			}
			match(RBRACKET);
#line 385 "Vrml97Parser.g"
			
			char const ** c_strs = new char const *[stringVector.size()];
			for (unsigned int i = 0; i < stringVector.size(); ++i) {
			c_strs[i] = stringVector[i].c_str();
			}
			
			msv = new VrmlMFString(stringVector.size(), c_strs);
			delete [] c_strs;
			
#line 1301 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_3);
	}
	return msv;
}

Vrml97Parser::NodeInterfaceType  Vrml97Parser::interfaceType() {
#line 361 "Vrml97Parser.g"
	Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
#line 1321 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		case KEYWORD_FIELD:
		{
			it=fieldInterfaceType();
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_8);
	}
	return it;
}

std::string  Vrml97Parser::stringValue() {
#line 1177 "Vrml97Parser.g"
	std::string str;
#line 1354 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		s = LT(1);
		match(STRING);
#line 1179 "Vrml97Parser.g"
		
		//
		// Why doesn't this work?
		//
		// str = std::string(s->getText().begin() + 1, s->getText().end() - 1);
		
		std::string temp(s->getText());
		str = std::string(temp.begin() + 1, temp.end() - 1);
		
#line 1370 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_14);
	}
	return str;
}

void Vrml97Parser::nodeBodyElement(
	VrmlNamespace & vrmlNamespace,
                Doc2 const * doc,
                VrmlNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 506 "Vrml97Parser.g"
	
	VrmlNodeType const & nodeType = node.nodeType();
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1391 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		{
			id = LT(1);
			match(ID);
#line 514 "Vrml97Parser.g"
			
			if (   ((ft = nodeType.hasField(id->getText().c_str())) == VrmlField::NO_FIELD)
			&& ((ft = nodeType.hasExposedField(id->getText().c_str())) == VrmlField::NO_FIELD)) {
			
			throw antlr::SemanticException(std::string(nodeType.getName()) + " node has no field or exposedField \"" + id->getText() + "\" (nodeBodyEl).");
			}
			
			VrmlField * fv = 0;
			
#line 1409 "Vrml97Parser.cpp"
			fv=fieldValue(vrmlNamespace, doc, ft);
#line 524 "Vrml97Parser.g"
			
			assert(fv);
			node.setField(id->getText().c_str(), *fv);
			delete fv;
			
#line 1417 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(vrmlNamespace);
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(vrmlNamespace, doc);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
}

void Vrml97Parser::scriptInterfaceDeclaration(
	VrmlNamespace & vrmlNamespace,
                           Doc2 const * doc,
                           VrmlNodeScript & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 533 "Vrml97Parser.g"
	
	NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1455 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 541 "Vrml97Parser.g"
			
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
			
#line 1485 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_FIELD:
		{
			scriptFieldInterfaceDeclaration(vrmlNamespace, doc, node);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
}

void Vrml97Parser::scriptFieldInterfaceDeclaration(
	VrmlNamespace & vrmlNamespace,
                                Doc2 const * doc,
                                VrmlNodeScript & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 562 "Vrml97Parser.g"
	
	VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD;
	VrmlField * fv = 0;
	
#line 1517 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
		fv=fieldValue(vrmlNamespace, doc, ft);
#line 570 "Vrml97Parser.g"
		
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
		
#line 1542 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
}

void Vrml97Parser::protoNodeBodyElement(
	Doc2 const * doc,
                     VrmlNodeType & protoNodeType,
                     VrmlNode & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 635 "Vrml97Parser.g"
	
	VrmlNodeType const & nodeType = node.nodeType();
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1563 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_ROUTE:
		{
			routeStatement(*protoNodeType.scope());
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(*protoNodeType.scope(), doc);
			break;
		}
		default:
			if (((LA(1)==ID))&&( nodeType.hasEventIn(LT(1)->getText().c_str()) != VrmlField::NO_FIELD ||
	  nodeType.hasEventOut(LT(1)->getText().c_str()) != VrmlField::NO_FIELD )) {
				eventId = LT(1);
				match(ID);
				isStatement(protoNodeType, node, eventId->getText());
			}
			else if ((LA(1)==ID)) {
				id = LT(1);
				match(ID);
#line 647 "Vrml97Parser.g"
				
				if (   ((ft = nodeType.hasField(id->getText().c_str())) == VrmlField::NO_FIELD)
				&& ((ft = nodeType.hasExposedField(id->getText().c_str())) == VrmlField::NO_FIELD)) {
				
				throw antlr::SemanticException(std::string(nodeType.getName()) + " node has no field or exposedField \"" + id->getText() + "\" (protoNodeBodyEl).");
				}
				
				VrmlField * fv = 0;
				
#line 1598 "Vrml97Parser.cpp"
				{
				switch ( LA(1)) {
				case LBRACKET:
				case ID:
				case INTEGER:
				case HEX_INTEGER:
				case REAL:
				case STRING:
				case KEYWORD_DEF:
				case KEYWORD_FALSE:
				case KEYWORD_NULL:
				case KEYWORD_TRUE:
				case KEYWORD_USE:
				{
					{
					fv=protoFieldValue(doc, protoNodeType, ft);
#line 659 "Vrml97Parser.g"
					
					assert(fv);
					node.setField(id->getText().c_str(), *fv);
					delete fv;
					
#line 1621 "Vrml97Parser.cpp"
					}
					break;
				}
				case KEYWORD_IS:
				{
					isStatement(protoNodeType, node, id->getText());
					break;
				}
				default:
				{
					throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
				}
				}
				}
			}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
}

void Vrml97Parser::protoScriptInterfaceDeclaration(
	Doc2 const * doc, VrmlNodeType & protoNodeType, VrmlNodeScript & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 698 "Vrml97Parser.g"
	
	NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1658 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 704 "Vrml97Parser.g"
			
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
			
#line 1688 "Vrml97Parser.cpp"
			{
			switch ( LA(1)) {
			case KEYWORD_IS:
			{
				isStatement(protoNodeType, node, id->getText());
				break;
			}
			case RBRACE:
			case ID:
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_FIELD:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
			break;
		}
		case KEYWORD_FIELD:
		{
			protoScriptFieldInterfaceDeclaration(doc, protoNodeType, node);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
}

void Vrml97Parser::isStatement(
	VrmlNodeType & protoNodeType,
            VrmlNode & node,
            std::string const & nodeInterfaceId
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(KEYWORD_IS);
		id = LT(1);
		match(ID);
#line 681 "Vrml97Parser.g"
		
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
		
		protoNodeType.addIS(id->getText().c_str(), node, nodeInterfaceId.c_str());
		
#line 1760 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
}

VrmlField *  Vrml97Parser::protoFieldValue(
	Doc2 const * doc, VrmlNodeType & protoNodeType, VrmlField::VrmlFieldType ft
) {
#line 786 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 1774 "Vrml97Parser.cpp"
	
	try {      // for error handling
		if (((_tokenSet_10.member(LA(1))))&&(    (ft == VrmlField::SFNODE)
          || (ft == VrmlField::MFNODE) )) {
			fv=protoNodeFieldValue(doc, protoNodeType, ft);
#line 789 "Vrml97Parser.g"
			
			assert(fv);
			
#line 1784 "Vrml97Parser.cpp"
		}
		else if ((_tokenSet_11.member(LA(1)))) {
			fv=nonNodeFieldValue(ft);
#line 793 "Vrml97Parser.g"
			
			assert(fv);
			
#line 1792 "Vrml97Parser.cpp"
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
	return fv;
}

void Vrml97Parser::protoScriptFieldInterfaceDeclaration(
	Doc2 const * doc, VrmlNodeType & protoNodeType, VrmlNodeScript & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 726 "Vrml97Parser.g"
	
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	VrmlField * fv = 0;
	
#line 1816 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 732 "Vrml97Parser.g"
		
		if (   (node.hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)
		|| (node.nodeType().hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)) {
		
		throw antlr::SemanticException("Interface \"" + id->getText() + "\" already declared for Script node.");
		}
		
		
#line 1832 "Vrml97Parser.cpp"
		{
		switch ( LA(1)) {
		case LBRACKET:
		case ID:
		case INTEGER:
		case HEX_INTEGER:
		case REAL:
		case STRING:
		case KEYWORD_DEF:
		case KEYWORD_FALSE:
		case KEYWORD_NULL:
		case KEYWORD_TRUE:
		case KEYWORD_USE:
		{
			{
			fv=protoFieldValue(doc, protoNodeType, ft);
#line 743 "Vrml97Parser.g"
			
			assert(fv);
			node.addField(id->getText().c_str(), ft, fv);
			delete fv;
			
#line 1855 "Vrml97Parser.cpp"
			}
			break;
		}
		case KEYWORD_IS:
		{
#line 750 "Vrml97Parser.g"
			
			node.addField(id->getText().c_str(), ft);
					
#line 1865 "Vrml97Parser.cpp"
			isStatement(protoNodeType, node, id->getText());
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
}

VrmlField *  Vrml97Parser::nodeFieldValue(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlField::VrmlFieldType ft
) {
#line 819 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 1888 "Vrml97Parser.cpp"
	
	if (((_tokenSet_16.member(LA(1))))&&( ft == VrmlField::SFNODE )) {
		fv=sfNodeValue(vrmlNamespace, doc);
	}
	else if ((_tokenSet_17.member(LA(1)))) {
		fv=mfNodeValue(vrmlNamespace, doc);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

VrmlField *  Vrml97Parser::nonNodeFieldValue(
	VrmlField::VrmlFieldType ft
) {
#line 798 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 1908 "Vrml97Parser.cpp"
	
	try {      // for error handling
		if (((LA(1)==KEYWORD_FALSE||LA(1)==KEYWORD_TRUE))&&( ft == VrmlField::SFBOOL )) {
			fv=sfBoolValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFCOLOR )) {
			fv=sfColorValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFFLOAT )) {
			fv=sfFloatValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==HEX_INTEGER))&&( ft == VrmlField::SFIMAGE )) {
			fv=sfImageValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==HEX_INTEGER))&&( ft == VrmlField::SFINT32 )) {
			fv=sfInt32Value();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFROTATION )) {
			fv=sfRotationValue();
		}
		else if (((LA(1)==STRING))&&( ft == VrmlField::SFSTRING )) {
			fv=sfStringValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFTIME )) {
			fv=sfTimeValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFVEC2F )) {
			fv=sfVec2fValue();
		}
		else if (((LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::SFVEC3F )) {
			fv=sfVec3fValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::MFCOLOR )) {
			fv=mfColorValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::MFFLOAT )) {
			fv=mfFloatValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==HEX_INTEGER))&&( ft == VrmlField::MFINT32 )) {
			fv=mfInt32Value();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::MFROTATION )) {
			fv=mfRotationValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==STRING))&&( ft == VrmlField::MFSTRING )) {
			fv=mfStringValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::MFTIME )) {
			fv=mfTimeValue();
		}
		else if (((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL))&&( ft == VrmlField::MFVEC2F )) {
			fv=mfVec2fValue();
		}
		else if ((LA(1)==LBRACKET||LA(1)==INTEGER||LA(1)==REAL)) {
			fv=mfVec3fValue();
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return fv;
}

VrmlField *  Vrml97Parser::protoNodeFieldValue(
	Doc2 const * doc, VrmlNodeType & protoNodeType, VrmlField::VrmlFieldType ft
) {
#line 827 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 1983 "Vrml97Parser.cpp"
	
	if (((_tokenSet_16.member(LA(1))))&&( ft == VrmlField::SFNODE )) {
		fv=protoSfNodeValue(doc, protoNodeType);
#line 832 "Vrml97Parser.g"
		
		assert(fv);
		
#line 1991 "Vrml97Parser.cpp"
	}
	else if ((_tokenSet_17.member(LA(1)))) {
		fv=protoMfNodeValue(doc, protoNodeType);
#line 836 "Vrml97Parser.g"
		
		assert(fv);
		
#line 1999 "Vrml97Parser.cpp"
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

VrmlSFBool *  Vrml97Parser::sfBoolValue() {
#line 841 "Vrml97Parser.g"
	VrmlSFBool * sbv = new VrmlSFBool();
#line 2011 "Vrml97Parser.cpp"
#line 841 "Vrml97Parser.g"
	
	bool val(false);
	
#line 2016 "Vrml97Parser.cpp"
	
	try {      // for error handling
		val=boolValue();
#line 845 "Vrml97Parser.g"
		*sbv = VrmlSFBool(val);
#line 2022 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return sbv;
}

VrmlSFColor *  Vrml97Parser::sfColorValue() {
#line 856 "Vrml97Parser.g"
	VrmlSFColor * scv = new VrmlSFColor();
#line 2035 "Vrml97Parser.cpp"
#line 856 "Vrml97Parser.g"
	
	float c[3];
	
#line 2040 "Vrml97Parser.cpp"
	
	try {      // for error handling
		colorValue(c);
#line 861 "Vrml97Parser.g"
		
		*scv = VrmlSFColor(c[0], c[1], c[2]);
		
#line 2048 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return scv;
}

VrmlSFFloat *  Vrml97Parser::sfFloatValue() {
#line 911 "Vrml97Parser.g"
	VrmlSFFloat * sfv = new VrmlSFFloat(0.0f);
#line 2061 "Vrml97Parser.cpp"
#line 911 "Vrml97Parser.g"
	
	float f(0.0f);
	
#line 2066 "Vrml97Parser.cpp"
	
	try {      // for error handling
		f=floatValue();
#line 916 "Vrml97Parser.g"
		
		*sfv = VrmlSFFloat(f);
		
#line 2074 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return sfv;
}

VrmlSFImage *  Vrml97Parser::sfImageValue() {
#line 949 "Vrml97Parser.g"
	VrmlSFImage * siv = new VrmlSFImage();
#line 2087 "Vrml97Parser.cpp"
#line 949 "Vrml97Parser.g"
	
	unsigned long w(0L), h(0L), com(0L), pixel(0L);
	
#line 2092 "Vrml97Parser.cpp"
	
	try {      // for error handling
		w=intValue();
		h=intValue();
		com=intValue();
#line 954 "Vrml97Parser.g"
		
		SimpleVector<unsigned char> pixelVector;
		
#line 2102 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==HEX_INTEGER)) {
				pixel=intValue();
#line 959 "Vrml97Parser.g"
				
				// need to confirm the cross-platform-ness of this, it
				// looks kind of ugly but might in fact be ok. basically,
				// we read the value as an integer, then strip off the
				// bytes one by one.
						for (int i = com - 1; i >= 0; i--) {
				pixelVector.add(static_cast<unsigned char>(pixel >> (8 * i) & 0xff));
				}
				
#line 2117 "Vrml97Parser.cpp"
			}
			else {
				goto _loop72;
			}
			
		}
		_loop72:;
		}
#line 969 "Vrml97Parser.g"
		
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
		
#line 2141 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return siv;
}

VrmlSFInt32 *  Vrml97Parser::sfInt32Value() {
#line 985 "Vrml97Parser.g"
	VrmlSFInt32 * siv = new VrmlSFInt32(0L);
#line 2154 "Vrml97Parser.cpp"
#line 985 "Vrml97Parser.g"
	
	long i(0L);
	
#line 2159 "Vrml97Parser.cpp"
	
	try {      // for error handling
		i=intValue();
#line 990 "Vrml97Parser.g"
		
		*siv = VrmlSFInt32(i);
		
#line 2167 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return siv;
}

VrmlSFRotation *  Vrml97Parser::sfRotationValue() {
#line 1085 "Vrml97Parser.g"
	VrmlSFRotation * srv = new VrmlSFRotation();
#line 2180 "Vrml97Parser.cpp"
#line 1085 "Vrml97Parser.g"
	
	float r[4];
	
#line 2185 "Vrml97Parser.cpp"
	
	try {      // for error handling
		rotationValue(r);
#line 1090 "Vrml97Parser.g"
		
		*srv = VrmlSFRotation(r[0], r[1], r[2], r[3]);
		
#line 2193 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return srv;
}

VrmlSFString *  Vrml97Parser::sfStringValue() {
#line 1137 "Vrml97Parser.g"
	VrmlSFString * ssv = new VrmlSFString();
#line 2206 "Vrml97Parser.cpp"
#line 1137 "Vrml97Parser.g"
	
	std::string s;
	
#line 2211 "Vrml97Parser.cpp"
	
	try {      // for error handling
		s=stringValue();
#line 1142 "Vrml97Parser.g"
		
		*ssv = VrmlSFString(s.c_str());
		
#line 2219 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return ssv;
}

VrmlSFTime *  Vrml97Parser::sfTimeValue() {
#line 1190 "Vrml97Parser.g"
	VrmlSFTime * stv = new VrmlSFTime(0.0);
#line 2232 "Vrml97Parser.cpp"
#line 1190 "Vrml97Parser.g"
	
	double t(0.0);
	
#line 2237 "Vrml97Parser.cpp"
	
	try {      // for error handling
		t=doubleValue();
#line 1195 "Vrml97Parser.g"
		
		*stv = VrmlSFTime(t);
		
#line 2245 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return stv;
}

VrmlSFVec2f *  Vrml97Parser::sfVec2fValue() {
#line 1228 "Vrml97Parser.g"
	VrmlSFVec2f * svv = new VrmlSFVec2f();
#line 2258 "Vrml97Parser.cpp"
#line 1228 "Vrml97Parser.g"
	
	float v[2];
	
#line 2263 "Vrml97Parser.cpp"
	
	try {      // for error handling
		vec2fValue(v);
#line 1233 "Vrml97Parser.g"
		
		*svv = VrmlSFVec2f(v[0], v[1]);
		
#line 2271 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return svv;
}

VrmlSFVec3f *  Vrml97Parser::sfVec3fValue() {
#line 1273 "Vrml97Parser.g"
	VrmlSFVec3f * svv = new VrmlSFVec3f();
#line 2284 "Vrml97Parser.cpp"
#line 1273 "Vrml97Parser.g"
	
	float v[3];
	
#line 2289 "Vrml97Parser.cpp"
	
	try {      // for error handling
		vec3fValue(v);
#line 1278 "Vrml97Parser.g"
		
		*svv = VrmlSFVec3f(v[0], v[1], v[2]);
		
#line 2297 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return svv;
}

VrmlMFColor *  Vrml97Parser::mfColorValue() {
#line 866 "Vrml97Parser.g"
	VrmlMFColor * mcv = new VrmlMFColor();
#line 2310 "Vrml97Parser.cpp"
#line 866 "Vrml97Parser.g"
	
	float c[3];
	
#line 2315 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			colorValue(c);
#line 871 "Vrml97Parser.g"
			
			*mcv = VrmlMFColor(1, c);
			
#line 2327 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 875 "Vrml97Parser.g"
			
			SimpleVector<float> colorVector;
			
#line 2337 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					colorValue(c);
#line 880 "Vrml97Parser.g"
					
					colorVector.add(c[0]);
					colorVector.add(c[1]);
					colorVector.add(c[2]);
					
#line 2348 "Vrml97Parser.cpp"
				}
				else {
					goto _loop62;
				}
				
			}
			_loop62:;
			}
			match(RBRACKET);
#line 886 "Vrml97Parser.g"
			
			*mcv = VrmlMFColor(colorVector.size() / 3L, colorVector.data());
			
#line 2362 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return mcv;
}

VrmlMFFloat *  Vrml97Parser::mfFloatValue() {
#line 921 "Vrml97Parser.g"
	VrmlMFFloat * mfv = new VrmlMFFloat();
#line 2382 "Vrml97Parser.cpp"
#line 921 "Vrml97Parser.g"
	
	float f(0.0f);
	
#line 2387 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			f=floatValue();
#line 926 "Vrml97Parser.g"
			
			*mfv = VrmlMFFloat(1, &f);
			
#line 2399 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 930 "Vrml97Parser.g"
			
			SimpleVector<float> floatVector;
			
#line 2409 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					f=floatValue();
#line 935 "Vrml97Parser.g"
					
					floatVector.add(f);
					
#line 2418 "Vrml97Parser.cpp"
				}
				else {
					goto _loop68;
				}
				
			}
			_loop68:;
			}
			match(RBRACKET);
#line 939 "Vrml97Parser.g"
			
			*mfv = VrmlMFFloat(floatVector.size(), floatVector.data());
			
#line 2432 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return mfv;
}

VrmlMFInt32 *  Vrml97Parser::mfInt32Value() {
#line 995 "Vrml97Parser.g"
	VrmlMFInt32 * miv = new VrmlMFInt32();
#line 2452 "Vrml97Parser.cpp"
#line 995 "Vrml97Parser.g"
	
	long i(0L);
	
#line 2457 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case HEX_INTEGER:
		{
			i=intValue();
#line 1000 "Vrml97Parser.g"
			
			*miv = VrmlMFInt32(1, &i);
			
#line 2469 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1004 "Vrml97Parser.g"
			
			SimpleVector<long> longVector;
			
#line 2479 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==HEX_INTEGER)) {
					i=intValue();
#line 1009 "Vrml97Parser.g"
					
					longVector.add(i);
					
#line 2488 "Vrml97Parser.cpp"
				}
				else {
					goto _loop76;
				}
				
			}
			_loop76:;
			}
			match(RBRACKET);
#line 1013 "Vrml97Parser.g"
			
			*miv = VrmlMFInt32(longVector.size(), longVector.data());
			
#line 2502 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return miv;
}

VrmlMFRotation *  Vrml97Parser::mfRotationValue() {
#line 1095 "Vrml97Parser.g"
	VrmlMFRotation * mrv = new VrmlMFRotation();
#line 2522 "Vrml97Parser.cpp"
#line 1095 "Vrml97Parser.g"
	
	float r[4];
	
#line 2527 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			rotationValue(r);
#line 1100 "Vrml97Parser.g"
			
			*mrv = VrmlMFRotation(1, r);
			
#line 2539 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1104 "Vrml97Parser.g"
			
			SimpleVector<float> floatVector;
			
#line 2549 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					rotationValue(r);
#line 1109 "Vrml97Parser.g"
					
					floatVector.add(r[0]);
					floatVector.add(r[1]);
					floatVector.add(r[2]);
					floatVector.add(r[3]);
					
#line 2561 "Vrml97Parser.cpp"
				}
				else {
					goto _loop89;
				}
				
			}
			_loop89:;
			}
			match(RBRACKET);
#line 1116 "Vrml97Parser.g"
			
			*mrv = VrmlMFRotation(floatVector.size() / 4L, floatVector.data());
			
#line 2575 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return mrv;
}

VrmlMFString *  Vrml97Parser::mfStringValue() {
#line 1147 "Vrml97Parser.g"
	VrmlMFString * msv = new VrmlMFString();
#line 2595 "Vrml97Parser.cpp"
#line 1147 "Vrml97Parser.g"
	
	std::string s;
	
#line 2600 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case STRING:
		{
			s=stringValue();
#line 1152 "Vrml97Parser.g"
			
			const char * const sPtr = s.c_str();
			*msv = VrmlMFString(1, &sPtr);
			
#line 2612 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1157 "Vrml97Parser.g"
			
			std::vector<std::string> stringVector;
			
#line 2622 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==STRING)) {
					s=stringValue();
#line 1162 "Vrml97Parser.g"
					
					stringVector.push_back(s);
					
#line 2631 "Vrml97Parser.cpp"
				}
				else {
					goto _loop94;
				}
				
			}
			_loop94:;
			}
			match(RBRACKET);
#line 1166 "Vrml97Parser.g"
			
			char const ** c_strs = new char const *[stringVector.size()];
			for (unsigned int i = 0; i < stringVector.size(); ++i) {
			c_strs[i] = stringVector[i].c_str();
			}
			
			*msv = VrmlMFString(stringVector.size(), c_strs);
			delete [] c_strs;
			
#line 2651 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return msv;
}

VrmlMFTime *  Vrml97Parser::mfTimeValue() {
#line 1200 "Vrml97Parser.g"
	VrmlMFTime * mtv = new VrmlMFTime();
#line 2671 "Vrml97Parser.cpp"
#line 1200 "Vrml97Parser.g"
	
	double t(0.0);
	
#line 2676 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			t=doubleValue();
#line 1205 "Vrml97Parser.g"
			
			*mtv = VrmlMFTime(1, &t);
			
#line 2688 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1209 "Vrml97Parser.g"
			
			SimpleVector<double> doubleVector;
			
#line 2698 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					t=doubleValue();
#line 1214 "Vrml97Parser.g"
					
					doubleVector.add(t);
					
#line 2707 "Vrml97Parser.cpp"
				}
				else {
					goto _loop99;
				}
				
			}
			_loop99:;
			}
			match(RBRACKET);
#line 1218 "Vrml97Parser.g"
			
			*mtv = VrmlMFTime(doubleVector.size(), doubleVector.data());
			
#line 2721 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return mtv;
}

VrmlMFVec2f *  Vrml97Parser::mfVec2fValue() {
#line 1238 "Vrml97Parser.g"
	VrmlMFVec2f * mvv = new VrmlMFVec2f();
#line 2741 "Vrml97Parser.cpp"
#line 1238 "Vrml97Parser.g"
	
	float v[2];
	
#line 2746 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			vec2fValue(v);
#line 1243 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec2f(1, v);
			
#line 2758 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1247 "Vrml97Parser.g"
			
			SimpleVector<float> floatVector;
			
#line 2768 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					vec2fValue(v);
#line 1252 "Vrml97Parser.g"
					
					floatVector.add(v[0]);
					floatVector.add(v[1]);
					
#line 2778 "Vrml97Parser.cpp"
				}
				else {
					goto _loop104;
				}
				
			}
			_loop104:;
			}
			match(RBRACKET);
#line 1257 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec2f(floatVector.size() / 2L, floatVector.data());
			
#line 2792 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return mvv;
}

VrmlMFVec3f *  Vrml97Parser::mfVec3fValue() {
#line 1283 "Vrml97Parser.g"
	VrmlMFVec3f * mvv = new VrmlMFVec3f();
#line 2812 "Vrml97Parser.cpp"
#line 1283 "Vrml97Parser.g"
	
	float v[3];
	
#line 2817 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			vec3fValue(v);
#line 1288 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec3f(1, v);
			
#line 2829 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1292 "Vrml97Parser.g"
			
			SimpleVector<float> floatVector;
			
#line 2839 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					vec3fValue(v);
#line 1297 "Vrml97Parser.g"
					
					floatVector.add(v[0]);
					floatVector.add(v[1]);
					floatVector.add(v[2]);
					
#line 2850 "Vrml97Parser.cpp"
				}
				else {
					goto _loop109;
				}
				
			}
			_loop109:;
			}
			match(RBRACKET);
#line 1303 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec3f(floatVector.size() / 3L, floatVector.data());
			
#line 2864 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return mvv;
}

VrmlSFNode *  Vrml97Parser::sfNodeValue(
	VrmlNamespace & vrmlNamespace,
            Doc2 const * doc
) {
#line 1023 "Vrml97Parser.g"
	VrmlSFNode * snv = new VrmlSFNode();
#line 2887 "Vrml97Parser.cpp"
#line 1023 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 2892 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(vrmlNamespace, doc);
#line 1030 "Vrml97Parser.g"
			
			*snv = VrmlSFNode(n);
			
#line 2905 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return snv;
}

VrmlMFNode *  Vrml97Parser::mfNodeValue(
	VrmlNamespace & vrmlNamespace, Doc2 const * doc
) {
#line 1049 "Vrml97Parser.g"
	VrmlMFNode * mnv = new VrmlMFNode();
#line 2932 "Vrml97Parser.cpp"
#line 1049 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 2937 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(vrmlNamespace, doc);
#line 1054 "Vrml97Parser.g"
			
			*mnv = VrmlMFNode(1, &n);
			
#line 2950 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{
			for (;;) {
				if ((LA(1)==ID||LA(1)==KEYWORD_DEF||LA(1)==KEYWORD_USE)) {
					n=nodeStatement(vrmlNamespace, doc);
#line 1061 "Vrml97Parser.g"
					
					mnv->addNode(*n);
					
#line 2964 "Vrml97Parser.cpp"
				}
				else {
					goto _loop82;
				}
				
			}
			_loop82:;
			}
			match(RBRACKET);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return mnv;
}

VrmlSFNode *  Vrml97Parser::protoSfNodeValue(
	Doc2 const * doc,
                 VrmlNodeType & protoNodeType
) {
#line 1036 "Vrml97Parser.g"
	VrmlSFNode * snv = new VrmlSFNode();
#line 2996 "Vrml97Parser.cpp"
#line 1036 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 3001 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, protoNodeType);
#line 1043 "Vrml97Parser.g"
			
			*snv = VrmlSFNode(n);
			
#line 3014 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_NULL:
		{
			match(KEYWORD_NULL);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
	return snv;
}

VrmlMFNode *  Vrml97Parser::protoMfNodeValue(
	Doc2 const * doc, VrmlNodeType & protoNodeType
) {
#line 1067 "Vrml97Parser.g"
	VrmlMFNode * mnv = new VrmlMFNode();
#line 3041 "Vrml97Parser.cpp"
#line 1067 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 3046 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, protoNodeType);
#line 1072 "Vrml97Parser.g"
			
			*mnv = VrmlMFNode(1, &n);
			
#line 3059 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{
			for (;;) {
				if ((LA(1)==ID||LA(1)==KEYWORD_DEF||LA(1)==KEYWORD_USE)) {
					n=protoNodeStatement(doc, protoNodeType);
#line 1079 "Vrml97Parser.g"
					
					mnv->addNode(*n);
					
#line 3073 "Vrml97Parser.cpp"
				}
				else {
					goto _loop85;
				}
				
			}
			_loop85:;
			}
			match(RBRACKET);
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_15);
	}
	return mnv;
}

bool  Vrml97Parser::boolValue() {
#line 848 "Vrml97Parser.g"
	bool val;
#line 3102 "Vrml97Parser.cpp"
#line 848 "Vrml97Parser.g"
	
	val = false;
	
#line 3107 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_TRUE:
		{
			match(KEYWORD_TRUE);
#line 852 "Vrml97Parser.g"
			val = true;
#line 3116 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_FALSE:
		{
			match(KEYWORD_FALSE);
#line 853 "Vrml97Parser.g"
			val = false;
#line 3124 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return val;
}

void Vrml97Parser::colorValue(
	float c[3]
) {
#line 891 "Vrml97Parser.g"
	
	float r(0.0f), g(0.0f), b(0.0f);
	
#line 3148 "Vrml97Parser.cpp"
	
	try {      // for error handling
		r=colorComponent();
		g=colorComponent();
		b=colorComponent();
#line 896 "Vrml97Parser.g"
		
		c[0] = r;
		c[1] = g;
		c[2] = b;
		
#line 3160 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
}

float  Vrml97Parser::colorComponent() {
#line 907 "Vrml97Parser.g"
	float val = 0.0f;
#line 3172 "Vrml97Parser.cpp"
	
	try {      // for error handling
		val=floatValue();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
	return val;
}

float  Vrml97Parser::floatValue() {
#line 944 "Vrml97Parser.g"
	float val = 0.0f;
#line 3188 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case REAL:
		{
			f0 = LT(1);
			match(REAL);
#line 945 "Vrml97Parser.g"
			val = atof(f0->getText().c_str());
#line 3200 "Vrml97Parser.cpp"
			break;
		}
		case INTEGER:
		{
			f1 = LT(1);
			match(INTEGER);
#line 946 "Vrml97Parser.g"
			val = atof(f1->getText().c_str());
#line 3209 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
	return val;
}

long  Vrml97Parser::intValue() {
#line 1018 "Vrml97Parser.g"
	long val = 0;
#line 3229 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  i0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		{
			i0 = LT(1);
			match(INTEGER);
#line 1019 "Vrml97Parser.g"
			val = atol(i0->getText().c_str());
#line 3241 "Vrml97Parser.cpp"
			break;
		}
		case HEX_INTEGER:
		{
			i1 = LT(1);
			match(HEX_INTEGER);
#line 1020 "Vrml97Parser.g"
			val = strtol(i1->getText().c_str(), 0, 16);
#line 3250 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_19);
	}
	return val;
}

void Vrml97Parser::rotationValue(
	float r[4]
) {
#line 1124 "Vrml97Parser.g"
	
	float x(0.0f), y(0.0f), z(0.0f), rot(0.0f);
	
#line 3274 "Vrml97Parser.cpp"
	
	try {      // for error handling
		x=floatValue();
		y=floatValue();
		z=floatValue();
		rot=floatValue();
#line 1129 "Vrml97Parser.g"
		
		r[0] = x;
		r[1] = y;
		r[2] = z;
		r[3] = rot;
		
#line 3288 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
}

double  Vrml97Parser::doubleValue() {
#line 1223 "Vrml97Parser.g"
	double val = 0.0;
#line 3300 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  d0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  d1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case REAL:
		{
			d0 = LT(1);
			match(REAL);
#line 1224 "Vrml97Parser.g"
			val = atof(d0->getText().c_str());
#line 3312 "Vrml97Parser.cpp"
			break;
		}
		case INTEGER:
		{
			d1 = LT(1);
			match(INTEGER);
#line 1225 "Vrml97Parser.g"
			val = atof(d1->getText().c_str());
#line 3321 "Vrml97Parser.cpp"
			break;
		}
		default:
		{
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		}
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
	return val;
}

void Vrml97Parser::vec2fValue(
	float v[2]
) {
#line 1262 "Vrml97Parser.g"
	
	float x(0.0f), y(0.0f);
	
#line 3345 "Vrml97Parser.cpp"
	
	try {      // for error handling
		x=floatValue();
		y=floatValue();
#line 1267 "Vrml97Parser.g"
		
		v[0] = x;
		v[1] = y;
		
#line 3355 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
}

void Vrml97Parser::vec3fValue(
	float v[3]
) {
#line 1308 "Vrml97Parser.g"
	
	float x(0.0f), y(0.0f), z(0.0f);
	
#line 3371 "Vrml97Parser.cpp"
	
	try {      // for error handling
		x=floatValue();
		y=floatValue();
		z=floatValue();
#line 1313 "Vrml97Parser.g"
		
		v[0] = x;
		v[1] = y;
		v[2] = z;
		
#line 3383 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
}

const char* Vrml97Parser::_tokenNames[] = {
	"<0>",
	"EOF",
	"<2>",
	"NULL_TREE_LOOKAHEAD",
	".",
	"[",
	"]",
	"{",
	"}",
	"an identifier",
	"an integer value",
	"an integer value",
	"a floating point value",
	"a string",
	"\"DEF\"",
	"\"eventIn\"",
	"\"eventOut\"",
	"\"exposedField\"",
	"\"EXTERNPROTO\"",
	"\"FALSE\"",
	"\"field\"",
	"\"IS\"",
	"\"NULL\"",
	"\"PROTO\"",
	"\"ROUTE\"",
	"\"TO\"",
	"\"TRUE\"",
	"\"USE\"",
	"FIELDTYPE_SFBOOL",
	"FIELDTYPE_SFCOLOR",
	"FIELDTYPE_SFFLOAT",
	"FIELDTYPE_SFIMAGE",
	"FIELDTYPE_SFINT32",
	"FIELDTYPE_SFNODE",
	"FIELDTYPE_SFROTATION",
	"FIELDTYPE_SFSTRING",
	"FIELDTYPE_SFTIME",
	"FIELDTYPE_SFVEC2F",
	"FIELDTYPE_SFVEC3F",
	"FIELDTYPE_MFCOLOR",
	"FIELDTYPE_MFFLOAT",
	"FIELDTYPE_MFINT32",
	"FIELDTYPE_MFNODE",
	"FIELDTYPE_MFROTATION",
	"FIELDTYPE_MFSTRING",
	"FIELDTYPE_MFTIME",
	"FIELDTYPE_MFVEC2F",
	"FIELDTYPE_MFVEC3F",
	0
};

const unsigned long Vrml97Parser::_tokenSet_0_data_[] = { 159662592UL, 0UL, 0UL, 0UL };
// ID "DEF" "EXTERNPROTO" "PROTO" "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long Vrml97Parser::_tokenSet_1_data_[] = { 2UL, 0UL, 0UL, 0UL };
// EOF 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long Vrml97Parser::_tokenSet_2_data_[] = { 159662594UL, 0UL, 0UL, 0UL };
// EOF ID "DEF" "EXTERNPROTO" "PROTO" "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long Vrml97Parser::_tokenSet_3_data_[] = { 160809730UL, 0UL, 0UL, 0UL };
// EOF RBRACE ID "DEF" "eventIn" "eventOut" "EXTERNPROTO" "field" "PROTO" 
// "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long Vrml97Parser::_tokenSet_4_data_[] = { 25428480UL, 0UL, 0UL, 0UL };
// ID "EXTERNPROTO" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long Vrml97Parser::_tokenSet_5_data_[] = { 1277952UL, 0UL, 0UL, 0UL };
// "eventIn" "eventOut" "exposedField" "field" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long Vrml97Parser::_tokenSet_6_data_[] = { 1278016UL, 0UL, 0UL, 0UL };
// RBRACKET "eventIn" "eventOut" "exposedField" "field" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long Vrml97Parser::_tokenSet_7_data_[] = { 256UL, 0UL, 0UL, 0UL };
// RBRACE 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long Vrml97Parser::_tokenSet_8_data_[] = { 4026531840UL, 65535UL, 0UL, 0UL };
// FIELDTYPE_SFBOOL FIELDTYPE_SFCOLOR FIELDTYPE_SFFLOAT FIELDTYPE_SFIMAGE 
// FIELDTYPE_SFINT32 FIELDTYPE_SFNODE FIELDTYPE_SFROTATION FIELDTYPE_SFSTRING 
// FIELDTYPE_SFTIME FIELDTYPE_SFVEC2F FIELDTYPE_SFVEC3F FIELDTYPE_MFCOLOR 
// FIELDTYPE_MFFLOAT FIELDTYPE_MFINT32 FIELDTYPE_MFNODE FIELDTYPE_MFROTATION 
// FIELDTYPE_MFSTRING FIELDTYPE_MFTIME FIELDTYPE_MFVEC2F FIELDTYPE_MFVEC3F 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long Vrml97Parser::_tokenSet_9_data_[] = { 512UL, 0UL, 0UL, 0UL };
// ID 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_9(_tokenSet_9_data_,4);
const unsigned long Vrml97Parser::_tokenSet_10_data_[] = { 138428960UL, 0UL, 0UL, 0UL };
// LBRACKET ID "DEF" "NULL" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_10(_tokenSet_10_data_,4);
const unsigned long Vrml97Parser::_tokenSet_11_data_[] = { 67648544UL, 0UL, 0UL, 0UL };
// LBRACKET INTEGER HEX_INTEGER REAL STRING "FALSE" "TRUE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_11(_tokenSet_11_data_,4);
const unsigned long Vrml97Parser::_tokenSet_12_data_[] = { 26706752UL, 0UL, 0UL, 0UL };
// RBRACKET RBRACE ID "eventIn" "eventOut" "exposedField" "EXTERNPROTO" 
// "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_12(_tokenSet_12_data_,4);
const unsigned long Vrml97Parser::_tokenSet_13_data_[] = { 159662848UL, 0UL, 0UL, 0UL };
// RBRACE ID "DEF" "EXTERNPROTO" "PROTO" "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_13(_tokenSet_13_data_,4);
const unsigned long Vrml97Parser::_tokenSet_14_data_[] = { 160949058UL, 0UL, 0UL, 0UL };
// EOF RBRACKET RBRACE ID STRING "DEF" "eventIn" "eventOut" "exposedField" 
// "EXTERNPROTO" "field" "PROTO" "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_14(_tokenSet_14_data_,4);
const unsigned long Vrml97Parser::_tokenSet_15_data_[] = { 26575616UL, 0UL, 0UL, 0UL };
// RBRACE ID "eventIn" "eventOut" "EXTERNPROTO" "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_15(_tokenSet_15_data_,4);
const unsigned long Vrml97Parser::_tokenSet_16_data_[] = { 138428928UL, 0UL, 0UL, 0UL };
// ID "DEF" "NULL" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_16(_tokenSet_16_data_,4);
const unsigned long Vrml97Parser::_tokenSet_17_data_[] = { 134234656UL, 0UL, 0UL, 0UL };
// LBRACKET ID "DEF" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_17(_tokenSet_17_data_,4);
const unsigned long Vrml97Parser::_tokenSet_18_data_[] = { 26711872UL, 0UL, 0UL, 0UL };
// RBRACKET RBRACE ID INTEGER REAL "eventIn" "eventOut" "exposedField" 
// "EXTERNPROTO" "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_18(_tokenSet_18_data_,4);
const unsigned long Vrml97Parser::_tokenSet_19_data_[] = { 26709824UL, 0UL, 0UL, 0UL };
// RBRACKET RBRACE ID INTEGER HEX_INTEGER "eventIn" "eventOut" "exposedField" 
// "EXTERNPROTO" "field" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet Vrml97Parser::_tokenSet_19(_tokenSet_19_data_,4);


