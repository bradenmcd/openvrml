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
 * @class openvrml::field_value::counted_impl_base
 *
 * @brief Base class for the internal reference-counted objects.
 */

/**
 * @brief Destroy.
 */
openvrml::field_value::counted_impl_base::~counted_impl_base() throw ()
{}

/**
 * @brief Clone.
 *
 * Delegates to <code>counted_impl_base::do_clone</code>.
 *
 * @return a clone of the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value::counted_impl_base>
openvrml::field_value::counted_impl_base::clone() const throw (std::bad_alloc)
{
    return this->do_clone();
}

/**
 * @fn std::auto_ptr<openvrml::field_value::counted_impl_base> openvrml::field_value::counted_impl_base::do_clone() const throw (std::bad_alloc)
 *
 * @brief Clone.
 *
 * Polymorphically construct a copy.
 *
 * @return a clone of the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @class openvrml::field_value::counted_impl
 *
 * @brief Concrete reference-counted implementation.
 */

/**
 * @var boost::shared_ptr<ValueType> openvrml::field_value::counted_impl::value_
 *
 * @brief A <code>shared_ptr</code> to an instance of the
 *        <code>value_type</code> of a
 *        @link openvrml::FieldValueConcept Field Value@endlink.
 */

/**
 * @fn openvrml::field_value::counted_impl::counted_impl(const ValueType & value)
 *
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn const ValueType & openvrml::field_value::counted_impl::value() const throw ()
 *
 * @brief Access.
 *
 * @return the value.
 */

/**
 * @fn void openvrml::field_value::counted_impl::value(const ValueType & val) throw (std::bad_alloc)
 *
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @fn std::auto_ptr<openvrml::field_value::counted_impl_base> openvrml::field_value::counted_impl::do_clone() const throw (std::bad_alloc)
 *
 * @brief Polymorphically construct a copy.
 *
 * @return a clone of the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @var std::auto_ptr<openvrml::field_value::counted_impl_base> openvrml::field_value::counted_impl_
 *
 * @brief Reference-counted implementation.
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
    case field_value::sfdouble_id:
        return auto_ptr<field_value>(new sfdouble);
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
    case field_value::sfvec2d_id:
        return auto_ptr<field_value>(new sfvec2d);
    case field_value::sfvec3f_id:
        return auto_ptr<field_value>(new sfvec3f);
    case field_value::sfvec3d_id:
        return auto_ptr<field_value>(new sfvec3d);
    case field_value::mfcolor_id:
        return auto_ptr<field_value>(new mfcolor);
    case field_value::mffloat_id:
        return auto_ptr<field_value>(new mffloat);
    case field_value::mfdouble_id:
        return auto_ptr<field_value>(new mfdouble);
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
    case field_value::mfvec2d_id:
        return auto_ptr<field_value>(new mfvec2d);
    case field_value::mfvec3f_id:
        return auto_ptr<field_value>(new mfvec3f);
    case field_value::mfvec3d_id:
        return auto_ptr<field_value>(new mfvec3d);
    default:
        assert(false);
    }
    return auto_ptr<field_value>(0);
}

/**
 * @class openvrml::field_value::value_type_constructor_tag
 *
 * @brief This struct exists only to disambiguate <code>field_value</code>'s
 *        constructor template from its copy constructor.
 */

/**
 * @fn openvrml::field_value::field_value(const ValueType & value, const value_type_constructor_tag &) throw (std::bad_alloc)
 *
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @brief Construct a copy.
 *
 * @param fv    field value to copy.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::field_value::field_value(const field_value & fv)
    throw (std::bad_alloc):
    counted_impl_(fv.counted_impl_->clone())
{}


/**
 * @brief Destroy.
 */
openvrml::field_value::~field_value() throw ()
{}

/**
 * @fn FieldValue & openvrml::field_value::operator=(const FieldValue & fv) throw ()
 *
 * @brief Assignment operator.
 *
 * @param value field value to assign.
 */

/**
 * @brief Polymorphically construct a copy.
 *
 * This function delegates to <code>field_value::do_clone</code>.
 *
 * @return a new <code>field_value</code> identical to this one.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::field_value::clone() const
    throw (std::bad_alloc)
{
    std::auto_ptr<field_value> result = this->do_clone();
    assert(typeid(*result) == typeid(*this)
           && "do_clone incorrectly overridden");
    return result;
}

/**
 * @fn std::auto_ptr<openvrml::field_value> openvrml::field_value::do_clone() const throw (std::bad_alloc)
 *
 * @brief Polymorphically construct a copy.
 *
 * @return a new field_value identical to this one.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

/**
 * @brief Virtual assignment.
 *
 * Thus function delegates to <code>field_value::do_assign</code>.
 *
 * @param value the value to assign to the object.
 *
 * @return this object.
 */
openvrml::field_value &
openvrml::field_value::assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    field_value & this_ = this->do_assign(value);
    assert(typeid(this_) == typeid(*this)
           && "do_assign incorrectly overridden");
    return this_;
}

/**
 * @fn openvrml::field_value & openvrml::field_value::do_assign(const field_value & value) throw (std::bad_cast, std::bad_alloc)
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
 * @fn void openvrml::field_value::swap(FieldValue & val) throw ()
 *
 * @brief Swap.
 *
 * @param val   the value to swap with this one.
 */

/**
 * @fn void openvrml::field_value::print(std::ostream & out) const
 *
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */

/**
 * @brief Get the field type.
 *
 * This function delegates to <code>field_value::do_type</code>.
 *
 * @return the <code>type_id</code> enumerant corresponding to the
 *        <code>field_value</code>'s type.
 */
openvrml::field_value::type_id openvrml::field_value::type() const throw ()
{
    return this->do_type();
}

/**
 * @fn openvrml::field_value::type_id openvrml::field_value::do_type() const throw ()
 *
 * @brief Get the field type.
 *
 * @return the type_id enumerant corresponding to the field_value's type
 */

/**
 * @fn const typename FieldValue::value_type & openvrml::field_value::value() const throw ()
 *
 * @brief Access.
 *
 * @return the current value.
 */

/**
 * @fn void openvrml::field_value::value(const typename FieldValue::value_type & val) throw (std::bad_alloc)
 *
 * @brief Mutate.
 *
 * @param val   new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */

namespace {
    const char * const field_value_type_id_[] = {
        "<invalid field type>",
        "SFBool",
        "SFColor",
        "SFFloat",
        "SFDouble",
        "SFImage",
        "SFInt32",
        "SFNode",
        "SFRotation",
        "SFString",
        "SFTime",
        "SFVec2f",
        "SFVec2d",
        "SFVec3f",
        "SFVec3d",
        "MFColor",
        "MFFloat",
        "MFDouble",
        "MFInt32",
        "MFNode",
        "MFRotation",
        "MFString",
        "MFTime",
        "MFVec2f",
        "MFVec2d",
        "MFVec3f",
        "MFVec3d"
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
 * @brief Construct.
 *
 * @param value initial value
 */
openvrml::sfbool::sfbool(const value_type value) throw ():
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfb   the instance to copy.
 */
openvrml::sfbool::sfbool(const sfbool & sfb):
    field_value(sfb)
{}

/**
 * @brief Destroy.
 */
openvrml::sfbool::~sfbool() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfb   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfbool & openvrml::sfbool::operator=(const sfbool & sfb)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(sfb);
}

/**
 * @brief Access.
 *
 * @return the boolean value.
 */
const openvrml::sfbool::value_type & openvrml::sfbool::value() const throw ()
{
    return this->field_value::value<sfbool>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfbool::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfbool>(val);
}

/**
 * @brief Swap.
 *
 * @param sfb   the value to swap with this one.
 */
void openvrml::sfbool::swap(sfbool & sfb) throw ()
{
    this->field_value::swap<sfbool>(sfb);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfbool::do_clone() const
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
openvrml::sfbool & openvrml::sfbool::do_assign(const field_value & value)
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
    out << (this->value() ? "TRUE" : "FALSE");
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfbool.
 */
openvrml::field_value::type_id openvrml::sfbool::do_type() const throw ()
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
    return lhs.value() == rhs.value();
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
 * @brief Construct.
 *
 * @param value initial value.
 */
openvrml::sfcolor::sfcolor(const value_type & value) throw ():
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfc   the instance to copy.
 */
openvrml::sfcolor::sfcolor(const sfcolor & sfc):
    field_value(sfc)
{}

/**
 * @brief Destroy.
 */
openvrml::sfcolor::~sfcolor() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfc   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfcolor & openvrml::sfcolor::operator=(const sfcolor & sfc)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sfc);
}

/**
 * @brief Access.
 *
 * @return the color value.
 */
const openvrml::sfcolor::value_type & openvrml::sfcolor::value() const throw ()
{
    return this->field_value::value<sfcolor>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfcolor::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfcolor>(val);
}

/**
 * @brief Swap.
 *
 * @param sfc   the value to swap with this one.
 */
void openvrml::sfcolor::swap(sfcolor & sfc) throw ()
{
    this->field_value::swap<sfcolor>(sfc);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfcolor::do_clone() const
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
openvrml::sfcolor & openvrml::sfcolor::do_assign(const field_value & value)
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
    out << this->value();
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfcolor.
 */
openvrml::field_value::type_id openvrml::sfcolor::do_type() const throw ()
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
    return lhs.value() == rhs.value();
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
 * @brief Construct.
 *
 * @param value initial value
 */
openvrml::sffloat::sffloat(const value_type value) throw ():
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sff   the instance to copy.
 */
openvrml::sffloat::sffloat(const sffloat & sff):
    field_value(sff)
{}

/**
 * @brief Destroy.
 */
openvrml::sffloat::~sffloat() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sff   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sffloat & openvrml::sffloat::operator=(const sffloat & sff)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(sff);
}

/**
 * @brief Access.
 *
 * @return the floating point value.
 */
const openvrml::sffloat::value_type & openvrml::sffloat::value() const throw ()
{
    return this->field_value::value<sffloat>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sffloat::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sffloat>(val);
}

/**
 * @brief Swap.
 *
 * @param sff   the value to swap with this one.
 */
void openvrml::sffloat::swap(sffloat & sff) throw ()
{
    this->field_value::swap<sffloat>(sff);
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sffloat::print(std::ostream & out) const
{
    out << this->value();
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sffloat::do_clone() const
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
openvrml::sffloat & openvrml::sffloat::do_assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sffloat &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sffloat.
 */
openvrml::field_value::type_id openvrml::sffloat::do_type() const throw ()
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
    return lhs.value() == rhs.value();
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
 * @class openvrml::sfdouble
 *
 * @ingroup fieldvalues
 *
 * @brief A single precision doubleing point node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::sfdouble::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sfdouble::field_value_type_id(sfdouble_id);

/**
 * @brief Construct.
 *
 * @param value initial value
 */
openvrml::sfdouble::sfdouble(const value_type value) throw ():
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value
 */
openvrml::sfdouble::sfdouble(const float value) throw ():
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfd   the instance to copy.
 */
openvrml::sfdouble::sfdouble(const sfdouble & sfd):
    field_value(sfd)
{}

/**
 * @brief Destroy.
 */
openvrml::sfdouble::~sfdouble() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfd   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfdouble & openvrml::sfdouble::operator=(const sfdouble & sfd)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sfd);
}

/**
 * @brief Access.
 *
 * @return the double-precision value.
 */
const openvrml::sfdouble::value_type & openvrml::sfdouble::value() const
    throw ()
{
    return this->field_value::value<sfdouble>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfdouble::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfdouble>(val);
}

/**
 * @brief Swap.
 *
 * @param sfd   the value to swap with this one.
 */
void openvrml::sfdouble::swap(sfdouble & sfd) throw ()
{
    this->field_value::swap<sfdouble>(sfd);
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfdouble::print(std::ostream & out) const
{
    out << this->value();
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfdouble::do_clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfdouble(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an sfdouble.
 */
openvrml::sfdouble & openvrml::sfdouble::do_assign(const field_value & value)
    throw (std::bad_cast)
{
    //allow to convert from a float to a double
    const sffloat * float_val = dynamic_cast<const sffloat *>(&value);
    if (!float_val) {
        this->value(float_val->value());
        return *this;
    }

    return (*this = dynamic_cast<const sfdouble &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfdouble.
 */
openvrml::field_value::type_id openvrml::sfdouble::do_type() const throw ()
{
    return field_value::sfdouble_id;
}

/**
 * @relatesalso openvrml::sfdouble
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfdouble & lhs, const sfdouble & rhs) throw ()
{
    return lhs.value() == rhs.value();
}

/**
 * @relatesalso openvrml::sfdouble
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator!=(const sfdouble & lhs, const sfdouble & rhs) throw ()
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
 * Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfimage::sfimage(const image & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfi   the instance to copy.
 */
openvrml::sfimage::sfimage(const sfimage & sfi):
    field_value(sfi)
{}

/**
 * @brief Destroy.
 */
openvrml::sfimage::~sfimage() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfi   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfimage & openvrml::sfimage::operator=(const sfimage & sfi)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sfi);
}

/**
 * @brief Access.
 *
 * @return the image value.
 */
const openvrml::sfimage::value_type & openvrml::sfimage::value() const throw ()
{
    return this->field_value::value<sfimage>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfimage::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfimage>(val);
}

/**
 * @brief Swap.
 *
 * @param sfi   the value to swap with this one.
 */
void openvrml::sfimage::swap(sfimage & sfi) throw ()
{
    this->field_value::swap<sfimage>(sfi);
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfimage::print(std::ostream & out) const
{
    out << this->value();
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfimage::do_clone() const
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
openvrml::sfimage & openvrml::sfimage::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const sfimage &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfimage.
 */
openvrml::field_value::type_id openvrml::sfimage::do_type() const throw ()
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
    return lhs.value() == rhs.value();
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
 * @brief Construct.
 *
 * @param value initial value.
 */
openvrml::sfint32::sfint32(const int32 value) throw ():
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfi   the instance to copy.
 */
openvrml::sfint32::sfint32(const sfint32 & sfi):
    field_value(sfi)
{}

/**
 * @brief Destroy.
 */
openvrml::sfint32::~sfint32() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfi   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfint32 & openvrml::sfint32::operator=(const sfint32 & sfi)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sfi);
}

/**
 * @brief Access.
 *
 * @return the integer value.
 */
const openvrml::sfint32::value_type & openvrml::sfint32::value() const throw ()
{
    return this->field_value::value<sfint32>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfint32::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfint32>(val);
}

/**
 * @brief Swap.
 *
 * @param sfi   the value to swap with this one.
 */
void openvrml::sfint32::swap(sfint32 & sfi) throw ()
{
    this->field_value::swap<sfint32>(sfi);
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfint32::print(std::ostream & out) const
{
    out << this->value();
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfint32::do_clone() const
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
openvrml::sfint32 & openvrml::sfint32::do_assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfint32 &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfint32.
 */
openvrml::field_value::type_id openvrml::sfint32::do_type() const throw ()
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
    return lhs.value() == rhs.value();
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
 * @brief Construct.
 *
 * @param node a <code>node_ptr</code>.
 */
openvrml::sfnode::sfnode(const node_ptr & node) throw ():
    field_value(node, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfn   the instance to copy.
 */
openvrml::sfnode::sfnode(const sfnode & sfn):
    field_value(sfn)
{}

/**
 * @brief Destroy.
 */
openvrml::sfnode::~sfnode() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfn   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfnode & openvrml::sfnode::operator=(const sfnode & sfn)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sfn);
}

/**
 * @brief Access.
 *
 * @return the node pointer.
 */
const openvrml::sfnode::value_type & openvrml::sfnode::value() const throw ()
{
    return this->field_value::value<sfnode>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfnode::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfnode>(val);
}

/**
 * @brief Swap.
 *
 * @param sfn   the value to swap with this one.
 */
void openvrml::sfnode::swap(sfnode & sfn) throw ()
{
    this->field_value::swap<sfnode>(sfn);
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfnode::print(std::ostream & out) const
{
    if (this->value()) {
        out << *this->value();
    } else {
        out << "NULL";
    }
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfnode::do_clone() const
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
openvrml::sfnode & openvrml::sfnode::do_assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfnode &>(value));
}

/**
 * @brief Get the field_value::type_id associated with this class.
 *
 * @return @c field_value::sfnode.
 */
openvrml::field_value::type_id openvrml::sfnode::do_type() const throw ()
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
    return lhs.value() == rhs.value();
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
 * @brief Construct.
 *
 * @param rot   initial value.
 */
openvrml::sfrotation::sfrotation(const rotation & rot) throw ():
    field_value(rot, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfr   the instance to copy.
 */
openvrml::sfrotation::sfrotation(const sfrotation & sfr):
    field_value(sfr)
{}

/**
 * @brief Destroy.
 */
openvrml::sfrotation::~sfrotation() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfr   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfrotation & openvrml::sfrotation::operator=(const sfrotation & sfr)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sfr);
}

/**
 * @brief Access.
 *
 * @return the rotation value.
 */
const openvrml::sfrotation::value_type & openvrml::sfrotation::value() const
    throw ()
{
    return this->field_value::value<sfrotation>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfrotation::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfrotation>(val);
}

/**
 * @brief Swap.
 *
 * @param sfr   the value to swap with this one.
 */
void openvrml::sfrotation::swap(sfrotation & sfr) throw ()
{
    this->field_value::swap<sfrotation>(sfr);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfrotation::do_clone() const
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
openvrml::sfrotation & openvrml::sfrotation::do_assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfrotation &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfrotation.
 */
openvrml::field_value::type_id openvrml::sfrotation::do_type() const throw ()
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
    out << this->value();
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
    return lhs.value() == rhs.value();
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
 * @brief Construct.
 *
 * @param value
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfstring::sfstring(const std::string & value) throw (std::bad_alloc):
   field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfs   the instance to copy.
 */
openvrml::sfstring::sfstring(const sfstring & sfs):
    field_value(sfs)
{}

/**
 * @brief Destroy.
 */
openvrml::sfstring::~sfstring() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfs   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfstring & openvrml::sfstring::operator=(const sfstring & sfs)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sfs);
}

/**
 * @brief Access.
 *
 * @return the string value.
 */
const openvrml::sfstring::value_type & openvrml::sfstring::value() const
    throw ()
{
    return this->field_value::value<sfstring>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfstring::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfstring>(val);
}

/**
 * @brief Swap.
 *
 * @param sfs   the value to swap with this one.
 */
void openvrml::sfstring::swap(sfstring & sfs) throw ()
{
    this->field_value::swap<sfstring>(sfs);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfstring::do_clone() const
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
openvrml::sfstring & openvrml::sfstring::do_assign(const field_value & value)
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
    out << '\"' << this->value() << '\"';
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfstring.
 */
openvrml::field_value::type_id openvrml::sfstring::do_type() const throw ()
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
    return lhs.value() == rhs.value();
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
 * @brief Construct.
 *
 * @param value initial value
 */
openvrml::sftime::sftime(double value) throw ():
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sft   the instance to copy.
 */
openvrml::sftime::sftime(const sftime & sft):
    field_value(sft)
{}

/**
 * @brief Destroy.
 */
openvrml::sftime::~sftime() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sft   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sftime & openvrml::sftime::operator=(const sftime & sft)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sft);
}

/**
 * @brief Access.
 *
 * @return the time value.
 */
const openvrml::sftime::value_type & openvrml::sftime::value() const throw ()
{
    return this->field_value::value<sftime>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sftime::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sftime>(val);
}

/**
 * @brief Swap.
 *
 * @param sfb   the value to swap with this one.
 */
void openvrml::sftime::swap(sftime & sft) throw ()
{
    this->field_value::swap<sftime>(sft);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sftime::do_clone() const
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
openvrml::sftime & openvrml::sftime::do_assign(const field_value & value)
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
    out << this->value();
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sftime.
 */
openvrml::field_value::type_id openvrml::sftime::do_type() const throw ()
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
    return lhs.value() == rhs.value();
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
 * @brief Construct.
 *
 * @param vec   initial value.
 */
openvrml::sfvec2f::sfvec2f(const vec2f & vec) throw ():
    field_value(vec, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfv   the instance to copy.
 */
openvrml::sfvec2f::sfvec2f(const sfvec2f & sfv):
    field_value(sfv)
{}

/**
 * @brief Destroy.
 */
openvrml::sfvec2f::~sfvec2f() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfv   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfvec2f & openvrml::sfvec2f::operator=(const sfvec2f & sfv)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sfv);
}

/**
 * @brief Access.
 *
 * @return the vector value.
 */
const openvrml::sfvec2f::value_type & openvrml::sfvec2f::value() const throw ()
{
    return this->field_value::value<sfvec2f>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfvec2f::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfvec2f>(val);
}

/**
 * @brief Swap.
 *
 * @param sfv   the value to swap with this one.
 */
void openvrml::sfvec2f::swap(sfvec2f & sfv) throw ()
{
    this->field_value::swap<sfvec2f>(sfv);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfvec2f::do_clone() const
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
openvrml::sfvec2f & openvrml::sfvec2f::do_assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfvec2f &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfvec2f.
 */
openvrml::field_value::type_id openvrml::sfvec2f::do_type() const throw ()
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
    out << this->value();
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
    return lhs.value() == rhs.value();
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
 * @class openvrml::sfvec2d
 *
 * @ingroup fieldvalues
 *
 * @brief A 2-component vector node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::sfvec2d::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::sfvec2d::field_value_type_id(sfvec2d_id);

/**
 * @brief Construct.
 *
 * @param vec   initial value.
 */
openvrml::sfvec2d::sfvec2d(const vec2d & vec) throw ():
    field_value(vec, value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param vec   initial value.
 */
openvrml::sfvec2d::sfvec2d(const vec2f & vec) throw ():
    field_value(vec2d(vec[0], vec[1]), value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfv   the instance to copy.
 */
openvrml::sfvec2d::sfvec2d(const sfvec2d & sfv):
    field_value(sfv)
{}

/**
 * @brief Destroy.
 */
openvrml::sfvec2d::~sfvec2d() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfv   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfvec2d & openvrml::sfvec2d::operator=(const sfvec2d & sfv)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sfv);
}

/**
 * @brief Access.
 *
 * @return the vector value.
 */
const openvrml::sfvec2d::value_type & openvrml::sfvec2d::value() const throw ()
{
    return this->field_value::value<sfvec2d>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfvec2d::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfvec2d>(val);
}

/**
 * @brief Swap.
 *
 * @param sfv   the value to swap with this one.
 */
void openvrml::sfvec2d::swap(sfvec2d & sfv) throw ()
{
    this->field_value::swap<sfvec2d>(sfv);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfvec2d::do_clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfvec2d(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @param value the new value to give the object.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast if @p value is not an sfvec2d.
 */
openvrml::sfvec2d & openvrml::sfvec2d::do_assign(const field_value & value)
    throw (std::bad_cast)
{
    //allow to convert from a float to a double
    const sfvec2f * float_val = dynamic_cast<const sfvec2f *>(&value);
    if (float_val != NULL)
    {
        this->value(vec2d(float_val->value()[0], float_val->value()[1]));
        return *this;
    }

    return (*this = dynamic_cast<const sfvec2d &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfvec2d.
 */
openvrml::field_value::type_id openvrml::sfvec2d::do_type() const throw ()
{
    return field_value::sfvec2d_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfvec2d::print(std::ostream & out) const
{
    out << this->value();
}

/**
 * @relatesalso openvrml::sfvec2d
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfvec2d & lhs, const sfvec2d & rhs) throw ()
{
    return lhs.value() == rhs.value();
}

/**
 * @relatesalso openvrml::sfvec2d
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const sfvec2d & lhs, const sfvec2d & rhs) throw ()
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
 * @brief Construct.
 *
 * @param vec   initial value.
 */
openvrml::sfvec3f::sfvec3f(const vec3f & vec) throw ():
    field_value(vec, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfv   the instance to copy.
 */
openvrml::sfvec3f::sfvec3f(const sfvec3f & sfv):
    field_value(sfv)
{}

/**
 * @brief Destroy.
 */
openvrml::sfvec3f::~sfvec3f() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfv   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfvec3f & openvrml::sfvec3f::operator=(const sfvec3f & sfv)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sfv);
}

/**
 * @brief Access.
 *
 * @return the vector value.
 */
const openvrml::sfvec3f::value_type & openvrml::sfvec3f::value() const throw ()
{
    return this->field_value::value<sfvec3f>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfvec3f::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfvec3f>(val);
}

/**
 * @brief Swap.
 *
 * @param sfv   the value to swap with this one.
 */
void openvrml::sfvec3f::swap(sfvec3f & sfv) throw ()
{
    this->field_value::swap<sfvec3f>(sfv);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfvec3f::do_clone() const
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
openvrml::sfvec3f & openvrml::sfvec3f::do_assign(const field_value & value)
    throw (std::bad_cast)
{
    return (*this = dynamic_cast<const sfvec3f &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfvec3f.
 */
openvrml::field_value::type_id openvrml::sfvec3f::do_type() const throw ()
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
    out << this->value();
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
    return lhs.value() == rhs.value();
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
 * @class openvrml::sfvec3d
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
openvrml::sfvec3d::field_value_type_id(sfvec3d_id);

/**
 * @typedef openvrml::sfvec3d::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief Construct.
 *
 * @param vec   initial value.
 */
openvrml::sfvec3d::sfvec3d(const vec3d & vec) throw ():
    field_value(vec, value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param vec   initial value.
 */
openvrml::sfvec3d::sfvec3d(const vec3f & vec) throw ():
    field_value(vec3d(vec[0], vec[1], vec[2]), value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param sfv   the instance to copy.
 */
openvrml::sfvec3d::sfvec3d(const sfvec3d & sfv):
    field_value(sfv)
{}

/**
 * @brief Destroy.
 */
openvrml::sfvec3d::~sfvec3d() throw ()
{}

/**
 * @brief Assign.
 *
 * @param sfv   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::sfvec3d & openvrml::sfvec3d::operator=(const sfvec3d & sfv)
     throw (std::bad_alloc)
{
    return this->field_value::operator=(sfv);
}

/**
 * @brief Access.
 *
 * @return the vector value.
 */
const openvrml::sfvec3d::value_type & openvrml::sfvec3d::value() const throw ()
{
    return this->field_value::value<sfvec3d>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::sfvec3d::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<sfvec3d>(val);
}

/**
 * @brief Swap.
 *
 * @param sfv   the value to swap with this one.
 */
void openvrml::sfvec3d::swap(sfvec3d & sfv) throw ()
{
    this->field_value::swap<sfvec3d>(sfv);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::sfvec3d::do_clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new sfvec3d(*this));
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
openvrml::sfvec3d & openvrml::sfvec3d::do_assign(const field_value & value)
    throw (std::bad_cast)
{
    //allow to convert from a float to a double
    const sfvec3f * float_val = dynamic_cast<const sfvec3f *>(&value);
    if (!float_val) {
        return *this = sfvec3d(float_val->value());
    }
    
    return (*this = dynamic_cast<const sfvec3d &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::sfvec3d.
 */
openvrml::field_value::type_id openvrml::sfvec3d::do_type() const throw ()
{
    return field_value::sfvec3d_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::sfvec3d::print(std::ostream & out) const
{
    out << this->value();
}

/**
 * @relatesalso openvrml::sfvec3d
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const sfvec3d & lhs, const sfvec3d & rhs) throw ()
{
    return lhs.value() == rhs.value();
}

/**
 * @relatesalso openvrml::sfvec3d
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const sfvec3d & lhs, const sfvec3d & rhs) throw ()
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
    field_value(std::vector<color>(n, value), value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfcolor::mfcolor(const value_type & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param mfc   the instance to copy.
 */
openvrml::mfcolor::mfcolor(const mfcolor & mfc):
    field_value(mfc)
{}

/**
 * @brief Destroy.
 *
 * Each of the <code>mfcolor</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfcolor::~mfcolor() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mfc   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfcolor & openvrml::mfcolor::operator=(const mfcolor & mfc)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mfc);
}

/**
 * @brief Access.
 *
 * @return the color values.
 */
const openvrml::mfcolor::value_type & openvrml::mfcolor::value() const throw ()
{
    return this->field_value::value<mfcolor>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mfcolor::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mfcolor>(val);
}

/**
 * @brief Swap.
 *
 * @param mfc   the value to swap with this one.
 */
void openvrml::mfcolor::swap(mfcolor & mfc) throw ()
{
    this->field_value::swap<mfcolor>(mfc);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfcolor::do_clone() const
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
openvrml::mfcolor & openvrml::mfcolor::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfcolor &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfcolor.
 */
openvrml::field_value::type_id openvrml::mfcolor::do_type() const throw ()
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
    if (this->value().size() > 1) {
        for (std::vector<color>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value().empty()) {
        out << this->value().back();
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
    return lhs.value() == rhs.value();
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
    return !(lhs == rhs);
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
    field_value(std::vector<float>(n, value), value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mffloat::mffloat(const value_type & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param mff   the instance to copy.
 */
openvrml::mffloat::mffloat(const mffloat & mff):
    field_value(mff)
{}

/**
 * @brief Destroy.
 *
 * Each of the <code>mffloat</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mffloat::~mffloat() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mff   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mffloat & openvrml::mffloat::operator=(const mffloat & mff)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mff);
}

/**
 * @brief Access.
 *
 * @return the floating point values.
 */
const openvrml::mffloat::value_type & openvrml::mffloat::value() const throw ()
{
    return this->field_value::value<mffloat>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mffloat::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mffloat>(val);
}

/**
 * @brief Swap.
 *
 * @param mff   the value to swap with this one.
 */
void openvrml::mffloat::swap(mffloat & mff) throw ()
{
    this->field_value::swap<mffloat>(mff);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mffloat::do_clone() const
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
openvrml::mffloat & openvrml::mffloat::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mffloat &>(value));
}

/**
 * @brief Get the type identifier for the class.
 *
 * @return @c field_value::mffloat_id.
 */
openvrml::field_value::type_id openvrml::mffloat::do_type() const throw ()
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
    if (this->value().size() > 1) {
        for (std::vector<float>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value().empty()) {
        out << this->value().back();
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
    return lhs.value() == rhs.value();
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
    return !(lhs == rhs);
}

/**
 * @class openvrml::mfdouble
 *
 * @ingroup fieldvalues
 *
 * @brief A double array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mfdouble::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mfdouble::field_value_type_id(mfdouble_id);

/**
 * @brief Construct.
 *
 * Creates an <code>mfdouble</code> with @p n copies of @p value.
 *
 * @param n     the number of elements in the <code>mfdouble</code>.
 * @param value used to initialize the <code>mfdouble</code>.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post size is @p n. Every element is a copy of @p value.
 */
openvrml::mfdouble::mfdouble(const std::vector<double>::size_type n,
                           const double value)
    throw (std::bad_alloc):
    field_value(std::vector<double>(n, value), value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfdouble::mfdouble(const value_type & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfdouble::mfdouble(const std::vector<float> & value)
    throw (std::bad_alloc):
    field_value(std::vector<double>(), value_type_constructor_tag())
{
    std::vector<double> temp(value.begin(), value.end());
    this->value(temp);
}

/**
 * @brief Construct a copy.
 *
 * @param mfd   the instance to copy.
 */
openvrml::mfdouble::mfdouble(const mfdouble & mfd):
    field_value(mfd)
{}

/**
 * @brief Destroy.
 *
 * Each of the <code>mfdouble</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfdouble::~mfdouble() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mfd   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfdouble & openvrml::mfdouble::operator=(const mfdouble & mfd)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mfd);
}

/**
 * @brief Access.
 *
 * @return the double-precision values.
 */
const openvrml::mfdouble::value_type & openvrml::mfdouble::value() const
    throw ()
{
    return this->field_value::value<mfdouble>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mfdouble::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mfdouble>(val);
}

/**
 * @brief Swap.
 *
 * @param mfd   the value to swap with this one.
 */
void openvrml::mfdouble::swap(mfdouble & mfd) throw ()
{
    this->field_value::swap<mfdouble>(mfd);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfdouble::do_clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mfdouble(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an mfdouble object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfdouble & openvrml::mfdouble::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    //allow to convert from a float to a double
    const mffloat * float_val = dynamic_cast<const mffloat *>(&value);
    if (!float_val) {
        return *this = mfdouble(float_val->value());
    }
    
    return (*this = dynamic_cast<const mfdouble &>(value));
}

/**
 * @brief Get the type identifier for the class.
 *
 * @return @c field_value::mfdouble_id.
 */
openvrml::field_value::type_id openvrml::mfdouble::do_type() const throw ()
{
    return field_value::mfdouble_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::mfdouble::print(std::ostream & out) const
{
    out << '[';
    if (this->value().size() > 1) {
        for (std::vector<double>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value().empty()) {
        out << this->value().back();
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mfdouble
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mfdouble & lhs, const mfdouble & rhs) throw ()
{
    return lhs.value() == rhs.value();
}

/**
 * @relatesalso openvrml::mfdouble
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const mfdouble & lhs, const mfdouble & rhs) throw ()
{
    return !(lhs == rhs);
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
    field_value(std::vector<int32>(n, value), value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfint32::mfint32(const value_type & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param mfi   the instance to copy.
 */
openvrml::mfint32::mfint32(const mfint32 & mfi):
    field_value(mfi)
{}

/**
 * @brief Destroy.
 *
 * Each of the <code>mfint32</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfint32::~mfint32() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mfi   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfint32 & openvrml::mfint32::operator=(const mfint32 & mfi)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mfi);
}

/**
 * @brief Access.
 *
 * @return the integer values.
 */
const openvrml::mfint32::value_type & openvrml::mfint32::value() const throw ()
{
    return this->field_value::value<mfint32>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mfint32::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mfint32>(val);
}

/**
 * @brief Swap.
 *
 * @param mfi   the value to swap with this one.
 */
void openvrml::mfint32::swap(mfint32 & mfi) throw ()
{
    this->field_value::swap<mfint32>(mfi);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfint32::do_clone() const
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
openvrml::mfint32 & openvrml::mfint32::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfint32 &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfint32_id.
 */
openvrml::field_value::type_id openvrml::mfint32::do_type() const throw ()
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
    if (this->value().size() > 1) {
        for (std::vector<int32>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value().empty()) {
        out << this->value().back();
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
    return lhs.value() == rhs.value();
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
    return !(lhs == rhs);
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
    field_value(std::vector<node_ptr>(n, value), value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfnode::mfnode(const value_type & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param mfn   the instance to copy.
 */
openvrml::mfnode::mfnode(const mfnode & mfn):
    field_value(mfn)
{}

/**
 * @brief Destroy.
 *
 * Each of the <code>mfnode</code>'s @a value elements is destroyed, and memory
 * allocated for them (if any) is deallocated.
 */
openvrml::mfnode::~mfnode() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mfn   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfnode & openvrml::mfnode::operator=(const mfnode & mfn)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mfn);
}

/**
 * @brief Access.
 *
 * @return the node pointers.
 */
const openvrml::mfnode::value_type & openvrml::mfnode::value() const throw ()
{
    return this->field_value::value<mfnode>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mfnode::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mfnode>(val);
}

/**
 * @brief Swap.
 *
 * @param mfn   the value to swap with this one.
 */
void openvrml::mfnode::swap(mfnode & mfn) throw ()
{
    this->field_value::swap<mfnode>(mfn);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfnode::do_clone() const
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
openvrml::mfnode & openvrml::mfnode::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfnode &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfnode_id.
 */
openvrml::field_value::type_id openvrml::mfnode::do_type() const throw ()
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
    if (this->value().size() > 1) {
        for (std::vector<node_ptr>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            if (*i) { out << **i << ", "; }
        }
    }
    if (!this->value().empty() && this->value().back()) {
        out << *this->value().back();
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
    return lhs.value() == rhs.value();
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
    return !(lhs == rhs);
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
    field_value(std::vector<rotation>(n, value), value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfrotation::mfrotation(const value_type & value)
    throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param mfr   the instance to copy.
 */
openvrml::mfrotation::mfrotation(const mfrotation & mfr):
    field_value(mfr)
{}

/**
 * @brief Destroy.
 *
 * Each of the <code>mfrotation</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfrotation::~mfrotation() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mfr   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfrotation & openvrml::mfrotation::operator=(const mfrotation & mfr)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mfr);
}

/**
 * @brief Access.
 *
 * @return the rotation values.
 */
const openvrml::mfrotation::value_type & openvrml::mfrotation::value() const
    throw ()
{
    return this->field_value::value<mfrotation>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mfrotation::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mfrotation>(val);
}

/**
 * @brief Swap.
 *
 * @param mfr   the value to swap with this one.
 */
void openvrml::mfrotation::swap(mfrotation & mfr) throw ()
{
    this->field_value::swap<mfrotation>(mfr);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfrotation::do_clone() const
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
openvrml::mfrotation & openvrml::mfrotation::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfrotation &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfrotation_id.
 */
openvrml::field_value::type_id openvrml::mfrotation::do_type() const throw ()
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
    if (this->value().size() > 1) {
        for (std::vector<rotation>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value().empty()) {
        out << this->value().back();
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
    return lhs.value() == rhs.value();
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
    return !(lhs == rhs);
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
    field_value(std::vector<std::string>(n, value),
                value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfstring::mfstring(const value_type & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param mfs   the instance to copy.
 */
openvrml::mfstring::mfstring(const mfstring & mfs):
    field_value(mfs)
{}

/**
 * @brief Destroy.
 *
 * Each of the <code>mfstring</code>'s elements is destroyed, and memory
 * allocated for them (if any) is deallocated.
 */
openvrml::mfstring::~mfstring() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mfs   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfstring & openvrml::mfstring::operator=(const mfstring & mfs)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mfs);
}

/**
 * @brief Access.
 *
 * @return the string values.
 */
const openvrml::mfstring::value_type & openvrml::mfstring::value() const
    throw ()
{
    return this->field_value::value<mfstring>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mfstring::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mfstring>(val);
}

/**
 * @brief Swap.
 *
 * @param mfs   the value to swap with this one.
 */
void openvrml::mfstring::swap(mfstring & mfs) throw ()
{
    this->field_value::swap<mfstring>(mfs);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfstring::do_clone() const
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
openvrml::mfstring & openvrml::mfstring::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfstring &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfstring_id.
 */
openvrml::field_value::type_id openvrml::mfstring::do_type() const throw ()
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
    if (this->value().size() > 1) {
        for (std::vector<std::string>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            out << '\"' << *i << "\", ";
        }
    }
    if (!this->value().empty()) {
        out << '\"' << this->value().back() << '\"';
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
    return lhs.value() == rhs.value();
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
    return !(lhs == rhs);
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
    field_value(std::vector<double>(n, value), value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mftime::mftime(const value_type & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param mft   the instance to copy.
 */
openvrml::mftime::mftime(const mftime & mft):
    field_value(mft)
{}

/**
 * @brief Destroy.
 *
 * Each of the openvrml::mftime's @a value elements is destroyed, and memory
 * allocated for them (if any) is deallocated.
 */
openvrml::mftime::~mftime() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mft   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mftime & openvrml::mftime::operator=(const mftime & mft)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mft);
}

/**
 * @brief Access.
 *
 * @return the time values.
 */
const openvrml::mftime::value_type & openvrml::mftime::value() const throw ()
{
    return this->field_value::value<mftime>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mftime::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mftime>(val);
}

/**
 * @brief Swap.
 *
 * @param mft   the value to swap with this one.
 */
void openvrml::mftime::swap(mftime & mft) throw ()
{
    this->field_value::swap<mftime>(mft);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value>
openvrml::mftime::do_clone() const throw (std::bad_alloc)
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
openvrml::mftime & openvrml::mftime::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mftime &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mftime_id.
 */
openvrml::field_value::type_id openvrml::mftime::do_type() const throw ()
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
    if (this->value().size() > 1) {
        for (std::vector<double>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value().empty()) {
        out << this->value().back();
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
    return lhs.value() == rhs.value();
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
    return !(lhs == rhs);
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
    field_value(std::vector<vec2f>(n, value), value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec2f::mfvec2f(const value_type & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param mfv   the instance to copy.
 */
openvrml::mfvec2f::mfvec2f(const mfvec2f & mfv):
    field_value(mfv)
{}

/**
 * @brief Destroy.
 *
 * Each of the <code>mfvec2f</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfvec2f::~mfvec2f() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mfv   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec2f & openvrml::mfvec2f::operator=(const mfvec2f & mfv)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mfv);
}

/**
 * @brief Access.
 *
 * @return the vector values.
 */
const openvrml::mfvec2f::value_type & openvrml::mfvec2f::value() const throw ()
{
    return this->field_value::value<mfvec2f>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mfvec2f::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mfvec2f>(val);
}

/**
 * @brief Swap.
 *
 * @param mfv   the value to swap with this one.
 */
void openvrml::mfvec2f::swap(mfvec2f & mfv) throw ()
{
    this->field_value::swap<mfvec2f>(mfv);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value>
openvrml::mfvec2f::do_clone() const throw (std::bad_alloc)
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
openvrml::mfvec2f & openvrml::mfvec2f::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfvec2f &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfvec2f_id.
 */
openvrml::field_value::type_id openvrml::mfvec2f::do_type() const throw ()
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
    if (this->value().size() > 1) {
        for (std::vector<vec2f>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value().empty()) {
        out << this->value().back();
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
    return lhs.value() == rhs.value();
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
    return !(lhs == rhs);
}

/**
 * @class openvrml::mfvec2d
 *
 * @ingroup fieldvalues
 *
 * @brief A 2-component vector array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mfvec2d::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mfvec2d::field_value_type_id(mfvec2d_id);

/**
 * @brief Construct.
 *
 * Creates an <code>mfvec2d</code> with @p n copies of @p value.
 *
 * @param n     the number elements in @a mfvec2d::value.
 * @param value used to initialize @a mfvec2d::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>mfvec2d::value.size()</code> is @p n. Every element in
 *      @a mfvec2d::value is a copy of @p value.
 */
openvrml::mfvec2d::mfvec2d(const std::vector<vec2d>::size_type n,
                           const vec2d & value)
    throw (std::bad_alloc):
    field_value(std::vector<vec2d>(n, value), value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec2d::mfvec2d(const value_type & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec2d::mfvec2d(const std::vector<vec2f> & value)
    throw (std::bad_alloc):
    field_value(std::vector<vec2d>(), value_type_constructor_tag())
{
    std::vector<vec2d> temp(value.size());
    for (std::vector<vec2d>::size_type i = 0; i < temp.size(); ++i) {
        temp[i] = vec2d(value[i].x(), value[i].y());
    }
    this->value(temp);
}

/**
 * @brief Construct a copy.
 *
 * @param mfv   the instance to copy.
 */
openvrml::mfvec2d::mfvec2d(const mfvec2d & mfv):
    field_value(mfv)
{}

/**
 * @brief Destroy.
 *
 * Each of the <code>mfvec2d</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfvec2d::~mfvec2d() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mfv   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec2d & openvrml::mfvec2d::operator=(const mfvec2d & mfv)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mfv);
}

/**
 * @brief Access.
 *
 * @return the vector values.
 */
const openvrml::mfvec2d::value_type & openvrml::mfvec2d::value() const throw ()
{
    return this->field_value::value<mfvec2d>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mfvec2d::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mfvec2d>(val);
}

/**
 * @brief Swap.
 *
 * @param mfv   the value to swap with this one.
 */
void openvrml::mfvec2d::swap(mfvec2d & mfv) throw ()
{
    this->field_value::swap<mfvec2d>(mfv);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value>
openvrml::mfvec2d::do_clone() const throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mfvec2d(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if  @p value is not an mfvec2d object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec2d & openvrml::mfvec2d::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    //allow to convert from a float to a double
    const mfvec2f * float_val = dynamic_cast<const mfvec2f *>(&value);
    if (!float_val) {
        return *this = mfvec2d(float_val->value());
    }
    
    return (*this = dynamic_cast<const mfvec2d &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfvec2d_id.
 */
openvrml::field_value::type_id openvrml::mfvec2d::do_type() const throw ()
{
    return field_value::mfvec2d_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::mfvec2d::print(std::ostream & out) const
{
    out << '[';
    if (this->value().size() > 1) {
        for (std::vector<vec2d>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value().empty()) {
        out << this->value().back();
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mfvec2d
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mfvec2d & lhs, const mfvec2d & rhs) throw ()
{
    return lhs.value() == rhs.value();
}

/**
 * @relatesalso openvrml::mfvec2d
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const mfvec2d & lhs, const mfvec2d & rhs) throw ()
{
    return !(lhs == rhs);
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
    field_value(std::vector<vec3f>(n, value), value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec3f::mfvec3f(const value_type & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct a copy.
 *
 * @param mfv   the instance to copy.
 */
openvrml::mfvec3f::mfvec3f(const mfvec3f & mfv):
    field_value(mfv)
{}

/**
 * @brief Destroy.
 *
 * Each of the <code>mfvec3f</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfvec3f::~mfvec3f() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mfv   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec3f & openvrml::mfvec3f::operator=(const mfvec3f & mfv)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mfv);
}

/**
 * @brief Access.
 *
 * @return the vector values.
 */
const openvrml::mfvec3f::value_type & openvrml::mfvec3f::value() const throw ()
{
    return this->field_value::value<mfvec3f>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mfvec3f::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mfvec3f>(val);
}

/**
 * @brief Swap.
 *
 * @param mfv   the value to swap with this one.
 */
void openvrml::mfvec3f::swap(mfvec3f & mfv) throw ()
{
    this->field_value::swap<mfvec3f>(mfv);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfvec3f::do_clone() const
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
openvrml::mfvec3f & openvrml::mfvec3f::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    return (*this = dynamic_cast<const mfvec3f &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfvec3f_id.
 */
openvrml::field_value::type_id openvrml::mfvec3f::do_type() const throw ()
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
    if (this->value().size() > 1) {
        for (std::vector<vec3f>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value().empty()) {
        out << this->value().back();
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
    return lhs.value() == rhs.value();
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
    return !(lhs == rhs);
}

/**
 * @class openvrml::mfvec3d
 *
 * @ingroup fieldvalues
 *
 * @brief A 3-component vector array node field value.
 *
 * @par Model of
 * @link openvrml::FieldValueConcept Field Value@endlink
 */

/**
 * @typedef openvrml::mfvec3d::value_type
 *
 * @brief Type of @a value.
 */

/**
 * @brief <code>field_value::type_id</code> for this class.
 */
const openvrml::field_value::type_id
openvrml::mfvec3d::field_value_type_id(mfvec3d_id);

/**
 * @brief Construct.
 *
 * Creates an <code>mfvec3d</code> with @p n copies of @p value.
 *
 * @param n     the number elements in @a mfvec3d::value.
 * @param value used to initialize @a mfvec3d::value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 *
 * @post <code>mfvec3d::value.size()</code> is @p n. Every element in
 *      @a mfvec3d::value is a copy of @p value.
 */
openvrml::mfvec3d::mfvec3d(const std::vector<vec3d>::size_type n,
                           const vec3d & value)
    throw (std::bad_alloc):
    field_value(std::vector<vec3d>(n, value), value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec3d::mfvec3d(const value_type & value) throw (std::bad_alloc):
    field_value(value, value_type_constructor_tag())
{}

/**
 * @brief Construct.
 *
 * @param value initial value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec3d::mfvec3d(const std::vector<vec3f> & value)
    throw (std::bad_alloc):
    field_value(std::vector<vec3d>(), value_type_constructor_tag())
{
    std::vector<vec3d> temp(value.size());
    for (std::vector<vec3d>::size_type i = 0; i < temp.size(); ++i) {
        temp[i] = vec3d(value[i].x(), value[i].y(), value[i].z());
    }
    this->value(temp);
}

/**
 * @brief Construct a copy.
 *
 * @param mfv   the instance to copy.
 */
openvrml::mfvec3d::mfvec3d(const mfvec3d & mfv):
    field_value(mfv)
{}

/**
 * @brief Destroy.
 *
 * Each of the <code>mfvec3d</code>'s @a value elements is destroyed, and
 * memory allocated for them (if any) is deallocated.
 */
openvrml::mfvec3d::~mfvec3d() throw ()
{}

/**
 * @brief Assign.
 *
 * @param mfv   the value to assign.
 *
 * @return a reference to the instance.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec3d & openvrml::mfvec3d::operator=(const mfvec3d & mfv)
    throw (std::bad_alloc)
{
    return this->field_value::operator=(mfv);
}

/**
 * @brief Access.
 *
 * @return the vector values.
 */
const openvrml::mfvec3d::value_type & openvrml::mfvec3d::value() const throw ()
{
    return this->field_value::value<mfvec3d>();
}

/**
 * @brief Mutate.
 *
 * @param val   the new value.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
void openvrml::mfvec3d::value(const value_type & val) throw (std::bad_alloc)
{
    this->field_value::value<mfvec3d>(val);
}

/**
 * @brief Swap.
 *
 * @param mfv   the value to swap with this one.
 */
void openvrml::mfvec3d::swap(mfvec3d & mfv) throw ()
{
    this->field_value::swap<mfvec3d>(mfv);
}

/**
 * @brief Polymorphically construct a copy.
 *
 * @return a pointer to a copy of the object.
 *
 * @exception std::bad_alloc    if memory allocation fails.
 */
std::auto_ptr<openvrml::field_value> openvrml::mfvec3d::do_clone() const
    throw (std::bad_alloc)
{
    return std::auto_ptr<field_value>(new mfvec3d(*this));
}

/**
 * @brief Virtual assignment.
 *
 * @return a reference to the object.
 *
 * @exception std::bad_cast     if @p value is not an <code>mfvec3d</code>
 *                              object.
 * @exception std::bad_alloc    if memory allocation fails.
 */
openvrml::mfvec3d & openvrml::mfvec3d::do_assign(const field_value & value)
    throw (std::bad_cast, std::bad_alloc)
{
    //allow to convert from a float to a double
    const mfvec3f * float_val = dynamic_cast<const mfvec3f *>(&value);
    if (!float_val) {
        return *this = mfvec3d(float_val->value());
    }
    
    return (*this = dynamic_cast<const mfvec3d &>(value));
}

/**
 * @brief Get the <code>field_value::type_id</code> associated with this class.
 *
 * @return @c field_value::mfvec3d_id.
 */
openvrml::field_value::type_id openvrml::mfvec3d::do_type() const throw ()
{
    return field_value::mfvec3d_id;
}

/**
 * @brief Print to an output stream.
 *
 * @param out   an output stream.
 */
void openvrml::mfvec3d::print(std::ostream & out) const
{
    out << '[';
    if (this->value().size() > 1) {
        for (std::vector<vec3d>::const_iterator i(this->value().begin());
             i != this->value().end() - 1; ++i) {
            out << *i << ", ";
        }
    }
    if (!this->value().empty()) {
        out << this->value().back();
    }
    out << ']';
}

/**
 * @relatesalso openvrml::mfvec3d
 *
 * @brief Compare for equality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs have the same value; @c false
 *         otherwise.
 */
bool openvrml::operator==(const mfvec3d & lhs, const mfvec3d & rhs) throw ()
{
    return lhs.value() == rhs.value();
}

/**
 * @relatesalso openvrml::mfvec3d
 *
 * @brief Compare for inequality.
 *
 * @param lhs   left-hand operand.
 * @param rhs   right-hand operand.
 *
 * @return @c true if @p lhs and @p rhs do not have the same value; @c false
 *      otherwise.
 */
bool openvrml::operator!=(const mfvec3d & lhs, const mfvec3d & rhs) throw ()
{
    return !(lhs == rhs);
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
