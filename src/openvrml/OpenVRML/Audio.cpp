// -*- Mode: C++; indent-tabs-mode: nil; c-basic-offset: 4; -*-
//
// OpenVRML
//
// Copyright (C) 1999  Kumaran Santhanam
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

# include <stdio.h>
# include <string.h>

# include "private.h"
# include "Audio.h"
# include "doc.h"
# include "system.h"
# include "field.h"

namespace OpenVRML {

/**
 * @class Audio
 *
 * @brief Play audio clips.
 */

typedef unsigned char   byte;
typedef unsigned short  two_bytes;
typedef unsigned int    four_bytes;

enum AudioFileType {
    AudioFile_UNKNOWN,
    AudioFile_WAV
};

struct WaveHeader {
    byte           riff_id[4];
    four_bytes     riff_size;
    byte           wave_id[4];
    byte           format_id[4];
    four_bytes     format_size;
    two_bytes      format_tag;
    two_bytes      num_channels;
    four_bytes     num_samples_per_sec;
    four_bytes     num_avg_bytes_per_sec;
    two_bytes      num_block_align;
    two_bytes      bits_per_sample;
    byte           data_id[4];
    four_bytes     num_data_bytes;
};

#define WAVE_FORMAT_PCM   1

namespace {
    /**
     * @brief Determine the audio file type.
     *
     * @param url   URL string.
     * @param file  included in case this function is updated to peek at the
     *              file header.
     *
     * @return the audio file type.
     */
    AudioFileType audioFileType(const std::string & url, FILE * file)
    {
        using std::string;
        string::size_type dotPos = url.find_last_of('.');
        if (dotPos != string::npos) {
            ++dotPos;
        }

        const string ext(url.substr(dotPos));
        if (ext == "wav" || ext == "WAV") {
            return AudioFile_WAV;
        } else {
            return AudioFile_UNKNOWN;
        }
    }
}

/**
 * @brief Constructor.
 *
 * @param url       URL string.
 * @param relative  Doc object.
 */
Audio::Audio(const std::string & url, doc *relative)
    : _doc(0),
      _encoding(AUDIO_LINEAR),
      _channels(0),
      _bits_per_sample(0),
      _samples_per_sec(0),
      _sample_blocksize(0),
      _num_samples(0),
      _samples(0)
{
    setURL (url, relative);
}

/**
 * @brief Destructor.
 */
Audio::~Audio()
{
    delete _doc;
    delete _samples;
}

/**
 * @brief Set the URL of the audio file and read it from the document object.
 *
 * @param url       URL string.
 * @param relative  doc object.
 *
 * @return @c true if the URL was read; @c false otherwise.
 */
bool Audio::setURL(const std::string & url, doc *relative)
{
#if HAVE_SOUND
    if (url == 0)
        return false;
#else
    return false;
#endif

    delete _doc;
    _doc = new doc (url, relative);
    FILE *fp = _doc->fopen ("rb");

    bool success = false;
    if (fp) {
        switch (audioFileType (url, fp)) {
        case AudioFile_WAV:
            success = wavread (fp);
            break;

        default:
            OPENVRML_PRINT_MESSAGE_("Unrecognized audio file format (" + url
                                    + ").");

            // Suppress the error message below
            success = true;
            break;
        }

        if (success == false) {
            OPENVRML_PRINT_MESSAGE_("Unable to read audio file (" + url
                                    + ").");
        }

        _doc->fclose ();
    } else {
        OPENVRML_PRINT_MESSAGE_("Unable to find audio file (" + url +").");
    }

    return (_num_samples > 0);
}

/**
 * @brief Try a list of URLs.
 *
 * @param urls      list of URLs.
 * @param relative  doc object.
 */
bool Audio::tryURLs(const mfstring & urls, doc * relative)
{
    for (size_t i = 0; i < urls.value.size(); ++i) {
        if (this->setURL(urls.value[i], relative)) { return true; }
    }
    return false;
}

/**
 * @brief Get the URL of the clip.
 *
 * @return the URL, or 0 if none exists.
 */
const char *Audio::url() const
{
    return (_doc ? _doc->url() : 0);
}

/**
 * @brief Read a WAV file.
 *
 * @param fp    file handle.
 *
 * @return @c true if the read succeeded; @c false otherwise.
 */
bool Audio::wavread (FILE *fp)
{
    WaveHeader wave_header;

    fread (&wave_header, sizeof(WaveHeader), 1, fp);
    rewind (fp);

    // Do all sorts of sanity checks
    if (strncmp ((const char*)wave_header.riff_id, "RIFF", 4) != 0)
        return false;

    if (strncmp ((const char*)wave_header.wave_id, "WAVE", 4) != 0)
        return false;

    if (strncmp ((const char*)wave_header.format_id, "fmt ", 4) != 0)
        return false;

    if (strncmp ((const char*)wave_header.data_id, "data", 4) != 0)
        return false;

    if (wave_header.format_tag != WAVE_FORMAT_PCM)
        return false;


    // Allocate the memory required
    delete _samples;
    _samples = new unsigned char[wave_header.num_data_bytes];
    if (_samples == 0)
        return false;

    // Now, we are ready to read the data
    fseek (fp, sizeof(WaveHeader), SEEK_SET);
    int bytes_read = fread (_samples, 1, wave_header.num_data_bytes, fp);

    _encoding         = AUDIO_LINEAR;
    _channels         = wave_header.num_channels;
    _bits_per_sample  = wave_header.bits_per_sample;
    _samples_per_sec  = wave_header.num_samples_per_sec;
    _sample_blocksize = wave_header.num_block_align;
    _num_samples      = bytes_read / _sample_blocksize;


    return true;
}

} // namespace OpenVRML
