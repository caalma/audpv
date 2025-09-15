#ifndef VISUAL_SPECTRUM_H
#define VISUAL_SPECTRUM_H

#include <SDL2/SDL.h>
#include <stddef.h>

void draw_spectrum(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, bool *running);

#endif
