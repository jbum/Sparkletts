
#include "Scope.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <math.h>
#include <time.h>
#include "Sprites.h"

static GLuint	blurTexture;
bool		bCanBlur = false;
bool		bNewScope = false;
bool		isMorphing = false;
GLfloat	morphStart, morphDuration, morphStartAngle, morphEndAngle;
GLint		biglistID;

#ifdef _DEBUG
FILE	*errFile;
void LogError(char *msg,...)
{
	char tbuf[128];
	va_list args;
	va_start(args,msg);
	vsprintf(tbuf,msg,args);
	va_end(args);

	if (errFile == NULL) {
		errFile = fopen("spark_log.txt","w");
		fprintf(errFile,"\nBegin Log\n\n");
	}
	fprintf(errFile,"%s", tbuf);
}
#else
void LogError(char *msg,...)
{
}
#endif



/*
FILE *logfile;
void outlog(char *str,...)
{
	char	tbuf[256];
	va_list args;
	va_start(args,str);
	vsprintf(tbuf,str,args);
	va_end(args);

	if (logfile == NULL)
		logfile = fopen("Escher.log","w");
	fputs(tbuf, logfile);
}
*/

ScopeApp	*gScopeApp;

void ScopeApp::randomizeMirrorSystem(bool wantsMorph)
{
	int	r;
	D LogError("Randomizing Mirror System\n");
	if (RandomI(4) == 0 && curNbrReflections < 24)	{	// large sizes 1/4 of the time
		r = RandomI(22);
		curNbrReflections = (float) (10+r*4);
	}
	else {	// narrow sizes
		r = RandomI(8);
		curNbrReflections = (float) (10+r*2);
	}
	m_morphStartAngle = curMirrorAngle;
	curMirrorAngle = 360.0f / curNbrReflections;
	if (wantsMorph)
	{
		m_morphEndAngle = curMirrorAngle;
		m_morphStartTime = m_fTime;
		m_morphLength = 2.0f;
		b_IsMorphing = true;
	}
}

void ScopeApp::SetupMirrorSystem(GLfloat angle)
{
		MirrorSystem	*ms = &gMirrorSystems;
		curMirrorAngle = angle;
		curNbrReflections = 360 / angle;
		D LogError("nbrReflectionsF = %f\n", curNbrReflections);
		ms->fMirrorAngle = angle;
		ms->nbrReflections = (int) (360.0f / angle);
		float d = 360 - (ms->nbrReflections*angle);
		if (d > .9 || curNbrReflections - (int) curNbrReflections > 0.001)
			ms->nbrReflections++;

//		outlog("Angle: %f Reflections %d\n", angle, ms->nbrReflections);

		SetVector3(&ms->mirror1, 0.0f, 0.0f, 0.0f);
		SetVector3(&ms->mirror2, cosf(270*kToRadians)*kMirrorRad,
															sinf(270*kToRadians)*kMirrorRad,
															0.0f);
		SetVector3(&ms->mirror3, cosf((270+ms->fMirrorAngle)*kToRadians)*kMirrorRad,
															sinf((270+ms->fMirrorAngle)*kToRadians)*kMirrorRad,
															0.0f);
		K_VECTOR3	delta;
		Vec3Subtract(&delta, &ms->mirror2, &ms->mirror3);
		ms->apertureLength = Vec3Length(&delta);
		ms->mirrorPlane1[0] = 1.0f;
		ms->mirrorPlane1[1] = 0.0f;
		ms->mirrorPlane1[2] = 0.0f;
		ms->mirrorPlane1[3] = 0.0f;

		ms->mirrorPlane2[0] = cos((270+ms->fMirrorAngle-90)*kToRadians);
		ms->mirrorPlane2[1] = sin((270+ms->fMirrorAngle-90)*kToRadians);
		ms->mirrorPlane2[2] = 0.0f;
		ms->mirrorPlane2[3] = 0.0f; // distance of mirror from origin

	float	partial = curNbrReflections - (int) curNbrReflections;
	if (partial > 0.001)
	{
		ms->lastIsPartial = true;
		ms->fPartialAngle = partial*angle;
		if ((ms->nbrReflections & 1) == 0)
		{
			// If even # of reflections, than the partial is non-reversed
			ms->partialPlane1[0] = ms->mirrorPlane1[0];
			ms->partialPlane1[1] = ms->mirrorPlane1[1];
			ms->partialPlane1[2] = ms->mirrorPlane1[2];
			ms->partialPlane1[3] = ms->mirrorPlane1[3];
			ms->partialPlane2[0] = cos((270+ms->fPartialAngle-90)*kToRadians);
			ms->partialPlane2[1] = sin((270+ms->fPartialAngle-90)*kToRadians);
			ms->partialPlane2[2] = 0.0f;
			ms->partialPlane2[3] = 0.0f; // distance of mirror from origin
		}
		else {
			// The partial is reversed
			float	iAngle = angle - ms->fPartialAngle;
			ms->partialPlane1[0] = cos((270+iAngle+90)*kToRadians);	// !!!!!
			ms->partialPlane1[1] = sin((270+iAngle+90)*kToRadians);
			ms->partialPlane1[2] = 0;
			ms->partialPlane1[3] = 0;
			
			ms->partialPlane2[0] = ms->mirrorPlane2[0];
			ms->partialPlane2[1] = ms->mirrorPlane2[1];
			ms->partialPlane2[2] = ms->mirrorPlane2[2];
			ms->partialPlane2[3] = ms->mirrorPlane2[3];
		}
	}
	else
		ms->lastIsPartial = false;

}

//  used by standalone app only
void ScopeApp::initGL(int width, int height)
{
	resizeGL(width, height);
}

void ScopeApp::resizeGL(int width, int height)
{
	this->width = width;
	this->height = height;
	glViewport(0,0,width,height);						// Reset The Current Viewport
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();												// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(90.0f,(GLfloat)width/(GLfloat)height,1.0f,20.0f);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
}

ScopeApp::ScopeApp(int width, int height, int scopeFlags)
{
	gScopeApp = this;
	b_IsMorphing = false;
	b_IsFullscreen = false;
	srand(time(NULL) % 256);
	this->width = width;
	this->height = height;
#ifdef STANDALONE
	this->m_desiredFPS = 24;	// was 30
#else
	this->m_desiredFPS = 24;	// was 24
#endif
	this->m_sleepAdjust = 1000*2/(m_desiredFPS+3);
	b_inhibitMirrors = false;
#ifdef STANDALONE
	b_inhibitShine = false;
#else
	b_inhibitShine = false;
#endif
	curNbrReflections = 10;
	curMirrorAngle = 360.0f / curNbrReflections;
	D LogError("Init: CurMirrorAngle = %f\n", curMirrorAngle);
	SetupMirrorSystem(curMirrorAngle);
	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();												// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(90.0f,(GLfloat)width/(GLfloat)height,1.0f,20.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix

	biglistID = glGenLists(1);


	b_UseMirrors = true;
	b_UseDepthBuffer = true;
	m_fFramesPerSec = 10.0f;

	InitTimer();

	
	itsObjects = new SpriteList(this);
	itsObjects->init();
	if (b_UseDepthBuffer) {
		glClearDepth(1.0f);									// Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
		glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	}
	fLastScopeStart= 0.0f;

	const char *extStr = (const char *) glGetString(GL_EXTENSIONS);

	bCanBlur = (strstr(extStr,"GL_EXT_compiled_vertex_array") != NULL) &&
						 (strstr(extStr,"GL_ARB_multitexture") != NULL);

	if (bCanBlur)
	{
		unsigned int* data;											// Stored Data

		// Create Storage Space For Texture Data (128x128x4)
		data = (unsigned int*)new GLuint[((128 * 128)* 4 * sizeof(unsigned int))];
		memset(data,0,((128 * 128)* 4 * sizeof(unsigned int)));	// Clear Storage Memory

		glGenTextures(1, &blurTexture);											// Create 1 Texture
		glBindTexture(GL_TEXTURE_2D, blurTexture);					// Bind The Texture
		glTexImage2D(GL_TEXTURE_2D, 0, 4, 128, 128, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, data);									// Build Texture Using Information In data
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		delete [] data;																			// Release data
	}

}

ScopeApp::~ScopeApp()
{
	glDeleteLists(biglistID, 1);
	if (itsObjects)
		delete itsObjects;
}

void ScopeApp::ToggleMirrors()
{
	b_inhibitMirrors = !b_inhibitMirrors;
}

void ScopeApp::ToggleShine()
{
	b_inhibitShine = !b_inhibitShine;
}


int ScopeApp::display(int scopeFlags)
{
	static int firstPass = 2;
	bool   swapBuffer = !(itsObjects->wantsTrails()  && !firstPass && (!itsObjects->wantsRadialBlur() || !bCanBlur || b_inhibitShine));

	this->b_UseMirrors = (scopeFlags & F_UseMirrors)? true : false;
	if (!itsObjects->wantsMirrors() || b_inhibitMirrors)
		this->b_UseMirrors = false;

	if (swapBuffer)
	{
		glDrawBuffer(GL_BACK);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else {
		glDrawBuffer(GL_FRONT);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	if (firstPass)
		firstPass--;

	glLoadIdentity();

	gluLookAt( 0.0f, 0.0f,  5.5f , // eye pt
						 0.0f, 0.0f,  0.0f,  // target (center)
						 0.0f, 1.0f,  0.0f); // up vector

	UpdateFrame();

	// !! perform morphing here...
	// !! Setup Morphing Here... (if morphing, use tempMS)

#define EASE(x)					((x)*(x)*(3 - 2*(x)))
	if (b_IsMorphing) {
		D LogError("Morphing\n");
		float r = (m_fTime - m_morphStartTime) / m_morphLength;
		if (r >= 1.0f) {
			curMirrorAngle = m_morphEndAngle;
			b_IsMorphing = false;
		}
		else {
			r = EASE(r);
			curMirrorAngle = m_morphStartAngle + (m_morphEndAngle-m_morphStartAngle)*r;
		}
	}
		
	D LogError("angle = %f\n", curMirrorAngle);
	SetupMirrorSystem(curMirrorAngle);

	RecordSystem();
	if (itsObjects->wantsRadialBlur() && bCanBlur && !b_inhibitShine)
		RenderToTexture();
	// glEnable(GL_BLEND);
	RenderSystem(true);
	// glDisable(GL_BLEND);
	if (itsObjects->wantsRadialBlur() && bCanBlur && !b_inhibitShine)
		DrawBlur(12,.02f);

  glFlush();

	return swapBuffer;
}

void ScopeApp::UpdateFrame()
{
  UpdateTimer();
	
	itsObjects->updateFrame();
}

void ScopeApp::RenderToTexture()											// Renders To A Texture
{
	glViewport(0,0,128,128);									// Set Our Viewport (Match Texture Size)

	RenderSystem(false);												// Render The Helix

	glBindTexture(GL_TEXTURE_2D,blurTexture);					// Bind To The Blur Texture

	// Copy Our ViewPort To The Blur Texture (From 0,0 To 128,128... No Border)
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 0, 0, 128, 128, 0);

	// glClearColor(0.0f, 0.0f, 0.5f, 0.5);						// Set The Clear Color To Medium Blue
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			// Clear The Screen And Depth Buffer

	glViewport(0 , 0, width , height);									// Set Viewport (0,0 to 640x480)
}

void ScopeApp::DrawBlur(int times, float inc)								// Draw The Blurred Image
{
	float spost = 0.0f;											// Starting Texture Coordinate Offset
	float alphainc = 0.9f / times;					// Fade Speed For Alpha Blending
	float alpha = 0.075f;											// Starting Alpha Value

	// Disable AutoTexture Coordinates
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	glEnable(GL_TEXTURE_2D);								// Enable 2D Texture Mapping
	glDisable(GL_DEPTH_TEST);								// Disable Depth Testing
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);				// Set Blending Mode
	glEnable(GL_BLEND);											// Enable Blending
	glBindTexture(GL_TEXTURE_2D,blurTexture);					// Bind To The Blur Texture
	ViewOrtho();														// Switch To An Ortho View

	alphainc = alpha / times;									// alphainc=0.2f / Times To Render Blur

	glBegin(GL_QUADS);											// Begin Drawing Quads
		for (int num = 0;num < times;num++)						// Number Of Times To Render Blur
		{
			glColor4f(1.0f, 1.0f, 1.0f, alpha);					// Set The Alpha Value (Starts At 0.2)
			glTexCoord2f(0+spost,1-spost);						// Texture Coordinate	( 0, 1 )
			glVertex2f(0,0);									// First Vertex		(   0,   0 )

			glTexCoord2f(0+spost,0+spost);						// Texture Coordinate	( 0, 0 )
			glVertex2f(0,480);									// Second Vertex	(   0, 480 )

			glTexCoord2f(1-spost,0+spost);						// Texture Coordinate	( 1, 0 )
			glVertex2f(640,480);								// Third Vertex		( 640, 480 )

			glTexCoord2f(1-spost,1-spost);						// Texture Coordinate	( 1, 1 )
			glVertex2f(640,0);									// Fourth Vertex	( 640,   0 )

			spost += inc;										// Gradually Increase spost (Zooming Closer To Texture Center)
			alpha = alpha - alphainc;							// Gradually Decrease alpha (Gradually Fading Image Out)
		}
	glEnd();													// Done Drawing Quads

	ViewPerspective();											// Switch To A Perspective View

	glEnable(GL_DEPTH_TEST);									// Enable Depth Testing
	glDisable(GL_TEXTURE_2D);									// Disable 2D Texture Mapping
	glDisable(GL_BLEND);										// Disable Blending
	glBindTexture(GL_TEXTURE_2D,0);								// Unbind The Blur Texture
}


void ScopeApp::RecordSystem()
{
#define kUseBigList	0
#if (kUseBigList)
	glNewList(biglistID, GL_COMPILE);
			glClipPlane (GL_CLIP_PLANE0, ms->mirrorPlane1);
			glClipPlane (GL_CLIP_PLANE1, ms->mirrorPlane2);
			// glClipPlane (GL_CLIP_PLANE2, ms->mirrorPlane3);
			itsObjects->draw();
	glEndList();
#endif
}

void ScopeApp::RenderSystem(bool drawBG)
{
	MirrorSystem *ms = &gMirrorSystems;
	glPushMatrix();
	itsObjects->preRender(drawBG);
	
	// Draw Mirror
/*
	if (!b_UseMirrors)
	{
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);							// Enable Lighting
	  glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_LINE_LOOP);
			glVertex2f(ms->mirror1.x,ms->mirror1.y);
			glVertex2f(ms->mirror2.x,ms->mirror2.y);
			glVertex2f(ms->mirror3.x,ms->mirror3.y);
		glEnd();
		glPopAttrib();							// Enable Lighting
	}
*/
	if (itsObjects->wantsClipping() && b_UseMirrors)
	{
		glEnable (GL_CLIP_PLANE0);
		glEnable (GL_CLIP_PLANE1);
		// glEnable (GL_CLIP_PLANE2);
	}

	// Was Recording System Here...

	// glPopAttrib();
	// was setting up clip plane here...
		bool lastOne = false;
		// int	n = 1 + ((int) (m_fTime*2) % ms->nbrReflections);
		int	n = ms->nbrReflections;
		for (int i = 0; i < n && !lastOne; i += 2)
		{
			// rotate scene
			glPushMatrix();
			// glTranslatef(0,0,-.001f*i);
			glRotatef(i * ms->fMirrorAngle, 0, 0, 1);

			glFrontFace(GL_CCW);
			if (i+1 < n)
			{
	#if (kUseBigList)
				 glCallList(biglistID);
	#else
				if (i+1 == ms->nbrReflections - 1 && ms->lastIsPartial)
				{
					glClipPlane (GL_CLIP_PLANE0, ms->partialPlane1);
					glClipPlane (GL_CLIP_PLANE1, ms->partialPlane2);
				}
				else {
					glClipPlane (GL_CLIP_PLANE0, ms->mirrorPlane1);
					glClipPlane (GL_CLIP_PLANE1, ms->mirrorPlane2);
				}
				itsObjects->draw();
	#endif
			}
			if (b_UseMirrors) {
				glScalef(-1,1,1);
				glFrontFace(GL_CW);
#if (kUseBigList)
				 glCallList(biglistID);
#else
					if (i == ms->nbrReflections - 1 && ms->lastIsPartial)
					{
						glClipPlane (GL_CLIP_PLANE0, ms->partialPlane1);
						glClipPlane (GL_CLIP_PLANE1, ms->partialPlane2);
					}
					else {
						glClipPlane (GL_CLIP_PLANE0, ms->mirrorPlane1);
						glClipPlane (GL_CLIP_PLANE1, ms->mirrorPlane2);
					}
					itsObjects->draw();
#endif
			}
			else {
				lastOne = true;
			}
			glPopMatrix();

		}

    // return S_OK;
		// Save the world matrix so it can be restored

    glDisable (GL_CLIP_PLANE0);
    glDisable (GL_CLIP_PLANE1);
    glDisable (GL_CLIP_PLANE2);

		// glDisable(GL_DEPTH_TEST);
		glFrontFace(GL_CCW);
		glPopMatrix();

		itsObjects->postRender();

		
		
/*
#if (SHOW_STATS)
    m_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), m_strFrameStats );
#endif
    return S_OK;

	*/

}

#define HAVE_GETTIMEODAY 1

#ifdef HAVE_GETTIMEODAY
static double startClock, lastClock;
#else
static long startClock, lastClock;
#endif
static bool gInitedStats;

// float gFrameRate, gTime;

void ScopeApp::InitTimer()
{
    if (!gInitedStats)
    {
	gInitedStats = true;
#ifdef HAVE_GETTIMEODAY
        struct timeval ts;
        gettimeofday(&ts, NULL);
        startClock = ts.tv_sec + ts.tv_usec/1000000.0;
        lastClock = startClock;
#else
	startClock = timeGetTime();
	lastClock = timeGetTime();
#endif
	m_iFrames = 0;
	m_fFramesPerSec = 50;
	m_fTime = 0;
	m_fLastKey = 0;
    }
}

void ScopeApp::UpdateTimer()
{
#ifdef HAVE_GETTIMEODAY
    double  rightNow;
    struct timeval ts;
    gettimeofday(&ts, NULL);
    rightNow = ts.tv_sec + ts.tv_usec/1000000.0;
    m_fTime = (rightNow - startClock);
    float dt = (rightNow - lastClock);
#else  
    long  rightNow;
    rightNow = timeGetTime();
    m_fTime = (rightNow - startClock)/1000.0f;
    float dt = (rightNow - lastClock)/1000.0f;
#endif    

    if (dt >= 1.0f) 
    {
			int lastFPS = (int) m_fFramesPerSec;
      m_fFramesPerSec = m_iFrames / dt;
      m_iFrames = 0;
      lastClock = rightNow;
      printf("Framerate = %5.2f\n", m_fFramesPerSec);
			if (lastFPS > 0)
				m_sleepAdjust += (1000/m_desiredFPS - 1000/lastFPS) / 2;		
			if (m_sleepAdjust < 1)
				m_sleepAdjust = 1;
			else if (m_sleepAdjust > 1000)
				m_sleepAdjust = 1000;
		}
		else
			m_iFrames++;
}

bool ScopeApp::HandleSpecialKey(int x)
{
	itsObjects->handleSpecialKey(x);
	return true;
}

void ScopeApp::ToggleFullscreen()
{
	static int sWidth, sHeight, sPosX, sPosY;

	b_IsFullscreen = ! b_IsFullscreen;
	
	if (b_IsFullscreen)
	{
		sPosX = glutGet(GLUT_WINDOW_X);
		sPosY = glutGet(GLUT_WINDOW_Y);
		sWidth = glutGet(GLUT_WINDOW_WIDTH);
		sHeight = glutGet(GLUT_WINDOW_HEIGHT);
		glutFullScreen();
	}
	else {
		glutReshapeWindow(sWidth, sHeight);
		glutPositionWindow(sPosX, sPosY);
	}
}

bool ScopeApp::HandleKey(int x)
{
	m_fLastKey = m_fTime;
	switch (tolower(x)) {
	case 'm':
		ToggleMirrors();
		break;
	case 's':
		ToggleShine();
		break;
	case 'f':
		ToggleFullscreen();
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
		if (x == '0')
			x = '0'+10;
		curNbrReflections = (float) (10+(x-'1')*4);
		m_morphStartAngle = curMirrorAngle;
		curMirrorAngle = 360.0f / curNbrReflections;
		m_morphEndAngle = curMirrorAngle;
		m_morphStartTime = m_fTime;
		m_morphLength = 2.0f;
		b_IsMorphing = true;
		itsObjects->bIsMagnetized = false;
		break;
#define kOpenIncrement	.1f
#define kDegIncrement		.14285f
	case ' ':			// Change major scope mode...
        itsObjects->handleSpecialKey(GLUT_KEY_DOWN);
        break;
	default:
		itsObjects->handleKey(x);
		return false;
	}
	return true;
}
