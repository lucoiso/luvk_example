// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/InputManager.hpp"

using namespace luvk_example;

luvk_example::InputManager::InputManager() = default;

void luvk_example::InputManager::BindEvent(const Uint32 Type, std::function<void(SDL_Event const&)> const& Callback)
{
    auto [Iterator, Inserted] = m_Bindings.try_emplace(Type, std::vector<std::function<void(SDL_Event const&)>>{});
    Iterator->second.push_back(Callback);
}

void luvk_example::InputManager::ProcessEvents()
{
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
        if (Event.type == SDL_QUIT)
        {
            m_Running = false;
        }
        else if (Event.type == SDL_KEYDOWN)
        {
            m_PressedKeys.insert(Event.key.keysym.sym);
        }
        else if (Event.type == SDL_KEYUP)
        {
            m_PressedKeys.erase(Event.key.keysym.sym);
        }
        else if (Event.type == SDL_MOUSEBUTTONDOWN && Event.button.button == SDL_BUTTON_LEFT)
        {
            m_LeftHeld = true;
        }
        else if (Event.type == SDL_MOUSEBUTTONUP && Event.button.button == SDL_BUTTON_LEFT)
        {
            m_LeftHeld = false;
        }

        if (auto Iterator = m_Bindings.find(Event.type);
            Iterator != std::end(m_Bindings))
        {
            for (auto& CallbackIt : Iterator->second)
            {
                CallbackIt(Event);
            }
        }

        if (auto Iterator = m_Bindings.find(SDL_USEREVENT);
            Iterator != std::end(m_Bindings))
        {
            for (auto& CallbackIt : Iterator->second)
            {
                CallbackIt(Event);
            }
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
