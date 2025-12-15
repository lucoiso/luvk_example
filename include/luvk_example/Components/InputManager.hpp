// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <luvk/Types/Vector.hpp>
#include <SDL3/SDL_events.h>

namespace luvk_example
{
    class InputManager
    {
        using EventCallbacks = luvk::Vector<std::function<void(const SDL_Event&)>>;

        std::unordered_map<Uint32, EventCallbacks> m_Bindings{};
        std::unordered_set<SDL_Keycode>            m_PressedKeys{};
        bool                                       m_Running{true};
        bool                                       m_LeftHeld{false};

    public:
        InputManager()  = default;
        ~InputManager() = default;

        void BindEvent(Uint32 Type, std::function<void(const SDL_Event&)>&& Callback);
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
        void InvokeCallbacks(Uint32 Type, const SDL_Event& Event);
    };
} // namespace luvk_example
