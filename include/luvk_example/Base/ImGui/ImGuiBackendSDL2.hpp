// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <array>
#include <imgui.h>
#include <SDL2/SDL.h>

namespace luvk_example
{
    class ImGuiBackendSDL2
    {
        SDL_Window*                                     m_Window{};
        std::array<SDL_Cursor*, ImGuiMouseCursor_COUNT> m_MouseCursors{};

    public:
        constexpr ImGuiBackendSDL2() = default;

        [[nodiscard]] bool Init(SDL_Window* Window);
        void               Shutdown();
        void               NewFrame() const;
        [[nodiscard]] bool ProcessEvent(const SDL_Event& Event) const;
    };
} // namespace luvk_example
