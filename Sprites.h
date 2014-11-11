#ifndef _H_Photos
#define _H_Photos

#include "ObjectCell.h"



enum { FS_Radius=0, FS_SpinRate, FS_Param, 
			 FS_HueC, FS_HueW, 
			 FS_SatC, FS_SatW,
			 FS_LumC, FS_LumW,
			 FS_Wiggle,
			 kMaxFSModes };



class ScopeApp;

class Sprite 
{
public:
	KS_VECTOR3	vPos;		    // location
	KS_VECTOR3	vVel;				// velocity
	KS_VECTOR3	vFce;				// force

	float				vDamp,weight;
	float				cycle,cycleStart;
	float				fRadius;		// radius
	float				fSpinRate;
	float				roll;
	int					iSpriteType;
	int					flags;
	GLfloat			cellx,celly;
	GLfloat			tx,ty,tw,th,rC,gC,bC,aC,rCD,gCD,bCD,aCD;
	GLfloat			param1,param2, param3;
	int					iParam1,iParam2;

	void init(int i);
	void randomize();
	void update();
	void draw();

};

class ScopeApp;

class SpriteList
{
	Sprite	*cList;

public:
	ScopeApp			*m_parent;
	float					fMaxSize, fMaxSpinRate, fParam;
	float					fHueC, fHueW, fSatC, fSatW, fLumC, fLumW, fWiggle;
	float					aSpin,rSpin;
	int						iCtr, iMaxCtr;
	int						iParam1,iParam2;
	bool					bRegularPolys;
	float					fSrcParam[kMaxFSModes],
								fDstParam[kMaxFSModes];
	int						iSpriteType;
	int						nbrSpriteTypes;
	int						targetReflections;
	float					vA;
	bool					bIsMagnetized;
	TextureImage	*m_PhotoTexture;


	SpriteList(ScopeApp	*parent);
	~SpriteList();
	void init();
	void paramTick();
	void morphParams(float dur);
	void handleKey(int key);
	void handleSpecialKey(int key);
	void handleMouseMove(float x, float y);
	void updateFrame();
	void draw();
	void preRender(bool drawBG);
	void postRender();
	bool allocateObjects();
	void deleteObjects();
	bool wantsClipping();
	bool wantsTrails();
	bool wantsRadialBlur();
	bool wantsMirrors();
	void Reset();
};


#endif
