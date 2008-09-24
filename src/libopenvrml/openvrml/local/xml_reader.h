// -*- mode: c++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2008  Braden McDaniel
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

# ifndef OPENVRML_LOCAL_XML_READER_H
#   define OPENVRML_LOCAL_XML_READER_H

#   include <openvrml-common.h>
#   ifdef _WIN32
#     include <XmlLite.h>
#   else
#     include <libxml/xmlreader.h>
#   endif
#   include <string>
#   include <stdexcept>

namespace openvrml {

    namespace local {

        /**
         * @brief A wrapper that abstracts commonality between libxml's
         *        XmlTextReader interface and Microsoft's XmlLite.
         *
         * There is a lot in common here between the two underlying APIs; and
         * that's no coincidence, apparently: both APIs are based on the C#
         * XmlReader API.
         */
        class OPENVRML_LOCAL xml_reader {
# ifdef _WIN32
            IStream * input;
            IXmlReader * reader;
# else
            xmlTextReaderPtr reader;
# endif
        public:
            //
            // Conveniently, these values are consistent between libxml and
            // XmlLite.
            //
            enum node_type_id {
                none_id                   = 0,
                element_id                = 1,
                attribute_id              = 2,
                text_id                   = 3,
                cdata_id                  = 4,
                processing_instruction_id = 7,
                comment_id                = 8,
                document_type_id          = 10,
                whitespace_id             = 13,
                end_element_id            = 15,
                xml_declaration_id        = 17
            };

            explicit xml_reader(const std::string & filename)
                OPENVRML_THROW1(std::runtime_error);
            ~xml_reader() OPENVRML_NOTHROW;

            int read() OPENVRML_NOTHROW;
            node_type_id node_type() const OPENVRML_NOTHROW;
            const std::string local_name() const
                OPENVRML_THROW2(std::runtime_error, std::bad_alloc);
            const std::string value() const
                OPENVRML_THROW2(std::runtime_error, std::bad_alloc);
            int move_to_first_attribute() OPENVRML_NOTHROW;
            int move_to_next_attribute() OPENVRML_NOTHROW;
        };
    }
}

# endif // ifndef OPENVRML_LOCAL_XML_READER_H
