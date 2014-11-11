#include "Scope.h"
#include <math.h>



void SetVector3(K_VECTOR3 *v, float x, float y, float z)

{

	v->x = x;

	v->y = y;

	v->z = z;

}



void Vec3Subtract(K_VECTOR3 *res, K_VECTOR3 *a, K_VECTOR3 *b)

{

	res->x = a->x - b->x;

	res->y = a->y - b->y;

	res->z = a->z - b->z;

}



void Vec3Add(K_VECTOR3 *res, K_VECTOR3 *a, K_VECTOR3 *b)

{

	res->x = a->x + b->x;

	res->y = a->y + b->y;

	res->z = a->z + b->z;

}



void Vec3Multiply(K_VECTOR3 *res, K_VECTOR3 *a, K_VECTOR3 *b)

{

	res->x = a->x * b->x;

	res->y = a->y * b->y;

	res->z = a->z * b->z;

}





GLfloat Vec3Length(K_VECTOR3 *v)

{

	return (GLfloat) sqrt(v->x*v->x + v->y*v->y + v->z*v->z);

}



void Vec3Normalize(K_VECTOR3 *v)

{

	float	len = Vec3Length(v);

	if (len != 0)

	{

		v->x /= len;

		v->y /= len;

		v->z /= len;

	}

}





KS_VECTOR3::KS_VECTOR3()

{

	x = 0;

	y = 0;

	z = 0;

}



KS_VECTOR3::KS_VECTOR3(GLfloat	x, GLfloat	y, GLfloat z)

{

	this->x = x;

	this->y = y;

	this->z = z;

}



KS_VECTOR3::KS_VECTOR3(GLfloat	*xyz)

{

	this->x = xyz[0];

	this->y = xyz[1];

	this->z = xyz[2];

}



// Calculates 3rd degree polynomial based on array of 4 points

// and a single variable (u) which is generally between 0 and 1

KS_VECTOR3 KS_VECTOR3::Bernstein(float u, KS_VECTOR3 *p) 

{

	KS_VECTOR3	a, b, c, d;



	a = p[0] * pow(u,3);

	b = p[1] * (3*pow(u,2)*(1-u));

	c = p[2] * (3*u*pow((1-u),2));

	d = p[3] * pow((1-u),3);



	return a+b+c+d;

}



KS_VECTOR2::KS_VECTOR2()

{

	x = 0;

	y = 0;

}



KS_VECTOR2::KS_VECTOR2(GLfloat	x, GLfloat	y)

{

	this->x = x;

	this->y = y;

}



KS_VECTOR2::KS_VECTOR2(GLfloat	*xy)

{

	this->x = xy[0];

	this->y = xy[1];

}

