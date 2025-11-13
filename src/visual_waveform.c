#include <SDL2/SDL.h>
#include <portaudio.h>

void draw_waveform_dot(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h) {
  size_t samples_per_channel = data_size / (channels * (format == paInt16 ? sizeof(int16_t) : sizeof(float)));
  float step = (float)w / samples_per_channel;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Limpiar pantalla
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Color blanco para el waveform

  int prev_x = -1, prev_y = -1; // Variables para almacenar la posición anterior

  for (size_t i = 0; i < samples_per_channel - 1; i++) {
    float sample1 = 0.0f, sample2 = 0.0f;
    if (format == paInt16) {
      int16_t *data = (int16_t *)audio_data;
      sample1 = data[i * channels] / 32768.0f; // Normalizar a [-1, 1]
      sample2 = data[(i + 1) * channels] / 32768.0f;
    } else if (format == paFloat32) {
      float *data = (float *)audio_data;
      sample1 = data[i * channels];
      sample2 = data[(i + 1) * channels];
    }

    int x1 = i * step;
    int y1 = h / 2 - (int)(sample1 * h / 2);
    int x2 = (i + 1) * step;
    int y2 = h / 2 - (int)(sample2 * h / 2);

    // Interpolar puntos adicionales entre (x1, y1) y (x2, y2)
    int num_interpolated_points = 1; // Número de puntos interpolados
    for (int j = 0; j <= num_interpolated_points; j++) {
      float t = (float)j / num_interpolated_points;
      int x_interp = x1 + t * (x2 - x1);
      int y_interp = y1 + t * (y2 - y1);

      if (prev_x != -1 && prev_y != -1) {
        //SDL_RenderDrawLine(renderer, prev_x, prev_y, x_interp, y_interp);
        SDL_RenderDrawPoint(renderer, prev_x, prev_y);

      }

      prev_x = x_interp;
      prev_y = y_interp;
    }
  }
  SDL_RenderPresent(renderer);
}


void draw_waveform_line(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h) {
  size_t samples_per_channel = data_size / (channels * (format == paInt16 ? sizeof(int16_t) : sizeof(float)));
  float step = (float)w / samples_per_channel;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Limpiar pantalla
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Color blanco para el waveform

  int prev_x = -1, prev_y = -1; // Variables para almacenar la posición anterior

  for (size_t i = 0; i < samples_per_channel; i++) {
    float sample = 0.0f;
    if (format == paInt16) {
      int16_t *data = (int16_t *)audio_data;
      sample = data[i * channels] / 32768.0f; // Normalizar a [-1, 1]
    } else if (format == paFloat32) {
      float *data = (float *)audio_data;
      sample = data[i * channels];
    }

    int x = i * step;
    int y = h / 2 - (int)(sample * h / 2);

    if (prev_x != -1 && prev_y != -1) {
      // Dibujar una línea desde el punto anterior al actual
      SDL_RenderDrawLine(renderer, prev_x, prev_y, x, y);
    }

    // Actualizar las coordenadas del punto anterior
    prev_x = x;
    prev_y = y;
  }

  SDL_RenderPresent(renderer);
}
