// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 78 -*-
//
// OpenVRML
//
// Copyright 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2010  Braden McDaniel
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

# ifdef HAVE_CONFIG_H
#   include <config.h>
# endif

# include <sstream>
# include <boost/algorithm/string/predicate.hpp>
# include <boost/scope_exit.hpp>
# include "image_stream_listener.h"

openvrml_node_vrml97::image_stream_listener::image_reader::~image_reader()
    OPENVRML_NOTHROW
{}

void
openvrml_node_vrml97::image_stream_listener::image_reader::
read(const std::vector<unsigned char> & data)
{
    this->do_read(data);
}

# ifdef OPENVRML_ENABLE_PNG_TEXTURES
void openvrml_png_info_callback(png_structp png_ptr, png_infop info_ptr)
{
    using boost::shared_mutex;
    using boost::unique_lock;

    typedef openvrml_node_vrml97::image_stream_listener::png_reader
        png_reader_t;
    png_reader_t & reader =
        *static_cast<png_reader_t *>(png_get_progressive_ptr(png_ptr));

    unique_lock<shared_mutex> lock(reader.stream_listener.image_mutex_);

    openvrml::image & image = reader.stream_listener.image_;

    image.comp(png_get_channels(png_ptr, info_ptr));
    const size_t width = png_get_image_width(png_ptr, info_ptr);
    const size_t height = png_get_image_height(png_ptr, info_ptr);
    image.resize(width, height);

    //
    // Strip 16 bit/color files to 8 bit/color.
    //
    png_set_strip_16(png_ptr);

    //
    // Extract multiple pixels with bit depths of 1, 2, and 4 from a
    // single byte into separate bytes.  (Usefule for paletted and
    // grayscale images.)
    //
    png_set_packing(png_ptr);

    //
    // Expand paletted colors into true RGB triplets.
    //
    const png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_expand(png_ptr);
        image.comp(3);
    } else {
        //
        // Expand grayscale images to the full 8 bits from 1, 2, or
        // 4 bits/pixel.
        //
        const png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8) {
            png_set_expand(png_ptr);
        }
    }

    //
    // Expand paletted or RGB images with transparency to full alpha
    // channels so the data will be available as RGBA quartets.
    //
    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_expand(png_ptr);
        image.comp(image.comp() + 1);
    }

    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        int num_palette;
        png_colorp palette;
        if (png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette)) {
            reader.gray_palette = true;
            for (int i = 0; i < num_palette; ++i) {
                if (palette[i].red != palette[i].green
                    || palette[i].blue != palette[i].green) {
                    reader.gray_palette = false;
                    break;
                }
            }
        }
    }

    if (reader.gray_palette) {
        if (image.comp() == 3) {
            image.comp(1);
        } else if (image.comp() == 4) {
            image.comp(2);
        }
    }

    //
    // Set gamma.
    //
    static const double screen_gamma = 2.2; // Display exponent.
    static const double default_gamma = 0.45455;
    double file_gamma;
    png_set_gamma(png_ptr,
                  screen_gamma,
                  png_get_gAMA(png_ptr, info_ptr, &file_gamma)
                  ? file_gamma
                  : default_gamma);

    png_set_interlace_handling(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    reader.width = png_get_image_width(png_ptr, info_ptr);
    reader.rowbytes = png_get_rowbytes(png_ptr, info_ptr);
    reader.old_row.resize(reader.rowbytes);
}

void openvrml_png_row_callback(png_structp png_ptr,
                               png_bytep new_row,
                               png_uint_32 row_num,
                               int /* pass */)
{
    using boost::unique_lock;
    using boost::shared_mutex;

    if (!new_row) { return; }

    typedef openvrml_node_vrml97::image_stream_listener::png_reader
        png_reader_t;
    png_reader_t & reader =
        *static_cast<png_reader_t *>(png_get_progressive_ptr(png_ptr));

    unique_lock<shared_mutex> lock(reader.stream_listener.image_mutex_);

    openvrml::image & image = reader.stream_listener.image_;

    assert(!reader.old_row.empty());

    png_progressive_combine_row(png_ptr, &reader.old_row.front(), new_row);

    //
    // openvrml::image pixels start at the bottom left.
    //
    const size_t image_row = (image.y() - 1) - row_num;
    for (size_t pixel_index = 0, byte_index = 0; pixel_index < reader.width;
         ++pixel_index) {
        using openvrml::int32;
        int32 pixel = 0x00000000;
        for (size_t component = image.comp();
             component > 0;
             --component) {
            pixel |= int32(new_row[byte_index]) << (8 * (component - 1));
            if (reader.gray_palette) {
                if (image.comp() == 1) {
                    // We just want every third pixel element.
                    byte_index += 3;
                } else if (image.comp() == 2) {
                    // We just want the first and fourth pixel elements.
                    byte_index += (byte_index % 4 == 0) ? 3 : 1;
                }
            } else {
                ++byte_index;
            }
        }
        image.pixel(image_row * reader.width + pixel_index, pixel);
    }

    reader.stream_listener.node_.modified(true);

    assert(reader.old_row.size() >= reader.rowbytes);

    copy(new_row, new_row + reader.rowbytes, reader.old_row.begin());
}

void openvrml_png_end_callback(png_structp, png_infop)
{}

openvrml_node_vrml97::image_stream_listener::png_reader::
png_reader(image_stream_listener & stream_listener):
    png_ptr_(0),
    info_ptr_(0),
    stream_listener(stream_listener),
    gray_palette(false)
{
    this->png_ptr_ =
        png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (!this->png_ptr_) { throw std::bad_alloc(); }
    bool succeeded = false;
    BOOST_SCOPE_EXIT((&succeeded)(&png_ptr_)(&info_ptr_)) {
        if (!succeeded) {
            png_destroy_read_struct(&png_ptr_, &info_ptr_, png_infopp(0));
        }
    } BOOST_SCOPE_EXIT_END

    this->info_ptr_ = png_create_info_struct(this->png_ptr_);
    if (!this->info_ptr_) { throw std::bad_alloc(); }

    png_set_progressive_read_fn(this->png_ptr_,
                                this,
                                openvrml_png_info_callback,
                                openvrml_png_row_callback,
                                openvrml_png_end_callback);
    succeeded = true;
}

openvrml_node_vrml97::image_stream_listener::png_reader::~png_reader()
    OPENVRML_NOTHROW
{
    png_destroy_read_struct(&this->png_ptr_, &this->info_ptr_, png_infopp(0));
}

void
openvrml_node_vrml97::image_stream_listener::png_reader::
do_read(const std::vector<unsigned char> & data)
{
    int jmpval = setjmp(png_jmpbuf(this->png_ptr_));
    if (jmpval != 0) { return; }

    png_process_data(this->png_ptr_,
                     this->info_ptr_,
                     const_cast<png_byte *>(&data[0]),
                     data.size());
}
# endif // defined OPENVRML_ENABLE_PNG_TEXTURES

# ifdef OPENVRML_ENABLE_JPEG_TEXTURES
void openvrml_jpeg_init_source(j_decompress_ptr)
{}

boolean openvrml_jpeg_fill_input_buffer(j_decompress_ptr cinfo)
{
    using std::vector;
    typedef openvrml_node_vrml97::image_stream_listener::jpeg_reader::source_mgr
        source_mgr_t;
    source_mgr_t & src =
        *static_cast<source_mgr_t *>(static_cast<void *>(cinfo->src));

    jpeg_source_mgr & source_mgr = src.pub;
    openvrml_node_vrml97::image_stream_listener::jpeg_reader & reader =
        *src.reader;

    if (reader.reading) {
        if (reader.buffer.empty()) { return false; /* Suspend. */ }

        vector<JOCTET>::const_iterator resume_pos = reader.buffer.begin();

        vector<JOCTET>::size_type bytes_now_in_buffer =
            reader.bytes_in_buffer;
        reader.bytes_in_buffer = 0;

        if (reader.bytes_to_skip > 0) {
            if (reader.bytes_to_skip < bytes_now_in_buffer) {
                resume_pos          += reader.bytes_to_skip;
                bytes_now_in_buffer -= reader.bytes_to_skip;
                reader.bytes_to_skip = 0;
            } else {
                reader.bytes_to_skip -= bytes_now_in_buffer;
                return false; // Suspend; we'll need to skip some more.
            }
        }

        reader.backtrack_buffer_bytes_unread = source_mgr.bytes_in_buffer;

        source_mgr.next_input_byte = &*resume_pos;
        source_mgr.bytes_in_buffer = bytes_now_in_buffer;
        reader.reading = false;
        return true;
    }

    if (reader.buffer.empty()
        || source_mgr.next_input_byte != &reader.buffer.front()) {
        reader.bytes_in_backtrack_buffer = 0;
        reader.backtrack_buffer_bytes_unread = 0;
    }

    const vector<JOCTET>::size_type bytes_now_in_backtrack_buffer =
        source_mgr.bytes_in_buffer + reader.bytes_in_backtrack_buffer;

    if (bytes_now_in_backtrack_buffer > 0) {
        reader.backtrack_buffer.resize(bytes_now_in_backtrack_buffer);

        std::copy(source_mgr.next_input_byte,
                  source_mgr.next_input_byte + source_mgr.bytes_in_buffer,
                  reader.backtrack_buffer.begin()
                  + reader.bytes_in_backtrack_buffer);

        source_mgr.next_input_byte =
            &*(reader.backtrack_buffer.begin()
               + reader.bytes_in_backtrack_buffer
               - reader.backtrack_buffer_bytes_unread);
        source_mgr.bytes_in_buffer += reader.backtrack_buffer_bytes_unread;
    }
    reader.bytes_in_backtrack_buffer = bytes_now_in_backtrack_buffer;
    reader.reading = true;
    return false;
}

void openvrml_jpeg_skip_input_data(const j_decompress_ptr cinfo,
                                   const long num_bytes)
{
    typedef openvrml_node_vrml97::image_stream_listener::jpeg_reader::source_mgr
        source_mgr_t;
    source_mgr_t & src =
        *static_cast<source_mgr_t *>(static_cast<void *>(cinfo->src));

    jpeg_source_mgr & source_mgr = src.pub;
    openvrml_node_vrml97::image_stream_listener::jpeg_reader & reader =
        *src.reader;

    if (size_t(num_bytes) > source_mgr.bytes_in_buffer) {
        reader.bytes_to_skip = num_bytes - source_mgr.bytes_in_buffer;
        source_mgr.next_input_byte += source_mgr.bytes_in_buffer;
        source_mgr.bytes_in_buffer = 0;
    } else {
        source_mgr.bytes_in_buffer -= num_bytes;
        source_mgr.next_input_byte += num_bytes;
    }
}

void openvrml_jpeg_term_source(j_decompress_ptr)
{}

void openvrml_jpeg_error_exit(j_common_ptr cinfo)
{
    typedef openvrml_node_vrml97::image_stream_listener::jpeg_reader::error_mgr
        error_mgr;
    error_mgr * const err =
        static_cast<error_mgr *>(static_cast<void *>(cinfo->err));
    (*cinfo->err->output_message)(cinfo);
    longjmp(err->jmpbuf, 1);
}

void openvrml_jpeg_output_message(j_common_ptr cinfo)
{
    typedef openvrml_node_vrml97::image_stream_listener::jpeg_reader::error_mgr
        error_mgr;
    error_mgr * const err = reinterpret_cast<error_mgr *>(cinfo->err);
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);
    assert(err->stream_listener);
    std::ostringstream msg;
    msg << err->stream_listener->uri_ << ": " << buffer;
    openvrml::browser & browser =
        err->stream_listener->node_.type().metatype().browser();
    browser.err(msg.str());
}

openvrml_node_vrml97::image_stream_listener::jpeg_reader::
jpeg_reader(image_stream_listener & stream_listener):
    stream_listener(stream_listener),
    reading(true),
    bytes_to_skip(0),
    backtrack_buffer_bytes_unread(0),
    bytes_in_buffer(0),
    bytes_in_backtrack_buffer(0),
    decoder_state(header),
    scanlines(0),
    progressive_scan_started(false)
{
    std::memset(&this->cinfo_, 0, sizeof this->cinfo_);
    std::memset(&this->source_mgr_, 0, sizeof this->source_mgr_);

    this->cinfo_.err = jpeg_std_error(&this->error_mgr_.pub);
    this->error_mgr_.pub.error_exit = openvrml_jpeg_error_exit;
    this->error_mgr_.pub.output_message = openvrml_jpeg_output_message;
    int jmpval = setjmp(this->error_mgr_.jmpbuf);
    if (jmpval != 0) { return; }
    this->error_mgr_.stream_listener = &stream_listener;
    jpeg_create_decompress(&this->cinfo_);
    this->source_mgr_.pub.next_input_byte = 0;
    this->source_mgr_.pub.bytes_in_buffer = 0;
    this->source_mgr_.pub.init_source = openvrml_jpeg_init_source;
    this->source_mgr_.pub.fill_input_buffer =
        openvrml_jpeg_fill_input_buffer;
    this->source_mgr_.pub.skip_input_data = openvrml_jpeg_skip_input_data;
    this->source_mgr_.pub.resync_to_restart = jpeg_resync_to_restart;
    this->source_mgr_.pub.term_source = openvrml_jpeg_term_source;
    this->source_mgr_.reader = this;

    this->cinfo_.src =
        static_cast<jpeg_source_mgr *>(
            static_cast<void *>(&this->source_mgr_));
}

openvrml_node_vrml97::image_stream_listener::jpeg_reader::~jpeg_reader() OPENVRML_NOTHROW
{
    jpeg_destroy_decompress(&this->cinfo_);
}

void
openvrml_node_vrml97::image_stream_listener::jpeg_reader::
do_read(const std::vector<unsigned char> & data)
{
    using boost::unique_lock;
    using boost::shared_mutex;
    unique_lock<shared_mutex> lock(this->stream_listener.image_mutex_);

    if (data.size() > this->buffer.size()) {
        this->buffer.resize(data.size());
    }
    copy(data.begin(), data.end(), this->buffer.begin());
    this->bytes_in_buffer = data.size();

    int jmpval = setjmp(this->error_mgr_.jmpbuf);
    if (jmpval != 0) { return; }

    switch (this->decoder_state) {
    case jpeg_reader::header:
    {
        static const bool require_image = true;
        const int read_header_result = jpeg_read_header(&this->cinfo_,
                                                        require_image);
        if (read_header_result == JPEG_SUSPENDED) { return; }

        switch (this->cinfo_.jpeg_color_space) {
        case JCS_GRAYSCALE:
        case JCS_RGB:
        case JCS_YCbCr:
            this->cinfo_.out_color_space = JCS_RGB;
            break;
        default:
            this->decoder_state = jpeg_reader::error;
            return;
        }

        this->cinfo_.buffered_image =
            jpeg_has_multiple_scans(&this->cinfo_);

        jpeg_calc_output_dimensions(&this->cinfo_);

        openvrml::image & image = this->stream_listener.image_;
        image.comp(this->cinfo_.num_components);
        image.resize(this->cinfo_.image_width, this->cinfo_.image_height);

        this->decoder_state = jpeg_reader::start_decompress;
    }
    case jpeg_reader::start_decompress:
    {
        this->cinfo_.dct_method = JDCT_ISLOW;
        this->cinfo_.dither_mode = JDITHER_FS;
        this->cinfo_.do_fancy_upsampling = true;
        this->cinfo_.enable_2pass_quant = false;
        this->cinfo_.do_block_smoothing = true;

        if (!jpeg_start_decompress(&this->cinfo_)) {
            return; // Input suspended.
        }

        const JDIMENSION samples_per_row =
            this->cinfo_.output_width * this->cinfo_.out_color_components;
        static const JDIMENSION num_rows = 1;
        this->scanlines =
            (*this->cinfo_.mem->alloc_sarray)(
                reinterpret_cast<j_common_ptr>(&this->cinfo_),
                JPOOL_IMAGE,
                samples_per_row,
                num_rows);

        this->decoder_state = this->cinfo_.buffered_image
            ? jpeg_reader::decompress_progressive
            : jpeg_reader::decompress_sequential;
    }
    case jpeg_reader::decompress_sequential:
        if (this->decoder_state == jpeg_reader::decompress_sequential) {
            if (!this->output_scanlines()) {
                return; // Input suspended.
            }
            this->decoder_state = jpeg_reader::done;
        }
    case jpeg_reader::decompress_progressive:
        if (this->decoder_state == jpeg_reader::decompress_progressive) {
            int status;
            do {
                status = jpeg_consume_input(&this->cinfo_);
            } while (status != JPEG_SUSPENDED
                     && status != JPEG_REACHED_EOI);

            while (true) {
                if (this->cinfo_.output_scanline == 0
                    && !this->progressive_scan_started) {
                    int scan = this->cinfo_.input_scan_number;

                    if (this->cinfo_.output_scan_number == 0
                        && scan > 1
                        && status != JPEG_REACHED_EOI) {
                        --scan;
                    }

                    if (!jpeg_start_output(&this->cinfo_, scan)) {
                        return; // Input suspended.
                    }
                    this->progressive_scan_started = true;
                }

                if (!this->output_scanlines()) {
                    return; // Input suspended.
                }

                if (this->cinfo_.output_scanline
                    == this->cinfo_.output_height) {
                    if (!jpeg_finish_output(&this->cinfo_)) {
                        return; // Input suspended.
                    }
                    if (jpeg_input_complete(&this->cinfo_)
                        && (this->cinfo_.input_scan_number
                            == this->cinfo_.output_scan_number)) {
                        break;
                    }
                    this->cinfo_.output_scanline = 0;
                    this->progressive_scan_started = false;
                }
            }

            this->decoder_state = jpeg_reader::done;
        }
    case jpeg_reader::done:
        if (!jpeg_finish_decompress(&this->cinfo_)) {
            return; // Input suspended.
        }
        this->decoder_state = jpeg_reader::sink_non_jpeg_trailer;
        break;

    case jpeg_reader::sink_non_jpeg_trailer:
        break;

    case jpeg_reader::error:
        break;
    }
}

bool
openvrml_node_vrml97::image_stream_listener::jpeg_reader::output_scanlines()
{
    JDIMENSION top = this->cinfo_.output_scanline;
    bool result = true;

    openvrml::image & image = this->stream_listener.image_;

    while (this->cinfo_.output_scanline < this->cinfo_.output_height) {
        JDIMENSION scanlines_completed =
            jpeg_read_scanlines(&this->cinfo_, this->scanlines, 1);
        if (scanlines_completed != 1) {
            result = false; // Suspend.
            break;
        }

        const size_t image_row = image.y() - this->cinfo_.output_scanline;
        for (size_t pixel_index = 0, byte_index = 0;
             pixel_index < image.x();
             ++pixel_index) {
            using openvrml::int32;
            int32 pixel = 0x00000000;
            for (size_t component = image.comp();
                 component > 0;
                 --component, ++byte_index) {
                const JSAMPLE sample = (*this->scanlines)[byte_index];
                pixel |= int32(sample) << (8 * (component - 1));
            }
            image.pixel(image_row * image.x() + pixel_index, pixel);
        }
    }

    if (top != this->cinfo_.output_scanline) {
        this->stream_listener.node_.modified(true);
    }

    return result;
}
# endif // defined OPENVRML_ENABLE_JPEG_TEXTURES

openvrml_node_vrml97::image_stream_listener::
image_stream_listener(const std::string & uri,
                      openvrml::image & image,
                      openvrml::node & node,
                      boost::shared_mutex & image_mutex):
    uri_(uri),
    image_mutex_(image_mutex),
    image_(image),
    node_(node)
{}

openvrml_node_vrml97::image_stream_listener::~image_stream_listener()
    OPENVRML_NOTHROW
{}

void
openvrml_node_vrml97::image_stream_listener::
do_stream_available(const std::string & /* uri */,
                    const std::string & media_type)
{
    using boost::algorithm::iequals;
    if (iequals(media_type, "image/png")
        || iequals(media_type, "image/x-png")) {
# ifdef OPENVRML_ENABLE_PNG_TEXTURES
        this->image_reader_.reset(new png_reader(*this));
# endif
    } else if (iequals(media_type, "image/jpeg")) {
# ifdef OPENVRML_ENABLE_JPEG_TEXTURES
        this->image_reader_.reset(new jpeg_reader(*this));
# endif
    }
}

void
openvrml_node_vrml97::image_stream_listener::
do_data_available(const std::vector<unsigned char> & data)
{
    if (this->image_reader_) { this->image_reader_->read(data); }
}
