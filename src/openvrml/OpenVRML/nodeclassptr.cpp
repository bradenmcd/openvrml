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

# include "nodeclassptr.h"
# include "node.h"

namespace OpenVRML {

/**
 * @class NodeClassPtr
 *
 * @brief NodeClassPtr is a reference-counting smart pointer for NodeClasss.
 *
 * NodeClassPtr is patterned after the <a href="http://boost.org/libs/smart_ptr/shared_ptr.htm">Boost shared_ptr</a>,
 * and it works basically the same way. A NodeClassPtr should be constructed
 * or reset with a node_class created with @c new (or 0).
 *
 * @see http://boost.org/libs/smart_ptr/shared_ptr.htm
 */

/**
 * @brief Constructor.
 *
 * @param nodeClass a pointer to a NodeClass constructed with @c new.
 */
NodeClassPtr::NodeClassPtr(node_class * const nodeClass):
    nodeClass(nodeClass)
{
    try {
        this->count = new size_t(1); // prevent leak if new throws
    } catch (...) {
        delete nodeClass;
        throw;
    }
}

/**
 * @brief Copy constructor.
 *
 * @param nodeClassPtr the NodeClassPtr to copy.
 */
NodeClassPtr::NodeClassPtr(const NodeClassPtr & nodeClassPtr):
    nodeClass(nodeClassPtr.nodeClass)
{
    ++*(this->count = nodeClassPtr.count); // never throws
}

/**
 * @brief Assignment operator.
 *
 * @param nodeClassPtr
 */
NodeClassPtr & NodeClassPtr::operator=(const NodeClassPtr & nodeClassPtr)
{
    if (this->count != nodeClassPtr.count) {
        ++*nodeClassPtr.count;
        this->dispose();
        this->nodeClass = nodeClassPtr.nodeClass;
        this->count = nodeClassPtr.count;
    }
    return *this;
}

/**
 * @brief Reset the NodeClassPtr to a new NodeClass.
 *
 * @param nodeClass a pointer to a node_class constructed with @c new.
 */
void NodeClassPtr::reset(node_class * const nodeClass)
{
    if (this->nodeClass == nodeClass) { return; }
    if (--*this->count == 0) {
        delete this->nodeClass;
    } else {
        try {
            this->count = new size_t;
        } catch (...) {
            ++*this->count;
            delete nodeClass;
            throw;
        }
    }
    *this->count = 1;
    this->nodeClass = nodeClass;
}

void NodeClassPtr::dispose()
{
    if (--*this->count == 0) {
        delete this->nodeClass;
        delete this->count;
    }
}

} // namespace OpenVRML
