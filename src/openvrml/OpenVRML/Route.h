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

# ifndef ROUTE_H
#   define ROUTE_H

class VrmlNode;

class Route {
public:
  Route(const char *fromEventOut, VrmlNode *toNode, const char *toEventIn);
  Route(const Route&);
  ~Route();

  char *fromEventOut() { return d_fromEventOut; }
  char *toEventIn() { return d_toEventIn; }
  VrmlNode *toNode() { return d_toNode; }

  Route *prev() { return d_prev; }
  Route *next() { return d_next; }
  void setPrev(Route* r) { d_prev = r; }
  void setNext(Route* r) { d_next = r; }
  
private:
  char *d_fromEventOut;
  VrmlNode *d_toNode;
  char *d_toEventIn;
  Route *d_prev, *d_next;
};

# endif
