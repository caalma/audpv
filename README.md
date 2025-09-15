# AUDio Player Visualizer

Reproductor y visualizador de datos crudos, como si fueran audios.

## Requerimientos

    sudo apt install cmake build-essential portaudio19-dev libsdl2-dev libfftw3-dev

## Compilación

    make

## Reseteo del proyecto

    make clean

## tests

    make test_wav_v_none
    make test_v_none
    make test_v_freq_lineal
    make test_v_freq_logaritmico
    make test_v_freqwalk_fit_lineal
    make test_v_freqwalk_fit_logaritmico
    make test_v_freqwalk_lineal
    make test_v_freqwalk_logaritmico
    make test_v_spectrum
    make test_v_waveform_linea
    make test_v_waveform_punto
