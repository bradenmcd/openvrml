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
# include "nodeptr.h"
# include "fieldvalueptr.h"
# define ANTLR_LBRACE {
# define ANTLR_RBRACE }

namespace OpenVRML {
    class Scope;
    class ScriptNode;
    class NodeType;
    class Doc2;
}

namespace {
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

        Vrml97Scanner(std::istream &);

        virtual antlr::RefToken nextToken();
        
        size_t line() const;
        size_t col() const;

    private:
        void _getNextChar();
        void _identifyKeyword(antlr::Token &);
        void _identifyFieldType(antlr::Token &);
        void _identifyTerminalSymbol(antlr::Token &);

        std::istream & _istm;
        size_t      _line;
        size_t      _col;
        int         _c;
        int         _prevChar;
        int         _prevTokenType;
        bool        _readTooMuch;
        bool        _expectingFieldType;
    };
}

namespace OpenVRML ANTLR_LBRACE

#line 107 "Vrml97Parser.hpp"
class Vrml97Parser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public Vrml97ParserTokenTypes
 {
#line 1 "Vrml97Parser.g"
#line 111 "Vrml97Parser.hpp"
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
		OpenVRML::VrmlScene & scene,
          OpenVRML::MFNode & mfNode,
          const OpenVRML::Doc2 * doc
	);
	public: void statement(
		OpenVRML::VrmlScene & scene,
          OpenVRML::MFNode & mfNode,
          const OpenVRML::ScopePtr & scope,
          const OpenVRML::Doc2 * doc
	);
	public: OpenVRML::NodePtr  nodeStatement(
		OpenVRML::VrmlScene & scene,
              const OpenVRML::ScopePtr & scope,
              const OpenVRML::Doc2 * doc
	);
	public: void protoStatement(
		OpenVRML::VrmlScene & scene,
               const OpenVRML::ScopePtr & scope,
               const OpenVRML::Doc2 * doc
	);
	public: void routeStatement(
		const OpenVRML::Scope & scope
	);
	public: OpenVRML::NodePtr  node(
		OpenVRML::VrmlScene & scene,
     const OpenVRML::ScopePtr & scope,
     const OpenVRML::Doc2 * doc,
     const std::string & nodeId
	);
	public: void externproto(
		OpenVRML::VrmlScene & scene, const OpenVRML::ScopePtr & scope,
            const OpenVRML::Doc2 * doc
	);
	public: void proto(
		OpenVRML::VrmlScene & scene,
      const OpenVRML::ScopePtr & scope,
      const OpenVRML::Doc2 * doc
	);
	public: void protoInterfaceDeclaration(
		const OpenVRML::ScopePtr & scope,
                          const OpenVRML::Doc2 * doc,
                          OpenVRML::ProtoNodeClass & proto
	);
	public: void protoBody(
		const OpenVRML::ScopePtr & scope,
          const OpenVRML::Doc2 * doc,
          OpenVRML::ProtoNodeClass & proto
	);
	public: OpenVRML::NodeInterface::Type  eventInterfaceType();
	public: OpenVRML::FieldValue::Type  fieldType();
	public: OpenVRML::NodeInterface::Type  fieldInterfaceType();
	public: OpenVRML::FieldValuePtr  fieldValue(
		OpenVRML::VrmlScene & scene,
           const OpenVRML::ScopePtr & scope,
           const OpenVRML::Doc2 * doc,
           OpenVRML::FieldValue::Type ft
	);
	public: OpenVRML::NodePtr  protoNodeStatement(
		OpenVRML::ProtoNodeClass & proto,
                   const OpenVRML::ScopePtr & scope,
                   const OpenVRML::Doc2 * doc
	);
	public: void protoBodyStatement(
		OpenVRML::ProtoNodeClass & proto,
                   const OpenVRML::ScopePtr & scope,
                   const OpenVRML::Doc2 * doc
	);
	public: OpenVRML::NodePtr  protoNode(
		OpenVRML::ProtoNodeClass & proto,
          const OpenVRML::ScopePtr & scope,
          const OpenVRML::Doc2 * doc,
          const std::string & nodeId
	);
	public: void externInterfaceDeclaration(
		OpenVRML::NodeInterfaceSet & interfaces
	);
	public: OpenVRML::MFString  externprotoUrlList();
	public: OpenVRML::NodeInterface::Type  interfaceType();
	public: std::string  stringValue();
	public: void nodeBodyElement(
		const OpenVRML::ScopePtr & scope,
                const OpenVRML::Doc2 * doc,
                OpenVRML::Node & node
	);
	public: void scriptInterfaceDeclaration(
		const OpenVRML::ScopePtr & scope,
                           const OpenVRML::Doc2 * doc,
                           OpenVRML::ScriptNode & node
	);
	public: void scriptFieldInterfaceDeclaration(
		const OpenVRML::ScopePtr & scope,
                                const OpenVRML::Doc2 * doc,
                                OpenVRML::ScriptNode & node
	);
	public: void protoNodeBodyElement(
		OpenVRML::ProtoNodeClass & proto,
                     const OpenVRML::ScopePtr & scope,
                     const OpenVRML::Doc2 * doc,
                     OpenVRML::Node & node
	);
	public: void protoScriptInterfaceDeclaration(
		OpenVRML::ProtoNodeClass & proto,
                                const OpenVRML::ScopePtr & scope,
                                const OpenVRML::Doc2 * doc,
                                OpenVRML::ScriptNode & node
	);
	public: OpenVRML::FieldValuePtr  protoFieldValue(
		OpenVRML::ProtoNodeClass & proto,
                const OpenVRML::ScopePtr & scope,
                const OpenVRML::Doc2 * doc,
                OpenVRML::FieldValue::Type ft
	);
	public: void isStatement(
		OpenVRML::ProtoNodeClass & proto, OpenVRML::Node & node,
            std::string const & nodeInterfaceId
	);
	public: void protoScriptFieldInterfaceDeclaration(
		OpenVRML::ProtoNodeClass & proto,
                                     const OpenVRML::ScopePtr & scope,
                                     const OpenVRML::Doc2 * doc,
                                     OpenVRML::ScriptNode & node
	);
	public: OpenVRML::FieldValuePtr  nodeFieldValue(
		OpenVRML::VrmlScene & scene,
               const OpenVRML::ScopePtr & scope,
               const OpenVRML::Doc2 * doc,
               OpenVRML::FieldValue::Type ft
	);
	public: OpenVRML::FieldValuePtr  nonNodeFieldValue(
		OpenVRML::FieldValue::Type ft
	);
	public: OpenVRML::FieldValuePtr  protoNodeFieldValue(
		OpenVRML::ProtoNodeClass & proto,
                    const OpenVRML::ScopePtr & scope,
                    const OpenVRML::Doc2 * doc,
                    OpenVRML::FieldValue::Type ft
	);
	public: OpenVRML::FieldValuePtr  sfBoolValue();
	public: OpenVRML::FieldValuePtr  sfColorValue();
	public: OpenVRML::FieldValuePtr  sfFloatValue();
	public: OpenVRML::FieldValuePtr  sfImageValue();
	public: OpenVRML::FieldValuePtr  sfInt32Value();
	public: OpenVRML::FieldValuePtr  sfRotationValue();
	public: OpenVRML::FieldValuePtr  sfStringValue();
	public: OpenVRML::FieldValuePtr  sfTimeValue();
	public: OpenVRML::FieldValuePtr  sfVec2fValue();
	public: OpenVRML::FieldValuePtr  sfVec3fValue();
	public: OpenVRML::FieldValuePtr  mfColorValue();
	public: OpenVRML::FieldValuePtr  mfFloatValue();
	public: OpenVRML::FieldValuePtr  mfInt32Value();
	public: OpenVRML::FieldValuePtr  mfRotationValue();
	public: OpenVRML::FieldValuePtr  mfStringValue();
	public: OpenVRML::FieldValuePtr  mfTimeValue();
	public: OpenVRML::FieldValuePtr  mfVec2fValue();
	public: OpenVRML::FieldValuePtr  mfVec3fValue();
	public: OpenVRML::FieldValuePtr  sfNodeValue(
		OpenVRML::VrmlScene & scene,
            const OpenVRML::ScopePtr & scope,
            const OpenVRML::Doc2 * doc
	);
	public: OpenVRML::FieldValuePtr  mfNodeValue(
		OpenVRML::VrmlScene & scene,
            const OpenVRML::ScopePtr & scope,
            const OpenVRML::Doc2 * doc
	);
	public: OpenVRML::FieldValuePtr  protoSfNodeValue(
		OpenVRML::ProtoNodeClass & proto,
                 const OpenVRML::ScopePtr & scope,
                 const OpenVRML::Doc2 * doc
	);
	public: OpenVRML::FieldValuePtr  protoMfNodeValue(
		OpenVRML::ProtoNodeClass & proto,
                 const OpenVRML::ScopePtr & scope,
                 const OpenVRML::Doc2 * doc
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
};

#endif /*INC_Vrml97Parser_hpp_*/
