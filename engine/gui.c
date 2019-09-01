#include "gui.h"
#include "r_surf.h"
#include "r_gui.h"
#include "r_common.h"
#include "r_main.h"
//#include "be_backend.h"
#include "input.h"

#include <stdio.h>

struct texture_handle_t gui_texture_handle;

extern struct renderer_t r_renderer;

#ifdef __cplusplus
extern "C"
{
#endif

int32_t gui_Init()
{
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    struct texture_t *texture;
    unsigned char *pixels;
    int width;
    int height;

    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    gui_texture_handle = r_CreateEmptyTexture();

    struct upload_texture_pixels_params_t params;

    params.file_name = "ImGui font atlas";
    params.texture_data = pixels;
    params.texture_handle = gui_texture_handle;
    params.texture_params.width = width;
    params.texture_params.height = height;

    r_UploadTexturePixels(&params);
    r_WaitEmptyQueue();

    texture = r_GetTexturePointer(gui_texture_handle);
    io.Fonts->TexID = (ImTextureID)texture->gl_handle;

    io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
    io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
    io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
    io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
    io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
    io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
    io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
    io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
    io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

    return 0;
}

void gui_Shutdown()
{

}


void gui_BeginFrame()
{
    ImGuiIO &io = ImGui::GetIO();
    vec2_t mouse_pos = in_GetMousePos();
    int mouse_status = in_GetMouseStatus();
    unsigned char *text_buffer;
    unsigned char *keys;
    int i;

    io.DisplaySize.x = r_renderer.renderer_width;
    io.DisplaySize.y = r_renderer.renderer_height;

    io.MousePos.x = r_renderer.renderer_width * (mouse_pos.x * 0.5 + 0.5);
    io.MousePos.y = r_renderer.renderer_height * (1.0 - (mouse_pos.y * 0.5 + 0.5));

    io.MouseDown[0] = (mouse_status & MOUSE_STATUS_LEFT_BUTTON_PRESSED) && 1;
    io.MouseDown[1] = (mouse_status & MOUSE_STATUS_RIGHT_BUTTON_PRESSED) && 1;
    io.MouseDown[2] = (mouse_status & MOUSE_STATUS_MIDDLE_BUTTON_PRESSED) && 1;

    if(io.WantTextInput)
    {
        in_TextInput(1);

        text_buffer = in_GetTextInputBuffer();

        while(*text_buffer)
        {
            io.AddInputCharacter(*text_buffer);
            text_buffer++;
        }

        keys = in_GetKeys();

        for(i = 0; i < 512; i++)
        {
            io.KeysDown[i] = keys[i];
        }

        in_ClearTextInputBuffer();
    }
    else
    {
        in_TextInput(0);
    }


    ImGui::NewFrame();
}

void gui_EndFrame()
{
    ImGui::Render();
    r_DrawGui();
}

#ifdef __cplusplus
}
#endif




