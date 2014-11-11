
#include "Scope.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

void HSVtoRGB(float h, float s, float v, K_VECTOR3 *mat)
{
	float	r,g,b,f,m,n,k;
	int		i;

	h *= 360;
//	h = (360 * *rp) / 255.0;
//	s = *gp / 255.0;
//	v = *bp / 255.0;

	if (s == 0.0) {
		r = g = b = v;
	}
	else {
		if (h >= 360.0)
			h = 0;
		else
			h /= 60.0;
		i = (int) h;
		f = h - i;
		m = v*(1-s);
		n = v*(1-s*f);
		k = v*(1-s*(1-f));
		switch (i) {
		case 0:
			r = v;
			g = k;
			b = m;
			break;
		case 1:
			r = n;
			g = v;
			b = m;
			break;
		case 2:
			r = m;
			g = v;
			b = k;
			break;
		case 3:
			r = m;
			g = n;
			b = v;
			break;
		case 4:
			r = k;
			g = m;
			b = v;
			break;
		default:
			r = v;
			g = m;
			b = n;
			break;
		}
	}
	mat->x = r; // r;
	mat->y = g; // g;
	mat->z = b;
}

void ViewOrtho()												// Set Up An Ortho View
{
	glMatrixMode(GL_PROJECTION);								// Select Projection
	glPushMatrix();												// Push The Matrix
	glLoadIdentity();											// Reset The Matrix
	glOrtho( 0, 640 , 480 , 0, -1, 1 );		// Select Ortho Mode (640x480)
	glMatrixMode(GL_MODELVIEW);						// Select Modelview Matrix
	glPushMatrix();												// Push The Matrix
	glLoadIdentity();											// Reset The Matrix
}

void ViewPerspective()											// Set Up A Perspective View
{
	glMatrixMode( GL_PROJECTION );								// Select Projection
	glPopMatrix();												// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );								// Select Modelview
	glPopMatrix();												// Pop The Matrix
}
