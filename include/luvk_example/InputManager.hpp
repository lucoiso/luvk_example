// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <SDL2/SDL.h>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstdint>

namespace luvk_example
{
    class InputManager
    {
        SDL_Window* m_Window{};
        bool m_Running{true};
        bool m_LeftHeld{false};
        std::unordered_map<Uint32, std::vector<std::function<void(SDL_Event const&)>>> m_Bindings{};
        std::unordered_set<SDL_Keycode> m_PressedKeys{};

    public:
        InputManager(std::uint32_t Width, std::uint32_t Height);
        ~InputManager();

        void BindEvent(Uint32 Type, std::function<void(SDL_Event const&)> const& Callback);
        void ProcessEvents();

        [[nodiscard]] bool Running() const noexcept;
        [[nodiscard]] bool IsKeyPressed(SDL_Keycode key) const noexcept;
        [[nodiscard]] bool LeftHeld() const noexcept;
        [[nodiscard]] SDL_Window* GetWindow() const noexcept;
    };
} // namespace luvk_example
