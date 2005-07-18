// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004  Braden McDaniel
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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <algorithm>
# include <numeric>
# include <private.h>
# include "field_value.h"
# include "node.h"

/**
 * @defgroup fieldvalues Field Values
 */

/**
 * @brief Stream output.
 *
 * @param out   an output stream.
 * @param value a field value.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out,
                                    const field_value & value)
{
    value.print(out);
    return out;
}

/**
 * @class openvrml::field_value
 *
 * @ingroup fieldvalues
 *
 * @brief Abstract base class for the VRML field types.
 */

/**
 * @enum openvrml::field_value::type_id
 *
 * @brief Used to identify field_value types.
 *
 * These tags are typically used to designate an expected type or to avoid a
 * @c dynamic_cast.
 */

/**
 * @var openvrml::field_value::invalid_type_id
 *
 * @brief Zero value typically used to indicate failure.
 */

/**
 * @var openvrml::field_value::sfbool_id
 *
 * @brief Designates an <code>sfbool</code>.
 */

/**
 * @var openvrml::field_value::sfcolor_id
 *
 * @brief Designates an <code>sfcolor</code>.
 */

/**
 * @var openvrml::field_value::sffloat_id
 *
 * @brief Designates an <code>sffloat</code>.
 */

/**
 * @var openvrml::field_value::sfimage_id
 *
 * @brief Designates an <code>sfimage</code>.
 */

/**
 * @var openvrml::field_value::sfint32_id
 *
 * @brief Designates an <code>sfint32</code>.
 */

/**
 * @var openvrml::field_value::sfnode_id
 *
 * @brief Designates an <code>sfnode</code>.
 */

/**
 * @var openvrml::field_value::sfrotation_id
 *
 * @brief Designates an <code>sfrotation</code>.
 */

/**
 * @var openvrml::field_value::sfstring_id
 *
 * @brief Designates an <code>sfstring</code>.
 */

/**
 * @var openvrml::field_value::sftime_id
 *
 * @brief Designates an <code>sftime</code>.
 */

/**
 * @var openvrml::field_value::sfvec2f_id
 *
 * @brief Designates an <code>sfvec2f</code>.
 */

/**
 * @var openvrml::field_value::sfvec3f_id
 *
 * @brief Designates an <code>sfvec3f</code>.
 */

/**
 * @var openvrml::field_value::mfcolor_id
 *
 * @brief Designates an <code>mfcolor</code>.
 */

/**
 * @var openvrml::field_value::mffloat_id
 *
 * @brief Designates an <code>mffloat</code>.
 */

/**
 * @var openvrml::field_value::mfint32_id
 *
 * @brief Designates an <code>mfint32</code>.
 */

/**
 * @var openvrml::field_value::mfnode_id
 *
 * @brief Designates an <code>mfnode</code>.
 */

/**
 * @var openvrml::field_value::mfrotation_id
 *
 * @brief Designates an <code>mfrotation</code>.
 */

/**
 * @var openvrml::field_value::mfstring_id
 *
 * @brief Designates an <code>mfstring</code>.
 */

/**
 * @var openvrml::field_value::mftime_id
 *
 * @brief Designates an <code>mftime</code>.
 */

/**
 * @var openvrml::field_value::mfvec2f_id
 *
 * @brief Designates an <code>mfvec2f</code>.
 */

/**
 * @var openvrml::field_value::mfvec3f_id
 *
 * @brief Designates an <code>mfvec3f</code>.
 */

/**
 * @brief Create a default instance of the type specified by @p type.
 *
 * @param type  field value type identifier.
 *
 * @return a default instance of the type specified by @p type.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value>
 openvrml::field_value::create(const type_id type)
    throw (std::bad_alloc)
{
    using std::auto_ptr;
    switch (type) {
    case field_value::sfbool_id:
        return auto_ptr<field_value>(new sfbool);
    case field_value::sfcolor_id:
        return auto_ptr<field_value>(new sfcolor);
    case field_value::sffloat_id:
        return auto_ptr<field_value>(new sffloat);
    case field_value::sfimage_id:
        return auto_ptr<field_value>(new sfimage);
    case field_value::sfint32_id:
        return auto_ptr<field_value>(new sfint32);
    case field_value::sfnode_id:
        return auto_ptr<field_value>(new sfnode);
    case field_value::sfrotation_id:
        return auto_ptr<field_value>(new sfrotation);
    case field_value::sfstring_id:
        return auto_ptr<field_value>(new sfstring);
    case field_value::sftime_id:
        return auto_ptr<field_value>(new sftime);
    case field_value::sfvec2f_id:
        return auto_ptr<field_value>(new sfvec2f);
    case field_value::sfvec3f_id:
        return auto_ptr<field_value>(new sfvec3f);
    case field_value::mfcolor_id:
        return auto_ptr<field_value>(new mfcolor);
    case field_value::mffloat_id:
        return auto_ptr<field_value>(new mffloat);
    case field_value::mfint32_id:
        return auto_ptr<field_value>(new mfint32);
    case field_value::mfnode_id:
        return auto_ptr<field_value>(new mfnode);
    case field_value::mfrotation_id:
        return auto_ptr<field_value>(new mfrotation);
    case field_value::mfstring_id:
        return auto_ptr<field_value>(new mfstring);
    case field_value::mftime_id:
        return auto_ptr<field_value>(new mftime);
    case field_value::mfvec2f_id:
        return auto_ptr<field_value>(new mfvec2f);
    case field_value::mfvec3f_id:
        return auto_ptr<field_value>(new mfvec3f);
    default:
        assert(false);
    }
    return auto_ptr<field_value>(0);
}

/**
 * @brief Construct.
 */
openvrml::field_value::field_value() throw ()
{}

/**
 * @brief Copy constructor.
 *
 * @param value field value to copy.
 */
openvrml::field_value::field_value(const field_value &) throw ()
{}

/**
 * @brief Destroy.
 */
openvrml::field_value::~field_value() throw ()
{}

/**
 * @brief Assignment operator.
 *
 * @param value field value to assign.
 */
openvrml::field_value &
openvrml::field_value::operator=(const field_value &) throw ()
{
    return *this;
}

/**
 * @fn std::auto_ptr<openvrml::field_value> openvrml::field_value::clone() const throw (std::bad_alloc)
 *
 * @brief Virtual copy constructor.
 *
 * @return a new field_value identical to this one.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn openvrml::field_value & openvrml::field_value::assign(const field_value & value) throw (std::bad_cast, std::bad_alloc)
 *
 * @brief Virtual assignment.
 *
 * @param value the value to assign to the object.
 *
 * @return this object.
 *
 * @exception std::bad_cast     if @p value is not of the same concrete type as
 *                              this object.
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn void openvrml::field_value::print(std::ostream & out) const
 *
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */

/**
 * @fn openvrml::field_value::type_id openvrml::field_value::type() const throw ()
 *
 * @brief Get the field type.
 *
 * @return the type_id enumerant corresponding to the field_value's type
 */

namespace {
    const char * const field_value_type_id_[] = {
        "<invalid field type>",
        "SFBool",
        "SFColor",
        "SFFloat",
        "SFImage",
        "SFInt32",
        "SFNode",
        "SFRotation",
        "SFString",
        "SFTime",
        "SFVec2f",
        "SFVec3f",
        "MFColor",
        "MFFloat",
        "MFInt32",
        "MFNode",
        "MFRotation",
        "MFString",
        "MFTime",
        "MFVec2f",
        "MFVec3f"
    };
}

/**
 * @brief Stream output.
 *
 * @relatesalso openvrml::field_value
 *
 * If @p type is <code>field_value::invalid_type</code>, @c failbit is set on
 * @p out.
 *
 * @param out       output stream.
 * @param type_id   <code>field_value</code> type identifier.
 *
 * @return @p out.
 */
std::ostream & openvrml::operator<<(std::ostream & out,
                                    const field_value::type_id type_id)
{
    if (type_id == field_value::invalid_type_id) {
        out.setstate(std::ios_base::failbit);
    } else {
        out << field_value_type_id_[type_id];
    }
    return out;
}

/**
 * @brief Stream input.
 *
 * @relatesalso openvrml::field_value
 *
 * @param in        input stream.
 * @param type_id   <code>field_value</code> type identifier.
 *
 * @return @p in.
 */
std::istream & openvrml::operator>>(std::istream & in,
                                    field_value::type_id & type_id)
{
    std::string str;
    in >> str;
    static const char * const * const begin =
            field_value_type_id_ + field_value::sfbool_id;
    static const char * const * const end =
            field_value_type_id_ + field_value::mfvec3f_id + 1;
    const char * const * const pos = std::find(begin, end, str);
    if (pos != end) {
        type_id = field_value::type_id(pos - begin);
    } else {
        in.setstate(std::ios_base::failbit);
    }
    return in;
}

/**
 * @struct openvrml::FieldValueConcept
 *
 * @brief Concept checking class to validate that a template parameter is a
 *        model of the FieldValue concept.
 *
 * A Field Value is a concrete type that inherits field_value and holds a
 * value for a field of a node.
 *
 * @par Refinement of
 * <a href="http://www.sgi.com/tech/stl/DefaultConstructible.html">
 * DefaultConstructible</a>,
 * <a href="http://www.boost.org/libs/utility/CopyConstructible.html">
 * CopyConstructible</a>,
 * <a href="http://www.sgi.com/tech/stl/Assignable.html">
 * Assignable</a>,
 * <a href="http://www.sgi.com/tech/stl/EqualityComparable.html">
 * EqualityComparable</a>
 *
 * @par Associated types
 * <table>
 *   <tr>
 *     <td>Value type</td>
 *     <td>T::value_type</td>
 *     <td>The type of T's @a value member. The value type must be
 *         <a href="http://www.sgi.com/tech/stl/Assignable.html">Assignable</a>
 *         and <a href="http://www.sgi.com/tech/stl/DefaultConstructible.html">
 *         DefaultConstructible</a>.</td>
 *   </tr>
 * </table>
 *
 * @par Notation
 * <table>
 *   <tr>
 *     <td><code>T</code></td>
 *     <td>A type that is a model of Field Value</td>
 *   </tr>
 *   <tr>
 *     <td><code>v</code></td>
 *     <td>A reference of type <code>field_value&nbsp;@&</code></td>
 *   </tr>
 * </table>
 *
 * @par Definitions
 *
 * @par Valid expressions
 * In addition to the expressions defined in
 * <a href="http://www.sgi.com/tech/stl/Assignable.html">Assignable</a> and
 * <a href="http://www.sgi.com/tech/stl/EqualityComparable.html">
 * EqualityComparable</a>, the following expressions must be valid.
 * <table>
 *   <tr>
 *     <th>Name</th>
 *     <th>Expression</th>
 *     <th>Type requirements</th>
 *     <th>Return type</th>
 *   </tr>
 *   <tr>
 *     <td>Downcast</td>
 *     <td><code>static_cast@<T&nbsp;@&@>(v)</code></td>
 *     <td></td>
 *     <td>T&nbsp;@&</td>
 *   </tr>
 * </table>
 *
 * @par Models
 * - <code>sfbool</code>
 * - <code>sfcolor</code>
 * - <code>sffloat</code>
 * - <code>sfimage</code>
 * - <code>sfint32</code>
 * - <code>sfnode</code>
 * - <code>sfrotation</code>
 * - <code>sfstring</code>
 * - <code>sftime</code>
 * - <code>sfvec2f</code>
 * - <code>sfvec3f</code>
 * - <code>mfcolor</code>
 * - <code>mffloat</code>
 * - <code>mfint32</code>
 * - <code>mfnode</code>
 * - <code>mfrotation</code>
 * - <code>mfstring</code>
 * - <code>mftime</code>
 * - <code>mfvec2f</code>
 * - <code>mfvec3f</code>
 */

/**
 * @fn void openvrml::FieldValueConcept<T>::constraints()
 *
 * @brief Validate that T is a model of the Field Value concept.
 */

/**
 * @class openvrml::sfbool
 *
 * @ingroup fieldvalues
 *
 * @brief A boolean node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::sfbool::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sfbool::field_value_type_id(sfbool_id);

/**
 * @var openvrml::sfbool::value
 *
 * @brief Boolean value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value
 */
openvrml::sfbool::sfbool(const bool value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
openvrml::sfbool::~sfbool() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfbool::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfbool(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an sfbool.
 */
openvrml::sfbool & openvrml::sfbool::assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfbool &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfbool::print(std::ostream & out) const
{
    out << (this->value ? "TRUE" : "FALSE");
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfbool.
 */
openvrml::field_value::type_id openvrml::sfbool::type() const throw ()
{
    return field_value::sfbool_id;
}

/**
 * @relatesalso openvrml::sfbool
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfbool & lhs, const sfbool & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::sfbool
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator!=(const sfbool & lhs, const sfbool & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::sfcolor
 *
 * @ingroup fieldvalues
 *
 * @brief A color node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::sfcolor::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sfcolor::field_value_type_id(sfcolor_id);

/**
 * @var openvrml::sfcolor::value
 *
 * @brief Color value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value.
 */
openvrml::sfcolor::sfcolor(const color & value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
openvrml::sfcolor::~sfcolor() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfcolor::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfcolor(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFColor.
 */
openvrml::sfcolor & openvrml::sfcolor::assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfcolor &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfcolor::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfcolor.
 */
openvrml::field_value::type_id openvrml::sfcolor::type() const throw ()
{
    return field_value::sfcolor_id;
}

/**
 * @relatesalso openvrml::sfcolor
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfcolor & lhs, const sfcolor & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::sfcolor
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const sfcolor & lhs, const sfcolor & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::sffloat
 *
 * @ingroup fieldvalues
 *
 * @brief A single precision floating point node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::sffloat::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sffloat::field_value_type_id(sffloat_id);

/**
 * @var openvrml::sffloat::value
 *
 * @brief Single precision floating point value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value
 */
openvrml::sffloat::sffloat(const float value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
openvrml::sffloat::~sffloat() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sffloat::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sffloat::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sffloat(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an sffloat.
 */
openvrml::sffloat & openvrml::sffloat::assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sffloat &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sffloat.
 */
openvrml::field_value::type_id openvrml::sffloat::type() const throw ()
{
    return field_value::sffloat_id;
}

/**
 * @relatesalso openvrml::sffloat
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sffloat & lhs, const sffloat & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::sffloat
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator!=(const sffloat & lhs, const sffloat & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::sfimage
 *
 * @ingroup fieldvalues
 *
 * @brief A pixmap.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::sfimage::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sfimage::field_value_type_id(sfimage_id);

/**
 * @var openvrml::image openvrml::sfimage::value
 *
 * @brief Image data.
 */

/**
 * Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfimage::sfimage(const image & value) throw (std::bad_alloc):
    value(value)
{}

/**
 * @brief Destroy.
 */
openvrml::sfimage::~sfimage() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfimage::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfimage::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfimage(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast  if @p value is not an sfimage.
 * @exception std::bad_alloc if memory allocation fails.
 */
openvrml::sfimage & openvrml::sfimage::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const sfimage &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfimage.
 */
openvrml::field_value::type_id openvrml::sfimage::type() const throw ()
{
    return field_value::sfimage_id;
}

/**
 * @relatesalso openvrml::sfimage
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfimage & lhs, const sfimage & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::sfimage
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const sfimage & lhs, const sfimage & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::sfint32
 *
 * @ingroup fieldvalues
 *
 * @brief A 32-bit integer node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::sfint32::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sfint32::field_value_type_id(sfint32_id);

/**
 * @var openvrml::sfint32::value
 *
 * @brief Signed 32-bit integer value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value.
 */
openvrml::sfint32::sfint32(const int32 value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
openvrml::sfint32::~sfint32() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfint32::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfint32::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfint32(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an sfint32.
 */
openvrml::sfint32 & openvrml::sfint32::assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfint32 &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfint32.
 */
openvrml::field_value::type_id openvrml::sfint32::type() const throw ()
{
    return field_value::sfint32_id;
}

/**
 * @relatesalso openvrml::sfint32
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfint32 & lhs, const sfint32 & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::sfint32
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const sfint32 & lhs, const sfint32 & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::sfnode
 *
 * @ingroup fieldvalues
 *
 * @brief A node field value to hold a single node reference.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::sfnode::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sfnode::field_value_type_id(sfnode_id);

/**
 * @var openvrml::sfnode::value
 *
 * @brief A node reference.
 */

/**
 * @brief Construct.
 *
 * @param node a <code>node_ptr</code>.
 */
openvrml::sfnode::sfnode(const node_ptr & node) throw ():
    value(node)
{}

/**
 * @brief Destroy.
 */
openvrml::sfnode::~sfnode() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfnode::print(std::ostream & out) const
{
    if (this->value) {
        out << *this->value;
    } else {
        out << "NULL";
    }
}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfnode::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfnode(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an sfnode.
 */
openvrml::sfnode & openvrml::sfnode::assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfnode &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfnode.
 */
openvrml::field_value::type_id openvrml::sfnode::type() const throw ()
{
    return field_value::sfnode_id;
}

/**
 *
 * @relatesalso openvrml::sfnode
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfnode & lhs, const sfnode & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::sfnode
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const sfnode & lhs, const sfnode & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::sfrotation
 *
 * @ingroup fieldvalues
 *
 * @brief A rotation node field value.
 *
 * Per the VRML97 specification, the axis of an sfrotation is a normalized
 * vector (5.8). The specification leaves undefined how to deal with an
 * attempt to construct an sfrotation from an axis vector that is not
 * normalized. In order to allow users of the library to minimize the number
 * of normalizations, openvrml takes the following approach:
 *
 * - Attempts to construct an sfrotation axis from a vector that is not
 *   normalized will yield an assertion failure (abort) unless NDEBUG is
 *   defined when compiling the library (in which case truly wacky behavior
 *   could result).
 * - Assignment to individual components of the axis will result in the
 *   axis being re-normalized upon each assignment.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::sfrotation::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sfrotation::field_value_type_id(sfrotation_id);

/**
 * @var openvrml::sfrotation::value
 *
 * @brief Rotation value.
 */

/**
 * @brief Construct.
 *
 * @param rot   initial value.
 */
openvrml::sfrotation::sfrotation(const rotation & rot) throw ():
    value(rot)
{}

/**
 * @brief Destroy.
 */
openvrml::sfrotation::~sfrotation() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfrotation::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfrotation(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an sfrotation.
 */
openvrml::sfrotation & openvrml::sfrotation::assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfrotation &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfrotation.
 */
openvrml::field_value::type_id openvrml::sfrotation::type() const throw ()
{
    return field_value::sfrotation_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfrotation::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @relatesalso openvrml::sfrotation
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfrotation & lhs, const sfrotation & rhs)
    throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::sfrotation
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const sfrotation & lhs, const sfrotation & rhs)
    throw ()
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::sfstring
 *
 * @ingroup fieldvalues
 *
 * @brief A string node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::sfstring::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sfstring::field_value_type_id(sfstring_id);

/**
 * @var openvrml::sfstring::value
 *
 * @brief String value.
 */

/**
 * @brief Construct.
 *
 * @param value
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfstring::sfstring(const std::string & value) throw (std::bad_alloc):
   value(value)
{}

/**
 * @brief Destroy.
 */
openvrml::sfstring::~sfstring() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfstring::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfstring(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if @p value is not an sfstring.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfstring & openvrml::sfstring::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const sfstring &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfstring::print(std::ostream & out) const
{
    out << '\"' << this->value << '\"';
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfstring.
 */
openvrml::field_value::type_id openvrml::sfstring::type() const throw ()
{
    return field_value::sfstring_id;
}

/**
 * @relatesalso openvrml::sfstring
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfstring & lhs, const sfstring & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::sfstring
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const sfstring & lhs, const sfstring & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::sftime
 *
 * @ingroup fieldvalues
 *
 * @brief A double precision floating point node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sftime::field_value_type_id(sftime_id);

/**
 * @typedef openvrml::sftime::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var openvrml::sftime::value
 *
 * @brief Double precision floating point value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value
 */
openvrml::sftime::sftime(double value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
openvrml::sftime::~sftime() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sftime::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sftime(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an sftime.
 */
openvrml::sftime & openvrml::sftime::assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sftime &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sftime::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sftime.
 */
openvrml::field_value::type_id openvrml::sftime::type() const throw ()
{
    return field_value::sftime_id;
}

/**
 * @relatesalso openvrml::sftime
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sftime & lhs, const sftime & rhs) throw ()
{
    return lhs.value == rhs.value;
}


/**
 * @relatesalso openvrml::sftime
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const sftime & lhs, const sftime & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::sfvec2f
 *
 * @ingroup fieldvalues
 *
 * @brief A 2-component vector node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::sfvec2f::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sfvec2f::field_value_type_id(sfvec2f_id);

/**
 * @var openvrml::sfvec2f::value
 *
 * @brief 2-component vector value.
 */

/**
 * @brief Construct.
 *
 * @param vec   initial value.
 */
openvrml::sfvec2f::sfvec2f(const vec2f & vec) throw ():
    value(vec)
{}

/**
 * @brief Destroy.
 */
openvrml::sfvec2f::~sfvec2f() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfvec2f::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfvec2f(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an sfvec2f.
 */
openvrml::sfvec2f & openvrml::sfvec2f::assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfvec2f &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfvec2f.
 */
openvrml::field_value::type_id openvrml::sfvec2f::type() const throw ()
{
    return field_value::sfvec2f_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfvec2f::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @relatesalso openvrml::sfvec2f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfvec2f & lhs, const sfvec2f & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::sfvec2f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const sfvec2f & lhs, const sfvec2f & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::sfvec3f
 *
 * @ingroup fieldvalues
 *
 * @brief A 3-component vector node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sfvec3f::field_value_type_id(sfvec3f_id);

/**
 * @typedef openvrml::sfvec3f::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var openvrml::sfvec3f::value
 *
 * @brief 3-component vector value.
 */

/**
 * @brief Construct.
 *
 * @param vec   initial value.
 */
openvrml::sfvec3f::sfvec3f(const vec3f & vec) throw ():
    value(vec)
{}

/**
 * @brief Destroy.
 */
openvrml::sfvec3f::~sfvec3f() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfvec3f::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfvec3f(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an SFBool.
 */
openvrml::sfvec3f & openvrml::sfvec3f::assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfvec3f &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfvec3f.
 */
openvrml::field_value::type_id openvrml::sfvec3f::type() const throw ()
{
    return field_value::sfvec3f_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfvec3f::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @relatesalso openvrml::sfvec3f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfvec3f & lhs, const sfvec3f & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::sfvec3f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const sfvec3f & lhs, const sfvec3f & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class openvrml::mfcolor
 *
 * @ingroup fieldvalues
 *
 * @brief A color array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mfcolor::value_type
 *
 * @brief The type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mfcolor::field_value_type_id(mfcolor_id);

/**
 * @var std::vector<openvrml::color> openvrml::mfcolor::value
 *
 * @brief Color values.
 */

/**
 * @brief Construct.
 *
 * Creates an <code>mfcolor</code> with @p n copies of @p value.
 *
 * @param n     the number elements in the mfcolor.
 * @param value used to initialize the mfcolor.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
openvrml::mfcolor::mfcolor(const std::vector<color>::size_type n,
                           const color & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfcolor::mfcolor(const std::vector<color> & value)
    throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> openvrml::mfcolor::mfcolor(InputIterator first, InputIterator last)
 *
 * @brief Create an <code>mfcolor</code> with a copy of a range.
 *
 * Creates an <code>mfcolor</code> with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the <code>mfcolor</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfcolor::~mfcolor() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfcolor::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mfcolor(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an mfcolor object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfcolor & openvrml::mfcolor::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfcolor &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfcolor.
 */
openvrml::field_value::type_id openvrml::mfcolor::type() const throw ()
{
    return field_value::mfcolor_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::mfcolor::print(std::ostream & out) const
{
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<color>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mfcolor
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mfcolor & lhs, const mfcolor & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::mfcolor
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator!=(const mfcolor & lhs, const mfcolor & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class openvrml::mffloat
 *
 * @ingroup fieldvalues
 *
 * @brief A float array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mffloat::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mffloat::field_value_type_id(mffloat_id);

/**
 * @var std::vector<float> openvrml::mffloat::value
 *
 * @brief Single precision floating point values.
 */

/**
 * @brief Construct.
 *
 * Creates an <code>mffloat</code> with @p n copies of @p value.
 *
 * @param n     the number of elements in the <code>mffloat</code>.
 * @param value used to initialize the <code>mffloat</code>.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
openvrml::mffloat::mffloat(const std::vector<float>::size_type n,
                           const float value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mffloat::mffloat(const std::vector<float> & value)
    throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> openvrml::mffloat::mffloat(InputIterator first, InputIterator last)
 *
 * @brief Create an <code>mffloat</code> with a copy of a range.
 *
 * Creates an <code>mffloat</code> with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the <code>mffloat</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mffloat::~mffloat() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mffloat::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mffloat(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an mffloat object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mffloat & openvrml::mffloat::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mffloat &>(value));
}

/**
 * @brief Get the type identifier for the class.
 *
 * @return @c field_value::mffloat_id.
 */
openvrml::field_value::type_id openvrml::mffloat::type() const throw ()
{
    return field_value::mffloat_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::mffloat::print(std::ostream & out) const
{
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<float>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mffloat
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mffloat & lhs, const mffloat & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::mffloat
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const mffloat & lhs, const mffloat & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class openvrml::mfint32
 *
 * @ingroup fieldvalues
 *
 * @brief An integer array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mfint32::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mfint32::field_value_type_id(mfint32_id);

/**
 * @var std::vector<openvrml::int32> openvrml::mfint32::value
 *
 * @brief Signed 32-bit integer values.
 */

/**
 * @brief Construct.
 *
 * Creates an <code>mfint32</code> with @p n copies of @p value.
 *
 * @param n     the number of elements in the <code>mfint32</code>.
 * @param value used to initialize the <code>mfint32</code>.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
openvrml::mfint32::mfint32(const std::vector<int32>::size_type n,
                           const int32 value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfint32::mfint32(const std::vector<int32> & value)
    throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> openvrml::mfint32::mfint32(InputIterator first, InputIterator last)
 *
 * @brief Create an <code>mfint32</code> with a copy of a range.
 *
 * Creates an <code>mfint32</code> with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the <code>mfint32</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfint32::~mfint32() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfint32::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mfint32(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an mfint32 object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfint32 & openvrml::mfint32::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfint32 &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfint32_id.
 */
openvrml::field_value::type_id openvrml::mfint32::type() const throw ()
{
    return field_value::mfint32_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::mfint32::print(std::ostream & out) const
{
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<int32>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mfint32
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mfint32 & lhs, const mfint32 & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::mfint32
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const mfint32 & lhs, const mfint32 & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class openvrml::mfnode
 *
 * @ingroup fieldvalues
 *
 * @brief A node reference array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mfnode::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mfnode::field_value_type_id(mfnode_id);

/**
 * @var std::vector<openvrml::node_ptr> openvrml::mfnode::value
 *
 * @brief Node references.
 */

/**
 * @brief Construct.
 *
 * Creates an <code>mfnode</code> with @p n copies of @p value.
 *
 * @param n     the number elements in the <code>mfnode</code>.
 * @param value used to initialize the <code>mfnode</code>.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
openvrml::mfnode::mfnode(const std::vector<node_ptr>::size_type n,
                         const node_ptr & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfnode::mfnode(const std::vector<node_ptr> & value)
    throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> openvrml::mfnode::mfnode(InputIterator first, InputIterator last)
 *
 * @brief Create an <code>mfnode</code> with a copy of a range.
 *
 * Creates an <code>mfnode</code> with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the <code>mfnode</code>'s @a value elements is destroyed, and memory
 * allocated for them (if any) is deallocated.
 */
openvrml::mfnode::~mfnode() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfnode::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mfnode(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an mfnode object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfnode & openvrml::mfnode::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfnode &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfnode_id.
 */
openvrml::field_value::type_id openvrml::mfnode::type() const throw ()
{
    return field_value::mfnode_id;
}

/**
 * @brief Print to an output stream.
 *
 * Any null elements in the mfnode will not get printed; VRML97 syntax does not
 * accommodate NULL in an mfnode.
 *
 * @param out   an output stream.
 */
void openvrml::mfnode::print(std::ostream & out) const
{
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<node_ptr>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            if (*i) { out << **i << ", "; }
        }
    }
    if (!this->value.empty() && this->value.back()) {
        out << *this->value.back();
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mfnode
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mfnode & lhs, const mfnode & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::mfnode
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const mfnode & lhs, const mfnode & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class openvrml::mfrotation
 *
 * @ingroup fieldvalues
 *
 * @brief A rotation array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mfrotation::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mfrotation::field_value_type_id(mfrotation_id);

/**
 * @var std::vector<openvrml::rotation> openvrml::mfrotation::value
 *
 * @brief Rotation values.
 */

/**
 * @brief Construct.
 *
 * Creates an <code>mfrotation</code> with @p n copies of @p value.
 *
 * @param n     the number elements in the <code>mfrotation</code>.
 * @param value used to initialize the <code>mfrotation</code>.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
openvrml::mfrotation::mfrotation(const std::vector<rotation>::size_type n,
                                 const rotation & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfrotation::mfrotation(const std::vector<rotation> & value)
    throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> openvrml::mfrotation::mfrotation(InputIterator first, InputIterator last)
 *
 * @brief Create an <code>mfrotation</code> with a copy of a range.
 *
 * Creates an <code>mfrotation</code> with a @a value that is a copy of the
 * range [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the <code>mfrotation</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfrotation::~mfrotation() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfrotation::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mfrotation(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an mfrotation object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfrotation & openvrml::mfrotation::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfrotation &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfrotation_id.
 */
openvrml::field_value::type_id openvrml::mfrotation::type() const throw ()
{
    return field_value::mfrotation_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::mfrotation::print(std::ostream & out) const
{
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<rotation>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mfrotation
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mfrotation & lhs, const mfrotation & rhs)
    throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::mfrotation
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const mfrotation & lhs, const mfrotation & rhs)
    throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class openvrml::mfstring
 *
 * @ingroup fieldvalues
 *
 * @brief A string array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mfstring::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mfstring::field_value_type_id(mfstring_id);

/**
 * @var std::vector<std::string> openvrml::mfstring::value
 *
 * @brief String values.
 */

/**
 * @brief Construct.
 *
 * Creates an <code>mfstring</code> with @p n copies of @p value.
 *
 * @param n     the number elements in @a mfstring::value.
 * @param value used to initialize @a mfstring::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>mfstring::value.size()</code> is @p n. Every element in
 *      @a mfstring::value is a copy of @p value.
 */
openvrml::mfstring::mfstring(const std::vector<std::string>::size_type n,
                             const std::string & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfstring::mfstring(const std::vector<std::string> & value)
    throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> openvrml::mfstring::mfstring(InputIterator first, InputIterator last)
 *
 * @brief Create an <code>mfstring</code> with a copy of a range.
 *
 * Creates an <code>mfstring</code> with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the <code>mfstring</code>'s elements is destroyed, and memory
 * allocated for them (if any) is deallocated.
 */
openvrml::mfstring::~mfstring() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfstring::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mfstring(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an mfstring object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfstring & openvrml::mfstring::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfstring &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfstring_id.
 */
openvrml::field_value::type_id openvrml::mfstring::type() const throw ()
{
    return field_value::mfstring_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::mfstring::print(std::ostream & out) const
{
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<std::string>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << '\"' << *i << "\", ";
        }
    }
    if (!this->value.empty()) {
        out << '\"' << this->value.back() << '\"';
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mfstring
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mfstring & lhs, const mfstring & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::mfstring
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const mfstring & lhs, const mfstring & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class openvrml::mftime
 *
 * @ingroup fieldvalues
 *
 * @brief A double array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mftime::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mftime::field_value_type_id(mftime_id);

/**
 * @var std::vector<double> openvrml::mftime::value
 *
 * @brief Double precision floating point values.
 */

/**
 * @brief Construct.
 *
 * Creates an <code>mftime</code> with @p n copies of @p value.
 *
 * @param n     the number of elements in @a mftime::value.
 * @param value used to initialize @a mftime::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>mftime::value.size()</code> is @p n. Every element in
 *      @a mftime::value is a copy of @p value.
 */
openvrml::mftime::mftime(const std::vector<double>::size_type n,
                         const double value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mftime::mftime(const std::vector<double> & value)
    throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> openvrml::mftime::mftime(InputIterator first, InputIterator last)
 *
 * @brief Create an <code>mftime</code> with a copy of a range.
 *
 * Creates an <code>mftime</code> with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the openvrml::mftime's @a value elements is destroyed, and memory
 * allocated for them (if any) is deallocated.
 */
openvrml::mftime::~mftime() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value>
openvrml::mftime::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mftime(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if @p value is not an <code>mftime</code>
 *                              object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mftime & openvrml::mftime::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mftime &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mftime_id.
 */
openvrml::field_value::type_id openvrml::mftime::type() const throw ()
{
    return field_value::mftime_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::mftime::print(std::ostream & out) const
{
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<double>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mftime
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mftime & lhs, const mftime & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::mftime
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const mftime & lhs, const mftime & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class openvrml::mfvec2f
 *
 * @ingroup fieldvalues
 *
 * @brief A 2-component vector array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mfvec2f::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mfvec2f::field_value_type_id(mfvec2f_id);

/**
 * @var std::vector<openvrml::vec2f> openvrml::mfvec2f::value
 *
 * @brief 2-component vector values.
 */

/**
 * @brief Construct.
 *
 * Creates an <code>mfvec2f</code> with @p n copies of @p value.
 *
 * @param n     the number elements in @a mfvec2f::value.
 * @param value used to initialize @a mfvec2f::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>mfvec2f::value.size()</code> is @p n. Every element in
 *      @a mfvec2f::value is a copy of @p value.
 */
openvrml::mfvec2f::mfvec2f(const std::vector<vec2f>::size_type n,
                           const vec2f & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec2f::mfvec2f(const std::vector<vec2f> & value)
    throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> openvrml::mfvec2f::mfvec2f(InputIterator first, InputIterator last)
 *
 * @brief Create an <code>mfvec2f</code> with a copy of a range.
 *
 * Creates an <code>mfvec2f</code> with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the <code>mfvec2f</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfvec2f::~mfvec2f() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value>
openvrml::mfvec2f::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mfvec2f(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an mfvec2f object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec2f & openvrml::mfvec2f::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfvec2f &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfvec2f_id.
 */
openvrml::field_value::type_id openvrml::mfvec2f::type() const throw ()
{
    return field_value::mfvec2f_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::mfvec2f::print(std::ostream & out) const
{
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<vec2f>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mfvec2f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mfvec2f & lhs, const mfvec2f & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::mfvec2f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const mfvec2f & lhs, const mfvec2f & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class openvrml::mfvec3f
 *
 * @ingroup fieldvalues
 *
 * @brief A 3-component vector array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mfvec3f::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mfvec3f::field_value_type_id(mfvec3f_id);

/**
 * @var std::vector<openvrml::vec3f> openvrml::mfvec3f::value
 *
 * @brief 3-component vector values.
 */

/**
 * @brief Construct.
 *
 * Creates an <code>mfvec3f</code> with @p n copies of @p value.
 *
 * @param n     the number elements in @a mfvec3f::value.
 * @param value used to initialize @a mfvec3f::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>mfvec3f::value.size()</code> is @p n. Every element in
 *      @a mfvec3f::value is a copy of @p value.
 */
openvrml::mfvec3f::mfvec3f(const std::vector<vec3f>::size_type n,
                           const vec3f & value)
    throw (std::bad_alloc):
    value(n, value)
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec3f::mfvec3f(const std::vector<vec3f> & value)
    throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> openvrml::mfvec3f::mfvec3f(InputIterator first, InputIterator last)
 *
 * @brief Create an <code>mfvec3f</code> with a copy of a range.
 *
 * Creates an <code>mfvec3f</code> with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the <code>mfvec3f</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfvec3f::~mfvec3f() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfvec3f::clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mfvec3f(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if @p value is not an <code>mfvec3f</code>
 *                              object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec3f & openvrml::mfvec3f::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfvec3f &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfvec3f_id.
 */
openvrml::field_value::type_id openvrml::mfvec3f::type() const throw ()
{
    return field_value::mfvec3f_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::mfvec3f::print(std::ostream & out) const
{
    out << '[';
    if (this->value.size() > 1) {
        for (std::vector<vec3f>::const_iterator i(this->value.begin());
                i != this->value.end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value.empty()) {
        out << this->value.back();
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mfvec3f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mfvec3f & lhs, const mfvec3f & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso openvrml::mfvec3f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const mfvec3f & lhs, const mfvec3f & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @fn template <> void std::swap(openvrml::mfcolor & a, openvrml::mfcolor & b)
 *
 * @relatesalso openvrml::mfcolor
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void std::swap(openvrml::mffloat & a, openvrml::mffloat & b)
 *
 * @relatesalso openvrml::mffloat
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void std::swap(openvrml::mfint32 & a, openvrml::mfint32 & b)
 *
 * @relatesalso openvrml::mfint32
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void std::swap(openvrml::mfnode & a, openvrml::mfnode & b)
 *
 * @relatesalso openvrml::mfnode
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void std::swap(openvrml::mfrotation & a, openvrml::mfrotation & b)
 *
 * @relatesalso openvrml::mfrotation
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void std::swap(openvrml::mfstring & a, openvrml::mfstring & b)
 *
 * @relatesalso openvrml::mfstring
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void std::swap(openvrml::mftime & a, openvrml::mftime & b)
 *
 * @relatesalso openvrml::mftime
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void std::swap(openvrml::mfvec2f & a, openvrml::mfvec2f & b)
 *
 * @relatesalso openvrml::mfvec2f
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */

/**
 * @fn template <> void std::swap(openvrml::mfvec3f & a, openvrml::mfvec3f & b)
 *
 * @relatesalso openvrml::mfvec3f
 *
 * @brief Swap the values of @p a and @p b.
 *
 * Does not throw.
 *
 * @param a
 * @param b
 */
