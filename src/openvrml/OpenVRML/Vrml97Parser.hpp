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

namespace OpenVRML {
    class VrmlNamespace;
    class ScriptNode;
    class NodeType;
    class Doc2;
}

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

#line 102 "Vrml97Parser.hpp"
class Vrml97Parser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public Vrml97ParserTokenTypes
 {
#line 586 "Vrml97Parser.g"

    public:
        enum NodeInterfaceType {
            INVALID_NODE_INTERFACE_TYPE, EVENTIN, EVENTOUT, FIELD, EXPOSEDFIELD
        };
#line 106 "Vrml97Parser.hpp"
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
		OpenVRML::MFNode & mfNode, OpenVRML::VrmlNamespace & vrmlNamespace,
          const OpenVRML::Doc2 * doc
	);
	public: void statement(
		OpenVRML::MFNode & mfNode, OpenVRML::VrmlNamespace & vrmlNamespace,
          const OpenVRML::Doc2 * doc
	);
	public: OpenVRML::NodePtr  nodeStatement(
		OpenVRML::VrmlNamespace & vrmlNamespace,
              const OpenVRML::Doc2 * doc
	);
	public: void protoStatement(
		OpenVRML::VrmlNamespace & vrmlNamespace,
               const OpenVRML::Doc2 * doc
	);
	public: void routeStatement(
		const OpenVRML::VrmlNamespace & vrmlNamespace
	);
	public: OpenVRML::NodePtr  node(
		OpenVRML::VrmlNamespace & vrmlNamespace, const OpenVRML::Doc2 * doc,
     const std::string & nodeId
	);
	public: void externproto(
		OpenVRML::VrmlNamespace & vrmlNamespace, const OpenVRML::Doc2 * doc
	);
	public: void proto(
		OpenVRML::VrmlNamespace & vrmlNamespace, const OpenVRML::Doc2 * doc
	);
	public: void protoInterfaceDeclaration(
		OpenVRML::VrmlNamespace & vrmlNamespace,
                          const OpenVRML::Doc2 * doc,
                          OpenVRML::NodeType & nodeType
	);
	public: void protoBody(
		const OpenVRML::Doc2 * doc, OpenVRML::NodeType & nodeType
	);
	public: Vrml97Parser::NodeInterfaceType  eventInterfaceType();
	public: OpenVRML::FieldValue::FieldType  fieldType();
	public: Vrml97Parser::NodeInterfaceType  fieldInterfaceType();
	public: OpenVRML::FieldValue *  fieldValue(
		OpenVRML::VrmlNamespace & vrmlNamespace, const OpenVRML::Doc2 * doc,
           OpenVRML::FieldValue::FieldType ft
	);
	public: OpenVRML::NodePtr  protoNodeStatement(
		const OpenVRML::Doc2 * doc,
                   OpenVRML::NodeType & protoNodeType
	);
	public: void protoBodyStatement(
		const OpenVRML::Doc2 * doc, OpenVRML::NodeType & nodeType
	);
	public: OpenVRML::NodePtr  protoNode(
		const OpenVRML::Doc2 * doc, OpenVRML::NodeType & protoNodeType,
          const std::string & nodeId
	);
	public: void externInterfaceDeclaration(
		OpenVRML::NodeType & nodeType
	);
	public: OpenVRML::MFString *  externprotoUrlList();
	public: Vrml97Parser::NodeInterfaceType  interfaceType();
	public: std::string  stringValue();
	public: void nodeBodyElement(
		OpenVRML::VrmlNamespace & vrmlNamespace,
                const OpenVRML::Doc2 * doc, OpenVRML::Node & node
	);
	public: void scriptInterfaceDeclaration(
		OpenVRML::VrmlNamespace & vrmlNamespace,
                           const OpenVRML::Doc2 * doc,
                           OpenVRML::ScriptNode & node
	);
	public: void scriptFieldInterfaceDeclaration(
		OpenVRML::VrmlNamespace & vrmlNamespace,
                                const OpenVRML::Doc2 * doc,
                                OpenVRML::ScriptNode & node
	);
	public: void protoNodeBodyElement(
		const OpenVRML::Doc2 * doc,
                     OpenVRML::NodeType & protoNodeType,
                     OpenVRML::Node & node
	);
	public: void protoScriptInterfaceDeclaration(
		const OpenVRML::Doc2 * doc,
                                OpenVRML::NodeType & protoNodeType,
                                OpenVRML::ScriptNode & node
	);
	public: void isStatement(
		OpenVRML::NodeType & protoNodeType, OpenVRML::Node & node,
            std::string const & nodeInterfaceId
	);
	public: OpenVRML::FieldValue *  protoFieldValue(
		const OpenVRML::Doc2 * doc, OpenVRML::NodeType & protoNodeType,
                OpenVRML::FieldValue::FieldType ft
	);
	public: void protoScriptFieldInterfaceDeclaration(
		const OpenVRML::Doc2 * doc,
                                     OpenVRML::NodeType & protoNodeType,
                                     OpenVRML::ScriptNode & node
	);
	public: OpenVRML::FieldValue *  nodeFieldValue(
		OpenVRML::VrmlNamespace & vrmlNamespace,
               const OpenVRML::Doc2 * doc, OpenVRML::FieldValue::FieldType ft
	);
	public: OpenVRML::FieldValue *  nonNodeFieldValue(
		OpenVRML::FieldValue::FieldType ft
	);
	public: OpenVRML::FieldValue *  protoNodeFieldValue(
		const OpenVRML::Doc2 * doc,
                    OpenVRML::NodeType & protoNodeType,
                    OpenVRML::FieldValue::FieldType ft
	);
	public: OpenVRML::SFBool *  sfBoolValue();
	public: OpenVRML::SFColor *  sfColorValue();
	public: OpenVRML::SFFloat *  sfFloatValue();
	public: OpenVRML::SFImage *  sfImageValue();
	public: OpenVRML::SFInt32 *  sfInt32Value();
	public: OpenVRML::SFRotation *  sfRotationValue();
	public: OpenVRML::SFString *  sfStringValue();
	public: OpenVRML::SFTime *  sfTimeValue();
	public: OpenVRML::SFVec2f *  sfVec2fValue();
	public: OpenVRML::SFVec3f *  sfVec3fValue();
	public: OpenVRML::MFColor *  mfColorValue();
	public: OpenVRML::MFFloat *  mfFloatValue();
	public: OpenVRML::MFInt32 *  mfInt32Value();
	public: OpenVRML::MFRotation *  mfRotationValue();
	public: OpenVRML::MFString *  mfStringValue();
	public: OpenVRML::MFTime *  mfTimeValue();
	public: OpenVRML::MFVec2f *  mfVec2fValue();
	public: OpenVRML::MFVec3f *  mfVec3fValue();
	public: OpenVRML::SFNode *  sfNodeValue(
		OpenVRML::VrmlNamespace & vrmlNamespace,
            const OpenVRML::Doc2 * doc
	);
	public: OpenVRML::MFNode *  mfNodeValue(
		OpenVRML::VrmlNamespace & vrmlNamespace, const OpenVRML::Doc2 * doc
	);
	public: OpenVRML::SFNode *  protoSfNodeValue(
		const OpenVRML::Doc2 * doc,
                 OpenVRML::NodeType & protoNodeType
	);
	public: OpenVRML::MFNode *  protoMfNodeValue(
		const OpenVRML::Doc2 * doc, OpenVRML::NodeType & protoNodeType
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
