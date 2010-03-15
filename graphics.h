#ifndef GRAPHICS_H
#define GRAPHICS_H

#define FILENAME "realcube.3ds"
#include "C3dsParser.h"
#include "sound.h"
#include <stdint.h>
#include <vector>
#include <GL/gl.h>

class Graphics {

public:
    Graphics(int argc, char **argv);
    void run();

private:
    static void display();
    static void reshape(int w, int h);
    static void drawCube(float width, float height, float depth);
    static void drawScene();
    void compileObject();
    void drawObject();
    vector<GLuint> m_callLists;
    
    static Graphics *me;
    C3dsParser* m_parser;

    float m_angle;
    float m_angleSpeed;
    float m_scale;

    Sound m_sound;
    suseconds_t m_lastUpdate;

};

#endif//GRAPHICS_H
