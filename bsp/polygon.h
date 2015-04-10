/*
 *	file:		polygon.h
 *	description:	polygon module
 *	author:		A. T. Campbell, III
 *	date:		February 9, 1995
 */

#ifndef polygon_h
#define polygon_h

#include <stdio.h>
#include "bspvector.h"

/* constants */
#define MAXPOLYVERT 32

/* type definitions */
typedef enum {sidePositive, sideNegative, sideNeither, sideBoth} E_side;

typedef struct {
	int numverts;
	Vec3f verts[MAXPOLYVERT];
	Vec3f plane_normal;
	size_t material_index;
        int facesForward;   // true: faces same dir as BSP node's plane faces
} Polygon3f;

/* return values */
extern void		PolygonCopy3f(Polygon3f *src, Polygon3f *dest);
extern Polygon3f 	*PolygonCreate3f(void);
extern void 		PolygonCutConvex3f(Polygon3f *p, Vec3f pntOnPlane,
					   Vec3f planeNormal, int maxPieces,
					   int *numpos, Polygon3f *posarr[],
					   int *numneg, Polygon3f *negarr[]);
extern void		PolygonDestroy3f(Polygon3f *p);
extern void		PolygonNormal3f(Polygon3f *p, Vec3f normv);
extern void		PolygonPrint3f(FILE *fp, Polygon3f *p);
extern void		PolygonSet3f(Polygon3f *p, int n, Vec3f v[]);
extern E_side 		PolygonSidePlane3f(Polygon3f *p, Vec3f pntOnPlane,
					   Vec3f planeNormal);
extern void 		PolygonTranslate3f(Polygon3f *p, Vec3f delta);
extern void 		PolygonBBox3f(Polygon3f *p, Vec3f low, Vec3f high);
#endif /* polygon_h */
