#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "tmp.h"
#include "common.h"
#include "C3dsParser.h"
#include "chunks.def"
#include "sdl.h"


C3dsParser foo("cube.3ds");
unsigned int vertCount;
unsigned int faceCount;
vector3* vertices = NULL;
face*    faces = NULL;
SDL_Event e;
bool quit = false;
GLuint disp;

void generateObject();
void readFile();
void drawObject();
static void handle_key_down(SDL_keysym* keysym);

int main(){
	unsigned int tick = 0;
	initSDL();

	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90,1.6,0.01,5);

	glMatrixMode(GL_MODELVIEW);

	readFile();
	generateObject();

	while ( ! quit ) {
		while ( SDL_PollEvent( &e ) ){
			switch ( e.type ){
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					handle_key_down( &e.key.keysym);
					break;
			}
		}
		drawObject();
	}

	//if ( faces != NULL ) delete faces;
	//if ( faces != NULL ) delete vertices;
	
	quitSDL();
	return 0;
}

void handle_key_down( SDL_keysym* keysym ){
	switch( keysym->sym ) {
		case SDLK_ESCAPE:
			quit = true;
			break;
	}
}

void drawObject(){
	glLoadIdentity();
	glTranslatef(0,0,1.5f);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 0, vertices);
	//glDrawElements(GL_TRIANGLES, faceCount, GL_UNSIGNED_SHORT, faces);
	//glDisableClientState(GL_VERTEX_ARRAY);
	glColor3f(1,1,1);
	glBegin(GL_QUADS);
		glVertex3i(1,0,0);
		glVertex3i(1,1,0);
		glVertex3i(0,1,0);
		glVertex3i(0,0,0);
	glEnd();
}

void generateObject(){
}

void readFile(){
	while ( !foo.eof() ){
		foo.enterChunk();
		switch ( foo.getChunkId() ){
			case MAIN_CHUNK:
				printf("Found main identifier %04x, length is %u bytes\n",foo.getChunkId(), foo.getChunkLength());
				break;
			case MAIN_VERSION:
				printf("3DS version: %u\n", foo.extractValue<unsigned short>()  );
				foo.skipChunk();
				break;
			case EDITOR_CHUNK:
				printf("Found editor chunk length %u\n",foo.getChunkId(), foo.getChunkLength());
				break;
		   	case OBJECT_BLOCK:
		      	     	printf("Object block, name %s\n", foo.extractStrData());
		   		break;
		   	case TRIANGULAR_MESH:
		   		printf("   Triangular mesh\n");
		         	break;
		    	case VERTICES_LIST:
				vertCount = foo.extractCount();
				printf(" found %u vertices\n", vertCount);
				vertices = foo.extractArray<vector3>(vertCount,sizeof(vector3));
		         	foo.skipChunk();
		         	break;
			case FACES_LIST:
				faceCount = foo.extractCount();
				printf(" found %u faces\n", faceCount);
				faces = foo.extractArray<face>(faceCount, 2);
		         	foo.skipChunk();
		         	break;
				
			case MATERIAL_BLOCK:
				printf("Material block length %u\n", foo.getChunkLength());
				break;

			case 0xA000:
				printf("Material name: %s\n", foo.extractStrData());
				foo.skipChunk();
				break;

			default:
				printf("Unkown chunk %04x of length %u\n", foo.getChunkId(), foo.getChunkLength());
				foo.skipChunk();
				break;
		}
	}
}
