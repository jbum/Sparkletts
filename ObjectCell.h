#ifndef _H_ObjectCell
#define _H_ObjectCell

#include "Scope.h"

class ObjectCell
{
public:
	

	ObjectCell(ScopeApp	*parent);
	virtual void init();
	virtual void updateFrame();
	virtual void draw();
	virtual void preRender(bool drawBG);
	virtual void postRender();
	virtual bool wantsClipping();
	virtual bool wantsTrails();
	virtual bool wantsReflections();
	virtual bool wantsRadialBlur();
	virtual bool wantsMirrors();
	virtual void morphParams(float duration);
};

#endif

