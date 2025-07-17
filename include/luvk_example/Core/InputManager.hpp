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
    /** Handles input events */
    class InputManager
    {
        using EventCallbacks = std::vector<std::function<void(SDL_Event const&)>>;

        std::unordered_map<Uint32, EventCallbacks> m_Bindings{};
        std::unordered_set<SDL_Keycode> m_PressedKeys{};
        bool m_Running{true};
        bool m_LeftHeld{false};

    public:
        /** Constructor */
        InputManager();
        ~InputManager() = default;

        /** Associates a callback with an SDL event */
        void BindEvent(Uint32 Type, std::function<void(SDL_Event const&)> const& Callback);

        /** Processes the event queue */
        void ProcessEvents();

        /** Whether the application is still running */
        [[nodiscard]] constexpr bool Running() const noexcept
        {
            return m_Running;
        }

        /** Checks if a key is pressed */
        [[nodiscard]] bool IsKeyPressed(SDL_Keycode key) const noexcept;

        /** Whether the left mouse button is held */
        [[nodiscard]] constexpr bool LeftHeld() const noexcept
        {
            return m_LeftHeld;
        }

    private:
        /** Executes the callbacks registered for the type */
        void InvokeCallbacks(Uint32 Type, SDL_Event const& Event);
    };
} // namespace luvk_example
