#include <SDL2/SDL.h>
#include <stdbool.h>
#include <portaudio.h>
#include <fftw3.h>
#include <math.h>

void draw_freq_linear(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, bool *running) {
  (void)running;

  size_t samples_per_channel = data_size / (channels * (format == paInt16 ? sizeof(int16_t) : sizeof(float)));
  fftw_complex *in, *out;
  fftw_plan plan;

  in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * samples_per_channel);
  out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * samples_per_channel);

  for (size_t i = 0; i < samples_per_channel; i++) {
    if (format == paInt16) {
      int16_t *data = (int16_t *)audio_data;
      in[i][0] = data[i * channels] / 32768.0f;
    } else {
      float *data = (float *)audio_data;
      in[i][0] = data[i * channels];
    }
    in[i][1] = 0.0;
  }

  plan = fftw_plan_dft_1d(samples_per_channel, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(plan);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

  for (int i = 0; i < w; i++) {
    int freq_index = i * (samples_per_channel / 2) / w;
    float magnitude = sqrt(out[freq_index][0] * out[freq_index][0] + out[freq_index][1] * out[freq_index][1]);
    int bar_height = (int)(h * magnitude / 100.0);
    if (bar_height > h) bar_height = h;

    SDL_RenderDrawLine(renderer, i, h, i, h - bar_height);
  }

  SDL_RenderPresent(renderer);

  fftw_destroy_plan(plan);
  fftw_free(in);
  fftw_free(out);
}
