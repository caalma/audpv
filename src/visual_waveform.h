#ifndef VISUAL_WAVEFORM_H
#define VISUAL_WAVEFORM_H

#include <SDL2/SDL.h>
#include <stddef.h>

void draw_waveform_dot(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h);

void draw_waveform_line(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h);

#endif // VISUAL_WAVEFORM_H
