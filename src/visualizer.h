#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <stdbool.h>
#include <stddef.h>
#include <SDL2/SDL.h>

// Inicializa el visualizador con un tipo específico
bool visualizer_init(int width, int height, const char *type, const char *window_title);

// Actualiza el visualizador con nuevos datos de audio
void visualizer_update(const void *audio_data, size_t data_size, int channels, int format, int sampleRate);

// Textura compartida para freqwalklin
extern SDL_Texture *waterfall_texture;

// Actualiza los eventos de ventana
void visualizer_events();

// Libera los recursos del visualizador
void visualizer_cleanup();

// Verifica si un tipo de visualización está disponible
bool visualizer_is_type_available(const char *type);


// --- Prototipos de visualizadores waterfall ---

void draw_freq_linear(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, bool *running);

void draw_freq_logarithmic(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, bool *running);

void draw_freqwalk_linear(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, int sampleRate, bool *running);

void draw_freqwalk_logarithmic(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, int sampleRate, bool *running);

void draw_freqwalk_fit_linear(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, int sampleRate, bool *running);

void draw_freqwalk_fit_logarithmic(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, int sampleRate, bool *running);

void draw_spectrum(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, bool *running);

void draw_waveform_dot(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h);

void draw_waveform_line(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h);


#endif
