/*

OpenVRML

Copyright (C) 1999  Kumaran Santhanam
 
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

/*  Play sound files without tying up system resources. */

#ifndef _SOUND_
#define _SOUND_

#ifdef	__cplusplus
extern "C" {
#endif


// Open the sound device for playing.
//
// This function returns an audio file descriptor for usage as 'fd'
// in the other functions.  The number of audio channels that may be
// opened is dependent on system hardware.
int openSound (int channels,
               int bits_per_sample,
               int speed);


// Output a chunk of sound to the audio device
//
// This function outputs enough sound to fill the audio device's DMA
// buffer.  The byte_index is a current counter within the sample buffer.
// The new byte_index is returned as the function's return value.
//
// If loop is non-zero, the sample is treated as a continuous loop.
//
// The intensity value specifies what type of scaling should be performed.
// It ranges from 0 to 1.0 and is a floating point value.  0 signifies
// no sound at all, while 1.0 signifies that the sound be played at the
// full recorded volume.
//
// Keep in mind that only a CHUNK of the sample is output at a time.
// To output the whole sample, use something like this:
//
// int byte_index = 0;
// while (byte_index < num_sample_bytes)
//     outputSoundChunk (num_sample_bytes, samples, byte_index,
//                       false, 1.0, audio_fd, 0);
//
// In practice, this isn't used, since the whole points of this function
// is to provide a way to output a chunk of sound and process some other
// events before outputting the next chunk.
int outputSoundChunk (int num_sample_bytes,
                      const unsigned char *samples,
                      int bits_per_sample,
                      int byte_index,
                      int loop,
                      double intensity,
                      int fd);


// Close the sound device
//
// This closes the sound device associated with the audio file descriptor.
// This function always returns -1.
int closeSound (int audio_fd);



#ifdef	__cplusplus
}
#endif


#endif  /* _SOUND_ */
