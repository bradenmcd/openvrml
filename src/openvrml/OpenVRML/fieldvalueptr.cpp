// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 2000  Braden McDaniel
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

# include "fieldvalueptr.h"
# include "field.h"

namespace OpenVRML {

/**
 * @class FieldValuePtr
 *
 * @brief FieldValuePtr is a reference-counting smart pointer for FieldValues.
 *
 * FieldValuePtr is patterned after the <a
 * href="http://boost.org/libs/smart_ptr/shared_ptr.htm">Boost shared_ptr</a>,
 * and it works basically the same way. A FieldValuePtr should be constructed
 * or reset with a FieldValue created with @c new (or 0).
 *
 * @see http://boost.org/libs/smart_ptr/shared_ptr.htm
 */

/**
 * @brief Constructor.
 *
 * @param fieldValue a pointer to a FieldValue constructed with @c new.
 */
FieldValuePtr::FieldValuePtr(field_value * const fieldValue):
    fieldValue(fieldValue)
{
    try {
        this->count = new size_t(1); // prevent leak if new throws
    } catch (...) {
        delete fieldValue;
        throw;
    }
}

/**
 * @brief Copy constructor.
 *
 * @param fieldValuePtr the FieldValuePtr to copy.
 */
FieldValuePtr::FieldValuePtr(const FieldValuePtr & fieldValuePtr):
    fieldValue(fieldValuePtr.fieldValue)
{
    ++*(this->count = fieldValuePtr.count); // never throws
}

/**
 * @brief Assignment operator.
 *
 * @param fieldValuePtr
 */
FieldValuePtr & FieldValuePtr::operator=(const FieldValuePtr & fieldValuePtr)
{
    if (this->count != fieldValuePtr.count) {
        ++*fieldValuePtr.count;
        this->dispose();
        this->fieldValue = fieldValuePtr.fieldValue;
        this->count = fieldValuePtr.count;
    }
    return *this;
}

/**
 * @brief Reset the FieldValuePtr to a new FieldValue.
 *
 * @param fieldValue a pointer to a FieldValue constructed with @c new.
 */
void FieldValuePtr::reset(field_value * const fieldValue)
{
    if (this->fieldValue == fieldValue) { return; }
    if (--*this->count == 0) {
        delete this->fieldValue;
    } else {
        try {
            this->count = new size_t;
        } catch (...) {
            ++*this->count;
            delete fieldValue;
            throw;
        }
    }
    *this->count = 1;
    this->fieldValue = fieldValue;
}

void FieldValuePtr::dispose()
{
    if (--*this->count == 0) {
        delete this->fieldValue;
        delete this->count;
    }
}

} // namespace OpenVRML
