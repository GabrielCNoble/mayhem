#include "w_main.h"
#include "r_common.h"
#include "r_verts.h"
#include "s_aux.h"
#include "physics.h"
#include "be_backend.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct verts_handle_t w_world_verts = R_INVALID_VERTS_HANDLE;
struct verts_handle_t w_world_indices = R_INVALID_VERTS_HANDLE;

struct list_t w_world_batches;
unsigned int *w_world_indices_buffer = NULL;
struct draw_triangle_t *w_world_draw_triangles;
struct bvh_node_t *w_world_bvh = NULL;

struct collider_handle_t w_world_collider;



void w_Init()
{
    w_world_batches.init(sizeof(struct draw_batch_t), 32);
}

void w_Shutdown()
{

}

void w_LoadLevel(char *file_name)
{
    struct geometry_data_t level_data;
    struct vertex_t *vertices;
    int i;

    aux_LoadWavefront(file_name, &level_data);

    //phy_BuildBvh((vec3_t *)level_data.vertices.buffer, level_data.vertices.cursor);
    w_world_collider = phy_CreateStaticCollider();
    phy_AddStaticTriangles(w_world_collider, (vec3_t *)level_data.vertices.buffer, level_data.vertices.cursor);
    phy_StaticColliderBvh(w_world_collider);


    vertices = (struct vertex_t *)calloc(sizeof(vertex_t), level_data.vertices.cursor);

    w_world_batches.cursor = 0;

    for(i = 0; i < level_data.vertices.cursor; i++)
    {
        vertices[i].position = *(vec3_t *)level_data.vertices.get(i);
        vertices[i].normal = *(vec3_t *)level_data.normals.get(i);
        vertices[i].tex_coord = vec2_t(0.0, 0.0);
    }

    for(i = 0; i < level_data.draw_batches.cursor ;i++)
    {
        w_world_batches.add(level_data.draw_batches.get(i));
    }

    w_world_verts = r_AllocVerts(sizeof(struct vertex_t) * level_data.vertices.cursor, sizeof(struct vertex_t));
    //w_world_indices = r_AllocIndex(sizeof(unsigned int) * level_data.vertices.cursor, sizeof(unsigned int ));

    w_world_indices_buffer = (unsigned int *)calloc(sizeof(unsigned int ), level_data.vertices.cursor);
    w_world_draw_triangles = (struct draw_triangle_t *)calloc(sizeof(struct draw_triangle_t ), level_data.vertices.cursor);

    w_world_bvh = w_BuildBvh((struct draw_batch_t *)level_data.draw_batches.buffer, level_data.draw_batches.cursor, (vec3_t *)level_data.vertices.buffer, w_world_draw_triangles);

    be_MemcpyTo(w_world_verts, vertices, sizeof(struct vertex_t) * level_data.vertices.cursor);
}

struct bvh_node_t *w_BuildBvh(struct draw_batch_t *batches, int batch_count, vec3_t *vertices, struct draw_triangle_t *draw_triangles)
{
    int batch_index;
    int triangle_index;
    int k;

    struct draw_batch_t *batch;
    struct bvh_node_t *nodes = NULL;
    struct bvh_node_t *new_node = NULL;
    struct draw_triangle_t *draw_triangle;
    vec3_t *triangle;

    if(batch_count)
    {
        for(batch_index = 0; batch_index < batch_count; batch_index++)
        {
            batch = batches + batch_index;

            for(triangle_index = 0; triangle_index < batch->count; triangle_index += 3)
            {
                triangle = vertices + batch->start + triangle_index;

                new_node = (struct bvh_node_t *)calloc(sizeof(struct bvh_node_t ), 1);
                new_node->left = NULL;
                new_node->right = NULL;

                new_node->max = vec3_t(-FLT_MAX, -FLT_MAX, -FLT_MAX);
                new_node->min = vec3_t(FLT_MAX, FLT_MAX, FLT_MAX);

                for(k = 0; k < 3; k++)
                {
                    if(triangle[k].x > new_node->max.x) new_node->max.x = triangle[k].x;
                    if(triangle[k].x < new_node->min.x) new_node->min.x = triangle[k].x;

                    if(triangle[k].y > new_node->max.y) new_node->max.y = triangle[k].y;
                    if(triangle[k].y < new_node->min.y) new_node->min.y = triangle[k].y;

                    if(triangle[k].z > new_node->max.z) new_node->max.z = triangle[k].z;
                    if(triangle[k].z < new_node->min.z) new_node->min.z = triangle[k].z;
                }

                for(k = 0; k < 3; k++)
                {
                    if(fabs(new_node->max[k] - new_node->min[k]) < 0.001)
                    {
                        new_node->max[k] += 0.05;
                        new_node->min[k] -= 0.05;
                    }
                }

                draw_triangle = draw_triangles + batch->start + triangle_index;
                draw_triangle->first_vertice = batch->start + triangle_index;
                draw_triangle->batch_index = batch_index;

                new_node->data = draw_triangle;

                new_node->parent = nodes;
                nodes = new_node;

            }
        }

        return phy_BuildBvhRecursive(nodes);
    }

    return NULL;
}

#ifdef __cplusplus
}
#endif






