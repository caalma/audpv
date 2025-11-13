#include <SDL2/SDL.h>
#include <stdbool.h>
#include <portaudio.h>
#include <fftw3.h>
#include <math.h>

void draw_freq_logarithmic(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, bool *running) {
    (void)running;

    size_t samples_per_channel = data_size / (channels * (format == paInt16 ? sizeof(int16_t) : sizeof(float)));
    fftw_complex *in = NULL, *out = NULL;
    fftw_plan plan;

    in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * samples_per_channel);
    out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * samples_per_channel);
    if (!in || !out) {
        if (in) fftw_free(in);
        if (out) fftw_free(out);
        return;
    }

    for (size_t i = 0; i < samples_per_channel; i++) {
        if (format == paInt16) {
            const int16_t *data = (const int16_t *)audio_data;
            in[i][0] = data[i * channels] / 32768.0f;
        } else {
            const float *data = (const float *)audio_data;
            in[i][0] = data[i * channels];
        }
        in[i][1] = 0.0;
    }

    plan = fftw_plan_dft_1d((int)samples_per_channel, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; i < w; i++) {
        double log_pos = pow(10.0, (double)i / w * log10((double)samples_per_channel / 2.0));
        size_t freq_index = (size_t)log_pos;  // ← Aquí el cambio clave
        if (freq_index >= samples_per_channel / 2)
            freq_index = samples_per_channel / 2 - 1;

        float magnitude = sqrtf(out[freq_index][0] * out[freq_index][0] + out[freq_index][1] * out[freq_index][1]);
        int bar_height = (int)(h * magnitude / 100.0);
        if (bar_height > h) bar_height = h;

        SDL_RenderDrawLine(renderer, i, h, i, h - bar_height);
    }

    SDL_RenderPresent(renderer);

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
}
