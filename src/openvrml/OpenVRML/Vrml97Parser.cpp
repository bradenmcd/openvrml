/* $ANTLR 2.7.1: "Vrml97Parser.g" -> "Vrml97Parser.cpp"$ */
#include "Vrml97Parser.hpp"
#include "antlr/NoViableAltException.hpp"
#include "antlr/SemanticException.hpp"
#line 34 "Vrml97Parser.g"

# include <assert.h>
# include "doc2.hpp"
# include "VrmlNamespace.h"
# include "VrmlNodeType.h"
# include "VrmlNode.h"
# include "VrmlNodeScript.h"

#line 15 "Vrml97Parser.cpp"
#line 1 "Vrml97Parser.g"

#line 18 "Vrml97Parser.cpp"
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
#line 63 "Vrml97Parser.g"
	
	VrmlNodePtr node;
	
#line 81 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			node=nodeStatement(vrmlNamespace, doc);
#line 68 "Vrml97Parser.g"
			
			assert(node);
			mfNode.addNode(*node);
			
#line 95 "Vrml97Parser.cpp"
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
#line 76 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 127 "Vrml97Parser.cpp"
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
#line 84 "Vrml97Parser.g"
		
		n = vrmlNamespace.findNode(id1->getText().c_str());
		if (!n) {
		throw antlr::SemanticException("Node \"" + id1->getText() + "\" has not been defined in this scope.");
		}
		
#line 157 "Vrml97Parser.cpp"
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
#line 309 "Vrml97Parser.g"
		
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
		
#line 268 "Vrml97Parser.cpp"
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
#line 360 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 282 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 360 "Vrml97Parser.g"
	
	VrmlNodeType const * nodeType = 0;
	
#line 289 "Vrml97Parser.cpp"
	
	if (((LA(1)==ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 371 "Vrml97Parser.g"
		
		nodeType = vrmlNamespace.findType("Script");
		assert(nodeType);
		
		n = VrmlNodePtr(nodeType->newNode());
		
		if (nodeId.size() > 0) {
		n->setName(nodeId.c_str(), &vrmlNamespace);
		}
		
		VrmlNodeScript * const scriptNode = n->toScript();
		assert(scriptNode);
		
#line 308 "Vrml97Parser.cpp"
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
#line 390 "Vrml97Parser.g"
		
		nodeType = vrmlNamespace.findType(nodeTypeId->getText().c_str());
		if (!nodeType) {
		throw antlr::SemanticException("Unknown node type \"" + nodeTypeId->getText() + "\".");
		}
		
		n = VrmlNodePtr(nodeType->newNode());
		
		if (nodeId.size() > 0) {
		n->setName(nodeId.c_str(), &vrmlNamespace);
		}
		
#line 354 "Vrml97Parser.cpp"
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
#line 215 "Vrml97Parser.g"
		
		VrmlNodeType * nodeType = new VrmlNodeType(id->getText().c_str());
		
#line 390 "Vrml97Parser.cpp"
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
#line 219 "Vrml97Parser.g"
		
		VrmlMFString * urlList = 0;
		
#line 409 "Vrml97Parser.cpp"
		urlList=externprotoUrlList();
#line 223 "Vrml97Parser.g"
		
		nodeType->setUrl(*urlList, doc);
		delete urlList;
		vrmlNamespace.addNodeType(nodeType);
		
#line 417 "Vrml97Parser.cpp"
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
#line 99 "Vrml97Parser.g"
		
		VrmlNodeType * nodeType = new VrmlNodeType(id->getText().c_str());
		nodeType->setScope(vrmlNamespace);
		
#line 440 "Vrml97Parser.cpp"
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
#line 106 "Vrml97Parser.g"
		
		vrmlNamespace.addNodeType(nodeType);
		
#line 462 "Vrml97Parser.cpp"
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
#line 111 "Vrml97Parser.g"
	
	NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
	VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD;
	
#line 481 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id0 = LT(1);
			match(ID);
#line 117 "Vrml97Parser.g"
			
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
			
#line 511 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		case KEYWORD_FIELD:
		{
			it=fieldInterfaceType();
			ft=fieldType();
			id1 = LT(1);
			match(ID);
#line 136 "Vrml97Parser.g"
			
			VrmlField * fv = 0;
			
#line 525 "Vrml97Parser.cpp"
			fv=fieldValue(vrmlNamespace, doc, ft);
#line 140 "Vrml97Parser.g"
			
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
			
#line 548 "Vrml97Parser.cpp"
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
#line 172 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 571 "Vrml97Parser.cpp"
	
	try {      // for error handling
		{
		for (;;) {
			if ((LA(1)==KEYWORD_EXTERNPROTO||LA(1)==KEYWORD_PROTO)) {
				protoStatement(*nodeType.getScope(), doc);
			}
			else {
				goto _loop15;
			}
			
		}
		_loop15:;
		}
		n=protoNodeStatement(doc, nodeType);
#line 177 "Vrml97Parser.g"
		
		assert(n);
		nodeType.addNode(*n);
		
#line 592 "Vrml97Parser.cpp"
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
#line 162 "Vrml97Parser.g"
	Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
#line 616 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		{
			match(KEYWORD_EVENTIN);
#line 163 "Vrml97Parser.g"
			it = EVENTIN;
#line 625 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EVENTOUT:
		{
			match(KEYWORD_EVENTOUT);
#line 164 "Vrml97Parser.g"
			it = EVENTOUT;
#line 633 "Vrml97Parser.cpp"
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
#line 653 "Vrml97Parser.g"
	VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD;
#line 653 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case FIELDTYPE_MFCOLOR:
		{
			match(FIELDTYPE_MFCOLOR);
#line 654 "Vrml97Parser.g"
			ft = VrmlField::MFCOLOR;
#line 662 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFFLOAT:
		{
			match(FIELDTYPE_MFFLOAT);
#line 655 "Vrml97Parser.g"
			ft = VrmlField::MFFLOAT;
#line 670 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFINT32:
		{
			match(FIELDTYPE_MFINT32);
#line 656 "Vrml97Parser.g"
			ft = VrmlField::MFINT32;
#line 678 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFNODE:
		{
			match(FIELDTYPE_MFNODE);
#line 657 "Vrml97Parser.g"
			ft = VrmlField::MFNODE;
#line 686 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFROTATION:
		{
			match(FIELDTYPE_MFROTATION);
#line 658 "Vrml97Parser.g"
			ft = VrmlField::MFROTATION;
#line 694 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFSTRING:
		{
			match(FIELDTYPE_MFSTRING);
#line 659 "Vrml97Parser.g"
			ft = VrmlField::MFSTRING;
#line 702 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFTIME:
		{
			match(FIELDTYPE_MFTIME);
#line 660 "Vrml97Parser.g"
			ft = VrmlField::MFTIME;
#line 710 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC2F:
		{
			match(FIELDTYPE_MFVEC2F);
#line 661 "Vrml97Parser.g"
			ft = VrmlField::MFVEC2F;
#line 718 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_MFVEC3F:
		{
			match(FIELDTYPE_MFVEC3F);
#line 662 "Vrml97Parser.g"
			ft = VrmlField::MFVEC3F;
#line 726 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFBOOL:
		{
			match(FIELDTYPE_SFBOOL);
#line 663 "Vrml97Parser.g"
			ft = VrmlField::SFBOOL;
#line 734 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFCOLOR:
		{
			match(FIELDTYPE_SFCOLOR);
#line 664 "Vrml97Parser.g"
			ft = VrmlField::SFCOLOR;
#line 742 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFFLOAT:
		{
			match(FIELDTYPE_SFFLOAT);
#line 665 "Vrml97Parser.g"
			ft = VrmlField::SFFLOAT;
#line 750 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFIMAGE:
		{
			match(FIELDTYPE_SFIMAGE);
#line 666 "Vrml97Parser.g"
			ft = VrmlField::SFIMAGE;
#line 758 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFINT32:
		{
			match(FIELDTYPE_SFINT32);
#line 667 "Vrml97Parser.g"
			ft = VrmlField::SFINT32;
#line 766 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFNODE:
		{
			match(FIELDTYPE_SFNODE);
#line 668 "Vrml97Parser.g"
			ft = VrmlField::SFNODE;
#line 774 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFROTATION:
		{
			match(FIELDTYPE_SFROTATION);
#line 669 "Vrml97Parser.g"
			ft = VrmlField::SFROTATION;
#line 782 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFSTRING:
		{
			match(FIELDTYPE_SFSTRING);
#line 670 "Vrml97Parser.g"
			ft = VrmlField::SFSTRING;
#line 790 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFTIME:
		{
			match(FIELDTYPE_SFTIME);
#line 671 "Vrml97Parser.g"
			ft = VrmlField::SFTIME;
#line 798 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC2F:
		{
			match(FIELDTYPE_SFVEC2F);
#line 672 "Vrml97Parser.g"
			ft = VrmlField::SFVEC2F;
#line 806 "Vrml97Parser.cpp"
			break;
		}
		case FIELDTYPE_SFVEC3F:
		{
			match(FIELDTYPE_SFVEC3F);
#line 673 "Vrml97Parser.g"
			ft = VrmlField::SFVEC3F;
#line 814 "Vrml97Parser.cpp"
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
#line 167 "Vrml97Parser.g"
	Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
#line 834 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_FIELD:
		{
			match(KEYWORD_FIELD);
#line 168 "Vrml97Parser.g"
			it = FIELD;
#line 843 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXPOSEDFIELD:
		{
			match(KEYWORD_EXPOSEDFIELD);
#line 169 "Vrml97Parser.g"
			it = EXPOSEDFIELD;
#line 851 "Vrml97Parser.cpp"
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
#line 676 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 873 "Vrml97Parser.cpp"
	
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
#line 197 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 901 "Vrml97Parser.cpp"
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
#line 205 "Vrml97Parser.g"
		
		n = protoNodeType.getScope()->findNode(id1->getText().c_str());
		if (!n) {
		throw antlr::SemanticException("Node \"" + id1->getText() + "\" has not been defined in this scope.");
		}
		
#line 931 "Vrml97Parser.cpp"
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
#line 184 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 949 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, nodeType);
#line 189 "Vrml97Parser.g"
			
			assert(n);
			nodeType.addNode(*n);
			
#line 963 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(*nodeType.getScope(), doc);
			break;
		}
		case KEYWORD_ROUTE:
		{
			routeStatement(*nodeType.getScope());
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
#line 484 "Vrml97Parser.g"
	VrmlNodePtr n;
#line 997 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 484 "Vrml97Parser.g"
	
	VrmlNodeType const * nodeType = 0;
	
#line 1004 "Vrml97Parser.cpp"
	
	if (((LA(1)==ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
#line 497 "Vrml97Parser.g"
		
		nodeType = protoNodeType.getScope()->findType("Script");
		assert(nodeType);
		
		n.reset(nodeType->newNode());
		
		if (nodeId.size() > 0) {
		n->setName(nodeId.c_str(), protoNodeType.getScope());
		}
		
		VrmlNodeScript * const scriptNode = n->toScript();
		assert(scriptNode);
		
#line 1023 "Vrml97Parser.cpp"
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
#line 516 "Vrml97Parser.g"
		
		nodeType = protoNodeType.getScope()->findType(nodeTypeId->getText().c_str());
		if (!nodeType) {
		throw antlr::SemanticException("Unknown node type \"" + nodeTypeId->getText() + "\".");
		}
		
		n = VrmlNodePtr(nodeType->newNode());
		
		if (nodeId.size() > 0) {
		n->setName(nodeId.c_str(), protoNodeType.getScope());
		}
		
#line 1069 "Vrml97Parser.cpp"
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
#line 230 "Vrml97Parser.g"
	
	NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1101 "Vrml97Parser.cpp"
	
	try {      // for error handling
		it=interfaceType();
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 236 "Vrml97Parser.g"
		
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
		
#line 1131 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_6);
	}
}

VrmlMFString *  Vrml97Parser::externprotoUrlList() {
#line 265 "Vrml97Parser.g"
	VrmlMFString * msv = 0;
#line 1143 "Vrml97Parser.cpp"
#line 265 "Vrml97Parser.g"
	
	std::string s;
	
#line 1148 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case STRING:
		{
			s=stringValue();
#line 270 "Vrml97Parser.g"
			
			const char * const sPtr = s.c_str();
			msv = new VrmlMFString(1, &sPtr);
			
#line 1160 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 275 "Vrml97Parser.g"
			
			std::vector<std::string> stringVector;
			
#line 1170 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==STRING)) {
					s=stringValue();
#line 280 "Vrml97Parser.g"
					
					stringVector.push_back(s);
					
#line 1179 "Vrml97Parser.cpp"
				}
				else {
					goto _loop27;
				}
				
			}
			_loop27:;
			}
			match(RBRACKET);
#line 284 "Vrml97Parser.g"
			
			char const ** c_strs = new char const *[stringVector.size()];
			for (unsigned int i = 0; i < stringVector.size(); ++i) {
			c_strs[i] = stringVector[i].c_str();
			}
			
			msv = new VrmlMFString(stringVector.size(), c_strs);
			delete [] c_strs;
			
#line 1199 "Vrml97Parser.cpp"
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
#line 260 "Vrml97Parser.g"
	Vrml97Parser::NodeInterfaceType it = INVALID_NODE_INTERFACE_TYPE;
#line 1219 "Vrml97Parser.cpp"
	
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
#line 1073 "Vrml97Parser.g"
	std::string str;
#line 1252 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		s = LT(1);
		match(STRING);
#line 1075 "Vrml97Parser.g"
		
		//
		// Why doesn't this work?
		//
		// str = std::string(s->getText().begin() + 1, s->getText().end() - 1);
		
		std::string temp(s->getText());
		str = std::string(temp.begin() + 1, temp.end() - 1);
		
#line 1268 "Vrml97Parser.cpp"
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
#line 405 "Vrml97Parser.g"
	
	VrmlNodeType const & nodeType = node.nodeType();
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1289 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		{
			id = LT(1);
			match(ID);
#line 413 "Vrml97Parser.g"
			
			if (   ((ft = nodeType.hasField(id->getText().c_str())) == VrmlField::NO_FIELD)
			&& ((ft = nodeType.hasExposedField(id->getText().c_str())) == VrmlField::NO_FIELD)) {
			
			throw antlr::SemanticException(std::string(nodeType.getName()) + " node has no field or exposedField \"" + id->getText() + "\" (nodeBodyEl).");
			}
			
			VrmlField * fv = 0;
			
#line 1307 "Vrml97Parser.cpp"
			fv=fieldValue(vrmlNamespace, doc, ft);
#line 423 "Vrml97Parser.g"
			
			assert(fv);
			node.setField(id->getText().c_str(), *fv);
			delete fv;
			
#line 1315 "Vrml97Parser.cpp"
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
                           const Doc2 * doc,
                           VrmlNodeScript & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 432 "Vrml97Parser.g"
	
	NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1353 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 440 "Vrml97Parser.g"
			
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
			
#line 1383 "Vrml97Parser.cpp"
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
                                const Doc2 * doc,
                                VrmlNodeScript & node
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 461 "Vrml97Parser.g"
	
	VrmlField::VrmlFieldType ft = VrmlField::NO_FIELD;
	VrmlField * fv = 0;
	
#line 1415 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
		fv=fieldValue(vrmlNamespace, doc, ft);
#line 469 "Vrml97Parser.g"
		
		assert(fv);
		
		const std::auto_ptr<VrmlField> autofv(fv);
		
		if (   (node.hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)
		|| (node.nodeType().hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)) {
		
		throw antlr::SemanticException("Interface \"" + id->getText() + "\" already declared for Script node.");
		}
		
		node.addField(id->getText().c_str(), ft, autofv.get());
		
#line 1437 "Vrml97Parser.cpp"
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
#line 531 "Vrml97Parser.g"
	
	VrmlNodeType const & nodeType = node.nodeType();
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1458 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_ROUTE:
		{
			routeStatement(*protoNodeType.getScope());
			break;
		}
		case KEYWORD_EXTERNPROTO:
		case KEYWORD_PROTO:
		{
			protoStatement(*protoNodeType.getScope(), doc);
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
#line 543 "Vrml97Parser.g"
				
				if (   ((ft = nodeType.hasField(id->getText().c_str())) == VrmlField::NO_FIELD)
				&& ((ft = nodeType.hasExposedField(id->getText().c_str())) == VrmlField::NO_FIELD)) {
				
				throw antlr::SemanticException(std::string(nodeType.getName()) + " node has no field or exposedField \"" + id->getText() + "\" (protoNodeBodyEl).");
				}
				
				VrmlField * fv = 0;
				
#line 1493 "Vrml97Parser.cpp"
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
#line 555 "Vrml97Parser.g"
					
					assert(fv);
					node.setField(id->getText().c_str(), *fv);
					delete fv;
					
#line 1516 "Vrml97Parser.cpp"
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
#line 594 "Vrml97Parser.g"
	
	NodeInterfaceType it(INVALID_NODE_INTERFACE_TYPE);
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	
#line 1553 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_EVENTIN:
		case KEYWORD_EVENTOUT:
		{
			it=eventInterfaceType();
			ft=fieldType();
			id = LT(1);
			match(ID);
#line 600 "Vrml97Parser.g"
			
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
			
#line 1583 "Vrml97Parser.cpp"
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
#line 577 "Vrml97Parser.g"
		
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
		
#line 1655 "Vrml97Parser.cpp"
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
#line 682 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 1669 "Vrml97Parser.cpp"
	
	try {      // for error handling
		if (((_tokenSet_10.member(LA(1))))&&(    (ft == VrmlField::SFNODE)
          || (ft == VrmlField::MFNODE) )) {
			fv=protoNodeFieldValue(doc, protoNodeType, ft);
#line 685 "Vrml97Parser.g"
			
			assert(fv);
			
#line 1679 "Vrml97Parser.cpp"
		}
		else if ((_tokenSet_11.member(LA(1)))) {
			fv=nonNodeFieldValue(ft);
#line 689 "Vrml97Parser.g"
			
			assert(fv);
			
#line 1687 "Vrml97Parser.cpp"
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
#line 622 "Vrml97Parser.g"
	
	VrmlField::VrmlFieldType ft(VrmlField::NO_FIELD);
	VrmlField * fv = 0;
	
#line 1711 "Vrml97Parser.cpp"
	
	try {      // for error handling
		match(KEYWORD_FIELD);
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 628 "Vrml97Parser.g"
		
		if (   (node.hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)
		|| (node.nodeType().hasInterface(id->getText().c_str()) != VrmlField::NO_FIELD)) {
		
		throw antlr::SemanticException("Interface \"" + id->getText() + "\" already declared for Script node.");
		}
		
		
#line 1727 "Vrml97Parser.cpp"
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
#line 639 "Vrml97Parser.g"
			
			assert(fv);
			node.addField(id->getText().c_str(), ft, fv);
			delete fv;
			
#line 1750 "Vrml97Parser.cpp"
			}
			break;
		}
		case KEYWORD_IS:
		{
#line 646 "Vrml97Parser.g"
			
			node.addField(id->getText().c_str(), ft);
					
#line 1760 "Vrml97Parser.cpp"
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
#line 715 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 1783 "Vrml97Parser.cpp"
	
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
#line 694 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 1803 "Vrml97Parser.cpp"
	
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
#line 723 "Vrml97Parser.g"
	VrmlField * fv = 0;
#line 1878 "Vrml97Parser.cpp"
	
	if (((_tokenSet_16.member(LA(1))))&&( ft == VrmlField::SFNODE )) {
		fv=protoSfNodeValue(doc, protoNodeType);
#line 728 "Vrml97Parser.g"
		
		assert(fv);
		
#line 1886 "Vrml97Parser.cpp"
	}
	else if ((_tokenSet_17.member(LA(1)))) {
		fv=protoMfNodeValue(doc, protoNodeType);
#line 732 "Vrml97Parser.g"
		
		assert(fv);
		
#line 1894 "Vrml97Parser.cpp"
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

VrmlSFBool *  Vrml97Parser::sfBoolValue() {
#line 737 "Vrml97Parser.g"
	VrmlSFBool * sbv = new VrmlSFBool();
#line 1906 "Vrml97Parser.cpp"
#line 737 "Vrml97Parser.g"
	
	bool val(false);
	
#line 1911 "Vrml97Parser.cpp"
	
	try {      // for error handling
		val=boolValue();
#line 741 "Vrml97Parser.g"
		*sbv = VrmlSFBool(val);
#line 1917 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return sbv;
}

VrmlSFColor *  Vrml97Parser::sfColorValue() {
#line 752 "Vrml97Parser.g"
	VrmlSFColor * scv = new VrmlSFColor();
#line 1930 "Vrml97Parser.cpp"
#line 752 "Vrml97Parser.g"
	
	float c[3];
	
#line 1935 "Vrml97Parser.cpp"
	
	try {      // for error handling
		colorValue(c);
#line 757 "Vrml97Parser.g"
		
		*scv = VrmlSFColor(c[0], c[1], c[2]);
		
#line 1943 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return scv;
}

VrmlSFFloat *  Vrml97Parser::sfFloatValue() {
#line 807 "Vrml97Parser.g"
	VrmlSFFloat * sfv = new VrmlSFFloat(0.0f);
#line 1956 "Vrml97Parser.cpp"
#line 807 "Vrml97Parser.g"
	
	float f(0.0f);
	
#line 1961 "Vrml97Parser.cpp"
	
	try {      // for error handling
		f=floatValue();
#line 812 "Vrml97Parser.g"
		
		*sfv = VrmlSFFloat(f);
		
#line 1969 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return sfv;
}

VrmlSFImage *  Vrml97Parser::sfImageValue() {
#line 845 "Vrml97Parser.g"
	VrmlSFImage * siv = new VrmlSFImage();
#line 1982 "Vrml97Parser.cpp"
#line 845 "Vrml97Parser.g"
	
	unsigned long w(0L), h(0L), com(0L), pixel(0L);
	
#line 1987 "Vrml97Parser.cpp"
	
	try {      // for error handling
		w=intValue();
		h=intValue();
		com=intValue();
#line 850 "Vrml97Parser.g"
		
		std::vector<unsigned char> pixelVector;
		
#line 1997 "Vrml97Parser.cpp"
		{
		for (;;) {
			if ((LA(1)==INTEGER||LA(1)==HEX_INTEGER)) {
				pixel=intValue();
#line 855 "Vrml97Parser.g"
				
				// need to confirm the cross-platform-ness of this, it
				// looks kind of ugly but might in fact be ok. basically,
				// we read the value as an integer, then strip off the
				// bytes one by one.
						for (int i = com - 1; i >= 0; i--) {
				pixelVector.push_back(static_cast<unsigned char>(pixel >> (8 * i) & 0xff));
				}
				
#line 2012 "Vrml97Parser.cpp"
			}
			else {
				goto _loop72;
			}
			
		}
		_loop72:;
		}
#line 865 "Vrml97Parser.g"
		
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
		*siv = VrmlSFImage(w, h, com, &pixelVector[0]); // hmmmm...
		
#line 2036 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return siv;
}

VrmlSFInt32 *  Vrml97Parser::sfInt32Value() {
#line 881 "Vrml97Parser.g"
	VrmlSFInt32 * siv = new VrmlSFInt32(0L);
#line 2049 "Vrml97Parser.cpp"
#line 881 "Vrml97Parser.g"
	
	long i(0L);
	
#line 2054 "Vrml97Parser.cpp"
	
	try {      // for error handling
		i=intValue();
#line 886 "Vrml97Parser.g"
		
		*siv = VrmlSFInt32(i);
		
#line 2062 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return siv;
}

VrmlSFRotation *  Vrml97Parser::sfRotationValue() {
#line 981 "Vrml97Parser.g"
	VrmlSFRotation * srv = new VrmlSFRotation();
#line 2075 "Vrml97Parser.cpp"
#line 981 "Vrml97Parser.g"
	
	float r[4];
	
#line 2080 "Vrml97Parser.cpp"
	
	try {      // for error handling
		rotationValue(r);
#line 986 "Vrml97Parser.g"
		
		*srv = VrmlSFRotation(r[0], r[1], r[2], r[3]);
		
#line 2088 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return srv;
}

VrmlSFString *  Vrml97Parser::sfStringValue() {
#line 1033 "Vrml97Parser.g"
	VrmlSFString * ssv = new VrmlSFString();
#line 2101 "Vrml97Parser.cpp"
#line 1033 "Vrml97Parser.g"
	
	std::string s;
	
#line 2106 "Vrml97Parser.cpp"
	
	try {      // for error handling
		s=stringValue();
#line 1038 "Vrml97Parser.g"
		
		*ssv = VrmlSFString(s.c_str());
		
#line 2114 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return ssv;
}

VrmlSFTime *  Vrml97Parser::sfTimeValue() {
#line 1086 "Vrml97Parser.g"
	VrmlSFTime * stv = new VrmlSFTime(0.0);
#line 2127 "Vrml97Parser.cpp"
#line 1086 "Vrml97Parser.g"
	
	double t(0.0);
	
#line 2132 "Vrml97Parser.cpp"
	
	try {      // for error handling
		t=doubleValue();
#line 1091 "Vrml97Parser.g"
		
		*stv = VrmlSFTime(t);
		
#line 2140 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return stv;
}

VrmlSFVec2f *  Vrml97Parser::sfVec2fValue() {
#line 1124 "Vrml97Parser.g"
	VrmlSFVec2f * svv = new VrmlSFVec2f();
#line 2153 "Vrml97Parser.cpp"
#line 1124 "Vrml97Parser.g"
	
	float v[2];
	
#line 2158 "Vrml97Parser.cpp"
	
	try {      // for error handling
		vec2fValue(v);
#line 1129 "Vrml97Parser.g"
		
		*svv = VrmlSFVec2f(v[0], v[1]);
		
#line 2166 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return svv;
}

VrmlSFVec3f *  Vrml97Parser::sfVec3fValue() {
#line 1169 "Vrml97Parser.g"
	VrmlSFVec3f * svv = new VrmlSFVec3f();
#line 2179 "Vrml97Parser.cpp"
#line 1169 "Vrml97Parser.g"
	
	float v[3];
	
#line 2184 "Vrml97Parser.cpp"
	
	try {      // for error handling
		vec3fValue(v);
#line 1174 "Vrml97Parser.g"
		
		*svv = VrmlSFVec3f(v[0], v[1], v[2]);
		
#line 2192 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_12);
	}
	return svv;
}

VrmlMFColor *  Vrml97Parser::mfColorValue() {
#line 762 "Vrml97Parser.g"
	VrmlMFColor * mcv = new VrmlMFColor();
#line 2205 "Vrml97Parser.cpp"
#line 762 "Vrml97Parser.g"
	
	float c[3];
	
#line 2210 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			colorValue(c);
#line 767 "Vrml97Parser.g"
			
			*mcv = VrmlMFColor(1, c);
			
#line 2222 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 771 "Vrml97Parser.g"
			
			std::vector<float> colorVector;
			
#line 2232 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					colorValue(c);
#line 776 "Vrml97Parser.g"
					
					colorVector.push_back(c[0]);
					colorVector.push_back(c[1]);
					colorVector.push_back(c[2]);
					
#line 2243 "Vrml97Parser.cpp"
				}
				else {
					goto _loop62;
				}
				
			}
			_loop62:;
			}
			match(RBRACKET);
#line 782 "Vrml97Parser.g"
			
			*mcv = VrmlMFColor(colorVector.size() / 3L, &colorVector[0]);
			
#line 2257 "Vrml97Parser.cpp"
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
#line 817 "Vrml97Parser.g"
	VrmlMFFloat * mfv = new VrmlMFFloat();
#line 2277 "Vrml97Parser.cpp"
#line 817 "Vrml97Parser.g"
	
	float f(0.0f);
	
#line 2282 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			f=floatValue();
#line 822 "Vrml97Parser.g"
			
			*mfv = VrmlMFFloat(1, &f);
			
#line 2294 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 826 "Vrml97Parser.g"
			
			std::vector<float> floatVector;
			
#line 2304 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					f=floatValue();
#line 831 "Vrml97Parser.g"
					
					floatVector.push_back(f);
					
#line 2313 "Vrml97Parser.cpp"
				}
				else {
					goto _loop68;
				}
				
			}
			_loop68:;
			}
			match(RBRACKET);
#line 835 "Vrml97Parser.g"
			
			*mfv = VrmlMFFloat(floatVector.size(), &floatVector[0]);
			
#line 2327 "Vrml97Parser.cpp"
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
#line 891 "Vrml97Parser.g"
	VrmlMFInt32 * miv = new VrmlMFInt32();
#line 2347 "Vrml97Parser.cpp"
#line 891 "Vrml97Parser.g"
	
	long i(0L);
	
#line 2352 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case HEX_INTEGER:
		{
			i=intValue();
#line 896 "Vrml97Parser.g"
			
			*miv = VrmlMFInt32(1, &i);
			
#line 2364 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 900 "Vrml97Parser.g"
			
			std::vector<long> longVector;
			
#line 2374 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==HEX_INTEGER)) {
					i=intValue();
#line 905 "Vrml97Parser.g"
					
					longVector.push_back(i);
					
#line 2383 "Vrml97Parser.cpp"
				}
				else {
					goto _loop76;
				}
				
			}
			_loop76:;
			}
			match(RBRACKET);
#line 909 "Vrml97Parser.g"
			
			*miv = VrmlMFInt32(longVector.size(), &longVector[0]);
			
#line 2397 "Vrml97Parser.cpp"
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
#line 991 "Vrml97Parser.g"
	VrmlMFRotation * mrv = new VrmlMFRotation();
#line 2417 "Vrml97Parser.cpp"
#line 991 "Vrml97Parser.g"
	
	float r[4];
	
#line 2422 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			rotationValue(r);
#line 996 "Vrml97Parser.g"
			
			*mrv = VrmlMFRotation(1, r);
			
#line 2434 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1000 "Vrml97Parser.g"
			
			std::vector<float> floatVector;
			
#line 2444 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					rotationValue(r);
#line 1005 "Vrml97Parser.g"
					
					floatVector.push_back(r[0]);
					floatVector.push_back(r[1]);
					floatVector.push_back(r[2]);
					floatVector.push_back(r[3]);
					
#line 2456 "Vrml97Parser.cpp"
				}
				else {
					goto _loop89;
				}
				
			}
			_loop89:;
			}
			match(RBRACKET);
#line 1012 "Vrml97Parser.g"
			
			*mrv = VrmlMFRotation(floatVector.size() / 4L, &floatVector[0]);
			
#line 2470 "Vrml97Parser.cpp"
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
#line 1043 "Vrml97Parser.g"
	VrmlMFString * msv = new VrmlMFString();
#line 2490 "Vrml97Parser.cpp"
#line 1043 "Vrml97Parser.g"
	
	std::string s;
	
#line 2495 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case STRING:
		{
			s=stringValue();
#line 1048 "Vrml97Parser.g"
			
			const char * const sPtr = s.c_str();
			*msv = VrmlMFString(1, &sPtr);
			
#line 2507 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1053 "Vrml97Parser.g"
			
			std::vector<std::string> stringVector;
			
#line 2517 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==STRING)) {
					s=stringValue();
#line 1058 "Vrml97Parser.g"
					
					stringVector.push_back(s);
					
#line 2526 "Vrml97Parser.cpp"
				}
				else {
					goto _loop94;
				}
				
			}
			_loop94:;
			}
			match(RBRACKET);
#line 1062 "Vrml97Parser.g"
			
			char const ** c_strs = new char const *[stringVector.size()];
			for (unsigned int i = 0; i < stringVector.size(); ++i) {
			c_strs[i] = stringVector[i].c_str();
			}
			
			*msv = VrmlMFString(stringVector.size(), c_strs);
			delete [] c_strs;
			
#line 2546 "Vrml97Parser.cpp"
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
#line 1096 "Vrml97Parser.g"
	VrmlMFTime * mtv = new VrmlMFTime();
#line 2566 "Vrml97Parser.cpp"
#line 1096 "Vrml97Parser.g"
	
	double t(0.0);
	
#line 2571 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			t=doubleValue();
#line 1101 "Vrml97Parser.g"
			
			*mtv = VrmlMFTime(1, &t);
			
#line 2583 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1105 "Vrml97Parser.g"
			
			std::vector<double> doubleVector;
			
#line 2593 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					t=doubleValue();
#line 1110 "Vrml97Parser.g"
					
					doubleVector.push_back(t);
					
#line 2602 "Vrml97Parser.cpp"
				}
				else {
					goto _loop99;
				}
				
			}
			_loop99:;
			}
			match(RBRACKET);
#line 1114 "Vrml97Parser.g"
			
			*mtv = VrmlMFTime(doubleVector.size(), &doubleVector[0]);
			
#line 2616 "Vrml97Parser.cpp"
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
#line 1134 "Vrml97Parser.g"
	VrmlMFVec2f * mvv = new VrmlMFVec2f();
#line 2636 "Vrml97Parser.cpp"
#line 1134 "Vrml97Parser.g"
	
	float v[2];
	
#line 2641 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			vec2fValue(v);
#line 1139 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec2f(1, v);
			
#line 2653 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1143 "Vrml97Parser.g"
			
			std::vector<float> floatVector;
			
#line 2663 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					vec2fValue(v);
#line 1148 "Vrml97Parser.g"
					
					floatVector.push_back(v[0]);
					floatVector.push_back(v[1]);
					
#line 2673 "Vrml97Parser.cpp"
				}
				else {
					goto _loop104;
				}
				
			}
			_loop104:;
			}
			match(RBRACKET);
#line 1153 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec2f(floatVector.size() / 2L, &floatVector[0]);
			
#line 2687 "Vrml97Parser.cpp"
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
#line 1179 "Vrml97Parser.g"
	VrmlMFVec3f * mvv = new VrmlMFVec3f();
#line 2707 "Vrml97Parser.cpp"
#line 1179 "Vrml97Parser.g"
	
	float v[3];
	
#line 2712 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		case REAL:
		{
			vec3fValue(v);
#line 1184 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec3f(1, v);
			
#line 2724 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
#line 1188 "Vrml97Parser.g"
			
			std::vector<float> floatVector;
			
#line 2734 "Vrml97Parser.cpp"
			{
			for (;;) {
				if ((LA(1)==INTEGER||LA(1)==REAL)) {
					vec3fValue(v);
#line 1193 "Vrml97Parser.g"
					
					floatVector.push_back(v[0]);
					floatVector.push_back(v[1]);
					floatVector.push_back(v[2]);
					
#line 2745 "Vrml97Parser.cpp"
				}
				else {
					goto _loop109;
				}
				
			}
			_loop109:;
			}
			match(RBRACKET);
#line 1199 "Vrml97Parser.g"
			
			*mvv = VrmlMFVec3f(floatVector.size() / 3L, &floatVector[0]);
			
#line 2759 "Vrml97Parser.cpp"
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
#line 919 "Vrml97Parser.g"
	VrmlSFNode * snv = new VrmlSFNode();
#line 2782 "Vrml97Parser.cpp"
#line 919 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 2787 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(vrmlNamespace, doc);
#line 926 "Vrml97Parser.g"
			
			*snv = VrmlSFNode(n);
			
#line 2800 "Vrml97Parser.cpp"
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
#line 945 "Vrml97Parser.g"
	VrmlMFNode * mnv = new VrmlMFNode();
#line 2827 "Vrml97Parser.cpp"
#line 945 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 2832 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=nodeStatement(vrmlNamespace, doc);
#line 950 "Vrml97Parser.g"
			
			*mnv = VrmlMFNode(1, &n);
			
#line 2845 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{
			for (;;) {
				if ((LA(1)==ID||LA(1)==KEYWORD_DEF||LA(1)==KEYWORD_USE)) {
					n=nodeStatement(vrmlNamespace, doc);
#line 957 "Vrml97Parser.g"
					
					mnv->addNode(*n);
					
#line 2859 "Vrml97Parser.cpp"
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
#line 932 "Vrml97Parser.g"
	VrmlSFNode * snv = new VrmlSFNode();
#line 2891 "Vrml97Parser.cpp"
#line 932 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 2896 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, protoNodeType);
#line 939 "Vrml97Parser.g"
			
			*snv = VrmlSFNode(n);
			
#line 2909 "Vrml97Parser.cpp"
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
#line 963 "Vrml97Parser.g"
	VrmlMFNode * mnv = new VrmlMFNode();
#line 2936 "Vrml97Parser.cpp"
#line 963 "Vrml97Parser.g"
	
	VrmlNodePtr n;
	
#line 2941 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case ID:
		case KEYWORD_DEF:
		case KEYWORD_USE:
		{
			n=protoNodeStatement(doc, protoNodeType);
#line 968 "Vrml97Parser.g"
			
			*mnv = VrmlMFNode(1, &n);
			
#line 2954 "Vrml97Parser.cpp"
			break;
		}
		case LBRACKET:
		{
			match(LBRACKET);
			{
			for (;;) {
				if ((LA(1)==ID||LA(1)==KEYWORD_DEF||LA(1)==KEYWORD_USE)) {
					n=protoNodeStatement(doc, protoNodeType);
#line 975 "Vrml97Parser.g"
					
					mnv->addNode(*n);
					
#line 2968 "Vrml97Parser.cpp"
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
#line 744 "Vrml97Parser.g"
	bool val;
#line 2997 "Vrml97Parser.cpp"
#line 744 "Vrml97Parser.g"
	
	val = false;
	
#line 3002 "Vrml97Parser.cpp"
	
	try {      // for error handling
		switch ( LA(1)) {
		case KEYWORD_TRUE:
		{
			match(KEYWORD_TRUE);
#line 748 "Vrml97Parser.g"
			val = true;
#line 3011 "Vrml97Parser.cpp"
			break;
		}
		case KEYWORD_FALSE:
		{
			match(KEYWORD_FALSE);
#line 749 "Vrml97Parser.g"
			val = false;
#line 3019 "Vrml97Parser.cpp"
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
#line 787 "Vrml97Parser.g"
	
	float r(0.0f), g(0.0f), b(0.0f);
	
#line 3043 "Vrml97Parser.cpp"
	
	try {      // for error handling
		r=colorComponent();
		g=colorComponent();
		b=colorComponent();
#line 792 "Vrml97Parser.g"
		
		c[0] = r;
		c[1] = g;
		c[2] = b;
		
#line 3055 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
}

float  Vrml97Parser::colorComponent() {
#line 803 "Vrml97Parser.g"
	float val = 0.0f;
#line 3067 "Vrml97Parser.cpp"
	
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
#line 840 "Vrml97Parser.g"
	float val = 0.0f;
#line 3083 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case REAL:
		{
			f0 = LT(1);
			match(REAL);
#line 841 "Vrml97Parser.g"
			val = atof(f0->getText().c_str());
#line 3095 "Vrml97Parser.cpp"
			break;
		}
		case INTEGER:
		{
			f1 = LT(1);
			match(INTEGER);
#line 842 "Vrml97Parser.g"
			val = atof(f1->getText().c_str());
#line 3104 "Vrml97Parser.cpp"
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
#line 914 "Vrml97Parser.g"
	long val = 0;
#line 3124 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  i0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case INTEGER:
		{
			i0 = LT(1);
			match(INTEGER);
#line 915 "Vrml97Parser.g"
			val = atol(i0->getText().c_str());
#line 3136 "Vrml97Parser.cpp"
			break;
		}
		case HEX_INTEGER:
		{
			i1 = LT(1);
			match(HEX_INTEGER);
#line 916 "Vrml97Parser.g"
			val = strtol(i1->getText().c_str(), 0, 16);
#line 3145 "Vrml97Parser.cpp"
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
#line 1020 "Vrml97Parser.g"
	
	float x(0.0f), y(0.0f), z(0.0f), rot(0.0f);
	
#line 3169 "Vrml97Parser.cpp"
	
	try {      // for error handling
		x=floatValue();
		y=floatValue();
		z=floatValue();
		rot=floatValue();
#line 1025 "Vrml97Parser.g"
		
		r[0] = x;
		r[1] = y;
		r[2] = z;
		r[3] = rot;
		
#line 3183 "Vrml97Parser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_18);
	}
}

double  Vrml97Parser::doubleValue() {
#line 1119 "Vrml97Parser.g"
	double val = 0.0;
#line 3195 "Vrml97Parser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  d0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  d1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		switch ( LA(1)) {
		case REAL:
		{
			d0 = LT(1);
			match(REAL);
#line 1120 "Vrml97Parser.g"
			val = atof(d0->getText().c_str());
#line 3207 "Vrml97Parser.cpp"
			break;
		}
		case INTEGER:
		{
			d1 = LT(1);
			match(INTEGER);
#line 1121 "Vrml97Parser.g"
			val = atof(d1->getText().c_str());
#line 3216 "Vrml97Parser.cpp"
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
#line 1158 "Vrml97Parser.g"
	
	float x(0.0f), y(0.0f);
	
#line 3240 "Vrml97Parser.cpp"
	
	try {      // for error handling
		x=floatValue();
		y=floatValue();
#line 1163 "Vrml97Parser.g"
		
		v[0] = x;
		v[1] = y;
		
#line 3250 "Vrml97Parser.cpp"
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
#line 1204 "Vrml97Parser.g"
	
	float x(0.0f), y(0.0f), z(0.0f);
	
#line 3266 "Vrml97Parser.cpp"
	
	try {      // for error handling
		x=floatValue();
		y=floatValue();
		z=floatValue();
#line 1209 "Vrml97Parser.g"
		
		v[0] = x;
		v[1] = y;
		v[2] = z;
		
#line 3278 "Vrml97Parser.cpp"
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


