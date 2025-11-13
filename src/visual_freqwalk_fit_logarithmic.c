#include <SDL2/SDL.h>
#include <stdbool.h>
#include <portaudio.h>
#include <fftw3.h>
#include <math.h>

extern SDL_Texture *waterfall_texture;
static int texture_width = 0;
static int texture_height = 0;

void draw_freqwalk_fit_logarithmic(SDL_Renderer *renderer,
                                   const void *audio_data,
                                   size_t data_size,
                                   int channels,
                                   int format,
                                   int w,
                                   int h,
                                   int sampleRate,
                                   bool *running)
{
  (void)running;
  const int column_width = 1;

  size_t samples_per_channel = data_size / (channels * (format == paInt16 ? sizeof(int16_t) : sizeof(float)));
  fftw_complex *in = NULL, *out = NULL;
  fftw_plan plan;

  if (waterfall_texture == NULL || texture_width != w || texture_height != h) {
    if (waterfall_texture) SDL_DestroyTexture(waterfall_texture);
    waterfall_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    if (!waterfall_texture) return;

    texture_width = w;
    texture_height = h;
    SDL_SetTextureBlendMode(waterfall_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, waterfall_texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderTarget(renderer, NULL);
  }

  in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * samples_per_channel);
  out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * samples_per_channel);
  if (!in || !out) { if (in) fftw_free(in); if (out) fftw_free(out); return; }

  for (size_t i = 0; i < samples_per_channel; ++i) {
    if (format == paInt16) {
      const int16_t *data = (const int16_t *)audio_data;
      in[i][0] = data[i * channels] / 32768.0f;
    } else {
      const float *data = (const float *)audio_data;
      in[i][0] = data[i * channels];
    }
    in[i][1] = 0.0f;
  }

  plan = fftw_plan_dft_1d((int)samples_per_channel, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(plan);

  // Detectar frecuencia máxima activa
  size_t max_idx = 0;
  for (size_t i = 1; i < samples_per_channel / 2; ++i) {
    float mag = sqrtf(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
    if (mag > 0.01f && i > max_idx) max_idx = i;
  }
  double detected_max = (double)max_idx * sampleRate / samples_per_channel;
  double freq_max = fmax(200.0, detected_max);
  double freq_min = 20.0;

  SDL_SetRenderTarget(renderer, waterfall_texture);
  SDL_SetTextureAlphaMod(waterfall_texture, 240);
  SDL_RenderCopy(renderer, waterfall_texture, NULL, NULL);
  SDL_SetTextureAlphaMod(waterfall_texture, 255);

  SDL_Rect src = { 0, 0, w - column_width, h };
  SDL_Rect dst = { column_width, 0, w - column_width, h };
  SDL_RenderCopy(renderer, waterfall_texture, &src, &dst);

  for (int y = 0; y < h; ++y) {
    double ratio = (double)y / (h - 1);
    double freq = freq_min * pow(freq_max / freq_min, ratio);

    size_t idx = (size_t)((freq / (double)sampleRate) * samples_per_channel);
    if (idx >= samples_per_channel / 2) idx = samples_per_channel / 2 - 1;

    float mag = sqrtf(out[idx][0] * out[idx][0] + out[idx][1] * out[idx][1]);
    mag /= (float)samples_per_channel;
    mag = powf(mag + 0.001f, 0.5f);

    Uint8 intensity = (Uint8)(mag * 255.0f);
    SDL_SetRenderDrawColor(renderer, intensity, intensity, intensity, 255);

    SDL_Rect rect = { 0, h - y - 1, column_width, 1 };
    SDL_RenderFillRect(renderer, &rect);
  }

  SDL_SetRenderTarget(renderer, NULL);
  SDL_RenderCopy(renderer, waterfall_texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  fftw_destroy_plan(plan);
  fftw_free(in);
  fftw_free(out);
}
