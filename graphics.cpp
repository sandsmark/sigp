#include "graphics.h"
#include "C3dsParser.h"
#include "chunks.def"
#include <GL/glut.h>
#include <assert.h>
#include <iostream>

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
    string currentMesh, currentMaterial, mat;
    uint16_t numFaces;

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
                currentMesh = me->m_parser->extractStrData();
		      	printf("Object block, name %s\n", currentMesh.c_str());
		   		break;

		   	case TRIANGULAR_MESH:
		   		printf("   Triangular mesh\n");
		        break;

		    case VERTICES_LIST:
                assert(!currentMesh.empty());

				me->m_vertCount[currentMesh] = me->m_parser->extractCount();
				printf(" found %u me->m_vertices\n", me->m_vertCount[currentMesh]);
				me->m_vertices[currentMesh] = me->m_parser->extractArray<vector3>(me->m_vertCount[currentMesh]);
		       	break;

			case FACES_LIST:
				me->m_faceCount[currentMesh] = me->m_parser->extractCount();
				printf(" found %u me->m_faces\n", me->m_faceCount[currentMesh]);
                printf("Offset: %x\n", me->m_parser->getChunkOffset());
				me->m_faces[currentMesh] = me->m_parser->extractArray<face>(me->m_faceCount[currentMesh], 2);
		       	break;
				
			case MATERIAL_BLOCK:
				printf("Material block length %u\n", me->m_parser->getChunkLength());
				break;

			case MATERIAL_NAME:
                currentMaterial = me->m_parser->extractStrData();
				printf("Material name: %s\n", currentMaterial.c_str());
				me->m_parser->skipChunk();
				break;

            case DIFFUSE_COLOR:
                break;

            case RGB1:
                m_colors[currentMaterial] = me->m_parser->extractValue<color>();
                printf("Diffuse color: r:%i, g:%i, b:%i\n", m_colors[currentMaterial]);
				me->m_parser->skipChunk();
                break;

            case FACES_MATERIAL:
                mat = me->m_parser->extractStrData();
                numFaces = me->m_parser->extractValue<uint16_t>();
                me->m_faceMaterial[currentMesh] = new color[numFaces];
                printf("Colored faces: %hu\n", numFaces);
                for (uint16_t i=0; i<numFaces; i++) {
                    me->m_faceMaterial[currentMesh][i] = m_colors[mat];
                }
                break;

			default:
				printf("Unkown chunk %04x of length %u\n", me->m_parser->getChunkId(), me->m_parser->getChunkLength());
				me->m_parser->skipChunk();
				break;

            }
    }
    printf("Finished parsing!\n");
    vector3 *vertex;
    face *cface;
    return;

    for (map<string, vector3*>::iterator it = m_vertices.begin(); it != m_vertices.end(); it++) {
        vertex = m_vertices[(*it).first];
        cface = m_faces[(*it).first];
        printf("Mesh:%s\n", (*it).first.c_str());
        for ( int i=0; i < 12; i++){
            printf(" Coordinates %f %f %f\n", vertex[i].x, vertex[i].y, vertex[i].z);
            printf(" Face %u:  %u %u %u\n", i, cface[i].a, cface[i].b, cface[i].c);
        }
    }
}

void Graphics::drawObject(){
    glEnableClientState(GL_VERTEX_ARRAY | GL_COLOR_ARRAY);
    string meshName, colorName; 
    for (map<string, vector3*>::iterator it = m_vertices.begin(); it != m_vertices.end(); it++) {
        meshName = (*it).first;

        glColorPointer(3, GL_UNSIGNED_BYTE, 0, me->m_faceMaterial[meshName]);
        glVertexPointer(3, GL_FLOAT, 0, me->m_vertices[meshName]);
        glDrawElements(GL_TRIANGLES, me->m_faceCount[meshName]*3, GL_UNSIGNED_SHORT, me->m_faces[meshName]);
    }
    glDisableClientState(GL_VERTEX_ARRAY);

    GLenum errCode;
    const GLubyte *errStr;
    if ((errCode = glGetError()) != GL_NO_ERROR) {
        errStr = gluErrorString(errCode);
        cerr << "OpenGL ERROR: " << (char*)errStr << endl;
    }
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
