//
// OpenVRML
//
// Copyright (C) 1998  Chris Morley
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
#ifndef VRMLNAMESPACE_H
#define VRMLNAMESPACE_H

//
// The VrmlNamespace class keeps track of defined nodes and prototypes.
//

#include <list>
#include "common.h"
#include "field.h"

class VrmlNode;
class VrmlNodeType;

OPENVRML_SCOPE class VrmlNamespace {
public:

  VrmlNamespace( VrmlNamespace *parent = 0 );
  ~VrmlNamespace();

  // PROTO definitions add node types to the namespace.
  // PROTO implementations are a separate node type namespace,
  // and require that any nested PROTOs NOT be available outside
  // the PROTO implementation. PROTOs defined outside the current
  // namespace are available.
  
  // addNodeType will print an error if the given type
  // is already defined (spec says behavior is undefined).
  void addNodeType(VrmlNodeType *);

  // DEFd nodes add node names to the namespace.
  // Node names are only defined in the current name space. They
  // are not available outside of the PROTO they are defined in,
  // nor are they available inside of nested PROTOs.

  void addNodeName(const VrmlNodePtr &);
  void removeNodeName(const VrmlNode &);

  // Find a node type, given a type name. Returns NULL if type is not defined.
  const VrmlNodeType *findType(const char *nm);
  
  // Larry Feb 10_99  Find a nodeType, given a PROTO name 
  const VrmlNodeType *findPROTO(const char *nm);
 
  // Return the first node type in scope (default EXTERNPROTO implementation)
  const VrmlNodeType *firstType();
  
  // Find a node by name.
  const VrmlNodePtr findNode(const char * name) const;
  
  const VrmlMFNode cloneNodes(const VrmlMFNode & mfnode);
  
private:

  void addBuiltIn(VrmlNodeType *);
  void defineBuiltIns();

  // Builtin node types are stored (once) in this data structure:
  static std::list< VrmlNodeType* > builtInList;

  // Ref count of namespaces so builtins can be freed
  static int s_nNamespaces;

  // Defined node types (PROTOs) for this namespace
  std::list< VrmlNodeType* > d_typeList;

  // Defined node names for this namespace
  std::list<VrmlNodePtr> d_nameList;

  // Parent namespace
  VrmlNamespace *d_parent;

};

#endif
