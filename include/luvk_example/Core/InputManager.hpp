// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace luvk_example
{
    class InputManager
    {
        using EventCallbacks = std::vector<std::function<void(SDL_Event const&)>>;

        bool m_Running{true};
        bool m_LeftHeld{false};
        std::unordered_map<Uint32, EventCallbacks> m_Bindings{};
        std::unordered_set<SDL_Keycode> m_PressedKeys{};

    public:
        InputManager();
        ~InputManager() = default;

        void BindEvent(Uint32 Type, std::function<void(SDL_Event const&)> const& Callback);
        void ProcessEvents();

        [[nodiscard]] bool Running() const noexcept;
        [[nodiscard]] bool IsKeyPressed(SDL_Keycode key) const noexcept;
        [[nodiscard]] bool LeftHeld() const noexcept;
    };
} // namespace luvk_example
