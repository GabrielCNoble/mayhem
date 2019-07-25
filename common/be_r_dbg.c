#include "be_r_dbg.h"
#include "r_verts.h"
#include "r_view.h"
#include "r_shader.h"

#include "../utils/geo.h"
#include "../gmath/geometry.h"

#include <math.h>

extern struct verts_handle_t r_debug_verts_buffer_handle;
extern unsigned int r_debug_verts_buffer_start;

extern struct renderer_t r_renderer;

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



