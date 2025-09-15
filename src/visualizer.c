#include "visualizer.h"
#include "visual_spectrum.h"
#include "visual_waveform.h"
#include "visual_freqwalk_linear.h"
#include "visual_freqwalk_logarithmic.h"
#include "visual_freqwalk_fit_linear.h"
#include "visual_freqwalk_fit_logarithmic.h"
#include "visual_freq_linear.h"
#include "visual_freq_logarithmic.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <string.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static bool running = true;
static const char *current_visualization = NULL;


// Imprimir atajos de teclado del visualizador
void print_help_visualizer() {
  printf("\n\n");
  printf("Atajos de teclado de Visualizer:\n");
  printf("  Alt+F4   Cierra la aplicación.\n");
  printf("  Ctrl+C   Cierra la aplicación.\n");
  printf("  Escape   Cierra la aplicación.\n");
  printf("  h        Muestra esta ayuda.\n");
  printf("  0        Visualización vacia.\n");
  printf("  1        Visualización Waveform-Punto.\n");
  printf("  2        Visualización Waveform-Línea.\n");
  printf("  3        Visualización Spectrum.\n");
  printf("  4        Visualización FreqWalk-Lineal.\n");
  printf("  5        Visualización FreqWalk-Ajustado-Lineal.\n");
  printf("  6        Visualización FreqWalk-Logarítmico.\n");
  printf("  7        Visualización FreqWalk-Ajustado-Logarítmico.\n");
  printf("  8        Visualización Freq-Lineal.\n");
  printf("  9        Visualización Freq-Logarítmico.\n");
  printf("\n\n");
}


// Función interna para no dibujar nada
void draw_none(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Limpiar pantalla
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);
}


// Inicializa el visualizador con un tipo específico
bool visualizer_init(int width, int height, const char *type, const char *window_title) {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
    fprintf(stderr, "Error al inicializar SDL: %s\n", SDL_GetError());
    return false;
  }

  window = SDL_CreateWindow(window_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            width, height, SDL_WINDOW_RESIZABLE);
  if (!window) {
    fprintf(stderr, "Error al crear la ventana: %s\n", SDL_GetError());
    SDL_Quit();
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    fprintf(stderr, "Error al crear el renderizador: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return false;
  }

  current_visualization = type;
  return true;
}


// Actualiza el visualizador con nuevos datos de audio
void visualizer_update(const void *audio_data, size_t data_size, int channels, int format, int sampleRate) {
  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  if (strcmp(current_visualization, "waveformdot") == 0) {
    draw_waveform_dot(renderer, audio_data, data_size, channels, format, w, h);

  } else if (strcmp(current_visualization, "waveformline") == 0) {
    draw_waveform_line(renderer, audio_data, data_size, channels, format, w, h);

  } else if (strcmp(current_visualization, "spectrum") == 0) {
    draw_spectrum(renderer, audio_data, data_size, channels, format, w, h, &running);

  } else if (strcmp(current_visualization, "freqwalklin") == 0) {
    draw_freqwalk_linear(renderer, audio_data, data_size, channels, format, w, h, sampleRate, &running);

  } else if (strcmp(current_visualization, "freqwalkfitlin") == 0) {
    draw_freqwalk_fit_linear(renderer, audio_data, data_size, channels, format, w, h, sampleRate, &running);

  } else if (strcmp(current_visualization, "freqwalklog") == 0) {
    draw_freqwalk_logarithmic(renderer, audio_data, data_size, channels, format, w, h, sampleRate, &running);

  } else if (strcmp(current_visualization, "freqwalkfitlog") == 0) {
    draw_freqwalk_fit_logarithmic(renderer, audio_data, data_size, channels, format, w, h, sampleRate, &running);

  } else if (strcmp(current_visualization, "freqlin") == 0) {
    draw_freq_linear(renderer, audio_data, data_size, channels, format, w, h, sampleRate, &running);

  } else if (strcmp(current_visualization, "freqlog") == 0) {
    draw_freq_logarithmic(renderer, audio_data, data_size, channels, format, w, h, sampleRate, &running);

  } else {
    draw_none(renderer);
  }
}


// Actualiza los eventos de ventana
void visualizer_events() {
  // Manejar eventos SDL (redimensionamiento, cierre)
  SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {

      case SDL_QUIT:
        // Si el usuario cierra la ventana (Alt+F4 o clic en el botón de cierre)
        running = false;
        break;

      case SDL_KEYDOWN:
        // Si el usuario presiona la tecla ...
        if (
            event.key.keysym.sym == SDLK_ESCAPE // ... 'Escape'
            || (event.key.keysym.sym == SDLK_c && (event.key.keysym.mod & KMOD_CTRL)) // ... 'Ctrl+C'
            ) {
          running = false;

        } else if (event.key.keysym.sym == SDLK_0) { // ... '0'
          current_visualization = "none";
          printf("Visualización actual: none \n");

        } else if (event.key.keysym.sym == SDLK_1) { // ... '1'
          current_visualization = "waveformdot";
          printf("Visualización actual: Waveform-Punto.\n");

        } else if (event.key.keysym.sym == SDLK_2) { // ... '2'
          current_visualization = "waveformline";
          printf("Visualización actual: Waveform-Linea.\n");

        } else if (event.key.keysym.sym == SDLK_3) { // ... '3'
          current_visualization = "spectrum";
          printf("Visualización actual: Spectrum.\n");

        } else if (event.key.keysym.sym == SDLK_4) { // ... '4'
          current_visualization = "freqwalklin";
          printf("Visualización actual: FreqWalk-Lineal.\n");

        } else if (event.key.keysym.sym == SDLK_5) { // ... '5'
          current_visualization = "freqwalkfitlin";
          printf("Visualización actual: FreqWalk-Ajustado-Lineal.\n");

        } else if (event.key.keysym.sym == SDLK_6) { // ... '6'
          current_visualization = "freqwalklog";
          printf("Visualización actual: FreqWalk-Logarítmico.\n");

        } else if (event.key.keysym.sym == SDLK_7) { // ... '7'
          current_visualization = "freqwalkfitlog";
          printf("Visualización actual: FreqWalk-Ajustado-Logarítmico.\n");

        } else if (event.key.keysym.sym == SDLK_8) { // ... '8'
          current_visualization = "freqlin";
          printf("Visualización actual: Freq-Lineal.\n");

        } else if (event.key.keysym.sym == SDLK_9) { // ... '9'
          current_visualization = "freqlog";
          printf("Visualización actual: Freq-Logarítmico.\n");

        } else if (event.key.keysym.sym == SDLK_h) { // ... 'h'
          print_help_visualizer();
        }
        break;
      }
    }

    // Si running es falso, detener la reproducción
    if (!running) {
      printf("Cerrando aplicación...\n");
      exit(0); // Termina el programa
    }
}


// Libera los recursos del visualizador
void visualizer_cleanup() {
  if (renderer) SDL_DestroyRenderer(renderer);
  if (window) SDL_DestroyWindow(window);
  SDL_Quit();
}


// Verifica si un tipo de visualización está disponible
bool visualizer_is_type_available(const char *type) {
  return
    strcmp(type, "waveformline") == 0
    || strcmp(type, "waveformdot") == 0
    || strcmp(type, "spectrum") == 0
    || strcmp(type, "freqwalklin") == 0
    || strcmp(type, "freqwalkfitlin") == 0
    || strcmp(type, "freqwalklog") == 0
    || strcmp(type, "freqwalkfitlog") == 0
    || strcmp(type, "freqlin") == 0
    || strcmp(type, "freqlog") == 0;
}
