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

# include "xml_reader.h"
# ifdef _WIN32
#   include <shlwapi.h>
# endif

/**
 * @enum openvrml::local::xml_reader::node_type_id
 *
 * @brief The node type identifier.
 *
 * Due to the common heritage of XmlTextReader and XmlLite, the underlying
 * APIs report consistent values that can be passed through directly to
 * the @c openvrml::local::xml_reader interface.
 *
 * @sa http://xmlsoft.org/html/libxml-xmlreader.html#xmlReaderTypes
 * @sa http://msdn.microsoft.com/en-us/library/ms753141(VS.85).aspx
 */

/**
 * @var openvrml::local::xml_reader::node_type_id openvrml::local::xml_reader::none_id
 *
 * @brief The &ldquo;null&rdquo; type identifier.
 */

/**
 * @var openvrml::local::xml_reader::node_type_id openvrml::local::xml_reader::element_id
 *
 * @brief Element identifier.
 */

/**
 * @var openvrml::local::xml_reader::node_type_id openvrml::local::xml_reader::attribute_id
 *
 * @brief Attribute identifier.
 */

/**
 * @var openvrml::local::xml_reader::node_type_id openvrml::local::xml_reader::text_id
 *
 * @brief Text identifier.
 */

/**
 * @var openvrml::local::xml_reader::node_type_id openvrml::local::xml_reader::cdata_id
 *
 * @brief CDATA identifier.
 */

/**
 * @var openvrml::local::xml_reader::node_type_id openvrml::local::xml_reader::processing_instruction_id
 *
 * @brief Processing instruction identifier.
 */

/**
 * @var openvrml::local::xml_reader::node_type_id openvrml::local::xml_reader::comment_id
 *
 * @brief Comment identifier.
 */

/**
 * @var openvrml::local::xml_reader::node_type_id openvrml::local::xml_reader::document_type_id
 *
 * @brief Document type identifier.
 */

/**
 * @var openvrml::local::xml_reader::node_type_id openvrml::local::xml_reader::whitespace_id
 *
 * @brief Whitespace identifier.
 */

/**
 * @var openvrml::local::xml_reader::node_type_id openvrml::local::xml_reader::end_element_id
 *
 * @brief Element end tag identifier.
 *
 * Unlike SAX, where you get an &ldquo;end of element&rdquo; callback at
 * the logical element boundary, @c #node_type only returns this value
 * when the current node is a literal end tag.
 */

/**
 * @var openvrml::local::xml_reader::node_type_id openvrml::local::xml_reader::xml_declaration_id
 *
 * @brief XML declaration identifier.
 */

/**
 * @brief Construct from a file name.
 *
 * @param[in] filename  the full path to a file.
 *
 * @exception std::runtime_error    if opening @p filename or creating the
 *                                  underlying XML reader fails.
 */
openvrml::local::xml_reader::xml_reader(const std::string & filename)
    OPENVRML_THROW1(std::runtime_error):
# ifdef _WIN32
    input(0),
# endif
    reader(0)
{
# ifdef _WIN32
    HRESULT hr;

    hr = SHCreateStreamOnFile(filename.c_str(), STGM_READ, &this->input);
    if (FAILED(hr)) {
        throw std::runtime_error("failed to open file \"" + filename
                                 + '\"');
    }
    scope_guard input_guard =
        make_obj_guard(*this->input, &IStream::Release);

    hr = CreateXmlReader(__uuidof(IXmlReader),
                         reinterpret_cast<void **>(&this->reader),
                         0);
    if (FAILED(hr)) {
        throw std::runtime_error("failed to create XML reader");
    }
    scope_guard reader_guard =
        make_obj_guard(*this->reader, &IXmlReader::Release);

    hr = this->reader->SetInput(this->input);
    if (FAILED(hr)) {
        throw std::runtime_error("failed to set input for XML reader");
    }

    input_guard.dismiss();
    reader_guard.dismiss();
# else
    static const char * const encoding = 0;
    static const int options = 0;
    this->reader = xmlReaderForFile(filename.c_str(), encoding, options);
    if (!this->reader) {
        throw std::runtime_error("failed to create XML reader");
    }
# endif
}

/**
 * @brief Destroy.
 */
openvrml::local::xml_reader::~xml_reader() OPENVRML_NOTHROW
{
# ifdef _WIN32
    this->reader->Release();
    this->input->Release();
# else
    xmlFreeTextReader(this->reader);
# endif
}

/**
 * @brief Advance to the next node in the stream.
 *
 * @retval 1 if the node was read successfully
 * @retval 0 if there are no more nodes to read
 * @retval -1 if there is an error.
 */
int openvrml::local::xml_reader::read() OPENVRML_NOTHROW
{
# ifdef _WIN32
    HRESULT hr = this->reader->Read(0);
    return (hr == S_OK)
        ? 1
        : (hr == S_FALSE)
        ? 0
        : -1;
# else
    return xmlTextReaderRead(this->reader);
# endif
}

/**
 * @brief The type of the current node.
 *
 * @return the type of the current node.
 */
openvrml::local::xml_reader::node_type_id
openvrml::local::xml_reader::node_type() const OPENVRML_NOTHROW
{
# ifdef _WIN32
    XmlNodeType type;
    this->reader->GetNodeType(&type);
    return node_type_id(type);
# else
    return node_type_id(xmlTextReaderNodeType(this->reader));
# endif
}

/**
 * @brief The local (i.e., unqualified) name of the node.
 *
 * @return the local name of the node.
 *
 * @exception std::runtime_error    if there is an error getting the name.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const std::string openvrml::local::xml_reader::local_name() const
    OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
{
# ifdef _WIN32
    const WCHAR * name;
    HRESULT hr = this->reader->GetLocalName(&name, 0);
    if (FAILED(hr)) {
        throw std::runtime_error("failed to get element name");
    }
    return std::string(name, name + wcslen(name));
# else
    const xmlChar * name = xmlTextReaderConstLocalName(this->reader);
    return std::string(name, name + xmlStrlen(name));
# endif
}

/**
 * @brief The text value of the node, if any.
 *
 * @return the text value of the node.
 *
 * @exception std::runtime_error    if there is an error getting the value.
 * @exception std::bad_alloc        if memory allocation fails.
 */
const std::string openvrml::local::xml_reader::value() const
    OPENVRML_THROW2(std::runtime_error, std::bad_alloc)
{
# ifdef _WIN32
    const WCHAR * val;
    HRESULT hr = this->reader->GetValue(&val, 0);
    if (FAILED(hr)) {
        throw std::runtime_error("failed to get a value");
    }
    return std::string(val, val + wcslen(val));
# else
    const xmlChar * val = xmlTextReaderConstValue(this->reader);
    return std::string(val, val + xmlStrlen(val));
# endif
}

/**
 * @brief Move to the first attribute associated with the current node.
 *
 * @retval 1 on success
 * @retval 0 if the current node has no attributes
 * @retval -1 if there is an error
 */
int openvrml::local::xml_reader::move_to_first_attribute() OPENVRML_NOTHROW
{
# ifdef _WIN32
    HRESULT hr = this->reader->MoveToFirstAttribute();
    return (hr == S_OK)
        ? 1
        : (hr == S_FALSE)
        ? 0
        : -1;
# else
    return xmlTextReaderMoveToFirstAttribute(this->reader);
# endif
}

/**
 * @brief Move to the next attribute associated with the current node.
 *
 * @retval 1 on success
 * @retval 0 if the current node has no more attributes
 * @retval -1 if there is an error
 */
int openvrml::local::xml_reader::move_to_next_attribute() OPENVRML_NOTHROW
{
# ifdef _WIN32
    HRESULT hr = this->reader->MoveToNextAttribute();
    return (hr == S_OK)
        ? 1
        : (hr == S_FALSE)
        ? 0
        : -1;
# else
    return xmlTextReaderMoveToNextAttribute(this->reader);
# endif
}
