// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 2002  Braden McDaniel
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

# ifndef OPENVRML_FIELDVALUEPTR_H
#   define OPENVRML_FIELDVALUEPTR_H

#   include <stddef.h>
#   include <assert.h>
#   include "common.h"

namespace OpenVRML {

    class FieldValue;

    class OPENVRML_SCOPE FieldValuePtr {
        FieldValue * fieldValue;
        size_t * count;

    public:
        explicit FieldValuePtr(FieldValue * fieldValue = 0);
        FieldValuePtr(const FieldValuePtr & fieldValuePtr);
        ~FieldValuePtr();

        operator bool() const;

        FieldValuePtr & operator=(const FieldValuePtr & fieldValuePtr);

        bool operator==(const FieldValuePtr & fieldValuePtr) const;

        FieldValue & operator*() const;
        FieldValue * operator->() const;
        FieldValue * get() const;

        void reset(FieldValue * fieldValue = 0);

    private:
        void dispose();
    };

    inline FieldValuePtr::~FieldValuePtr() {
        this->dispose();
    }

    inline FieldValuePtr::operator bool() const {
        return this->fieldValue;
    }

    inline bool FieldValuePtr::operator==(const FieldValuePtr & fieldValuePtr) const {
        return (this->fieldValue == fieldValuePtr.fieldValue);
    }

    inline FieldValue & FieldValuePtr::operator*() const {
        assert(this->fieldValue);
        return *this->fieldValue;
    }

    inline FieldValue * FieldValuePtr::operator->() const {
        assert(this->fieldValue);
        return this->fieldValue;
    }

    inline FieldValue * FieldValuePtr::get() const {
        assert(this->fieldValue);
        return this->fieldValue;
    }
}

# endif
