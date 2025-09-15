#include <SDL2/SDL.h>
#include <stdbool.h>
#include <portaudio.h>
#include <fftw3.h>


// Función interna para dibujar el analizador de frecuencias
void draw_spectrum(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, bool *running) {
  size_t samples_per_channel = data_size / (channels * (format == paInt16 ? sizeof(int16_t) : sizeof(float)));
  fftw_complex *in, *out;
  fftw_plan plan;

  // Reservar memoria para la FFT
  in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * samples_per_channel);
  out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * samples_per_channel);

  // Copiar datos de audio al buffer de entrada
  for (size_t i = 0; i < samples_per_channel; i++) {
    if (format == paInt16) {
      int16_t *data = (int16_t *)audio_data;
      in[i][0] = data[i * channels] / 32768.0f; // Parte real
      in[i][1] = 0.0f;                         // Parte imaginaria
    } else if (format == paFloat32) {
      float *data = (float *)audio_data;
      in[i][0] = data[i * channels];           // Parte real
      in[i][1] = 0.0f;                         // Parte imaginaria
    }
  }

  // Crear el plan FFT
  plan = fftw_plan_dft_1d(samples_per_channel, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

  // Ejecutar la FFT
  fftw_execute(plan);

  // Limpiar pantalla
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Color blanco para el espectro

  // Dibujar el espectro
  for (size_t i = 0; i < samples_per_channel / 2; i++) { // Solo la mitad positiva de las frecuencias
    float magnitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
    int bar_height = (int)(magnitude * h / samples_per_channel);
    if (bar_height > h) bar_height = h;

    SDL_Rect rect = {i * (w / (samples_per_channel / 2)), h - bar_height, 2, bar_height};
    SDL_RenderFillRect(renderer, &rect);
  }

  SDL_RenderPresent(renderer);

  // Liberar recursos de FFT
  fftw_destroy_plan(plan);
  fftw_free(in);
  fftw_free(out);

  // Manejar eventos
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      *running = false;
    }
  }
}
