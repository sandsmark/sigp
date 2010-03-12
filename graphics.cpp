#include "graphics.h"
#include "C3dsParser.h"
#include "chunks.def"
#include <GL/glut.h>
#include <assert.h>
#include <iostream>

Graphics *Graphics::me;


Graphics::Graphics(int argc, char **argv):
        m_angle(0),
        m_angleSpeed(1) 
{
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
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
}

void Graphics::display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(-5,  5, 5, // Eye
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
    map<string, face*>    faces;
    map<string, vector3*> vertices;
    map<string, int>      vertCount;
    map<string, int>      faceCount;
    map<string, color*>   faceMaterial;
    map<string, color>    colors;

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

				vertCount[currentMesh] = me->m_parser->extractCount();
				printf(" found %u me->m_vertices\n", vertCount[currentMesh]);
				vertices[currentMesh] = me->m_parser->extractArray<vector3>(vertCount[currentMesh]);
		       	break;

			case FACES_LIST:
				faceCount[currentMesh] = me->m_parser->extractCount();
				printf(" found %u me->m_faces\n", faceCount[currentMesh]);
                printf("Offset: %x\n", me->m_parser->getChunkOffset());
				faces[currentMesh] = me->m_parser->extractArray<face>(faceCount[currentMesh], 2);
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
                colors[currentMaterial] = me->m_parser->extractValue<color>();
                printf("Diffuse color: r:%i, g:%i, b:%i\n", colors[currentMaterial]);
				me->m_parser->skipChunk();
                break;

            case FACES_MATERIAL:
                mat = me->m_parser->extractStrData();
                numFaces = me->m_parser->extractValue<uint16_t>();
                faceMaterial[currentMesh] = new color[numFaces];
                printf("Colored faces: %hu\n", numFaces);
                for (uint16_t i=0; i<numFaces; i++) {
                    faceMaterial[currentMesh][i] = colors[mat];
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
    vector3 normal;

    for (map<string, vector3*>::iterator it = vertices.begin(); it != vertices.end(); it++) {
        me->m_callLists[(*it).first] = glGenLists(1);
        glNewList(me->m_callLists[(*it).first], GL_COMPILE);

        vertex = vertices[(*it).first];
        cface = faces[(*it).first];

        for (int i=0; i<faceCount[(*it).first]; i++) {
            glBegin(GL_TRIANGLES);
            normal = vector3::normal(vertex[cface[i].a], vertex[cface[i].b], vertex[cface[i].c]);
            glNormal3f(normal.x, normal.y, normal.z);
            glMaterialiv(GL_FRONT, GL_SPECULAR, (GLint*)&colors[(*it).first]);
            glVertex3fv((GLfloat*)&vertex[cface[i].a]);
            glVertex3fv((GLfloat*)&vertex[cface[i].b]);
            glVertex3fv((GLfloat*)&vertex[cface[i].c]);
            glEnd();
        }

        glEndList();

//        printf("Mesh:%s\n", (*it).first.c_str());
//        for ( int i=0; i < 12; i++){
//            printf(" Coordinates %f %f %f\n", vertex[i].x, vertex[i].y, vertex[i].z);
//            printf(" Face %u:  %u %u %u\n", i, cface[i].a, cface[i].b, cface[i].c);
//        }
    }
}

void Graphics::drawObject(){
    m_angle += m_angleSpeed;
    glRotatef(m_angle, 1, 1, 1);

    m_scale += 0.01;
    if (m_scale > 1) m_scale = 0.5;
    glScalef(m_scale, m_scale, m_scale);

    for (map<string, GLuint>::iterator it = m_callLists.begin(); it != m_callLists.end(); it++) {
        glCallList(m_callLists[(*it).first]);
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
