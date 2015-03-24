
/*
 *	File:		brep.c
 *	Description:	boundary rep module
 *	Author:		A. T. Campbell, III
 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "brep.h"
#include "bspvector.h"

/******************************************************************************/

/* constants */
#ifndef FALSE
#define FALSE		0
#define TRUE		1
#endif /* FALSE */

/******************************************************************************/

/* return values */
static void int_seg_plane(Vec3f v1, Vec3f v2, float coeffs[], Vec3f vout);
static int  	Vplanesideeps(float *v, float *coeffs, float eps);

/******************************************************************************/

void
brep_copy(int sn, Vec3f sv[], int *dn, Vec3f dv[])
{
	int	i;

	*dn = sn;
	for (i = 0; i < sn; i++)
		Vcopy3f(sv[i], dv[i]);
}

/******************************************************************************/

void
brep_init(float coeffs[4], int *n, Vec3f v[])
{
	int	d, dmax, i, index, inc;
	static Vec3f	projarr[3][4] = {
			{
				{0., -UNIVSZ, -UNIVSZ},
				{0., UNIVSZ, -UNIVSZ},
				{0., UNIVSZ, UNIVSZ},
				{0., -UNIVSZ, UNIVSZ},
			},
			{
				{-UNIVSZ, 0., -UNIVSZ},
				{-UNIVSZ, 0., UNIVSZ},
				{UNIVSZ, 0., UNIVSZ},
				{UNIVSZ, 0., -UNIVSZ},
				},
			{
				{-UNIVSZ, -UNIVSZ, 0.},
				{UNIVSZ, -UNIVSZ, 0.},
				{UNIVSZ, UNIVSZ, 0.},
				{-UNIVSZ, UNIVSZ, 0.},
			} 	
	};

	*n = 4;

	/* determine projection axis */
	dmax = 0;
	for (d = 1; d < 3; d++)
		if (fabs(coeffs[d]) > fabs(coeffs[dmax]))
			dmax = d;

	/* project */
	if (coeffs[dmax] > 0.) {
		inc = 1;
		index = 0;
	}
	else {
		inc = -1;
		index = 3;
	}
	for (i = 0; i < 4; i++, index += inc) {
		Vcopy3f(projarr[dmax][index], v[i]);
		v[i][dmax] = (-Vdot3f(v[i], coeffs) - coeffs[3])/coeffs[dmax];
	}
}

/******************************************************************************/

int
brep_partition(int n, Vec3f v[], float coeffs[], float eps)
{
	int	i, pos, neg, side, val;

	pos = neg = FALSE;
	for (i = 0; i < n; i++) {
		side = Vplanesideeps(v[i], coeffs, eps);
		if (side < 0)
			neg = TRUE;
		else if (side > 0)
			pos = TRUE;
		if (pos && neg) break;
	}
	if (!pos && !neg)
		val = PON;
	else if (pos && neg)
		val = PINBOTH;
	else if (pos && !neg)
		val = PINPOS;
	else if (!pos && neg)
		val = PINNEG;
	return val;
}

/******************************************************************************/

static int
Vplanesideeps(float *v, float *coeffs, float eps)
{
	float	t;

	t = v[0] * coeffs[0] + v[1] * coeffs[1] + v[2] * coeffs[2] + coeffs[3];
	if (t > eps) return(1);
	if (t < -eps) return(-1);
	return(0);
}

/******************************************************************************/

void
brep_refine(float coeffs[4], int *n, Vec3f v[], int dir, float eps)
{
	int	i, iprev, j, nout, nv, side, sideprev;
	Vec3f	outv[BREPMAXVERTS], pnt;

	if (*n <= 0) return;	/* no verts - trivial reject */

	side = brep_partition(*n, v, coeffs, eps);
	if (side == PON) { 	/* on plane */
		*n = 0;
		return;
	}

	/* find a vertex on the correct side */
	nv = *n;
	for (i = 0; (i < nv) && Vplanesideeps(v[i], coeffs, eps)
		 != dir; i++)
		;

	if (i == nv) {		/* brep completely not on that side */
		*n = 0;
		return;
	}

	/* perform clipping */ 
	nout = 0;
	side = dir;
	for (j = 0, sideprev = side, iprev = i, i = (i+1)%nv; j < nv; j++,
		sideprev = side, iprev = i, i = (i+1)%nv) { 
		side = Vplanesideeps(v[i], coeffs, eps);
		if (sideprev == dir) {
			if (side == dir) {
				Vcopy3f(v[i], outv[nout]);
				nout++;
			}
			else if (side == 0) {
				Vcopy3f(v[i], outv[nout]);
				nout++;
			}
			else {
				/* compute intersection pnt */
				int_seg_plane(v[iprev], v[i], coeffs, pnt);
				Vcopy3f(pnt, outv[nout]);
				nout++;
			}
		}
		else if (sideprev == -dir) {
			if (side == dir) {
				/* compute intersection pnt */
				int_seg_plane(v[iprev], v[i], coeffs, pnt);
				Vcopy3f(pnt, outv[nout]);
				nout++;
				Vcopy3f(v[i], outv[nout]);
				nout++;
			}
			else if (side == 0) {
				Vcopy3f(v[i], outv[nout]);
				nout++;
			}
		}
		else if (sideprev == 0) {
			if (side == dir) {
				Vcopy3f(v[i], outv[nout]);
				nout++;
			}
			else if (side == 0) {
				Vcopy3f(v[i], outv[nout]);
				nout++;
			}
		}
		else
			fprintf(stderr, "brep_refine:weird case\n");
	}
	for (i = 0; i < nout; i++)
		Vcopy3f(outv[i], v[i]);
	*n = nout;
}

/******************************************************************************/

static void 
int_seg_plane(Vec3f v1, Vec3f v2, float coeffs[], Vec3f vout)
{
	Vec3f  dir, off;
        float   t, denom;

	Vsub3f(v2, v1, dir);
	denom = Vdot3f(coeffs, dir);
	if (denom == 0.)
		Vcopy3f(v1, vout);
	else {
		t = -(Vdot3f(coeffs, v1) + coeffs[3])/denom;
		Vtimes3f(t, dir, off);
		Vadd3f(off, v1, vout);
	}
}

/******************************************************************************/

void	
brep_split(float coeffs[4], int n, Vec3f v[], float eps,
	int *npos, Vec3f vpos[], int *nneg, Vec3f vneg[])
{
	int	i, iprev, j, nposout, nnegout, side, sideprev, sidelastreal;
	Vec3f	pnt;

	*npos = *nneg = 0;
	side = brep_partition(n, v, coeffs, eps);
	if (side != PINBOTH) {
		if (side == PINPOS) 
			brep_copy(n, v, npos, vpos);
		else if (side == PINNEG) 
			brep_copy(n, v, nneg, vneg);
		else /* side == PON */ 
			/* to make shadow tols work, map to - side */
			brep_copy(n, v, nneg, vneg);
		return;
	}

	/* find a vertex on one side or the other */
	for (i = 0; (i < n) && 
	     (side = Vplanesideeps(v[i], coeffs, eps)) == 0; i++)
		;

	/* perform clipping */ 
	nposout = nnegout = 0;
	sidelastreal = side;
	for (j = 0, sideprev = side, iprev = i, i = (i+1)%n; j < n; j++,
		sideprev = side, iprev = i, i = (i+1)%n) { 
		side = Vplanesideeps(v[i], coeffs, eps);
		if (sideprev > 0) {
			if (side > 0) {
				Vcopy3f(v[i], vpos[nposout]);
				nposout++;
			}
			else if (side == 0) {
				sidelastreal = 1;
				Vcopy3f(v[i], vpos[nposout]);
				nposout++;
			}
			else { /* side < 0 */
				/* compute intersection pnt */
				int_seg_plane(v[iprev], v[i], coeffs, pnt);
				Vcopy3f(pnt, vpos[nposout]);
				nposout++;
				Vcopy3f(pnt, vneg[nnegout]);
				nnegout++;
				Vcopy3f(v[i], vneg[nnegout]);
				nnegout++;
			}
		}
		else if (sideprev < 0) {
			if (side > 0) {
				/* compute intersection pnt */
				int_seg_plane(v[iprev], v[i], coeffs, pnt);
				Vcopy3f(pnt, vneg[nnegout]);
				nnegout++;
				Vcopy3f(pnt, vpos[nposout]);
				nposout++;
				Vcopy3f(v[i], vpos[nposout]);
				nposout++;
			}
			else if (side == 0) {
				sidelastreal = -1;
				Vcopy3f(v[i], vneg[nnegout]);
				nnegout++;
			}
			else {  /* side < 0 */
				Vcopy3f(v[i], vneg[nnegout]);
				nnegout++;
			}
		}
		else if (sideprev == 0) {
			if (side > 0) {
				if (sidelastreal < 0) {
					Vcopy3f(v[iprev], vpos[nposout]);
					nposout++;
				}
				Vcopy3f(v[i], vpos[nposout]);
				nposout++;
			}
			else if (side == 0) {
				if (sidelastreal > 0) {
					Vcopy3f(v[i], vpos[nposout]);
					nposout++;
				}
				else {
					Vcopy3f(v[i], vneg[nnegout]);
					nnegout++;
				}
			}
			else { /* side < 0 */
				if (sidelastreal > 0) {
					Vcopy3f(v[iprev], vneg[nnegout]);
					nnegout++;
				}
				Vcopy3f(v[i], vneg[nnegout]);
				nnegout++;
			}
		}
		else
			fprintf(stderr, "brep_split:weird case\n");
	}
	*npos = nposout;
	*nneg = nnegout;
}

/******************************************************************************/

#ifdef TEST	/* test program for brep module */

void refine_test(void);
void do_refine_test(int nv, Vec3f v[], float coef[]);
void split_test(void);
void do_split_test(int nv, Vec3f v[], float coef[]);

/******************************************************************************/

int
main(int arg, char *argv[])
{
	/* refine_test(); */
	split_test();
	return 0;
}

/******************************************************************************/

void
refine_test(void)
{
	static Vec3f	v[][4] = {
		{
			{-1., -1., 0.},	
			{1., -1., 0.},	
			{1., 1., 0.},	
			{-1., 1., 0.},	
		},
		{
			{-1., 0., 0.},
			{1., -1., 0.},	
			{1., 0., 0.},	
			{1., 1., 0.},	
		},
	};
	static float	c[][4] = {
			{1., 0., 0., 2.},
			{1., 0., 0., 1.},
			{1., 0., 0., 0.},
			{1., 0., 0., -1.},
			{1., 0., 0., -2.},
			{0., 1., 0., 2.},
			{0., 1., 0., 1.},
			{0., 1., 0., 0.},
			{0., 1., 0., -1.},
			{0., 1., 0., -2.},
			{0., 0., 1., 2.},
			{0., 0., 1., 1.},
			{0., 0., 1., 0.},
			{0., 0., 1., -1.},
			{0., 0., 1., -2.},
	};
	int	i, j;

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 15; j++) {
			do_refine_test(4, v[i], c[j]);
		}
	}
}

/******************************************************************************/

void
do_refine_test(int nv, Vec3f v[], float coef[])
{
	int	nout, i, j;	
	Vec3f	vout[BREPMAXVERTS];

	printf("test inputs:\n");
	printf("\tplane = %f %f %f %f\n", coef[0], coef[1], coef[2], coef[3]);;
	printf("\tpoly = \n");
	for (i = 0; i < nv; i++)
		printf("\t%f %f %f\n", v[i][0], v[i][1], v[i][2]);

	brep_copy(nv, v, &nout, vout);
	brep_refine(coef, &nout, vout, -1, UNIVEPS);
	printf("old negative output:\n");
	for (i = 0; i < nout; i++)
		printf("\t%f %f %f\n", vout[i][0], vout[i][1], vout[i][2]);

	brep_copy(nv, v, &nout, vout);
	brep_refine(coef, &nout, vout, 1, UNIVEPS);
	printf("positive output:\n");
	for (i = 0; i < nout; i++)
		printf("\t%f %f %f\n", vout[i][0], vout[i][1], vout[i][2]);

	printf("-----------------\n");
}

/******************************************************************************/

void
split_test(void)
{
	static Vec3f	v[][4] = {
		{
			{-1., -1., 0.},	
			{1., -1., 0.},	
			{1., 1., 0.},	
			{-1., 1., 0.},	
		},
		{
			{-1., 0., 0.},
			{1., -1., 0.},	
			{1., 0., 0.},	
			{1., 1., 0.},	
		},
	};
	static float	c[][4] = {
			{1., 0., 0., 2.},
			{1., 0., 0., 1.},
			{1., 0., 0., 0.},
			{1., 0., 0., -1.},
			{1., 0., 0., -2.},
			{0., 1., 0., 2.},
			{0., 1., 0., 1.},
			{0., 1., 0., 0.},
			{0., 1., 0., -1.},
			{0., 1., 0., -2.},
			{0., 0., 1., 2.},
			{0., 0., 1., 1.},
			{0., 0., 1., 0.},
			{0., 0., 1., -1.},
			{0., 0., 1., -2.},
	};
	int	i, j;

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 15; j++) {
			do_split_test(4, v[i], c[j]);
		}
	}
}

/******************************************************************************/

void
do_split_test(int nv, Vec3f v[], float coef[])
{
	int	nout, i, j, npos, nneg;	
	Vec3f	vout[BREPMAXVERTS], vpos[BREPMAXVERTS], vneg[BREPMAXVERTS];

	printf("test inputs:\n");
	printf("\tplane = %f %f %f %f\n", coef[0], coef[1], coef[2], coef[3]);;
	printf("\tpoly = \n");
	for (i = 0; i < nv; i++)
		printf("\t%f %f %f\n", v[i][0], v[i][1], v[i][2]);

	brep_copy(nv, v, &nout, vout);
	brep_refine(coef, &nout, vout, -1, UNIVEPS);
	printf("refine negative output:\n");
	for (i = 0; i < nout; i++)
		printf("\t%f %f %f\n", vout[i][0], vout[i][1], vout[i][2]);

	brep_copy(nv, v, &nout, vout);
	brep_refine(coef, &nout, vout, 1, UNIVEPS);
	printf("refine positive output:\n");
	for (i = 0; i < nout; i++)
		printf("\t%f %f %f\n", vout[i][0], vout[i][1], vout[i][2]);

	brep_split(coef, nv, v, UNIVEPS, &npos, vpos, &nneg, vneg);
	printf("split negative output:\n");
	for (i = 0; i < nneg; i++)
		printf("\t%f %f %f\n", vneg[i][0], vneg[i][1], vneg[i][2]);
	printf("split positive output:\n");
	for (i = 0; i < npos; i++)
		printf("\t%f %f %f\n", vpos[i][0], vpos[i][1], vpos[i][2]);

	printf("-----------------\n");
}

/******************************************************************************/

#endif /* TEST */
