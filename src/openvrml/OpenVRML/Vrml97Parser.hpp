#ifndef INC_Vrml97Parser_hpp_
#define INC_Vrml97Parser_hpp_

#include "antlr/config.hpp"
/* $ANTLR 2.7.1: "Vrml97Parser.g" -> "Vrml97Parser.hpp"$ */
#include "antlr/TokenStream.hpp"
#include "antlr/TokenBuffer.hpp"
#include "Vrml97ParserTokenTypes.hpp"
#include "antlr/LLkParser.hpp"

#line 21 "Vrml97Parser.g"

# include <memory>
# include "field.h"
# include "VrmlNodePtr.h"
# define ANTLR_LBRACE {
# define ANTLR_RBRACE }
class VrmlNamespace;
class VrmlNodeScript;
class NodeType;
class Doc2;

namespace ANTLR_LBRACE
    class Vrml97Scanner : public antlr::TokenStream {
    public:
        static const int EOF_;
        //
        // The following identifiers for constants must match those in the file
        // Vrml97TokenTypes.txt.
        //
        static const int PERIOD;
        static const int LBRACKET;
        static const int RBRACKET;
        static const int LBRACE;
        static const int RBRACE;
        static const int ID;
        static const int INTEGER;
        static const int HEX_INTEGER;
        static const int REAL;
        static const int STRING;
        static const int KEYWORD_DEF;
        static const int KEYWORD_EVENTIN;
        static const int KEYWORD_EVENTOUT;
        static const int KEYWORD_EXPOSEDFIELD;
        static const int KEYWORD_EXTERNPROTO;
        static const int KEYWORD_FALSE;
        static const int KEYWORD_FIELD;
        static const int KEYWORD_IS;
        static const int KEYWORD_NULL;
        static const int KEYWORD_PROTO;
        static const int KEYWORD_ROUTE;
        static const int KEYWORD_TO;
        static const int KEYWORD_TRUE;
        static const int KEYWORD_USE;
        static const int FIELDTYPE_SFBOOL;
        static const int FIELDTYPE_SFCOLOR;
        static const int FIELDTYPE_SFFLOAT;
        static const int FIELDTYPE_SFIMAGE;
        static const int FIELDTYPE_SFINT32;
        static const int FIELDTYPE_SFNODE;
        static const int FIELDTYPE_SFROTATION;
        static const int FIELDTYPE_SFSTRING;
        static const int FIELDTYPE_SFTIME;
        static const int FIELDTYPE_SFVEC2F;
        static const int FIELDTYPE_SFVEC3F;
        static const int FIELDTYPE_MFCOLOR;
        static const int FIELDTYPE_MFFLOAT;
        static const int FIELDTYPE_MFINT32;
        static const int FIELDTYPE_MFNODE;
        static const int FIELDTYPE_MFROTATION;
        static const int FIELDTYPE_MFSTRING;
        static const int FIELDTYPE_MFTIME;
        static const int FIELDTYPE_MFVEC2F;
        static const int FIELDTYPE_MFVEC3F;

        Vrml97Scanner(istream &);

        antlr::RefToken nextToken();
        size_t line() const;
        size_t col() const;

    private:
        void _getNextChar();
        void _identifyKeyword(antlr::Token &);
        void _identifyFieldType(antlr::Token &);
        void _identifyTerminalSymbol(antlr::Token &);

        istream &   _istm;
        size_t      _line;
        size_t      _col;
        int         _c;
        int         _prevChar;
        int         _prevTokenType;
        bool        _readTooMuch;
        bool        _expectingFieldType;
    };

#line 99 "Vrml97Parser.hpp"
class Vrml97Parser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public Vrml97ParserTokenTypes
 {
#line 583 "Vrml97Parser.g"

    public:
        enum NodeInterfaceType {
            INVALID_NODE_INTERFACE_TYPE, EVENTIN, EVENTOUT, FIELD, EXPOSEDFIELD
        };
#line 103 "Vrml97Parser.hpp"
protected:
	Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	Vrml97Parser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	public: void vrmlScene(
		VrmlMFNode & mfNode, VrmlNamespace & vrmlNamespace, Doc2 const * doc
	);
	public: void statement(
		VrmlMFNode & mfNode, VrmlNamespace & vrmlNamespace, Doc2 const * doc
	);
	public: VrmlNodePtr  nodeStatement(
		VrmlNamespace & vrmlNamespace, Doc2 const * doc
	);
	public: void protoStatement(
		VrmlNamespace & vrmlNamespace, Doc2 const * doc
	);
	public: void routeStatement(
		VrmlNamespace const & vrmlNamespace
	);
	public: VrmlNodePtr  node(
		VrmlNamespace & vrmlNamespace, const Doc2 * doc,
     const std::string & nodeId
	);
	public: void externproto(
		VrmlNamespace & vrmlNamespace, Doc2 const * doc
	);
	public: void proto(
		VrmlNamespace & vrmlNamespace, Doc2 const * doc
	);
	public: void protoInterfaceDeclaration(
		VrmlNamespace & vrmlNamespace, Doc2 const * doc,
                          NodeType & nodeType
	);
	public: void protoBody(
		Doc2 const * doc, NodeType & nodeType
	);
	public: Vrml97Parser::NodeInterfaceType  eventInterfaceType();
	public: VrmlField::VrmlFieldType  fieldType();
	public: Vrml97Parser::NodeInterfaceType  fieldInterfaceType();
	public: VrmlField *  fieldValue(
		VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlField::VrmlFieldType ft
	);
	public: VrmlNodePtr  protoNodeStatement(
		Doc2 const * doc, NodeType & protoNodeType
	);
	public: void protoBodyStatement(
		Doc2 const * doc, NodeType & nodeType
	);
	public: VrmlNodePtr  protoNode(
		const Doc2 * doc, NodeType & protoNodeType,
          const std::string & nodeId
	);
	public: void externInterfaceDeclaration(
		NodeType & nodeType
	);
	public: VrmlMFString *  externprotoUrlList();
	public: Vrml97Parser::NodeInterfaceType  interfaceType();
	public: std::string  stringValue();
	public: void nodeBodyElement(
		VrmlNamespace & vrmlNamespace, Doc2 const * doc,
                VrmlNode & node
	);
	public: void scriptInterfaceDeclaration(
		VrmlNamespace & vrmlNamespace,
                           const Doc2 * doc,
                           VrmlNodeScript & node
	);
	public: void scriptFieldInterfaceDeclaration(
		VrmlNamespace & vrmlNamespace,
                                const Doc2 * doc,
                                VrmlNodeScript & node
	);
	public: void protoNodeBodyElement(
		Doc2 const * doc,
                     NodeType & protoNodeType,
                     VrmlNode & node
	);
	public: void protoScriptInterfaceDeclaration(
		Doc2 const * doc, NodeType & protoNodeType, VrmlNodeScript & node
	);
	public: void isStatement(
		NodeType & protoNodeType,
            VrmlNode & node,
            std::string const & nodeInterfaceId
	);
	public: VrmlField *  protoFieldValue(
		Doc2 const * doc, NodeType & protoNodeType, VrmlField::VrmlFieldType ft
	);
	public: void protoScriptFieldInterfaceDeclaration(
		Doc2 const * doc, NodeType & protoNodeType, VrmlNodeScript & node
	);
	public: VrmlField *  nodeFieldValue(
		VrmlNamespace & vrmlNamespace, Doc2 const * doc, VrmlField::VrmlFieldType ft
	);
	public: VrmlField *  nonNodeFieldValue(
		VrmlField::VrmlFieldType ft
	);
	public: VrmlField *  protoNodeFieldValue(
		Doc2 const * doc, NodeType & protoNodeType, VrmlField::VrmlFieldType ft
	);
	public: VrmlSFBool *  sfBoolValue();
	public: VrmlSFColor *  sfColorValue();
	public: VrmlSFFloat *  sfFloatValue();
	public: VrmlSFImage *  sfImageValue();
	public: VrmlSFInt32 *  sfInt32Value();
	public: VrmlSFRotation *  sfRotationValue();
	public: VrmlSFString *  sfStringValue();
	public: VrmlSFTime *  sfTimeValue();
	public: VrmlSFVec2f *  sfVec2fValue();
	public: VrmlSFVec3f *  sfVec3fValue();
	public: VrmlMFColor *  mfColorValue();
	public: VrmlMFFloat *  mfFloatValue();
	public: VrmlMFInt32 *  mfInt32Value();
	public: VrmlMFRotation *  mfRotationValue();
	public: VrmlMFString *  mfStringValue();
	public: VrmlMFTime *  mfTimeValue();
	public: VrmlMFVec2f *  mfVec2fValue();
	public: VrmlMFVec3f *  mfVec3fValue();
	public: VrmlSFNode *  sfNodeValue(
		VrmlNamespace & vrmlNamespace,
            Doc2 const * doc
	);
	public: VrmlMFNode *  mfNodeValue(
		VrmlNamespace & vrmlNamespace, Doc2 const * doc
	);
	public: VrmlSFNode *  protoSfNodeValue(
		Doc2 const * doc,
                 NodeType & protoNodeType
	);
	public: VrmlMFNode *  protoMfNodeValue(
		Doc2 const * doc, NodeType & protoNodeType
	);
	public: bool  boolValue();
	public: void colorValue(
		float c[3]
	);
	public: float  colorComponent();
	public: float  floatValue();
	public: long  intValue();
	public: void rotationValue(
		float r[4]
	);
	public: double  doubleValue();
	public: void vec2fValue(
		float v[2]
	);
	public: void vec3fValue(
		float v[3]
	);
private:
	static const char* _tokenNames[];
	
	static const unsigned long _tokenSet_0_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_0;
	static const unsigned long _tokenSet_1_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_1;
	static const unsigned long _tokenSet_2_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_2;
	static const unsigned long _tokenSet_3_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_3;
	static const unsigned long _tokenSet_4_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_4;
	static const unsigned long _tokenSet_5_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_5;
	static const unsigned long _tokenSet_6_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_6;
	static const unsigned long _tokenSet_7_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_7;
	static const unsigned long _tokenSet_8_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_8;
	static const unsigned long _tokenSet_9_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_9;
	static const unsigned long _tokenSet_10_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_10;
	static const unsigned long _tokenSet_11_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_11;
	static const unsigned long _tokenSet_12_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_12;
	static const unsigned long _tokenSet_13_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_13;
	static const unsigned long _tokenSet_14_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_14;
	static const unsigned long _tokenSet_15_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_15;
	static const unsigned long _tokenSet_16_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_16;
	static const unsigned long _tokenSet_17_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_17;
	static const unsigned long _tokenSet_18_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_18;
	static const unsigned long _tokenSet_19_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_19;
};

#endif /*INC_Vrml97Parser_hpp_*/
