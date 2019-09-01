#include "r_raytrace.h"

#include "r_common.h"
#include "GL/glew.h"
#include <stdio.h>
#include <stdlib.h>

extern struct renderer_t r_renderer;
//
//unsigned int r_triangles_uniform_buffer;
//unsigned int r_triangle_indices_uniform_buffer;

#ifdef __cplusplus
extern "C"
{
#endif

void r_InitRaytrace()
{
//    glGenBuffers(1, &r_renderer.r_triangles_uniform_buffer);
//    glGenBuffers(1, &r_renderer.r_triangle_indices_uniform_buffer);
//
//    glBindBuffer(GL_UNIFORM_BUFFER, r_renderer.r_triangles_uniform_buffer);
//    glBufferData(GL_UNIFORM_BUFFER, sizeof(vec3_t) * R_MAX_RAYTRACE_TRIANGLES, NULL, GL_STREAM_DRAW);
//
//    glBindBuffer(GL_UNIFORM_BUFFER, r_renderer.r_triangle_indices_uniform_buffer);
//    glBufferData(GL_UNIFORM_BUFFER, sizeof(unsigned int) * (R_MAX_RAYTRACE_TRIANGLES / 3), NULL, GL_STREAM_DRAW);
}

void r_ShutdownRaytrace()
{
//    glDeleteBuffers(1, &r_renderer.r_triangles_uniform_buffer);
//    glDeleteBuffers(1, &r_renderer.r_triangle_indices_uniform_buffer);
}

void r_UploadTriangles(vec3_t *vertices, int count)
{
    vec4_t *raytrace_vertices;
    int i;

    int start;

    if(count && vertices)
    {
        count -= count % 3;

        if(count > R_MAX_RAYTRACE_TRIANGLES * 3)
        {
            count = R_MAX_RAYTRACE_TRIANGLES * 3;
        }

        raytrace_vertices = (vec4_t *)calloc(sizeof(vec4_t), count);

        for(i = 0; i < count; i++)
        {
            raytrace_vertices[i].x = vertices[i].x;
            raytrace_vertices[i].y = vertices[i].y;
            raytrace_vertices[i].z = vertices[i].z;
            raytrace_vertices[i].w = 1.0;
        }

        start = r_renderer.uniform_buffers[R_TRIANGLES_UNIFORM_BUFFER_BINDING].offset;

        glBufferSubData(GL_UNIFORM_BUFFER, start, sizeof(vec4_t ) * count, raytrace_vertices);

        free(raytrace_vertices);
    }
}

void r_UploadTriangeIndices(unsigned int *indices, int count)
{
    unsigned int start;

    if(count && indices)
    {
        if(count > R_MAX_RAYTRACE_TRIANGLES)
        {
            count = R_MAX_RAYTRACE_TRIANGLES;
        }

        start = r_renderer.uniform_buffers[R_TRIANGLE_INDICES_UNIFORM_BUFFER_BINDING].offset;

        glBufferSubData(GL_UNIFORM_BUFFER, start, sizeof(unsigned int ) * count, indices);
    }
}

#ifdef __cplusplus
}
#endif





