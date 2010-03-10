#include "graphics.h"
#include "C3dsParser.h"
#include "chunks.def"
#include <GL/glut.h>

Graphics *Graphics::me;


Graphics::Graphics(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(640, 480);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutCreateWindow("iTK - DG - FK");
    
    glClearColor(0, 0, 0, 0); 
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_TEXTURE_2D);  
    
    glutDisplayFunc(&display);
    glutIdleFunc(&display);
    glutReshapeFunc(&reshape);
    glEnable(GL_DEPTH_TEST);

    me = this; // having fun already
    me->m_parser = new C3dsParser(FILENAME);
    me->compileObject();
}

void Graphics::display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(-15,  10, 10, // Eye
               0, 0, 0,  // Focus
               0.0,  1.0,  0.0); // Up
    
    //glTranslatef(0.0,0.0,-2.0);
    
    //drawScene();
    me->drawObject();
    glutSwapBuffers();

}

void Graphics::compileObject(){
    while ( !me->m_parser->eof() ) {
		me->m_parser->enterChunk();
		switch ( me->m_parser->getChunkId() ){
			case MAIN_CHUNK:
				printf("Found main identifier %04x, length is %u bytes\n",me->m_parser->getChunkId(), me->m_parser->getChunkLength());
				break;

			case MAIN_VERSION:
				printf("3DS version: %u\n", me->m_parser->extractValue<unsigned short>()  );
				me->m_parser->skipChunk();
				break;

			case EDITOR_CHUNK:
				printf("Found editor chunk length %u\n",me->m_parser->getChunkId(), me->m_parser->getChunkLength());
				break;

		   	case OBJECT_BLOCK:
		      	printf("Object block, name %s\n", me->m_parser->extractStrData());
		   		break;

		   	case TRIANGULAR_MESH:
		   		printf("   Triangular mesh\n");
		        break;

		    case VERTICES_LIST:
				me->m_vertCount = me->m_parser->extractCount();
				printf(" found %u me->m_vertices\n", me->m_vertCount);
				me->m_vertices = me->m_parser->extractArray<vector3>(me->m_vertCount);
		        //me->m_parser->skipChunk();
		       	break;

			case FACES_LIST:
				me->m_faceCount = me->m_parser->extractCount();
				printf(" found %u me->m_faces\n", me->m_faceCount);
                printf("Offset: %x\n", me->m_parser->getChunkOffset());
				me->m_faces = me->m_parser->extractArray<face>(me->m_faceCount, 2);
		       	//me->m_parser->skipChunk();
		       	break;
				
			case MATERIAL_BLOCK:
				printf("Material block length %u\n", me->m_parser->getChunkLength());
				break;

			case 0xA000:
				printf("Material name: %s\n", me->m_parser->extractStrData());
				me->m_parser->skipChunk();
				break;

			default:
				printf("Unkown chunk %04x of length %u\n", me->m_parser->getChunkId(), me->m_parser->getChunkLength());
				me->m_parser->skipChunk();
				break;

            }
    }
    printf("Object compiled!\n");
    for ( int i=0; i < 12; i++){
        printf("Coordinates %f %f %f\n", me->m_vertices[i].x, me->m_vertices[i].y, me->m_vertices[i].z);
        printf("Face %u:  %u %u %u\n", i, me->m_faces[i].a, me->m_faces[i].b, me->m_faces[i].c);
    }



}

void Graphics::drawObject(){
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3,GL_FLOAT, 0, me->m_vertices);
    glDrawElements(GL_TRIANGLES, me->m_faceCount*3, GL_UNSIGNED_SHORT, me->m_faces);
    glDisableClientState(GL_VERTEX_ARRAY);
}



void Graphics::reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f,(GLfloat)w/(GLfloat)h,0.1f,100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}


void Graphics::run() {
  glutMainLoop();
}
