#ifndef GRAPHICS_H
#define GRAPHICS_H

#define FILENAME "cube.3ds"
#include "C3dsParser.h"
#include <stdint.h>
#include <map>
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
    map<string, GLuint> m_callLists;
    
    static Graphics *me;
    C3dsParser* m_parser;

    float m_angle;
    float m_angleSpeed;
    float m_scale;

};

#endif//GRAPHICS_H
