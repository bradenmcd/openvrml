// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// pretty-print
//
// Copyright 2007  Braden McDaniel
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <openvrml/vrml97_grammar.h>
# include <fstream>

using namespace std;
using namespace boost::spirit;
using namespace openvrml;

class indent;

std::ostream & operator<<(std::ostream & out, const indent & indent_);

class indent {
    friend std::ostream & operator<<(std::ostream & out, const indent & indent_);

    const std::string increment_;
    std::vector<std::string> indent_;

public:
    explicit indent(const std::string & increment):
        increment_(increment)
    {}

    indent & operator++()
    {
        this->indent_.push_back(this->increment_);
        return *this;
    }

    indent & operator--()
    {
        this->indent_.pop_back();
        return *this;
    }
};

std::ostream & operator<<(std::ostream & out, const indent & indent_)
{
    std::copy(indent_.indent_.begin(),
              indent_.indent_.end(),
              std::ostream_iterator<std::string>(out));
    return out;
}

struct actions;

template <typename MFValueType>
struct on_mfvalue {
    explicit on_mfvalue(actions & a):
        a(a)
    {}

    void operator()(const std::vector<MFValueType> & val) const;

private:
    actions & a;
};

template <>
struct on_mfvalue<std::string> {
    explicit on_mfvalue(actions & a):
        a(a)
    {}

    void operator()(const std::vector<std::string> & val) const;

private:
    actions & a;
};

struct actions : openvrml::null_vrml_parse_actions {

    template <typename SFValueType>
    struct on_sfvalue {
        explicit on_sfvalue(actions & a):
            a(a)
        {}

        void operator()(const SFValueType & val) const
        {
            a.out << ' ' << val << '\n';
        }

    private:
        actions & a;
    };

    std::ostream & out;
    indent indent_;

    explicit actions(std::ostream & out):
        out(out),
        indent_("  "),
        on_externproto(*this),
        on_proto_start(*this),
        on_proto_interface(*this),
        on_proto_body_start(*this),
        on_proto_finish(*this),
        on_node_start(*this),
        on_node_finish(*this),
        on_script_interface_decl(*this),
        on_route(*this),
        on_is_mapping(*this),
        on_field_start(*this),
        on_sfnode(*this),
        on_sfbool(*this),
        on_sfcolor(*this),
        on_sffloat(*this),
        on_sfimage(*this),
        on_sfint32(*this),
        on_sfrotation(*this),
        on_sfstring(*this),
        on_sftime(*this),
        on_sfvec2f(*this),
        on_sfvec3f(*this),
        on_mfcolor(*this),
        on_mffloat(*this),
        on_mfint32(*this),
        on_mfrotation(*this),
        on_mfstring(*this),
        on_mftime(*this),
        on_mfvec2f(*this),
        on_mfvec3f(*this)
    {}

    struct on_externproto_t {
        explicit on_externproto_t(actions & a):
            a(a)
        {}

        void operator()(const std::string & node_type_id,
                        const node_interface_set & interfaces,
                        const std::vector<std::string> & uri_list) const
        {
            using std::string;
            using std::vector;
            a.out << a.indent_ << "EXTERNPROTO " << node_type_id << " [\n";
            ++a.indent_;
            for (node_interface_set::const_iterator interface =
                     interfaces.begin();
                 interface != interfaces.end();
                 ++interface) {
                a.out << a.indent_ << *interface << '\n';
            }
            --a.indent_;
            a.out << a.indent_ << "] [\n";
            ++a.indent_;
            for (vector<string>::const_iterator uri = uri_list.begin();
                 uri != uri_list.end();
                 ++uri) {
                a.out << a.indent_ << '"' << *uri << '"';
                if (uri + 1 != uri_list.end()) {
                    a.out << ',';
                }
                a.out << '\n';
            }
            --a.indent_;
            a.out << a.indent_ << "]\n";
        }

        actions & a;
    } on_externproto;

    struct on_proto_start_t {
        explicit on_proto_start_t(actions & a):
            a(a)
        {}

        void operator()(const std::string & node_type_id) const
        {
            a.out << "PROTO " << node_type_id << " [\n";
            ++a.indent_;
        }

        actions & a;
    } on_proto_start;

    struct on_proto_interface_t {
        explicit on_proto_interface_t(actions & a):
            a(a)
        {}

        void operator()(const node_interface & interface) const
        {
            a.out << a.indent_ << interface;
            if (interface.type == node_interface::eventin_id
                || interface.type == node_interface::eventout_id) {
                a.out << '\n';
            }
        }

        actions & a;
    } on_proto_interface;

    struct on_proto_body_start_t {
        explicit on_proto_body_start_t(actions & a):
            a(a)
        {}

        void operator()() const
        {
            --a.indent_;
            a.out << a.indent_ << "] {\n";
            ++a.indent_;
        }

        actions & a;
    } on_proto_body_start;

    struct on_proto_finish_t {
        explicit on_proto_finish_t(actions & a):
            a(a)
        {}

        void operator()() const
        {
            --a.indent_;
            a.out << a.indent_ << "}\n";
        }

        actions & a;
    } on_proto_finish;

    struct on_node_start_t {
        explicit on_node_start_t(actions & a):
            a(a)
        {}

        void operator()(const std::string & node_name_id,
                        const std::string & node_type_id) const
        {
            a.out << '\n' << a.indent_;
            if (!node_name_id.empty()) {
                a.out << "DEF " << node_name_id << ' ';
            }
            a.out << node_type_id << " {\n";
            ++a.indent_;
        }

        actions & a;
    } on_node_start;

    struct on_node_finish_t {
        explicit on_node_finish_t(actions & a):
            a(a)
        {}

        void operator()() const
        {
            --a.indent_;
            a.out << a.indent_ << "}\n";
        }

        actions & a;
    } on_node_finish;

    struct on_script_interface_decl_t {
        explicit on_script_interface_decl_t(actions & a):
            a(a)
        {}

        void operator()(const node_interface & interface) const
        {
            a.out << a.indent_ << interface;
        }

        actions & a;
    } on_script_interface_decl;

    struct on_route_t {
        explicit on_route_t(actions & a):
            a(a)
        {}

        void operator()(const std::string & from_node_name_id,
                        const node_interface & from_node_interface,
                        const std::string & to_node_name_id,
                        const node_interface & to_node_interface) const
        {
            a.out << a.indent_ << "ROUTE "
                  << from_node_name_id << '.' << from_node_interface.id
                  << " TO "
                  << to_node_name_id << '.' << to_node_interface.id << '\n';
        }

        actions & a;
    } on_route;

    struct on_is_mapping_t {
        explicit on_is_mapping_t(actions & a):
            a(a)
        {}

        void operator()(const std::string & proto_interface_id) const
        {
            a.out << " IS " << proto_interface_id << '\n';
        }

        actions & a;
    } on_is_mapping;

    struct on_field_start_t {
        explicit on_field_start_t(actions & a):
            a(a)
        {}

        void operator()(const std::string & field_name_id,
                        field_value::type_id /* field_type */) const
        {
            a.out << a.indent_ << field_name_id;
        }

        actions & a;
    } on_field_start;

    struct on_sfnode_t {
        explicit on_sfnode_t(actions & a):
            a(a)
        {}

        void operator()(const bool null) const
        {
            if (null) { a.out << " NULL\n"; }
        }

        actions & a;
    } on_sfnode;

    struct on_sfbool_t {
        explicit on_sfbool_t(actions & a):
            a(a)
        {}

        void operator()(const bool val) const
        {
            a.out << ' ' << (val ? "TRUE" : "FALSE") << '\n';
        }

        actions & a;
    } on_sfbool;

    on_sfvalue<color> on_sfcolor;
    on_sfvalue<float> on_sffloat;
    on_sfvalue<image> on_sfimage;
    on_sfvalue<int32> on_sfint32;
    on_sfvalue<rotation> on_sfrotation;
    on_sfvalue<string> on_sfstring;
    on_sfvalue<double> on_sftime;
    on_sfvalue<vec2f> on_sfvec2f;
    on_sfvalue<vec3f> on_sfvec3f;
    on_mfvalue<color> on_mfcolor;
    on_mfvalue<float> on_mffloat;
    on_mfvalue<int32> on_mfint32;
    on_mfvalue<rotation> on_mfrotation;
    on_mfvalue<string> on_mfstring;
    on_mfvalue<double> on_mftime;
    on_mfvalue<vec2f> on_mfvec2f;
    on_mfvalue<vec3f> on_mfvec3f;
};

void init_vrml97_node_types(node_type_decls & node_types);

int main(int argc, char * argv[])
{
    ifstream infile;
    if (argc > 1) {
        infile.open(argv[1]);
        if (!infile.is_open()) {
            cerr << argv[0] << ": could not open file \"" << argv[1] << endl;
            return EXIT_FAILURE;
        }
    }

    istream & in(infile.is_open() ? infile : cin);
    const string filename(infile.is_open() ? argv[1] : "<stdin>");

    typedef skip_parser_iteration_policy<vrml97_skip_grammar> iterator_policy_t;
    typedef scanner_policies<iterator_policy_t> scanner_policies_t;
    typedef multi_pass<istreambuf_iterator<char> > multi_pass_iterator_t;
    typedef scanner<multi_pass_iterator_t, scanner_policies_t> scanner_t;
    typedef istream::char_type char_t;

    vrml97_skip_grammar skip_g;
    iterator_policy_t iterator_policy(skip_g);
    scanner_policies_t scanner_policies(iterator_policy);
    multi_pass_iterator_t
        in_begin(make_multi_pass(istreambuf_iterator<char_t>(in))),
        in_end(make_multi_pass(istreambuf_iterator<char_t>()));

    typedef position_iterator<multi_pass_iterator_t> iterator_t;

    iterator_t first(in_begin, in_end, filename), last;

    node_type_decls node_types;
    init_vrml97_node_types(node_types);
    actions act(std::cout);
    vrml_parse_error_handler handler(std::cerr);
    vrml97_grammar<vrml_parse_error_handler, actions> g(node_types,
                                                        act,
                                                        handler);

    BOOST_SPIRIT_DEBUG_NODE(skip_g);
    BOOST_SPIRIT_DEBUG_NODE(g);

    guard<vrml_parse_error> guard;

    if (!parse(first, last, guard(g)[handler], skip_g).full) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

template <typename MFValueType>
void
on_mfvalue<MFValueType>::operator()(const std::vector<MFValueType> & val) const
{
    a.out << " [\n";
    ++a.indent_;
    for (typename std::vector<MFValueType>::const_iterator element =
             val.begin();
         element != val.end();
         ++element) {
        a.out << a.indent_ << *element << ",\n";
    }
    --a.indent_;
    a.out << a.indent_ << "]\n";
}

void
on_mfvalue<std::string>::operator()(const std::vector<std::string> & val) const
{
    a.out << " [\n";
    ++a.indent_;
    for (std::vector<std::string>::const_iterator element =
             val.begin();
         element != val.end();
         ++element) {
        a.out << a.indent_ << '"' << *element << "\",\n";
    }
    --a.indent_;
    a.out << a.indent_ << "]\n";
}


void init_vrml97_node_types(node_type_decls & node_types)
{
    //
    // Anchor node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "description"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "parameter"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "url"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 8);
        const bool succeeded =
            node_types.insert(make_pair("Anchor", interface_set)).second;
        assert(succeeded);
    }

    //
    // Appearance node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "material"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "texture"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "textureTransform")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 3);
        const bool succeeded =
            node_types.insert(make_pair("Appearance", interface_set)).second;
        assert(succeeded);
    }

    //
    // AudioClip node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "description"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "loop"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "pitch"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sftime_id,
                           "startTime"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sftime_id,
                           "stopTime"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "url"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "duration_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 8);
        const bool succeeded =
            node_types.insert(make_pair("AudioClip", interface_set)).second;
        assert(succeeded);
    }

    //
    // Background node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "set_bind"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "groundAngle"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfcolor_id,
                           "groundColor"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "backUrl"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "bottomUrl"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "frontUrl"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "leftUrl"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "rightUrl"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "topUrl"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "skyAngle"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfcolor_id,
                           "skyColor"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isBound")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 12);
        const bool succeeded =
            node_types.insert(make_pair("Background", interface_set)).second;
        assert(succeeded);
    }

    //
    // Billboard node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "axisOfRotation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 6);
        const bool succeeded =
            node_types.insert(make_pair("Billboard", interface_set)).second;
        assert(succeeded);
    }

    //
    // Box node
    //
    {
        static const node_interface interface =
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "size");
        static const node_interface_set interface_set(&interface,
                                                      &interface + 1);
        const bool succeeded =
            node_types.insert(make_pair("Box", interface_set)).second;
        assert(succeeded);
    }

    //
    // Collision node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "collide"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize"),
            node_interface(node_interface::field_id,
                           field_value::sfnode_id,
                           "proxy"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "collideTime")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 8);
        const bool succeeded =
            node_types.insert(make_pair("Collision", interface_set)).second;
        assert(succeeded);
    }

    //
    // Color node
    //
    {
        static const node_interface interface =
            node_interface(node_interface::exposedfield_id,
                           field_value::mfcolor_id,
                           "color");
        static const node_interface_set interface_set(&interface,
                                                      &interface + 1);
        const bool succeeded =
            node_types.insert(make_pair("Color", interface_set)).second;
        assert(succeeded);
    }

    //
    // ColorInterpolator node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "key"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfcolor_id,
                           "keyValue"),
            node_interface(node_interface::eventout_id,
                           field_value::sfcolor_id,
                           "value_changed")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 4);
        const bool succeeded =
            node_types.insert(make_pair("ColorInterpolator", interface_set)).second;
        assert(succeeded);
    }

    //
    // Cone node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "bottomRadius"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "height"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "side"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "bottom")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 4);
        const bool succeeded =
            node_types.insert(make_pair("Cone", interface_set)).second;
        assert(succeeded);
    }

    //
    // Coordinate node
    //
    {
        static const node_interface interface =
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3f_id,
                           "point");
        static const node_interface_set interface_set(&interface,
                                                      &interface + 1);
        const bool succeeded =
            node_types.insert(make_pair("Coordinate", interface_set)).second;
        assert(succeeded);
    }

    //
    // CoordinateInterpolator node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "key"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3f_id,
                           "keyValue"),
            node_interface(node_interface::eventout_id,
                           field_value::mfvec3f_id,
                           "value_changed")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 4);
        const bool succeeded =
            node_types.insert(make_pair("CoordinateInterpolator",
                                        interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Cylinder node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "bottom"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "height"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "radius"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "side"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "top")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 5);
        const bool succeeded =
            node_types.insert(make_pair("Cylinder", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // CylinderSensor node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "autoOffset"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "diskAngle"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "enabled"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "maxAngle"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "minAngle"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "offset"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive"),
            node_interface(node_interface::eventout_id,
                           field_value::sfrotation_id,
                           "rotation_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "trackPoint_changed")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 9);
        const bool succeeded =
            node_types.insert(make_pair("CylinderSensor", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // DirectionalLight node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "ambientIntensity"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfcolor_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "direction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "intensity"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "on")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 5);
        const bool succeeded =
            node_types.insert(make_pair("DirectionalLight", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // ElevationGrid node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::mffloat_id,
                           "set_height"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "normal"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "texCoord"),
            node_interface(node_interface::field_id,
                           field_value::mffloat_id,
                           "height"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "ccw"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "colorPerVertex"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "creaseAngle"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "normalPerVertex"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "xDimension"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "xSpacing"),
            node_interface(node_interface::field_id,
                           field_value::sfint32_id,
                           "zDimension"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "zSpacing")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 14);
        const bool succeeded =
            node_types.insert(make_pair("ElevationGrid", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Extrusion node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::mfvec2f_id,
                           "set_crossSection"),
            node_interface(node_interface::eventin_id,
                           field_value::mfrotation_id,
                           "set_orientation"),
            node_interface(node_interface::eventin_id,
                           field_value::mfvec2f_id,
                           "set_scale"),
            node_interface(node_interface::eventin_id,
                           field_value::mfvec3f_id,
                           "set_spine"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "beginCap"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "ccw"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "convex"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "creaseAngle"),
            node_interface(node_interface::field_id,
                           field_value::mfvec2f_id,
                           "crossSection"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "endCap"),
            node_interface(node_interface::field_id,
                           field_value::mfrotation_id,
                           "orientation"),
            node_interface(node_interface::field_id,
                           field_value::mfvec2f_id,
                           "scale"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid"),
            node_interface(node_interface::field_id,
                           field_value::mfvec3f_id,
                           "spine")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 14);
        const bool succeeded =
            node_types.insert(make_pair("Extrusion", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Fog node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "set_bind"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfcolor_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfstring_id,
                           "fogType"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "visibilityRange"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isBound")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 5);
        const bool succeeded =
            node_types.insert(make_pair("Fog", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // FontStyle node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "family"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "horizontal"),
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "justify"),
            node_interface(node_interface::field_id,
                           field_value::sfstring_id,
                           "language"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "leftToRight"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "size"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "spacing"),
            node_interface(node_interface::field_id,
                           field_value::sfstring_id,
                           "style"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "topToBottom")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 9);
        const bool succeeded =
            node_types.insert(make_pair("FontStyle", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Group node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 5);
        const bool succeeded =
            node_types.insert(make_pair("Group", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // ImageTexture node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "url"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "repeatS"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "repeatT")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 3);
        const bool succeeded =
            node_types.insert(make_pair("ImageTexture", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // IndexedFaceSet node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::mfint32_id,
                           "set_colorIndex"),
            node_interface(node_interface::eventin_id,
                           field_value::mfint32_id,
                           "set_coordIndex"),
            node_interface(node_interface::eventin_id,
                           field_value::mfint32_id,
                           "set_normalIndex"),
            node_interface(node_interface::eventin_id,
                           field_value::mfint32_id,
                           "set_texCoordIndex"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "coord"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "normal"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "texCoord"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "ccw"),
            node_interface(node_interface::field_id,
                           field_value::mfint32_id,
                           "colorIndex"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "colorPerVertex"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "convex"),
            node_interface(node_interface::field_id,
                           field_value::mfint32_id,
                           "coordIndex"),
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "creaseAngle"),
            node_interface(node_interface::field_id,
                           field_value::mfint32_id,
                           "normalIndex"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "normalPerVertex"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "solid"),
            node_interface(node_interface::field_id,
                           field_value::mfint32_id,
                           "texCoordIndex")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 18);
        const bool succeeded =
            node_types.insert(make_pair("IndexedFaceSet", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // IndexedLineSet node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::mfint32_id,
                           "set_colorIndex"),
            node_interface(node_interface::eventin_id,
                           field_value::mfint32_id,
                           "set_coordIndex"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "coord"),
            node_interface(node_interface::field_id,
                           field_value::mfint32_id,
                           "colorIndex"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "colorPerVertex"),
            node_interface(node_interface::field_id,
                           field_value::mfint32_id,
                           "coordIndex")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 7);
        const bool succeeded =
            node_types.insert(make_pair("IndexedLineSet", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Inline node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "url"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 3);
        const bool succeeded =
            node_types.insert(make_pair("Inline", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // LOD node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "level"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "center"),
            node_interface(node_interface::field_id,
                           field_value::mffloat_id,
                           "range")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 3);
        const bool succeeded =
            node_types.insert(make_pair("LOD", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Material node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "ambientIntensity"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfcolor_id,
                           "diffuseColor"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfcolor_id,
                           "emissiveColor"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "shininess"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfcolor_id,
                           "specularColor"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "transparency")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 6);
        const bool succeeded =
            node_types.insert(make_pair("Material", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // MovieTexture node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "loop"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "speed"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sftime_id,
                           "startTime"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sftime_id,
                           "stopTime"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "url"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "repeatS"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "repeatT"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "duration_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 9);
        const bool succeeded =
            node_types.insert(make_pair("MovieTexture", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // NavigationInfo node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "set_bind"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "avatarSize"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "headlight"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "speed"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "type"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "visibilityLimit"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isBound")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 7);
        const bool succeeded =
            node_types.insert(make_pair("NavigationInfo", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Normal node
    //
    {
        static const node_interface interface =
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3f_id,
                           "vector");
        static const node_interface_set interface_set(&interface,
                                                      &interface + 1);
        const bool succeeded =
            node_types.insert(make_pair("Normal", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // NormalInterpolator node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "key"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3f_id,
                           "keyValue"),
            node_interface(node_interface::eventout_id,
                           field_value::mfvec3f_id,
                           "value_changed")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 4);
        const bool succeeded =
            node_types.insert(make_pair("NormalInterpolator", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // OrientationInterpolator node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "key"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfrotation_id,
                           "keyValue"),
            node_interface(node_interface::eventout_id,
                           field_value::sfrotation_id,
                           "value_changed")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 4);
        const bool succeeded =
            node_types.insert(make_pair("OrientationInterpolator",
                                        interface_set))
            .second;
        assert(succeeded);
    }

    //
    // PixelTexture node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfimage_id,
                           "image"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "repeatS"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "repeatT")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 3);
        const bool succeeded =
            node_types.insert(make_pair("PixelTexture", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // PlaneSensor node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "autoOffset"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "enabled"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec2f_id,
                           "maxPosition"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec2f_id,
                           "minPosition"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "offset"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "trackPoint_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "translation_changed")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 8);
        const bool succeeded =
            node_types.insert(make_pair("PlaneSensor", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // PointLight node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "ambientIntensity"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "attenuation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfcolor_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "intensity"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "location"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "on"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "radius")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 7);
        const bool succeeded =
            node_types.insert(make_pair("PointLight", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // PointSet node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "coord")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 2);
        const bool succeeded =
            node_types.insert(make_pair("PointSet", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // PositionInterpolator node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "key"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec3f_id,
                           "keyValue"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "value_changed")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 4);
        const bool succeeded =
            node_types.insert(make_pair("PositionInterpolator", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // ProximitySensor node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "center"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "size"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "enabled"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "position_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfrotation_id,
                           "orientation_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "enterTime"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "exitTime")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 8);
        const bool succeeded =
            node_types.insert(make_pair("ProximitySensor", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // ScalarInterpolator node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::sffloat_id,
                           "set_fraction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "key"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "keyValue"),
            node_interface(node_interface::eventout_id,
                           field_value::sffloat_id,
                           "value_changed")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 4);
        const bool succeeded =
            node_types.insert(make_pair("ScalarInterpolator", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Shape node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "appearance"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "geometry")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 2);
        const bool succeeded =
            node_types.insert(make_pair("Shape", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Sound node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "direction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "intensity"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "location"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "maxBack"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "maxFront"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "minBack"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "minFront"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "priority"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "source"),
            node_interface(node_interface::field_id,
                           field_value::sfbool_id,
                           "spatialize")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 10);
        const bool succeeded =
            node_types.insert(make_pair("Sound", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Sphere node
    //
    {
        static const node_interface interface =
            node_interface(node_interface::field_id,
                           field_value::sffloat_id,
                           "radius");
        static const node_interface_set interface_set(&interface,
                                                      &interface + 1);
        const bool succeeded =
            node_types.insert(make_pair("Sphere", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // SphereSensor node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "autoOffset"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "enabled"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "offset"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive"),
            node_interface(node_interface::eventout_id,
                           field_value::sfrotation_id,
                           "rotation_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "trackPoint_changed")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 6);
        const bool succeeded =
            node_types.insert(make_pair("SphereSensor", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // SpotLight node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "ambientIntensity"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "attenuation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "beamWidth"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfcolor_id,
                           "color"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "cutOffAngle"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "direction"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "intensity"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "location"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "on"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "radius")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 10);
        const bool succeeded =
            node_types.insert(make_pair("SpotLight", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Switch node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "choice"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfint32_id,
                           "whichChoice")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 2);
        const bool succeeded =
            node_types.insert(make_pair("Switch", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Text node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::mfstring_id,
                           "string"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfnode_id,
                           "fontStyle"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mffloat_id,
                           "length"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "maxExtent")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 4);
        const bool succeeded =
            node_types.insert(make_pair("Text", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // TextureCoordinate node
    //
    {
        static const node_interface interface =
            node_interface(node_interface::exposedfield_id,
                           field_value::mfvec2f_id,
                           "point");
        static const node_interface_set interface_set(&interface,
                                                      &interface + 1);
        const bool succeeded =
            node_types.insert(make_pair("TextureCoordinate", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // TextureTransform node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec2f_id,
                           "center"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "rotation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec2f_id,
                           "scale"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec2f_id,
                           "translation")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 4);
        const bool succeeded =
            node_types.insert(make_pair("TextureTransform", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // TimeSensor node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sftime_id,
                           "cycleInterval"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "enabled"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "loop"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sftime_id,
                           "startTime"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sftime_id,
                           "stopTime"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "cycleTime"),
            node_interface(node_interface::eventout_id,
                           field_value::sffloat_id,
                           "fraction_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "time")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 9);
        const bool succeeded =
            node_types.insert(make_pair("TimeSensor", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // TouchSensor node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "enabled"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "hitNormal_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec3f_id,
                           "hitPoint_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfvec2f_id,
                           "hitTexCoord_changed"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isOver"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "touchTime")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 7);
        const bool succeeded =
            node_types.insert(make_pair("TouchSensor", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Transform node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "addChildren"),
            node_interface(node_interface::eventin_id,
                           field_value::mfnode_id,
                           "removeChildren"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "center"),
            node_interface(node_interface::exposedfield_id,
                           field_value::mfnode_id,
                           "children"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "rotation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "scale"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "scaleOrientation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "translation"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxCenter"),
            node_interface(node_interface::field_id,
                           field_value::sfvec3f_id,
                           "bboxSize")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 10);
        const bool succeeded =
            node_types.insert(make_pair("Transform", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // Viewpoint node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::eventin_id,
                           field_value::sfbool_id,
                           "set_bind"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sffloat_id,
                           "fieldOfView"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "jump"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfrotation_id,
                           "orientation"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "position"),
            node_interface(node_interface::field_id,
                           field_value::sfstring_id,
                           "description"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "bindTime"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isBound")
        };
        static const node_interface_set
            interface_set(interfaces, interfaces + 8);
        const bool succeeded =
            node_types.insert(make_pair("Viewpoint", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // VisibilitySensor node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "center"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfbool_id,
                           "enabled"),
            node_interface(node_interface::exposedfield_id,
                           field_value::sfvec3f_id,
                           "size"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "enterTime"),
            node_interface(node_interface::eventout_id,
                           field_value::sftime_id,
                           "exitTime"),
            node_interface(node_interface::eventout_id,
                           field_value::sfbool_id,
                           "isActive")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 6);
        const bool succeeded =
            node_types.insert(make_pair("VisibilitySensor", interface_set))
            .second;
        assert(succeeded);
    }

    //
    // WorldInfo node
    //
    {
        static const node_interface interfaces[] = {
            node_interface(node_interface::field_id,
                           field_value::mfstring_id,
                           "info"),
            node_interface(node_interface::field_id,
                           field_value::sfstring_id,
                           "title")
        };
        static const node_interface_set interface_set(interfaces,
                                                      interfaces + 2);
        const bool succeeded =
            node_types.insert(make_pair("WorldInfo", interface_set))
            .second;
        assert(succeeded);
    }
}
