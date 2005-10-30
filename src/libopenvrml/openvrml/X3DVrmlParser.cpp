/* $ANTLR 2.7.4: "expandedX3DVrmlParser.g" -> "X3DVrmlParser.cpp"$ */
#line 45 "expandedX3DVrmlParser.g"

# include "X3DVrmlParser.hpp"
ANTLR_RBRACE

#line 8 "X3DVrmlParser.cpp"
#include "X3DVrmlParser.hpp"
#include <antlr/NoViableAltException.hpp>
#include <antlr/SemanticException.hpp>
#include <antlr/ASTFactory.hpp>
#line 49 "expandedX3DVrmlParser.g"

# include <cctype>
# include <antlr/CommonToken.hpp>
# include <boost/lexical_cast.hpp>
# include <private.h>
# include "scope.h"
# include "script.h"

namespace {
const int X3DVrmlScanner::COLON                  (48);
const int X3DVrmlScanner::KEYWORD_AS             (49);
const int X3DVrmlScanner::KEYWORD_COMPONENT      (50);
const int X3DVrmlScanner::KEYWORD_EXPORT         (51);
const int X3DVrmlScanner::KEYWORD_IMPORT         (52);
const int X3DVrmlScanner::KEYWORD_META           (53);
const int X3DVrmlScanner::KEYWORD_PROFILE        (54);
const int X3DVrmlScanner::KEYWORD_INPUTONLY      (55);
const int X3DVrmlScanner::KEYWORD_OUTPUTONLY     (56);
const int X3DVrmlScanner::KEYWORD_INPUTOUTPUT    (57);
const int X3DVrmlScanner::KEYWORD_INITIALIZEONLY (58);
const int X3DVrmlScanner::FIELDTYPE_SFCOLORRGBA  (59);
const int X3DVrmlScanner::FIELDTYPE_SFDOUBLE     (60);
const int X3DVrmlScanner::FIELDTYPE_SFVEC2D      (61);
const int X3DVrmlScanner::FIELDTYPE_SFVEC3D      (62);
const int X3DVrmlScanner::FIELDTYPE_MFBOOL       (63);
const int X3DVrmlScanner::FIELDTYPE_MFCOLORRGBA  (64);
const int X3DVrmlScanner::FIELDTYPE_MFDOUBLE     (65);
const int X3DVrmlScanner::FIELDTYPE_MFIMAGE      (66);
const int X3DVrmlScanner::FIELDTYPE_MFVEC2D      (67);
const int X3DVrmlScanner::FIELDTYPE_MFVEC3D      (68);

X3DVrmlScanner::X3DVrmlScanner(std::istream & in):
    Vrml97Scanner(in)
{}

inline void X3DVrmlScanner::identifyKeyword(antlr::Token & token)
{
    this->Vrml97Scanner::identifyKeyword(token);
    if (token.getType() == antlr::Token::INVALID_TYPE) {
        const std::string token_text(token.getText());
        if (token_text == "AS") {
            token.setType(KEYWORD_AS);
        } else if (token_text == "COMPONENT") {
            token.setType(KEYWORD_COMPONENT);
        } else if (token_text == "EXPORT") {
            token.setType(KEYWORD_EXPORT);
        } else if (token_text == "IMPORT") {
            token.setType(KEYWORD_IMPORT);
        } else if (token_text == "META") {
            token.setType(KEYWORD_META);
        } else if (token_text == "PROFILE") {
            token.setType(KEYWORD_PROFILE);
        } else if (token_text == "inputOnly") {
            this->expectFieldType();
            token.setType(KEYWORD_INPUTONLY);
        } else if (token_text == "outputOnly") {
            this->expectFieldType();
            token.setType(KEYWORD_OUTPUTONLY);
        } else if (token_text == "inputOutput") {
            this->expectFieldType();
            token.setType(KEYWORD_INPUTOUTPUT);
        } else if (token_text == "initializeOnly") {
            this->expectFieldType();
            token.setType(KEYWORD_INITIALIZEONLY);
        }
    }
}

inline void X3DVrmlScanner::identifyFieldType(antlr::Token & token)
{
    this->Vrml97Scanner::identifyFieldType(token);

    if (token.getType() == antlr::Token::INVALID_TYPE) {
        const std::string token_text(token.getText());
        if (token_text == "SFColorRGBA") {
            token.setType(FIELDTYPE_SFCOLORRGBA);
        } else if (token_text == "SFDouble") {
            token.setType(FIELDTYPE_SFDOUBLE);
        } else if (token_text == "SFVec2d") {
            token.setType(FIELDTYPE_SFVEC2D);
        } else if (token_text == "SFVec3d") {
            token.setType(FIELDTYPE_SFVEC3D);
        } else if (token_text == "MFBool") {
            token.setType(FIELDTYPE_MFBOOL);
        } else if (token_text == "MFColorRGBA") {
            token.setType(FIELDTYPE_MFCOLORRGBA);
        } else if (token_text == "MFDouble") {
            token.setType(FIELDTYPE_MFDOUBLE);
        } else if (token_text == "MFImage") {
            token.setType(FIELDTYPE_MFIMAGE);
        } else if (token_text == "MFVec2d") {
            token.setType(FIELDTYPE_MFVEC2D);
        } else if (token_text == "MFVec3d") {
            token.setType(FIELDTYPE_MFVEC3D);
        }
    }
}


} // namespace

namespace openvrml ANTLR_LBRACE


#line 118 "X3DVrmlParser.cpp"
#line 1 "expandedX3DVrmlParser.g"
#line 120 "X3DVrmlParser.cpp"
X3DVrmlParser::X3DVrmlParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,k)
{
}

X3DVrmlParser::X3DVrmlParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(tokenBuf,1)
{
}

X3DVrmlParser::X3DVrmlParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,k)
{
}

X3DVrmlParser::X3DVrmlParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(lexer,1)
{
}

X3DVrmlParser::X3DVrmlParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state)
: ANTLR_USE_NAMESPACE(antlr)LLkParser(state,1)
{
}

void X3DVrmlParser::vrmlScene(
	openvrml::scene & scene,
          std::vector<boost::intrusive_ptr<openvrml::node> > & nodes
) {
#line 174 "expandedX3DVrmlParser.g"
	
	profile_id profile = invalid_profile_id;
	std::auto_ptr<openvrml::scope> root_scope_auto_ptr =
	scene.browser().create_root_scope(this->uri);
	const boost::shared_ptr<openvrml::scope> root_scope(root_scope_auto_ptr);
	
#line 157 "X3DVrmlParser.cpp"
	
	profile=profileStatement();
#line 185 "expandedX3DVrmlParser.g"
	scene.profile_ = profile;
#line 162 "X3DVrmlParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == KEYWORD_COMPONENT)) {
			componentStatement();
		}
		else {
			goto _loop3;
		}
		
	}
	_loop3:;
	} // ( ... )*
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == KEYWORD_META)) {
			metaStatement();
		}
		else {
			goto _loop5;
		}
		
	}
	_loop5:;
	} // ( ... )*
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			statement(scene.browser(), nodes, root_scope);
		}
		else {
			goto _loop7;
		}
		
	}
	_loop7:;
	} // ( ... )*
}

openvrml::profile_id  X3DVrmlParser::profileStatement() {
#line 190 "expandedX3DVrmlParser.g"
	openvrml::profile_id profile = invalid_profile_id;
#line 204 "X3DVrmlParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	try {      // for error handling
		match(KEYWORD_PROFILE);
		id = LT(1);
		match(ID);
#line 190 "expandedX3DVrmlParser.g"
		
		std::string profile_id = id->getText();
		// XXX
		// XXX Checks for supported profiles should go here.
		// XXX
		if (false) {
		} else {
		throw antlr::SemanticException("Unsupported profile \""
		+ id->getText() + "\".",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
#line 226 "X3DVrmlParser.cpp"
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
	return profile;
}

void X3DVrmlParser::componentStatement() {
#line 206 "expandedX3DVrmlParser.g"
	
	size_t level;
	
#line 241 "X3DVrmlParser.cpp"
	
	try {      // for error handling
		match(KEYWORD_COMPONENT);
		match(ID);
		match(COLON);
		level=intValue();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_1);
	}
}

void X3DVrmlParser::metaStatement() {
#line 212 "expandedX3DVrmlParser.g"
	
	std::string key, value;
	
#line 261 "X3DVrmlParser.cpp"
	
	try {      // for error handling
		match(KEYWORD_META);
		key=stringValue();
		value=stringValue();
	}
	catch (ANTLR_USE_NAMESPACE(antlr)RecognitionException& ex) {
		reportError(ex);
		consume();
		consumeUntil(_tokenSet_2);
	}
}

void X3DVrmlParser::statement(
	openvrml::browser & browser,
          std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
          const boost::shared_ptr<openvrml::scope> & scope
) {
#line 403 "expandedX3DVrmlParser.g"
	
	boost::intrusive_ptr<openvrml::node> node;
	boost::shared_ptr<node_type> nodeType;
	
#line 285 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case ID:
	case KEYWORD_DEF:
	case KEYWORD_USE:
	{
		node=nodeStatement(browser, scope, std::string());
#line 413 "expandedX3DVrmlParser.g"
		
		//
		// If we are unable to parse a node, node will be null.
		//
		if (node) { nodes.push_back(node); }
		
#line 300 "X3DVrmlParser.cpp"
		break;
	}
	case KEYWORD_EXTERNPROTO:
	case KEYWORD_PROTO:
	{
		protoStatement(browser, scope);
		break;
	}
	case KEYWORD_ROUTE:
	{
		routeStatement(*scope);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

int32  X3DVrmlParser::intValue() {
#line 1683 "expandedX3DVrmlParser.g"
	int32 val;
#line 324 "X3DVrmlParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  i0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  i1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case INTEGER:
	{
		i0 = LT(1);
		match(INTEGER);
#line 1687 "expandedX3DVrmlParser.g"
		
		std::istringstream(i0->getText()) >> val;
		
#line 337 "X3DVrmlParser.cpp"
		break;
	}
	case HEX_INTEGER:
	{
		i1 = LT(1);
		match(HEX_INTEGER);
#line 1690 "expandedX3DVrmlParser.g"
		
		std::istringstream(i1->getText()) >> std::hex >> val;
		
#line 348 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return val;
}

std::string  X3DVrmlParser::stringValue() {
#line 1892 "expandedX3DVrmlParser.g"
	std::string str;
#line 362 "X3DVrmlParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  s = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	s = LT(1);
	match(STRING);
#line 1897 "expandedX3DVrmlParser.g"
	
	using std::string;
	const string & token_text(s->getText());
	//
	// "Unescape" the string.
	//
	bool escape = false;
	for (string::const_iterator pos = token_text.begin() + 1;
	pos != token_text.end() - 1;
	++pos) {
	if (*pos == '\\' && !escape) {
	escape = true;
	} else {
	str.push_back(*pos);
	escape = false;
	}
	}
	
#line 386 "X3DVrmlParser.cpp"
	return str;
}

openvrml::field_value::type_id  X3DVrmlParser::fieldType() {
#line 218 "expandedX3DVrmlParser.g"
	openvrml::field_value::type_id ft =
         openvrml::field_value::invalid_type_id;
#line 394 "X3DVrmlParser.cpp"
#line 218 "expandedX3DVrmlParser.g"
	
	using openvrml::field_value;
	
#line 399 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case FIELDTYPE_MFBOOL:
	{
		match(FIELDTYPE_MFBOOL);
#line 226 "expandedX3DVrmlParser.g"
		ft = field_value::invalid_type_id;
#line 407 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFCOLOR:
	{
		match(FIELDTYPE_MFCOLOR);
#line 227 "expandedX3DVrmlParser.g"
		ft = field_value::mfcolor_id;
#line 415 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFCOLORRGBA:
	{
		match(FIELDTYPE_MFCOLORRGBA);
#line 228 "expandedX3DVrmlParser.g"
		ft = field_value::invalid_type_id;
#line 423 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFFLOAT:
	{
		match(FIELDTYPE_MFFLOAT);
#line 229 "expandedX3DVrmlParser.g"
		ft = field_value::mffloat_id;
#line 431 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFDOUBLE:
	{
		match(FIELDTYPE_MFDOUBLE);
#line 230 "expandedX3DVrmlParser.g"
		ft = field_value::mfdouble_id;
#line 439 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFIMAGE:
	{
		match(FIELDTYPE_MFIMAGE);
#line 231 "expandedX3DVrmlParser.g"
		ft = field_value::invalid_type_id;
#line 447 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFINT32:
	{
		match(FIELDTYPE_MFINT32);
#line 232 "expandedX3DVrmlParser.g"
		ft = field_value::mfint32_id;
#line 455 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFNODE:
	{
		match(FIELDTYPE_MFNODE);
#line 233 "expandedX3DVrmlParser.g"
		ft = field_value::mfnode_id;
#line 463 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFROTATION:
	{
		match(FIELDTYPE_MFROTATION);
#line 234 "expandedX3DVrmlParser.g"
		ft = field_value::mfrotation_id;
#line 471 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFSTRING:
	{
		match(FIELDTYPE_MFSTRING);
#line 235 "expandedX3DVrmlParser.g"
		ft = field_value::mfstring_id;
#line 479 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFTIME:
	{
		match(FIELDTYPE_MFTIME);
#line 236 "expandedX3DVrmlParser.g"
		ft = field_value::mftime_id;
#line 487 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFVEC2F:
	{
		match(FIELDTYPE_MFVEC2F);
#line 237 "expandedX3DVrmlParser.g"
		ft = field_value::mfvec2f_id;
#line 495 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFVEC2D:
	{
		match(FIELDTYPE_MFVEC2D);
#line 238 "expandedX3DVrmlParser.g"
		ft = field_value::mfvec2d_id;
#line 503 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFVEC3F:
	{
		match(FIELDTYPE_MFVEC3F);
#line 239 "expandedX3DVrmlParser.g"
		ft = field_value::mfvec3f_id;
#line 511 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_MFVEC3D:
	{
		match(FIELDTYPE_MFVEC3D);
#line 240 "expandedX3DVrmlParser.g"
		ft = field_value::mfvec3d_id;
#line 519 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFBOOL:
	{
		match(FIELDTYPE_SFBOOL);
#line 241 "expandedX3DVrmlParser.g"
		ft = field_value::sfbool_id;
#line 527 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFCOLOR:
	{
		match(FIELDTYPE_SFCOLOR);
#line 242 "expandedX3DVrmlParser.g"
		ft = field_value::sfcolor_id;
#line 535 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFCOLORRGBA:
	{
		match(FIELDTYPE_SFCOLORRGBA);
#line 243 "expandedX3DVrmlParser.g"
		ft = field_value::invalid_type_id;
#line 543 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFFLOAT:
	{
		match(FIELDTYPE_SFFLOAT);
#line 244 "expandedX3DVrmlParser.g"
		ft = field_value::sffloat_id;
#line 551 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFDOUBLE:
	{
		match(FIELDTYPE_SFDOUBLE);
#line 245 "expandedX3DVrmlParser.g"
		ft = field_value::sfdouble_id;
#line 559 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFIMAGE:
	{
		match(FIELDTYPE_SFIMAGE);
#line 246 "expandedX3DVrmlParser.g"
		ft = field_value::sfimage_id;
#line 567 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFINT32:
	{
		match(FIELDTYPE_SFINT32);
#line 247 "expandedX3DVrmlParser.g"
		ft = field_value::sfint32_id;
#line 575 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFNODE:
	{
		match(FIELDTYPE_SFNODE);
#line 248 "expandedX3DVrmlParser.g"
		ft = field_value::sfnode_id;
#line 583 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFROTATION:
	{
		match(FIELDTYPE_SFROTATION);
#line 249 "expandedX3DVrmlParser.g"
		ft = field_value::sfrotation_id;
#line 591 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFSTRING:
	{
		match(FIELDTYPE_SFSTRING);
#line 250 "expandedX3DVrmlParser.g"
		ft = field_value::sfstring_id;
#line 599 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFTIME:
	{
		match(FIELDTYPE_SFTIME);
#line 251 "expandedX3DVrmlParser.g"
		ft = field_value::sftime_id;
#line 607 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFVEC2F:
	{
		match(FIELDTYPE_SFVEC2F);
#line 252 "expandedX3DVrmlParser.g"
		ft = field_value::sfvec2f_id;
#line 615 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFVEC2D:
	{
		match(FIELDTYPE_SFVEC2D);
#line 253 "expandedX3DVrmlParser.g"
		ft = field_value::sfvec2d_id;
#line 623 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFVEC3F:
	{
		match(FIELDTYPE_SFVEC3F);
#line 254 "expandedX3DVrmlParser.g"
		ft = field_value::sfvec3f_id;
#line 631 "X3DVrmlParser.cpp"
		break;
	}
	case FIELDTYPE_SFVEC3D:
	{
		match(FIELDTYPE_SFVEC3D);
#line 255 "expandedX3DVrmlParser.g"
		ft = field_value::sfvec3d_id;
#line 639 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return ft;
}

boost::shared_ptr<field_value>  X3DVrmlParser::nonNodeFieldValue(
	openvrml::field_value::type_id ft
) {
#line 258 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> fv;
#line 655 "X3DVrmlParser.cpp"
	
	if (((LA(1) == KEYWORD_FALSE || LA(1) == KEYWORD_TRUE))&&( ft == field_value::sfbool_id )) {
		fv=sfBoolValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sfcolor_id )) {
		fv=sfColorValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sffloat_id )) {
		fv=sfFloatValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sfdouble_id )) {
		fv=sfDoubleValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == HEX_INTEGER))&&( ft == field_value::sfimage_id )) {
		fv=sfImageValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == HEX_INTEGER))&&( ft == field_value::sfint32_id )) {
		fv=sfInt32Value();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sfrotation_id )) {
		fv=sfRotationValue();
	}
	else if (((LA(1) == STRING))&&( ft == field_value::sfstring_id )) {
		fv=sfStringValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sftime_id )) {
		fv=sfTimeValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sfvec2f_id )) {
		fv=sfVec2fValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sfvec2d_id )) {
		fv=sfVec2dValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sfvec3f_id )) {
		fv=sfVec3fValue();
	}
	else if (((LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::sfvec3d_id )) {
		fv=sfVec3dValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mfcolor_id )) {
		fv=mfColorValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mffloat_id )) {
		fv=mfFloatValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mfdouble_id )) {
		fv=mfDoubleValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == HEX_INTEGER))&&( ft == field_value::mfint32_id )) {
		fv=mfInt32Value();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mfrotation_id )) {
		fv=mfRotationValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == STRING))&&( ft == field_value::mfstring_id )) {
		fv=mfStringValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mftime_id )) {
		fv=mfTimeValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mfvec2f_id )) {
		fv=mfVec2fValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mfvec2d_id )) {
		fv=mfVec2dValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mfvec3f_id )) {
		fv=mfVec3fValue();
	}
	else if (((LA(1) == LBRACKET || LA(1) == INTEGER || LA(1) == REAL))&&( ft == field_value::mfvec3d_id )) {
		fv=mfVec3dValue();
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfBoolValue() {
#line 1494 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> sbv;
#line 739 "X3DVrmlParser.cpp"
#line 1494 "expandedX3DVrmlParser.g"
	
	bool val(false);
	
#line 744 "X3DVrmlParser.cpp"
	
	val=boolValue();
#line 1501 "expandedX3DVrmlParser.g"
	sbv.reset(new sfbool(val));
#line 749 "X3DVrmlParser.cpp"
	return sbv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfColorValue() {
#line 1514 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> scv;
#line 756 "X3DVrmlParser.cpp"
#line 1514 "expandedX3DVrmlParser.g"
	
	color c;
	
#line 761 "X3DVrmlParser.cpp"
	
	colorValue(c);
#line 1521 "expandedX3DVrmlParser.g"
	scv.reset(new sfcolor(c));
#line 766 "X3DVrmlParser.cpp"
	return scv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfFloatValue() {
#line 1576 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> sfv;
#line 773 "X3DVrmlParser.cpp"
#line 1576 "expandedX3DVrmlParser.g"
	
	float f;
	
#line 778 "X3DVrmlParser.cpp"
	
	f=floatValue();
#line 1583 "expandedX3DVrmlParser.g"
	sfv.reset(new sffloat(f));
#line 783 "X3DVrmlParser.cpp"
	return sfv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfDoubleValue() {
#line 288 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> sfv;
#line 790 "X3DVrmlParser.cpp"
#line 288 "expandedX3DVrmlParser.g"
	
	double d;
	
#line 795 "X3DVrmlParser.cpp"
	
	d=doubleValue();
#line 295 "expandedX3DVrmlParser.g"
	sfv.reset(new sfdouble(d));
#line 800 "X3DVrmlParser.cpp"
	return sfv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfImageValue() {
#line 1616 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> siv;
#line 807 "X3DVrmlParser.cpp"
#line 1616 "expandedX3DVrmlParser.g"
	
	using antlr::SemanticException;
	size_t x, y, comp;
	int32 pixel;
	size_t pixel_index = 0;
	
#line 815 "X3DVrmlParser.cpp"
	
	x=intValue();
	y=intValue();
	comp=intValue();
#line 1626 "expandedX3DVrmlParser.g"
	
	image img(x, y, comp);
	
#line 824 "X3DVrmlParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == INTEGER || LA(1) == HEX_INTEGER)) {
			pixel=intValue();
#line 1629 "expandedX3DVrmlParser.g"
			
			if (!(pixel_index < x * y)) {
			throw SemanticException("Too many pixel values for "
			"SFImage.",
			this->uri,
			LT(1)->getLine(),
			LT(1)->getColumn());
			}
			img.pixel(pixel_index++, pixel);
			
#line 840 "X3DVrmlParser.cpp"
		}
		else {
			goto _loop94;
		}
		
	}
	_loop94:;
	} // ( ... )*
#line 1639 "expandedX3DVrmlParser.g"
	
	if (pixel_index != x * y) {
	throw antlr::SemanticException("Insufficient pixel values for "
	"SFImage.",
	this->uri,
	LT(1)->getLine(),
	LT(1)->getColumn());
	}
	siv.reset(new sfimage(img));
	
#line 860 "X3DVrmlParser.cpp"
	return siv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfInt32Value() {
#line 1652 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> siv;
#line 867 "X3DVrmlParser.cpp"
#line 1652 "expandedX3DVrmlParser.g"
	
	long i;
	
#line 872 "X3DVrmlParser.cpp"
	
	i=intValue();
#line 1659 "expandedX3DVrmlParser.g"
	siv.reset(new sfint32(i));
#line 877 "X3DVrmlParser.cpp"
	return siv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfRotationValue() {
#line 1807 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> srv;
#line 884 "X3DVrmlParser.cpp"
#line 1807 "expandedX3DVrmlParser.g"
	rotation r;
#line 887 "X3DVrmlParser.cpp"
	
	rotationValue(r);
#line 1812 "expandedX3DVrmlParser.g"
	srv.reset(new sfrotation(r));
#line 892 "X3DVrmlParser.cpp"
	return srv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfStringValue() {
#line 1862 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> ssv;
#line 899 "X3DVrmlParser.cpp"
#line 1862 "expandedX3DVrmlParser.g"
	std::string s;
#line 902 "X3DVrmlParser.cpp"
	
	s=stringValue();
#line 1867 "expandedX3DVrmlParser.g"
	ssv.reset(new sfstring(s));
#line 907 "X3DVrmlParser.cpp"
	return ssv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfTimeValue() {
#line 1918 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> stv;
#line 914 "X3DVrmlParser.cpp"
#line 1918 "expandedX3DVrmlParser.g"
	double t(0.0);
#line 917 "X3DVrmlParser.cpp"
	
	t=doubleValue();
#line 1923 "expandedX3DVrmlParser.g"
	stv.reset(new sftime(t));
#line 922 "X3DVrmlParser.cpp"
	return stv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfVec2fValue() {
#line 1956 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> svv;
#line 929 "X3DVrmlParser.cpp"
#line 1956 "expandedX3DVrmlParser.g"
	vec2f v;
#line 932 "X3DVrmlParser.cpp"
	
	vec2fValue(v);
#line 1961 "expandedX3DVrmlParser.g"
	svv.reset(new sfvec2f(v));
#line 937 "X3DVrmlParser.cpp"
	return svv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfVec2dValue() {
#line 317 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> svv;
#line 944 "X3DVrmlParser.cpp"
#line 317 "expandedX3DVrmlParser.g"
	
	vec2d v;
	
#line 949 "X3DVrmlParser.cpp"
	
	vec2dValue(v);
#line 324 "expandedX3DVrmlParser.g"
	svv.reset(new sfvec2d(v));
#line 954 "X3DVrmlParser.cpp"
	return svv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfVec3fValue() {
#line 1997 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> svv;
#line 961 "X3DVrmlParser.cpp"
#line 1997 "expandedX3DVrmlParser.g"
	
	vec3f v;
	
#line 966 "X3DVrmlParser.cpp"
	
	vec3fValue(v);
#line 2004 "expandedX3DVrmlParser.g"
	svv.reset(new sfvec3f(v));
#line 971 "X3DVrmlParser.cpp"
	return svv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfVec3dValue() {
#line 359 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> svv;
#line 978 "X3DVrmlParser.cpp"
#line 359 "expandedX3DVrmlParser.g"
	
	vec3d v;
	
#line 983 "X3DVrmlParser.cpp"
	
	vec3dValue(v);
#line 366 "expandedX3DVrmlParser.g"
	svv.reset(new sfvec3d(v));
#line 988 "X3DVrmlParser.cpp"
	return svv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfColorValue() {
#line 1525 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> mcv =
            boost::shared_ptr<field_value>(new mfcolor);
#line 996 "X3DVrmlParser.cpp"
#line 1525 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	color c;
	mfcolor & colors = static_cast<mfcolor &>(*mcv);
	
#line 1004 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		colorValue(c);
#line 1536 "expandedX3DVrmlParser.g"
		colors.value(vector<color>(1, c));
#line 1013 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1537 "expandedX3DVrmlParser.g"
		
		vector<color> value;
		
#line 1023 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				colorValue(c);
#line 1539 "expandedX3DVrmlParser.g"
				value.push_back(c);
#line 1030 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop84;
			}
			
		}
		_loop84:;
		} // ( ... )*
		match(RBRACKET);
#line 1539 "expandedX3DVrmlParser.g"
		
		colors.value(value);
		
#line 1044 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mcv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfFloatValue() {
#line 1587 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> mfv =
            boost::shared_ptr<field_value>(new mffloat);
#line 1059 "X3DVrmlParser.cpp"
#line 1587 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	float f;
	mffloat & floats = static_cast<mffloat &>(*mfv);
	
#line 1067 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		f=floatValue();
#line 1598 "expandedX3DVrmlParser.g"
		floats.value(vector<float>(1, f));
#line 1076 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1599 "expandedX3DVrmlParser.g"
		
		vector<float> value;
		
#line 1086 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				f=floatValue();
#line 1601 "expandedX3DVrmlParser.g"
				value.push_back(f);
#line 1093 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop90;
			}
			
		}
		_loop90:;
		} // ( ... )*
		match(RBRACKET);
#line 1601 "expandedX3DVrmlParser.g"
		
		floats.value(value);
		
#line 1107 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mfv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfDoubleValue() {
#line 298 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> mfv =
         boost::shared_ptr<field_value>(new mfdouble);
#line 1122 "X3DVrmlParser.cpp"
#line 298 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	double d;
	mfdouble & doubles = static_cast<mfdouble &>(*mfv);
	
#line 1130 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		d=doubleValue();
#line 309 "expandedX3DVrmlParser.g"
		doubles.value(vector<double>(1, d));
#line 1139 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 310 "expandedX3DVrmlParser.g"
		
		vector<double> value;
		
#line 1149 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				d=doubleValue();
#line 312 "expandedX3DVrmlParser.g"
				value.push_back(d);
#line 1156 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop16;
			}
			
		}
		_loop16:;
		} // ( ... )*
		match(RBRACKET);
#line 312 "expandedX3DVrmlParser.g"
		
		doubles.value(value);
		
#line 1170 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mfv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfInt32Value() {
#line 1663 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> miv =
            boost::shared_ptr<field_value>(new mfint32);
#line 1185 "X3DVrmlParser.cpp"
#line 1663 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	long i;
	mfint32 & int32s = static_cast<mfint32 &>(*miv);
	
#line 1193 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case HEX_INTEGER:
	{
		i=intValue();
#line 1674 "expandedX3DVrmlParser.g"
		int32s.value(vector<int32>(1, i));
#line 1202 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1675 "expandedX3DVrmlParser.g"
		
		vector<int32> value;
		
#line 1212 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == HEX_INTEGER)) {
				i=intValue();
#line 1677 "expandedX3DVrmlParser.g"
				value.push_back(i);
#line 1219 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop98;
			}
			
		}
		_loop98:;
		} // ( ... )*
		match(RBRACKET);
#line 1677 "expandedX3DVrmlParser.g"
		
		int32s.value(value);
		
#line 1233 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return miv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfRotationValue() {
#line 1816 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> mrv =
         boost::shared_ptr<field_value>(new mfrotation);
#line 1248 "X3DVrmlParser.cpp"
#line 1816 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	rotation r;
	mfrotation & rotations = static_cast<mfrotation &>(*mrv);
	
#line 1256 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		rotationValue(r);
#line 1827 "expandedX3DVrmlParser.g"
		rotations.value(vector<rotation>(1, r));
#line 1265 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1828 "expandedX3DVrmlParser.g"
		
		vector<rotation> value;
		
#line 1275 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				rotationValue(r);
#line 1830 "expandedX3DVrmlParser.g"
				value.push_back(r);
#line 1282 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop111;
			}
			
		}
		_loop111:;
		} // ( ... )*
		match(RBRACKET);
#line 1830 "expandedX3DVrmlParser.g"
		
		rotations.value(value);
		
#line 1296 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mrv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfStringValue() {
#line 1871 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> msv =
         boost::shared_ptr<field_value>(new mfstring);
#line 1311 "X3DVrmlParser.cpp"
#line 1871 "expandedX3DVrmlParser.g"
	
	using std::string;
	using std::vector;
	
	string s;
	mfstring & strings = static_cast<mfstring &>(*msv);
	
#line 1320 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case STRING:
	{
		s=stringValue();
#line 1883 "expandedX3DVrmlParser.g"
		strings.value(vector<string>(1, s));
#line 1328 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1884 "expandedX3DVrmlParser.g"
		
		vector<string> value;
		
#line 1338 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == STRING)) {
				s=stringValue();
#line 1886 "expandedX3DVrmlParser.g"
				value.push_back(s);
#line 1345 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop116;
			}
			
		}
		_loop116:;
		} // ( ... )*
		match(RBRACKET);
#line 1886 "expandedX3DVrmlParser.g"
		
		strings.value(value);
		
#line 1359 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return msv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfTimeValue() {
#line 1927 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> mtv =
         boost::shared_ptr<field_value>(new mftime);
#line 1374 "X3DVrmlParser.cpp"
#line 1927 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	double t;
	mftime & times = static_cast<mftime &>(*mtv);
	
#line 1382 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		t=doubleValue();
#line 1938 "expandedX3DVrmlParser.g"
		times.value(vector<double>(1, t));
#line 1391 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1939 "expandedX3DVrmlParser.g"
		
		vector<double> value;
		
#line 1401 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				t=doubleValue();
#line 1941 "expandedX3DVrmlParser.g"
				value.push_back(t);
#line 1408 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop121;
			}
			
		}
		_loop121:;
		} // ( ... )*
		match(RBRACKET);
#line 1941 "expandedX3DVrmlParser.g"
		
		times.value(value);
		
#line 1422 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mtv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfVec2fValue() {
#line 1965 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> mvv =
         boost::shared_ptr<field_value>(new mfvec2f);
#line 1437 "X3DVrmlParser.cpp"
#line 1965 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	vec2f v;
	mfvec2f & vec2fs = static_cast<mfvec2f &>(*mvv);
	
#line 1445 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec2fValue(v);
#line 1976 "expandedX3DVrmlParser.g"
		vec2fs.value(vector<vec2f>(1, v));
#line 1454 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1977 "expandedX3DVrmlParser.g"
		
		vector<vec2f> value;
		
#line 1464 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				vec2fValue(v);
#line 1979 "expandedX3DVrmlParser.g"
				value.push_back(v);
#line 1471 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop126;
			}
			
		}
		_loop126:;
		} // ( ... )*
		match(RBRACKET);
#line 1979 "expandedX3DVrmlParser.g"
		
		vec2fs.value(value);
		
#line 1485 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mvv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfVec2dValue() {
#line 327 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> mvv =
         boost::shared_ptr<field_value>(new mfvec2d);
#line 1500 "X3DVrmlParser.cpp"
#line 327 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	vec2d v;
	mfvec2d & vec2ds = static_cast<mfvec2d &>(*mvv);
	
#line 1508 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec2dValue(v);
#line 338 "expandedX3DVrmlParser.g"
		vec2ds.value(vector<vec2d>(1, v));
#line 1517 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 339 "expandedX3DVrmlParser.g"
		
		vector<vec2d> value;
		
#line 1527 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				vec2dValue(v);
#line 341 "expandedX3DVrmlParser.g"
				value.push_back(v);
#line 1534 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop20;
			}
			
		}
		_loop20:;
		} // ( ... )*
		match(RBRACKET);
#line 341 "expandedX3DVrmlParser.g"
		
		vec2ds.value(value);
		
#line 1548 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mvv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfVec3fValue() {
#line 2008 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> mvv =
         boost::shared_ptr<field_value>(new mfvec3f);
#line 1563 "X3DVrmlParser.cpp"
#line 2008 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	vec3f v;
	mfvec3f & vec3fs = static_cast<mfvec3f &>(*mvv);
	
#line 1571 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec3fValue(v);
#line 2019 "expandedX3DVrmlParser.g"
		vec3fs.value(vector<vec3f>(1, v));
#line 1580 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 2020 "expandedX3DVrmlParser.g"
		
		vector<vec3f> value;
		
#line 1590 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				vec3fValue(v);
#line 2022 "expandedX3DVrmlParser.g"
				value.push_back(v);
#line 1597 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop131;
			}
			
		}
		_loop131:;
		} // ( ... )*
		match(RBRACKET);
#line 2022 "expandedX3DVrmlParser.g"
		
		vec3fs.value(value);
		
#line 1611 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mvv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfVec3dValue() {
#line 369 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> mvv =
         boost::shared_ptr<field_value>(new mfvec3d);
#line 1626 "X3DVrmlParser.cpp"
#line 369 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	vec3d v;
	mfvec3d & vec3ds = static_cast<mfvec3d &>(*mvv);
	
#line 1634 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case INTEGER:
	case REAL:
	{
		vec3dValue(v);
#line 380 "expandedX3DVrmlParser.g"
		vec3ds.value(vector<vec3d>(1, v));
#line 1643 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 381 "expandedX3DVrmlParser.g"
		
		vector<vec3d> value;
		
#line 1653 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == INTEGER || LA(1) == REAL)) {
				vec3dValue(v);
#line 383 "expandedX3DVrmlParser.g"
				value.push_back(v);
#line 1660 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop25;
			}
			
		}
		_loop25:;
		} // ( ... )*
		match(RBRACKET);
#line 383 "expandedX3DVrmlParser.g"
		
		vec3ds.value(value);
		
#line 1674 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mvv;
}

double  X3DVrmlParser::doubleValue() {
#line 1947 "expandedX3DVrmlParser.g"
	double val = 0.0;
#line 1688 "X3DVrmlParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  d0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  d1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case REAL:
	{
		d0 = LT(1);
		match(REAL);
#line 1951 "expandedX3DVrmlParser.g"
		std::istringstream(d0->getText()) >> val;
#line 1699 "X3DVrmlParser.cpp"
		break;
	}
	case INTEGER:
	{
		d1 = LT(1);
		match(INTEGER);
#line 1952 "expandedX3DVrmlParser.g"
		std::istringstream(d1->getText()) >> val;
#line 1708 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return val;
}

void X3DVrmlParser::vec2dValue(
	vec2d & v
) {
#line 346 "expandedX3DVrmlParser.g"
	
	double x, y;
	
#line 1726 "X3DVrmlParser.cpp"
	
	x=doubleValue();
	y=doubleValue();
#line 353 "expandedX3DVrmlParser.g"
	
	v.x(x);
	v.y(y);
	
#line 1735 "X3DVrmlParser.cpp"
}

void X3DVrmlParser::vec3dValue(
	vec3d & v
) {
#line 388 "expandedX3DVrmlParser.g"
	
	double x, y, z;
	
#line 1745 "X3DVrmlParser.cpp"
	
	x=doubleValue();
	y=doubleValue();
	z=doubleValue();
#line 395 "expandedX3DVrmlParser.g"
	
	v.x(x);
	v.y(y);
	v.z(z);
	
#line 1756 "X3DVrmlParser.cpp"
}

boost::intrusive_ptr<openvrml::node>  X3DVrmlParser::nodeStatement(
	openvrml::browser & browser,
              const boost::shared_ptr<openvrml::scope> & scope,
              const std::string & script_node_id
) {
#line 424 "expandedX3DVrmlParser.g"
	boost::intrusive_ptr<openvrml::node> n;
#line 1766 "X3DVrmlParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case KEYWORD_DEF:
	{
		match(KEYWORD_DEF);
		id0 = LT(1);
		match(ID);
		n=node(browser, scope, id0->getText());
		break;
	}
	case KEYWORD_USE:
	{
		match(KEYWORD_USE);
		id1 = LT(1);
		match(ID);
#line 431 "expandedX3DVrmlParser.g"
		
		if (id1->getText() == script_node_id) {
		//
		// Script node self-reference.
		//
		n = openvrml::node::self_tag;
		} else {
		using antlr::SemanticException;
		assert(scope);
		n = boost::intrusive_ptr<openvrml::node>(
		scope->find_node(id1->getText()));
		if (!n) {
		throw SemanticException("Node \"" + id1->getText()
		+ "\" has not been defined in "
		+ "this scope.",
		this->uri,
		id1->getLine(),
		id1->getColumn());
		}
		}
		
#line 1806 "X3DVrmlParser.cpp"
		break;
	}
	case ID:
	{
		n=node(browser, scope, std::string());
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return n;
}

void X3DVrmlParser::protoStatement(
	openvrml::browser & browser, const boost::shared_ptr<openvrml::scope> & scope
) {
	
	switch ( LA(1)) {
	case KEYWORD_EXTERNPROTO:
	{
		externproto(browser, scope);
		break;
	}
	case KEYWORD_PROTO:
	{
		proto(browser, scope);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void X3DVrmlParser::routeStatement(
	const openvrml::scope & scope
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  from_node_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventout_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  to_node_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventin_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	match(KEYWORD_ROUTE);
	from_node_id = LT(1);
	match(ID);
	match(PERIOD);
	eventout_id = LT(1);
	match(ID);
	match(KEYWORD_TO);
	to_node_id = LT(1);
	match(ID);
	match(PERIOD);
	eventin_id = LT(1);
	match(ID);
#line 834 "expandedX3DVrmlParser.g"
	
	using std::runtime_error;
	using antlr::SemanticException;
	
	openvrml::node * const from_node =
	scope.find_node(from_node_id->getText());
	if (!from_node) {
	throw SemanticException("Node \"" + from_node_id->getText()
	+ "\" has not been defined in this "
	"scope.",
	this->uri,
	from_node_id->getLine(),
	from_node_id->getColumn());
	}
	
	openvrml::node * const to_node =
	scope.find_node(to_node_id->getText());
	if (!to_node) {
	throw SemanticException("Node \"" + to_node_id->getText()
	+ "\" has not been defined in this "
	"scope.",
	this->uri,
	to_node_id->getLine(),
	to_node_id->getColumn());
	}
	
	try {
	//
	// Redundant routing is ignored (4.10.2), so we don't care
	// if add_route returns false.
	//
	add_route(*from_node, eventout_id->getText(),
	*to_node, eventin_id->getText());
	} catch (runtime_error & ex) {
	throw SemanticException(ex.what(),
	this->uri,
	from_node_id->getLine(),
	from_node_id->getColumn());
	}
	
#line 1905 "X3DVrmlParser.cpp"
}

boost::intrusive_ptr<openvrml::node>  X3DVrmlParser::node(
	openvrml::browser & browser,
     const boost::shared_ptr<openvrml::scope> & scope,
     const std::string & node_id
) {
#line 963 "expandedX3DVrmlParser.g"
	boost::intrusive_ptr<openvrml::node> n;
#line 1915 "X3DVrmlParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 963 "expandedX3DVrmlParser.g"
	
	using antlr::SemanticException;
	using boost::intrusive_ptr;
	
	initial_value_map initial_values;
	node_interface_set interfaces;
	boost::shared_ptr<node_type> nodeType;
	
#line 1927 "X3DVrmlParser.cpp"
	
	try {      // for error handling
		if (((LA(1) == ID))&&( !LT(1)->getText().compare("Script") )) {
			scriptId = LT(1);
			match(ID);
			match(LBRACE);
			{ // ( ... )*
			for (;;) {
				switch ( LA(1)) {
				case ID:
				case KEYWORD_EXTERNPROTO:
				case KEYWORD_PROTO:
				case KEYWORD_ROUTE:
				{
					nodeBodyElement(browser,
                            scope,
                            script_node_interface_set,
                            initial_values);
					break;
				}
				case KEYWORD_EVENTIN:
				case KEYWORD_EVENTOUT:
				case KEYWORD_FIELD:
				{
					scriptInterfaceDeclaration(browser,
                                         scope,
                                         interfaces,
                                         initial_values,
                                         node_id);
					break;
				}
				default:
				{
					goto _loop55;
				}
				}
			}
			_loop55:;
			} // ( ... )*
			match(RBRACE);
#line 987 "expandedX3DVrmlParser.g"
			
			n = intrusive_ptr<openvrml::node>(
			new script_node(browser.script_node_class_,
			scope,
			interfaces,
			initial_values));
			if (!node_id.empty()) { n->id(node_id); }
			
#line 1977 "X3DVrmlParser.cpp"
		}
		else if ((LA(1) == ID)) {
			nodeTypeId = LT(1);
			match(ID);
#line 995 "expandedX3DVrmlParser.g"
			
			nodeType = scope->find_type(nodeTypeId->getText());
			if (!nodeType) {
			throw SemanticException("Unknown node type \""
			+ nodeTypeId->getText() + "\".",
			this->uri,
			nodeTypeId->getLine(),
			nodeTypeId->getColumn());
			}
			
			
#line 1994 "X3DVrmlParser.cpp"
			match(LBRACE);
			{ // ( ... )*
			for (;;) {
				if ((_tokenSet_3.member(LA(1)))) {
					nodeBodyElement(browser,
                                  scope,
                                  nodeType->interfaces(),
                                  initial_values);
				}
				else {
					goto _loop57;
				}
				
			}
			_loop57:;
			} // ( ... )*
			match(RBRACE);
#line 1009 "expandedX3DVrmlParser.g"
			
			n = boost::intrusive_ptr<openvrml::node>(nodeType->create_node(scope, initial_values));
			
			if (!node_id.empty()) { n->id(node_id); }
			
#line 2018 "X3DVrmlParser.cpp"
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
	}
	catch (std::invalid_argument & ex) {
#line 1016 "expandedX3DVrmlParser.g"
		
		throw SemanticException(ex.what(),
		this->uri,
		LT(1)->getLine(),
		LT(1)->getColumn());
		
#line 2033 "X3DVrmlParser.cpp"
	}
	catch (unsupported_interface & ex) {
#line 1022 "expandedX3DVrmlParser.g"
		
		throw SemanticException(ex.what(),
		this->uri,
		LT(1)->getLine(),
		LT(1)->getColumn());
		
#line 2043 "X3DVrmlParser.cpp"
	}
	catch (std::bad_cast & ex) {
#line 1028 "expandedX3DVrmlParser.g"
		
		throw SemanticException("Incorrect value type for field or "
		"exposedField.",
		this->uri,
		LT(1)->getLine(),
		LT(1)->getColumn());
		
#line 2054 "X3DVrmlParser.cpp"
	}
	return n;
}

void X3DVrmlParser::externproto(
	openvrml::browser & browser,
            const boost::shared_ptr<openvrml::scope> & scope
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 725 "expandedX3DVrmlParser.g"
	
	openvrml::node_interface_set interfaces;
	openvrml::mfstring url_list;
	boost::shared_ptr<node_type> node_type;
	
#line 2070 "X3DVrmlParser.cpp"
	
	match(KEYWORD_EXTERNPROTO);
	id = LT(1);
	match(ID);
	match(LBRACKET);
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_4.member(LA(1)))) {
			externInterfaceDeclaration(interfaces);
		}
		else {
			goto _loop45;
		}
		
	}
	_loop45:;
	} // ( ... )*
	match(RBRACKET);
	url_list=externprotoUrlList();
#line 737 "expandedX3DVrmlParser.g"
	
	for (size_t i = 0; i < url_list.value().size(); ++i) {
	boost::shared_ptr<openvrml::node_class> node_class =
	browser.node_class_map_.find(url_list.value()[i]);
	if (node_class) {
	node_type = node_class->create_type(id->getText(),
	interfaces);
	break;
	}
	}
	//
	// If we weren't able to create a node_type, that means that we
	// don't already have a node_class for the node. Currently we only
	// support referring to existing node_classes with EXTERNPROTO;
	// adding new node_classes via EXTERNPROTO is not supported. In
	// practice, this means that the ordinary way of using EXTERNPROTOs
	// in VRML worlds will fail.
	//
	if (node_type) {
	if (!scope->add_type(node_type)) {
	using antlr::SemanticException;
	throw SemanticException("Node type \"" + node_type->id()
	+ "\" has already been defined in "
	" this scope.",
	this->uri,
	id->getLine(),
	id->getColumn());
	}
	}
	
#line 2121 "X3DVrmlParser.cpp"
}

void X3DVrmlParser::proto(
	openvrml::browser & browser, const boost::shared_ptr<openvrml::scope> & scope
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 465 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	node_interface_set interfaces;
	proto_node_class::default_value_map_t default_value_map;
	vector<boost::intrusive_ptr<openvrml::node> > impl_nodes;
	proto_node_class::is_map_t is_map;
	proto_node_class::routes_t routes;
	
#line 2138 "X3DVrmlParser.cpp"
	
	match(KEYWORD_PROTO);
	id = LT(1);
	match(ID);
#line 478 "expandedX3DVrmlParser.g"
	
	boost::shared_ptr<openvrml::scope>
	proto_scope(new openvrml::scope(id->getText(), scope));
	
#line 2148 "X3DVrmlParser.cpp"
	match(LBRACKET);
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_4.member(LA(1)))) {
			protoInterfaceDeclaration(browser,
                                              scope,
                                              id->getText(),
                                              interfaces,
                                              default_value_map);
		}
		else {
			goto _loop32;
		}
		
	}
	_loop32:;
	} // ( ... )*
	match(RBRACKET);
	match(LBRACE);
	protoBody(browser,
                         proto_scope,
                         interfaces,
                         impl_nodes,
                         is_map,
                         routes);
	match(RBRACE);
#line 491 "expandedX3DVrmlParser.g"
	
	boost::shared_ptr<openvrml::node_class>
	node_class(new proto_node_class(browser,
	interfaces,
	default_value_map,
	impl_nodes,
	is_map,
	routes));
	//
	// Add the new node_class (prototype definition) to the browser's
	// node_class_map.
	//
	// First, construct the id for the node implementation.
	//
	std::string impl_id;
	do {
	impl_id = '#' + proto_scope->id() + impl_id;
	} while ((proto_scope = proto_scope->parent()));
	impl_id = scope->id() + impl_id;
	browser.node_class_map_.insert(impl_id, node_class);
	
	//
	// PROTOs implicitly introduce a new node type as well.
	//
	const boost::shared_ptr<node_type> node_type =
	node_class->create_type(id->getText(), interfaces);
	assert(node_type);
	assert(scope);
	if (!scope->add_type(node_type)) {
	using antlr::SemanticException;
	throw SemanticException("Node type \"" + node_type->id()
	+ "\" has already been defined in "
	"this scope.",
	this->uri,
	id->getLine(),
	id->getColumn());
	}
	
#line 2214 "X3DVrmlParser.cpp"
}

void X3DVrmlParser::protoInterfaceDeclaration(
	
    openvrml::browser & browser,
    const boost::shared_ptr<openvrml::scope> & outer_scope,
    const std::string & proto_id,
    node_interface_set & interfaces,
    proto_node_class::default_value_map_t & default_value_map
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 532 "expandedX3DVrmlParser.g"
	
	using antlr::SemanticException;
	
	node_interface::type_id it;
	field_value::type_id ft;
	boost::shared_ptr<field_value> fv;
	
#line 2235 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_EVENTIN:
	case KEYWORD_EVENTOUT:
	{
		it=eventInterfaceType();
		ft=fieldType();
		id0 = LT(1);
		match(ID);
#line 548 "expandedX3DVrmlParser.g"
		
		const node_interface interface(it, ft, id0->getText());
		const bool succeeded = interfaces.insert(interface).second;
		if (!succeeded) {
		using std::string;
		using boost::lexical_cast;
		throw SemanticException("Interface \""
		+ lexical_cast<string>(interface)
		+ "\" conflicts with previous "
		"declaration",
		this->uri,
		id0->getLine(),
		id0->getColumn());
		}
		
#line 2261 "X3DVrmlParser.cpp"
		break;
	}
	case KEYWORD_EXPOSEDFIELD:
	case KEYWORD_FIELD:
	{
		it=fieldInterfaceType();
		ft=fieldType();
		id1 = LT(1);
		match(ID);
#line 564 "expandedX3DVrmlParser.g"
		
		//
		// The field value declaration should have access to the node
		// types in the outer scope.
		//
		const boost::shared_ptr<openvrml::scope> field_decl_scope(
		new scope(proto_id + '.' + id1->getText(), outer_scope));
		
#line 2280 "X3DVrmlParser.cpp"
		fv=fieldValue(browser, field_decl_scope, ft, std::string());
#line 572 "expandedX3DVrmlParser.g"
		
		assert(fv);
		
		const node_interface interface(it, ft, id1->getText());
		bool succeeded = interfaces.insert(interface).second;
		if (!succeeded) {
		using std::string;
		using boost::lexical_cast;
		throw SemanticException("Interface \""
		+ lexical_cast<string>(interface)
		+ "\" conflicts with previous "
		"declaration",
		this->uri,
		id1->getLine(),
		id1->getColumn());
		}
		succeeded =
		default_value_map.insert(make_pair(id1->getText(), fv)).second;
		assert(succeeded);
		
#line 2303 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void X3DVrmlParser::protoBody(
	openvrml::browser & browser,
          const boost::shared_ptr<openvrml::scope> & scope,
          const node_interface_set & interfaces,
          std::vector<boost::intrusive_ptr<openvrml::node> > & impl_nodes,
          proto_node_class::is_map_t & is_map,
          proto_node_class::routes_t & routes
) {
#line 613 "expandedX3DVrmlParser.g"
	
	assert(scope);
	assert(impl_nodes.empty());
	assert(is_map.empty());
	assert(routes.empty());
	
	boost::intrusive_ptr<openvrml::node> n;
	
#line 2330 "X3DVrmlParser.cpp"
	
	{ // ( ... )*
	for (;;) {
		if ((LA(1) == KEYWORD_EXTERNPROTO || LA(1) == KEYWORD_PROTO)) {
			protoStatement(browser, scope);
		}
		else {
			goto _loop38;
		}
		
	}
	_loop38:;
	} // ( ... )*
	n=protoNodeStatement(browser,
                                 scope,
                                 interfaces,
                                 is_map,
                                 routes,
                                 std::string());
#line 637 "expandedX3DVrmlParser.g"
	
	assert(n);
	impl_nodes.push_back(n);
	
#line 2355 "X3DVrmlParser.cpp"
	{ // ( ... )*
	for (;;) {
		if ((_tokenSet_0.member(LA(1)))) {
			protoBodyStatement(browser,
                            scope,
                            interfaces,
                            impl_nodes,
                            is_map,
                            routes);
		}
		else {
			goto _loop40;
		}
		
	}
	_loop40:;
	} // ( ... )*
}

node_interface::type_id  X3DVrmlParser::eventInterfaceType() {
#line 595 "expandedX3DVrmlParser.g"
	node_interface::type_id it;
#line 2378 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_EVENTIN:
	{
		match(KEYWORD_EVENTIN);
#line 599 "expandedX3DVrmlParser.g"
		it = node_interface::eventin_id;
#line 2386 "X3DVrmlParser.cpp"
		break;
	}
	case KEYWORD_EVENTOUT:
	{
		match(KEYWORD_EVENTOUT);
#line 600 "expandedX3DVrmlParser.g"
		it = node_interface::eventout_id;
#line 2394 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return it;
}

node_interface::type_id  X3DVrmlParser::fieldInterfaceType() {
#line 604 "expandedX3DVrmlParser.g"
	node_interface::type_id it;
#line 2408 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_FIELD:
	{
		match(KEYWORD_FIELD);
#line 608 "expandedX3DVrmlParser.g"
		it = node_interface::field_id;
#line 2416 "X3DVrmlParser.cpp"
		break;
	}
	case KEYWORD_EXPOSEDFIELD:
	{
		match(KEYWORD_EXPOSEDFIELD);
#line 609 "expandedX3DVrmlParser.g"
		it = node_interface::exposedfield_id;
#line 2424 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return it;
}

boost::shared_ptr<field_value>  X3DVrmlParser::fieldValue(
	openvrml::browser & browser,
           const boost::shared_ptr<openvrml::scope> & scope,
           const openvrml::field_value::type_id ft,
           const std::string & node_id
) {
#line 1416 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> fv;
#line 2443 "X3DVrmlParser.cpp"
	
	if (((_tokenSet_5.member(LA(1))))&&( (ft == field_value::sfnode_id) || (ft == field_value::mfnode_id) )) {
		fv=nodeFieldValue(browser, scope, ft, node_id);
	}
	else if ((_tokenSet_6.member(LA(1)))) {
		fv=nonNodeFieldValue(ft);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

boost::intrusive_ptr<openvrml::node>  X3DVrmlParser::protoNodeStatement(
	openvrml::browser & browser,
                   const boost::shared_ptr<openvrml::scope> & scope,
                   const node_interface_set & proto_interfaces,
                   proto_node_class::is_map_t & is_map,
                   proto_node_class::routes_t & routes,
                   const std::string & script_node_id
) {
#line 679 "expandedX3DVrmlParser.g"
	boost::intrusive_ptr<openvrml::node> n;
#line 2468 "X3DVrmlParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  id0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  id1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 679 "expandedX3DVrmlParser.g"
	
	using antlr::SemanticException;
	
#line 2475 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_DEF:
	{
		match(KEYWORD_DEF);
		id0 = LT(1);
		match(ID);
		n=protoNode(browser,
                                       scope,
                                       proto_interfaces,
                                       is_map,
                                       routes,
                                       id0->getText());
		break;
	}
	case KEYWORD_USE:
	{
		match(KEYWORD_USE);
		id1 = LT(1);
		match(ID);
#line 697 "expandedX3DVrmlParser.g"
		
		if (id1->getText() == script_node_id) {
		//
		// Script node self-reference.
		//
		n = node::self_tag;
		} else {
		n = boost::intrusive_ptr<openvrml::node>(
		scope->find_node(id1->getText()));
		if (!n) {
		throw SemanticException("Node \"" + id1->getText()
		+ "\" has not been defined in "
		+ "this scope.",
		this->uri,
		id1->getLine(),
		id1->getColumn());
		}
		}
		
#line 2516 "X3DVrmlParser.cpp"
		break;
	}
	case ID:
	{
		n=protoNode(browser,
                    scope,
                    proto_interfaces,
                    is_map,
                    routes,
                    std::string());
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return n;
}

void X3DVrmlParser::protoBodyStatement(
	openvrml::browser & browser,
                   const boost::shared_ptr<openvrml::scope> & scope,
                   const node_interface_set & interfaces,
                   std::vector<boost::intrusive_ptr<openvrml::node> > & impl_nodes,
                   proto_node_class::is_map_t & is_map,
                   proto_node_class::routes_t & routes
) {
#line 650 "expandedX3DVrmlParser.g"
	
	assert(scope);
	assert(!impl_nodes.empty());
	
	boost::intrusive_ptr<openvrml::node> n;
	
#line 2552 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case ID:
	case KEYWORD_DEF:
	case KEYWORD_USE:
	{
		n=protoNodeStatement(browser,
                             scope,
                             interfaces,
                             is_map,
                             routes,
                             std::string());
#line 670 "expandedX3DVrmlParser.g"
		
		assert(n);
		impl_nodes.push_back(n);
		
#line 2570 "X3DVrmlParser.cpp"
		break;
	}
	case KEYWORD_EXTERNPROTO:
	case KEYWORD_PROTO:
	{
		protoStatement(browser, scope);
		break;
	}
	case KEYWORD_ROUTE:
	{
		protoRouteStatement(*scope, routes);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void X3DVrmlParser::protoRouteStatement(
	const openvrml::scope & scope,
                    proto_node_class::routes_t & routes
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  from_node_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventout_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  to_node_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  eventin_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	match(KEYWORD_ROUTE);
	from_node_id = LT(1);
	match(ID);
	match(PERIOD);
	eventout_id = LT(1);
	match(ID);
	match(KEYWORD_TO);
	to_node_id = LT(1);
	match(ID);
	match(PERIOD);
	eventin_id = LT(1);
	match(ID);
#line 884 "expandedX3DVrmlParser.g"
	
	using std::runtime_error;
	using antlr::SemanticException;
	
	openvrml::node * const from_node =
	scope.find_node(from_node_id->getText());
	if (!from_node) {
	throw SemanticException("Node \"" + from_node_id->getText()
	+ "\" has not been defined in this "
	"scope.",
	this->uri,
	from_node_id->getLine(),
	from_node_id->getColumn());
	}
	
	openvrml::node * const to_node =
	scope.find_node(to_node_id->getText());
	if (!to_node) {
	throw SemanticException("Node \"" + to_node_id->getText()
	+ "\" has not been defined in this "
	"scope.",
	this->uri,
	to_node_id->getLine(),
	to_node_id->getColumn());
	}
	
	//
	// Routes outside PROTO definitions get added with add_route, which
	// does the appropriate validation.  However, in a PROTO
	// definition, we don't actually establish a route; the actual
	// routes (event_listeners) are established when the PROTO is
	// instantiated.  So, we need to do validation here.
	//
	const node_interface_set & from_node_interfaces =
	from_node->type().interfaces();
	const node_interface_set::const_iterator from_interface =
	find_if(from_node_interfaces.begin(),
	from_node_interfaces.end(),
	bind2nd(node_interface_matches_eventout(),
	eventout_id->getText()));
	if (from_interface == from_node_interfaces.end()) {
	throw SemanticException(from_node->type().id() + " node has "
	"no eventOut \""
	+ eventout_id->getText() + "\".",
	this->uri,
	eventout_id->getLine(),
	eventout_id->getColumn());
	}
	
	const node_interface_set & to_node_interfaces =
	to_node->type().interfaces();
	const node_interface_set::const_iterator to_interface =
	find_if(to_node_interfaces.begin(), to_node_interfaces.end(),
	bind2nd(node_interface_matches_eventin(),
	eventin_id->getText()));
	if (to_interface == to_node_interfaces.end()) {
	throw SemanticException(to_node->type().id() + " node has no "
	"eventIn \"" + eventin_id->getText()
	+ "\".",
	this->uri,
	eventin_id->getLine(),
	eventin_id->getColumn());
	}
	
	if (to_interface->field_type != from_interface->field_type) {
	throw SemanticException("Mismatch between eventOut and "
	"eventIn types.",
	this->uri,
	eventin_id->getLine(),
	eventin_id->getColumn());
	}
	
	const proto_node_class::route r(*from_node, eventout_id->getText(),
	*to_node, eventin_id->getText());
	routes.push_back(r);
	
#line 2689 "X3DVrmlParser.cpp"
}

boost::intrusive_ptr<openvrml::node>  X3DVrmlParser::protoNode(
	openvrml::browser & browser,
          const boost::shared_ptr<openvrml::scope> & scope,
          const node_interface_set & proto_interfaces,
          proto_node_class::is_map_t & is_map,
          proto_node_class::routes_t & routes,
          const std::string & node_id
) {
#line 1164 "expandedX3DVrmlParser.g"
	boost::intrusive_ptr<openvrml::node> n;
#line 2702 "X3DVrmlParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  scriptId = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  nodeTypeId = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1164 "expandedX3DVrmlParser.g"
	
	using antlr::SemanticException;
	
	initial_value_map initial_values;
	node_interface_set interfaces;
	is_list is_mappings;
	boost::shared_ptr<node_type> nodeType;
	
#line 2714 "X3DVrmlParser.cpp"
	
	{
	if (((LA(1) == ID))&&( !LT(1)->getText().compare("Script") )) {
		scriptId = LT(1);
		match(ID);
		match(LBRACE);
		{ // ( ... )*
		for (;;) {
			switch ( LA(1)) {
			case ID:
			case KEYWORD_EXTERNPROTO:
			case KEYWORD_PROTO:
			case KEYWORD_ROUTE:
			{
				protoNodeBodyElement(browser,
                                     scope,
                                     proto_interfaces,
                                     is_map,
                                     routes,
                                     script_node_interface_set,
                                     initial_values,
                                     is_mappings);
				break;
			}
			case KEYWORD_EVENTIN:
			case KEYWORD_EVENTOUT:
			case KEYWORD_FIELD:
			{
				protoScriptInterfaceDeclaration(browser,
                                                  scope,
                                                  proto_interfaces,
                                                  is_map,
                                                  routes,
                                                  node_id,
                                                  interfaces,
                                                  initial_values,
                                                  is_mappings);
				break;
			}
			default:
			{
				goto _loop64;
			}
			}
		}
		_loop64:;
		} // ( ... )*
		match(RBRACE);
#line 1200 "expandedX3DVrmlParser.g"
		
		n = boost::intrusive_ptr<openvrml::node>(
		new script_node(browser.script_node_class_,
		scope,
		interfaces,
		initial_values));
		if (!node_id.empty()) { n->id(node_id); }
		
#line 2772 "X3DVrmlParser.cpp"
	}
	else if ((LA(1) == ID)) {
		nodeTypeId = LT(1);
		match(ID);
#line 1209 "expandedX3DVrmlParser.g"
		
		nodeType = scope->find_type(nodeTypeId->getText());
		if (!nodeType) {
		throw SemanticException("Unknown node type \""
		+ nodeTypeId->getText() + "\".",
		this->uri,
		nodeTypeId->getLine(),
		nodeTypeId->getColumn());
		}
		
#line 2788 "X3DVrmlParser.cpp"
		match(LBRACE);
		{ // ( ... )*
		for (;;) {
			if ((_tokenSet_3.member(LA(1)))) {
				protoNodeBodyElement(browser,
                                         scope,
                                         proto_interfaces,
                                         is_map,
                                         routes,
                                         nodeType->interfaces(),
                                         initial_values,
                                         is_mappings);
			}
			else {
				goto _loop66;
			}
			
		}
		_loop66:;
		} // ( ... )*
		match(RBRACE);
#line 1226 "expandedX3DVrmlParser.g"
		
		n = nodeType->create_node(scope, initial_values);
		if (!node_id.empty()) { n->id(node_id); }
		
#line 2815 "X3DVrmlParser.cpp"
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	}
#line 1230 "expandedX3DVrmlParser.g"
	
	for (is_list::const_iterator is_mapping = is_mappings.begin();
	is_mapping != is_mappings.end();
	++is_mapping) {
	typedef proto_node_class::is_target is_target;
	is_map.insert(make_pair(is_mapping->second,
	is_target(*n, is_mapping->first)));
	}
	
#line 2832 "X3DVrmlParser.cpp"
	return n;
}

void X3DVrmlParser::externInterfaceDeclaration(
	openvrml::node_interface_set & interfaces
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 770 "expandedX3DVrmlParser.g"
	
	using openvrml::node_interface;
	using openvrml::field_value;
	using antlr::SemanticException;
	node_interface::type_id it(node_interface::invalid_type_id);
	field_value::type_id ft(field_value::invalid_type_id);
	
#line 2848 "X3DVrmlParser.cpp"
	
	it=interfaceType();
	ft=fieldType();
	id = LT(1);
	match(ID);
#line 781 "expandedX3DVrmlParser.g"
	
	const node_interface interface(it, ft, id->getText());
	bool succeeded = interfaces.insert(interface).second;
	if (!succeeded) {
	using std::string;
	using boost::lexical_cast;
	throw SemanticException("Interface \""
	+ lexical_cast<string>(interface)
	+ "\" conflicts with previous "
	"declaration",
	this->uri,
	id->getLine(),
	id->getColumn());
	}
	
#line 2870 "X3DVrmlParser.cpp"
}

mfstring  X3DVrmlParser::externprotoUrlList() {
#line 808 "expandedX3DVrmlParser.g"
	mfstring urlList;
#line 2876 "X3DVrmlParser.cpp"
#line 808 "expandedX3DVrmlParser.g"
	
	using std::string;
	using std::vector;
	using openvrml::mfstring;
	
	string s;
	
#line 2885 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case STRING:
	{
		s=stringValue();
#line 819 "expandedX3DVrmlParser.g"
		urlList.value(vector<string>(1, s));
#line 2893 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 820 "expandedX3DVrmlParser.g"
		
		vector<string> value;
		
#line 2903 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == STRING)) {
				s=stringValue();
#line 822 "expandedX3DVrmlParser.g"
				value.push_back(s);
#line 2910 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop50;
			}
			
		}
		_loop50:;
		} // ( ... )*
		match(RBRACKET);
#line 822 "expandedX3DVrmlParser.g"
		
		urlList.value(value);
		
#line 2924 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return urlList;
}

node_interface::type_id  X3DVrmlParser::interfaceType() {
#line 799 "expandedX3DVrmlParser.g"
	node_interface::type_id interface_type;
#line 2938 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_EVENTIN:
	case KEYWORD_EVENTOUT:
	{
		interface_type=eventInterfaceType();
		break;
	}
	case KEYWORD_EXPOSEDFIELD:
	case KEYWORD_FIELD:
	{
		interface_type=fieldInterfaceType();
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return interface_type;
}

void X3DVrmlParser::nodeBodyElement(
	browser & b,
                const boost::shared_ptr<openvrml::scope> & scope,
                const node_interface_set & interfaces,
                initial_value_map & initial_values
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1037 "expandedX3DVrmlParser.g"
	
	using std::find_if;
	using std::bind2nd;
	using antlr::SemanticException;
	field_value::type_id ft = field_value::invalid_type_id;
	boost::shared_ptr<field_value> fv;
	
#line 2976 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case ID:
	{
		id = LT(1);
		match(ID);
#line 1051 "expandedX3DVrmlParser.g"
		
		node_interface_set::const_iterator interface =
		find_if(interfaces.begin(), interfaces.end(),
		bind2nd(node_interface_matches_field(),
		id->getText()));
		if (interface == interfaces.end()) {
		interface =
		find_if(interfaces.begin(), interfaces.end(),
		bind2nd(node_interface_matches_exposedfield(),
		id->getText()));
		}
		if (interface == interfaces.end()) {
		throw SemanticException("Node has no field or exposedField \""
		+ id->getText() + "\"",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		ft = interface->field_type;
		
#line 3004 "X3DVrmlParser.cpp"
		fv=fieldValue(b, scope, ft, std::string());
#line 1070 "expandedX3DVrmlParser.g"
		
		assert(fv);
		const bool succeeded =
		initial_values.insert(make_pair(id->getText(), fv)).second;
		if (!succeeded) {
		throw SemanticException("Value for " + id->getText()
		+ " already declared.",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
#line 3019 "X3DVrmlParser.cpp"
		break;
	}
	case KEYWORD_ROUTE:
	{
		routeStatement(*scope);
		break;
	}
	case KEYWORD_EXTERNPROTO:
	case KEYWORD_PROTO:
	{
		protoStatement(b, scope);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void X3DVrmlParser::scriptInterfaceDeclaration(
	browser & b,
                           const boost::shared_ptr<openvrml::scope> & scope,
                           node_interface_set & interfaces,
                           initial_value_map & initial_values,
                           const std::string & node_id
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1087 "expandedX3DVrmlParser.g"
	
	using antlr::SemanticException;
	node_interface::type_id it(node_interface::invalid_type_id);
	field_value::type_id ft(field_value::invalid_type_id);
	
#line 3054 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_EVENTIN:
	case KEYWORD_EVENTOUT:
	{
		it=eventInterfaceType();
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 1100 "expandedX3DVrmlParser.g"
		
		const node_interface interface(it, ft, id->getText());
		const bool succeeded = interfaces.insert(interface).second;
		if (!succeeded) {
		using std::string;
		using boost::lexical_cast;
		throw SemanticException("Interface \""
		+ lexical_cast<string>(interface)
		+ "\" conflicts with previous "
		"declaration",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
#line 3080 "X3DVrmlParser.cpp"
		break;
	}
	case KEYWORD_FIELD:
	{
		scriptFieldInterfaceDeclaration(b,
                                      scope,
                                      interfaces,
                                      initial_values,
                                      node_id);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void X3DVrmlParser::scriptFieldInterfaceDeclaration(
	
    browser & b,
    const boost::shared_ptr<openvrml::scope> & scope,
    node_interface_set & interfaces,
    initial_value_map & initial_values,
    const std::string & script_node_id
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1123 "expandedX3DVrmlParser.g"
	
	using std::find_if;
	using antlr::SemanticException;
	
	field_value::type_id ft = field_value::invalid_type_id;
	boost::shared_ptr<field_value> fv;
	
#line 3116 "X3DVrmlParser.cpp"
	
	match(KEYWORD_FIELD);
	ft=fieldType();
	id = LT(1);
	match(ID);
	fv=fieldValue(b,
                                                       scope,
                                                       ft,
                                                       script_node_id);
#line 1142 "expandedX3DVrmlParser.g"
	
	assert(fv);
	bool succeeded =
	interfaces.insert(node_interface(node_interface::field_id,
	ft,
	id->getText()))
	.second;
	if (!succeeded) {
	throw SemanticException("Interface \"" + id->getText()
	+ "\" already declared for Script "
	"node.",
	this->uri,
	id->getLine(),
	id->getColumn());
	}
	succeeded = initial_values.insert(make_pair(id->getText(), fv))
	.second;
	assert(succeeded);
	
#line 3146 "X3DVrmlParser.cpp"
}

void X3DVrmlParser::protoNodeBodyElement(
	openvrml::browser & browser,
                     const boost::shared_ptr<openvrml::scope> & scope,
                     const node_interface_set & proto_interfaces,
                     proto_node_class::is_map_t & is_map,
                     proto_node_class::routes_t & routes,
                     const node_interface_set & node_interfaces,
                     initial_value_map & initial_values,
                     is_list & is_mappings
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  interface_id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1242 "expandedX3DVrmlParser.g"
	
	using std::string;
	using antlr::SemanticException;
	
	boost::shared_ptr<field_value> fv;
	
#line 3167 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case ID:
	{
		interface_id = LT(1);
		match(ID);
#line 1259 "expandedX3DVrmlParser.g"
		
		const node_interface_set::const_iterator impl_node_interface =
		find_interface(node_interfaces, interface_id->getText());
		if (impl_node_interface == node_interfaces.end()) {
		throw SemanticException("Node has no interface \""
		+ interface_id->getText() + "\".",
		this->uri,
		interface_id->getLine(),
		interface_id->getColumn());
		}
		
#line 3186 "X3DVrmlParser.cpp"
		{
		if (((_tokenSet_7.member(LA(1))))&&(impl_node_interface->type == node_interface::field_id
            || impl_node_interface->type == node_interface::exposedfield_id)) {
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
				fv=protoFieldValue(browser,
                                   scope,
                                   proto_interfaces,
                                   is_map,
                                   routes,
                                   impl_node_interface->field_type,
                                   std::string());
#line 1278 "expandedX3DVrmlParser.g"
				
				assert(fv);
				bool succeeded =
				initial_values.insert(
				make_pair(interface_id->getText(), fv)).second;
				assert(succeeded);
				
#line 3219 "X3DVrmlParser.cpp"
				break;
			}
			case KEYWORD_IS:
			{
				isStatement(impl_node_interface->id, is_mappings);
				break;
			}
			default:
			{
				throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
			}
			}
			}
		}
		else if ((LA(1) == KEYWORD_IS)) {
			isStatement(impl_node_interface->id, is_mappings);
		}
		else {
			throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
		}
		
		}
		break;
	}
	case KEYWORD_ROUTE:
	{
		protoRouteStatement(*scope, routes);
		break;
	}
	case KEYWORD_EXTERNPROTO:
	case KEYWORD_PROTO:
	{
		protoStatement(browser, scope);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

void X3DVrmlParser::protoScriptInterfaceDeclaration(
	openvrml::browser & browser,
                                const boost::shared_ptr<openvrml::scope> & scope,
                                const node_interface_set & proto_interfaces,
                                proto_node_class::is_map_t & is_map,
                                proto_node_class::routes_t & routes,
                                const std::string & script_node_id,
                                node_interface_set & interfaces,
                                initial_value_map & initial_values,
                                is_list & is_mappings
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1306 "expandedX3DVrmlParser.g"
	
	using antlr::SemanticException;
	node_interface::type_id it;
	field_value::type_id ft;
	
#line 3280 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_EVENTIN:
	case KEYWORD_EVENTOUT:
	{
		it=eventInterfaceType();
		ft=fieldType();
		id = LT(1);
		match(ID);
#line 1323 "expandedX3DVrmlParser.g"
		
		const node_interface interface(it, ft, id->getText());
		const bool succeeded = interfaces.insert(interface).second;
		if (!succeeded) {
		using std::string;
		using boost::lexical_cast;
		throw SemanticException("Interface  \""
		+ lexical_cast<string>(interface)
		+ "\" conflicts with previous "
		"declaration",
		this->uri,
		id->getLine(),
		id->getColumn());
		}
		
#line 3306 "X3DVrmlParser.cpp"
		{
		switch ( LA(1)) {
		case KEYWORD_IS:
		{
			isStatement(id->getText(), is_mappings);
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
		protoScriptFieldInterfaceDeclaration(browser,
                                             scope,
                                             proto_interfaces,
                                             is_map,
                                             routes,
                                             interfaces,
                                             initial_values,
                                             script_node_id,
                                             is_mappings);
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
}

boost::shared_ptr<field_value>  X3DVrmlParser::protoFieldValue(
	openvrml::browser & browser,
                const boost::shared_ptr<openvrml::scope> & scope,
                const node_interface_set & proto_interfaces,
                proto_node_class::is_map_t & is_map,
                proto_node_class::routes_t & routes,
                const field_value::type_id ft,
                const std::string & script_node_id
) {
#line 1429 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> fv;
#line 3364 "X3DVrmlParser.cpp"
	
	if (((_tokenSet_5.member(LA(1))))&&( (ft == field_value::sfnode_id) || (ft == field_value::mfnode_id) )) {
		fv=protoNodeFieldValue(browser,
                               scope,
                               proto_interfaces,
                               is_map,
                               routes,
                               ft,
                               script_node_id);
#line 1446 "expandedX3DVrmlParser.g"
		
		assert(fv);
		
#line 3378 "X3DVrmlParser.cpp"
	}
	else if ((_tokenSet_6.member(LA(1)))) {
		fv=nonNodeFieldValue(ft);
#line 1449 "expandedX3DVrmlParser.g"
		assert(fv);
#line 3384 "X3DVrmlParser.cpp"
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

void X3DVrmlParser::isStatement(
	const std::string & impl_node_interface_id,
            is_list & is_mappings
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	match(KEYWORD_IS);
	id = LT(1);
	match(ID);
#line 1299 "expandedX3DVrmlParser.g"
	
	is_mappings.insert(make_pair(impl_node_interface_id,
	id->getText()));
	
#line 3407 "X3DVrmlParser.cpp"
}

void X3DVrmlParser::protoScriptFieldInterfaceDeclaration(
	
    openvrml::browser & browser,
    const boost::shared_ptr<openvrml::scope> & scope,
    const node_interface_set & proto_interfaces,
    proto_node_class::is_map_t & is_map,
    proto_node_class::routes_t & routes,
    node_interface_set & interfaces,
    initial_value_map & initial_values,
    const std::string & script_node_id,
    is_list & is_mappings
) {
	ANTLR_USE_NAMESPACE(antlr)RefToken  id = ANTLR_USE_NAMESPACE(antlr)nullToken;
#line 1350 "expandedX3DVrmlParser.g"
	
	field_value::type_id ft;
	boost::shared_ptr<field_value> fv;
	bool succeeded;
	
#line 3429 "X3DVrmlParser.cpp"
	
	match(KEYWORD_FIELD);
	ft=fieldType();
	id = LT(1);
	match(ID);
#line 1368 "expandedX3DVrmlParser.g"
	
	succeeded =
	interfaces.insert(node_interface(node_interface::field_id,
	ft,
	id->getText()))
	.second;
	if (!succeeded) {
	using antlr::SemanticException;
	throw SemanticException("Interface \"" + id->getText()
	+ "\" already declared for Script "
	"node.",
	this->uri,
	id->getLine(),
	id->getColumn());
	}
	
#line 3452 "X3DVrmlParser.cpp"
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
		fv=protoFieldValue(browser,
                               scope,
                               proto_interfaces,
                               is_map,
                               routes,
                               ft,
                               script_node_id);
#line 1390 "expandedX3DVrmlParser.g"
		
		assert(fv);
		succeeded = initial_values.insert(make_pair(id->getText(), fv))
		.second;
		assert(succeeded);
		
#line 3481 "X3DVrmlParser.cpp"
		break;
	}
	case KEYWORD_IS:
	{
		isStatement(id->getText(), is_mappings);
#line 1396 "expandedX3DVrmlParser.g"
		
		using std::auto_ptr;
		using boost::shared_ptr;
		
		//
		// The field needs some default value as a placeholder. This
		// is never actually used in a PROTO instance.
		//
		auto_ptr<field_value> value = field_value::create(ft);
		succeeded =
		initial_values.insert(
		make_pair(id->getText(),
		shared_ptr<field_value>(value)))
		.second;
		assert(succeeded);
		
#line 3504 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	}
}

boost::shared_ptr<field_value>  X3DVrmlParser::nodeFieldValue(
	openvrml::browser & browser,
               const boost::shared_ptr<openvrml::scope> & scope,
               openvrml::field_value::type_id ft,
               const std::string & script_node_id
) {
#line 1453 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> fv;
#line 3523 "X3DVrmlParser.cpp"
#line 1453 "expandedX3DVrmlParser.g"
	
	using openvrml::field_value;
	
#line 3528 "X3DVrmlParser.cpp"
	
	if (((_tokenSet_8.member(LA(1))))&&( ft == field_value::sfnode_id )) {
		fv=sfNodeValue(browser, scope, script_node_id);
	}
	else if ((_tokenSet_9.member(LA(1)))) {
		fv=mfNodeValue(browser, scope, script_node_id);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::protoNodeFieldValue(
	openvrml::browser & browser,
                    const boost::shared_ptr<openvrml::scope> & scope,
                    const node_interface_set & proto_interfaces,
                    proto_node_class::is_map_t & is_map,
                    proto_node_class::routes_t & routes,
                    field_value::type_id ft,
                    const std::string & script_node_id
) {
#line 1469 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> fv;
#line 3554 "X3DVrmlParser.cpp"
	
	if (((_tokenSet_8.member(LA(1))))&&( ft == field_value::sfnode_id )) {
		fv=protoSfNodeValue(browser,
                                scope,
                                proto_interfaces,
                                is_map, routes,
                                script_node_id);
	}
	else if ((_tokenSet_9.member(LA(1)))) {
		fv=protoMfNodeValue(browser,
                            scope,
                            proto_interfaces,
                            is_map,
                            routes,
                            script_node_id);
	}
	else {
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	
	return fv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::sfNodeValue(
	openvrml::browser & browser,
            const boost::shared_ptr<openvrml::scope> & scope,
            const std::string & script_node_id
) {
#line 1696 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> snv;
#line 3585 "X3DVrmlParser.cpp"
#line 1696 "expandedX3DVrmlParser.g"
	
	boost::intrusive_ptr<openvrml::node> n;
	
#line 3590 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case ID:
	case KEYWORD_DEF:
	case KEYWORD_USE:
	{
		n=nodeStatement(browser, scope, script_node_id);
#line 1705 "expandedX3DVrmlParser.g"
		
		snv.reset(new sfnode(n));
		
#line 3602 "X3DVrmlParser.cpp"
		break;
	}
	case KEYWORD_NULL:
	{
		match(KEYWORD_NULL);
#line 1708 "expandedX3DVrmlParser.g"
		snv.reset(new sfnode);
#line 3610 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return snv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::mfNodeValue(
	openvrml::browser & browser,
            const boost::shared_ptr<openvrml::scope> & scope,
            const std::string & script_node_id
) {
#line 1738 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> mnv =
         boost::shared_ptr<field_value>(new mfnode);
#line 3629 "X3DVrmlParser.cpp"
#line 1738 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	boost::intrusive_ptr<openvrml::node> n;
	mfnode & nodes = static_cast<mfnode &>(*mnv);
	
#line 3637 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case ID:
	case KEYWORD_DEF:
	case KEYWORD_USE:
	{
		n=nodeStatement(browser, scope, script_node_id);
#line 1751 "expandedX3DVrmlParser.g"
		
		if (n) { nodes.value(mfnode::value_type(1, n)); }
		
#line 3649 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1754 "expandedX3DVrmlParser.g"
		
		mfnode::value_type value;
		
#line 3659 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == ID || LA(1) == KEYWORD_DEF || LA(1) == KEYWORD_USE)) {
				n=nodeStatement(browser, scope, script_node_id);
#line 1757 "expandedX3DVrmlParser.g"
				
				if (n) { value.push_back(n); }
				
#line 3668 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop104;
			}
			
		}
		_loop104:;
		} // ( ... )*
		match(RBRACKET);
#line 1760 "expandedX3DVrmlParser.g"
		
		nodes.value(value);
		
#line 3682 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mnv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::protoSfNodeValue(
	openvrml::browser & browser,
                 const boost::shared_ptr<openvrml::scope> & scope,
                 const node_interface_set & proto_interfaces,
                 proto_node_class::is_map_t & is_map,
                 proto_node_class::routes_t & routes,
                 const std::string & script_node_id
) {
#line 1712 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> snv;
#line 3703 "X3DVrmlParser.cpp"
#line 1712 "expandedX3DVrmlParser.g"
	
	boost::intrusive_ptr<openvrml::node> n;
	
#line 3708 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case ID:
	case KEYWORD_DEF:
	case KEYWORD_USE:
	{
		n=protoNodeStatement(browser,
                             scope,
                             proto_interfaces,
                             is_map,
                             routes,
                             script_node_id);
#line 1729 "expandedX3DVrmlParser.g"
		
		snv.reset(new sfnode(n));
		
#line 3725 "X3DVrmlParser.cpp"
		break;
	}
	case KEYWORD_NULL:
	{
		match(KEYWORD_NULL);
#line 1732 "expandedX3DVrmlParser.g"
		
		snv.reset(new sfnode);
		
#line 3735 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return snv;
}

boost::shared_ptr<field_value>  X3DVrmlParser::protoMfNodeValue(
	openvrml::browser & browser,
                 const boost::shared_ptr<openvrml::scope> & scope,
                 const node_interface_set & proto_interfaces,
                 proto_node_class::is_map_t & is_map,
                 proto_node_class::routes_t & routes,
                 const std::string & script_node_id
) {
#line 1766 "expandedX3DVrmlParser.g"
	boost::shared_ptr<field_value> mnv =
         boost::shared_ptr<field_value>(new mfnode);
#line 3757 "X3DVrmlParser.cpp"
#line 1766 "expandedX3DVrmlParser.g"
	
	using std::vector;
	
	boost::intrusive_ptr<openvrml::node> n;
	mfnode & nodes = static_cast<mfnode &>(*mnv);
	
#line 3765 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case ID:
	case KEYWORD_DEF:
	case KEYWORD_USE:
	{
		n=protoNodeStatement(browser,
                             scope,
                             proto_interfaces,
                             is_map,
                             routes,
                             script_node_id);
#line 1787 "expandedX3DVrmlParser.g"
		
		if (n) { nodes.value(mfnode::value_type(1, n)); }
		
#line 3782 "X3DVrmlParser.cpp"
		break;
	}
	case LBRACKET:
	{
		match(LBRACKET);
#line 1790 "expandedX3DVrmlParser.g"
		
		mfnode::value_type value;
		
#line 3792 "X3DVrmlParser.cpp"
		{ // ( ... )*
		for (;;) {
			if ((LA(1) == ID || LA(1) == KEYWORD_DEF || LA(1) == KEYWORD_USE)) {
				n=protoNodeStatement(browser,
                                 scope,
                                 proto_interfaces,
                                 is_map,
                                 routes,
                                 script_node_id);
#line 1798 "expandedX3DVrmlParser.g"
				
				if (n) { value.push_back(n); }
				
#line 3806 "X3DVrmlParser.cpp"
			}
			else {
				goto _loop107;
			}
			
		}
		_loop107:;
		} // ( ... )*
		match(RBRACKET);
#line 1801 "expandedX3DVrmlParser.g"
		
		nodes.value(value);
		
#line 3820 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return mnv;
}

bool  X3DVrmlParser::boolValue() {
#line 1505 "expandedX3DVrmlParser.g"
	bool val = false;
#line 3834 "X3DVrmlParser.cpp"
	
	switch ( LA(1)) {
	case KEYWORD_TRUE:
	{
		match(KEYWORD_TRUE);
#line 1509 "expandedX3DVrmlParser.g"
		val = true;
#line 3842 "X3DVrmlParser.cpp"
		break;
	}
	case KEYWORD_FALSE:
	{
		match(KEYWORD_FALSE);
#line 1510 "expandedX3DVrmlParser.g"
		val = false;
#line 3850 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return val;
}

void X3DVrmlParser::colorValue(
	color & c
) {
#line 1545 "expandedX3DVrmlParser.g"
	
	float r, g, b;
	
#line 3868 "X3DVrmlParser.cpp"
	
	r=colorComponent();
	g=colorComponent();
	b=colorComponent();
#line 1552 "expandedX3DVrmlParser.g"
	c.r(r);
	c.g(g);
	c.b(b);
#line 3877 "X3DVrmlParser.cpp"
}

float  X3DVrmlParser::colorComponent() {
#line 1558 "expandedX3DVrmlParser.g"
	float val = 0.0f;
#line 3883 "X3DVrmlParser.cpp"
	
	val=floatValue();
#line 1562 "expandedX3DVrmlParser.g"
	
	if (val < 0.0 || val > 1.0) {
	this->reportWarning("Color component values must be from 0 to "
	"1.");
	if (val < 0.0) {
	val = 0.0;
	} else if (val > 1.0) {
	val = 1.0;
	}
	}
	
#line 3898 "X3DVrmlParser.cpp"
	return val;
}

float  X3DVrmlParser::floatValue() {
#line 1607 "expandedX3DVrmlParser.g"
	float val;
#line 3905 "X3DVrmlParser.cpp"
	ANTLR_USE_NAMESPACE(antlr)RefToken  f0 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	ANTLR_USE_NAMESPACE(antlr)RefToken  f1 = ANTLR_USE_NAMESPACE(antlr)nullToken;
	
	switch ( LA(1)) {
	case REAL:
	{
		f0 = LT(1);
		match(REAL);
#line 1611 "expandedX3DVrmlParser.g"
		std::istringstream(f0->getText()) >> val;
#line 3916 "X3DVrmlParser.cpp"
		break;
	}
	case INTEGER:
	{
		f1 = LT(1);
		match(INTEGER);
#line 1612 "expandedX3DVrmlParser.g"
		std::istringstream(f1->getText()) >> val;
#line 3925 "X3DVrmlParser.cpp"
		break;
	}
	default:
	{
		throw ANTLR_USE_NAMESPACE(antlr)NoViableAltException(LT(1), getFilename());
	}
	}
	return val;
}

void X3DVrmlParser::rotationValue(
	rotation & r
) {
#line 1836 "expandedX3DVrmlParser.g"
	
	using openvrml_::fequal;
	float x, y, z, angle;
	
#line 3944 "X3DVrmlParser.cpp"
	
	x=floatValue();
	y=floatValue();
	z=floatValue();
	angle=floatValue();
#line 1844 "expandedX3DVrmlParser.g"
	
	vec3f axis(x, y, z);
	
	const float axisLength = axis.length();
	if (!fequal<float>()(axisLength, 1.0f)) {
	this->reportWarning("The axis component of a rotation must be "
	"a normalized vector.");
	if (fequal<float>()(axisLength, 0.0f)) {
	axis.z(1.0);
	} else {
	axis = axis.normalize();
	}
	}
	r = rotation(axis, angle);
	
#line 3966 "X3DVrmlParser.cpp"
}

void X3DVrmlParser::vec2fValue(
	vec2f & v
) {
#line 1985 "expandedX3DVrmlParser.g"
	
	float x, y;
	
#line 3976 "X3DVrmlParser.cpp"
	
	x=floatValue();
	y=floatValue();
#line 1992 "expandedX3DVrmlParser.g"
	v.x(x);
	v.y(y);
#line 3983 "X3DVrmlParser.cpp"
}

void X3DVrmlParser::vec3fValue(
	vec3f & v
) {
#line 2028 "expandedX3DVrmlParser.g"
	
	float x, y, z;
	
#line 3993 "X3DVrmlParser.cpp"
	
	x=floatValue();
	y=floatValue();
	z=floatValue();
#line 2035 "expandedX3DVrmlParser.g"
	v.x(x);
	v.y(y);
	v.z(z);
#line 4002 "X3DVrmlParser.cpp"
}

void X3DVrmlParser::initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& )
{
}
const char* X3DVrmlParser::tokenNames[] = {
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
	":",
	"\"AS\"",
	"\"COMPONENT\"",
	"\"EXPORT\"",
	"\"IMPORT\"",
	"\"META\"",
	"\"PROFILE\"",
	"\"inputOnly\"",
	"\"outputOnly\"",
	"\"inputOutput\"",
	"\"initializeOnly\"",
	"FIELDTYPE_SFCOLORRGBA",
	"FIELDTYPE_SFDOUBLE",
	"FIELDTYPE_SFVEC2D",
	"FIELDTYPE_SFVEC3D",
	"FIELDTYPE_MFBOOL",
	"FIELDTYPE_MFCOLORRGBA",
	"FIELDTYPE_MFDOUBLE",
	"FIELDTYPE_MFIMAGE",
	"FIELDTYPE_MFVEC3D",
	"FIELDTYPE_MFVEC2D",
	0
};

const unsigned long X3DVrmlParser::_tokenSet_0_data_[] = { 159662592UL, 0UL, 0UL, 0UL };
// ID "DEF" "EXTERNPROTO" "PROTO" "ROUTE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet X3DVrmlParser::_tokenSet_0(_tokenSet_0_data_,4);
const unsigned long X3DVrmlParser::_tokenSet_1_data_[] = { 159662594UL, 2359296UL, 0UL, 0UL };
// EOF ID "DEF" "EXTERNPROTO" "PROTO" "ROUTE" "USE" "COMPONENT" "META" 
const ANTLR_USE_NAMESPACE(antlr)BitSet X3DVrmlParser::_tokenSet_1(_tokenSet_1_data_,4);
const unsigned long X3DVrmlParser::_tokenSet_2_data_[] = { 159662594UL, 2097152UL, 0UL, 0UL };
// EOF ID "DEF" "EXTERNPROTO" "PROTO" "ROUTE" "USE" "META" 
const ANTLR_USE_NAMESPACE(antlr)BitSet X3DVrmlParser::_tokenSet_2(_tokenSet_2_data_,4);
const unsigned long X3DVrmlParser::_tokenSet_3_data_[] = { 25428480UL, 0UL, 0UL, 0UL };
// ID "EXTERNPROTO" "PROTO" "ROUTE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet X3DVrmlParser::_tokenSet_3(_tokenSet_3_data_,4);
const unsigned long X3DVrmlParser::_tokenSet_4_data_[] = { 1277952UL, 0UL, 0UL, 0UL };
// "eventIn" "eventOut" "exposedField" "field" 
const ANTLR_USE_NAMESPACE(antlr)BitSet X3DVrmlParser::_tokenSet_4(_tokenSet_4_data_,4);
const unsigned long X3DVrmlParser::_tokenSet_5_data_[] = { 138428960UL, 0UL, 0UL, 0UL };
// LBRACKET ID "DEF" "NULL" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet X3DVrmlParser::_tokenSet_5(_tokenSet_5_data_,4);
const unsigned long X3DVrmlParser::_tokenSet_6_data_[] = { 67648544UL, 0UL, 0UL, 0UL };
// LBRACKET INTEGER HEX_INTEGER REAL STRING "FALSE" "TRUE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet X3DVrmlParser::_tokenSet_6(_tokenSet_6_data_,4);
const unsigned long X3DVrmlParser::_tokenSet_7_data_[] = { 208174624UL, 0UL, 0UL, 0UL };
// LBRACKET ID INTEGER HEX_INTEGER REAL STRING "DEF" "FALSE" "IS" "NULL" 
// "TRUE" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet X3DVrmlParser::_tokenSet_7(_tokenSet_7_data_,4);
const unsigned long X3DVrmlParser::_tokenSet_8_data_[] = { 138428928UL, 0UL, 0UL, 0UL };
// ID "DEF" "NULL" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet X3DVrmlParser::_tokenSet_8(_tokenSet_8_data_,4);
const unsigned long X3DVrmlParser::_tokenSet_9_data_[] = { 134234656UL, 0UL, 0UL, 0UL };
// LBRACKET ID "DEF" "USE" 
const ANTLR_USE_NAMESPACE(antlr)BitSet X3DVrmlParser::_tokenSet_9(_tokenSet_9_data_,4);


