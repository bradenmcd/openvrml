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
# include "private.h"
# include "field.h"
# include "node.h"

namespace openvrml {

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
std::ostream & operator<<(std::ostream & out, const field_value & value)
{
    value.print(out);
    return out;
}

/**
 * @class field_value
 *
 * @ingroup fieldvalues
 *
 * @brief Abstract base class for the VRML field types.
 */

/**
 * @enum field_value::type_id
 *
 * @brief Used to identify field_value types.
 *
 * These tags are typically used to designate an expected type or to avoid a
 * @c dynamic_cast.
 */

/**
 * @var field_value::invalid_type_id
 *
 * @brief Zero value typically used to indicate failure.
 */

/**
 * @var field_value::sfbool_id
 *
 * @brief Designates an sfbool.
 */

/**
 * @var field_value::sfcolor_id
 *
 * @brief Designates an sfcolor.
 */

/**
 * @var field_value::sffloat_id
 *
 * @brief Designates an sffloat.
 */

/**
 * @var field_value::sfimage_id
 *
 * @brief Designates an sfimage.
 */

/**
 * @var field_value::sfint32_id
 *
 * @brief Designates an sfint32.
 */

/**
 * @var field_value::sfnode_id
 *
 * @brief Designates an sfnode.
 */

/**
 * @var field_value::sfrotation_id
 *
 * @brief Designates an sfrotation.
 */

/**
 * @var field_value::sfstring_id
 *
 * @brief Designates an sfstring.
 */

/**
 * @var field_value::sftime_id
 *
 * @brief Designates an sftime.
 */

/**
 * @var field_value::sfvec2f_id
 *
 * @brief Designates an sfvec2f.
 */

/**
 * @var field_value::sfvec3f_id
 *
 * @brief Designates an sfvec3f.
 */

/**
 * @var field_value::mfcolor_id
 *
 * @brief Designates an mfcolor.
 */

/**
 * @var field_value::mffloat_id
 *
 * @brief Designates an mffloat.
 */

/**
 * @var field_value::mfint32_id
 *
 * @brief Designates an mfint32.
 */

/**
 * @var field_value::mfnode_id
 *
 * @brief Designates an mfnode.
 */

/**
 * @var field_value::mfrotation_id
 *
 * @brief Designates an mfrotation.
 */

/**
 * @var field_value::mfstring_id
 *
 * @brief Designates an mfstring.
 */

/**
 * @var field_value::mftime_id
 *
 * @brief Designates an mftime.
 */

/**
 * @var field_value::mfvec2f_id
 *
 * @brief Designates an mfvec2f.
 */

/**
 * @var field_value::mfvec3f_id
 *
 * @brief Designates an mfvec3f.
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
std::auto_ptr<field_value> field_value::create(const type_id type)
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
field_value::field_value() throw ()
{}

/**
 * @brief Copy constructor.
 *
 * @param value field value to copy.
 */
field_value::field_value(const field_value & value) throw ()
{}

/**
 * @brief Destroy.
 */
field_value::~field_value() throw ()
{}

/**
 * @brief Assignment operator.
 *
 * @param value field value to assign.
 */
field_value & field_value::operator=(const field_value & value) throw ()
{
    return *this;
}

/**
 * @fn std::auto_ptr<field_value> field_value::clone() const throw (std::bad_alloc)
 *
 * @brief Virtual copy constructor.
 *
 * @return a new field_value identical to this one.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn field_value & field_value::assign(const field_value & value) throw (std::bad_cast, std::bad_alloc)
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
 * @fn void field_value::print(std::ostream & out) const
 *
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */

/**
 * @fn field_value::type_id field_value::type() const throw ()
 *
 * @brief Get the field type.
 *
 * @return the type_id enumerant corresponding to the field_value's type
 */

/**
 * @typedef field_value_ptr
 *
 * @brief A boost::shared_ptr to a field_value.
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
 * @relatesalso field_value
 *
 * If @p type is field_value::invalid_type, @c failbit is set on @p out.
 *
 * @param out       output stream.
 * @param type_id   field_value type identifier.
 *
 * @return @p out.
 */
std::ostream & operator<<(std::ostream & out,
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
 * @relatesalso field_value
 *
 * @param in        input stream.
 * @param type_id   field_value type identifier.
 *
 * @return @p in.
 */
std::istream & operator>>(std::istream & in, field_value::type_id & type_id)
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
 * @struct FieldValueConcept
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
 * - sfbool
 * - sfcolor
 * - sffloat
 * - sfimage
 * - sfint32
 * - sfnode
 * - sfrotation
 * - sfstring
 * - sftime
 * - sfvec2f
 * - sfvec3f
 * - mfcolor
 * - mffloat
 * - mfint32
 * - mfnode
 * - mfrotation
 * - mfstring
 * - mftime
 * - mfvec2f
 * - mfvec3f
 */

/**
 * @fn void FieldValueConcept<T>::constraints()
 *
 * @brief Validate that T is a model of the FieldValue concept.
 */

/**
 * @class sfbool
 *
 * @ingroup fieldvalues
 *
 * @brief A boolean node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef sfbool::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var sfbool::value
 *
 * @brief Boolean value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value
 */
sfbool::sfbool(const bool value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
sfbool::~sfbool() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> sfbool::clone() const throw (std::bad_alloc)
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
field_value & sfbool::assign(const field_value & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfbool &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void sfbool::print(std::ostream & out) const
{
    out << (this->value ? "TRUE" : "FALSE");
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfbool.
 */
field_value::type_id sfbool::type() const throw ()
{
    return field_value::sfbool_id;
}

/**
 * @relatesalso sfbool
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const sfbool & lhs, const sfbool & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso sfbool
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *         otherwise.
 */
bool operator!=(const sfbool & lhs, const sfbool & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class sfcolor
 *
 * @ingroup fieldvalues
 *
 * @brief A color node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef sfcolor::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var sfcolor::value
 *
 * @brief Color value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value.
 */
sfcolor::sfcolor(const color & value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
sfcolor::~sfcolor() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> sfcolor::clone() const throw (std::bad_alloc)
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
field_value & sfcolor::assign(const field_value & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfcolor &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void sfcolor::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfcolor.
 */
field_value::type_id sfcolor::type() const throw ()
{
    return field_value::sfcolor_id;
}

/**
 * @relatesalso sfcolor
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const sfcolor & lhs, const sfcolor & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso sfcolor
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const sfcolor & lhs, const sfcolor & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class sffloat
 *
 * @ingroup fieldvalues
 *
 * @brief A single precision floating point node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef sffloat::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var sffloat::value
 *
 * @brief Single precision floating point value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value
 */
sffloat::sffloat(const float value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
sffloat::~sffloat() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void sffloat::print(std::ostream & out) const
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
std::auto_ptr<field_value> sffloat::clone() const throw (std::bad_alloc)
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
field_value & sffloat::assign(const field_value & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sffloat &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sffloat.
 */
field_value::type_id sffloat::type() const throw ()
{
    return field_value::sffloat_id;
}

/**
 * @relatesalso sffloat
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const sffloat & lhs, const sffloat & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso sffloat
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *         otherwise.
 */
bool operator!=(const sffloat & lhs, const sffloat & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class sfimage
 *
 * @ingroup fieldvalues
 *
 * @brief A pixmap.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef sfimage::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var image sfimage::value
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
sfimage::sfimage(const image & value) throw (std::bad_alloc):
    value(value)
{}

/**
 * @brief Destroy.
 */
sfimage::~sfimage() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void sfimage::print(std::ostream & out) const
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
std::auto_ptr<field_value> sfimage::clone() const throw (std::bad_alloc)
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
field_value & sfimage::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const sfimage &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfimage.
 */
field_value::type_id sfimage::type() const throw ()
{
    return field_value::sfimage_id;
}

/**
 * @relatesalso sfimage
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const sfimage & lhs, const sfimage & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso sfimage
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const sfimage & lhs, const sfimage & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class sfint32
 *
 * @ingroup fieldvalues
 *
 * @brief A 32-bit integer node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef sfint32::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var sfint32::value
 *
 * @brief Signed 32-bit integer value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value.
 */
sfint32::sfint32(const int32 value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
sfint32::~sfint32() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void sfint32::print(std::ostream & out) const
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
std::auto_ptr<field_value> sfint32::clone() const throw (std::bad_alloc)
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
field_value & sfint32::assign(const field_value & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfint32 &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfint32.
 */
field_value::type_id sfint32::type() const throw ()
{
    return field_value::sfint32_id;
}

/**
 * @relatesalso sfint32
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const sfint32 & lhs, const sfint32 & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso sfint32
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const sfint32 & lhs, const sfint32 & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class sfnode
 *
 * @ingroup fieldvalues
 *
 * @brief A node field value to hold a single node reference.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef sfnode::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var sfnode::value
 *
 * @brief A Node reference.
 */

/**
 * @brief Construct.
 *
 * @param node a node_ptr
 */
sfnode::sfnode(const node_ptr & node) throw ():
    value(node)
{}

/**
 * @brief Destroy.
 */
sfnode::~sfnode() throw ()
{}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void sfnode::print(std::ostream & out) const
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
std::auto_ptr<field_value> sfnode::clone() const throw (std::bad_alloc)
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
field_value & sfnode::assign(const field_value & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfnode &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfnode.
 */
field_value::type_id sfnode::type() const throw ()
{
    return field_value::sfnode_id;
}

/**
 *
 * @relatesalso sfnode
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const sfnode & lhs, const sfnode & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso sfnode
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const sfnode & lhs, const sfnode & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class sfrotation
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
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef sfrotation::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var sfrotation::value
 *
 * @brief Rotation value.
 */

/**
 * @brief Construct.
 *
 * @param rot   initial value.
 */
sfrotation::sfrotation(const rotation & rot) throw ():
    value(rot)
{}

/**
 * @brief Destroy.
 */
sfrotation::~sfrotation() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> sfrotation::clone() const throw (std::bad_alloc)
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
field_value & sfrotation::assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfrotation &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfrotation.
 */
field_value::type_id sfrotation::type() const throw ()
{
    return field_value::sfrotation_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void sfrotation::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @relatesalso sfrotation
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const sfrotation & lhs, const sfrotation & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso sfrotation
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const sfrotation & lhs, const sfrotation & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class sfstring
 *
 * @ingroup fieldvalues
 *
 * @brief A string node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef sfstring::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var sfstring::value
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
sfstring::sfstring(const std::string & value) throw (std::bad_alloc):
   value(value)
{}

/**
 * @brief Destroy.
 */
sfstring::~sfstring() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> sfstring::clone() const throw (std::bad_alloc)
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
field_value & sfstring::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const sfstring &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void sfstring::print(std::ostream & out) const
{
    out << '\"' << this->value << '\"';
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfstring.
 */
field_value::type_id sfstring::type() const throw ()
{
    return field_value::sfstring_id;
}

/**
 * @relatesalso sfstring
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const sfstring & lhs, const sfstring & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso sfstring
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const sfstring & lhs, const sfstring & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class sftime
 *
 * @ingroup fieldvalues
 *
 * @brief A double precision floating point node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef sftime::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var sftime::value
 *
 * @brief Double precision floating point value.
 */

/**
 * @brief Construct.
 *
 * @param value initial value
 */
sftime::sftime(double value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
sftime::~sftime() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> sftime::clone() const throw (std::bad_alloc)
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
field_value & sftime::assign(const field_value & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sftime &>(value));
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void sftime::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sftime.
 */
field_value::type_id sftime::type() const throw ()
{
    return field_value::sftime_id;
}

/**
 * @relatesalso sftime
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const sftime & lhs, const sftime & rhs) throw ()
{
    return lhs.value == rhs.value;
}


/**
 * @relatesalso sftime
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const sftime & lhs, const sftime & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class sfvec2f
 *
 * @ingroup fieldvalues
 *
 * @brief A 2-component vector node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef sfvec2f::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var sfvec2f::value
 *
 * @brief 2-component vector value.
 */

/**
 * @brief Construct.
 *
 * @param vec   initial value.
 */
sfvec2f::sfvec2f(const vec2f & vec) throw ():
    value(vec)
{}

/**
 * @brief Destroy.
 */
sfvec2f::~sfvec2f() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> sfvec2f::clone() const throw (std::bad_alloc)
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
field_value & sfvec2f::assign(const field_value & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfvec2f &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfvec2f.
 */
field_value::type_id sfvec2f::type() const throw ()
{
    return field_value::sfvec2f_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void sfvec2f::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @relatesalso sfvec2f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const sfvec2f & lhs, const sfvec2f & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso sfvec2f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const sfvec2f & lhs, const sfvec2f & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class sfvec3f
 *
 * @ingroup fieldvalues
 *
 * @brief A 3-component vector node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef sfvec3f::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var sfvec3f::value
 *
 * @brief 3-component vector value.
 */

/**
 * @brief Construct.
 *
 * @param vec   initial value.
 */
sfvec3f::sfvec3f(const vec3f & vec) throw ():
    value(vec)
{}

/**
 * @brief Destroy.
 */
sfvec3f::~sfvec3f() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> sfvec3f::clone() const throw (std::bad_alloc)
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
field_value & sfvec3f::assign(const field_value & value) throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfvec3f &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfvec3f.
 */
field_value::type_id sfvec3f::type() const throw ()
{
    return field_value::sfvec3f_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void sfvec3f::print(std::ostream & out) const
{
    out << this->value;
}

/**
 * @relatesalso sfvec3f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const sfvec3f & lhs, const sfvec3f & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso sfvec3f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const sfvec3f & lhs, const sfvec3f & rhs) throw ()
{
    return !(lhs == rhs);
}


/**
 * @class mfcolor
 *
 * @ingroup fieldvalues
 *
 * @brief A color array node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef mfcolor::value_type
 *
 * @brief The type of @a value.
 */

/**
 * @var std::vector<color> mfcolor::value
 *
 * @brief Color values.
 */

/**
 * @brief Construct.
 *
 * Creates an mfcolor with @p n copies of @p value.
 *
 * @param n     the number elements in the mfcolor.
 * @param value used to initialize the mfcolor.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
mfcolor::mfcolor(const std::vector<color>::size_type n, const color & value)
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
mfcolor::mfcolor(const std::vector<color> & value) throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> mfcolor::mfcolor(InputIterator first, InputIterator last)
 *
 * @brief Create an mfcolor with a copy of a range.
 *
 * Creates an mfcolor with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the mfcolor's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
mfcolor::~mfcolor() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> mfcolor::clone() const throw (std::bad_alloc)
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
field_value & mfcolor::assign(const field_value & value)
        throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfcolor &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::mfcolor.
 */
field_value::type_id mfcolor::type() const throw ()
{
    return field_value::mfcolor_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void mfcolor::print(std::ostream & out) const
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
 * @relatesalso mfcolor
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const mfcolor & lhs, const mfcolor & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso mfcolor
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *         otherwise.
 */
bool operator!=(const mfcolor & lhs, const mfcolor & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class mffloat
 *
 * @ingroup fieldvalues
 *
 * @brief A float array node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef mffloat::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var std::vector<float> mffloat::value
 *
 * @brief Single precision floating point values.
 */

/**
 * @brief Construct.
 *
 * Creates an mffloat with @p n copies of @p value.
 *
 * @param n     the number of elements in the mffloat.
 * @param value used to initialize the mffloat.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
mffloat::mffloat(const std::vector<float>::size_type n, const float value)
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
mffloat::mffloat(const std::vector<float> & value) throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> mffloat::mffloat(InputIterator first, InputIterator last)
 *
 * @brief Create an mffloat with a copy of a range.
 *
 * Creates an mffloat with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the mffloat's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
mffloat::~mffloat() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> mffloat::clone() const throw (std::bad_alloc)
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
field_value & mffloat::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mffloat &>(value));
}

/**
 * @brief Get the type identifier for the class.
 *
 * @return @c field_value::mffloat_id.
 */
field_value::type_id mffloat::type() const throw ()
{
    return field_value::mffloat_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void mffloat::print(std::ostream & out) const
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
 * @relatesalso mffloat
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const mffloat & lhs, const mffloat & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso mffloat
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const mffloat & lhs, const mffloat & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class mfint32
 *
 * @ingroup fieldvalues
 *
 * @brief An integer array node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef mfint32::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var std::vector<int32> mfint32::value
 *
 * @brief Signed 32-bit integer values.
 */

/**
 * @brief Construct.
 *
 * Creates an mfint32 with @p n copies of @p value.
 *
 * @param n     the number of elements in the mfint32.
 * @param value used to initialize the mfint32.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
mfint32::mfint32(const std::vector<int32>::size_type n, const int32 value)
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
mfint32::mfint32(const std::vector<int32> & value) throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> mfint32::mfint32(InputIterator first, InputIterator last)
 *
 * @brief Create an mfint32 with a copy of a range.
 *
 * Creates an mfint32 with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the mfint32's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
mfint32::~mfint32() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> mfint32::clone() const throw (std::bad_alloc)
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
field_value & mfint32::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfint32 &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::mfint32_id.
 */
field_value::type_id mfint32::type() const throw ()
{
    return field_value::mfint32_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void mfint32::print(std::ostream & out) const
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
 * @relatesalso mfint32
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const mfint32 & lhs, const mfint32 & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso mfint32
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const mfint32 & lhs, const mfint32 & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class mfnode
 *
 * @ingroup fieldvalues
 *
 * @brief A node reference array node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef mfnode::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var std::vector<node_ptr> mfnode::value
 *
 * @brief Node references.
 */

/**
 * @brief Construct.
 *
 * Creates an mfnode with @p n copies of @p value.
 *
 * @param n     the number elements in the mfnode.
 * @param value used to initialize the mfnode.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
mfnode::mfnode(const std::vector<node_ptr>::size_type n,
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
mfnode::mfnode(const std::vector<node_ptr> & value) throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> mfnode::mfnode(InputIterator first, InputIterator last)
 *
 * @brief Create an mfnode with a copy of a range.
 *
 * Creates an mfnode with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the mfnode's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
mfnode::~mfnode() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> mfnode::clone() const throw (std::bad_alloc)
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
field_value & mfnode::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfnode &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::mfnode_id.
 */
field_value::type_id mfnode::type() const throw ()
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
void mfnode::print(std::ostream & out) const
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
 * @relatesalso mfnode
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const mfnode & lhs, const mfnode & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso mfnode
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const mfnode & lhs, const mfnode & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class mfrotation
 *
 * @ingroup fieldvalues
 *
 * @brief A rotation array node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef mfrotation::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var std::vector<rotation> mfrotation::value
 *
 * @brief Rotation values.
 */

/**
 * @brief Construct.
 *
 * Creates an mfrotation with @p n copies of @p value.
 *
 * @param n     the number elements in the mfrotation.
 * @param value used to initialize the mfrotation.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
mfrotation::mfrotation(const std::vector<rotation>::size_type n,
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
mfrotation::mfrotation(const std::vector<rotation> & value)
    throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> mfrotation::mfrotation(InputIterator first, InputIterator last)
 *
 * @brief Create an mfrotation with a copy of a range.
 *
 * Creates an mfrotation with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the mfrotation's @a value elements is destroyed, and memory
 * allocated for them (if any) is deallocated.
 */
mfrotation::~mfrotation() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> mfrotation::clone() const throw (std::bad_alloc)
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
field_value & mfrotation::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfrotation &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::mfrotation_id.
 */
field_value::type_id mfrotation::type() const throw ()
{
    return field_value::mfrotation_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void mfrotation::print(std::ostream & out) const
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
 * @relatesalso mfrotation
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const mfrotation & lhs, const mfrotation & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso mfrotation
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const mfrotation & lhs, const mfrotation & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class mfstring
 *
 * @ingroup fieldvalues
 *
 * @brief A string array node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef mfstring::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var std::vector<std::string> mfstring::value
 *
 * @brief String values.
 */

/**
 * @brief Construct.
 *
 * Creates an mfstring with @p n copies of @p value.
 *
 * @param n     the number elements in @a mfstring::value.
 * @param value used to initialize @a mfstring::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>mfstring::value.size()</code> is @p n. Every element in
 *      @a mfstring::value is a copy of @p value.
 */
mfstring::mfstring(const std::vector<std::string>::size_type n,
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
mfstring::mfstring(const std::vector<std::string> & value)
    throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> mfstring::mfstring(InputIterator first, InputIterator last)
 *
 * @brief Create an mfstring with a copy of a range.
 *
 * Creates an mfstring with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the mfstring's elements is destroyed, and memory allocated for them
 * (if any) is deallocated.
 */
mfstring::~mfstring() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> mfstring::clone() const throw (std::bad_alloc)
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
field_value & mfstring::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfstring &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::mfstring_id.
 */
field_value::type_id mfstring::type() const throw ()
{
    return field_value::mfstring_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void mfstring::print(std::ostream & out) const
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
 * @relatesalso mfstring
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const mfstring & lhs, const mfstring & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso mfstring
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const mfstring & lhs, const mfstring & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class mftime
 *
 * @ingroup fieldvalues
 *
 * @brief A double array node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef mftime::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var std::vector<double> mftime::value
 *
 * @brief Double precision floating point values.
 */

/**
 * @brief Construct.
 *
 * Creates an mftime with @p n copies of @p value.
 *
 * @param n     the number of elements in @a mftime::value.
 * @param value used to initialize @a mftime::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>mftime::value.size()</code> is @p n. Every element in
 *      @a mftime::value is a copy of @p value.
 */
mftime::mftime(const std::vector<double>::size_type n, const double value)
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
mftime::mftime(const std::vector<double> & value) throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> mftime::mftime(InputIterator first, InputIterator last)
 *
 * @brief Create an mftime with a copy of a range.
 *
 * Creates an mftime with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the mftime's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
mftime::~mftime() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> mftime::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mftime(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an mftime object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
field_value & mftime::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mftime &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::mftime_id.
 */
field_value::type_id mftime::type() const throw ()
{
    return field_value::mftime_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void mftime::print(std::ostream & out) const
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
 * @relatesalso mftime
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const mftime & lhs, const mftime & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso mftime
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const mftime & lhs, const mftime & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class mfvec2f
 *
 * @ingroup fieldvalues
 *
 * @brief A 2-component vector array node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef mfvec2f::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var std::vector<vec2f> mfvec2f::value
 *
 * @brief 2-component vector values.
 */

/**
 * @brief Construct.
 *
 * Creates an mfvec2f with @p n copies of @p value.
 *
 * @param n     the number elements in @a mfvec2f::value.
 * @param value used to initialize @a mfvec2f::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>mfvec2f::value.size()</code> is @p n. Every element in
 *      @a mfvec2f::value is a copy of @p value.
 */
mfvec2f::mfvec2f(const std::vector<vec2f>::size_type n, const vec2f & value)
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
mfvec2f::mfvec2f(const std::vector<vec2f> & value) throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> mfvec2f::mfvec2f(InputIterator first, InputIterator last)
 *
 * @brief Create an mfvec2f with a copy of a range.
 *
 * Creates an mfvec2f with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the mfvec2f's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
mfvec2f::~mfvec2f() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> mfvec2f::clone() const throw (std::bad_alloc)
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
field_value & mfvec2f::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfvec2f &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::mfvec2f_id.
 */
field_value::type_id mfvec2f::type() const throw ()
{
    return field_value::mfvec2f_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void mfvec2f::print(std::ostream & out) const
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
 * @relatesalso mfvec2f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const mfvec2f & lhs, const mfvec2f & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso mfvec2f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const mfvec2f & lhs, const mfvec2f & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class mfvec3f
 *
 * @ingroup fieldvalues
 *
 * @brief A 3-component vector array node field value.
 *
 * @par Model of
 * @link FieldValueConcept FieldValue@endlink
 */

/**
 * @typedef mfvec3f::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @var std::vector<vec3f> mfvec3f::value
 *
 * @brief 3-component vector values.
 */

/**
 * @brief Construct.
 *
 * Creates an mfvec3f with @p n copies of @p value.
 *
 * @param n     the number elements in @a mfvec3f::value.
 * @param value used to initialize @a mfvec3f::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>mfvec3f::value.size()</code> is @p n. Every element in
 *      @a mfvec3f::value is a copy of @p value.
 */
mfvec3f::mfvec3f(const std::vector<vec3f>::size_type n, const vec3f & value)
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
mfvec3f::mfvec3f(const std::vector<vec3f> & value) throw (std::bad_alloc):
    value(value)
{}

/**
 * @fn template <typename InputIterator> mfvec3f::mfvec3f(InputIterator first, InputIterator last)
 *
 * @brief Create an mfvec3f with a copy of a range.
 *
 * Creates an mfvec3f with a @a value that is a copy of the range
 * [@p first, @p last).
 *
 * @param first an iterator pointing to the beginning of the range.
 * @param last  an iterator pointing one past the end of the range.
 */

/**
 * @brief Destroy.
 *
 * Each of the mfvec3f's @a value elements is destroyed, and memory allocated
 * for them (if any) is deallocated.
 */
mfvec3f::~mfvec3f() throw ()
{}

/**
 * @brief Virtual copy constructor.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<field_value> mfvec3f::clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mfvec3f(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an mfvec3f object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
field_value & mfvec3f::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfvec3f &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::mfvec3f_id.
 */
field_value::type_id mfvec3f::type() const throw ()
{
    return field_value::mfvec3f_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void mfvec3f::print(std::ostream & out) const
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
 * @relatesalso mfvec3f
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool operator==(const mfvec3f & lhs, const mfvec3f & rhs) throw ()
{
    return lhs.value == rhs.value;
}

/**
 * @relatesalso mfvec3f
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool operator!=(const mfvec3f & lhs, const mfvec3f & rhs) throw ()
{
    return lhs.value != rhs.value;
}


/**
 * @class event_listener
 *
 * @brief Abstract base class of event listeners.
 */

/**
 * @var node & event_listener::node
 *
 * @brief The node to which the event_listener belongs.
 */

/**
 * @brief Construct.
 *
 * @param node  the node to which the event_listener belongs.
 */
event_listener::event_listener(openvrml::node & node) throw ():
    node(node)
{}

/**
 * @brief Destroy.
 */
event_listener::~event_listener() throw ()
{}


/**
 * @class field_value_listener
 *
 * @brief Concrete event listener template.
 */

/**
 * @fn field_value_listener<FieldValue>::field_value_listener(openvrml::node & node) throw ()
 *
 * @brief Construct.
 *
 * @param node  the node to which the event_listener belongs.
 */

/**
 * @fn field_value_listener<FieldValue>::~field_value_listener() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn void field_value_listener<FieldValue>::process_event(const FieldValue & value, double timestamp) throw (std::bad_alloc)
 *
 * @brief Process an event.
 *
 * @param value     the event value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @typedef sfbool_listener
 *
 * @brief sfbool event listener.
 */

/**
 * @typedef sfcolor_listener
 *
 * @brief sfcolor event listener.
 */

/**
 * @typedef sffloat_listener
 *
 * @brief sffloat event listener.
 */

/**
 * @typedef sfimage_listener
 *
 * @brief sfimage event listener.
 */

/**
 * @typedef sfint32_listener
 *
 * @brief sfint32 event listener.
 */

/**
 * @typedef sfnode_listener
 *
 * @brief sfnode event listener.
 */

/**
 * @typedef sfrotation_listener
 *
 * @brief sfrotation event listener.
 */

/**
 * @typedef sfstring_listener
 *
 * @brief sfstring event listener.
 */

/**
 * @typedef sftime_listener
 *
 * @brief sftime event listener.
 */

/**
 * @typedef sfvec2f_listener
 *
 * @brief sfvec2f event listener.
 */

/**
 * @typedef sfvec3f_listener
 *
 * @brief sfvec3f event listener.
 */

/**
 * @typedef mfcolor_listener
 *
 * @brief mfcolor event listener.
 */

/**
 * @typedef mffloat_listener
 *
 * @brief mffloat event listener.
 */

/**
 * @typedef mfint32_listener
 *
 * @brief mfint32 event listener.
 */

/**
 * @typedef mfnode_listener
 *
 * @brief mfnode event listener.
 */

/**
 * @typedef mfrotation_listener
 *
 * @brief mfrotation event listener.
 */

/**
 * @typedef mfstring_listener
 *
 * @brief mfstring event listener.
 */

/**
 * @typedef mftime_listener
 *
 * @brief mftime event listener.
 */

/**
 * @typedef mfvec2f_listener
 *
 * @brief mfvec2f event listener.
 */

/**
 * @typedef mfvec3f_listener
 *
 * @brief mfvec3f event listener.
 */


/**
 * @class event_emitter
 *
 * @brief Abstract base class of event emitters.
 */

/**
 * @brief Create an event_emitter.
 *
 * @param value value to emit.
 *
 * @return an event_emitter.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<event_emitter>
event_emitter::create(const field_value & value) throw (std::bad_alloc)
{
    std::auto_ptr<event_emitter> emitter;
    switch (value.type()) {
    case field_value::sfbool_id:
        emitter.reset(new sfbool_emitter(static_cast<const sfbool &>(value)));
        break;
    case field_value::sfcolor_id:
        emitter.reset(
            new sfcolor_emitter(static_cast<const sfcolor &>(value)));
        break;
    case field_value::sffloat_id:
        emitter.reset(
            new sffloat_emitter(static_cast<const sffloat &>(value)));
        break;
    case field_value::sfimage_id:
        emitter.reset(
            new sfimage_emitter(static_cast<const sfimage &>(value)));
        break;
    case field_value::sfint32_id:
        emitter.reset(
            new sfint32_emitter(static_cast<const sfint32 &>(value)));
        break;
    case field_value::sfnode_id:
        emitter.reset(new sfnode_emitter(static_cast<const sfnode &>(value)));
        break;
    case field_value::sfstring_id:
        emitter.reset(
            new sfstring_emitter(static_cast<const sfstring &>(value)));
        break;
    case field_value::sfrotation_id:
        emitter.reset(
            new sfrotation_emitter(static_cast<const sfrotation &>(value)));
        break;
    case field_value::sftime_id:
        emitter.reset(new sftime_emitter(static_cast<const sftime &>(value)));
        break;
    case field_value::sfvec2f_id:
        emitter.reset(
            new sfvec2f_emitter(static_cast<const sfvec2f &>(value)));
        break;
    case field_value::sfvec3f_id:
        emitter.reset(
            new sfvec3f_emitter(static_cast<const sfvec3f &>(value)));
        break;
    case field_value::mfcolor_id:
        emitter.reset(
            new mfcolor_emitter(static_cast<const mfcolor &>(value)));
        break;
    case field_value::mffloat_id:
        emitter.reset(
            new mffloat_emitter(static_cast<const mffloat &>(value)));
        break;
    case field_value::mfint32_id:
        emitter.reset(
            new mfint32_emitter(static_cast<const mfint32 &>(value)));
        break;
    case field_value::mfnode_id:
        emitter.reset(new mfnode_emitter(static_cast<const mfnode &>(value)));
        break;
    case field_value::mfstring_id:
        emitter.reset(
            new mfstring_emitter(static_cast<const mfstring &>(value)));
        break;
    case field_value::mfrotation_id:
        emitter.reset(
            new mfrotation_emitter(static_cast<const mfrotation &>(value)));
        break;
    case field_value::mftime_id:
        emitter.reset(new mftime_emitter(static_cast<const mftime &>(value)));
        break;
    case field_value::mfvec2f_id:
        emitter.reset(
            new mfvec2f_emitter(static_cast<const mfvec2f &>(value)));
        break;
    case field_value::mfvec3f_id:
        emitter.reset(
            new mfvec3f_emitter(static_cast<const mfvec3f &>(value)));
        break;
    default:
        assert(false);
    }
    return emitter;
}

/**
 * @var const field_value & event_emitter::value
 *
 * @brief A reference to the field_value for the event_emitter.
 */

/**
 * @typedef event_emitter::listener_set
 *
 * @brief Set of event_listeners.
 */

/**
 * @var event_emitter::listener_set event_emitter::listeners_
 *
 * @brief The listeners registered for this emitter.
 *
 * When emit_event is called, each of the registered listeners will be sent an
 * event.
 */

/**
 * @var double event_emitter::last_time
 *
 * @brief The timestamp of the last event emitted.
 */

/**
 * @brief Construct.
 *
 * @param value field_value associated with this emitter.
 */
event_emitter::event_emitter(const field_value & value) throw ():
    value(value)
{}

/**
 * @brief Destroy.
 */
event_emitter::~event_emitter() throw ()
{}

/**
 * @brief Registered listeners.
 *
 * @return the set of registered event_listeners.
 */
const event_emitter::listener_set & event_emitter::listeners() const throw ()
{
    return this->listeners_;
}

/**
 * @fn void event_emitter::emit_event(double timestamp) throw (std::bad_alloc)
 *
 * @brief Emit an event.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * This function is called by node::emit_event.
 */


/**
 * @class field_value_emitter
 *
 * @brief Concrete event emitter template.
 */

/**
 * @fn field_value_emitter<FieldValue>::field_value_emitter(const FieldValue & value) throw ()
 *
 * @brief Construct.
 */

/**
 * @fn field_value_emitter<FieldValue>::~field_value_emitter() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn void field_value_emitter<FieldValue>::emit_event(double timestamp) throw (std::bad_alloc)
 *
 * @brief Emit an event.
 *
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn bool field_value_emitter<FieldValue>::add(field_value_listener<FieldValue> & listener) throw (std::bad_alloc)
 *
 * @brief Add an event listener.
 *
 * @param listener  the listener to add.
 *
 * @return @c true if @p listener was added; @c false if @p listener was not
 *         added (if it was already registered for the emitter).
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn bool field_value_emitter<FieldValue>::remove(field_value_listener<FieldValue> & listener)
 *
 * @brief Remove an event listener.
 *
 * @param listener  the listener to remove.
 *
 * @return @c true if @p listener was removed; @c false if @p listener was not
 *         removed (if it was not registered for the emitter).
 */

/**
 * @typedef sfbool_emitter
 *
 * @brief sfbool event emitter.
 */

/**
 * @typedef sfcolor_emitter
 *
 * @brief sfcolor event emitter.
 */

/**
 * @typedef sffloat_emitter
 *
 * @brief sffloat event emitter.
 */

/**
 * @typedef sfimage_emitter
 *
 * @brief sfimage event emitter.
 */

/**
 * @typedef sfint32_emitter
 *
 * @brief sfint32 event emitter.
 */

/**
 * @typedef sfnode_emitter
 *
 * @brief sfnode event emitter.
 */

/**
 * @typedef sfrotation_emitter
 *
 * @brief sfrotation event emitter.
 */

/**
 * @typedef sfstring_emitter
 *
 * @brief sfstring event emitter.
 */

/**
 * @typedef sftime_emitter
 *
 * @brief sftime event emitter.
 */

/**
 * @typedef sfvec2f_emitter
 *
 * @brief sfvec2f event emitter.
 */

/**
 * @typedef sfvec3f_emitter
 *
 * @brief sfvec3f event emitter.
 */

/**
 * @typedef mfcolor_emitter
 *
 * @brief mfcolor event emitter.
 */

/**
 * @typedef mffloat_emitter
 *
 * @brief mffloat event emitter.
 */

/**
 * @typedef mfint32_emitter
 *
 * @brief mfint32 event emitter.
 */

/**
 * @typedef mfnode_emitter
 *
 * @brief mfnode event emitter.
 */

/**
 * @typedef mfrotation_emitter
 *
 * @brief mfrotation event emitter.
 */

/**
 * @typedef mfstring_emitter
 *
 * @brief mfstring event emitter.
 */

/**
 * @typedef mftime_emitter
 *
 * @brief mftime event emitter.
 */

/**
 * @typedef mfvec2f_emitter
 *
 * @brief mfvec2f event emitter.
 */

/**
 * @typedef mfvec3f_emitter
 *
 * @brief mfvec3f event emitter.
 */

/**
 * @class exposedfield
 *
 * @brief Class template to simplify implementation of exposedFields.
 */

/**
 * @fn template <typename FieldValue> exposedfield<FieldValue>::exposedfield(openvrml::node & node, const typename FieldValue::value_type & value)
 *
 * @brief Construct.
 *
 * @param node  a reference to the node to which the exposedField belongs.
 * @param value default value.
 */

/**
 * @fn template <typename FieldValue> exposedfield<FieldValue>::~exposedfield() throw ()
 *
 * @brief Destroy.
 */

/**
 * @fn template <typename FieldValue> void exposedfield<FieldValue>::process_event(const FieldValue & value, double timestamp) throw (std::bad_alloc)
 *
 * @brief Process an event.
 *
 * This function performs the following steps:
 *
 * -# set the exposedField value.
 * -# call exposedfield<FieldValue>::do_process_event.
 * -# set the modified flag.
 * -# emit the event.
 *
 * @param value     new value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn template <typename FieldValue> void exposedfield<FieldValue>::do_process_event(const FieldValue & value, double timestamp) throw (std::bad_alloc)
 *
 * @brief Called by exposedfield<FieldValue>::process_event.
 *
 * Subclasses should override this method to implement event handling
 * functionality specific to a particular exposedField. The default
 * implementation of this function does nothing.
 *
 * @param value     new value.
 * @param timestamp the current time.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

} // namespace openvrml

namespace std {

/**
 * @fn template <> void swap(openvrml::mfcolor & a, openvrml::mfcolor & b)
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
 * @fn template <> void swap(openvrml::mffloat & a, openvrml::mffloat & b)
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
 * @fn template <> void swap(openvrml::mfint32 & a, openvrml::mfint32 & b)
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
 * @fn template <> void swap(openvrml::mfnode & a, openvrml::mfnode & b)
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
 * @fn template <> void swap(openvrml::mfrotation & a, openvrml::mfrotation & b)
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
 * @fn template <> void swap(openvrml::mfstring & a, openvrml::mfstring & b)
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
 * @fn template <> void swap(openvrml::mftime & a, openvrml::mftime & b)
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
 * @fn template <> void swap(openvrml::mfvec2f & a, openvrml::mfvec2f & b)
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
 * @fn template <> void swap(openvrml::mfvec3f & a, openvrml::mfvec3f & b)
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

}
