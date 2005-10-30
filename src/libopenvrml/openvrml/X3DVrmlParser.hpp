#ifndef INC_X3DVrmlParser_hpp_
#define INC_X3DVrmlParser_hpp_

#include <antlr/config.hpp>
/* $ANTLR 2.7.4: "expandedX3DVrmlParser.g" -> "X3DVrmlParser.hpp"$ */
#include <antlr/TokenStream.hpp>
#include <antlr/TokenBuffer.hpp>
#include "X3DVrmlParserTokenTypes.hpp"
#include <antlr/LLkParser.hpp>

#line 1 "expandedX3DVrmlParser.g"

# include "Vrml97Parser.hpp"

namespace {
    class X3DVrmlScanner : public Vrml97Scanner {
    public:
        //
        // The following identifiers for constants must match those in the file
        // X3DVrmlTokenTypes.txt.
        //
        static const int COLON;
        static const int KEYWORD_AS;
        static const int KEYWORD_COMPONENT;
        static const int KEYWORD_EXPORT;
        static const int KEYWORD_IMPORT;
        static const int KEYWORD_META;
        static const int KEYWORD_PROFILE;
        static const int KEYWORD_INPUTONLY;
        static const int KEYWORD_OUTPUTONLY;
        static const int KEYWORD_INPUTOUTPUT;
        static const int KEYWORD_INITIALIZEONLY;
        static const int FIELDTYPE_SFCOLORRGBA;
        static const int FIELDTYPE_SFDOUBLE;
        static const int FIELDTYPE_SFVEC2D;
        static const int FIELDTYPE_SFVEC3D;
        static const int FIELDTYPE_MFBOOL;
        static const int FIELDTYPE_MFCOLORRGBA;
        static const int FIELDTYPE_MFDOUBLE;
        static const int FIELDTYPE_MFIMAGE;
        static const int FIELDTYPE_MFVEC2D;
        static const int FIELDTYPE_MFVEC3D;

        X3DVrmlScanner(std::istream &);

    protected:
        virtual void identifyFieldType(antlr::Token &);

    private:
        virtual void identifyKeyword(antlr::Token &);
    };
}

namespace openvrml ANTLR_LBRACE

#line 57 "X3DVrmlParser.hpp"
class CUSTOM_API X3DVrmlParser : public ANTLR_USE_NAMESPACE(antlr)LLkParser, public X3DVrmlParserTokenTypes
{
#line 164 "expandedX3DVrmlParser.g"

public:
    X3DVrmlParser(antlr::TokenStream & lexer, const std::string & uri):
        antlr::LLkParser(lexer, 1),
        uri(uri)
    {}

private:
    const std::string uri;
#line 61 "X3DVrmlParser.hpp"
public:
	void initializeASTFactory( ANTLR_USE_NAMESPACE(antlr)ASTFactory& factory );
protected:
	X3DVrmlParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf, int k);
public:
	X3DVrmlParser(ANTLR_USE_NAMESPACE(antlr)TokenBuffer& tokenBuf);
protected:
	X3DVrmlParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer, int k);
public:
	X3DVrmlParser(ANTLR_USE_NAMESPACE(antlr)TokenStream& lexer);
	X3DVrmlParser(const ANTLR_USE_NAMESPACE(antlr)ParserSharedInputState& state);
	int getNumTokens() const
	{
		return X3DVrmlParser::NUM_TOKENS;
	}
	const char* getTokenName( int type ) const
	{
		if( type > getNumTokens() ) return 0;
		return X3DVrmlParser::tokenNames[type];
	}
	const char* const* getTokenNames() const
	{
		return X3DVrmlParser::tokenNames;
	}
	public: void vrmlScene(
		openvrml::scene & scene,
          std::vector<boost::intrusive_ptr<openvrml::node> > & nodes
	);
	public: openvrml::profile_id  profileStatement();
	public: void componentStatement();
	public: void metaStatement();
	public: void statement(
		openvrml::browser & browser,
          std::vector<boost::intrusive_ptr<openvrml::node> > & nodes,
          const boost::shared_ptr<openvrml::scope> & scope
	);
	public: int32  intValue();
	public: std::string  stringValue();
	public: openvrml::field_value::type_id  fieldType();
	public: boost::shared_ptr<field_value>  nonNodeFieldValue(
		openvrml::field_value::type_id ft
	);
	public: boost::shared_ptr<field_value>  sfBoolValue();
	public: boost::shared_ptr<field_value>  sfColorValue();
	public: boost::shared_ptr<field_value>  sfFloatValue();
	public: boost::shared_ptr<field_value>  sfDoubleValue();
	public: boost::shared_ptr<field_value>  sfImageValue();
	public: boost::shared_ptr<field_value>  sfInt32Value();
	public: boost::shared_ptr<field_value>  sfRotationValue();
	public: boost::shared_ptr<field_value>  sfStringValue();
	public: boost::shared_ptr<field_value>  sfTimeValue();
	public: boost::shared_ptr<field_value>  sfVec2fValue();
	public: boost::shared_ptr<field_value>  sfVec2dValue();
	public: boost::shared_ptr<field_value>  sfVec3fValue();
	public: boost::shared_ptr<field_value>  sfVec3dValue();
	public: boost::shared_ptr<field_value>  mfColorValue();
	public: boost::shared_ptr<field_value>  mfFloatValue();
	public: boost::shared_ptr<field_value>  mfDoubleValue();
	public: boost::shared_ptr<field_value>  mfInt32Value();
	public: boost::shared_ptr<field_value>  mfRotationValue();
	public: boost::shared_ptr<field_value>  mfStringValue();
	public: boost::shared_ptr<field_value>  mfTimeValue();
	public: boost::shared_ptr<field_value>  mfVec2fValue();
	public: boost::shared_ptr<field_value>  mfVec2dValue();
	public: boost::shared_ptr<field_value>  mfVec3fValue();
	public: boost::shared_ptr<field_value>  mfVec3dValue();
	public: double  doubleValue();
	public: void vec2dValue(
		vec2d & v
	);
	public: void vec3dValue(
		vec3d & v
	);
	public: boost::intrusive_ptr<openvrml::node>  nodeStatement(
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
	public: boost::intrusive_ptr<openvrml::node>  node(
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
          std::vector<boost::intrusive_ptr<openvrml::node> > & impl_nodes,
          proto_node_class::is_map_t & is_map,
          proto_node_class::routes_t & routes
	);
	public: node_interface::type_id  eventInterfaceType();
	public: node_interface::type_id  fieldInterfaceType();
	public: boost::shared_ptr<field_value>  fieldValue(
		openvrml::browser & browser,
           const boost::shared_ptr<openvrml::scope> & scope,
           const openvrml::field_value::type_id ft,
           const std::string & node_id
	);
	public: boost::intrusive_ptr<openvrml::node>  protoNodeStatement(
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
                   std::vector<boost::intrusive_ptr<openvrml::node> > & impl_nodes,
                   proto_node_class::is_map_t & is_map,
                   proto_node_class::routes_t & routes
	);
	public: void protoRouteStatement(
		const openvrml::scope & scope,
                    proto_node_class::routes_t & routes
	);
	public: boost::intrusive_ptr<openvrml::node>  protoNode(
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
	public: boost::shared_ptr<field_value>  protoNodeFieldValue(
		openvrml::browser & browser,
                    const boost::shared_ptr<openvrml::scope> & scope,
                    const node_interface_set & proto_interfaces,
                    proto_node_class::is_map_t & is_map,
                    proto_node_class::routes_t & routes,
                    field_value::type_id ft,
                    const std::string & script_node_id
	);
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
	public: void rotationValue(
		rotation & r
	);
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
	static const int NUM_TOKENS = 69;
#else
	enum {
		NUM_TOKENS = 69
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
	static const unsigned long _tokenSet_8_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_8;
	static const unsigned long _tokenSet_9_data_[];
	static const ANTLR_USE_NAMESPACE(antlr)BitSet _tokenSet_9;
};

#endif /*INC_X3DVrmlParser_hpp_*/
