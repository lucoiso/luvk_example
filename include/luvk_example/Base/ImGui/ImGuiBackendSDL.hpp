// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <imgui.h>
#include <luvk/Types/Array.hpp>
#include <SDL3/SDL.h>

namespace luvk_example
{
    class ImGuiBackendSDL
    {
        SDL_Window*                                      m_Window{};
        luvk::Array<SDL_Cursor*, ImGuiMouseCursor_COUNT> m_MouseCursors{};

    public:
        constexpr ImGuiBackendSDL() = default;

        [[nodiscard]] bool Init(SDL_Window* Window);
        void               Shutdown();
        void               NewFrame() const;
        [[nodiscard]] bool ProcessEvent(const SDL_Event& Event) const;
    };
} // namespace luvk_example
