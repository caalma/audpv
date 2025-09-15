#include <SDL2/SDL.h>
#include <stdbool.h>
#include <portaudio.h>
#include <fftw3.h>


void draw_freqwalk_fit_logarithmic(SDL_Renderer *renderer, const void *audio_data, size_t data_size, int channels, int format, int w, int h, int sampleRate, bool *running) {
    static int x_offset = 0; // Posición horizontal actual del barrido
    const int column_width = 1; // Ancho de cada columna

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

    // Limpiar la columna anterior
    // --- DESHABILITADA
    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //SDL_Rect clear_rect = {x_offset, 0, column_width, h};
    //SDL_RenderFillRect(renderer, &clear_rect);

    // Detectar el rango de frecuencias activas
    size_t max_active_index = 0;
    for (size_t i = 0; i < samples_per_channel / 2; i++) {
        float magnitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
        if (magnitude > 0.01 && i > max_active_index) { // Umbral de actividad
            max_active_index = i;
        }
    }

    double freq_min = 20.0; // Frecuencia mínima (20 Hz)
    double freq_max = (double)max_active_index * sampleRate / samples_per_channel;
    if (freq_max < 200.0) freq_max = 200.0; // Evitar rangos demasiado pequeños

    // Dibujar el espectro ajustado
    for (int y = 0; y < h; y++) {
        // Calcular la frecuencia correspondiente al píxel y
        double freq = freq_min * pow(freq_max / freq_min, (double)y / (h - 1));

        // Mapear la frecuencia al índice de la FFT
        size_t fft_index = (size_t)((freq / sampleRate) * samples_per_channel);
        if (fft_index >= samples_per_channel / 2) fft_index = samples_per_channel / 2 - 1;

        // Obtener la magnitud de la frecuencia
        float magnitude = sqrt(out[fft_index][0] * out[fft_index][0] + out[fft_index][1] * out[fft_index][1]);
        magnitude /= samples_per_channel; // Normalizar

        // Aplicar una transformación no lineal para potenciar la intensidad
        float gamma = 0.55; // Factor de ajuste (menor que 1 para amplificar intensidades bajas)
        magnitude = pow(magnitude + 0.001f, gamma); // Añadir un pequeño offset para evitar log(0)

        // Asignar un color basado en la intensidad
        Uint8 intensity = (Uint8)(magnitude * 255); // Escalar a [0, 255]
        SDL_SetRenderDrawColor(renderer, intensity, intensity, intensity, 255);

        // Dibujar un rectángulo de 1 píxel de alto
        SDL_Rect rect = {x_offset, h - y - 1, column_width, 1};
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_RenderPresent(renderer);

    // Actualizar la posición horizontal
    x_offset += column_width;
    if (x_offset >= w) x_offset = 0;

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
