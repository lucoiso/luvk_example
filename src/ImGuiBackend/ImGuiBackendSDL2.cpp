// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/ImGuiBackend/ImGuiBackendSDL2.hpp"
#include <SDL2/SDL_vulkan.h>

#include "imgui.h"

using namespace luvk_example;

static ImGuiKey SDLKeyToImGuiKey(SDL_Keycode Keycode, SDL_Scancode Scancode);
static void UpdateModifiers(SDL_Keymod Mods);

bool ImGuiBackendSDL2::Init(SDL_Window* Window)
{
    m_Window = Window;
    ImGuiIO& io = ImGui::GetIO();
    io.BackendPlatformName = "ImGuiBackendSDL2";
    return true;
}

void ImGuiBackendSDL2::Shutdown()
{
    m_Window = nullptr;
}

void ImGuiBackendSDL2::NewFrame() const
{
    ImGuiIO& io = ImGui::GetIO();
    int w, h, dw, dh;
    SDL_GetWindowSize(m_Window, &w, &h);
    SDL_Vulkan_GetDrawableSize(m_Window, &dw, &dh);
    io.DisplaySize = ImVec2((float)w, (float)h);
    if (w > 0 && h > 0)
        io.DisplayFramebufferScale = ImVec2((float)dw / w, (float)dh / h);
}

bool ImGuiBackendSDL2::ProcessEvent(SDL_Event const& Event) const
{
    ImGuiIO& io = ImGui::GetIO();
    switch (Event.type)
    {
    case SDL_MOUSEWHEEL:
        io.AddMouseWheelEvent((float)Event.wheel.x, (float)Event.wheel.y);
        return true;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        if (Event.button.button == SDL_BUTTON_LEFT) io.AddMouseButtonEvent(0, Event.type == SDL_MOUSEBUTTONDOWN);
        if (Event.button.button == SDL_BUTTON_RIGHT) io.AddMouseButtonEvent(1, Event.type == SDL_MOUSEBUTTONDOWN);
        if (Event.button.button == SDL_BUTTON_MIDDLE) io.AddMouseButtonEvent(2, Event.type == SDL_MOUSEBUTTONDOWN);
        return true;
    case SDL_MOUSEMOTION:
        io.AddMousePosEvent((float)Event.motion.x, (float)Event.motion.y);
        return true;
    case SDL_TEXTINPUT:
        io.AddInputCharactersUTF8(Event.text.text);
        return true;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        {
            ImGuiKey Key = SDLKeyToImGuiKey(Event.key.keysym.sym, Event.key.keysym.scancode);
            io.AddKeyEvent(Key, Event.type == SDL_KEYDOWN);
            UpdateModifiers(static_cast<SDL_Keymod>(Event.key.keysym.mod));
        }
        return true;
    default:
        break;
    }
    return false;
}

static ImGuiKey SDLKeyToImGuiKey(SDL_Keycode Keycode, SDL_Scancode Scancode)
{
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
    case SDLK_COMMA: return ImGuiKey_Comma;
    case SDLK_PERIOD: return ImGuiKey_Period;
    case SDLK_SEMICOLON: return ImGuiKey_Semicolon;
    case SDLK_CAPSLOCK: return ImGuiKey_CapsLock;
    case SDLK_SCROLLLOCK: return ImGuiKey_ScrollLock;
    case SDLK_NUMLOCKCLEAR: return ImGuiKey_NumLock;
    case SDLK_PRINTSCREEN: return ImGuiKey_PrintScreen;
    case SDLK_PAUSE: return ImGuiKey_Pause;
    case SDLK_KP_0: return ImGuiKey_Keypad0;
    case SDLK_KP_1: return ImGuiKey_Keypad1;
    case SDLK_KP_2: return ImGuiKey_Keypad2;
    case SDLK_KP_3: return ImGuiKey_Keypad3;
    case SDLK_KP_4: return ImGuiKey_Keypad4;
    case SDLK_KP_5: return ImGuiKey_Keypad5;
    case SDLK_KP_6: return ImGuiKey_Keypad6;
    case SDLK_KP_7: return ImGuiKey_Keypad7;
    case SDLK_KP_8: return ImGuiKey_Keypad8;
    case SDLK_KP_9: return ImGuiKey_Keypad9;
    case SDLK_KP_PERIOD: return ImGuiKey_KeypadDecimal;
    case SDLK_KP_DIVIDE: return ImGuiKey_KeypadDivide;
    case SDLK_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
    case SDLK_KP_MINUS: return ImGuiKey_KeypadSubtract;
    case SDLK_KP_PLUS: return ImGuiKey_KeypadAdd;
    case SDLK_KP_ENTER: return ImGuiKey_KeypadEnter;
    case SDLK_LCTRL: return ImGuiKey_LeftCtrl;
    case SDLK_LSHIFT: return ImGuiKey_LeftShift;
    case SDLK_LALT: return ImGuiKey_LeftAlt;
    case SDLK_LGUI: return ImGuiKey_LeftSuper;
    case SDLK_RCTRL: return ImGuiKey_RightCtrl;
    case SDLK_RSHIFT: return ImGuiKey_RightShift;
    case SDLK_RALT: return ImGuiKey_RightAlt;
    case SDLK_RGUI: return ImGuiKey_RightSuper;
    case SDLK_APPLICATION: return ImGuiKey_Menu;
    default: break;
    }

    switch (Scancode)
    {
    case SDL_SCANCODE_GRAVE: return ImGuiKey_GraveAccent;
    case SDL_SCANCODE_MINUS: return ImGuiKey_Minus;
    case SDL_SCANCODE_EQUALS: return ImGuiKey_Equal;
    case SDL_SCANCODE_LEFTBRACKET: return ImGuiKey_LeftBracket;
    case SDL_SCANCODE_RIGHTBRACKET: return ImGuiKey_RightBracket;
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

static void UpdateModifiers(SDL_Keymod Mods)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiMod_Ctrl, (Mods & KMOD_CTRL) != 0);
    io.AddKeyEvent(ImGuiMod_Shift, (Mods & KMOD_SHIFT) != 0);
    io.AddKeyEvent(ImGuiMod_Alt, (Mods & KMOD_ALT) != 0);
    io.AddKeyEvent(ImGuiMod_Super, (Mods & KMOD_GUI) != 0);
}
