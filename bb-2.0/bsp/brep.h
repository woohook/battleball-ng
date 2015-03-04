
/*
 *	File:		brep.h
 *	Description:	include file for brep merging
 *	Author:		A. T. Campbell, II
 *
 */

#ifndef BREP_H
#define BREP_H

/******************************************************************************/

/* include files */
#include "bspvector.h"

/******************************************************************************/

/* constants */
#define UNIVSZ		(1.0e3)	/* size of universe */
#define UNIVEPS		(1.0e-3)  /* smallest piece of universe */

#define BREPMAXVERTS	32	/* maximum number of vertices */

/* partitioning values */
enum {	PINNEG, PINPOS, PINBOTH, PON};

/******************************************************************************/

/* return values */
extern void	brep_copy(int sn, Vec3f sv[], int *dn, Vec3f dv[]);
extern void	brep_init(float coeffs[4], int *n, Vec3f v[]);
extern int 	brep_partition(int n, Vec3f v[], float coeffs[], float eps);
extern void	brep_refine(float coeffs[4], int *n, Vec3f v[], int dir,
		float eps);
extern void	brep_split(float coeffs[4], int n, Vec3f v[], float eps,
			int *npos, Vec3f vpos[], int *nneg, Vec3f vneg[]);

/******************************************************************************/

#endif /* BREP_H */
