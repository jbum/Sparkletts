#ifndef _H_ScopeLocal
#define _H_ScopeLocal	1

#ifdef WIN32

	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <regstr.h>
	#include <mmsystem.h>
	#include <stdlib.h>

	#include <gl/gl.h>
	#include <gl/glu.h>
	#include <gl/glut.h>
    
    #define WIN_TIMER // To use Win-specific Queryperformance

#else
  #include <OpenGL/OpenGL.h>
  #include <OpenGL/gl.h>
  #include <OpenGL/glu.h>
  #include <GLUT/glut.h>
  #define HAVE_GETTIMEODAY  1
  #include <sys/time.h>
  
#endif

#endif