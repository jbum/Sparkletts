#ifndef _H_Scope
#define _H_Scope	1

#include "ScopeLocal.h"

#include <math.h>
#include <stdlib.h>
#include "KS_VECTOR.h"

#define kFirstCellListID	2 // First Useable Displaylist by a Cell

// enum { CT_Photos, kNbrCellTypes };
#define F_UseMirrors	0x10000

#define kMirrorRad				4.0f
#define kMaxReflections		24
#define kBlockLength			15.0f

inline float rnd() { return (((FLOAT)rand())/RAND_MAX); }
inline int RandomI(int r) { return (int)(rnd()*r); }
inline float ease(float x) { return (x)*(x) * (3 - 2*(x)); }
// inline long FtoDW( FLOAT f ) { return *((DWORD*)&f); }


enum { MS_10, MS_12, MS_14, MS_16, MS_18, MS_20, MS_22, MS_24, kNbrMirrorSystems };

typedef struct {
	float	fMirrorAngle;
	int		nbrReflections;
	K_VECTOR3	mirror1, mirror2, mirror3;
	float	apertureLength;
	GLdouble	mirrorPlane1[4];	// for clipping
	GLdouble	mirrorPlane2[4];

	// partial stuff 
	GLdouble	partialPlane1[4];	// for clipping
	GLdouble	partialPlane2[4];	// for clipping
	bool	lastIsPartial;
	float	fPartialAngle;
} MirrorSystem;

extern MirrorSystem gMirrorSystems[kNbrMirrorSystems];

class SpriteList;

class ScopeApp
{
public:
	MirrorSystem gMirrorSystems;
	SpriteList	*itsObjects;
	int				width, height;
	int				curMIdx;
	bool			b_UseMirrors,  b_UseDepthBuffer;
	float			m_fFramesPerSec, fLastScopeStart, elapsedTime;
	float			m_fTime;
	float			m_fLastKey;

	float			m_morphStartTime, m_morphLength;
	float			m_morphStartAngle, m_morphEndAngle;
	bool			b_IsMorphing;
	float			m_fStartFrameTime, m_fEndFrameTime;
	int				m_iFrames;
	int				m_desiredFPS, m_sleepAdjust;
	bool			b_inhibitMirrors;
	bool			b_inhibitShine;
	bool		  b_IsFullscreen;
	float			curMirrorAngle, curNbrReflections;

	ScopeApp(int width, int height, int scopeFlags);
	~ScopeApp();
	void SetupMirrorSystem(GLfloat angle);
	void randomizeMirrorSystem(bool wantsMorph);
	void initGL(int width, int height);
	void resizeGL(int width, int height);
	void OneTimeSceneInit();
	int  display(int scopeFlags);
	void UpdateFrame();
	void RenderSystem(bool drawBG);
	void RecordSystem();
	void InitTimer();
	void UpdateTimer();
	void RenderToTexture();
	void DrawBlur(int times, float inc);
	void TakeSnapshot();
	void ToggleMirrors();
	void ToggleFullscreen();
	void ToggleShine();
	bool HandleKey(int x);
	bool HandleSpecialKey(int x);
};

#define K_PI		3.14159f
#define kToRadians	(K_PI/180)

typedef struct												// Create A Structure
{
	GLubyte	*imageData;										// Image Data (Up To 32 Bits)
	GLuint	bpp;											// Image Color Depth In Bits Per Pixel.
	GLuint	width;											// Image Width
	GLuint	height;											// Image Height
	GLuint	texID;											// Texture ID Used To Select A Texture
} TextureImage;												// Structure Name

// Utils
void SetVector3(K_VECTOR3 *v, float x, float y, float z);
void Vec3Subtract(K_VECTOR3 *res, K_VECTOR3 *a, K_VECTOR3 *b);
void Vec3Add(K_VECTOR3 *res, K_VECTOR3 *a, K_VECTOR3 *b);
// K_VECTOR3 * Vec3Multiply(K_VECTOR3 *res, K_VECTOR3 *a, K_VECTOR3 *b);
// K_VECTOR3 * Vec3Multiply(K_VECTOR3 *res, K_VECTOR3 *a, GLfloat b);
GLfloat Vec3Length(K_VECTOR3 *v);
void Vec3Normalize(K_VECTOR3 *v);
GLfloat Vec3Dot(K_VECTOR3 *a, K_VECTOR3 *b);
void Vec3Cross(K_VECTOR3 *dst, K_VECTOR3 *a, K_VECTOR3 *b);

// bool LoadPNG(TextureImage *texture, char *filename);
// bool LoadJPEGwMask(TextureImage *texture, const char *rgbname, const char *maskname);
bool LoadJPEGTexture(TextureImage *texture, const unsigned char *maskname, unsigned int masksize, int mode);
void ViewOrtho();
void ViewPerspective();

void HSVtoRGB(float h, float s, float v, K_VECTOR3 *mat);
float gnoise3d(float x, float y, float z);

typedef struct 
{
		unsigned int pos,size; 
		const char *buf;
} 
MFILE;

MFILE *mopen(const unsigned char *buf,unsigned int size);
void  mclose(MFILE *m);

extern void LogError(const char *msg,...);

#ifdef _DEBUG
#define D	
#else
#define D	if (0)
#endif


#endif
