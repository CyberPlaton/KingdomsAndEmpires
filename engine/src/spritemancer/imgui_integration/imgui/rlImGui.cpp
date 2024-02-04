/**********************************************************************************************
*
*   raylibExtras * Utilities and Shared Components for Raylib
*
*   rlImGui * basic ImGui integration
*
*   LICENSE: ZLIB
*
*   Copyright (c) 2020 Jeffery Myers
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy
*   of this software and associated documentation files (the "Software"), to deal
*   in the Software without restriction, including without limitation the rights
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*   copies of the Software, and to permit persons to whom the Software is
*   furnished to do so, subject to the following conditions:
*
*   The above copyright notice and this permission notice shall be included in all
*   copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*   SOFTWARE.
*
**********************************************************************************************/
#include "rlImGui.h"
#include "imgui.h"
#include <raylib/GLFW/glfw3.h>
#include <math.h>
#include <vector>
#include <map>

#ifndef NO_FONT_AWESOME
#include "FA6FreeSolidFontData.h"
#endif

using namespace raylib;

static std::vector<raylib::Texture> LoadedTextures;
static raylib::Texture2D FontTexture;

static ImGuiMouseCursor CurrentMouseCursor = ImGuiMouseCursor_COUNT;
static std::map<ImGuiMouseCursor, raylib::MouseCursor> MouseCursorMap;

static const char* rlImGuiGetClipText(void*)
{
    return raylib::GetClipboardText();
}

static void rlImGuiSetClipText(void*, const char* text)
{
	raylib::SetClipboardText(text);
}

static void rlImGuiNewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    if (raylib::IsWindowFullscreen())
    {
        int monitor = raylib::GetCurrentMonitor();
        io.DisplaySize.x = float(raylib::GetMonitorWidth(monitor));
        io.DisplaySize.y = float(raylib::GetMonitorHeight(monitor));
    }
    else
    {
        io.DisplaySize.x = float(raylib::GetScreenWidth());
        io.DisplaySize.y = float(raylib::GetScreenHeight());
    }

    int width = io.DisplaySize.x, height = io.DisplaySize.y;
#ifdef PLATFORM_DESKTOP
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
#endif
    if (width > 0 && height > 0) {
        io.DisplayFramebufferScale = ImVec2((float)width / io.DisplaySize.x, (float)height / io.DisplaySize.y);
    }
    else {
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    }

    io.DeltaTime = raylib::GetFrameTime();

    io.KeyCtrl = raylib::IsKeyDown(raylib::KEY_RIGHT_CONTROL) || raylib::IsKeyDown(raylib::KEY_LEFT_CONTROL);
    io.KeyShift = raylib::IsKeyDown(raylib::KEY_RIGHT_SHIFT) || raylib::IsKeyDown(raylib::KEY_LEFT_SHIFT);
    io.KeyAlt = raylib::IsKeyDown(raylib::KEY_RIGHT_ALT) || raylib::IsKeyDown(raylib::KEY_LEFT_ALT);
    io.KeySuper = raylib::IsKeyDown(raylib::KEY_RIGHT_SUPER) || raylib::IsKeyDown(raylib::KEY_LEFT_SUPER);

    if (io.WantSetMousePos)
    {
		raylib::SetMousePosition((int)io.MousePos.x, (int)io.MousePos.y);
    }
    else
    {
        io.MousePos.x = (float)raylib::GetMouseX();
        io.MousePos.y = (float)raylib::GetMouseY();
    }

    io.MouseDown[0] = raylib::IsMouseButtonDown(raylib::MOUSE_LEFT_BUTTON);
    io.MouseDown[1] = raylib::IsMouseButtonDown(raylib::MOUSE_RIGHT_BUTTON);
    io.MouseDown[2] = raylib::IsMouseButtonDown(raylib::MOUSE_MIDDLE_BUTTON);

    if (raylib::GetMouseWheelMove() > 0)
        io.MouseWheel += 1;
    else if (raylib::GetMouseWheelMove() < 0)
        io.MouseWheel -= 1;

    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
    {
        ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
        if (imgui_cursor != CurrentMouseCursor || io.MouseDrawCursor)
        {
            CurrentMouseCursor = imgui_cursor;
            if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
            {
				raylib::HideCursor();
            }
            else
            {
				raylib::ShowCursor();

                if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
                {
                    auto itr = MouseCursorMap.find(imgui_cursor);
                    if (itr == MouseCursorMap.end())
						raylib::SetMouseCursor(MOUSE_CURSOR_DEFAULT);
                    else
						raylib::SetMouseCursor(itr->second);
                }
            }
        }
    }
}

#define FOR_ALL_KEYS(X) \
    do { \
    X(KEY_APOSTROPHE); \
    X(KEY_COMMA); \
    X(KEY_MINUS); \
    X(KEY_PERIOD); \
    X(KEY_SLASH); \
    X(KEY_ZERO); \
    X(KEY_ONE); \
    X(KEY_TWO); \
    X(KEY_THREE); \
    X(KEY_FOUR); \
    X(KEY_FIVE); \
    X(KEY_SIX); \
    X(KEY_SEVEN); \
    X(KEY_EIGHT); \
    X(KEY_NINE); \
    X(KEY_SEMICOLON); \
    X(KEY_EQUAL); \
    X(KEY_A); \
    X(KEY_B); \
    X(KEY_C); \
    X(KEY_D); \
    X(KEY_E); \
    X(KEY_F); \
    X(KEY_G); \
    X(KEY_H); \
    X(KEY_I); \
    X(KEY_J); \
    X(KEY_K); \
    X(KEY_L); \
    X(KEY_M); \
    X(KEY_N); \
    X(KEY_O); \
    X(KEY_P); \
    X(KEY_Q); \
    X(KEY_R); \
    X(KEY_S); \
    X(KEY_T); \
    X(KEY_U); \
    X(KEY_V); \
    X(KEY_W); \
    X(KEY_X); \
    X(KEY_Y); \
    X(KEY_Z); \
    X(KEY_SPACE); \
    X(KEY_ESCAPE); \
    X(KEY_ENTER); \
    X(KEY_TAB); \
    X(KEY_BACKSPACE); \
    X(KEY_INSERT); \
    X(KEY_DELETE); \
    X(KEY_RIGHT); \
    X(KEY_LEFT); \
    X(KEY_DOWN); \
    X(KEY_UP); \
    X(KEY_PAGE_UP); \
    X(KEY_PAGE_DOWN); \
    X(KEY_HOME); \
    X(KEY_END); \
    X(KEY_CAPS_LOCK); \
    X(KEY_SCROLL_LOCK); \
    X(KEY_NUM_LOCK); \
    X(KEY_PRINT_SCREEN); \
    X(KEY_PAUSE); \
    X(KEY_F1); \
    X(KEY_F2); \
    X(KEY_F3); \
    X(KEY_F4); \
    X(KEY_F5); \
    X(KEY_F6); \
    X(KEY_F7); \
    X(KEY_F8); \
    X(KEY_F9); \
    X(KEY_F10); \
    X(KEY_F11); \
    X(KEY_F12); \
    X(KEY_LEFT_SHIFT); \
    X(KEY_LEFT_CONTROL); \
    X(KEY_LEFT_ALT); \
    X(KEY_LEFT_SUPER); \
    X(KEY_RIGHT_SHIFT); \
    X(KEY_RIGHT_CONTROL); \
    X(KEY_RIGHT_ALT); \
    X(KEY_RIGHT_SUPER); \
    X(KEY_KB_MENU); \
    X(KEY_LEFT_BRACKET); \
    X(KEY_BACKSLASH); \
    X(KEY_RIGHT_BRACKET); \
    X(KEY_GRAVE); \
    X(KEY_KP_0); \
    X(KEY_KP_1); \
    X(KEY_KP_2); \
    X(KEY_KP_3); \
    X(KEY_KP_4); \
    X(KEY_KP_5); \
    X(KEY_KP_6); \
    X(KEY_KP_7); \
    X(KEY_KP_8); \
    X(KEY_KP_9); \
    X(KEY_KP_DECIMAL); \
    X(KEY_KP_DIVIDE); \
    X(KEY_KP_MULTIPLY); \
    X(KEY_KP_SUBTRACT); \
    X(KEY_KP_ADD); \
    X(KEY_KP_ENTER); \
    X(KEY_KP_EQUAL); \
    } while(0)

#define SET_KEY_DOWN(KEY) io.KeysDown[KEY] = IsKeyDown(KEY)


static void rlImGuiEvents()
{
    ImGuiIO& io = ImGui::GetIO();
    FOR_ALL_KEYS(SET_KEY_DOWN);

    unsigned int pressed = GetCharPressed();
    if (pressed != 0)
        io.AddInputCharacter(pressed);
}

static void rlImGuiTriangleVert(ImDrawVert& idx_vert)
{
    Color* c;
    c = (Color*)&idx_vert.col;
    raylib::rlColor4ub(c->r, c->g, c->b, c->a);
    raylib::rlTexCoord2f(idx_vert.uv.x, idx_vert.uv.y);
    raylib::rlVertex2f(idx_vert.pos.x, idx_vert.pos.y);
}

static void rlImGuiRenderTriangles(unsigned int count, int indexStart, const ImVector<ImDrawIdx>& indexBuffer, const ImVector<ImDrawVert>& vertBuffer, void* texturePtr)
{
    if (count < 3)
        return;
	
    Texture* texture = (Texture*)texturePtr;

    unsigned int textureId = (texture == nullptr) ? 0 : texture->id;

	raylib::rlBegin(RL_TRIANGLES);
	raylib::rlSetTexture(textureId);

    for (unsigned int i = 0; i <= (count - 3); i += 3)
    {
        if(raylib::rlCheckRenderBatchLimit(3))
        {
			raylib::rlBegin(RL_TRIANGLES);
			raylib::rlSetTexture(textureId);
        }

        ImDrawIdx indexA = indexBuffer[indexStart + i];
        ImDrawIdx indexB = indexBuffer[indexStart + i + 1];
        ImDrawIdx indexC = indexBuffer[indexStart + i + 2];

        ImDrawVert vertexA = vertBuffer[indexA];
        ImDrawVert vertexB = vertBuffer[indexB];
        ImDrawVert vertexC = vertBuffer[indexC];

        rlImGuiTriangleVert(vertexA);
        rlImGuiTriangleVert(vertexB);
        rlImGuiTriangleVert(vertexC);
    }
	raylib::rlEnd();
}

static void EnableScissor(float x, float y, float width, float height)
{
	raylib::rlEnableScissorTest();
    ImGuiIO& io = ImGui::GetIO();
	raylib::rlScissor((int)x * io.DisplayFramebufferScale.x, (GetScreenHeight() - (int)(y + height)) * io.DisplayFramebufferScale.y, (int)width * io.DisplayFramebufferScale.x, (int)height * io.DisplayFramebufferScale.y);
}

static void rlRenderData(ImDrawData* data)
{
	raylib::rlDrawRenderBatchActive();
	raylib::rlDisableBackfaceCulling();

    for (int l = 0; l < data->CmdListsCount; ++l)
    {
        const ImDrawList* commandList = data->CmdLists[l];

        for (const auto& cmd : commandList->CmdBuffer)
        {
            EnableScissor(cmd.ClipRect.x - data->DisplayPos.x, cmd.ClipRect.y - data->DisplayPos.y, cmd.ClipRect.z - (cmd.ClipRect.x - data->DisplayPos.x), cmd.ClipRect.w - (cmd.ClipRect.y - data->DisplayPos.y));
            if (cmd.UserCallback != nullptr)
            {
                cmd.UserCallback(commandList, &cmd);
  
                continue;
            }

			rlImGuiRenderTriangles(cmd.ElemCount, cmd.IdxOffset, commandList->IdxBuffer, commandList->VtxBuffer, cmd.TextureId);
			raylib::rlDrawRenderBatchActive();
        }
    }

	raylib::rlSetTexture(0);
	raylib::rlDisableScissorTest();
	raylib::rlEnableBackfaceCulling();
}

void SetupMouseCursors()
{
    MouseCursorMap.clear();
    MouseCursorMap[ImGuiMouseCursor_Arrow] = raylib::MOUSE_CURSOR_ARROW;
    MouseCursorMap[ImGuiMouseCursor_TextInput] = raylib::MOUSE_CURSOR_IBEAM;
    MouseCursorMap[ImGuiMouseCursor_Hand] = raylib::MOUSE_CURSOR_POINTING_HAND;
    MouseCursorMap[ImGuiMouseCursor_ResizeAll] = raylib::MOUSE_CURSOR_RESIZE_ALL;
    MouseCursorMap[ImGuiMouseCursor_ResizeEW] = raylib::MOUSE_CURSOR_RESIZE_EW;
    MouseCursorMap[ImGuiMouseCursor_ResizeNESW] = raylib::MOUSE_CURSOR_RESIZE_NESW;
    MouseCursorMap[ImGuiMouseCursor_ResizeNS] = raylib::MOUSE_CURSOR_RESIZE_NS;
    MouseCursorMap[ImGuiMouseCursor_ResizeNWSE] = raylib::MOUSE_CURSOR_RESIZE_NWSE;
    MouseCursorMap[ImGuiMouseCursor_NotAllowed] = raylib::MOUSE_CURSOR_NOT_ALLOWED;
}

void rlImGuiBeginInitImGui()
{
    ImGui::CreateContext(nullptr);
}

void rlImGuiEndInitImGui()
{
    SetupMouseCursors();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "imgui_impl_raylib";

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

    io.KeyMap[ImGuiKey_Tab] = raylib::KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = raylib::KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = raylib::KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = raylib::KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = raylib::KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = raylib::KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_PageDown] = raylib::KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_Home] = raylib::KEY_HOME;
    io.KeyMap[ImGuiKey_End] = raylib::KEY_END;
    io.KeyMap[ImGuiKey_Insert] = raylib::KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = raylib::KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = raylib::KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = raylib::KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = raylib::KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = raylib::KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = raylib::KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = raylib::KEY_A;
    io.KeyMap[ImGuiKey_C] = raylib::KEY_C;
    io.KeyMap[ImGuiKey_V] = raylib::KEY_V;
    io.KeyMap[ImGuiKey_X] = raylib::KEY_X;
    io.KeyMap[ImGuiKey_Y] = raylib::KEY_Y;
    io.KeyMap[ImGuiKey_Z] = raylib::KEY_Z;

    io.MousePos = ImVec2(0, 0);

    io.SetClipboardTextFn = rlImGuiSetClipText;
    io.GetClipboardTextFn = rlImGuiGetClipText;

    io.ClipboardUserData = nullptr;


    rlImGuiReloadFonts();
}

void rlImGuiSetup(bool dark)
{
    rlImGuiBeginInitImGui();

    if (dark)
        ImGui::StyleColorsDark();
    else
        ImGui::StyleColorsLight();

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontDefault();

#ifndef NO_FONT_AWESOME
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    io.Fonts->AddFontFromMemoryCompressedTTF((void*)fa_solid_900_compressed_data, fa_solid_900_compressed_size, 13, &icons_config, icons_ranges);
#endif

    rlImGuiEndInitImGui();
}

void rlImGuiReloadFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels = nullptr;

    int width;
    int height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, nullptr);
	raylib::Image image = raylib::GenImageColor(width, height, {0, 0, 0, 0});
    memcpy(image.data, pixels, width * height * 4);

    if (FontTexture.id != 0)
        UnloadTexture(FontTexture);

    FontTexture = raylib::LoadTextureFromImage(image);
    UnloadImage(image);
    io.Fonts->TexID = &FontTexture;
}

void rlImGuiBegin()
{
    rlImGuiNewFrame();
    rlImGuiEvents();
    ImGui::NewFrame();
}

void rlImGuiEnd()
{
    ImGui::Render();
    rlRenderData(ImGui::GetDrawData());
}

void rlImGuiShutdown()
{
    for (const auto& tx : LoadedTextures)
        UnloadTexture(tx);

    UnloadTexture(FontTexture);
    LoadedTextures.clear();
}

void rlImGuiImage(const raylib::Texture *image)
{
    ImGui::Image((ImTextureID)image, ImVec2(float(image->width), float(image->height)));
}

bool rlImGuiImageButton(const raylib::Texture *image) {
    return ImGui::ImageButton((ImTextureID)image, ImVec2(float(image->width), float(image->height)));
}

void rlImGuiImageSize(const raylib::Texture *image, int width, int height)
{
    ImGui::Image((ImTextureID)image, ImVec2(float(width), float(height)));
}

bool rlImGuiImageButtonSize(const raylib::Texture* image, int width, int height)
{
    return ImGui::ImageButton((ImTextureID)image, ImVec2(float(width), float(height)));
}

void rlImGuiImageRect(const raylib::Texture* image, int destWidth, int destHeight, raylib::Rectangle sourceRect)
{
    ImVec2 uv0;
    ImVec2 uv1;

    if (sourceRect.width < 0)
    {
        uv0.x = -((float)sourceRect.x / image->width);
        uv1.x = (uv0.x - (float)(fabs(sourceRect.width) / image->width));
    }
    else
    {
        uv0.x = (float)sourceRect.x / image->width;
        uv1.x = uv0.x + (float)(sourceRect.width / image->width);
    }

    if (sourceRect.height < 0)
    {
        uv0.y = -((float)sourceRect.y / image->height);
        uv1.y = (uv0.y - (float)(fabs(sourceRect.height) / image->height));
    }
    else
    {
        uv0.y = (float)sourceRect.y / image->height;
        uv1.y = uv0.y + (float)(sourceRect.height / image->height);
    }

    ImGui::Image((ImTextureID)image, ImVec2(float(destWidth), float(destHeight)),uv0,uv1);
}
