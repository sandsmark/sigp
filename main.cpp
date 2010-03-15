#include <X11/Xlib.h>
#include "graphics.h"

int main (int argc, char **argv) {
//    XInitThreads();
    Graphics g(argc, argv);
    g.run();
    return 0;
}
