// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 1998  Chris Morley
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
// Copyright 2002  S. K. Bose
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 3 of the License, or (at your
// option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, see <http://www.gnu.org/licenses/>.
//

# ifndef OPENVRML_NODE_VRML97_TEXT_H
#   define OPENVRML_NODE_VRML97_TEXT_H

#   include <openvrml/node.h>
#   ifdef HAVE_CONFIG_H
#     include <config.h>
#   endif
#   ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
#     include <ft2build.h>
#     include FT_FREETYPE_H
#   endif

namespace openvrml_node_vrml97 {

    /**
     * @brief Class object for Text nodes.
     */
    class OPENVRML_LOCAL text_metatype : public openvrml::node_metatype {
    public:
        static const char * const id;

# ifdef OPENVRML_ENABLE_RENDER_TEXT_NODE
        FT_Library freeTypeLibrary;
# endif

        explicit text_metatype(openvrml::browser & browser);
        virtual ~text_metatype() OPENVRML_NOTHROW;

    private:
        virtual const boost::shared_ptr<openvrml::node_type>
        do_create_type(const std::string & id,
                       const openvrml::node_interface_set & interfaces) const
            OPENVRML_THROW2(openvrml::unsupported_interface, std::bad_alloc);
    };
}

# endif // ifndef OPENVRML_NODE_VRML97_TEXT_H
