#ifndef PHYSICS_H
#define PHYSICS_H


#include "phy_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

void phy_Init();

void phy_Shutdown();

struct collider_handle_t phy_CreateCollider(int type);

struct collider_handle_t phy_CreateStaticCollider();

void phy_DestroyCollider(struct collider_handle_t collider_handle);

struct base_collider_t *phy_GetColliderPointer(struct collider_handle_t collider);






struct player_collider_t *phy_GetPlayerColliderPointer(struct collider_handle_t collider);

struct portal_collider_t *phy_GetPortalColliderPointer(struct collider_handle_t collider);

struct static_collider_t *phy_GetStaticColliderPointer(struct collider_handle_t collider);

struct rigid_body_collider_t *phy_GetRigidBodyColliderPointer(struct collider_handle_t collider);




struct collision_shape_handle_t phy_AllocCollisionShape(int shape);

void phy_FreeCollisionShape(struct collision_shape_handle_t shape);

struct base_shape_t *phy_GetCollisionShapePointer(struct collision_shape_handle_t shape);


void phy_Step();



/*
============================================
============================================
============================================
*/


void phy_Jump(struct collider_handle_t collider);

void phy_Move(struct collider_handle_t collider, vec3_t acceleration);



/*
============================================
============================================
============================================
*/


int phy_AllocDbvhNode();

void phy_DeallocDbvhNode(int node_index);

struct dbvh_node_t *phy_GetDbvhNodePointer(int node_index);

void phy_GenerateColliderCollisionPairs(struct collider_handle_t collider);

void phy_GenerateCollisionPairs();

void phy_DbvhNodesVolume(int node_a, int node_b, vec3_t *max, vec3_t *min);


/*
============================================
============================================
============================================
*/


void phy_ClearBvh(struct bvh_node_t *node);

struct bvh_node_t *phy_BuildBvhRecursive(struct bvh_node_t *nodes);

//void phy_BuildBvh(vec3_t *vertices, int vertice_count);

void phy_AddStaticTriangles(struct collider_handle_t static_collider, vec3_t *vertices, int count);

void phy_StaticColliderBvh(struct collider_handle_t static_collider);



/*
============================================
============================================
============================================
*/

vec3_t phy_ClosestPointOnTriangle(struct collision_triangle_t *triangle, vec3_t point);

struct list_t *phy_BoxOnBvh(vec3_t &aabb_max, vec3_t &aabb_min, struct bvh_node_t *nodes);

struct list_t *phy_FindContactPointsPlayerStatic(struct collider_handle_t player_collider, struct collider_handle_t static_collider);

struct list_t *phy_FindContactPointsRigidStatic(struct collider_handle_t rigid_body_collider, struct collider_handle_t static_collider);

struct list_t *phy_FindContactPointsPlayerPortal(struct collider_handle_t player_collider, struct collider_handle_t portal_collider);




void phy_Collide(struct collider_handle_t collider_a, struct collider_handle_t collider_b);

//void phy_CollidePlayerWorld(struct collider_handle_t player_collider);

void phy_CollidePlayerStatic(struct collider_handle_t player_collider, struct collider_handle_t static_collider);

void phy_CollidePlayerPortal(struct collider_handle_t player_collider, struct collider_handle_t portal_collider);

void phy_ColliderRigidStatic(struct collider_handle_t rigid_collider, struct collider_handle_t static_collider);



#ifdef __cplusplus
}
#endif



#endif // PHYSICS_H




