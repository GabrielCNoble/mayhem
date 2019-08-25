#include "be_r_dbg.h"
#include "r_verts.h"
#include "r_view.h"
#include "r_shader.h"
#include "phy_common.h"

#include "../common/utils/geo.h"
#include "../common/gmath/geometry.h"
#include "../common/containers/stack_list.h"

#include <math.h>

extern struct verts_handle_t r_debug_verts_buffer_handle;
extern unsigned int r_debug_verts_buffer_start;

extern struct renderer_t r_renderer;

extern struct stack_list_t phy_colliders[];

void be_r_DrawLine(struct draw_line_data_t *line)
{
    struct vertex_t vertices[2];
    struct view_t *active_view;
    vec4_t color;

    active_view = r_GetActiveView();

    vertices[0].position = line->a;
    vertices[1].position = line->b;


    color = vec4_t(line->color, 1.0);

    r_MemcpyToUnmapped(r_debug_verts_buffer_handle, &vertices, sizeof(struct vertex_t) * 2);
    r_SetShader(r_renderer.debug_shader);
    r_DefaultUniformMatrix4fv(r_ViewProjectionMatrix, (float *)active_view->view_projection_matrix.floats);
    r_DefaultUniform4fv(r_DebugColor, (float *)color.floats);
    glDrawArrays(GL_LINES, r_debug_verts_buffer_start, 2);
}

void be_r_DrawTriangle(struct draw_triangle_data_t *triangle)
{
    struct vertex_t vertices[3];
    struct view_t *active_view;
    vec4_t color;

    active_view = r_GetActiveView();

    vertices[0].position = triangle->a;
    vertices[1].position = triangle->b;
    vertices[2].position = triangle->c;

    color = vec4_t(triangle->color, 1.0);

    r_MemcpyToUnmapped(r_debug_verts_buffer_handle, &vertices, sizeof(struct vertex_t) * 3);
    r_SetShader(r_renderer.debug_shader);
    r_DefaultUniformMatrix4fv(r_ViewProjectionMatrix, (float *)active_view->view_projection_matrix.floats);
    r_DefaultUniform4fv(r_DebugColor, (float *)color.floats);

    if(triangle->wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if(!triangle->backface_culling)
    {
        glDisable(GL_CULL_FACE);
    }

    if(!triangle->depth_test)
    {
        glDisable(GL_DEPTH_TEST);
    }

    glDrawArrays(GL_TRIANGLES, r_debug_verts_buffer_start, 3);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

#define SEGMENT_VERT_COUNT 64

void be_r_DrawCapsule(struct draw_capsule_data_t *capsule)
{
    struct vertex_t vertices[SEGMENT_VERT_COUNT * 3];
    int i = 0;
    float increment = (2.0 * 3.14159265) / (float)SEGMENT_VERT_COUNT;
    float cur_angle;
    struct view_t *active_view = r_GetActiveView();
    vec3_t normal;
    vec3_t dir;
    vec4_t color;

    cur_angle = 0.0;
    for(; i < SEGMENT_VERT_COUNT; i++)
    {
        dir = vec3_t(cos(cur_angle), sin(cur_angle), 0.0);
        vertices[i].position = support_vertex_upright_capsule(dir, capsule->height, capsule->radius, normal);
        cur_angle += increment;
    }

    for(; i < SEGMENT_VERT_COUNT * 2; i++)
    {
        vertices[i].position.z = vertices[i - SEGMENT_VERT_COUNT].position.x;
        vertices[i].position.y = vertices[i - SEGMENT_VERT_COUNT].position.y;
        vertices[i].position.x = 0.0;
    }

    for(; i < SEGMENT_VERT_COUNT * 3; i++)
    {
        dir = vec3_t(cos(cur_angle), 0.0, sin(cur_angle));
        vertices[i].position = support_vertex_upright_capsule(dir, capsule->height, capsule->radius, normal);
        cur_angle += increment;
    }

    for(i = 0; i < SEGMENT_VERT_COUNT * 3; i++)
    {
        vertices[i].position += capsule->position;
    }

    color = vec4_t(0.0, 1.0, 0.0, 1.0);

    r_MemcpyToUnmapped(r_debug_verts_buffer_handle, &vertices, sizeof(struct vertex_t ) * SEGMENT_VERT_COUNT * 3);
    r_SetShader(r_renderer.debug_shader);
    r_DefaultUniformMatrix4fv(r_ViewProjectionMatrix, (float *)active_view->view_projection_matrix.floats);
    r_DefaultUniform4fv(r_DebugColor, (float *)color.floats);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start, SEGMENT_VERT_COUNT);
    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start + SEGMENT_VERT_COUNT, SEGMENT_VERT_COUNT);
    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start + SEGMENT_VERT_COUNT * 2, SEGMENT_VERT_COUNT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void be_r_DrawPoint(struct draw_point_data_t *point)
{
    struct vertex_t vertice;
    struct view_t *active_view;
    vec4_t color = vec4_t(point->color, 1.0);

    active_view = r_GetActiveView();
    vertice.position = point->position;
    r_MemcpyToUnmapped(r_debug_verts_buffer_handle, &vertice, sizeof(struct vertex_t));
    r_SetShader(r_renderer.debug_shader);
    r_DefaultUniformMatrix4fv(r_ViewProjectionMatrix, (float *)active_view->view_projection_matrix.floats);
    r_DefaultUniform4fv(r_DebugColor, (float *)color.floats);

    glPointSize(point->size);

    if(point->smooth)
    {
        glEnable(GL_POINT_SMOOTH);
    }

    glDrawArrays(GL_POINTS, r_debug_verts_buffer_start, 1);

    glDisable(GL_POINT_SMOOTH);
    glPointSize(1.0);
}

#define SEGMENT_VERT_COUNT 64
void be_r_DrawDebug(struct draw_debug_data_t *data)
{
    int i;
    int j;
    int type;
    int c;
    struct player_collider_t *player_collider;
    struct projectile_collider_t *projectile_collider;
    struct rigid_body_collider_t *rigid_collider;

    struct vertex_t vertices[SEGMENT_VERT_COUNT * 3];
    float increment = (2.0 * 3.14159265) / (float)SEGMENT_VERT_COUNT;
    float cur_angle;
    vec3_t dir;
    vec3_t normal;
    vec4_t color = vec4_t(1.0, 1.0, 1.0, 1.0);

    vec3_t pos;
    vec3_t corner;
    vec3_t size;
    float radius;
    float height;

    r_SetShader(r_renderer.debug_shader);
    r_DefaultUniformMatrix4fv(r_ViewProjectionMatrix, (float *)data->view_projection_matrix.floats);


    for(type = PHY_COLLIDER_TYPE_PLAYER; type < PHY_COLLIDER_TYPE_NONE; type++)
    {
        c = phy_colliders[type].cursor;

        switch(type)
        {
            case PHY_COLLIDER_TYPE_PLAYER:

                color = vec4_t(1.0, 1.0, 1.0, 1.0);
                r_DefaultUniform4fv(r_DebugColor, (float *)color.floats);

                for(j = 0; j < c; j++)
                {
                    player_collider = (struct player_collider_t *)phy_colliders[type].get(j);

                    if(player_collider->base.type != PHY_COLLIDER_TYPE_PLAYER)
                    {
                        continue;
                    }

                    cur_angle = 0.0;
                    for(i = 0; i < SEGMENT_VERT_COUNT; i++)
                    {
                        dir = vec3_t(cos(cur_angle), sin(cur_angle), 0.0);
                        vertices[i].position = support_vertex_upright_capsule(dir, player_collider->height, player_collider->radius, normal);
                        cur_angle += increment;
                    }

                    for(; i < SEGMENT_VERT_COUNT * 2; i++)
                    {
                        vertices[i].position.z = vertices[i - SEGMENT_VERT_COUNT].position.x;
                        vertices[i].position.y = vertices[i - SEGMENT_VERT_COUNT].position.y;
                        vertices[i].position.x = 0.0;
                    }

                    for(; i < SEGMENT_VERT_COUNT * 3; i++)
                    {
                        dir = vec3_t(cos(cur_angle), 0.0, sin(cur_angle));
                        vertices[i].position = support_vertex_upright_capsule(dir, player_collider->height, player_collider->radius, normal);
                        cur_angle += increment;
                    }

                    for(i = 0; i < SEGMENT_VERT_COUNT * 3; i++)
                    {
                        vertices[i].position += player_collider->base.position;
                    }

                    r_MemcpyToUnmapped(r_debug_verts_buffer_handle, &vertices, sizeof(struct vertex_t ) * SEGMENT_VERT_COUNT * 3);

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start, SEGMENT_VERT_COUNT);
                    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start + SEGMENT_VERT_COUNT, SEGMENT_VERT_COUNT);
                    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start + SEGMENT_VERT_COUNT * 2, SEGMENT_VERT_COUNT);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }

            break;

            case PHY_COLLIDER_TYPE_PROJECTILE:

                color = vec4_t(1.0, 1.0, 0.0, 1.0);
                r_DefaultUniform4fv(r_DebugColor, (float *)color.floats);

                for(j = 0; j < c; j++)
                {
                    projectile_collider = (struct projectile_collider_t *)phy_colliders[type].get(j);

                    if(projectile_collider->base.type != PHY_COLLIDER_TYPE_PROJECTILE)
                    {
                        continue;
                    }

                    cur_angle = 0.0;
                    for(i = 0; i < SEGMENT_VERT_COUNT; i++)
                    {
                        vertices[i].position = vec3_t(cos(cur_angle), sin(cur_angle), 0.0) * projectile_collider->radius;
                        cur_angle += increment;
                    }

                    for(; i < SEGMENT_VERT_COUNT * 2; i++)
                    {
                        vertices[i].position.z = vertices[i - SEGMENT_VERT_COUNT].position.x;
                        vertices[i].position.y = vertices[i - SEGMENT_VERT_COUNT].position.y;
                        vertices[i].position.x = 0.0;
                    }

                    for(; i < SEGMENT_VERT_COUNT * 3; i++)
                    {
                        vertices[i].position.x = vertices[i - SEGMENT_VERT_COUNT * 2].position.x;
                        vertices[i].position.y = 0.0;
                        vertices[i].position.z = vertices[i - SEGMENT_VERT_COUNT * 2].position.y;
                    }

                    for(i = 0; i < SEGMENT_VERT_COUNT * 3; i++)
                    {
                        vertices[i].position += projectile_collider->base.position;
                    }

                    r_MemcpyToUnmapped(r_debug_verts_buffer_handle, &vertices, sizeof(struct vertex_t ) * SEGMENT_VERT_COUNT * 3);

                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start, SEGMENT_VERT_COUNT);
                    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start + SEGMENT_VERT_COUNT, SEGMENT_VERT_COUNT);
                    glDrawArrays(GL_LINE_LOOP, r_debug_verts_buffer_start + SEGMENT_VERT_COUNT * 2, SEGMENT_VERT_COUNT);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            break;
        }
    }
}
#undef SEGMENT_VERT_COUNT



