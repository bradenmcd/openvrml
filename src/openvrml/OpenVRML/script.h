//
// OpenVRML
//
// Copyright (C) 2001  Braden McDaniel
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

# ifndef OPENVRML_SCRIPT_H
#   define OPENVRML_SCRIPT_H

#   include <stddef.h>
#   include <string>
#   include "common.h"

namespace OpenVRML {

    class FieldValue;
    class ScriptNode;

    class OPENVRML_SCOPE Script {
    public:
        virtual ~Script() = 0;
        virtual void initialize(double timestamp) = 0;
        virtual void processEvent(const std::string & id,
                                  const FieldValue & value,
                                  double timestamp) = 0;
        virtual void eventsProcessed(double timestamp) = 0;
        virtual void shutdown(double timestamp) = 0;

    protected:
        ScriptNode & scriptNode;

        Script(ScriptNode & scriptNode);

    private:
        // non-copyable
        Script(const Script &);
        Script & operator=(const Script &);
    };
}

# endif
