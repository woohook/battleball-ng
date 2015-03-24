
/*
 *      File Name: bsp.h
 *      Description: include file for the bsp tree module
 *	Author: A. T. Campbell, III
 *
 */

#ifndef BSP_H
#define BSP_H

/* include files */
#include <stdio.h>
#include "bspvector.h"
#include "polygon.h"

/* constants */
#define BSP_DEF_CAND	7			/* default # of separator
						 * candidates 
						 */

typedef enum {					/* traversal order */
	BSP_TRAV_BACK_TO_FRONT,
	BSP_TRAV_FRONT_TO_BACK
	} BSP_TRAV_TYPE;

typedef enum {					/* type of node */
	BSP_NODE_INTERNAL,
	BSP_NODE_IN,
	BSP_NODE_OUT,
	BSP_NODE_NONSOLID
	} BSP_NODE_TYPE;

typedef enum {					/* type of tree */
	BSP_TREE_SOLID,
	BSP_TREE_SPACE_DIVISION,
	BSP_TREE_VISIBILITY
	} BSP_TREE_TYPE;

typedef enum {					/* set operations */
	BSP_SETOP_DIFFERENCE,
	BSP_SETOP_INTERSECTION,
	BSP_SETOP_SYMMETRIC_DIFFERENCE,
	BSP_SETOP_UNION
	} BSP_SETOP_TYPE;

typedef struct BSP	{			/* node of a bsp tree */
		float		coeffs[4];	/* planar coefficients */
		int		numpolys;	/* number of embedded polygons*/
		Polygon3f	**polys;	/* polygons in this node */ 
		BSP_NODE_TYPE	type;		/* INTERNAL or leaf class */
		struct BSP	*parent;	/* parent node */
		struct BSP	*positive;	/* positive children */
		struct BSP	*negative;	/* negative children */
	} BSP;

/* return values */
extern BSP	*bsp_build(int n, Polygon3f *list[], BSP_TREE_TYPE tree_type);
extern void	bsp_copy(BSP *src, BSP *dest);
extern void 	bsp_destroy(BSP *tree);
extern void	bsp_draw(BSP *tree, Vec3f eyePos, 
		void (*drawPolygon)(Polygon3f *p));
extern int 	bsp_height(BSP *tree);
extern int 	bsp_is_cell(BSP *tree);
extern BSP 	*bsp_merge(BSP *t1, BSP *t2, BSP_SETOP_TYPE op);
extern BSP      *bsp_node_create(float coeffs[4], BSP_NODE_TYPE node_type);
extern int	bsp_num_nodes(BSP *tree);
extern int	bsp_num_polys(BSP *tree);
extern void	bsp_print(FILE *fp, BSP *tree);
extern void	bsp_transform(BSP *tree, float mat[4][4]);
extern void 	bsp_traverse_nodes(BSP_TRAV_TYPE tflag, Vec3f viewpos, 
		BSP *tree, int *n, BSP *list[]);
extern void 	bsp_traverse_polys(BSP_TRAV_TYPE tflag, Vec3f viewpos, 
		BSP *tree, int *n, Polygon3f ***list);
extern Polygon3f** bsp_traverse_polys2(BSP *t, Vec3f vp, Polygon3f **list);
extern Polygon3f** bsp_traverse_polys3(BSP *t, Vec3f vp, Polygon3f **list);
extern void 	bsp_translate(BSP *tree, Vec3f delta);
extern int 	bsp_contains(BSP *tree, Vec3f pt, int);
extern void	bsp_printf(BSP *t,int level);
extern void	bsp_bbox(BSP *t, Vec3f low, Vec3f high);

#endif /* BSP_H */
