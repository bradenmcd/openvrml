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
