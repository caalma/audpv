#ifndef VISUAL_FREQ_LINEAR_H
#define VISUAL_FREQ_LINEAR_H

#include <SDL2/SDL.h>
#include <stddef.h>

void draw_freq_linear(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, int sampleRate, bool *running);

#endif
