// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// pretty-print
//
// Copyright 2007  Braden McDaniel
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.
//
// You should have received a copy of the GNU General Public License along
// with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <openvrml/vrml97_grammar.h>
# include <openvrml/browser.h>
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

struct actions : openvrml::null_vrml97_parse_actions {

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
        on_use(*this),
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
            for (node_interface_set::const_iterator interface_ =
                     interfaces.begin();
                 interface_ != interfaces.end();
                 ++interface_) {
                a.out << a.indent_ << *interface_ << '\n';
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

        void operator()(const node_interface & interface_) const
        {
            a.out << a.indent_ << interface_;
            if (interface_.type == node_interface::eventin_id
                || interface_.type == node_interface::eventout_id) {
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

        void operator()(const node_interface & interface_) const
        {
            a.out << a.indent_ << interface_;
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

    struct on_use_t {
        explicit on_use_t(actions & a):
            a(a)
        {}

        void operator()(const std::string & node_name_id) const
        {
            a.out << '\n' << a.indent_ << "USE" << node_name_id << '\n';
        }

        actions & a;
    } on_use;

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

    typedef multi_pass<istreambuf_iterator<char> > multi_pass_iterator_t;
    typedef istream::char_type char_t;

    multi_pass_iterator_t
        in_begin(make_multi_pass(istreambuf_iterator<char_t>(in))),
        in_end(make_multi_pass(istreambuf_iterator<char_t>()));

    typedef position_iterator<multi_pass_iterator_t> iterator_t;

    iterator_t first(in_begin, in_end, filename), last;

    vrml97_skip_grammar skip_g;
    actions act(std::cout);
    vrml97_grammar<actions> g(act);

    BOOST_SPIRIT_DEBUG_NODE(skip_g);
    BOOST_SPIRIT_DEBUG_NODE(g);

    if (!parse(first, last, g, skip_g).full) {
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
