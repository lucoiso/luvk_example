// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <luvk/Types/Array.hpp>
#include <SDL3/SDL_events.h>

class SDL_Window;
class SDL_Cursor;

namespace luvk_example
{
    class ImGuiBackendSDL
    {
        SDL_Window*                  m_Window{};
        luvk::Array<SDL_Cursor*, 11> m_MouseCursors{};

    public:
        ImGuiBackendSDL() = delete;
        explicit ImGuiBackendSDL(SDL_Window* Window);
        ~ImGuiBackendSDL();

        void               NewFrame() const;
        [[nodiscard]] bool ProcessEvent(const SDL_Event& Event) const;
    };
} // namespace luvk_example
