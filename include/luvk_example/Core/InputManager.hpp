// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <SDL2/SDL.h>

namespace luvk_example
{
    class InputManager
    {
        using EventCallbacks = std::vector<std::function<void(SDL_Event const&)>>;

        std::unordered_map<Uint32, EventCallbacks> m_Bindings{};
        std::unordered_set<SDL_Keycode> m_PressedKeys{};
        bool m_Running{true};
        bool m_LeftHeld{false};

    public:
        InputManager();
        ~InputManager() = default;

        void BindEvent(Uint32 Type, std::function<void(SDL_Event const&)> const& Callback);
        void ProcessEvents();

        [[nodiscard]] constexpr bool Running() const noexcept
        {
            return m_Running;
        }

        [[nodiscard]] bool IsKeyPressed(SDL_Keycode key) const noexcept;

        [[nodiscard]] constexpr bool LeftHeld() const noexcept
        {
            return m_LeftHeld;
        }

    private:
        void InvokeCallbacks(Uint32 Type, SDL_Event const& Event);
    };
} // namespace luvk_example
