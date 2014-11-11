
/*
 *	Fix up/down arrows to affect # elements, and size
 *  Add HSV conversion for color params.  Add params
 *  for saturation, hue and lightness ranges.
 */

#include "Scope.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Sprites.h"
#include "scopeData.h"

static SpriteList	*gList;
#define kMaxTextures				8
#define kMaxSprites					60	// was 30 // 0
#define kLargestSprite			0.9f
#define kSmallestSprite			0.2f
#define kCenterZ						-0.5f
#define kMinRoll						720
#define kMaxRoll						3200
#define kSweepTime					20

#define kGravAngle					0
#define kCollisionDamp			0.85f
#define kDamp								0.85f
#define kGravWeight					(0.005f)
#define kSpinDamp						0.9f
#define kSpeed							(0.3f)


bool		bUseBlur = false;

void Sprite::init(int i)
{
	float r = i / (float) kMaxSprites;
	float a = rnd()*2*3.14159f;
	float	d = rnd()*kMirrorRad;
//	float a = r*2*3.14159f;
//	float	d = r*kMirrorRad;
	vPos = KS_VECTOR3(cosf(a)*d, sinf(a)*d, -r*.25f);
	vVel = KS_VECTOR3(0,0,0);
	vFce = KS_VECTOR3(0,0,0);
	fRadius = kSmallestSprite + rnd()*(kLargestSprite - kSmallestSprite);
	cycle = r;

	randomize();
}



void Sprite::randomize()
{
	cycleStart = gList->m_parent->m_fTime - cycle*kSweepTime;
	float	hue,sat,lum;

	flags = 0;
	if (gList->bRegularPolys) {
		fSpinRate = gList->fMaxSpinRate;
		iSpriteType = gList->iSpriteType;
	}
	else {		
		fSpinRate = rnd()*gList->fMaxSpinRate*2;
		iSpriteType = RandomI(gList->nbrSpriteTypes);
	}
	hue = gList->fHueC + (rnd()-.5f)*gList->fHueW;
	sat = gList->fSatC + (rnd()-.5f)*gList->fSatW;
	lum = gList->fLumC + (rnd()-.5f)*gList->fLumW;
	if (hue < 0)
		hue += 1;
	if (sat < 0)
		sat += 1;
	if (lum < 0)
		lum += 1;
	if (hue > 1)
		hue -= 1;
	if (sat > 1)
		sat -= 1;
	if (lum > 1)
		lum -= 1;
	K_VECTOR3	rgb;
	HSVtoRGB(hue,sat,lum,&rgb);
	// iSpriteType = 4+RandomI(4);
	fSpinRate = kMinRoll > fSpinRate? kMinRoll : fSpinRate;

	tx = (iSpriteType*64)/(float) gList->m_PhotoTexture->width;
	ty = 0;
	tw = 64/(float) gList->m_PhotoTexture->width;
	th = 64/(float) gList->m_PhotoTexture->height;


	rC = rgb.x;
	gC = rgb.y;
	bC = rgb.z;
	aC = .5f+rnd()*.5f;


	rCD = rnd() - rC;
	gCD = rnd() - gC;
	bCD = rnd() - bC;
	aCD = rnd() - aC;
	rCD = gCD = bCD = 0;

	roll = rnd()*360;
}

void Sprite::update()
{
	float frameRate = gList->m_parent->m_fFramesPerSec;
	// float apertureWidth = gList->m_parent->gMirrorSystems.apertureLength;

	if (frameRate < .001 || frameRate > 1000)
		frameRate = 30.0f;


	cycle = (gList->m_parent->m_fTime - cycleStart)/kSweepTime;
	
	if (cycle > 1) {
		cycle -= (int) cycle;
		gList->paramTick();
		if (vPos.x+fRadius < 0 || vPos.y-fRadius > 0)
			randomize();
	}
	roll += vVel.length()*100;

}

void Sprite::draw()
{

	glPushMatrix();
	glTranslatef(vPos.x, vPos.y, vPos.z);
	glRotatef(roll, 0, 0, 1);
	glScalef(fRadius, fRadius, 1);

//		glColor4f(1,1,1,1);

	glBegin(GL_QUADS);
		glNormal3f(0, 0, 1);
		float	rc = (cosf(cycle*8*3.1415f)+1)*.5f;
		glColor4f(rC+rCD*rc,gC+gCD*rc,bC+bCD*rc,aC+aCD*rc);
		glTexCoord2f(tx,		ty+th); glVertex3f(-1.0f, -1.0f,  1.0f);	// Point 1 (Front)
//		glColor4f(rC,gC,bC,aC);
		glTexCoord2f(tx+tw, ty+th); glVertex3f( 1.0f, -1.0f,  1.0f);	// Point 2 (Front)
//		glColor4f(rC,gC,bC,aC);
		glTexCoord2f(tx+tw, ty); glVertex3f( 1.0f,  1.0f,  1.0f);	// Point 3 (Front)
		glTexCoord2f(tx,		ty); glVertex3f(-1.0f,  1.0f,  1.0f);	// Point 4 (Front)

		glEnd();

	glPopMatrix();
}

SpriteList::SpriteList(ScopeApp *parent)
{
  D LogError("Initing Sprite List\n");
	m_parent = parent;
	gList = this;
	cList = NULL;
	iCtr = iMaxCtr = 0;
	fMaxSize = kSmallestSprite+rnd()*(kLargestSprite-kSmallestSprite);
	fMaxSpinRate = rnd()*kMaxRoll;
	fHueC = rnd();
	fHueW = rnd();
	fSatC = rnd();
	fSatW = rnd();
	fLumC = .5f+rnd()*.5f;
	fLumW = rnd()*.5f;
	// float center = .5f + rnd(); // center color
	// float rad = rnd()*.5f;			 // color width
	bRegularPolys = RandomI(2) == 0? true : false;
	m_PhotoTexture = NULL;
	nbrSpriteTypes = kMaxTextures;
	iSpriteType = 0;
	aSpin = 0.0f;
	rSpin = aSpin*3.14159f/180;
	targetReflections = 10;
	bIsMagnetized = true;
	vA = 0;
}

SpriteList::~SpriteList()
{
	glDeleteLists(1,1);
	// Delete Textures Here
	if (m_PhotoTexture)
	{
		glDeleteTextures(1, &m_PhotoTexture->texID);
		free(m_PhotoTexture);
	}
	m_PhotoTexture = NULL;
	if (cList)
		delete cList;
}

bool SpriteList::allocateObjects()
{
  D LogError("Allocating Objects\n");
	iSpriteType = RandomI(nbrSpriteTypes);

	m_PhotoTexture = (TextureImage *) malloc(sizeof(TextureImage)*2);
	memset(m_PhotoTexture, 0, sizeof(TextureImage)*2);
	LoadJPEGTexture(&m_PhotoTexture[0], DiscMasks_jpg, DiscMasks_jpg_size, GL_ALPHA);
	LoadJPEGTexture(&m_PhotoTexture[1], KrazyDadCom_jpg, KrazyDadCom_jpg_size, GL_ALPHA);

	glDisable(GL_LIGHTING);							// Enable Lighting
	return true;
}

void SpriteList::Reset()
{
	for (int i = 0; i < kMaxSprites; ++i)
	{
		cList[i].init(i);
	}
}

void SpriteList::init()
{
  D LogError("Init Sprite List\n");
	iSpriteType = RandomI(nbrSpriteTypes);
	cList = new Sprite[kMaxSprites];
	allocateObjects();
	Reset();
	glClearColor(0, 0, 0, 1.0f);
}

void SpriteList::updateFrame()
{
	Sprite *iSpr, *jSpr;
	int			i,j;

#define kMagnetSpeed 0.001f
#define kSpinRate 0.001f

	if (bIsMagnetized)
	{
		if (m_parent->curNbrReflections != targetReflections) 
		{
			float	da = targetReflections - m_parent->curNbrReflections > 0? kMagnetSpeed : -kMagnetSpeed;
			vA += da;
			if ((da > 0 && m_parent->curNbrReflections+vA > targetReflections) ||
				  (da < 0 && m_parent->curNbrReflections+vA < targetReflections))
				m_parent->curNbrReflections = (float) targetReflections;
			else
				m_parent->curNbrReflections += vA;
			m_parent->curMirrorAngle = 360.0f / m_parent->curNbrReflections;
		}
	}
    float speedScale =  24.0 / gList->m_parent->m_fFramesPerSec;
    float gravWeight = kGravWeight * speedScale;
    float mySpeed = kSpeed * speedScale;
	
	iSpr = cList;
	for (i = kMaxSprites-1; i >= 0; --i,++iSpr)
	{
		GLfloat	a = atan2f(iSpr->vPos.y,iSpr->vPos.x);
		GLfloat	d = iSpr->vPos.lengthxy();
		iSpr->vPos.x = cosf(a+rSpin)*d;
		iSpr->vPos.y = sinf(a+rSpin)*d;

        GLfloat a2 = a + kSpinRate; // !! this should take time into account...
        GLfloat dx2 = cos(a2)*d - iSpr->vPos.x;
        GLfloat dy2 = sin(a2)*d - iSpr->vPos.y;
        
        iSpr->vFce = KS_VECTOR3(sinf(kGravAngle)*gravWeight + dx2,-cosf(kGravAngle)*gravWeight + dy2,0);
        
	}
	iSpr = cList;
	for (i = kMaxSprites-1; i >= 0; --i,++iSpr)
	{
#if 1
		jSpr = cList;
		for (j = i-1; j >= 0; --j,++jSpr)
		{
				if (iSpr->vPos.x+iSpr->fRadius > jSpr->vPos.x-jSpr->fRadius &&
						iSpr->vPos.x-iSpr->fRadius < jSpr->vPos.x+jSpr->fRadius &&
						iSpr->vPos.y+iSpr->fRadius > jSpr->vPos.y-jSpr->fRadius &&
						iSpr->vPos.y-iSpr->fRadius < jSpr->vPos.y+jSpr->fRadius)
				{
					GLfloat dx = iSpr->vPos.x - jSpr->vPos.x;
					GLfloat dy = iSpr->vPos.y - jSpr->vPos.y;
					GLfloat dist = sqrtf(dx*dx+dy*dy);
					GLfloat	maxDist = iSpr->fRadius+jSpr->fRadius;
					GLfloat dmax = maxDist - dist;
					if (dmax > 0 && dist > 0) 
					{
						GLfloat	scale = dmax/maxDist;
						scale *= scale;
						scale *= mySpeed/dist;
						iSpr->vFce.x += dx*scale;
						iSpr->vFce.y += dy*scale;
						// iSpr->vFce.z += dz*scale;
					}
					
				}
		}
#endif
		
		GLfloat dx = 0-(iSpr->vPos.x+iSpr->vVel.x);
		GLfloat dy = 0-(iSpr->vPos.y+iSpr->vVel.y);
		GLfloat dist = sqrtf(dx*dx+dy*dy);
		GLfloat dmax = dist - (kMirrorRad - iSpr->fRadius*2);
		if (dmax > 0 && dist > 0) {
			GLfloat	scale = dmax/(iSpr->fRadius*2);
			scale *= scale;
			scale *= mySpeed/dist;
			iSpr->vFce.x += dx*scale;
			iSpr->vFce.y += dy*scale;
		}
/*

		GLfloat	dz = kCenterZ-(iSpr->vPos.z+iSpr->vVel.z);
		dist = (float) fabs(dz);
		dmax = dist - ((float) fabs(kCenterZ)-iSpr->fRadius*2);
		if (dmax > 0)
		{
			GLfloat	scale = dmax/(iSpr->fRadius*2);
			scale *= scale;
			iSpr->weight += scale;
			scale *= mySpeed/dist;
			iSpr->vFce.z += dz*scale;
		}
*/
	}
	iSpr = cList;
	for (i = kMaxSprites-1; i >= 0; --i,++iSpr)
	{
		iSpr->vVel = iSpr->vVel+iSpr->vFce;
		iSpr->vVel = iSpr->vVel*kDamp;
		iSpr->vPos = iSpr->vPos + iSpr->vVel;
		iSpr->update();
	}
//	for (int i = 0; i < kMaxSprites; ++i)
//		cList[i].update();
	rSpin *= kSpinDamp;
	// reset the scope every couple minutes
	if (m_parent->m_fTime - m_parent->m_fLastKey > 40) {
		m_parent->m_fLastKey = m_parent->m_fTime ;
		m_parent->HandleSpecialKey(GLUT_KEY_DOWN);
	}
}



void SpriteList::preRender(bool drawBG)
{

	
	glDisable(GL_CULL_FACE);

	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);								// Enable Texture Mapping
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	// Draw zooming clouds

	if (0) // (drawBG)
	{
		// Draw Logo WaterMark
		float rc = (cosf(gList->m_parent->m_fTime*0.2f)+1)*.5f;
		glBindTexture(GL_TEXTURE_2D, gList->m_PhotoTexture[1].texID);
		glPushMatrix();
		float	tx = 0, ty = 0, tw = 1, th = 1;
		float	lw = 2, lh = lw*64.0f/400.0f;
		glTranslatef(5,-5,0);
		glColor4f(rc,rc,1-rc,.1f);
		glBegin(GL_QUADS);
			glTexCoord2f(tx,		ty+th); glVertex3f(-lw, -lh,  0.0f);	// Point 1 (Front)
			glTexCoord2f(tx+tw, ty+th); glVertex3f( lw, -lh,  0.0f);	// Point 2 (Front)
			glTexCoord2f(tx+tw, ty);		glVertex3f( lw,  lh,  0.0f);	// Point 3 (Front)
			glTexCoord2f(tx,		ty);		glVertex3f(-lw,  lh,  0.0f);	// Point 4 (Front)
		glEnd();
		glPopMatrix();
	}
	
	
	glBindTexture(GL_TEXTURE_2D, gList->m_PhotoTexture->texID);
  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	// glPushMatrix();
	// Fix this to compensate for grav rotation, keeping gravity down
	// glRotatef(-gList->m_parent->m_fTime*kSpinA,0,0,1);
}

void SpriteList::postRender()
{
	// glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);								// Disable Texture Mapping


// Draw Background Here
}

void SpriteList::draw()
{
	for (int i = 0; i < kMaxSprites; ++i)
		cList[i].draw();
}


void SpriteList::morphParams(float duration)
{
	iMaxCtr = (int) ((duration * kMaxSprites) / kSweepTime);
    // (duration 19.925793 * maxsprites 60) / sweeptime 20  (maxctr = 59) 
	// ATypical: (duration 0.500000 * maxsprites 60) / sweeptime 20  (maxctr = 1) 
	// printf("(duration %f * maxsprites %d) / sweeptime %d  (maxctr = %d)\n", duration, kMaxSprites, kSweepTime, iMaxCtr);
	iCtr = 0;
	bUseBlur = RandomI(3) == 0? true : false;

	bRegularPolys = !bRegularPolys;
	iSpriteType = RandomI(nbrSpriteTypes);
		
	fSrcParam[FS_Radius] = fMaxSize;
	if (rnd() < .5) {
		float	deltaSize = kLargestSprite - kSmallestSprite;		
		fDstParam[FS_Radius] = kSmallestSprite+rnd()*deltaSize/2+(fMaxSize < kSmallestSprite+deltaSize/2? deltaSize/2  : 0);
		fDstParam[FS_Radius] -= fSrcParam[FS_Radius];
	}
	else {
		fDstParam[FS_Radius] = 0;
	}

	fSrcParam[FS_SpinRate] = fMaxSpinRate;
	if (rnd() < .5) {
		fDstParam[FS_SpinRate] = rnd()*kMaxRoll/2 + (fMaxSpinRate > kMaxRoll/2? 0  : kMaxRoll/2);
		fDstParam[FS_SpinRate] -= fSrcParam[FS_SpinRate];
	}
	else
		fDstParam[FS_SpinRate] = 0;

	fSrcParam[FS_HueC] = fHueC;
	if (rnd() < .5) {
		fDstParam[FS_HueC] = rnd();
		fDstParam[FS_HueC] -= fSrcParam[FS_HueC];
	}
	else
		fDstParam[FS_HueC] = 0;

	fSrcParam[FS_HueW] = fHueW;
	if (rnd() < .5) {
		fDstParam[FS_HueW] = rnd();
		fDstParam[FS_HueW] -= fSrcParam[FS_HueW];
	}
	else
		fDstParam[FS_HueW] = 0;

	fSrcParam[FS_SatC] = fSatC;
	if (rnd() < .5) {
		fDstParam[FS_SatC] = rnd();
		fDstParam[FS_SatC] -= fSrcParam[FS_SatC];
	}
	else
		fDstParam[FS_SatC] = 0;

	fSrcParam[FS_SatW] = fSatW;
	if (rnd() < .5) {
		fDstParam[FS_SatW] = rnd();
		fDstParam[FS_SatW] -= fSrcParam[FS_SatW];
	}
	else
		fDstParam[FS_SatW] = 0;

	fSrcParam[FS_LumC] = fLumC;
	if (rnd() < .5) {
		fDstParam[FS_LumC] = .5f+rnd()*.5f;
		fDstParam[FS_LumC] -= fSrcParam[FS_LumC];
	}
	else
		fDstParam[FS_LumC] = 0;

	fSrcParam[FS_LumW] = fLumW;
	if (rnd() < .5) {
		fDstParam[FS_LumW] = rnd()*.5f;
		fDstParam[FS_LumW] -= fSrcParam[FS_LumW];
	}
	else
		fDstParam[FS_LumW] = 0;

}


void SpriteList::paramTick()
{
	++iCtr;
	if (iCtr > iMaxCtr) {
		float dur = 1+rnd()*kSweepTime;
		// printf("reset dur=%f\n", dur);
		morphParams(dur);
	}
	float r = (float) iCtr / iMaxCtr;
	r = ease(r);	// this is a macro, don't combine with above step

	// Over the course of the cycle, each parameter changes to the new setting...
	fMaxSize					 	= fSrcParam[FS_Radius] + r*fDstParam[FS_Radius];
	fMaxSpinRate 				= fSrcParam[FS_SpinRate] + r*fDstParam[FS_SpinRate];
	fHueC				 				= fSrcParam[FS_HueC] + r*fDstParam[FS_HueC];
	fHueW				 				= fSrcParam[FS_HueW] + r*fDstParam[FS_HueW];
	fSatC				 				= fSrcParam[FS_SatC] + r*fDstParam[FS_SatC];
	fSatW				 				= fSrcParam[FS_SatW] + r*fDstParam[FS_SatW];
	fLumC				 				= fSrcParam[FS_LumC] + r*fDstParam[FS_LumC];
	fLumW				 				= fSrcParam[FS_LumW] + r*fDstParam[FS_LumW];

}

bool SpriteList::wantsClipping()
{
	return true;
}


bool SpriteList::wantsTrails()
{
	return false;
}

bool SpriteList::wantsRadialBlur()
{
	return true; // was true
}

bool SpriteList::wantsMirrors()
{
	return true;
}

float lastMouseX = 0;
float lastMouseY = 0;

void SpriteList::handleSpecialKey(int key)
{
	float	x;
	switch (key) {
	case GLUT_KEY_UP:
		rSpin += 3.14159f/360; // half a degree
		break;
	case GLUT_KEY_DOWN:
		// printf("Key down\n");
		morphParams(0.5f);
		Reset();
		break;
#define kArrowSpeed	0.01f
	
	case GLUT_KEY_RIGHT:
		// for the mouse, if x goes from 0-1
		// reflections = 4*powf(3,x*2);
		// invert this to convert curNbrReflections to a linear value from 0-1
    x = (logf(m_parent->curNbrReflections/4)/logf(3))/2;
		x += kArrowSpeed;
		if (x > 1)
				x = 1;
		// bIsMagnetized = true;
		handleMouseMove(x,lastMouseY);
		break;

	case GLUT_KEY_LEFT:
		// for the mouse, if x goes from 0-1
		// reflections = 4*powf(3,x*2);
		// invert this to convert curNbrReflections to a linear value from 0-1
    x = (logf(m_parent->curNbrReflections/4)/logf(3))/2;
		x -= kArrowSpeed;
		if (x < 0)
				x = 0;
		// bIsMagnetized = true;
		handleMouseMove(x,lastMouseY);
		break;
	}

}

void SpriteList::handleKey(int key)
{
	switch (key) {
	case 'g':
	case 'G':
		bIsMagnetized = !bIsMagnetized;
		break;
	}
}

void SpriteList::handleMouseMove(float x, float y)
{
  // min 4, max 32, 12 at midpoint
	// modify this to tween to next notch
	float mReflections = 4*powf(3,x*2);
	m_parent->curNbrReflections = mReflections;
	m_parent->curMirrorAngle = 360.0f / m_parent->curNbrReflections;
	vA = 0;
  targetReflections = (int) mReflections;
	if (targetReflections & 1)
		++targetReflections;

//	if (x < .5f) {
//		m_parent->curNbrReflections = 4+x*16;
//		m_parent->curMirrorAngle = 360.0f / m_parent->curNbrReflections;
//	}
//	else {
//		m_parent->curMirrorAngle = 30-(x-.5f)*40;
//		m_parent->curNbrReflections = 360 / m_parent->curMirrorAngle;
//	}
	rSpin += y*3.14159f*4/360;
	bIsMagnetized = true;
//	if (lastMouseY != y)
//			bIsMagnetized = false;
	lastMouseX = x;
	lastMouseY = y;
}
