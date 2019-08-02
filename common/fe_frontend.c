#include "fe_frontend.h"
#include "be_backend.h"
#include "input.h"
#include "r_view.h"
#include "r_verts.h"
#include "r_common.h"
#include "r_shader.h"
#include "r_portal.h"
#include "r_light.h"
#include "r_vis.h"
#include "s_aux.h"
#include "w_main.h"

#include "player.h"
#include "physics.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern struct renderer_t r_renderer;
extern struct list_t w_world_batches;

int fe_Frontend(void *data)
{
    //struct view_t *view;
    //struct draw_batch_t *batch;
    //struct geometry_data_t level_data;
    struct player_handle_t player;

  //  struct draw_command_buffer_t cmd_buffer;
  //  struct draw_command_t draw_cmd;

 //   struct vertex_t *vertices;
 //   struct verts_handle_t handle;

   // int shd = fe_LoadShader("shaders/test");

    player = player_CreatePlayer("default", vec3_t(-3.0, 1.0, 0.0), vec3_t(0.0, 0.3, 0.0));
    player_SetActivePlayer(player);
//
//    player_CreatePlayer("default1", vec3_t(4.0, 0.22, 0.0));
//    player_CreatePlayer("default2", vec3_t(-3.0, 0.22, -4.0));
//    player_CreatePlayer("default2", vec3_t(-3.0, 2.22, 5.0));

    in_RegisterKey(SDL_SCANCODE_W);
    in_RegisterKey(SDL_SCANCODE_S);
    in_RegisterKey(SDL_SCANCODE_A);
    in_RegisterKey(SDL_SCANCODE_D);
    in_RegisterKey(SDL_SCANCODE_E);
    in_RegisterKey(SDL_SCANCODE_SPACE);
    in_RegisterKey(SDL_SCANCODE_ESCAPE);
    in_RegisterKey(SDL_SCANCODE_LCTRL);

    r_CreateLight(vec3_t(5.0, 0.9, 0.0), vec3_t(1.0, 1.0, 1.0), 0.2, 20.0);
    r_CreateLight(vec3_t(6.0, 1.5, 6.0), vec3_t(1.0, 1.0, 1.0), 0.2, 20.0);
    r_CreateLight(vec3_t(0.0, 4.2, 2.0), vec3_t(1.0, 1.0, 1.0), 0.2, 20.0);
    r_CreateLight(vec3_t(-5.0, 1.2, -3.0), vec3_t(0.0, 0.2, 0.4), 0.3, 20.0);
    r_CreateLight(vec3_t(0.0, 8.2, 1.5), vec3_t(1.0, 0.2, 0.4), 0.3, 20.0);
    r_CreateLight(vec3_t(0.0, -12.0, 0.0), vec3_t(1.0, 1.0, 1.0), 0.3, 20.0);
    r_CreateLight(vec3_t(-20.0, -2.0, 0.0), vec3_t(1.0, 1.0, 1.0), 0.3, 20.0);

    #define LEVEL_7

    #if defined LEVEL_6

    w_LoadLevel("test6.obj");

    int portal0 = r_CreatePortal(vec3_t(0.0, 1.0, -0.8), vec2_t(1.5, 2.2));
    int portal1 = r_CreatePortal(vec3_t(0.0, -5.23, -7.6), vec2_t(1.5, 2.2));
    r_LinkPortals(portal0, portal1);

    int portal2 = r_CreatePortal(vec3_t(0.0, -5.23, 7.6), vec2_t(1.5, 2.2));
    int portal3 = r_CreatePortal(vec3_t(0.0, 1.0, 0.8), vec2_t(1.5, 2.2));
    r_LinkPortals(portal2, portal3);

    struct portal_t *portal;

    portal = r_GetPortalPointer(portal1);
    portal->orientation = rotate_y(1.0);

    portal = r_GetPortalPointer(portal3);
    portal->orientation = rotate_y(1.0);

    #elif defined LEVEL_7
//
//    w_LoadLevel("../models/test5.gmy");
    w_LoadLevel("../models/phys_test6.gmy");
    struct model_handle_t cube = mdl_LoadModel("../models/monkey.gmy");
//
//
//    r_LoadTexture("../pk02_ceiling02_C.tga");
//
//    r_LoadTexture("../../../textures/pk02/pk02_ceiling02_C.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_ceiling02_I.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_ceiling02_N.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_ceiling02_S.tga");
//
//    r_LoadTexture("../../../textures/pk02/pk02_ceiling03_C.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_ceiling03_I.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_ceiling03_N.tga");
//
//    r_LoadTexture("../../../textures/pk02/pk02_computer01a_C.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_computer01a_I.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_computer01a_N.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_computer01a_S.tga");
//
//    r_LoadTexture("../../../textures/pk02/pk02_computer01b_C.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_computer01b_I.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_computer01b_N.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_computer01b_S.tga");
//
//    r_LoadTexture("../../../textures/pk02/pk02_computer01c_C.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_computer01c_I.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_computer01c_N.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_computer01c_S.tga");
//
//    r_LoadTexture("../../../textures/pk02/pk02_dec_num00.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_dec_num01.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_dec_num02.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_dec_num03.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_dec_num04.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_dec_num05.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_dec_num06.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_dec_num07.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_dec_num08.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_dec_num09.tga");
//
//    r_LoadTexture("../../../textures/pk02/pk02_door01_C.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_door01_N.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_door01_S.tga");
//
//    r_LoadTexture("../../../textures/pk02/pk02_door02_C.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_door02_N.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_door02_S.tga");
//
//    r_LoadTexture("../../../textures/pk02/pk02_door03_C.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_door03_N.tga");
//    r_LoadTexture("../../../textures/pk02/pk02_door03_S.tga");


//    struct entity_handle_t entity_handle = ent_CreateEntityInstance();
//    ent_AddComponent(entity_handle, ENT_COMPONENT_TYPE_TRANSFORM);
//    ent_AddModelComponent(entity_handle, cube);
//
//    struct entity_t *entity = ent_GetEntityPointer(entity_handle);
//    struct transform_component_t *transform;
//
//    transform = (struct transform_component_t *)ent_GetComponentPointer(entity->components[ENT_COMPONENT_TYPE_TRANSFORM]);
//
//    transform->orientation.identity();
//    transform->position.y = 1.0;

//    int portal0 = r_CreatePortal(vec3_t(0.0, 3.0, 0.5), vec2_t(2.5, 3.0));
//    int portal1 = r_CreatePortal(vec3_t(0.0, 1.5, 2.0), vec2_t(2.5, 3.0));
//    r_LinkPortals(portal0, portal1);
//
//    struct portal_t *portal = r_GetPortalPointer(portal0);
//    portal->orientation = rotate_x(-0.5);
//    rotate_y(portal->orientation, 0.05);



//    int portal0 = r_CreatePortal(vec3_t(0.0, 4.6, -3.0), vec2_t(2.5, 2.5));
//    int portal1 = r_CreatePortal(vec3_t(0.0, -11.0, 5.0), vec2_t(2.5, 2.5));
//    r_LinkPortals(portal0, portal1);
//
//    int portal2 = r_CreatePortal(vec3_t(0.0, -13.6, -5.0), vec2_t(2.5, 2.5));
//    int portal3 = r_CreatePortal(vec3_t(0.0, 3.0, -3.0), vec2_t(2.5, 2.5));
//    r_LinkPortals(portal2, portal3);
//
//    struct portal_t *portal;
//
//    portal = r_GetPortalPointer(portal0);
//    portal->orientation = rotate_x(0.5);
//
//    portal = r_GetPortalPointer(portal1);
//    portal->orientation = rotate_x(-0.5);
//    rotate_y(portal->orientation, 1.0);
//
//    portal = r_GetPortalPointer(portal2);
//    portal->orientation = rotate_x(0.5);
//
//    portal = r_GetPortalPointer(portal3);
//    portal->orientation = rotate_x(-0.5);
//    rotate_y(portal->orientation, 1.0);
    #endif

//    struct collider_handle_t collider = phy_CreateCollider(PHY_COLLIDER_TYPE_RIGID);
//    struct rigid_body_collider_t *collider_ptr = phy_GetRigidBodyColliderPointer(collider);
//    struct box_shape_t *box_shape;
//
//    collider_ptr->base.position = vec3_t(5.0, 3.0, -4.0);
//    collider_ptr->base.linear_velocity = vec3_t(0.0, 0.0, 0.0);
//    collider_ptr->collision_shape = phy_AllocCollisionShape(PHY_COLLISION_SHAPE_BOX);
//    collider_ptr->orientation = rotate_x(0.25);
//
//    box_shape = (struct box_shape_t *)phy_GetCollisionShapePointer(collider_ptr->collision_shape);
//    box_shape->size = vec3_t(1.0, 1.0, 1.0);

//    int portal0 = r_CreatePortal(vec3_t(0.0, 0.2, -5.8), vec2_t(1.8, 2.8));
//    int portal1 = r_CreatePortal(vec3_t(0.0, 0.2, 5.8), vec2_t(1.8, 2.8));
//    r_LinkPortals(portal0, portal1);
//
//    struct portal_t *portal;
//
//    portal = r_GetPortalPointer(portal0);
//    portal->orientation = rotate_y(1.0);
//    portal = r_GetPortalPointer(portal1);
//
//
//    portal0 = r_CreatePortal(vec3_t(5.8, 0.2, 0.0), vec2_t(1.8, 2.8));
//    portal1 = r_CreatePortal(vec3_t(-5.8, 0.2, 0.0), vec2_t(1.8, 2.8));
//    r_LinkPortals(portal0, portal1);
//
//    portal = r_GetPortalPointer(portal0);
//    portal->orientation = rotate_y(0.5);
//    portal = r_GetPortalPointer(portal1);
//    portal->orientation = rotate_y(-0.5);


    //portal->orientation = rotate_y(-1.0);

    //portal->orientation = rotate_x(-0.5);
    //portal->orientation = rotate(vec3_t(1.0, 0.0, 0.0), -0.5);


    float pos = 2.5;
    float angle = 0.0;

    struct view_t *active_view;

    active_view = r_GetActiveView();

    active_view->position = vec3_t(2.0, 3.0, -2.0);

    while(1)
    {
        //portal->orientation = rotate_y(portal->orientation, angle);

        //collider_ptr->orientation = rotate_z(angle);

      /*  portal->position.y = pos + sin(angle) * 2.0;*/
//        angle += 0.009;
        be_QueueCmd(BE_CMD_UPDATE_INPUT, NULL, 0);
        be_WaitEmptyQueue();
        be_Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        player_UpdatePlayers();
        phy_Step();
        ent_UpdatePhysicsComponents();
        ent_UpdateTransformComponents();
        player_PostUpdatePlayers();


        r_VisibleWorld();
        be_SwapBuffers();
        be_WaitEmptyQueue();

        if(in_GetKeyStatus(SDL_SCANCODE_ESCAPE) & KEY_STATUS_JUST_PRESSED)
        {
            be_QueueCmd(BE_CMD_SHUTDOWN, NULL, 0);
            break;
        }
    }

    return 0;
}

void fe_RunFrontend()
{
    SDL_Thread *frontend_thread;
    frontend_thread = SDL_CreateThread(fe_Frontend, "Frontend", NULL);
    SDL_DetachThread(frontend_thread);
}

int fe_LoadShader(char *file_name)
{
    be_LoadShader(file_name);
    be_WaitEmptyQueue();
    return r_GetShader(file_name);
}




