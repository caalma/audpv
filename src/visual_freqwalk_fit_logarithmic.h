#ifndef VISUAL_FREQWALK_LOGARITHMIC_H
#define VISUAL_FREQWALK_LOGARITHMIC_H

#include <SDL2/SDL.h>
#include <stddef.h>

void draw_freqwalk_fit_logarithmic(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, int sampleRate, bool *running);

#endif
