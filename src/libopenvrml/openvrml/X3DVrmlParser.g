// -*- Mode: Antlr; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 2000, 2001, 2003, 2004  Braden N. McDaniel
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
}

header "pre_include_cpp" {
# include "X3DVrmlParser.hpp"
ANTLR_RBRACE
}

header "post_include_cpp" {
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

} // header "post_include_cpp"

options {
    language="Cpp";
}

class X3DVrmlParser extends Vrml97Parser;
options {
    k=1;
    importVocab=X3DVrml;
}

{
public:
    X3DVrmlParser(antlr::TokenStream & lexer, const std::string & uri):
        antlr::LLkParser(lexer, 1),
        uri(uri)
    {}

private:
    const std::string uri;
}

vrmlScene[openvrml::scene & scene,
          std::vector<boost::intrusive_ptr<openvrml::node> > & nodes]
options { defaultErrorHandler=false; }
{
    profile_id profile = invalid_profile_id;
    std::auto_ptr<openvrml::scope> root_scope_auto_ptr =
        create_root_scope(scene.browser(), this->uri);
    const boost::shared_ptr<openvrml::scope> root_scope(root_scope_auto_ptr);
}
    :   profile=profileStatement { scene.profile_ = profile; }
        (componentStatement)* (metaStatement)*
        (statement[scene, nodes, root_scope])*
    ;

profileStatement
returns [openvrml::profile_id profile = invalid_profile_id]
    :   KEYWORD_PROFILE id:ID {
            std::string profile_id = id->getText();
            // XXX
            // XXX Checks for supported profiles should go here.
            // XXX
            if (false) {
            } else {
                throw antlr::SemanticException("unsupported profile \""
                                               + id->getText() + "\"",
                                               this->uri,
                                               id->getLine(),
                                               id->getColumn());
            }
        }
    ;

componentStatement
{
    size_t level;
}
    :   KEYWORD_COMPONENT ID COLON level=intValue
    ;

metaStatement
{
    std::string key, value;
}
    :   KEYWORD_META key=stringValue value=stringValue
    ;

fieldType
returns [openvrml::field_value::type_id ft =
         openvrml::field_value::invalid_type_id]
options { defaultErrorHandler=false; }
{
    using openvrml::field_value;
}
    :   FIELDTYPE_MFBOOL      { ft = field_value::invalid_type_id; }
    |   FIELDTYPE_MFCOLOR     { ft = field_value::mfcolor_id; }
    |   FIELDTYPE_MFCOLORRGBA { ft = field_value::invalid_type_id; }
    |   FIELDTYPE_MFFLOAT     { ft = field_value::mffloat_id; }
    |   FIELDTYPE_MFDOUBLE    { ft = field_value::mfdouble_id; }
    |   FIELDTYPE_MFIMAGE     { ft = field_value::invalid_type_id; }
    |   FIELDTYPE_MFINT32     { ft = field_value::mfint32_id; }
    |   FIELDTYPE_MFNODE      { ft = field_value::mfnode_id; }
    |   FIELDTYPE_MFROTATION  { ft = field_value::mfrotation_id; }
    |   FIELDTYPE_MFSTRING    { ft = field_value::mfstring_id; }
    |   FIELDTYPE_MFTIME      { ft = field_value::mftime_id; }
    |   FIELDTYPE_MFVEC2F     { ft = field_value::mfvec2f_id; }
    |   FIELDTYPE_MFVEC2D     { ft = field_value::mfvec2d_id; }
    |   FIELDTYPE_MFVEC3F     { ft = field_value::mfvec3f_id; }
    |   FIELDTYPE_MFVEC3D     { ft = field_value::mfvec3d_id; }
    |   FIELDTYPE_SFBOOL      { ft = field_value::sfbool_id; }
    |   FIELDTYPE_SFCOLOR     { ft = field_value::sfcolor_id; }
    |   FIELDTYPE_SFCOLORRGBA { ft = field_value::invalid_type_id; }
    |   FIELDTYPE_SFFLOAT     { ft = field_value::sffloat_id; }
    |   FIELDTYPE_SFDOUBLE    { ft = field_value::sfdouble_id; }
    |   FIELDTYPE_SFIMAGE     { ft = field_value::sfimage_id; }
    |   FIELDTYPE_SFINT32     { ft = field_value::sfint32_id; }
    |   FIELDTYPE_SFNODE      { ft = field_value::sfnode_id; }
    |   FIELDTYPE_SFROTATION  { ft = field_value::sfrotation_id; }
    |   FIELDTYPE_SFSTRING    { ft = field_value::sfstring_id; }
    |   FIELDTYPE_SFTIME      { ft = field_value::sftime_id; }
    |   FIELDTYPE_SFVEC2F     { ft = field_value::sfvec2f_id; }
    |   FIELDTYPE_SFVEC2D     { ft = field_value::sfvec2d_id; }
    |   FIELDTYPE_SFVEC3F     { ft = field_value::sfvec3f_id; }
    |   FIELDTYPE_SFVEC3D     { ft = field_value::sfvec3d_id; }
    ;

nonNodeFieldValue[openvrml::field_value::type_id ft]
returns [boost::shared_ptr<field_value> fv]
options { defaultErrorHandler=false; }
    :   { ft == field_value::sfbool_id }? fv=sfBoolValue
    |   { ft == field_value::sfcolor_id }? fv=sfColorValue
    |   { ft == field_value::sffloat_id }? fv=sfFloatValue
    |   { ft == field_value::sfdouble_id }? fv=sfDoubleValue
    |   { ft == field_value::sfimage_id }? fv=sfImageValue
    |   { ft == field_value::sfint32_id }? fv=sfInt32Value
    |   { ft == field_value::sfrotation_id }? fv=sfRotationValue
    |   { ft == field_value::sfstring_id }? fv=sfStringValue
    |   { ft == field_value::sftime_id }? fv=sfTimeValue
    |   { ft == field_value::sfvec2f_id }? fv=sfVec2fValue
    |   { ft == field_value::sfvec2d_id }? fv=sfVec2dValue
    |   { ft == field_value::sfvec3f_id }? fv=sfVec3fValue
    |   { ft == field_value::sfvec3d_id }? fv=sfVec3dValue
    |   { ft == field_value::mfcolor_id }? fv=mfColorValue
    |   { ft == field_value::mffloat_id }? fv=mfFloatValue
    |   { ft == field_value::mfdouble_id }? fv=mfDoubleValue
    |   { ft == field_value::mfint32_id }? fv=mfInt32Value
    |   { ft == field_value::mfrotation_id }? fv=mfRotationValue
    |   { ft == field_value::mfstring_id }? fv=mfStringValue
    |   { ft == field_value::mftime_id }? fv=mfTimeValue
    |   { ft == field_value::mfvec2f_id }? fv=mfVec2fValue
    |   { ft == field_value::mfvec2d_id }? fv=mfVec2dValue
    |   { ft == field_value::mfvec3f_id }? fv=mfVec3fValue
    |   { ft == field_value::mfvec3d_id }? fv=mfVec3dValue
    ;


sfDoubleValue returns [boost::shared_ptr<field_value> sfv]
options { defaultErrorHandler=false; }
{
    double d;
}
    :   d=doubleValue { sfv.reset(new sfdouble(d)); }
    ;

mfDoubleValue
returns [boost::shared_ptr<field_value> mfv =
         boost::shared_ptr<field_value>(new mfdouble)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    double d;
    mfdouble & doubles = static_cast<mfdouble &>(*mfv);
}
    :   d=doubleValue { doubles.value(vector<double>(1, d)); }
    |   LBRACKET {
            vector<double> value;
        } (d=doubleValue { value.push_back(d); })* RBRACKET {
            doubles.value(value);
        }
    ;

sfVec2dValue returns [boost::shared_ptr<field_value> svv]
options { defaultErrorHandler=false; }
{
    vec2d v;
}
    :   vec2dValue[v] { svv.reset(new sfvec2d(v)); }
    ;

mfVec2dValue
returns [boost::shared_ptr<field_value> mvv =
         boost::shared_ptr<field_value>(new mfvec2d)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    vec2d v;
    mfvec2d & vec2ds = static_cast<mfvec2d &>(*mvv);
}
    :   vec2dValue[v] { vec2ds.value(vector<vec2d>(1, v)); }
    |   LBRACKET {
            vector<vec2d> value;
        } (vec2dValue[v] { value.push_back(v); })* RBRACKET {
            vec2ds.value(value);
        }
    ;

vec2dValue[vec2d & v]
options { defaultErrorHandler=false; }
{
    double x, y;
}
    :   x=doubleValue y=doubleValue {
            v.x(x);
            v.y(y);
        }
    ;

sfVec3dValue returns [boost::shared_ptr<field_value> svv]
options { defaultErrorHandler=false; }
{
    vec3d v;
}
    :   vec3dValue[v] { svv.reset(new sfvec3d(v)); }
    ;

mfVec3dValue
returns [boost::shared_ptr<field_value> mvv =
         boost::shared_ptr<field_value>(new mfvec3d)]
options { defaultErrorHandler=false; }
{
    using std::vector;

    vec3d v;
    mfvec3d & vec3ds = static_cast<mfvec3d &>(*mvv);
}
    :   vec3dValue[v] { vec3ds.value(vector<vec3d>(1, v)); }
    |   LBRACKET {
            vector<vec3d> value;
        } (vec3dValue[v] { value.push_back(v); })* RBRACKET {
            vec3ds.value(value);
        }
    ;

vec3dValue[vec3d & v]
options { defaultErrorHandler=false; }
{
    double x, y, z;
}
    :   x=doubleValue y=doubleValue z=doubleValue {
            v.x(x);
            v.y(y);
            v.z(z);
        }
    ;
