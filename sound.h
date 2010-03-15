#ifndef SOUND_H
#define SOUND_H

#include <pthread.h>
#include <alsa/asoundlib.h>

#include "fht.h"


/**
 * Class providing sound input and transforms to the frequency domain.
 */

class Sound {
public:
    Sound(const char *device);
    ~Sound();
    int getBass();

private:
    static void *startLoop(void *);
    void mainloop();

    int16_t *m_samples;
    pthread_mutex_t m_mutex;
    pthread_t *m_thread;
    FHT m_fht;

    bool m_stopping;

    snd_pcm_t *m_captureHandle;
    int m_err;

};

#endif //SOUND_H

