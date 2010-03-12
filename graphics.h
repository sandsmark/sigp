#ifndef GRAPHICS_H
#define GRAPHICS_H

#define FILENAME "lolqueb.3ds"
#include "C3dsParser.h"
#include <stdint.h>
#include <map>

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
    map<string, face*>    m_faces;
    map<string, vector3*> m_vertices;
    map<string, int>      m_vertCount;
    map<string, int>      m_faceCount;
    map<string, color*>  m_faceMaterial;
    
    map<string, color>    m_colors;
    static Graphics *me;
    C3dsParser* m_parser;

};

#endif//GRAPHICS_H
