//
// OpenVRML
//
// Copyright (C) 2000  Braden N. McDaniel
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

#include "sound.h"

int openSound(int channels, int bits_per_sample, int speed) {
    return -1;
}

int outputSoundChunk(int num_sample_bytes,
                     const unsigned char * samples,
                     int bits_per_sample,
                     int byte_index,
                     int loop,
                     double intensity,
                     int fd) {
    return -1;
}

int closeSound(int audio_fd) {
    return -1;
}
