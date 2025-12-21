// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/UserInterface/Backend/ImGuiBackendSDL.hpp"
#include <imgui.h>
#include <ostream>
#include <volk.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_video.h>

using namespace luvk_example;

static ImGuiKey SDLKeyToImGuiKey(std::uint32_t Keycode, SDL_Scancode Scancode);
static void     UpdateModifiers(SDL_Keymod Mods);

ImGuiBackendSDL::ImGuiBackendSDL(SDL_Window* Window)
    : m_Window(Window)
{
    ImGuiIO& GuiIO            = ImGui::GetIO();
    GuiIO.BackendPlatformName = "ImGuiBackendSDL";
    GuiIO.BackendFlags        |= ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos;

    m_MouseCursors.at(ImGuiMouseCursor_Arrow)      = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    m_MouseCursors.at(ImGuiMouseCursor_TextInput)  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_TEXT);
    m_MouseCursors.at(ImGuiMouseCursor_ResizeAll)  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
    m_MouseCursors.at(ImGuiMouseCursor_ResizeNS)   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
    m_MouseCursors.at(ImGuiMouseCursor_ResizeEW)   = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
    m_MouseCursors.at(ImGuiMouseCursor_ResizeNESW) = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
    m_MouseCursors.at(ImGuiMouseCursor_ResizeNWSE) = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
    m_MouseCursors.at(ImGuiMouseCursor_Hand)       = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER);
    m_MouseCursors.at(ImGuiMouseCursor_NotAllowed) = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NOT_ALLOWED);
}

ImGuiBackendSDL::~ImGuiBackendSDL()
{
    for (SDL_Cursor*& Cursor : m_MouseCursors)
    {
        if (Cursor)
        {
            SDL_DestroyCursor(Cursor);
            Cursor = nullptr;
        }
    }
}

void ImGuiBackendSDL::NewFrame() const
{
    ImGuiIO& GuiIO = ImGui::GetIO();

    std::int32_t Width  = 0.0f;
    std::int32_t Height = 0.0f;
    SDL_GetWindowSize(m_Window, &Width, &Height);

    std::int32_t DrawWidth  = 0;
    std::int32_t DrawHeight = 0;
    SDL_GetWindowSizeInPixels(m_Window, &DrawWidth, &DrawHeight);

    GuiIO.DisplaySize = ImVec2(static_cast<float>(Width), static_cast<float>(Height));

    if (Width > 0 && Height > 0)
    {
        GuiIO.DisplayFramebufferScale = ImVec2(static_cast<float>(DrawWidth) / static_cast<float>(Width),
                                               static_cast<float>(DrawHeight) / static_cast<float>(Height));
    }

    if (!(GuiIO.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange))
    {
        if (const ImGuiMouseCursor ImgCursor = ImGui::GetMouseCursor();
            GuiIO.MouseDrawCursor || ImgCursor == ImGuiMouseCursor_None)
        {
            SDL_HideCursor();
        }
        else
        {
            SDL_Cursor* const Cursor = m_MouseCursors.at(ImgCursor)
                                           ? m_MouseCursors.at(ImgCursor)
                                           : m_MouseCursors.at(ImGuiMouseCursor_Arrow);
            SDL_SetCursor(Cursor);
            SDL_ShowCursor();
        }
    }
}

bool ImGuiBackendSDL::ProcessEvent(const SDL_Event& Event) const
{
    ImGuiIO& GuiIO = ImGui::GetIO();

    switch (Event.type)
    {
    case SDL_EVENT_MOUSE_WHEEL:
        {
            GuiIO.AddMouseWheelEvent(Event.wheel.x, Event.wheel.y);
            return true;
        }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            if (Event.button.button == SDL_BUTTON_LEFT)
            {
                GuiIO.AddMouseButtonEvent(0, Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            }
            else if (Event.button.button == SDL_BUTTON_RIGHT)
            {
                GuiIO.AddMouseButtonEvent(1, Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            }
            else if (Event.button.button == SDL_BUTTON_MIDDLE)
            {
                GuiIO.AddMouseButtonEvent(2, Event.type == SDL_EVENT_MOUSE_BUTTON_DOWN);
            }

            return true;
        }
    case SDL_EVENT_MOUSE_MOTION:
        {
            GuiIO.AddMousePosEvent(Event.motion.x, Event.motion.y);
            return true;
        }
    case SDL_EVENT_TEXT_INPUT:
        {
            GuiIO.AddInputCharactersUTF8(Event.text.text);
            return true;
        }
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP:
        {
            UpdateModifiers(Event.key.mod);

            const ImGuiKey Key = SDLKeyToImGuiKey(Event.key.key, Event.key.scancode);
            GuiIO.AddKeyEvent(Key, Event.type == SDL_EVENT_KEY_DOWN);

            return true;
        }
    default: break;
    }
    return false;
}

static ImGuiKey SDLKeyToImGuiKey(const std::uint32_t Keycode, const SDL_Scancode Scancode)
{
    switch (Scancode)
    {
    case SDL_SCANCODE_KP_0: return ImGuiKey_Keypad0;
    case SDL_SCANCODE_KP_1: return ImGuiKey_Keypad1;
    case SDL_SCANCODE_KP_2: return ImGuiKey_Keypad2;
    case SDL_SCANCODE_KP_3: return ImGuiKey_Keypad3;
    case SDL_SCANCODE_KP_4: return ImGuiKey_Keypad4;
    case SDL_SCANCODE_KP_5: return ImGuiKey_Keypad5;
    case SDL_SCANCODE_KP_6: return ImGuiKey_Keypad6;
    case SDL_SCANCODE_KP_7: return ImGuiKey_Keypad7;
    case SDL_SCANCODE_KP_8: return ImGuiKey_Keypad8;
    case SDL_SCANCODE_KP_9: return ImGuiKey_Keypad9;
    case SDL_SCANCODE_KP_PERIOD: return ImGuiKey_KeypadDecimal;
    case SDL_SCANCODE_KP_DIVIDE: return ImGuiKey_KeypadDivide;
    case SDL_SCANCODE_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
    case SDL_SCANCODE_KP_MINUS: return ImGuiKey_KeypadSubtract;
    case SDL_SCANCODE_KP_PLUS: return ImGuiKey_KeypadAdd;
    case SDL_SCANCODE_KP_ENTER: return ImGuiKey_KeypadEnter;
    case SDL_SCANCODE_KP_EQUALS: return ImGuiKey_KeypadEqual;
    default: break;
    }

    switch (Keycode)
    {
    case SDLK_TAB: return ImGuiKey_Tab;
    case SDLK_LEFT: return ImGuiKey_LeftArrow;
    case SDLK_RIGHT: return ImGuiKey_RightArrow;
    case SDLK_UP: return ImGuiKey_UpArrow;
    case SDLK_DOWN: return ImGuiKey_DownArrow;
    case SDLK_PAGEUP: return ImGuiKey_PageUp;
    case SDLK_PAGEDOWN: return ImGuiKey_PageDown;
    case SDLK_HOME: return ImGuiKey_Home;
    case SDLK_END: return ImGuiKey_End;
    case SDLK_INSERT: return ImGuiKey_Insert;
    case SDLK_DELETE: return ImGuiKey_Delete;
    case SDLK_BACKSPACE: return ImGuiKey_Backspace;
    case SDLK_SPACE: return ImGuiKey_Space;
    case SDLK_RETURN: return ImGuiKey_Enter;
    case SDLK_ESCAPE: return ImGuiKey_Escape;
    case SDLK_APOSTROPHE: return ImGuiKey_Apostrophe;
    case SDLK_COMMA: return ImGuiKey_Comma;
    case SDLK_MINUS: return ImGuiKey_Minus;
    case SDLK_PERIOD: return ImGuiKey_Period;
    case SDLK_SLASH: return ImGuiKey_Slash;
    case SDLK_SEMICOLON: return ImGuiKey_Semicolon;
    case SDLK_EQUALS: return ImGuiKey_Equal;
    case SDLK_LEFTBRACKET: return ImGuiKey_LeftBracket;
    case SDLK_BACKSLASH: return ImGuiKey_Backslash;
    case SDLK_RIGHTBRACKET: return ImGuiKey_RightBracket;
    case SDLK_GRAVE: return ImGuiKey_GraveAccent;
    case SDLK_CAPSLOCK: return ImGuiKey_CapsLock;
    case SDLK_SCROLLLOCK: return ImGuiKey_ScrollLock;
    case SDLK_NUMLOCKCLEAR: return ImGuiKey_NumLock;
    case SDLK_PRINTSCREEN: return ImGuiKey_PrintScreen;
    case SDLK_PAUSE: return ImGuiKey_Pause;
    case SDLK_LCTRL: return ImGuiKey_LeftCtrl;
    case SDLK_LSHIFT: return ImGuiKey_LeftShift;
    case SDLK_LALT: return ImGuiKey_LeftAlt;
    case SDLK_LGUI: return ImGuiKey_LeftSuper;
    case SDLK_RCTRL: return ImGuiKey_RightCtrl;
    case SDLK_RSHIFT: return ImGuiKey_RightShift;
    case SDLK_RALT: return ImGuiKey_RightAlt;
    case SDLK_RGUI: return ImGuiKey_RightSuper;
    case SDLK_APPLICATION: return ImGuiKey_Menu;
    case SDLK_0: return ImGuiKey_0;
    case SDLK_1: return ImGuiKey_1;
    case SDLK_2: return ImGuiKey_2;
    case SDLK_3: return ImGuiKey_3;
    case SDLK_4: return ImGuiKey_4;
    case SDLK_5: return ImGuiKey_5;
    case SDLK_6: return ImGuiKey_6;
    case SDLK_7: return ImGuiKey_7;
    case SDLK_8: return ImGuiKey_8;
    case SDLK_9: return ImGuiKey_9;
    case SDLK_A: return ImGuiKey_A;
    case SDLK_B: return ImGuiKey_B;
    case SDLK_C: return ImGuiKey_C;
    case SDLK_D: return ImGuiKey_D;
    case SDLK_E: return ImGuiKey_E;
    case SDLK_F: return ImGuiKey_F;
    case SDLK_G: return ImGuiKey_G;
    case SDLK_H: return ImGuiKey_H;
    case SDLK_I: return ImGuiKey_I;
    case SDLK_J: return ImGuiKey_J;
    case SDLK_K: return ImGuiKey_K;
    case SDLK_L: return ImGuiKey_L;
    case SDLK_M: return ImGuiKey_M;
    case SDLK_N: return ImGuiKey_N;
    case SDLK_O: return ImGuiKey_O;
    case SDLK_P: return ImGuiKey_P;
    case SDLK_Q: return ImGuiKey_Q;
    case SDLK_R: return ImGuiKey_R;
    case SDLK_S: return ImGuiKey_S;
    case SDLK_T: return ImGuiKey_T;
    case SDLK_U: return ImGuiKey_U;
    case SDLK_V: return ImGuiKey_V;
    case SDLK_W: return ImGuiKey_W;
    case SDLK_X: return ImGuiKey_X;
    case SDLK_Y: return ImGuiKey_Y;
    case SDLK_Z: return ImGuiKey_Z;
    case SDLK_F1: return ImGuiKey_F1;
    case SDLK_F2: return ImGuiKey_F2;
    case SDLK_F3: return ImGuiKey_F3;
    case SDLK_F4: return ImGuiKey_F4;
    case SDLK_F5: return ImGuiKey_F5;
    case SDLK_F6: return ImGuiKey_F6;
    case SDLK_F7: return ImGuiKey_F7;
    case SDLK_F8: return ImGuiKey_F8;
    case SDLK_F9: return ImGuiKey_F9;
    case SDLK_F10: return ImGuiKey_F10;
    case SDLK_F11: return ImGuiKey_F11;
    case SDLK_F12: return ImGuiKey_F12;
    case SDLK_F13: return ImGuiKey_F13;
    case SDLK_F14: return ImGuiKey_F14;
    case SDLK_F15: return ImGuiKey_F15;
    case SDLK_F16: return ImGuiKey_F16;
    case SDLK_F17: return ImGuiKey_F17;
    case SDLK_F18: return ImGuiKey_F18;
    case SDLK_F19: return ImGuiKey_F19;
    case SDLK_F20: return ImGuiKey_F20;
    case SDLK_F21: return ImGuiKey_F21;
    case SDLK_F22: return ImGuiKey_F22;
    case SDLK_F23: return ImGuiKey_F23;
    case SDLK_F24: return ImGuiKey_F24;
    case SDLK_AC_BACK: return ImGuiKey_AppBack;
    case SDLK_AC_FORWARD: return ImGuiKey_AppForward;
    default: break;
    }

    switch (Scancode)
    {
    case SDL_SCANCODE_GRAVE: return ImGuiKey_GraveAccent;
    case SDL_SCANCODE_MINUS: return ImGuiKey_Minus;
    case SDL_SCANCODE_EQUALS: return ImGuiKey_Equal;
    case SDL_SCANCODE_LEFTBRACKET: return ImGuiKey_LeftBracket;
    case SDL_SCANCODE_RIGHTBRACKET: return ImGuiKey_RightBracket;
    case SDL_SCANCODE_NONUSBACKSLASH: return ImGuiKey_Oem102;
    case SDL_SCANCODE_BACKSLASH: return ImGuiKey_Backslash;
    case SDL_SCANCODE_SEMICOLON: return ImGuiKey_Semicolon;
    case SDL_SCANCODE_APOSTROPHE: return ImGuiKey_Apostrophe;
    case SDL_SCANCODE_COMMA: return ImGuiKey_Comma;
    case SDL_SCANCODE_PERIOD: return ImGuiKey_Period;
    case SDL_SCANCODE_SLASH: return ImGuiKey_Slash;
    default: break;
    }

    return ImGuiKey_None;
}

static void UpdateModifiers(const SDL_Keymod Mods)
{
    ImGuiIO& GuiIO = ImGui::GetIO();
    GuiIO.AddKeyEvent(ImGuiMod_Ctrl, (Mods & SDL_KMOD_CTRL) != 0);
    GuiIO.AddKeyEvent(ImGuiMod_Shift, (Mods & SDL_KMOD_SHIFT) != 0);
    GuiIO.AddKeyEvent(ImGuiMod_Alt, (Mods & SDL_KMOD_ALT) != 0);
    GuiIO.AddKeyEvent(ImGuiMod_Super, (Mods & SDL_KMOD_GUI) != 0);
}
