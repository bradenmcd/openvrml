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
#ifndef SCRIPTOBJECT_H
#define SCRIPTOBJECT_H
//
//  Abstract Script Object class
//

class VrmlField;
class VrmlMFString;
class VrmlNodeScript;

class ScriptObject {

protected:  
  ScriptObject();		// Use create()

public:

  virtual ~ScriptObject() = 0;

  // Script object factory.
  static ScriptObject *create( VrmlNodeScript *, VrmlMFString &url );

  // Invoke a named function
  virtual void activate( double timeStamp,
			 const char *fname,
			 int argc,
			 const VrmlField *argv[] ) = 0;

};

#endif // SCRIPTOBJECT_H
