#ifndef GRAPHICS_H
#define GRAPHICS_H

#define FILENAME "lekube.3ds"
#include "C3dsParser.h"
#include <stdint.h>

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
    vector3 *m_vertices;
    face    *m_faces;
    int     m_vertCount;
    int     m_faceCount;
    static Graphics *me;
    C3dsParser* m_parser;

};

#endif//GRAPHICS_H
