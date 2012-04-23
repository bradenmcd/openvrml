// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007  Braden McDaniel
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

# ifndef OPENVRML_NODE_VRML97_IMAGE_STREAM_LISTENER_H
#   define OPENVRML_NODE_VRML97_IMAGE_STREAM_LISTENER_H

# ifdef OPENVRML_ENABLE_PNG_TEXTURES
#   include <png.h>
# endif
# ifdef OPENVRML_ENABLE_JPEG_TEXTURES
extern "C" {
#   include <jpeglib.h>
}
# endif

# include <openvrml/browser.h>

# ifdef OPENVRML_ENABLE_PNG_TEXTURES
extern "C" void openvrml_png_info_callback(png_structp png_ptr,
                                           png_infop info_ptr);
extern "C" void openvrml_png_row_callback(png_structp png_ptr,
                                          png_bytep new_row,
                                          png_uint_32 row_num,
                                          int pass);
extern "C" void openvrml_png_end_callback(png_structp png_ptr,
                                          png_infop info_ptr);
# endif

# ifdef OPENVRML_ENABLE_JPEG_TEXTURES
extern "C" void openvrml_jpeg_init_source(j_decompress_ptr cinfo);
extern "C" boolean openvrml_jpeg_fill_input_buffer(j_decompress_ptr cinfo);
extern "C" void openvrml_jpeg_skip_input_data(j_decompress_ptr cinfo,
                                              long num_bytes);
extern "C" void openvrml_jpeg_term_source(j_decompress_ptr cinfo);
extern "C" void openvrml_jpeg_error_exit(j_common_ptr cinfo);
extern "C" void openvrml_jpeg_output_message(j_common_ptr cinfo);
# endif

namespace openvrml_node_vrml97 {

    class image_stream_listener : public openvrml::stream_listener {
        const std::string uri_;
        boost::shared_mutex & image_mutex_;
        openvrml::image & image_;
        openvrml::node & node_;

        class image_reader {
        public:
            virtual ~image_reader() OPENVRML_NOTHROW = 0;
            void read(const std::vector<unsigned char> & data);

        private:
            virtual void do_read(const std::vector<unsigned char> & data) = 0;
        };

# ifdef OPENVRML_ENABLE_PNG_TEXTURES
        friend void (::openvrml_png_info_callback)(png_structp png_ptr,
                                                   png_infop info_ptr);
        friend void (::openvrml_png_row_callback)(png_structp png_ptr,
                                                  png_bytep new_row,
                                                  png_uint_32 row_num,
                                                  int pass);
        friend void (::openvrml_png_end_callback)(png_structp png_ptr,
                                                  png_infop info_ptr);

        class png_reader : public image_reader {
            png_structp png_ptr_;
            png_infop info_ptr_;

        public:
            image_stream_listener & stream_listener;
            std::vector<png_byte> old_row;
            bool gray_palette;
            png_uint_32 width;
            png_size_t rowbytes;

            explicit png_reader(image_stream_listener & stream_listener);
            virtual ~png_reader() OPENVRML_NOTHROW;

        private:
            virtual void do_read(const std::vector<unsigned char> & data);
        };
# endif

# ifdef OPENVRML_ENABLE_JPEG_TEXTURES
        friend void (::openvrml_jpeg_init_source)(j_decompress_ptr cinfo);
        friend boolean (::openvrml_jpeg_fill_input_buffer)(j_decompress_ptr cinfo);
        friend void (::openvrml_jpeg_skip_input_data)(j_decompress_ptr cinfo,
                                                      long num_bytes);
        friend void (::openvrml_jpeg_term_source)(j_decompress_ptr cinfo);
        friend void (::openvrml_jpeg_error_exit)(j_common_ptr cinfo);
        friend void (::openvrml_jpeg_output_message)(j_common_ptr cinfo);

        class jpeg_reader : public image_reader {
        public:
            struct source_mgr {
                jpeg_source_mgr pub;
                jpeg_reader * reader;
            };

            struct error_mgr {
                jpeg_error_mgr pub;
                jmp_buf jmpbuf;
                image_stream_listener * stream_listener;
            };

        private:
            jpeg_decompress_struct cinfo_;
            error_mgr error_mgr_;
            source_mgr source_mgr_;

        public:
            image_stream_listener & stream_listener;
            bool reading;
            std::vector<JOCTET>::size_type bytes_to_skip;
            std::vector<JOCTET>::size_type backtrack_buffer_bytes_unread;
            std::vector<JOCTET> buffer, backtrack_buffer;
            std::vector<JOCTET>::size_type bytes_in_buffer,
                bytes_in_backtrack_buffer;
            enum decoder_state_t {
                header,
                start_decompress,
                decompress_progressive,
                decompress_sequential,
                done,
                sink_non_jpeg_trailer,
                error
            } decoder_state;
            JSAMPARRAY scanlines;
            bool progressive_scan_started;

            explicit jpeg_reader(image_stream_listener & stream_listener);
            virtual ~jpeg_reader() OPENVRML_NOTHROW;

        private:
            virtual void do_read(const std::vector<unsigned char> & data);

            bool output_scanlines();
        };
# endif

        boost::scoped_ptr<image_reader> image_reader_;

    public:
        image_stream_listener(const std::string & uri,
                              openvrml::image & image,
                              openvrml::node & node,
                              boost::shared_mutex & image_mutex);
        virtual ~image_stream_listener() OPENVRML_NOTHROW;

    private:
        virtual void
        do_stream_available(const std::string & uri,
                            const std::string & media_type);

        virtual void
        do_data_available(const std::vector<unsigned char> & data);
    };
}

# endif // ifndef OPENVRML_NODE_VRML97_IMAGE_STREAM_LISTENER_H
