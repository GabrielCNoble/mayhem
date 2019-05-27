#include "ent.h"
#include "stack_list.h"

#include <stdio.h>

struct stack_list_t ent_components[2][ENT_COMPONENT_TYPE_NONE];


struct stack_list_t ent_entities[2];


void ent_Init()
{
    ent_components[0][ENT_COMPONENT_TYPE_TRANSFORM].init(sizeof(struct transform_component_t), 32);
    ent_components[1][ENT_COMPONENT_TYPE_TRANSFORM].init(sizeof(struct transform_component_t), 32);

    ent_entities[0].init(sizeof(struct entity_t), 32);
    ent_entities[1].init(sizeof(struct entity_t), 32);
}

void ent_Shutdown()
{

}

struct entity_handle_t ent_CreateEntity(int def)
{
    struct entity_handle_t handle = INVALID_ENTITY_HANDLE;
    struct entity_t *entity;

    def = def && 1;

    handle.def = def;
    handle.index = ent_entities[def].add(NULL);

    entity = (struct entity_t* )ent_entities[def].get(handle.index);

    for(int i = ENT_COMPONENT_TYPE_TRANSFORM; i < ENT_COMPONENT_TYPE_NONE; i++)
    {
        entity->components[i] = INVALID_COMPONENT_HANDLE;
    }

    return handle;
}

void ent_DestroyEntity(struct entity_handle_t entity_handle)
{

}

struct entity_t *ent_GetEntityPointer(struct entity_handle_t entity_handle)
{
    struct entity_t *entity = NULL;

    if(entity_handle.index != INVALID_ENTITY_INDEX)
    {
        entity = (struct entity_t* )ent_entities[entity_handle.def].get(entity_handle.index);

        if(entity)
        {

        }
    }

    return entity;
}

struct component_handle_t ent_AllocComponent(int type, int def)
{
    struct component_handle_t handle = INVALID_COMPONENT_HANDLE;
    struct base_component_t *component;

    if(type >= ENT_COMPONENT_TYPE_TRANSFORM && type < ENT_COMPONENT_TYPE_NONE)
    {
        def = def && 1;

        handle.type = type;
        handle.index = ent_components[def][type].add(NULL);

        component = (struct base_component_t* )ent_components[def][type].get(handle.index);
        component->type = type;
        component->owner_entity = INVALID_ENTITY_HANDLE;
    }

    return handle;
}

void ent_DeallocComponent(struct component_handle_t component_handle)
{

}

struct base_component_t *ent_GetComponentPointer(struct component_handle_t component_handle)
{
    struct base_component_t *component = NULL;

    if(component_handle.type >= ENT_COMPONENT_TYPE_TRANSFORM && component_handle.type < ENT_COMPONENT_TYPE_NONE)
    {
        component = (struct base_component_t* )ent_components[component_handle.def][component_handle.type].get(component_handle.index);

        if(component)
        {
            if(component->type == ENT_COMPONENT_TYPE_NONE)
            {
                component = NULL;
            }
        }
    }

    return component;
}














