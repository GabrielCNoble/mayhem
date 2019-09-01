#ifndef ENT_H
#define ENT_H

#include "../common/gmath/vector.h"
#include "../common/gmath/matrix.h"
#include "phy_common.h"
#include "mdl.h"

enum ENT_COMPONENT_TYPE
{
    ENT_COMPONENT_TYPE_TRANSFORM = 0,
    ENT_COMPONENT_TYPE_MODEL,
    ENT_COMPONENT_TYPE_PHYSICS,
    ENT_COMPONENT_TYPE_NET,
    ENT_COMPONENT_TYPE_NONE
};

struct component_handle_t
{
    unsigned type : 4;
    unsigned def : 1;
    unsigned index : 27;
};

#define ENT_COMPONENT_HANDLE(type, def, index) (struct component_handle_t){type, def, index}
#define ENT_INVALID_COMPONENT_INDEX 0x07ffffff
#define ENT_INVALID_COMPONENT_HANDLE ENT_COMPONENT_HANDLE(ENT_COMPONENT_TYPE_NONE, 0, ENT_INVALID_COMPONENT_INDEX)




struct entity_handle_t
{
    unsigned def : 1;
    unsigned index : 31;
};

#define ENT_ENTITY_HANDLE(def, index)(struct entity_handle_t){def, index}
#define ENT_INVALID_ENTITY_INDEX 0x7fffffff
#define ENT_INVALID_ENTITY_HANDLE ENT_ENTITY_HANDLE(1, ENT_INVALID_ENTITY_INDEX)




struct base_component_t
{
    unsigned int type;
    struct entity_handle_t owner_entity;
};

struct transform_component_t
{
    struct base_component_t base;
    mat3_t orientation;
    vec3_t position;
};

struct model_component_t
{
    struct base_component_t base;
    struct model_handle_t model;
};

struct physics_component_t
{
    struct base_component_t base;
    struct collider_handle_t collider;
};

struct net_component_t
{
    struct base_component_t base;
};

enum ENT_ENTITY_FLAGS
{
    ENT_ENTITY_FLAG_INVALID = 1,
};

struct entity_t
{
    int flags;
    struct component_handle_t components[ENT_COMPONENT_TYPE_NONE];
    char *name;
};

#ifdef __cplusplus
extern "C"
{
#endif

void ent_Init();

void ent_Shutdown();

struct entity_handle_t ent_CreateEntity(int def);

struct entity_handle_t ent_CreateEntityInstance();

struct entity_handle_t ent_CreateEntityDef();

void ent_DestroyEntity(struct entity_handle_t entity_handle);

struct entity_t *ent_GetEntityPointer(struct entity_handle_t entity_handle);

vec3_t ent_GetEntityPosition(struct entity_handle_t entity_handle);




struct component_handle_t ent_AllocComponent(int type, int def);

void ent_DeallocComponent(struct component_handle_t component_handle);

void ent_AddComponent(struct entity_handle_t entity_handle, int type);

void ent_RemoveComponent(struct entity_handle_t entity_handle, int type);

struct base_component_t *ent_GetComponentPointer(struct component_handle_t component_handle);

void ent_AddPhysicsComponent(struct entity_handle_t entity_handle, int collider_type);

void ent_AddModelComponent(struct entity_handle_t entity_handle, struct model_handle_t model_handle);

void ent_AddNetComponent(struct entity_handle_t entity_handle);




void ent_UpdateTransformComponents();

void ent_UpdatePhysicsComponents();

void ent_UpdateNetComponents();



void ent_GetEntityListPointer(struct entity_t **entities, int *count, int def);

#ifdef __cplusplus
}
#endif


#endif // ENT_H






