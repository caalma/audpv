# Compilador y opciones
CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lportaudio -lSDL2 -lfftw3 -lm

# Directorios
SRC_DIR = src
OBJ_DIR = tmp
BIN_DIR = bin

# Generador de audio
WAV = cat ./extra/audio.wav
GENERADOR = ./extra/audio_bb_generator "t 5 >> t 11 >> & t *" "0"

# Lista de archivos fuente y objetos
SRCS = audpv.c visualizer.c visual_waveform.c visual_spectrum.c \
	   visual_freqwalk_linear.c visual_freqwalk_fit_linear.c \
	   visual_freqwalk_logarithmic.c visual_freqwalk_fit_logarithmic.c \
	   visual_freq_linear.c visual_freq_logarithmic.c

OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

# Binario final
TARGET = $(BIN_DIR)/audpv

# Regla principal: construir el binario
all: $(TARGET)

# Construcción del binario
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(LIBS)

# Reglas para compilar cada archivo objeto
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpieza
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Reglas de prueba
test_wav_v_none:
	$(WAV) | ./$(TARGET) -f s16le -r 8000 -c 1 2>/dev/null

test_v_none:
	$(GENERADOR) | ./$(TARGET) -f s16le -r 8000 -c 1 2>/dev/null

test_v_waveform_punto:
	$(GENERADOR) | ./$(TARGET) -f s16le -r 8000 -c 1 -v waveformdot 2>/dev/null

test_v_waveform_linea:
	$(GENERADOR) | ./$(TARGET) -f s16le -r 8000 -c 1 -v waveformline 2>/dev/null

test_v_spectrum:
	$(GENERADOR) | ./$(TARGET) -f s16le -r 8000 -c 1 -v spectrum 2>/dev/null

test_v_freqwalk_lineal:
	$(GENERADOR) | ./$(TARGET) -f s16le -r 8000 -c 1 -v freqwalklin 2>/dev/null

test_v_freqwalk_logaritmico:
	$(GENERADOR) | ./$(TARGET) -f s16le -r 8000 -c 1 -v freqwalklog 2>/dev/null

test_v_freqwalk_fit_lineal:
	$(GENERADOR) | ./$(TARGET) -f s16le -r 8000 -c 1 -v freqwalkfitlin 2>/dev/null

test_v_freqwalk_fit_logaritmico:
	$(GENERADOR) | ./$(TARGET) -f s16le -r 8000 -c 1 -v freqwalkfitlog 2>/dev/null

test_v_freq_lineal:
	$(GENERADOR) | ./$(TARGET) -f s16le -r 8000 -c 1 -v freqlin 2>/dev/null

test_v_freq_logaritmico:
	$(GENERADOR) | ./$(TARGET) -f s16le -r 8000 -c 1 -v freqlog 2>/dev/null
