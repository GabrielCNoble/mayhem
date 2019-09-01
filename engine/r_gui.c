#include "r_gui.h"
#include "r_common.h"
#include "r_shader.h"
#include "r_verts.h"
#include "r_main.h"
//#include "be_backend.h"
#include "GL/gl.h"
#include "gui.h"

#include <stdlib.h>
#include <stdio.h>


#define R_GUI_VERTS_PER_DRAW_CALL 4096 * 3

//unsigned int r_gui_vertex_buffer = 0;
//unsigned int r_gui_index_buffer = 0;
struct verts_handle_t r_gui_vertex_buffer;
struct verts_handle_t r_gui_index_buffer;
unsigned int *r_gui_indexes;
unsigned int r_gui_vert_start;
unsigned int r_gui_index_start;

unsigned int r_gui_vertex_shader = 0;
unsigned int r_gui_fragment_shader = 0;
unsigned int r_gui_shader_program = 0;

unsigned int r_gui_shader_vertex_position = 0;
unsigned int r_gui_shader_vertex_tex_coord = 0;
unsigned int r_gui_shader_vertex_color = 0;
unsigned int r_gui_shader_projection_matrix = 0;
unsigned int r_gui_shader_atlas = 0;

extern struct renderer_t r_renderer;

#ifdef __cplusplus
extern "C"
{
#endif

void r_InitGui()
{
    char *vertex_shader_source =
        "#version 330\n"
        "in vec4 r_gui_position;\n"
        "in vec2 r_gui_tex_coord;\n"
        "in vec4 r_gui_color;\n"
        "uniform mat4 r_gui_view_projection_matrix;\n"
        "out vec2 tex_coord;\n"
        "out vec4 color;\n"
        "void main(){\n"
        "gl_Position = r_gui_view_projection_matrix * r_gui_position;\n"
        "tex_coord = r_gui_tex_coord;\n"
        "color = r_gui_color;}\n";

    char *fragment_shader_source =
        "#version 330\n"
        "in vec2 tex_coord;\n"
        "in vec4 color;\n"
        "uniform sampler2D r_gui_atlas;\n"
        "void main(){\n"
        "gl_FragColor = color * texture(r_gui_atlas, tex_coord);}\n";


    unsigned int vertex_shader;
    unsigned int fragment_shader;

    r_gui_vertex_shader = r_CreateAndCompileShader(vertex_shader_source, GL_VERTEX_SHADER);
    r_gui_fragment_shader = r_CreateAndCompileShader(fragment_shader_source, GL_FRAGMENT_SHADER);
    r_gui_shader_program = r_CreateAndLinkProgram(r_gui_vertex_shader, r_gui_fragment_shader);
//
    r_gui_shader_vertex_position = glGetAttribLocation(r_gui_shader_program, "r_gui_position");
    r_gui_shader_vertex_tex_coord = glGetAttribLocation(r_gui_shader_program, "r_gui_tex_coord");
    r_gui_shader_vertex_color = glGetAttribLocation(r_gui_shader_program, "r_gui_color");
    r_gui_shader_projection_matrix = glGetUniformLocation(r_gui_shader_program, "r_gui_view_projection_matrix");
    r_gui_shader_atlas = glGetUniformLocation(r_gui_shader_program, "r_gui_atlas");
//
    r_gui_vertex_buffer = r_AllocVerts(R_GUI_VERTS_PER_DRAW_CALL * sizeof(ImDrawVert), sizeof(ImDrawVert));
    r_gui_index_buffer = r_AllocIndex(R_GUI_VERTS_PER_DRAW_CALL * sizeof(unsigned int), sizeof(unsigned int ));
    r_gui_vert_start = r_GetAllocStart(r_gui_vertex_buffer) / sizeof(ImDrawVert);
    r_gui_index_start = r_GetAllocStart(r_gui_index_buffer) / sizeof(unsigned int);
    r_gui_indexes = (unsigned int *)calloc(sizeof(unsigned int ), R_GUI_VERTS_PER_DRAW_CALL);
//    glCreateBuffers(1, &r_gui_vertex_buffer);
//    glCreateBuffers(1, &r_gui_index_buffer);
}

void r_ShutdownGui()
{
    glDeleteProgram(r_gui_shader_program);
    glDeleteShader(r_gui_vertex_shader);
    glDeleteShader(r_gui_fragment_shader);

    r_Free(r_gui_vertex_buffer);
    r_Free(r_gui_index_buffer);
//    glDeleteBuffers(1, &r_gui_vertex_buffer);
//    glDeleteBuffers(1, &r_gui_fragment_shader);
}

void r_DrawGui()
{
    r_QueueCmd(R_CMD_DRAW_GUI, NULL, 0);
    r_WaitEmptyQueue();
}


void r_be_DrawGui()
{
    int i;
    int j;
    ImDrawData *draw_data = NULL;
    ImDrawList *cmd_list = NULL;
    ImDrawCmd *cmd = NULL;

    mat4_t gui_projection;

    vec4_t clip_rect;

    gui_projection.identity();
    gui_projection[0][0] = 2.0 / r_renderer.renderer_width;
    gui_projection[1][1] = -2.0 / r_renderer.renderer_height;
    gui_projection[2][2] = -1.0;
    gui_projection[3][3] = 1.0;
    gui_projection[3][0] = -1.0;
    gui_projection[3][1] = 1.0;

    draw_data = ImGui::GetDrawData();

    glUseProgram(r_gui_shader_program);

    glEnableVertexAttribArray(r_gui_shader_vertex_position);
    glEnableVertexAttribArray(r_gui_shader_vertex_color);
    glEnableVertexAttribArray(r_gui_shader_vertex_tex_coord);

    glVertexAttribPointer(r_gui_shader_vertex_position, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
    glVertexAttribPointer(r_gui_shader_vertex_tex_coord, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
    glVertexAttribPointer(r_gui_shader_vertex_color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid *)IM_OFFSETOF(ImDrawVert, col));

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
//    glEnable(GL_SCISSOR_TEST);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(r_gui_shader_atlas, 0);
    glUniformMatrix4fv(r_gui_shader_projection_matrix, 1, GL_FALSE, (GLfloat *)gui_projection.floats);
//    glScissor(0, 0, r_renderer.window_width, r_renderer.window_height);

    for(i = 0; i < draw_data->CmdListsCount; i++)
    {
        cmd_list = draw_data->CmdLists[i];

        r_MemcpyToUnmapped(r_gui_vertex_buffer, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));

        for(j = 0; j < cmd_list->IdxBuffer.Size; j++)
        {
            r_gui_indexes[j] = cmd_list->IdxBuffer[j] + r_gui_vert_start;
        }

        r_MemcpyToUnmapped(r_gui_index_buffer, r_gui_indexes, cmd_list->IdxBuffer.Size * sizeof(unsigned int));

        for(j = 0; j < cmd_list->CmdBuffer.Size; j++)
        {
            cmd = &cmd_list->CmdBuffer[j];

//            clip_rect.x = cmd->ClipRect.x;
//            clip_rect.y = r_renderer.window_height - cmd->ClipRect.y;
////            clip_rect.z = cmd->ClipRect.w - clip_rect.x;
//            clip_rect.z = r_renderer.window_width;
//            clip_rect.w = r_renderer.window_height;
//            clip_rect.w = clip_rect.y - (r_renderer.window_height - cmd->ClipRect.w);

//            printf("[%f %f %f %f]\n", clip_rect.x, clip_rect.y, clip_rect.z, clip_rect.w);
//            glScissor(clip_rect.x, clip_rect.y, clip_rect.z, clip_rect.w);
            glBindTexture(GL_TEXTURE_2D, (unsigned long long )cmd->TextureId);
            glDrawElements(GL_TRIANGLES, cmd->ElemCount, GL_UNSIGNED_INT, (void *)((cmd->IdxOffset + r_gui_index_start) * sizeof(unsigned int )));
        }
    }

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
}

#ifdef __cplusplus
}
#endif











