#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <portaudio.h>
#include <signal.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "visualizer.h"

typedef struct {
  int format;                     // Formato de audio (ej.: paInt16, paFloat32)
  int channels;                   // Número de canales
  int sampleRate;                 // Tasa de muestreo
  int refreshRate;                // Tasa de refresco del monitor
  unsigned long framesPerBuffer;  // Cuadros en cada buffer de visualización
  bool running;                   // Bandera para controlar la ejecución
} AudioConfig;

static AudioConfig config = {0};
bool visualize = false;
const char *visualization_type = NULL;

// Función para mostrar la ayuda
void print_help(const char *program_name) {
  printf("Uso: %s [opciones]\n", program_name);
  printf("\nOpciones:\n");
  printf("  -f FORMATO      Especifica el formato de audio (s16le o f32le)\n");
  printf("  -r FRECUENCIA   Especifica la tasa de muestreo en Hz (ej. 8000, 44100)\n");
  printf("  -c CANALES      Especifica el número de canales (1 para mono, 2 para estéreo)\n");
  printf("  -v VISUALIZADOR Especifica el tipo de visualización:\n");
  printf("                     waveformdot, waveformline, spectrum,\n");
  printf("                     freqwalklog, freqwalklin.\n");
  printf("  -t              Establece el nombre de la ventana.\n");
  printf("  -h, --help      Muestra esta ayuda.\n");
  printf("\nNotas importantes:\n");
  printf("  - Este programa requiere un flujo de datos de audio (streaming) desde stdin.\n");
  printf("  - Ejemplo de uso con ffmpeg: ffmpeg -i archivo.mp3 -f s16le -ar 8000 - | %s -f s16le -r 8000 -c 1 -v waveformdot\n", program_name);
}


// Manejador de señal para detener la reproducción
void handle_signal(int sig) {
  if (sig == SIGINT) {
    config.running = false;
  }
}

// Callback de PortAudio para procesar los datos de audio
static int audioCallback(const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData) {
  (void) inputBuffer; // No usamos la entrada
  (void) timeInfo;
  (void) statusFlags;
  (void) userData;

  size_t bytesPerFrame = config.channels * (config.format == paInt16 ? sizeof(short) : sizeof(float));
  size_t bytesToRead = framesPerBuffer * bytesPerFrame;

  // Leer datos desde stdin
  size_t bytesRead = fread(outputBuffer, 1, bytesToRead, stdin);
  if (bytesRead < bytesToRead) {
    // Si no hay suficientes datos, rellenar con ceros
    memset((char *)outputBuffer + bytesRead, 0, bytesToRead - bytesRead);
  }

  if (visualize) {
    visualizer_update(outputBuffer, bytesToRead, config.channels, config.format, config.sampleRate);
  }

  return paContinue;
}


// Función para calcular FRAMES_PER_BUFFER dinámicamente
unsigned long calculate_frames_per_buffer(int sampleRate, int refreshRate) {
    unsigned long framesPerBuffer = sampleRate / refreshRate;

    // Asegurarse de que framesPerBuffer sea un valor razonable
    if (framesPerBuffer < 64) {
        framesPerBuffer = 64; // Mínimo razonable para evitar latencia excesiva
    } else if (framesPerBuffer > 8192) {
        framesPerBuffer = 8192; // Máximo razonable para evitar buffers demasiado grandes
    }

    return framesPerBuffer;
}


int get_refresh_rate() {
  // Inicializar SDL para obtener la frecuencia de refresco
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error al inicializar SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_DisplayMode display_mode;
    if (SDL_GetCurrentDisplayMode(0, &display_mode) != 0) {
        fprintf(stderr, "Error al obtener la frecuencia de refresco: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    int refresh_rate = display_mode.refresh_rate > 0 ? display_mode.refresh_rate : 60; // Predeterminado: 60 Hz
    return refresh_rate;
}



int main(int argc, char *argv[]) {
  // Configuración inicial
  config.format = paInt16; // Por defecto: s16le
  config.channels = 1;     // Por defecto: mono
  config.sampleRate = 8000; // Por defecto: 8kHz
  config.refreshRate = get_refresh_rate(); // Detectada dinámicamente
  config.running = true;

  char *window_title = "AudioPlayerVisualizer";

  // Parsear argumentos
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
      if (strcmp(argv[i + 1], "s16le") == 0) {
        config.format = paInt16;
      } else if (strcmp(argv[i + 1], "f32le") == 0) {
        config.format = paFloat32;
      } else {
        fprintf(stderr, "Formato no soportado: %s\n", argv[i + 1]);
        return 1;
      }
      i++;
    } else if (strcmp(argv[i], "-r") == 0 && i + 1 < argc) {
      config.sampleRate = atoi(argv[i + 1]);
      i++;
    } else if (strcmp(argv[i], "-c") == 0 && i + 1 < argc) {
      config.channels = atoi(argv[i + 1]);
      i++;
    } else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
      visualize = true;
      visualization_type = argv[i + 1];
      if (!visualizer_is_type_available(visualization_type)) {
        fprintf(stderr, "Tipo de visualización no soportado: %s\n", visualization_type);
        return 1;
      }
      i++;
    } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
      window_title = argv[i + 1];
      i++;
    } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      print_help(argv[0]);
      return 0;
    } else {
      fprintf(stderr, "Argumento desconocido: %s\n", argv[i]);
      print_help(argv[0]);
      return 1;
    }
  }

  // Validar argumentos obligatorios
  if (config.format == 0 || config.sampleRate <= 0 || config.channels <= 0) {
    fprintf(stderr, "Faltan argumentos obligatorios.\n");
    print_help(argv[0]);
    return 1;
  }

  // Definir dinámicamente el FramesPerBuffer según el samplerate seleccionado
  config.framesPerBuffer = calculate_frames_per_buffer(config.sampleRate, config.refreshRate);

  // Inicializar PortAudio
  PaError err = Pa_Initialize();
  if (err != paNoError) {
    fprintf(stderr, "Error al inicializar PortAudio: %s\n", Pa_GetErrorText(err));
    return 1;
  }

  // Configurar el stream
  PaStream *stream;
  err = Pa_OpenDefaultStream(&stream, 0, config.channels, config.format,
                             config.sampleRate, config.framesPerBuffer,
                             audioCallback, NULL);
  if (err != paNoError) {
    fprintf(stderr, "Error al abrir el stream: %s\n", Pa_GetErrorText(err));
    Pa_Terminate();
    print_help(argv[0]);
    return 1;
  }

  // Seteo de visulizador
  if (visualize) {
    if (!visualizer_init(800, 400, visualization_type, window_title)) {
      fprintf(stderr, "Error al inicializar el visualizador.\n");
      Pa_CloseStream(stream);
      Pa_Terminate();
      return 1;
    }
  }

  // Iniciar el stream
  err = Pa_StartStream(stream);
  if (err != paNoError) {
    fprintf(stderr, "Error al iniciar el stream: %s\n", Pa_GetErrorText(err));
    Pa_CloseStream(stream);
    Pa_Terminate();
    //print_help(argv[0]);
    return 1;
  }

  // Manejar señales para detener la reproducción
  signal(SIGINT, handle_signal);

  printf("Reproduciendo... Presiona Ctrl+C para detener.\n");

  // Loop principal basado en eventos de SDL
  while (config.running) {
    visualizer_events();
    Pa_Sleep(10);
  }

  // Detener y cerrar el stream
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  Pa_Terminate();

  if (visualize && strcmp(visualization_type, "waveform") == 0) {
    visualizer_cleanup();
  }

  printf("\nReproducción detenida.\n");
  return 0;
}
