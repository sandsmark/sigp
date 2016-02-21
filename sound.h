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
    float getBass();

private:
    static void *startLoop(void *);
    void mainloop();

    int16_t *m_samples;
    float *m_history;
    pthread_mutex_t *m_mutex;
    pthread_t *m_thread;
    FHT m_fht;

    bool m_stopping;

    snd_pcm_t *m_captureHandle;
    int m_err;

};

#endif //SOUND_H

