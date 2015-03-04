/*
 *	file:		polygon.c
 *	description:	polygon module
 *	author:		A. T. Campbell, III
 *	date:		February 9, 1995
 */

#include <math.h>
#include <stdlib.h>
#include "polygon.h"
#include "brep.h"
#include "bspvector.h"

/* copy a polygon */
void
PolygonCopy3f(Polygon3f *src, Polygon3f *dest)
{
	int i;
	dest->numverts = src->numverts;
	for (i = 0; i < src->numverts; i++)
		Vcopy3f(src->verts[i], dest->verts[i]);
	Vcopy3f(src->plane_normal, dest->plane_normal);
	dest->material_index = src->material_index;
	dest->facesForward= src->facesForward;
}

/* allocate memory */
Polygon3f *
PolygonCreate3f(void)
{
	Polygon3f *p;
	p = (Polygon3f *)malloc(sizeof(Polygon3f));
	if (p != NULL) {
		p->numverts = 0;
		Vset3f(0., 0., 0., p->plane_normal);
		p->material_index = 0;
	}
	return p;
}

/* split a convex polygon by a plane */
void
PolygonCutConvex3f(Polygon3f *p, Vec3f pntOnPlane, Vec3f planeNormal,
		   int maxPieces,
		   int *numpos, Polygon3f *posarr[],
		   int *numneg, Polygon3f *negarr[])
{
	E_side side;
	Vec4f coeffs;

	side = PolygonSidePlane3f(p, pntOnPlane, planeNormal);
	if (side == sidePositive) {
		*numpos = 1;
		*numneg = 0;
		posarr[0] = PolygonCreate3f();
		PolygonCopy3f(p, posarr[0]);
	}
	else if (side == sideNegative) {
		*numpos = 0;
		*numneg = 1;
		negarr[0] = PolygonCreate3f();
		PolygonCopy3f(p, negarr[0]);
	}
	else if (side == sideBoth) {
		/* Dave Carver gets to write this one */
		*numpos = *numneg = 0;
		posarr[0] = PolygonCreate3f();
		negarr[0] = PolygonCreate3f();
		Vset4f (planeNormal[0], planeNormal[1], planeNormal[2],
			-Vdot3f(pntOnPlane,planeNormal),coeffs);
		brep_split(planeNormal, p->numverts, p->verts, UNIVEPS,
			&(posarr[0]->numverts), posarr[0]->verts, 
			&(negarr[0]->numverts), negarr[0]->verts);
		if (posarr[0]->numverts == 0)
			PolygonDestroy3f(posarr[0]);
		else {
			*numpos = 1;
			Vcopy3f(p->plane_normal, posarr[0]->plane_normal);
			posarr[0]->material_index = p->material_index;
		}
		if (negarr[0]->numverts == 0)
			PolygonDestroy3f(negarr[0]);
		else {
			*numneg = 1;
			Vcopy3f(p->plane_normal, negarr[0]->plane_normal);
			negarr[0]->material_index = p->material_index;
		}
	}
	else { /* side = sideNeither */
		*numpos = *numneg = 0;
	}
		
}

/* free memory */
void
PolygonDestroy3f(Polygon3f *p)
{
	if (p != NULL)
		free((char *)p);
}

/* compute normal to a polygon */
/* use Newell's formulation (Foley, Van Dam, Feiner, & Hughes, pp. 477) */
void
PolygonNormal3f(Polygon3f *p, Vec3f normv)
{
	Vec3f *v;
	int i, ip1, n;
	if (p == NULL) return;
	n = p->numverts;
	v = p->verts;
	Vset3f(0., 0., 0., normv);
	i = n - 1;
	for (ip1 = 0; ip1 < n; ip1++) {
		normv[0] += (v[i][1] - v[ip1][1]) *
			(v[i][2] + v[ip1][2]);
		normv[1] += (v[i][2] - v[ip1][2]) *
			(v[i][0] + v[ip1][0]);
		normv[2] += (v[i][0] - v[ip1][0]) *
                                   (v[i][1] + v[ip1][1]);
		i = ip1;
	}
	Vnormalize3f(normv);
	
}

/* print information about a polygon */
void
PolygonPrint3f(FILE *fp, Polygon3f *p)
{
	int i;
	fprintf(fp, "numverts=%d material_index=%d\n", p->numverts, p->material_index);
	for (i = 0; i < p->numverts; i++)
		fprintf(fp, "%f %f %f\n",
			p->verts[i][0], p->verts[i][1], p->verts[i][2]);
}

/* set the vertices of a polygon */
void
PolygonSet3f(Polygon3f *p, int n, Vec3f v[])
{
	int i;
	p->numverts = n;
	for (i = 0; i < n; i++)
		Vcopy3f(v[i], p->verts[i]);
}

/* determine what side of a plane a polygon is on */
E_side
PolygonSidePlane3f(Polygon3f *p, Vec3f pntOnPlane, Vec3f planeNormal)
{
	int positive, negative;
	Vec4f planeCoeffs;
	float testVal;
	int i;

	/* compute planar equation */
	Vset4f(planeNormal[0], planeNormal[1], planeNormal[2],
	       -Vdot3f(pntOnPlane, planeNormal), planeCoeffs);

	/* test all vertices */
	positive = negative = 0;
	for (i = 0; i < p->numverts; i++) {
		testVal = planeCoeffs[0]*p->verts[i][0] +
			planeCoeffs[1]*p->verts[i][1] +
			planeCoeffs[2]*p->verts[i][2] +
			planeCoeffs[3];
		if (testVal < 0)
			negative = 1;
		if (testVal > 0)
			positive = 1;
	}

	/* determine return value */
	if (positive && !negative)
		return sidePositive;
	else if (negative && !positive)
		return sideNegative;
	else if (negative && positive)
		return sideBoth;
	else
		return sideNeither;
}

/* translate the vertices of a polygon */
void
PolygonTranslate3f(Polygon3f *p, Vec3f delta)
{
	int i;
	for (i = 0; i < p->numverts; i++)
		Vadd3f(delta, p->verts[i], p->verts[i]);
}

/* get the bounding box of a polygon */
void PolygonBBox3f(Polygon3f *p, Vec3f low, Vec3f high) {
  int i;
  if (p->numverts <1) return;
  
  Vcopy3f(p->verts[0],low);
  Vcopy3f(p->verts[0],high);
  for (i=1; i <p->numverts; i++) {
    Vmin3f(low,p->verts[i],low);
    Vmax3f(high,p->verts[i],high);
  }
}
