#ifndef _H_KS_VECTOR
#define _H_KS_VECTOR	1

class KS_VECTOR3 {
public:
	GLfloat	x,y,z;
	KS_VECTOR3();
	KS_VECTOR3(GLfloat	x,GLfloat	y,GLfloat	z);
	KS_VECTOR3(GLfloat	*xyz);

	static KS_VECTOR3 Bernstein(float u, KS_VECTOR3 *p);

	inline KS_VECTOR3	operator+(KS_VECTOR3	&b)
	{
		return KS_VECTOR3(x+b.x, y+b.y, z+b.z);
	}

	inline KS_VECTOR3	operator+=(KS_VECTOR3	&b)
	{
		return KS_VECTOR3(x+b.x, y+b.y, z+b.z);
	}

	inline KS_VECTOR3	operator-(KS_VECTOR3	&b)
	{
		return KS_VECTOR3(x-b.x, y-b.y, z-b.z);
	}

	inline KS_VECTOR3	operator*(KS_VECTOR3	&b)
	{
		return KS_VECTOR3(x*b.x, y*b.y, z*b.z);
	}

	inline KS_VECTOR3	operator*(GLfloat f)
	{
		return KS_VECTOR3(x*f, y*f, z*f);
	}

	inline KS_VECTOR3	operator*(double f)
	{
		return KS_VECTOR3( x*(GLfloat)f,y*(GLfloat) f, z*(GLfloat)f);
	}

	inline GLfloat	length()
	{
		return sqrtf(x*x + y*y + z*z);
	}

	inline GLfloat	lengthxy()
	{
		return sqrtf(x*x + y*y);
	}

	inline void normalize()
	{
		GLfloat	len = length();
		if (len != 0.0)
		{
			x /= len;
			y /= len;
			z /= len;
		}
	}



	inline KS_VECTOR3	operator/(GLfloat f)
	{
		return KS_VECTOR3(x/f, y/f, z/f);
	}
	inline KS_VECTOR3	operator/(double f)
	{
		return KS_VECTOR3(x/(GLfloat)f, y/(GLfloat)f, z/(GLfloat)f);
	}
	static inline KS_VECTOR3 Cross(KS_VECTOR3 vVector1, KS_VECTOR3 vVector2)
	{
		KS_VECTOR3 vCross;								// The vector to hold the cross product
													// Get the X value
		vCross.x = ((vVector1.y * vVector2.z) - (vVector1.z * vVector2.y));
													// Get the Y value
		vCross.y = ((vVector1.z * vVector2.x) - (vVector1.x * vVector2.z));
													// Get the Z value
		vCross.z = ((vVector1.x * vVector2.y) - (vVector1.y * vVector2.x));

		return vCross;								// Return the cross product
	}

};

class KS_VECTOR2 {
public:
	GLfloat	x,y;
	KS_VECTOR2();
	KS_VECTOR2(GLfloat	x,GLfloat	y);
	KS_VECTOR2(GLfloat	*xy);

	inline	KS_VECTOR2	operator+(KS_VECTOR2	&b)
		{
			return KS_VECTOR2(x+b.x, y+b.y);
		}

	inline		KS_VECTOR2	operator-(KS_VECTOR2	&b)
		{
			return KS_VECTOR2(x-b.x, y-b.y);
		}

	inline		KS_VECTOR2	operator*(KS_VECTOR2	&b)
		{
			return KS_VECTOR2(x*b.x, y*b.y);
		}

	inline		KS_VECTOR2	operator*(GLfloat f)
		{
			return KS_VECTOR2(x*f, y*f);
		}

	inline		KS_VECTOR2	operator/(GLfloat f)
		{
			return KS_VECTOR2(x/f, y/f);
		}

	inline		KS_VECTOR2	operator*(double f)
		{
			return KS_VECTOR2( x*(GLfloat)f,y*(GLfloat)f);
		}

	inline		KS_VECTOR2	operator/(double f)
		{
			return KS_VECTOR2( x/(GLfloat)f,y/(GLfloat)f);
		}

	inline	GLfloat	length()
		{
			return sqrtf(x*x + y*y);
		}

	inline	void normalize()
		{
			GLfloat	len = length();
			if (len != 0.0)
			{
				x /= len;
				y /= len;
			}
		}
};



typedef struct {
	GLfloat	x,y,z;
} K_VECTOR3;

typedef struct {
	GLfloat	x,y;
} K_VECTOR2;

typedef float FLOAT;

inline FLOAT VecMin( K_VECTOR3 v )
{
    if( v.x < v.y ) return (v.x < v.z ) ? v.x : v.z;
    else            return (v.y < v.z ) ? v.y : v.z;
}

inline FLOAT VecMax( K_VECTOR3 v )
{
    if( v.x > v.y ) return (v.x > v.z ) ? v.x : v.z;
    else            return (v.y > v.z ) ? v.y : v.z;
}

#endif