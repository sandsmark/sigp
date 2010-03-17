// Graphics display
//
// Copyright (C) 2010  Martin Sandsmark - martin.sandsmark@kde.org
// Copyright (C) 2010  Amund Hov - amundhov@samfundet.no
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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#define FILENAME "cube.3ds"
#include "C3dsParser.h"
#include "sound.h"
#include <stdint.h>
#include <vector>
#include <GL/gl.h>

class Sound;

class Graphics {
public:
    Graphics(int argc, char **argv);

private:
    static void display();
    static void reshape(int w, int h);
    static void drawCube(float width, float height, float depth);
    static void drawScene();
    static void keydown(unsigned char key, int x, int y);
    static void compileObject();
    static vector<GLuint> m_callLists;
    
    static C3dsParser* m_parser;

    static float m_angle;
    static float m_angleSpeed;
    static float m_scale;
    static int m_angleSpin[3];

    static Sound m_sound;
    static suseconds_t m_lastUpdate;
};

#endif//GRAPHICS_H
