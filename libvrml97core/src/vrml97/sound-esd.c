//
//  Vrml 97 library
//  Copyright (C) 1998 Chris Morley
//  See the file COPYING for license details.
//
//  sound.c
//    contributed by Kumaran Santhanam
//
//  Play sound files without tying up system resources.

#include "config.h"
#include "sound.h"

#if HAVE_ESD
#include <esd.h>
#include <unistd.h>
#endif

#include <stdio.h>

int openSound (int channels,
               int bits_per_sample,
               int speed)
{
#if HAVE_ESD 
  int esd_flags = ESD_STREAM | ESD_PLAY;
  int fd;

  printf("here is my data: %d %d %d\n", channels, bits_per_sample, speed);

  if (bits_per_sample == 8)
    esd_flags |= ESD_BITS8;
  else if (bits_per_sample == 16)
    esd_flags |= ESD_BITS16;
  else
    return -1;

  if (channels == 1)
    esd_flags |= ESD_MONO;
  else if (channels == 2)
    esd_flags |= ESD_STEREO;
  else
    return -1;

  fd = esd_play_stream(esd_flags, speed, "localhost", NULL);
  printf("got an fd!  here it is: %d\n", fd);
  return fd;
#else
  return -1;
#endif
}



int outputSoundChunk (int num_sample_bytes,
                      const unsigned char *samples,
                      int bits_per_sample,
                      int byte_index,
                      int loop,
                      double intensity,
                      int fd)
{
  int new_index = byte_index + 4096 * bits_per_sample; /* play up to 32768 samples */

  if (new_index >= num_sample_bytes)
    new_index = num_sample_bytes - 1;

#if HAVE_ESD
  if (fd > 0) {
    write(fd, &samples[byte_index], new_index - byte_index);
  }
#endif

  return new_index;
}



int closeSound (int audio_fd)
{
#if HAVE_ESD
  if (audio_fd > 0)
    close(audio_fd);
#endif
  return -1;
}

