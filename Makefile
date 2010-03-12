demo: common.cpp C3dsParser.cpp C3dsParser.h graphics.cpp graphics.h sound.h sound.cpp main.cpp fht.cpp fht.h
	g++ common.cpp fht.cpp graphics.cpp sound.cpp main.cpp C3dsParser.cpp -g -lGL -lGLU -lglut -lasound -std=gnu++0x -o demo

.PHONY :  clean

clean:
	rm demo

run:demo
	./demo
