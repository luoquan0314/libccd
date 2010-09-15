/***
 * libgjk
 * ---------------------------------
 * Copyright (c)2010 Daniel Fiser <danfis@danfis.cz>
 *
 *
 *  This file is part of libgjk.
 *
 *  libgjk is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation; either version 3 of
 *  the License, or (at your option) any later version.
 *
 *  libgjk is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GJK_POLYTOPE_H__
#define __GJK_POLYTOPE_H__

#include <stdlib.h>
#include <gjk/support.h>
#include <gjk/list.h>
#include <gjk/alloc.h>
#include <gjk/dbg.h>// TODO: remove this!

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GJK_PT_VERTEX 1
#define GJK_PT_EDGE   2
#define GJK_PT_FACE   3


#define __GJK_PT_EL \
    int type;           /*! type of element */ \
    double dist;        /*! distance from origin */ \
    gjk_vec3_t witness; /*! witness point of projection of origin */ \
    gjk_list_t list;    /*! list of elements of same type */

/**
 * General polytope element.
 * Could be vertex, edge or triangle.
 */
struct _gjk_pt_el_t {
    __GJK_PT_EL
};
typedef struct _gjk_pt_el_t gjk_pt_el_t;

struct _gjk_pt_edge_t;
struct _gjk_pt_face_t;

/**
 * Polytope's vertex.
 */
struct _gjk_pt_vertex_t {
    __GJK_PT_EL

    int id;
    gjk_support_t v;
    gjk_list_t edges; //!< List of edges
};
typedef struct _gjk_pt_vertex_t gjk_pt_vertex_t;

/**
 * Polytope's edge.
 */
struct _gjk_pt_edge_t {
    __GJK_PT_EL

    gjk_pt_vertex_t *vertex[2]; //!< Reference to vertices
    struct _gjk_pt_face_t *faces[2]; //!< Reference to faces

    gjk_list_t vertex_list[2]; //!< List items in vertices' lists
};
typedef struct _gjk_pt_edge_t gjk_pt_edge_t;

/**
 * Polytope's triangle faces.
 */
struct _gjk_pt_face_t {
    __GJK_PT_EL

    gjk_pt_edge_t *edge[3]; //!< Reference to surrounding edges
};
typedef struct _gjk_pt_face_t gjk_pt_face_t;


/**
 * Struct containing polytope.
 */
struct _gjk_pt_t {
    gjk_list_t vertices; //!< List of vertices
    gjk_list_t edges; //!< List of edges
    gjk_list_t faces; //!< List of faces

    gjk_pt_el_t *nearest;
    double nearest_dist;
    int nearest_type;
};
typedef struct _gjk_pt_t gjk_pt_t;


void gjkPtInit(gjk_pt_t *pt);
void gjkPtDestroy(gjk_pt_t *pt);

/**
 * Returns vertices surrounding given triangle face.
 */
_gjk_inline void gjkPtFaceVec3(const gjk_pt_face_t *face,
                               gjk_vec3_t **a,
                               gjk_vec3_t **b,
                               gjk_vec3_t **c);
_gjk_inline void gjkPtFaceVertices(const gjk_pt_face_t *face,
                                   gjk_pt_vertex_t **a,
                                   gjk_pt_vertex_t **b,
                                   gjk_pt_vertex_t **c);
_gjk_inline void gjkPtFaceEdges(const gjk_pt_face_t *f,
                                gjk_pt_edge_t **a,
                                gjk_pt_edge_t **b,
                                gjk_pt_edge_t **c);

_gjk_inline void gjkPtEdgeVec3(const gjk_pt_edge_t *e,
                               gjk_vec3_t **a,
                               gjk_vec3_t **b);
_gjk_inline void gjkPtEdgeVertices(const gjk_pt_edge_t *e,
                                   gjk_pt_vertex_t **a,
                                   gjk_pt_vertex_t **b);
_gjk_inline void gjkPtEdgeFaces(const gjk_pt_edge_t *e,
                                gjk_pt_face_t **f1,
                                gjk_pt_face_t **f2);


/**
 * Adds vertex to polytope and returns pointer to newly created vertex.
 */
gjk_pt_vertex_t *gjkPtAddVertex(gjk_pt_t *pt, const gjk_support_t *v);
_gjk_inline gjk_pt_vertex_t *gjkPtAddVertexCoords(gjk_pt_t *pt,
                                                  double x, double y, double z);

/**
 * Adds edge to polytope.
 */
gjk_pt_edge_t *gjkPtAddEdge(gjk_pt_t *pt, gjk_pt_vertex_t *v1,
                                          gjk_pt_vertex_t *v2);

/**
 * Adds face to polytope.
 */
gjk_pt_face_t *gjkPtAddFace(gjk_pt_t *pt, gjk_pt_edge_t *e1,
                                          gjk_pt_edge_t *e2,
                                          gjk_pt_edge_t *e3);

/**
 * Deletes vertex from polytope.
 * Returns 0 on success, -1 otherwise.
 */
_gjk_inline int gjkPtDelVertex(gjk_pt_t *pt, gjk_pt_vertex_t *);
_gjk_inline int gjkPtDelEdge(gjk_pt_t *pt, gjk_pt_edge_t *);
_gjk_inline int gjkPtDelFace(gjk_pt_t *pt, gjk_pt_face_t *);


/**
 * Recompute distances from origin for all elements in pt.
 */
void gjkPtRecomputeDistances(gjk_pt_t *pt);

/**
 * Returns nearest element to origin.
 */
gjk_pt_el_t *gjkPtNearest(gjk_pt_t *pt);


void gjkPtDumpSVT(gjk_pt_t *pt, const char *fn);
void gjkPtDumpSVT2(gjk_pt_t *pt, FILE *);


/**** INLINES ****/
_gjk_inline gjk_pt_vertex_t *gjkPtAddVertexCoords(gjk_pt_t *pt,
                                                  double x, double y, double z)
{
    gjk_support_t s;
    gjkVec3Set(&s.v, x, y, z);
    return gjkPtAddVertex(pt, &s);
}

_gjk_inline int gjkPtDelVertex(gjk_pt_t *pt, gjk_pt_vertex_t *v)
{
    // test if any edge is connected to this vertex
    if (!gjkListEmpty(&v->edges))
        return -1;

    // delete vertex from main list
    gjkListDel(&v->list);

    if ((void *)pt->nearest == (void *)v){
        pt->nearest = NULL;
    }

    free(v);
    return 0;
}

_gjk_inline int gjkPtDelEdge(gjk_pt_t *pt, gjk_pt_edge_t *e)
{
    // text if any face is connected to this edge (faces[] is always
    // aligned to lower indices)
    if (e->faces[0] != NULL)
        return -1;

    // disconnect edge from lists of edges in vertex struct
    gjkListDel(&e->vertex_list[0]);
    gjkListDel(&e->vertex_list[1]);

    // disconnect edge from main list
    gjkListDel(&e->list);

    if ((void *)pt->nearest == (void *)e){
        pt->nearest = NULL;
    }

    free(e);
    return 0;
}

_gjk_inline int gjkPtDelFace(gjk_pt_t *pt, gjk_pt_face_t *f)
{
    gjk_pt_edge_t *e;
    size_t i;

    // remove face from edges' recerence lists
    for (i = 0; i < 3; i++){
        e = f->edge[i];
        if (e->faces[0] == f){
            e->faces[0] = e->faces[1];
        }
        e->faces[1] = NULL;
    }

    // remove face from list of all faces
    gjkListDel(&f->list);

    if ((void *)pt->nearest == (void *)f){
        pt->nearest = NULL;
    }

    free(f);
    return 0;
}

_gjk_inline void gjkPtFaceVec3(const gjk_pt_face_t *face,
                               gjk_vec3_t **a,
                               gjk_vec3_t **b,
                               gjk_vec3_t **c)
{
    *a = &face->edge[0]->vertex[0]->v.v;
    *b = &face->edge[0]->vertex[1]->v.v;

    if (face->edge[1]->vertex[0] != face->edge[0]->vertex[0]
            && face->edge[1]->vertex[0] != face->edge[0]->vertex[1]){
        *c = &face->edge[1]->vertex[0]->v.v;
    }else{
        *c = &face->edge[1]->vertex[1]->v.v;
    }
}

_gjk_inline void gjkPtFaceVertices(const gjk_pt_face_t *face,
                                   gjk_pt_vertex_t **a,
                                   gjk_pt_vertex_t **b,
                                   gjk_pt_vertex_t **c)
{
    *a = face->edge[0]->vertex[0];
    *b = face->edge[0]->vertex[1];

    if (face->edge[1]->vertex[0] != face->edge[0]->vertex[0]
            && face->edge[1]->vertex[0] != face->edge[0]->vertex[1]){
        *c = face->edge[1]->vertex[0];
    }else{
        *c = face->edge[1]->vertex[1];
    }
}

_gjk_inline void gjkPtFaceEdges(const gjk_pt_face_t *f,
                                gjk_pt_edge_t **a,
                                gjk_pt_edge_t **b,
                                gjk_pt_edge_t **c)
{
    *a = f->edge[0];
    *b = f->edge[1];
    *c = f->edge[2];
}

_gjk_inline void gjkPtEdgeVec3(const gjk_pt_edge_t *e,
                               gjk_vec3_t **a,
                               gjk_vec3_t **b)
{
    *a = &e->vertex[0]->v.v;
    *b = &e->vertex[1]->v.v;
}

_gjk_inline void gjkPtEdgeVertices(const gjk_pt_edge_t *e,
                                   gjk_pt_vertex_t **a,
                                   gjk_pt_vertex_t **b)
{
    *a = e->vertex[0];
    *b = e->vertex[1];
}

_gjk_inline void gjkPtEdgeFaces(const gjk_pt_edge_t *e,
                                gjk_pt_face_t **f1,
                                gjk_pt_face_t **f2)
{
    *f1 = e->faces[0];
    *f2 = e->faces[1];
}


#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __GJK_POLYTOPE_H__ */
