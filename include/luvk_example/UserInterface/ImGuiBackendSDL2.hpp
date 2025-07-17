// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <SDL2/SDL.h>
#include <array>
#include <imgui.h>

namespace luvk_example
{
    /** SDL2 backend for ImGui integration */
    class ImGuiBackendSDL2
    {
        SDL_Window* m_Window{};
        std::array<SDL_Cursor*, ImGuiMouseCursor_COUNT> m_MouseCursors{};

    public:
        /** Constructor */
        constexpr ImGuiBackendSDL2() = default;

        /** Initializes using the SDL window */
        bool Init(SDL_Window* Window);

        /** Releases resources */
        void Shutdown();

        /** Begins a new frame */
        void NewFrame() const;

        /** Processes an SDL event */
        bool ProcessEvent(SDL_Event const& Event) const;
    };
} // namespace luvk_example
