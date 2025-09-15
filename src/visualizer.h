#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <stdbool.h>
#include <stddef.h>

// Inicializa el visualizador con un tipo específico
bool visualizer_init(int width, int height, const char *type, const char *window_title);

// Actualiza el visualizador con nuevos datos de audio
void visualizer_update(const void *audio_data, size_t data_size, int channels, int format, int sampleRate);

// Actualiza los eventos de ventana
void visualizer_events();

// Libera los recursos del visualizador
void visualizer_cleanup();

// Verifica si un tipo de visualización está disponible
bool visualizer_is_type_available(const char *type);

#endif
