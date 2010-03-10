#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "sound.h"
      
Sound::Sound (char *device) : 
     m_fht(128),
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

    if (pthread_mutex_init(&m_mutex, 0))
        pthread_create(&m_thread, 0, &Sound::startLoop, this);
}

Sound::~Sound() {
    m_stopping = true;
    pthread_join(m_thread, 0);

    delete m_samples;
    snd_pcm_close (m_captureHandle);

    pthread_mutex_destroy(&m_mutex);
}

void *Sound::startLoop(void *obj) {
    reinterpret_cast<Sound*>(obj)->mainloop();
}

void Sound::mainloop() {
    while (!m_stopping) {
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
    int bass;
    pthread_mutex_lock(&m_mutex);
    
    float *spectrum = new float[m_fht.size()];
    float *samples = new float[m_fht.size()];
    for (short i=0; i<m_fht.size(); i++) {//bleh, must be a better way
        samples[i] = spectrum[i] = m_samples[i];
    }

//    m_fht.copy(&spectrum[0], samples);
    m_fht.logSpectrum(samples, spectrum);
    m_fht.scale(samples, 1.0 / 20);
    bass = 10 * samples[m_fht.size()/2 - 1];// 128 audio samples in → 64 data points out
    
    pthread_mutex_unlock(&m_mutex);

    return bass;
}

