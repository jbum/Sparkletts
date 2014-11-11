/*
 *  Doublebuffer.cpp
 *  OpenGL Full Screen
 *
 *  Jim Bumgardner / Eugene Hourany
 *  Copyright (c) 2003 KrazyDad.com All rights reserved.
 *
 */
// TODO: Work on Windows Screensaver options.
//

#include "Scope.h"
#include "Sprites.h"

#include <math.h>
#include <ctype.h>

extern ScopeApp *gScopeApp;
int		 gWidth=800, gHeight=600;
bool	 isScreensaver=false;

void displayC()
{
    if (gScopeApp->display(0x10001))  
        glutSwapBuffers();
}



void reshapeC(int w, int h)
{
	int x = w % 4;
	gWidth = w;
	gHeight = h;
	switch (x){
		case 1: w += 3; gScopeApp->resizeGL(w, h); break;
		case 2: w += 2; gScopeApp->resizeGL(w, h); break;
		case 3: w += 1; gScopeApp->resizeGL(w, h); break;
		case 0: gScopeApp->resizeGL(w, h); break;
	}
}

void myMouseMove(int x, int y)
{
	gScopeApp->itsObjects->handleMouseMove(x/(float) gWidth, y/(float) gHeight);
}

void myMouseClick(int button, int state, int x, int y)
{
}

void keyC(unsigned char key, int x, int y)
{
	switch (key){
		case 3:
		case 27:
						exit(0);
						break;
		case 'p':
		case 'P': gScopeApp->TakeSnapshot();	break;
		default:	gScopeApp->HandleKey(key);	break;
	}
}

void specialKeyC(int key, int x, int y)
{
	gScopeApp->HandleSpecialKey(key);
}

void menuC(int itemNbr)
{
	switch (itemNbr) {
	case 1:	gScopeApp->ToggleFullscreen();	break;
	case 2:	gScopeApp->ToggleMirrors();			break;
	case 3:	gScopeApp->ToggleShine();				break;
	case 4:	exit(0);													break;
	}
}

// This limits framerate to 30, which significantly uses GPU usage/fan
void myIdle(int arg)
{
    glutPostRedisplay();
    glutTimerFunc(33, myIdle, 0);
}
//void idleHandler()
//{
//    glutPostRedisplay();
//}

int main(int argc, char **argv )
{
		for (int i = 1; i < argc; ++i) {
			if (argv[i][0] == '-') {
				for (char *p = argv[i]; *p; ++p) {
					switch (tolower(*p)) {
					case 's':
							isScreensaver = true;
							break;
					}
				}
			}
		}
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(gWidth, gHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    gScopeApp = new ScopeApp(800, 600, 1);
    
    glutDisplayFunc(displayC);
    glutReshapeFunc(reshapeC);
		glutPassiveMotionFunc(myMouseMove);
    glutMouseFunc(myMouseClick);
		glutKeyboardFunc(keyC);
		glutSpecialFunc(specialKeyC);

    glutTimerFunc(33, myIdle, 0);
    // glutIdleFunc(idleHandler);


		glutCreateMenu(menuC);
		glutAddMenuEntry("Fullscreen", 1);
		glutAddMenuEntry("Mirrors", 2);
		glutAddMenuEntry("Shine", 3);
		glutAddMenuEntry("Quit", 4);
		glutAttachMenu(GLUT_RIGHT_BUTTON);

		if (isScreensaver)
			gScopeApp->ToggleFullscreen();

    glutMainLoop();
    delete gScopeApp;
    return 0;
}