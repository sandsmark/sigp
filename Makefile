# COPYRIGHT 2010 Martin T. Sandsmark
OBJS = C3dsParser.o graphics.o sound.o main.o fht.o
CC = g++
CFLAGS =-c -g -std=c++0x
LFLAGS =
LIBS =-lGL -lGLU -lglut -lasound -lpthread

run: demo
	./demo

demo: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(LIBS) -o $@ 

C3dsParser.o : C3dsParser.cpp C3dsParser.h
	$(CC) $(CFLAGS) -o $@ C3dsParser.cpp

graphics.o : graphics.cpp graphics.h C3dsParser.h chunks.def sound.h vector.h
	$(CC) $(CFLAGS) -o $@ graphics.cpp

sound.o : sound.cpp sound.h fht.h
	$(CC) $(CFLAGS) -o $@ sound.cpp

main.o : main.cpp graphics.h
	$(CC) $(CFLAGS) -o $@ main.cpp

fht.o : fht.cpp fht.h
	$(CC) $(CFLAGS) -o $@ fht.cpp

.PHONY :  clean

clean:
	rm demo $(OBJS)
