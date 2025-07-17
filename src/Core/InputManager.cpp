// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Core/InputManager.hpp"

using namespace luvk_example;

luvk_example::InputManager::InputManager() = default;

void luvk_example::InputManager::BindEvent(const Uint32 Type, std::function<void(SDL_Event const&)> const& Callback)
{
    auto [Iterator, Inserted] = m_Bindings.try_emplace(Type, EventCallbacks{});
    Iterator->second.push_back(Callback);
}

void luvk_example::InputManager::ProcessEvents()
{
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
        switch (Event.type)
        {
        case SDL_QUIT: m_Running = false; break;
        case SDL_KEYDOWN: m_PressedKeys.insert(Event.key.keysym.sym); break;
        case SDL_KEYUP: m_PressedKeys.erase(Event.key.keysym.sym); break;
        case SDL_MOUSEBUTTONDOWN:
            if (Event.button.button == SDL_BUTTON_LEFT)
            {
                m_LeftHeld = true;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (Event.button.button == SDL_BUTTON_LEFT)
            {
                m_LeftHeld = false;
            }
            break;
        default:
            break;
        }

        InvokeCallbacks(Event.type, Event);
        InvokeCallbacks(SDL_USEREVENT, Event);
    }
}

void luvk_example::InputManager::InvokeCallbacks(const Uint32 Type, SDL_Event const& Event)
{
    if (auto It = m_Bindings.find(Type); It != m_Bindings.end())
    {
        for (auto const& CallbackIt : It->second)
        {
            CallbackIt(Event);
        }
    }
}

bool luvk_example::InputManager::Running() const noexcept
{
    return m_Running;
}

bool luvk_example::InputManager::IsKeyPressed(const SDL_Keycode key) const noexcept
{
    return m_PressedKeys.contains(key);
}

bool luvk_example::InputManager::LeftHeld() const noexcept
{
    return m_LeftHeld;
}
