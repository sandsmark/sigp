default : common.cc C3dsParser.cc C3dsParser.h graphics.cpp graphics.h sound.h sound.cpp main.cpp
	g++ common.cc graphics.cpp sound.cpp main.cpp C3dsParser.cc -g -lGL -lGLU -lglut -lasound -std=gnu++0x -o demo

.PHONY :  clean

clean : 
	rm xwin

