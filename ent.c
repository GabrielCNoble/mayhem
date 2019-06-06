#include "ent.h"
#include "stack_list.h"
#include "physics.h"

#include <stdio.h>

struct stack_list_t ent_components[2][ENT_COMPONENT_TYPE_NONE];


struct stack_list_t ent_entities[2];


void ent_Init()
{
    ent_components[0][ENT_COMPONENT_TYPE_TRANSFORM].init(sizeof(struct transform_component_t), 32);
    ent_components[0][ENT_COMPONENT_TYPE_PHYSICS].init(sizeof(struct physics_component_t), 32);
    ent_components[0][ENT_COMPONENT_TYPE_MODEL].init(sizeof(struct model_component_t), 32);

    ent_components[1][ENT_COMPONENT_TYPE_TRANSFORM].init(sizeof(struct transform_component_t), 32);
    ent_components[1][ENT_COMPONENT_TYPE_PHYSICS].init(sizeof(struct physics_component_t), 32);
    ent_components[1][ENT_COMPONENT_TYPE_MODEL].init(sizeof(struct model_component_t), 32);

    ent_entities[0].init(sizeof(struct entity_t), 32);
    ent_entities[1].init(sizeof(struct entity_t), 32);
}

void ent_Shutdown()
{

}

struct entity_handle_t ent_CreateEntity(int def)
{
    struct entity_handle_t handle = ENT_INVALID_ENTITY_HANDLE;
    struct entity_t *entity;

    def = def && 1;

    handle.def = def;
    handle.index = ent_entities[def].add(NULL);

    entity = (struct entity_t* )ent_entities[def].get(handle.index);

    entity->flags = 0;

    for(int i = ENT_COMPONENT_TYPE_TRANSFORM; i < ENT_COMPONENT_TYPE_NONE; i++)
    {
        entity->components[i] = ENT_INVALID_COMPONENT_HANDLE;
    }

    return handle;
}


struct entity_handle_t ent_CreateEntityInstance()
{
    return ent_CreateEntity(0);
}

struct entity_handle_t ent_CreateEntityDef()
{
    return ent_CreateEntity(1);
}

void ent_DestroyEntity(struct entity_handle_t entity_handle)
{
    struct entity_t *entity;

    entity = ent_GetEntityPointer(entity_handle);

    if(entity)
    {
        entity->flags = ENT_ENTITY_FLAG_INVALID;

        for(int i = 0; i < ENT_COMPONENT_TYPE_NONE; i++)
        {
            ent_DeallocComponent(entity->components[i]);
        }

        ent_entities[entity_handle.def].remove(entity_handle.index);
    }
}


struct entity_t *ent_GetEntityPointer(struct entity_handle_t entity_handle)
{
    struct entity_t *entity = NULL;

    if(entity_handle.index != ENT_INVALID_ENTITY_INDEX)
    {
        entity = (struct entity_t* )ent_entities[entity_handle.def].get(entity_handle.index);

        if(entity && (entity->flags & ENT_ENTITY_FLAG_INVALID))
        {
            entity = NULL;
        }
    }

    return entity;
}

struct component_handle_t ent_AllocComponent(int type, int def)
{
    struct component_handle_t handle = ENT_INVALID_COMPONENT_HANDLE;
    struct base_component_t *component;

    if(type >= ENT_COMPONENT_TYPE_TRANSFORM && type < ENT_COMPONENT_TYPE_NONE)
    {
        def = def && 1;

        handle.type = type;
        handle.index = ent_components[def][type].add(NULL);

        component = (struct base_component_t* )ent_components[def][type].get(handle.index);
        component->type = type;
        component->owner_entity = ENT_INVALID_ENTITY_HANDLE;
    }

    return handle;
}

void ent_DeallocComponent(struct component_handle_t component_handle)
{
    struct base_component_t *component;
    struct physics_component_t *physics_component;

    component = ent_GetComponentPointer(component_handle);

    if(component)
    {
        switch(component->type)
        {
            case ENT_COMPONENT_TYPE_PHYSICS:
                physics_component = (struct physics_component_t* )component;

                if(physics_component->collider.index != PHY_INVALID_COLLIDER_INDEX)
                {

                }
            break;
        }
        component->type = ENT_COMPONENT_TYPE_NONE;
        ent_components[component_handle.def][component_handle.type].remove(component_handle.index);
    }
}

void ent_AddComponent(struct entity_handle_t entity_handle, int type)
{
    struct entity_t *entity;
    struct base_component_t *base_component;
    struct component_handle_t component_handle;

    entity = ent_GetEntityPointer(entity_handle);

    if(entity)
    {
        component_handle = ent_AllocComponent(type, entity_handle.def);

        base_component = ent_GetComponentPointer(component_handle);
        base_component->owner_entity = entity_handle;

        if(base_component)
        {
            switch(type)
            {
                case ENT_COMPONENT_TYPE_TRANSFORM:

                break;

                case ENT_COMPONENT_TYPE_PHYSICS:

                break;
            }

            entity->components[type] = component_handle;
        }
    }
}

void ent_RemoveComponent(struct entity_handle_t entity_handle, int type)
{
    struct entity_t *entity;
    struct base_component_t *base_component;
    struct physics_component_t *physics_component;


    entity = ent_GetEntityPointer(entity_handle);

    if(entity)
    {
        base_component = ent_GetComponentPointer(entity->components[type]);

        if(base_component)
        {
            switch(type)
            {
                case ENT_COMPONENT_TYPE_PHYSICS:
                    physics_component = (struct physics_component_t *)base_component;
                    phy_DestroyCollider(physics_component->collider);
                break;
            }

            ent_DeallocComponent(entity->components[type]);
        }
    }
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





void ent_UpdateTransformComponents()
{

}

void ent_UpdatePhysicsComponents()
{
    int i;
    int c;
    struct physics_component_t *physics_components;
    struct physics_component_t *physics_component;
    struct transform_component_t *transform_component;
    struct base_collider_t *base_collider;
    struct entity_t *entity;

    physics_components = (struct physics_component_t *)ent_components[0][ENT_COMPONENT_TYPE_PHYSICS].buffer;
    c = ent_components[0][ENT_COMPONENT_TYPE_PHYSICS].cursor;

    for(i = 0; i < c; i++)
    {
        physics_component = physics_components + i;

        if(physics_component->base.type == ENT_COMPONENT_TYPE_NONE)
        {
            continue;
        }

        base_collider = phy_GetColliderPointer(physics_component->collider);
        entity = ent_GetEntityPointer(physics_component->base.owner_entity);

        if(entity)
        {
            transform_component = (struct transform_component_t *)ent_GetComponentPointer(entity->components[ENT_COMPONENT_TYPE_TRANSFORM]);
            transform_component->position = base_collider->position;
        }
    }
}











