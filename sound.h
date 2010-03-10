#ifndef SOUND_H
#define SOUND_H

#include <pthread.h>
#include <alsa/asoundlib.h>

#include "fht.h"

class Sound {
public:
    Sound(char *device);
    ~Sound();
    void run();
    void stop();
    int getBass();

private:
    int16_t *m_samples;
    pthread_mutex_t m_mutex;
    bool m_running;
    snd_pcm_t *m_captureHandle;
    int m_err;
    FHT m_fht;
};

#endif //SOUND_H
