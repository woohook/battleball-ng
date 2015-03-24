/*
 *	file:		bspvector.h
 *	description:	vector module
 *	author:		A. T. Campbell, III
 *	date:		February 9, 1995
 */

#ifndef bspvector_h
#define bspvector_h

/* type definitions */
typedef float Vec2f[2];
typedef float Vec3f[3];
typedef float Vec4f[4];

/* return values */

/* 3D functions */
inline void		Vadd3f(Vec3f v1, Vec3f v2, Vec3f sum);
inline void		Vcopy3f(Vec3f src, Vec3f dest);
inline void		Vcross3f(Vec3f va, Vec3f vb, Vec3f vprod);
inline float		Vdot3f(Vec3f v1, Vec3f v2);
inline void		Vnormalize3f(Vec3f vec);
inline void		Vset3f(float x, float y, float z, Vec3f vec);
inline void		Vsub3f(Vec3f v1, Vec3f v2, Vec3f diff);
inline void		Vtimes3f(float factor, Vec3f vin, Vec3f vout);
inline void		Vmin3f(Vec3f v1, Vec3f v2, Vec3f minvec);
inline void		Vmax3f(Vec3f v1, Vec3f v2, Vec3f maxvec);

/* 4D functions */
inline void		Vcopy4f(Vec4f src, Vec4f dest);
inline void		Vset4f(float x, float y, float z, float w, Vec4f vec);

#include <math.h>
/* #include <stdio.h> #include <stdlib.h> */

inline void
Vadd3f(Vec3f v1, Vec3f v2, Vec3f sum)
{
	sum[0] = v1[0] + v2[0];
	sum[1] = v1[1] + v2[1];
	sum[2] = v1[2] + v2[2];
}

inline void
Vcopy3f(Vec3f src, Vec3f dest)
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}

inline void             
Vcross3f(Vec3f va, Vec3f vb, Vec3f vprod)
{
	Vec3f temp;
       	temp[0] = va[1] * vb[2] - va[2] * vb[1];
	temp[1] = va[2] * vb[0] - va[0] * vb[2];
	vprod[2] = va[0] * vb[1] - va[1] * vb[0];
       	vprod[0] = temp[0];
	vprod[1] = temp[1];
}

inline float
Vdot3f(Vec3f v1, Vec3f v2)
{
	return(v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2]);
}

inline void
Vnormalize3f(Vec3f vec)
{
	float length, lengthSquared;
	lengthSquared = Vdot3f(vec, vec);
	if (lengthSquared <= 0.)
		Vset3f(0., 0., 0., vec);
	else {
		length = sqrt(lengthSquared);
		Vtimes3f(1./length, vec, vec);
	}
}
inline void
Vset3f(float x, float y, float z, Vec3f vec)
{
	vec[0] = x;
	vec[1] = y;
	vec[2] = z;
}

inline void
Vsub3f(Vec3f v1, Vec3f v2, Vec3f diff)
{
	diff[0] = v1[0] - v2[0];
	diff[1] = v1[1] - v2[1];
	diff[2] = v1[2] - v2[2];
}

inline void
Vtimes3f(float factor, Vec3f vin, Vec3f vout)
{
	vout[0] = vin[0] * factor;
	vout[1] = vin[1] * factor;
	vout[2] = vin[2] * factor;
}

inline void
Vcopy4f(Vec4f src, Vec4f dest)
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
}

inline void
Vset4f(float x, float y, float z, float w, Vec3f vec)
{
	vec[0] = x;
	vec[1] = y;
	vec[2] = z;
	vec[3] = w;
}

/* funcs added by PAH below: */

inline void Vmin3f(Vec3f v1, Vec3f v2, Vec3f minvec) {
  minvec[0]= v1[0]<v2[0] ? v1[0] : v2[0];
  minvec[1]= v1[1]<v2[1] ? v1[1] : v2[1];
  minvec[2]= v1[2]<v2[2] ? v1[2] : v2[2];
}

inline void Vmax3f(Vec3f v1, Vec3f v2, Vec3f maxvec) {
  maxvec[0]= v1[0]>v2[0] ? v1[0] : v2[0];
  maxvec[1]= v1[1]>v2[1] ? v1[1] : v2[1];
  maxvec[2]= v1[2]>v2[2] ? v1[2] : v2[2];
}


#endif /* bspvector_h */
