// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include "imgui.h"

namespace luvk_example
{
    class ImGuiBackendSDL2
    {
        SDL_Window* m_Window{};

    public:
        ImGuiBackendSDL2() = default;

        bool Init(SDL_Window* Window);
        void Shutdown();
        void NewFrame() const;
        bool ProcessEvent(SDL_Event const& Event) const;
    };
} // namespace luvk_example
