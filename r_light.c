#include "r_light.h"
#include "stack_list.h"
#include "GL/glew.h"

#include <stdio.h>

extern struct renderer_t r_renderer;


struct stack_list_t r_lights(sizeof(struct light_t), 32);
//unsigned int r_light_uniform_buffer;

void r_InitLights()
{
//    glGenBuffers(1, &r_renderer.r_light_uniform_buffer);
//    glBindBuffer(GL_UNIFORM_BUFFER, r_renderer.r_light_uniform_buffer);
//    glBufferData(GL_UNIFORM_BUFFER, sizeof(struct light_t) * 128, NULL, GL_DYNAMIC_DRAW);
}

void r_ShutdownLights()
{
//    glDeleteBuffers(1, &r_renderer.r_light_uniform_buffer);
}

int r_CreateLight(vec3_t position, vec3_t color, float energy, float radius)
{
    int light_handle;
    struct light_t *light;

    light_handle = r_lights.add(NULL);
    light = (struct light_t *)r_lights.get(light_handle);

    if(radius < R_MIN_LIGHT_RADIUS)
    {
        radius = R_MIN_LIGHT_RADIUS;
    }

    light->pos_radius = vec4_t(position, radius);
    light->col_energy = vec4_t(color, energy);

    return light_handle;
}

void r_DestroyLight(int light_handle)
{
    struct light_t *light;

    light = r_GetLightPointer(light_handle);

    if(light)
    {
        light->pos_radius.w = -1.0;
        r_lights.remove(light_handle);
    }
}

struct light_t *r_GetLightPointer(int light_handle)
{
    struct light_t *light = NULL;

    light = (struct light_t *)r_lights.get(light_handle);

    if(light)
    {
        if(light->pos_radius.w <= 0.0)
        {
            light = NULL;
        }
    }

    return light;
}

void r_UploadLights(struct light_buffer_t *light_buffer)
{
    int start;

    if(light_buffer->lights.cursor)
    {
        start = r_renderer.uniform_buffers[R_LIGHTS_UNIFORM_BUFFER_BINDING].offset;

        glBufferSubData(GL_UNIFORM_BUFFER, start, sizeof(struct light_t) * light_buffer->lights.cursor, light_buffer->lights.buffer);
        glBufferSubData(GL_UNIFORM_BUFFER, start + sizeof(struct light_t) * 64, sizeof(int), &light_buffer->lights.cursor);
    }

    light_buffer->used = 1;
}






