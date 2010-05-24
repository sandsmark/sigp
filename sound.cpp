// Sound capture and analyzation
//
// Copyright (C) 2010  Martin Sandsmark - martin.sandsmark@kde.org
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, 51 Franklin Steet, Fifth Floor, Boston, MA 02110-1301, USA

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "sound.h"
      
Sound::Sound (const char *device) : 
     m_fht(7),
     m_stopping(false)
{
    snd_pcm_hw_params_t *hw_params;
    unsigned int rate = 44100;

    if ((m_err = snd_pcm_open (&m_captureHandle, device, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf (stderr, "cannot open audio device %s (%s)\n", 
             device,
             snd_strerror (m_err));
        exit (1);
    }
       
    if ((m_err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
             snd_strerror (m_err));
        exit (1);
    }
             
    if ((m_err = snd_pcm_hw_params_any (m_captureHandle, hw_params)) < 0) {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
             snd_strerror (m_err));
        exit (1);
    }

    if ((m_err = snd_pcm_hw_params_set_access (m_captureHandle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf (stderr, "cannot set access type (%s)\n",
             snd_strerror (m_err));
        exit (1);
    }

    if ((m_err = snd_pcm_hw_params_set_format (m_captureHandle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
        fprintf (stderr, "cannot set sample format (%s)\n",
             snd_strerror (m_err));
        exit (1);
    }

    if ((m_err = snd_pcm_hw_params_set_rate_near (m_captureHandle, hw_params, &rate, 0)) < 0) {
        fprintf (stderr, "cannot set sample rate (%s)\n",
             snd_strerror (m_err));
        exit (1);
    }

    if ((m_err = snd_pcm_hw_params_set_channels (m_captureHandle, hw_params, 1)) < 0) {
        fprintf (stderr, "cannot set channel count (%s)\n",
             snd_strerror (m_err));
        exit (1);
    }

    if ((m_err = snd_pcm_hw_params (m_captureHandle, hw_params)) < 0) {
        fprintf (stderr, "cannot set parameters (%s)\n",
             snd_strerror (m_err));
        exit (1);
    }

    snd_pcm_hw_params_free (hw_params);

    if ((m_err = snd_pcm_prepare (m_captureHandle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
             snd_strerror (m_err));
        exit (1);
    }

    m_samples = new int16_t[128];
    m_history = new float[128];

    m_mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t)); 
    pthread_mutex_init(m_mutex, 0); // fuck error handling
    m_thread = (pthread_t*)malloc(sizeof(pthread_t));
    pthread_create(m_thread, 0, &Sound::startLoop, this);
}

Sound::~Sound() {
    m_stopping = true;
    pthread_join((*m_thread), 0);

    delete m_samples;
    snd_pcm_close (m_captureHandle);

    pthread_mutex_destroy(m_mutex);
}

void *Sound::startLoop(void *obj) {
    reinterpret_cast<Sound*>(obj)->mainloop();
}

void Sound::mainloop() {
    int16_t *buffer;
    int16_t *tmp;
    while (!m_stopping) {
        if ((m_err = snd_pcm_readi (m_captureHandle, buffer, 128)) != 128) {
            fprintf (stderr, "read from audio interface failed (%s)\n",
                 snd_strerror (m_err));
            exit (1);
        }

        // Switcharoo
        pthread_mutex_lock(m_mutex);
        tmp = m_samples;
        m_samples = buffer;
        pthread_mutex_unlock(m_mutex);
        buffer = tmp;
    }

    pthread_mutex_unlock(m_mutex);
}

int Sound::getBass() {
    int bass = 0;
    pthread_mutex_lock(m_mutex);
    
    float *buffer = new float[128];
    float *input = new float[512];

    for (int i=0; i<128; i++) {
        input[i] = static_cast<float>(m_samples[i]) / static_cast<float>(0x7fff);
    }   
    pthread_mutex_unlock(m_mutex);

    m_fht.copy(buffer, input);
    m_fht.logSpectrum(input, buffer);
    m_fht.scale(buffer, 1.0/20);

    m_fht.ewma(m_history, buffer, .75);

    bass += 10 * buffer[1];// 128 audio samples in → 64 data points out
    
    return bass;
}

