#include "r_verts.h"
#include "stack_list.h"
#include "list.h"

#include <string.h>

#include "r_common.h"

#define R_HEAP_SIZE 67108864

struct gpu_chunk_t
{
    unsigned int size;
    unsigned int start;
    unsigned short mapped;
    unsigned short align;
};


void *r_mapped[2] = {NULL};
unsigned int r_mapped_count[2] = {0};
unsigned int r_buffers[2] = {0};
struct stack_list_t r_allocs[2];
struct list_t r_frees[2];


void r_InitVerts()
{
    glGenBuffers(2, r_buffers);

    struct gpu_chunk_t first_chunk;

    glBindBuffer(GL_ARRAY_BUFFER, r_buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, R_HEAP_SIZE, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_buffers[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, R_HEAP_SIZE, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    r_allocs[0].init(sizeof(struct gpu_chunk_t), 32);
    r_allocs[1].init(sizeof(struct gpu_chunk_t), 32);

    r_frees[0].init(sizeof(struct gpu_chunk_t), 32);
    r_frees[1].init(sizeof(struct gpu_chunk_t), 32);

    first_chunk.size = R_HEAP_SIZE;
    first_chunk.start = 0;

    r_frees[0].add(&first_chunk);
    r_frees[1].add(&first_chunk);

}

void r_ShutdownVerts()
{
    glDeleteBuffers(2, r_buffers);
}

struct verts_handle_t r_AllocVerts(unsigned int size)
{
    return r_Alloc(size, 0);
}

struct verts_handle_t r_AllocIndex(unsigned int size)
{
    return r_Alloc(size, 1);
}

struct verts_handle_t r_Alloc(unsigned int size, int is_index_alloc)
{
    struct verts_handle_t handle = R_INVALID_VERTS_HANDLE;
    struct gpu_chunk_t *free_chunks;
    struct gpu_chunk_t *alloc_chunk;
    int i;
    int c;

    is_index_alloc = is_index_alloc && 1;

    handle.is_index_alloc = is_index_alloc;

    free_chunks = (struct gpu_chunk_t *)r_frees[is_index_alloc].buffer;
    c = r_frees[is_index_alloc].cursor;

    size = (size + 3) & (~3);

    for(i = 0; i < c; i++)
    {
        if(free_chunks[i].size >= size)
        {
            handle.index = r_allocs[is_index_alloc].add(NULL);
            alloc_chunk = (struct gpu_chunk_t *)r_allocs[is_index_alloc].get(handle.index);

            alloc_chunk->start = free_chunks[i].start;
            alloc_chunk->size = size;

            free_chunks[i].start += size;
            free_chunks[i].size -= size;

            if(free_chunks[i].size == 0)
            {
                r_frees[is_index_alloc].remove(i);
            }

            break;
        }
    }

    return handle;
}

void r_Free(struct verts_handle_t handle)
{
    struct gpu_chunk_t *alloc_chunk;

    if(handle.index != R_INVALID_ALLOC_INDEX)
    {
        alloc_chunk = (struct gpu_chunk_t *)r_allocs[handle.is_index_alloc].get(handle.index);
        if(alloc_chunk->size)
        {
            r_frees[handle.is_index_alloc].add(alloc_chunk);
            alloc_chunk->size = 0;
            r_allocs[handle.is_index_alloc].remove(handle.index);
        }
    }
}



void r_SortFreeChunks(int left, int right, struct gpu_chunk_t *chunks)
{
    int i;
    int j;
    struct gpu_chunk_t *middle_chunk;
    struct gpu_chunk_t temp_chunk;
    int middle = (right + left) / 2;

    middle_chunk = chunks + middle;

    i = left;
    j = right;

    while(i <= j)
    {
        for(; i < right && chunks[i].start < middle_chunk->start; i++);
        for(; j > left && chunks[j].start > middle_chunk->start; j--);

        if(i <= j)
        {
            temp_chunk = chunks[i];
            chunks[i] = chunks[j];
            chunks[j] = temp_chunk;

            i++;
            j--;
        }
    }

    if(i < right) r_SortFreeChunks(i, right, chunks);
    if(j > left)r_SortFreeChunks(left, j, chunks);
}


void r_DefragVerts(int index_list)
{
    int i;
    int j;
    int c;
    struct gpu_chunk_t *chunks;


    index_list = index_list && 1;
    chunks = (struct gpu_chunk_t *)r_frees[index_list].buffer;
    c = r_frees[index_list].cursor;

    /* sort the free chunks based on their starting point... */
    r_SortFreeChunks(0, c - 1, chunks);


    for(i = 0; i < c; i++)
    {
        for(j = i + 1; j < c; j++)
        {
            if(chunks[i].start + chunks[i].size == chunks[j].start)
            {
                /* merge adjacent chunks into the current chunk... */
                chunks[i].size += chunks[j].size;

                /* mark this chunk as merged... */
                chunks[j].size = 0;
            }
            else
            {
                /* found a chunk that can't be merged into the current
                chunk, so use it as the new starting point and try to
                merge preceding chunks into it... */
                i = j - 1;
                break;
            }
        }
    }

    for(i = 0; i < c; i++)
    {
        if(chunks[i].size == 0)
        {
            /* found a chunk that got merged into another chunk (size == 0)... */
            for(j = i + 1; j < c && !chunks[j].size; j++)
            {
                /* look forward for a chunk that wasn't merged (size != 0)... */
                r_frees[index_list].cursor--;
            }

            if(j < c)
            {
                /* overwrite the current merged chunk with the valid chunk found... */
                chunks[i] = chunks[j];
            }

            /* restart the search at next chunk after the one we just moved... */
            i = j;
        }
    }
}





void *r_MapAlloc(struct verts_handle_t handle)
{
    void *pointer = NULL;
    unsigned short target;
    struct gpu_chunk_t *alloc;

    if(handle.index != R_INVALID_ALLOC_INDEX)
    {
        alloc = (struct gpu_chunk_t *)r_allocs[handle.is_index_alloc].get(handle.index);

        if(alloc)
        {
            if(!alloc->mapped)
            {
                alloc->mapped = 1;
                r_mapped_count[handle.is_index_alloc]++;
            }

            if(!r_mapped[handle.is_index_alloc])
            {
                target = handle.is_index_alloc ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
                glBindBuffer(target, r_buffers[handle.is_index_alloc]);
                r_mapped[handle.is_index_alloc] = glMapBuffer(target, GL_READ_WRITE);
            }

            pointer = (unsigned char *)r_mapped[handle.is_index_alloc] + alloc->start;
        }
    }

    return pointer;
}

void r_UnmapAlloc(struct verts_handle_t handle)
{
    struct gpu_chunk_t *alloc;
    unsigned short target;

    if(handle.index != R_INVALID_ALLOC_INDEX)
    {
        alloc = (struct gpu_chunk_t *)r_allocs[handle.is_index_alloc].get(handle.index);

        if(alloc)
        {
            if(alloc->mapped)
            {
                alloc->mapped = 0;
                r_mapped_count[handle.is_index_alloc]--;
            }

            if(!r_mapped_count[handle.is_index_alloc] && r_mapped[handle.is_index_alloc])
            {
                target = handle.is_index_alloc ? GL_ELEMENT_ARRAY_BUFFER : GL_ARRAY_BUFFER;
                glUnmapBuffer(target);
                r_mapped[handle.is_index_alloc] = NULL;
            }
        }
    }
}

void r_UnmapAllAllocs()
{
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    int i;
    struct gpu_chunk_t *alloc;

    r_mapped[0] = NULL;
    r_mapped[1] = NULL;

    r_mapped_count[0] = 0;
    r_mapped_count[1] = 0;

    for(i = 0; i < r_allocs[0].cursor; i++)
    {
        alloc = (struct gpu_chunk_t *)r_allocs[0].get(i);
        alloc->mapped = 0;
    }

    for(i = 0; i < r_allocs[1].cursor; i++)
    {
        alloc = (struct gpu_chunk_t *)r_allocs[1].get(i);
        alloc->mapped = 0;
    }
}


void r_MemcpyTo(struct verts_handle_t dst, void *src, unsigned int size)
{
    void *dst_ptr;

    dst_ptr = r_MapAlloc(dst);

    if(dst_ptr)
    {
        memcpy(dst_ptr, src, size);
        r_UnmapAlloc(dst);
    }
}

void r_MemcpyFrom(void *dst, struct verts_handle_t src, unsigned int size)
{
    void *src_ptr;

    src_ptr = r_MapAlloc(src);

    if(src_ptr)
    {
        memcpy(dst, src_ptr, size);
        r_UnmapAlloc(src);
    }
}










