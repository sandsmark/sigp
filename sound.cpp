#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "sound.h"
#include "fht.h"
      
Sound::Sound (char *device) {
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

    if ((m_err = snd_pcm_hw_params_set_channels (m_captureHandle, hw_params, 2)) < 0) {
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
    m_mutex = PTHREAD_MUTEX_INITIALIZER;
    m_running = true;
}

Sound::~Sound() {
    m_running = false;

    pthread_mutex_lock(&m_mutex);
    delete m_samples;
    pthread_mutex_unlock(&m_mutex);
    snd_pcm_close (m_captureHandle);
}

void Sound::run() {
    while (m_running) {
        pthread_mutex_lock(&m_mutex);
        if ((m_err = snd_pcm_readi (m_captureHandle, m_samples, 128)) != 128) {
            fprintf (stderr, "read from audio interface failed (%s)\n",
                 snd_strerror (m_err));
            exit (1);
        }
        pthread_mutex_unlock(&m_mutex);
    }

    pthread_mutex_unlock;
}

int Sound::getBass() {
        pthread_mutex_lock(&m_mutex);
        pthread_mutex_unlock(&m_mutex);
}

