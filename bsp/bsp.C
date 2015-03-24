
/*
 *	File Name: 	bsp.c
 *	Description:	BSP tree routines
 *	Author:		A. T. Campbell,III
 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "brep.h"
#include "bsp.h"
#include "polygon.h"
#include "bspvector.h"

/*****************************************************************************/

/* constants */
#ifndef TRUE
#define FALSE		0
#define TRUE		1
#endif /* TRUE */

#ifndef LARGE
#define LARGE 	(1.0e6)
#endif

#define MAXCAND		20

/*****************************************************************************/

/* macros */
#undef MIN
#undef SIGN
#define MIN(A, B)	(((A) < (B))? (A) : (B))
#define SIGN(X)		(((X) < 0)? -1 : 1)

/*****************************************************************************/

/* local routines */
static void 	bi_partition(float pcoeffs[], int pn, Vec3f pv[],
        	float tcoeffs[], int tn, Vec3f tv[], int *pstat, int *tstat,
        	BSP *part, BSP *tree);
static void 	bi_partition_fixup(float pcoeffs[], int pn, Vec3f pv[],
        	float tcoeffs[], int tn, Vec3f tv[], int *pstat, int *tstat);
static void	bsp_brep_calc(BSP *tree, int *n, Vec3f v[]);
static void 	bsp_brep_context_refine(BSP *context, int *n, Vec3f v[]);
static BSP	*bsp_build_tree(int n, Polygon3f *list[], int candmax,
		int solid,
		int (*costfunc)(Polygon3f *, int, Polygon3f **));
static void	bsp_compress(BSP *tree);
static void	bsp_node_copy(BSP *src, BSP *dest);
static Polygon3f *bsp_separator(int n, Polygon3f *list[], 
		int candmax,
		int (*costfunc)(Polygon3f *, int, Polygon3f **));
static void 	bsp_partition(BSP *tree, BSP *part,
        	int pn, Vec3f pv[], BSP **pos, BSP **neg);
static BSP	*bsp_setop_compliment(BSP *tree);
static BSP	*bsp_setop_difference(BSP *t1, BSP *t2);
static BSP	*bsp_setop_intersection(BSP *t1, BSP *t2);
static BSP	*bsp_setop_symmetric_difference(BSP *t1, BSP *t2);
static BSP	*bsp_setop_union(BSP *t1, BSP *t2);
static BSP	*cell_merge(BSP *t1, BSP *t2, BSP_SETOP_TYPE op);
static int 	count_cuts(Polygon3f *cutter, int n, Polygon3f *list[]);
static int 	count_imbalance(Polygon3f *cutter, int n, Polygon3f *list[]);
static void	split_polys(Polygon3f *sep, float coeffs[], int numpolys, 
		Polygon3f *polys[],
		int *numpos, Polygon3f *pos[], int *numneg, Polygon3f *neg[],
		int *numon, Polygon3f *on[]); 


/*****************************************************************************/

static void bsp_faces_forward(BSP* tree) {
  if (tree) {
    for (int i= 0; i <tree->numpolys; i++) {
      Polygon3f *p= tree->polys[i];
      p->facesForward= Vdot3f(tree->coeffs,p->plane_normal) >= 0.0;
    }
    bsp_faces_forward(tree->positive);
    bsp_faces_forward(tree->negative);
  }
}


BSP *
bsp_build(int n, Polygon3f *list[], BSP_TREE_TYPE tree_type)
{
	BSP	*tree;
	int	i;

	/* setup calculations */
	for (i = 0; i < n; i++)
		PolygonNormal3f(list[i], list[i]->plane_normal);

	if (tree_type == BSP_TREE_VISIBILITY)
		tree = bsp_build_tree(n, list, BSP_DEF_CAND, FALSE, count_cuts);
	else if (tree_type == BSP_TREE_SPACE_DIVISION)
		tree = bsp_build_tree(n, list, BSP_DEF_CAND, FALSE, 
			count_imbalance);
	else if (tree_type == BSP_TREE_SOLID)
		tree = bsp_build_tree(n, list, BSP_DEF_CAND, TRUE, 
			count_imbalance);
	else
		tree = NULL;
	bsp_faces_forward(tree);
	return tree;
}


/*****************************************************************************/

static BSP *
bsp_build_tree(int n, Polygon3f *list[], int candmax, int solid,
	int (*costfunc)(Polygon3f *, int, Polygon3f **))
{
	static float leafcoeffs[4] 		/* leaf coefficients */
		= {0., 0., 0., 0.};
	Polygon3f *sep;				/* separating polygon */
	BSP	*root;				/* root of the tree */
	float	coeffs[4];			/* planar eq of splitter */
	Polygon3f	**mon, **mpos, **mneg;
	int	i, npos, nneg, non;

	/* trivial cases */
	if (n <= 0 || list == NULL)
		return(bsp_node_create(leafcoeffs, BSP_NODE_NONSOLID));

	/* get separator */
	sep = bsp_separator(n, list, candmax, costfunc);

	/* create root node */
	Vcopy3f(sep->plane_normal, coeffs);
	coeffs[3] = -Vdot3f(sep->plane_normal, sep->verts[0]);
	root = bsp_node_create(coeffs, BSP_NODE_INTERNAL);
	
	/* create arrays of indices */ 
	mon = (Polygon3f **) malloc(n * sizeof(Polygon3f *));
	mpos = (Polygon3f **) malloc(n * sizeof(Polygon3f *));
	mneg = (Polygon3f **) malloc(n * sizeof(Polygon3f *));
	non = npos = nneg = 0;

	/* split polygons */
	split_polys(sep, coeffs, n, list, &npos, mpos, &nneg, mneg,
		&non, mon);

	/* append polygons to root node */
	root->numpolys = non;
	if (non > 0) {
		root->polys = (Polygon3f **) malloc(non*sizeof(Polygon3f *));
		for (i = 0; i < non; i++) {
			root->polys[i] = mon[i];
		}
	}

	/* recursively build rest of tree */
	root->positive = bsp_build_tree(npos, mpos, candmax, solid, costfunc);
	root->negative = bsp_build_tree(nneg, mneg, candmax, solid, costfunc);
	if (root->positive) root->positive->parent = root;
	if (root->negative) root->negative->parent = root;

	/* set leaf nodes */
	if (solid) {
		if (npos == 0)
			root->positive->type = BSP_NODE_OUT;
		if (nneg == 0)
			root->negative->type = BSP_NODE_IN;
	}

	/* free temporary arrays */
	if (mpos != NULL) free((char *)mpos);
	if (mneg != NULL) free((char *)mneg);
	if (mon != NULL) free((char *)mon);

	return(root);
}

/*****************************************************************************/

static void	
split_polys(Polygon3f *sep, float coeffs[], int numpolys, Polygon3f *polys[],
	int *numpos, Polygon3f *pos[], int *numneg, Polygon3f *neg[],
	int *numon, Polygon3f *on[])
{
	int		i; 
	int		np, nn;			/* sizes of lists */
	Polygon3f 	*posarr[2], *negarr[2];	/* partitioned lists */
	Polygon3f	*p;
	E_side		side;
	int		maxd;
	Vec3f		pnt;

	/* find a point on the plane */
	maxd = 0;			/* find the largest dimension */
	for (i = 0; i < 3; i++)
		if (fabs(coeffs[i]) > fabs(coeffs[maxd]))
			maxd = i;
	Vset3f(0., 0., 0., pnt);	/* set all but maxd'th component to 0 */
	pnt[maxd] = -coeffs[3]/coeffs[maxd];

	/* separate polygons into positive, negative, and on lists */
	*numpos = *numneg = *numon = 0;
	for (i = 0; i < numpolys; i++) {
		p = polys[i];
		side = PolygonSidePlane3f(p, pnt, coeffs);
		if (p == sep) {
			on[*numon] = p;
			(*numon)++;
		}
		else if (side == sideNeither) {
			on[*numon] = p;
			(*numon)++;
		}
		else if (side == sidePositive) {
			pos[*numpos] = p;
			(*numpos)++;
		}
		else if (side == sideNegative) {
			neg[*numneg] = p;
			(*numneg)++;
		}
		else if (side == sideBoth) {
			PolygonCutConvex3f(p, pnt, coeffs, 2, &np, posarr, 
				&nn, negarr);
			if (np > 0) {
				pos[*numpos] = posarr[0];
				(*numpos)++;
			}
			if (nn > 0) {
				neg[*numneg] = negarr[0];
				(*numneg)++;
			}
		}
	}
}

/*****************************************************************************/

static Polygon3f *
bsp_separator(int n, Polygon3f *list[], int candmax,
	int (*costfunc)(Polygon3f *, int, Polygon3f **))
{
	int	candid[MAXCAND];		/* indices of candidates */
	Polygon3f *cand[MAXCAND];		/* list of candidates */
	Polygon3f *leadcand;			/* leading candidate */
	int	mincost;			/* minimum cost */
	int	cost;				/* current cost */
	int	nc;				/* number of candidates */
	int	i, j;				/* loop control */
	int	r;				/* random index */
	
	/* trivial cases */
	if (n <= 0) return(NULL);
	if (n == 1) return(list[0]);
	if (candmax <= 1) return(list[0]);

	/* get candidate indices */
	nc = MIN(n, MAXCAND);			/* limit # of candidates */
	nc = MIN(nc, candmax);			/* limit # of candidates */
	for (i = 0; i < nc; i++) {
		r = rand() % n;			/* select a random index */
		for (j = i; j && candid[j-1] > r; j--)
			candid[j] = candid[j-1];
		candid[j] = r;			/* insert in sorted order */
	}	

	/* get candidates */
	for (i = 0; i < nc; i++) {		/* search in sorted order */
		cand[i] = list[candid[i]];
	}

	/* determine which polygon will have least cost */
	mincost = costfunc(cand[0], n, list);
	leadcand = cand[0];
	for (i = 1; i < nc; i++) {
		cost = costfunc(cand[i], n, list);
		if (cost < mincost) {
			mincost = cost;
			leadcand = cand[i];
		}
	}
	return(leadcand);
}

/*****************************************************************************/

static int
count_cuts(Polygon3f *cutter, int n, Polygon3f *list[])	
				/* count cuts caused by partition */
{
	int	ncuts;
	int	i;

	ncuts = 0;
	for (i = 0; i < n; i++)
		ncuts += (PolygonSidePlane3f(list[i], cutter->verts[0],
			  cutter->plane_normal) == sideBoth);
	return(ncuts);
}

/*****************************************************************************/

static int
count_imbalance(Polygon3f *cutter, int n, Polygon3f *list[])	
				/* count imbalance caused by partition */
{
	int	i, npos, nneg, diff;
	E_side 	side;

	npos = nneg = 0;
	for (i = 0; i < n; i++) {
		side = PolygonSidePlane3f(list[i], cutter->verts[0],
			  cutter->plane_normal);
		if (side == sidePositive) 
			npos++;
		else if (side == sideNegative) 
			nneg++;
	}
	diff = npos - nneg;
	if (diff < 0) 
		diff = -diff;
	return(diff);
}

/*****************************************************************************/

static void
bsp_brep_calc(BSP *tree, int *n, Vec3f v[])
{
	brep_init(tree->coeffs, n, v);
	bsp_brep_context_refine(tree, n, v);
}

/*****************************************************************************/

static void
bsp_brep_context_refine(BSP *context, int *n, Vec3f v[])
{
	BSP  	*pp;
        int     dir;

        for (pp = context; pp != NULL && pp->parent != NULL; pp = pp->parent) {
                dir = ((pp == pp->parent->positive)? 1: -1);
                brep_refine(pp->parent->coeffs, n, v, dir, UNIVEPS);
        }
}

/*****************************************************************************/

static void
bsp_compress(BSP *tree)
{
	if (!bsp_is_cell(tree)) {
                bsp_compress(tree->positive);
                bsp_compress(tree->negative);
                if (bsp_is_cell(tree->positive) && 
		    bsp_is_cell(tree->negative) &&
		    (	 (tree->positive->type == BSP_NODE_IN &&
			  tree->negative->type == BSP_NODE_IN) ||
			 (tree->positive->type == BSP_NODE_OUT &&
			  tree->negative->type == BSP_NODE_OUT))	) {
                        tree->type = tree->positive->type;
                        Vset4f(0., 0., 0., 0., tree->coeffs);
                        bsp_destroy(tree->negative);
                        bsp_destroy(tree->positive);
			tree->negative = tree->positive = NULL;
		}
        }
}

/*****************************************************************************/

void
bsp_translate(BSP *t, Vec3f delta)
{ int i;
  
  if (t != NULL) {
    t->coeffs[3] -= Vdot3f(delta,t->coeffs);
    for (i= 0; i <t->numpolys; i++)
      PolygonTranslate3f(t->polys[i],delta);
    bsp_translate(t->positive,delta);
    bsp_translate(t->negative,delta);
  }
}

/*****************************************************************************/

void
bsp_copy(BSP *src, BSP *dest)
{
	static float leafcoeffs[4] 		/* leaf coefficients */
		= {0., 0., 0., 0.};

	if (src != NULL && dest != NULL) {
		bsp_node_copy(src, dest);
		if (src->positive != NULL) {
			dest->positive = bsp_node_create(leafcoeffs, 
				src->positive->type);
			bsp_copy(src->positive, dest->positive);
			dest->positive->parent = dest;
		}
		if (src->negative != NULL) {
			dest->negative = bsp_node_create(leafcoeffs, 
				src->negative->type);
			bsp_copy(src->negative, dest->negative);
			dest->negative->parent = dest;
		}
	}
}

/*****************************************************************************/

void
bsp_destroy(BSP *tree)
{
	int	i;

	if (tree != NULL) {
		bsp_destroy(tree->positive);
		bsp_destroy(tree->negative);
		for (i = 0; i < tree->numpolys; i++)
			PolygonDestroy3f(tree->polys[i]);
		if (tree->polys != NULL)
			free((char *) tree->polys);
		free((char *) tree);
	}
}

/*****************************************************************************/

void
bsp_draw(BSP *tree, Vec3f eyePos, void (*drawPolygon)(Polygon3f *p))
{
	float value, backfaceValue;
	int i;
	Polygon3f *q;

	if (tree != NULL) {
		value = tree->coeffs[0] * eyePos[0] +
			tree->coeffs[1] * eyePos[1] +
			tree->coeffs[2] * eyePos[2] + tree->coeffs[3];
		/* Draw stuff further away */
		if (value < 0.0)
			bsp_draw(tree->positive, eyePos, drawPolygon);
		else
			bsp_draw(tree->negative, eyePos, drawPolygon);	
		
		/* Draw current stuff */
		for (i=0; i < tree->numpolys; i++) {
			q = tree->polys[i];
			backfaceValue = q->plane_normal[0] * eyePos[0] +
					q->plane_normal[1] * eyePos[1] +
					q->plane_normal[2] * eyePos[2] 
					- Vdot3f(q->plane_normal, q->verts[0]);
			if (backfaceValue > 0.)
				drawPolygon(q);
		}
			
		/* Draw closer stuff */
		if (value < 0.0)
			bsp_draw(tree->negative, eyePos, drawPolygon);	
		else
			bsp_draw(tree->positive, eyePos, drawPolygon);
	}
}

/*****************************************************************************/

int 
bsp_height(BSP *tree)
{
	int	htpos, htneg, ht;

	if (tree == NULL)
		ht = 0;
	else {
		htpos = bsp_height(tree->positive);
		htneg = bsp_height(tree->negative);
		if (htpos > htneg)
			ht = 1 + htpos;
		else
			ht = 1 + htneg;
	}
	return ht;
}

/*****************************************************************************/

int 
bsp_is_cell(BSP *tree)
{
	int	val;

	if (tree == NULL) 
		val = 0;
	else
		val = (tree->positive == NULL && tree->negative == NULL);
	return val;
}

/*****************************************************************************/

/* merge bsp trees t1 and t2.  this routine will destroy the original trees */
BSP	*
bsp_merge(BSP *t1, BSP *t2, BSP_SETOP_TYPE op)
{
	BSP	*postree, *negtree, *val;
	int	pn;
	Vec3f	pv[BREPMAXVERTS];

	if (bsp_is_cell(t1) || bsp_is_cell(t2))
		val = cell_merge(t1, t2, op);
	else {
		/* split t2 with the partitioner of t1 */
		bsp_brep_calc(t1, &pn, pv);
		bsp_partition(t2, t1, pn, pv, &postree, &negtree);

		/* recursively merge pieces with t1's subtrees */
		val = bsp_node_create(t1->coeffs, BSP_NODE_INTERNAL);
		bsp_node_copy(t1, val);
		val->positive = bsp_merge(t1->positive, postree, op);
		val->positive->parent = val;
		val->negative = bsp_merge(t1->negative, negtree, op);
		val->negative->parent = val;

		/* free storage */
		t1->positive = t1->negative = (BSP *)NULL;
		bsp_destroy(t1);
	}
	bsp_compress(val);
	return(val);
}

/*****************************************************************************/

static BSP *
cell_merge(BSP *t1, BSP *t2, BSP_SETOP_TYPE op)
{
	if (op == BSP_SETOP_DIFFERENCE)
		return(bsp_setop_difference(t1, t2));
	else if (op == BSP_SETOP_INTERSECTION)
		return(bsp_setop_intersection(t1, t2));
	else if (op == BSP_SETOP_SYMMETRIC_DIFFERENCE)
		return(bsp_setop_symmetric_difference(t1, t2));
	else if (op == BSP_SETOP_UNION) 
		return(bsp_setop_union(t1, t2));
	else
		return NULL;
}

/*****************************************************************************/

/* create compliment of a tree */
static BSP *
bsp_setop_compliment(BSP *tree)
{
	int	i;

	if (bsp_is_cell(tree)) {	
		if (tree->type == BSP_NODE_OUT)
			tree->type = BSP_NODE_IN;
		else if (tree->type == BSP_NODE_IN)
			tree->type = BSP_NODE_OUT;
		for (i = 0; i < tree->numpolys; i++)
			Vtimes3f(-1., (tree->polys[i])->plane_normal, 
				(tree->polys[i])->plane_normal);
	}
	else {
		tree->positive = bsp_setop_compliment(tree->positive);
		tree->negative = bsp_setop_compliment(tree->negative);
	}
	return tree;
}

/*****************************************************************************/

/* compute difference between two trees */
static BSP *
bsp_setop_difference(BSP *t1, BSP *t2)
{
	BSP	*val;

	if (bsp_is_cell(t1)) {	
		if (t1->type == BSP_NODE_IN) {
			val = bsp_setop_compliment(t2);
			bsp_destroy(t1);
		}
		else if (t1->type == BSP_NODE_OUT) {
			val = t1;
			bsp_destroy(t2);
		}
		else if (t1->type == BSP_NODE_NONSOLID) {
			val = t1;
			bsp_destroy(t2);
		}
	}
	else {
		if (t2->type == BSP_NODE_IN) {
			val = bsp_setop_compliment(t1);
			bsp_destroy(t2);
		}
		else if (t2->type == BSP_NODE_OUT) {
			val = t2;
			bsp_destroy(t1);
		}
		else if (t2->type == BSP_NODE_NONSOLID) {
			val = t2;
			bsp_destroy(t1);
		}
	}
	return(val);
}

/*****************************************************************************/

/* perform intersection of a cell with a tree. the original trees may be 
   destroyed */
static BSP *
bsp_setop_intersection(BSP *t1, BSP *t2)
{
	BSP	*val;

	if (bsp_is_cell(t1)) {
		if (t1->type == BSP_NODE_IN) {
			val = t2;
			bsp_destroy(t1);
		}
		else if (t1->type == BSP_NODE_OUT) {
			val = t1;
			bsp_destroy(t2);
		}
		else if (t1->type == BSP_NODE_NONSOLID) {
			val = t1;
			bsp_destroy(t2);
		}
	}
	else {
		if (t2->type == BSP_NODE_IN) {
			val = t1;
			bsp_destroy(t2);
		}
		else if (t2->type == BSP_NODE_OUT) {
			val = t2;
			bsp_destroy(t1);
		}
		else if (t2->type == BSP_NODE_NONSOLID) {
			val = t2;
			bsp_destroy(t1);
		}
	}
	return(val);
}

/*****************************************************************************/

/* compute symmetric difference between two trees */
static BSP *
bsp_setop_symmetric_difference(BSP *t1, BSP *t2)
{
	BSP	*val;

	if (bsp_is_cell(t1)) {	
		if (t1->type == BSP_NODE_IN) {
			val = bsp_setop_compliment(t2);
			bsp_destroy(t1);
		}
		else if (t1->type == BSP_NODE_OUT) {
			val = t2;
			bsp_destroy(t1);
		}
		else if (t1->type == BSP_NODE_NONSOLID) {
			val = t1;
			bsp_destroy(t2);
		}
	}
	else {
		if (t2->type == BSP_NODE_IN) {
			val = bsp_setop_compliment(t1);
			bsp_destroy(t2);
		}
		else if (t2->type == BSP_NODE_OUT) {
			val = t1;
			bsp_destroy(t2);
		}
		else if (t2->type == BSP_NODE_NONSOLID) {
			val = t2;
			bsp_destroy(t1);
		}
	}
	return(val);
}

/*****************************************************************************/

/* perform union of a cell with a tree.  the original trees may be destroyed */
static BSP *
bsp_setop_union(BSP *t1, BSP *t2)
{
	BSP	*val;

	if (bsp_is_cell(t1)) {
		if (t1->type == BSP_NODE_IN) {
			val = t1;
			bsp_destroy(t2);
		}
		else if (t1->type == BSP_NODE_OUT) {
			val = t2;
			bsp_destroy(t1);
		}
		else if (t1->type == BSP_NODE_NONSOLID) {
			val = t2;
			bsp_destroy(t1);
		}
	}
	else {
		if (t2->type == BSP_NODE_IN) {
			val = t2;
			bsp_destroy(t1);
		}
		else if (t2->type == BSP_NODE_OUT) {
			val = t1;
			bsp_destroy(t2);
		}
		else if (t2->type == BSP_NODE_NONSOLID) {
			val = t1;
			bsp_destroy(t2);
		}
	}
	return(val);
}

/*****************************************************************************/

/* partition a BSP tree. the original tree will be destroyed */
static void
bsp_partition(BSP *tree, BSP *part, int pn, Vec3f pv[],
	BSP **pos, BSP **neg)
{
	static float leafcoeffs[4] 		/* leaf coefficients */
		= {0., 0., 0., 0.};
	int	treestat, partstat;
	int	i, tn, pposn, pnegn;
	Vec3f	tv[BREPMAXVERTS], pposv[BREPMAXVERTS], pnegv[BREPMAXVERTS];
	Polygon3f **copylist, **onlist;
	int	numon;

	if (bsp_is_cell(tree)) { /* leaf node */
		*pos = bsp_node_create(leafcoeffs, tree->type);
		*neg = bsp_node_create(leafcoeffs, tree->type);
		bsp_destroy(tree);
	}
	else {
		/* bi-partition planes */
		bsp_brep_calc(tree, &tn, tv);
		bsp_brep_context_refine(part, &tn, tv);
		bi_partition(part->coeffs, pn, pv, tree->coeffs, tn, tv, 
			&partstat, &treestat, part, tree);

		/* assign children to subtrees */
		if (partstat == PINBOTH && treestat == PINBOTH) {
			*pos = bsp_node_create(tree->coeffs, BSP_NODE_INTERNAL);
			*neg = bsp_node_create(tree->coeffs, BSP_NODE_INTERNAL);

			/* copy embedded polygons */
			if (tree->numpolys > 0) {
				(*pos)->polys = (Polygon3f **) 
				malloc(tree->numpolys * sizeof(Polygon3f *));
				(*neg)->polys = (Polygon3f **) 
				malloc(tree->numpolys * sizeof(Polygon3f *));
				copylist = (Polygon3f **) 
				malloc(tree->numpolys * sizeof(Polygon3f *));
				onlist = (Polygon3f **) 
				malloc(tree->numpolys * sizeof(Polygon3f *));
				for (i = 0; i < tree->numpolys; i++) {
					copylist[i] = PolygonCreate3f();
					PolygonCopy3f(tree->polys[i],
						copylist[i]);
				}
				split_polys(NULL, part->coeffs, tree->numpolys, 
					copylist, 
					&((*pos)->numpolys), (*pos)->polys,
					&((*neg)->numpolys), (*neg)->polys,
					&numon, onlist);
			}
			brep_split(tree->coeffs, pn, pv, UNIVEPS, &pposn, pposv,
				&pnegn, pnegv);
			bsp_partition(tree->negative, part, pnegn, pnegv,
				&((*pos)->negative), &((*neg)->negative));
			bsp_partition(tree->positive, part, pposn, pposv,
				&((*pos)->positive), &((*neg)->positive));
		}
		else if (partstat == PINPOS && treestat == PINNEG) {
			*neg = bsp_node_create(tree->coeffs, BSP_NODE_INTERNAL);
			bsp_node_copy(tree, *neg);
			(*neg)->negative = tree->negative;
			bsp_partition(tree->positive, part, pn, pv,
				pos, &((*neg)->positive));
		}
		else if (partstat == PINPOS && treestat == PINPOS) {
			*pos = bsp_node_create(tree->coeffs, BSP_NODE_INTERNAL);
			bsp_node_copy(tree, *pos);
			(*pos)->negative = tree->negative;
			bsp_partition(tree->positive, part, pn, pv,
				&((*pos)->positive), neg);
		}
		else if (partstat == PINNEG && treestat == PINNEG) {
			*neg = bsp_node_create(tree->coeffs, BSP_NODE_INTERNAL);
			bsp_node_copy(tree, *neg);
			(*neg)->positive = tree->positive;
			bsp_partition(tree->negative, part, pn, pv,
				pos, &((*neg)->negative));
		}
		else if (partstat == PINNEG && treestat == PINPOS) {
			*pos = bsp_node_create(tree->coeffs, BSP_NODE_INTERNAL);
			bsp_node_copy(tree, *pos);
			(*pos)->positive = tree->positive;
			bsp_partition(tree->negative, part, pn, pv,
				&((*pos)->negative), neg);
		}
		else if (partstat == PON && treestat == PON) {
			/* copy embedded polygons */
			if ((part->numpolys + tree->numpolys) > 0) {
				onlist = (Polygon3f **) 
				malloc((tree->numpolys + part->numpolys)
					* sizeof(Polygon3f *));
				numon = 0;
				for (i = 0; i < part->numpolys; i++, numon++)
					onlist[numon] = part->polys[i];
				for (i = 0; i < tree->numpolys; i++, numon++){
					onlist[numon] = PolygonCreate3f();
					PolygonCopy3f(tree->polys[i], 
						onlist[numon]);
				}
				if (part->polys) free((char *) (part->polys));
				part->polys = onlist;
				part->numpolys = numon;
			}
			if (Vdot3f(part->coeffs, tree->coeffs) > 0.) {
				/* parallel */
				*neg = tree->negative;
				*pos = tree->positive;
			}
			else {
				/* anti-parallel */
				*neg = tree->positive;
				*pos = tree->negative;
			}
		}
		else {
			fprintf(stderr, "Partition: UNKNOWN pairing\n");
			return;
		}
	
		/* set parents */
		(*neg)->parent = (*pos)->parent = NULL;
		if (*pos && (*pos)->positive) ((*pos)->positive)->parent = *pos;
		if (*pos && (*pos)->negative) ((*pos)->negative)->parent = *pos;
		if (*neg && (*neg)->positive) ((*neg)->positive)->parent = *neg;
		if (*neg && (*neg)->negative) ((*neg)->negative)->parent = *neg;
	
		/* compress resultant trees */
		bsp_compress(*pos);
		bsp_compress(*neg);
	
		/* free storage */
		tree->positive = tree->negative = (BSP *)NULL;
		bsp_destroy(tree);
	}
}

/*****************************************************************************/

static void
bi_partition(float pcoeffs[], int pn, Vec3f pv[], float tcoeffs[], 
	int tn, Vec3f tv[], int *pstat, int *tstat,
	BSP *part, BSP *tree)
{
	/* determine spatial relationship */
	*pstat = brep_partition(pn, pv, tcoeffs, UNIVEPS);
	*tstat = brep_partition(tn, tv, pcoeffs, UNIVEPS);

	/* check if this is an allowed configuration */
	if (*pstat == PINPOS && *tstat == PINPOS) return;
	if (*pstat == PINPOS && *tstat == PINNEG) return;
	if (*pstat == PINNEG && *tstat == PINPOS) return;
	if (*pstat == PINNEG && *tstat == PINNEG) return;
	if (*pstat == PINBOTH && *tstat == PINBOTH) return;
	if (*pstat == PON && *tstat == PON) return;

	bi_partition_fixup(pcoeffs, pn, pv, tcoeffs, tn, tv, pstat, tstat);
}

/*****************************************************************************/

static void
bi_partition_fixup(float pcoeffs[], int pn, Vec3f pv[], float tcoeffs[], 
	int tn, Vec3f tv[], int *pstat, int *tstat)
{
	float	max, min, dist;
	int	i;

	/* correct degenerate configurations */
	if ((*pstat == PINPOS) || (*pstat == PINNEG)) {
		if (*tstat == PON) {
			if (Vdot3f(tcoeffs, pcoeffs) > 0) 
				*tstat = (*pstat == PINPOS)? PINNEG: PINPOS;
			else
				*tstat = *pstat;
		}
		else if (*tstat == PINBOTH) {
			min = LARGE;
			max = -LARGE;
			for (i = 0; i < tn; i++) {
				dist = Vdot3f(pcoeffs, tv[i]) + pcoeffs[3];
				if (dist < min) min = dist;
				if (dist > max) max = dist;
			}
			*tstat = (fabs(min) > fabs(max))? PINNEG: PINPOS;
		}
		else
			fprintf(stderr, "fixup called erroneously\n");
	}
	else if (*pstat == PON) {
		if (*tstat == PINBOTH)
			*tstat = PON;
		else {		/* *tstat == PINNEG or PINPOS */
			if (Vdot3f(tcoeffs, pcoeffs) > 0) 
				*pstat = (*tstat == PINPOS)? PINNEG: PINPOS;
			else
				*pstat = *tstat;
		}
	}
	else { 	/* (*pstat == PINBOTH) */
		if (*tstat == PON) 
			*pstat = PON;
		else { 		/* *tstat == PINPOS or PINNEG */
			min = LARGE;
			max = -LARGE;
			for (i = 0; i < pn; i++) {
				dist = Vdot3f(tcoeffs, pv[i]) + tcoeffs[3];
				if (dist < min) min = dist;
				if (dist > max) max = dist;
			}
			*pstat = (fabs(min) > fabs(max))? PINNEG: PINPOS;
		}
	}
}

/*****************************************************************************/

static void
bsp_node_copy(BSP *src, BSP *dest)
{
	int i;

	Vcopy4f(src->coeffs, dest->coeffs);
	dest->type = src->type;
	dest->numpolys = src->numpolys;
	if (dest->numpolys > 0) {
		dest->polys = (Polygon3f **) 
				malloc(dest->numpolys * sizeof(Polygon3f *));
		for (i = 0; i < dest->numpolys; i++) {
			dest->polys[i] = PolygonCreate3f();
			PolygonCopy3f(src->polys[i], dest->polys[i]);
		}
	}
	else
		dest->polys = NULL;
}

/*****************************************************************************/

BSP *
bsp_node_create(float coeffs[4], BSP_NODE_TYPE node_type)
{
	BSP	*b;

	b = (BSP *) malloc(sizeof(BSP));
	if (!b) fprintf(stderr, "Out of memory in bsp_node_create()\n");
	Vcopy4f(coeffs, b->coeffs);
	b->numpolys = 0;
	b->polys = NULL;
	b->type = node_type;
	b->parent = b->positive = b->negative = NULL;
	return(b);
}

/*****************************************************************************/

int
bsp_num_nodes(BSP *tree)
{
	if (!tree) return(0);
	return(1 + bsp_num_nodes(tree->positive) + 
		bsp_num_nodes(tree->negative));
}

/*****************************************************************************/

int
bsp_num_polys(BSP *tree)
{
	if (!tree) return(0);
	return(tree->numpolys + bsp_num_polys(tree->positive) + 
		bsp_num_polys(tree->negative));
}

/*****************************************************************************/

void	
bsp_print(FILE *fp, BSP *tree)
{
	int	i;
	if (!tree) return;
	fprintf(fp, "BSP coeffs: %f %f %f %f\n", tree->coeffs[0], 
		tree->coeffs[1], tree->coeffs[2], tree->coeffs[3]); 
	fprintf(fp, "BSP polys:\n");
	for (i = 0; i < tree->numpolys; i++)
		PolygonPrint3f(fp, tree->polys[i]);
	bsp_print(fp, tree->positive);
	bsp_print(fp, tree->negative);
}

/*****************************************************************************/

void
bsp_traverse_nodes(BSP_TRAV_TYPE tflag, Vec3f viewpos, BSP *tree, 
	int *n, BSP *list[])
{
	int	t;  // is this really supposed to be an int? -PAH
	int	nf, nb;
	int	l, r;
	BSP	*tmp;
	
	if (!tree) {
		*n = 0;
	}
	else {
		t = (int) (Vdot3f(viewpos, tree->coeffs) + tree->coeffs[3]);
		if (t > 0) {
			bsp_traverse_nodes(tflag, viewpos, tree->positive, 
				&nf, list);
			list[nf] = tree;
			bsp_traverse_nodes(tflag, viewpos, tree->negative, 
				&nb, &(list[nf+1]));
		}
		else {
			bsp_traverse_nodes(tflag, viewpos, tree->negative, 
				&nf, list);
			list[nf] = tree;
			bsp_traverse_nodes(tflag, viewpos, tree->positive, 
				&nb, &(list[nf+1]));
		}
		*n = nb + nf + 1;
		if (tflag == BSP_TRAV_BACK_TO_FRONT) {
			for (l = 0, r = *n - 1; l < r; l++, r--) {
				tmp = list[l];
				list[l] = list[r];
				list[r] = tmp;
			}
		}
	}
}

/*****************************************************************************/

/* sort polygons in tree */	
void
bsp_traverse_polys(BSP_TRAV_TYPE tflag, Vec3f viewpos, BSP *tree, int *n, 
	Polygon3f ***list)
{
	int	t;  // is this really supposed to be an int? -PAH
	Polygon3f	**fl, **bl;
	int	i, j, nf, nb;
	Polygon3f	**sp, **dp;
	
	if (!tree) {
		*n = 0;
		*list = NULL;
	}
	else {
		t = (int) (Vdot3f(viewpos, tree->coeffs) + tree->coeffs[3]);
		if (t > 0) {
			bsp_traverse_polys(tflag, viewpos, tree->negative, 
			&nb, &bl);
			bsp_traverse_polys(tflag, viewpos, tree->positive, 
			&nf, &fl);
		}
		else {
			bsp_traverse_polys(tflag, viewpos, tree->positive, 
			&nb, &bl);
			bsp_traverse_polys(tflag, viewpos, tree->negative, 
			&nf, &fl);
		}
		*n = nb + nf + tree->numpolys;
		*list = (Polygon3f **) 
				malloc((unsigned)((*n)*sizeof(Polygon3f *)));
		if (!list) fprintf(stderr, "Out of memory in bsp_traverse()\n");
		dp = *list;
		if (tflag == BSP_TRAV_BACK_TO_FRONT) {
			for (sp = bl, i = 0; i < nb; i++, sp++, dp++)
				*dp = *sp;
			for (j = 0; j < tree->numpolys; j++)
				*dp++ = tree->polys[j];
			for (sp = fl, i = 0; i < nf; i++, sp++, dp++)
				*dp = *sp;
		}
		else {
			for (sp = fl, i = 0; i < nf; i++, sp++, dp++)
				*dp = *sp;
			for (j = 0; j < tree->numpolys; j++)
				*dp++ = tree->polys[j];
			for (sp = bl, i = 0; i < nb; i++, sp++, dp++)
				*dp = *sp;
		}
		if (bl) free((char *)bl);
		if (fl) free((char *)fl);
	}
}

 
/*****************************************************************************/
/* This function executes faster than bsp_traverse_polys().
   In:  list = an array big enough to hold ptrs to all the polys in the tree
   Out: list = contains ptrs to the tree's polys in back-to-front order
   Returns a ptr past the last (Polygon3f*) written into list.
*/
Polygon3f **bsp_traverse_polys2(BSP *tree, Vec3f viewpos, Polygon3f **list) {
  int i;
  if (tree) {
    if (Vdot3f(viewpos, tree->coeffs) +tree->coeffs[3] >0) {
      // polys in negative subtree are farthest back
      list= bsp_traverse_polys2(tree->negative,viewpos,list);
      for (i=0; i <tree->numpolys; i++)
	*(list++)= tree->polys[i];
      list= bsp_traverse_polys2(tree->positive,viewpos,list);
    }
    else {
      // polys in positive subtree are farthest back
      list= bsp_traverse_polys2(tree->positive,viewpos,list);
      for (i=0; i <tree->numpolys; i++)
        *(list++)= tree->polys[i];
      list= bsp_traverse_polys2(tree->negative,viewpos,list);
    }
  }
  return list;
}


Polygon3f **bsp_traverse_polys3(BSP *tree, Vec3f viewpos, Polygon3f **list) {
  static BSP* nodeStack[1000];
  static BSP* nextStack[1000];
  int  size= 0;     // stack size

  while (TRUE) {
    if (tree==NULL) {
      if (size==0) return list;
      size--;
      BSP *node= nodeStack[size];
      for (int i=0; i <node->numpolys; i++)
	*(list++)= node->polys[i];
      tree= nextStack[size];

    } else {

      nodeStack[size]= tree;
      if (Vdot3f(viewpos, tree->coeffs) > -tree->coeffs[3]) {
	nextStack[size]= tree->positive;
	tree= tree->negative;
      } else {
	nextStack[size]= tree->negative;
	tree= tree->positive;
      }
      size++;
    }
  }
}

/*****************************************************************************/

/* In:  tree = BSP tree
        pt   = point needing "in" or "out" classification
        on_surface_means_containment = 
	  true ->  surface of the BSP tree is considered part of the tree
	  false -> surface of the BSP tree is not considered part of the tree
	  
   Returns true if the tree contains the point pt, otherwise returns false.
   Assumes tree is of type BSP_TREE_SOLID
*/

int bsp_contains(BSP *tree, Vec3f pt, int on_surface_means_containment) {
  float side;
  int   neg_contains,
        pos_contains;

  if (tree->type==BSP_NODE_INTERNAL) {
    side= Vdot3f(tree->coeffs,pt) + tree->coeffs[3];
    if (side > 0.)
      return bsp_contains(tree->positive,pt,on_surface_means_containment);
    else if (side < 0.)
      return bsp_contains(tree->negative,pt,on_surface_means_containment);
    else {
      pos_contains= bsp_contains(tree->positive,pt,on_surface_means_containment);
      neg_contains= bsp_contains(tree->negative,pt,on_surface_means_containment);
      if (pos_contains==neg_contains)
	return pos_contains;
      else
	return on_surface_means_containment;
    }
  }
  else
    return (tree->type==BSP_NODE_IN);
}

/*****************************************************************************/

/* In:  tree = BSP tree
        pt1  = beginning point
        pt2  = ending point
   Out: p    = ptr to the penetrated Polygon3f, or NULL if no penetrated
               Polygon3f was found.

   Finds which polygon in the BSP tree is penetrated by a path from pt1 to
   pt2.
   Assumes that the number of polygons which both face into the path and
   are penetrated by the path is no more than 1.
   Assumes that polygons in the tree are convex.
*/

void bsp_find_penetrated(BSP *tree, Vec3f pt1, Vec3f pt2, Polygon3f *p) {
  int i;

  if (tree->type==BSP_NODE_INTERNAL) {
    for (i=0; i <tree->numpolys; i++) {
      p= tree->polys[i];
      /*if (PolygonPenetrates3f(p,pt1,pt2)) return; -PAH */
    }
    bsp_find_penetrated(tree->positive,pt1,pt2,p);
    if (p==NULL)
      bsp_find_penetrated(tree->positive,pt1,pt2,p);
  }
  else
    p= NULL;
}

/*****************************************************************************/

void bsp_printf(BSP *t,int level) {
  int i;

  if (t==NULL) return;
  for(i=0; i<level; i++) printf("  ");
  printf("%d: num=%d type=%d %.2f %.2f %.2f %.2f ",
         level,t->numpolys,t->type,
         t->coeffs[0], t->coeffs[1], t->coeffs[2], t->coeffs[3]);

  if (t->positive && t->positive->parent != t)
    printf("bad_pos_parent ");
  if (t->negative && t->negative->parent != t)
    printf("bad_neg_parent ");

  for (i=0; i<t->numpolys; i++)
    printf("%ld ", (t->polys[i]->material_index) % 100000);

  if (t->positive)
    if (t->negative)
      printf("children\n");
    else
      printf("positive\n");
  else
    if (t->negative)
      printf("negative\n");
    else
      printf("\n");
  bsp_printf(t->positive,level+1);
  bsp_printf(t->negative,level+1);
}

/*****************************************************************************/

void bsp_bbox2(BSP *t, Vec3f low, Vec3f high, int *bboxHasBeenInit) {
  Vec3f plow,phigh;
  int i;

  if (t==NULL) return;
  if (*bboxHasBeenInit)
    for(i=0; i <t->numpolys; i++) {
      PolygonBBox3f(t->polys[i],plow,phigh);
      Vmin3f(plow,low,low);
      Vmax3f(phigh,high,high);
    }
  else
    if (t->numpolys >0) {
      PolygonBBox3f(t->polys[0],low,high);
      *bboxHasBeenInit= 1;
      for(i=1; i <t->numpolys; i++) {
	PolygonBBox3f(t->polys[i],plow,phigh);
	Vmin3f(plow,low,low);
	Vmax3f(phigh,high,high);
      }
    }
  bsp_bbox2(t->positive,low,high,bboxHasBeenInit);
  bsp_bbox2(t->negative,low,high,bboxHasBeenInit);
}


void bsp_bbox(BSP *t, Vec3f low, Vec3f high) {
  int bboxHasBeenInit= 0;
  bsp_bbox2(t,low,high,&bboxHasBeenInit);
}
