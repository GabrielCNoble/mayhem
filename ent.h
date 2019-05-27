#ifndef ENT_H
#define ENT_H

#include "vector.h"
#include "matrix.h"
#include "handle.h"



enum ENT_COMPONENT_TYPE
{
    ENT_COMPONENT_TYPE_TRANSFORM = 0,
//    ENT_COMPONENT_TYPE_MODEL,
    ENT_COMPONENT_TYPE_NONE
};

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

struct entity_t
{
    struct component_handle_t components[ENT_COMPONENT_TYPE_NONE];
};


void ent_Init();

void ent_Shutdown();

struct entity_handle_t ent_CreateEntity(int def);

void ent_DestroyEntity(struct entity_handle_t entity_handle);



struct component_handle_t ent_AllocComponent(int type);

void ent_DeallocComponent(struct component_handle_t component_handle);

struct base_component_t *ent_GetComponentPointer(struct component_handle_t component_handle);

#endif // ENT_H






