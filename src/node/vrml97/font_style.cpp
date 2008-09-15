// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
// Copyright 2002  S. K. Bose
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <boost/array.hpp>
# include <openvrml/node_impl_util.h>
# include <private.h>
# include "font_style.h"

namespace {

    class OPENVRML_LOCAL font_style_node :
        public openvrml::node_impl_util::abstract_node<font_style_node>,
        public openvrml::font_style_node {

        friend class openvrml_node_vrml97::font_style_metatype;

        openvrml::mfstring family_;
        openvrml::sfbool horizontal_;
        openvrml::mfstring justify_;
        openvrml::sfstring language_;
        openvrml::sfbool leftToRight;
        openvrml::sffloat size_;
        openvrml::sffloat spacing_;
        openvrml::sfstring style_;
        openvrml::sfbool topToBottom;

    public:
        font_style_node(const openvrml::node_type & type,
                        const boost::shared_ptr<openvrml::scope> & scope);
        virtual ~font_style_node() OPENVRML_NOTHROW;

        //
        // font_style_node implementation
        //
        virtual const std::vector<std::string> & family() const OPENVRML_NOTHROW;
        virtual bool horizontal() const OPENVRML_NOTHROW;
        virtual const std::vector<std::string> & justify() const OPENVRML_NOTHROW;
        virtual const std::string & language() const OPENVRML_NOTHROW;
        virtual bool left_to_right() const OPENVRML_NOTHROW;
        virtual float size() const OPENVRML_NOTHROW;
        virtual float spacing() const OPENVRML_NOTHROW;
        virtual const std::string & style() const OPENVRML_NOTHROW;
        virtual bool top_to_bottom() const OPENVRML_NOTHROW;
    };

    /**
     * @class font_style_node
     *
     * @brief FontStyle node instances.
     */

    /**
     * @var class font_style_node::font_style_metatype
     *
     * @brief Class object for FontStyle nodes.
     */

    /**
     * @var openvrml::mfstring font_style_node::family_
     *
     * @brief family field.
     */

    /**
     * @var openvrml::sfbool font_style_node::horizontal_
     *
     * @brief horizontal field.
     */

    /**
     * @var openvrml::mfstring font_style_node::justify_
     *
     * @brief justify field.
     */

    /**
     * @var openvrml::sfstring font_style_node::language_
     *
     * @brief language field.
     */

    /**
     * @var openvrml::sfbool font_style_node::leftToRight
     *
     * @brief leftToRight field.
     */

    /**
     * @var openvrml::sffloat font_style_node::size_
     *
     * @brief size field.
     */

    /**
     * @var openvrml::sffloat font_style_node::spacing_
     *
     * @brief spacing field.
     */

    /**
     * @var openvrml::sfstring font_style_node::style_
     *
     * @brief style field.
     */

    /**
     * @var openvrml::sfbool font_style_node::topToBottom
     *
     * @brief topToBottom field.
     */

    const std::string fontStyleInitFamily_[] = { "SERIF" };
    const std::string fontStyleInitJustify_[] = { "BEGIN", "FIRST" };

    /**
     * @brief Construct.
     *
     * @param type  the node_type associated with the node instance.
     * @param scope the scope to which the node belongs.
     */
    font_style_node::
    font_style_node(const openvrml::node_type & type,
                    const boost::shared_ptr<openvrml::scope> & scope):
        node(type, scope),
        openvrml::node_impl_util::abstract_node<font_style_node>(type, scope),
        openvrml::font_style_node(type, scope),
        family_(std::vector<std::string>(fontStyleInitFamily_,
                                         fontStyleInitFamily_ + 1)),
        horizontal_(true),
        justify_(std::vector<std::string>(fontStyleInitJustify_,
                                          fontStyleInitJustify_ + 2)),
        leftToRight(true),
        size_(1.0),
        spacing_(1.0),
        style_("PLAIN"),
        topToBottom(true)
    {}

    /**
     * @brief Destroy.
     */
    font_style_node::~font_style_node() OPENVRML_NOTHROW
    {}

    /**
     * @brief Get the list of font families.
     *
     * @return an mfstring containing the font families that may be used for
     *         this <code>font_style_node</code>.
     */
    const std::vector<std::string> &
    font_style_node::family() const OPENVRML_NOTHROW
    {
        return this->family_.value();
    }

    /**
     * @brief Get the flag indicating whether the text should be horizontal or
     *        vertical.
     *
     * @return @c true if the text should be horizontal, or @c false if the
     *         text should be vertical.
     */
    bool font_style_node::horizontal() const OPENVRML_NOTHROW
    {
        return this->horizontal_.value();
    }

    /**
     * @brief Get the descriptor for the text justification.
     *
     * @return a string array describing the characteristics of the text
     *         justification.
     */
    const std::vector<std::string> &
    font_style_node::justify() const OPENVRML_NOTHROW
    {
        return this->justify_.value();
    }

    /**
     * @brief Get the language code.
     *
     * @return the language code.
     */
    const std::string & font_style_node::language() const
        OPENVRML_NOTHROW
    {
        return this->language_.value();
    }

    /**
     * @brief Get the flag indicating whether the text should be rendered
     *      left-to-right.
     *
     * @return @c true if the text should be rendered left-to-right, or
     *         @c false if the text should be rendered right-to-left.
     */
    bool font_style_node::left_to_right() const OPENVRML_NOTHROW
    {
        return this->leftToRight.value();
    }

    /**
     * @brief Get the size of the text.
     *
     * @return the size of the text.
     */
    float font_style_node::size() const OPENVRML_NOTHROW
    {
        return this->size_.value();
    }

    /**
     * @brief Get the spacing for the text.
     *
     * @return the spacing for the text.
     */
    float font_style_node::spacing() const OPENVRML_NOTHROW
    {
        return this->spacing_.value();
    }

    /**
     * @brief Get the style for the text.
     *
     * @return an string descriptor of the text style.
     */
    const std::string & font_style_node::style() const
        OPENVRML_NOTHROW
    {
        return this->style_.value();
    }

    /**
     * @brief Get the flag indicating whether the text should be rendered
     *      top-to-bottom.
     *
     * @return @c true if the text should be rendered top-to-bottom, or
     *         @c false if the text should be rendered bottom-to-top.
     */
    bool font_style_node::top_to_bottom() const OPENVRML_NOTHROW
    {
        return this->topToBottom.value();
    }
}


/**
 * @brief @c node_metatype identifier.
 */
const char * const openvrml_node_vrml97::font_style_metatype::id =
    "urn:X-openvrml:node:FontStyle";

/**
 * @brief Construct.
 *
 * @param browser the @c browser associated with this class object.
 */
openvrml_node_vrml97::font_style_metatype::
font_style_metatype(openvrml::browser & browser):
    node_metatype(font_style_metatype::id, browser)
{}

/**
 * @brief Destroy.
 */
openvrml_node_vrml97::font_style_metatype::~font_style_metatype()
    OPENVRML_NOTHROW
{}

/**
 * @brief Create a node_type.
 *
 * @param id            the name for the new node_type.
 * @param interfaces    the interfaces for the new node_type.
 *
 * @return a boost::shared_ptr<node_type> to a node_type capable of
 *         creating FontStyle nodes.
 *
 * @exception openvrml::unsupported_interface if @p interfaces includes an interface
 *                                  not supported by font_style_metatype.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const boost::shared_ptr<openvrml::node_type>
openvrml_node_vrml97::font_style_metatype::
do_create_type(const std::string & id,
               const openvrml::node_interface_set & interfaces) const
    OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc)
{
    using openvrml::field_value;
    using openvrml::sfbool;
    using openvrml::sffloat;
    using openvrml::sfnode;
    using openvrml::sfstring;
    using openvrml::mfstring;
    using openvrml::node_interface;
    using openvrml::node_interface_set;
    using openvrml::node_type;
    using openvrml::unsupported_interface;
    using namespace openvrml::node_impl_util;

    typedef boost::array<node_interface, 10> supported_interfaces_t;
    static const supported_interfaces_t supported_interfaces = {
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
                       "topToBottom"),
        node_interface(node_interface::exposedfield_id,
                       field_value::sfnode_id,
                       "metadata")
    };

    typedef openvrml::node_impl_util::node_type_impl<font_style_node>
        node_type_t;

    const boost::shared_ptr<node_type> type(new node_type_t(*this, id));
    node_type_t & fontStyleNodeType = static_cast<node_type_t &>(*type);
    for (node_interface_set::const_iterator interface_(interfaces.begin());
         interface_ != interfaces.end();
         ++interface_) {
        supported_interfaces_t::const_iterator supported_interface =
            supported_interfaces.begin() - 1;
        if (*interface_ == *++supported_interface) {
            fontStyleNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &font_style_node::family_);
        } else if (*interface_ == *++supported_interface) {
            fontStyleNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &font_style_node::horizontal_);
        } else if (*interface_ == *++supported_interface) {
            fontStyleNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &font_style_node::justify_);
        } else if (*interface_ == *++supported_interface) {
            fontStyleNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &font_style_node::language_);
        } else if (*interface_ == *++supported_interface) {
            fontStyleNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &font_style_node::leftToRight);
        } else if (*interface_ == *++supported_interface) {
            fontStyleNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &font_style_node::size_);
        } else if (*interface_ == *++supported_interface) {
            fontStyleNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &font_style_node::spacing_);
        } else if (*interface_ == *++supported_interface) {
            fontStyleNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &font_style_node::style_);
        } else if (*interface_ == *++supported_interface) {
            fontStyleNodeType.add_field(
                supported_interface->field_type,
                supported_interface->id,
                &font_style_node::topToBottom);
        } else if (*interface_ == *++supported_interface) {
            fontStyleNodeType.add_exposedfield(
                supported_interface->field_type,
                supported_interface->id,
                &font_style_node::metadata);
        } else {
            throw unsupported_interface(*interface_);
        }
    }
    return type;
}
