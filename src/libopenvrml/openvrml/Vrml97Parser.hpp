#ifndef INC_Vrml97Parser_hpp_
#define INC_Vrml97Parser_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.4: "Vrml97Parser.g" -> "Vrml97Parser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "Vrml97ParserTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

#line 22 "Vrml97Parser.g"

# include <memory>
# include <boost/shared_ptr.hpp>
# include "field_value.h"
# include "node_ptr.h"
# define ANTLR_LBRACE {
# define ANTLR_RBRACE }

namespace openvrml {
    class scope;
    class script_node;
    class node_type;
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

    private:
        void getNextChar();
        void identifyKeyword(antlr::Token &);
        void identifyFieldType(antlr::Token &);
        void identifyTerminalSymbol(antlr::Token &);

        std::istream & in_;
        size_t      line_;
        size_t      col_;
        int         c_;
        int         prev_char_;
        int         prev_token_type_;
        bool        read_too_much_;
        bool        expecting_field_type_;
    };

    //
    // Per-node list of IS mappings. A multimap is used for no other reason
    // than that redundancies are checked later.
    //
    typedef std::multimap<std::string, std::string> is_list;
}

namespace openvrml ANTLR_LBRACE

#line 109 "Vrml97Parser.hpp"
class CUSTOM_API Vrml97Parser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public Vrml97ParserTokenTypes
{
#line 606 "Vrml97Parser.g"

public:
    Vrml97Parser(antlr::TokenStream & lexer, const std::string & uri):
        antlr::LLkParser(lexer, 1),
        uri(uri)
    {}

private:
    const std::string uri;
#line 113 "Vrml97Parser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	Vrml97Parser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	Vrml97Parser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return Vrml97Parser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return Vrml97Parser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return Vrml97Parser::tokenNames;
	}
	public: void vrmlScene(
		openvrml::browser & browser,
          std::vector<node_ptr> & nodes
	);
	public: void statement(
		openvrml::browser & browser,
          std::vector<node_ptr> & nodes,
          const boost::shared_ptr<openvrml::scope> & scope
	);
	public: node_ptr  nodeStatement(
		openvrml::browser & browser,
              const boost::shared_ptr<openvrml::scope> & scope,
              const std::string & script_node_id
	);
	public: void protoStatement(
		openvrml::browser & browser, const boost::shared_ptr<openvrml::scope> & scope
	);
	public: void routeStatement(
		const openvrml::scope & scope
	);
	public: node_ptr  node(
		openvrml::browser & browser,
     const boost::shared_ptr<openvrml::scope> & scope,
     const std::string & node_id
	);
	public: void externproto(
		openvrml::browser & browser,
            const boost::shared_ptr<openvrml::scope> & scope
	);
	public: void proto(
		openvrml::browser & browser, const boost::shared_ptr<openvrml::scope> & scope
	);
	public: void protoInterfaceDeclaration(
		
    openvrml::browser & browser,
    const boost::shared_ptr<openvrml::scope> & outer_scope,
    const std::string & proto_id,
    node_interface_set & interfaces,
    proto_node_class::default_value_map_t & default_value_map
	);
	public: void protoBody(
		openvrml::browser & browser,
          const boost::shared_ptr<openvrml::scope> & scope,
          const node_interface_set & interfaces,
          std::vector<node_ptr> & impl_nodes,
          proto_node_class::is_map_t & is_map,
          proto_node_class::routes_t & routes
	);
	public: node_interface::type_id  eventInterfaceType();
	public: openvrml::field_value::type_id  fieldType();
	public: node_interface::type_id  fieldInterfaceType();
	public: boost::shared_ptr<field_value>  fieldValue(
		openvrml::browser & browser,
           const boost::shared_ptr<openvrml::scope> & scope,
           const openvrml::field_value::type_id ft,
           const std::string & node_id
	);
	public: node_ptr  protoNodeStatement(
		openvrml::browser & browser,
                   const boost::shared_ptr<openvrml::scope> & scope,
                   const node_interface_set & proto_interfaces,
                   proto_node_class::is_map_t & is_map,
                   proto_node_class::routes_t & routes,
                   const std::string & script_node_id
	);
	public: void protoBodyStatement(
		openvrml::browser & browser,
                   const boost::shared_ptr<openvrml::scope> & scope,
                   const node_interface_set & interfaces,
                   std::vector<node_ptr> & impl_nodes,
                   proto_node_class::is_map_t & is_map,
                   proto_node_class::routes_t & routes
	);
	public: void protoRouteStatement(
		const openvrml::scope & scope,
                    proto_node_class::routes_t & routes
	);
	public: node_ptr  protoNode(
		openvrml::browser & browser,
          const boost::shared_ptr<openvrml::scope> & scope,
          const node_interface_set & proto_interfaces,
          proto_node_class::is_map_t & is_map,
          proto_node_class::routes_t & routes,
          const std::string & node_id
	);
	public: void externInterfaceDeclaration(
		openvrml::node_interface_set & interfaces
	);
	public: mfstring  externprotoUrlList();
	public: node_interface::type_id  interfaceType();
	public: std::string  stringValue();
	public: void nodeBodyElement(
		browser & b,
                const boost::shared_ptr<openvrml::scope> & scope,
                const node_interface_set & interfaces,
                initial_value_map & initial_values
	);
	public: void scriptInterfaceDeclaration(
		browser & b,
                           const boost::shared_ptr<openvrml::scope> & scope,
                           node_interface_set & interfaces,
                           initial_value_map & initial_values,
                           const std::string & node_id
	);
	public: void scriptFieldInterfaceDeclaration(
		
    browser & b,
    const boost::shared_ptr<openvrml::scope> & scope,
    node_interface_set & interfaces,
    initial_value_map & initial_values,
    const std::string & script_node_id
	);
	public: void protoNodeBodyElement(
		openvrml::browser & browser,
                     const boost::shared_ptr<openvrml::scope> & scope,
                     const node_interface_set & proto_interfaces,
                     proto_node_class::is_map_t & is_map,
                     proto_node_class::routes_t & routes,
                     const node_interface_set & node_interfaces,
                     initial_value_map & initial_values,
                     is_list & is_mappings
	);
	public: void protoScriptInterfaceDeclaration(
		openvrml::browser & browser,
                                const boost::shared_ptr<openvrml::scope> & scope,
                                const node_interface_set & proto_interfaces,
                                proto_node_class::is_map_t & is_map,
                                proto_node_class::routes_t & routes,
                                const std::string & script_node_id,
                                node_interface_set & interfaces,
                                initial_value_map & initial_values,
                                is_list & is_mappings
	);
	public: boost::shared_ptr<field_value>  protoFieldValue(
		openvrml::browser & browser,
                const boost::shared_ptr<openvrml::scope> & scope,
                const node_interface_set & proto_interfaces,
                proto_node_class::is_map_t & is_map,
                proto_node_class::routes_t & routes,
                const field_value::type_id ft,
                const std::string & script_node_id
	);
	public: void isStatement(
		const std::string & impl_node_interface_id,
            is_list & is_mappings
	);
	public: void protoScriptFieldInterfaceDeclaration(
		
    openvrml::browser & browser,
    const boost::shared_ptr<openvrml::scope> & scope,
    const node_interface_set & proto_interfaces,
    proto_node_class::is_map_t & is_map,
    proto_node_class::routes_t & routes,
    node_interface_set & interfaces,
    initial_value_map & initial_values,
    const std::string & script_node_id,
    is_list & is_mappings
	);
	public: boost::shared_ptr<field_value>  nodeFieldValue(
		openvrml::browser & browser,
               const boost::shared_ptr<openvrml::scope> & scope,
               openvrml::field_value::type_id ft,
               const std::string & script_node_id
	);
	public: boost::shared_ptr<field_value>  nonNodeFieldValue(
		openvrml::field_value::type_id ft
	);
	public: boost::shared_ptr<field_value>  protoNodeFieldValue(
		openvrml::browser & browser,
                    const boost::shared_ptr<openvrml::scope> & scope,
                    const node_interface_set & proto_interfaces,
                    proto_node_class::is_map_t & is_map,
                    proto_node_class::routes_t & routes,
                    field_value::type_id ft,
                    const std::string & script_node_id
	);
	public: boost::shared_ptr<field_value>  sfBoolValue();
	public: boost::shared_ptr<field_value>  sfColorValue();
	public: boost::shared_ptr<field_value>  sfFloatValue();
	public: boost::shared_ptr<field_value>  sfImageValue();
	public: boost::shared_ptr<field_value>  sfInt32Value();
	public: boost::shared_ptr<field_value>  sfRotationValue();
	public: boost::shared_ptr<field_value>  sfStringValue();
	public: boost::shared_ptr<field_value>  sfTimeValue();
	public: boost::shared_ptr<field_value>  sfVec2fValue();
	public: boost::shared_ptr<field_value>  sfVec3fValue();
	public: boost::shared_ptr<field_value>  mfColorValue();
	public: boost::shared_ptr<field_value>  mfFloatValue();
	public: boost::shared_ptr<field_value>  mfInt32Value();
	public: boost::shared_ptr<field_value>  mfRotationValue();
	public: boost::shared_ptr<field_value>  mfStringValue();
	public: boost::shared_ptr<field_value>  mfTimeValue();
	public: boost::shared_ptr<field_value>  mfVec2fValue();
	public: boost::shared_ptr<field_value>  mfVec3fValue();
	public: boost::shared_ptr<field_value>  sfNodeValue(
		openvrml::browser & browser,
            const boost::shared_ptr<openvrml::scope> & scope,
            const std::string & script_node_id
	);
	public: boost::shared_ptr<field_value>  mfNodeValue(
		openvrml::browser & browser,
            const boost::shared_ptr<openvrml::scope> & scope,
            const std::string & script_node_id
	);
	public: boost::shared_ptr<field_value>  protoSfNodeValue(
		openvrml::browser & browser,
                 const boost::shared_ptr<openvrml::scope> & scope,
                 const node_interface_set & proto_interfaces,
                 proto_node_class::is_map_t & is_map,
                 proto_node_class::routes_t & routes,
                 const std::string & script_node_id
	);
	public: boost::shared_ptr<field_value>  protoMfNodeValue(
		openvrml::browser & browser,
                 const boost::shared_ptr<openvrml::scope> & scope,
                 const node_interface_set & proto_interfaces,
                 proto_node_class::is_map_t & is_map,
                 proto_node_class::routes_t & routes,
                 const std::string & script_node_id
	);
	public: bool  boolValue();
	public: void colorValue(
		color & c
	);
	public: float  colorComponent();
	public: float  floatValue();
	public: int32  intValue();
	public: void rotationValue(
		rotation & r
	);
	public: double  doubleValue();
	public: void vec2fValue(
		vec2f & v
	);
	public: void vec3fValue(
		vec3f & v
	);
public:
	ANTLR_USE_NAMESPACE(antlr)RefAST getAST()
	{
		return returnAST;
	}
	
protected:
	ANTLR_USE_NAMESPACE(antlr)RefAST returnAST;
private:
	static const char* tokenNames[];
#ifndef NO_STATIC_CONSTS
	static const int NUM_TOKENS = 48;
#else
	enum {
		NUM_TOKENS = 48
	};
#endif
	
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
};

#endif /*INC_Vrml97Parser_hpp_*/
