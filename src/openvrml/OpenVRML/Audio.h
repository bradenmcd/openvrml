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

#ifndef AUDIO_H
#define AUDIO_H

#include <stdio.h>
#include <math.h>
#include <string>
#include "common.h"

namespace OpenVRML {

    class Doc;
    class mfstring;

    OPENVRML_SCOPE enum AudioEncoding { AUDIO_LINEAR, AUDIO_ULAW };

    class OPENVRML_SCOPE Audio {
        Doc *           _doc;

        AudioEncoding   _encoding;
        int             _channels;
        int             _bits_per_sample;
        int             _samples_per_sec;

        // Samples are stored in aligned blocks.  Sometimes, the
        // block will be larger than the sample itself.  Usually,
        // however, an 8-bit sample will be in a 1-byte block and
        // a 16-bit sample will be in a 2-byte block.
        int             _sample_blocksize;

        int             _num_samples;
        unsigned char * _samples;

    public:
        explicit Audio(const std::string & url, Doc * relative = 0);
        ~Audio ();

        bool setURL(const std::string & url, Doc * relative = 0);
        bool tryURLs(const mfstring & urls, Doc * relative = 0);

        const char *url() const;

        AudioEncoding encoding() const        { return _encoding; }
        int channels() const                  { return _channels; }
        int bitsPerSample() const             { return _bits_per_sample; }
        int samplesPerSec() const             { return _samples_per_sec; }
        int sampleBlockSize() const           { return _sample_blocksize; }
        int numSamples() const                { return _num_samples; }

        int numBytes() const         { return _num_samples * _sample_blocksize; }
        const unsigned char *samples() const  { return _samples; }

        double duration() const  {
            if (_samples_per_sec > 0)
                return (double)_num_samples/(double)_samples_per_sec;
            else
                return 0;
        }

        // Get the sample index given a floating point time index
        // If the time index is greater than the duration, the sample
        // index is wrapped back to the beginning of the sample.
        // From: Alex Funk <Alexander.Funk@nord-com.net>
        // Avoid int overflow when multiplying time_index by samples_per_sec
        // Modified to use fmod() by Kumaran Santhanam.
        int getByteIndex(double time_index) const  {
          if (_num_samples > 0 && _samples_per_sec > 0)
	    return _sample_blocksize *
	      (int)(fmod(time_index, duration()) * (double)_samples_per_sec);
          else
	    return -1;
        }

    private:
        bool wavread (FILE *fp);
    };
}

#endif // AUDIO_H
