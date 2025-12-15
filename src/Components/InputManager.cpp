// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Components/InputManager.hpp"

using namespace luvk_example;

void InputManager::BindEvent(const Uint32                            Type,
                             std::function<void(const SDL_Event&)>&& Callback)
{
    auto [Iterator, Inserted] = m_Bindings.try_emplace(Type, EventCallbacks{});
    Iterator->second.emplace_back(std::move(Callback));
}

void InputManager::ProcessEvents()
{
    SDL_Event Event;

    while (SDL_PollEvent(&Event))
    {
        switch (Event.type)
        {
        case SDL_QUIT:
            {
                m_Running = false;
                break;
            }
        case SDL_KEYDOWN:
            {
                m_PressedKeys.insert(Event.key.keysym.sym);
                break;
            }
        case SDL_KEYUP:
            {
                m_PressedKeys.erase(Event.key.keysym.sym);
                break;
            }
        case SDL_MOUSEBUTTONDOWN:
            {
                if (Event.button.button == SDL_BUTTON_LEFT)
                {
                    m_LeftHeld = true;
                }
                break;
            }
        case SDL_MOUSEBUTTONUP:
            {
                if (Event.button.button == SDL_BUTTON_LEFT)
                {
                    m_LeftHeld = false;
                }
                break;
            }
        default: break;
        }

        InvokeCallbacks(Event.type, Event);
        InvokeCallbacks(SDL_USEREVENT, Event);
    }
}

void InputManager::InvokeCallbacks(const Uint32 Type, const SDL_Event& Event)
{
    if (const auto It = m_Bindings.find(Type);
        It != std::end(m_Bindings))
    {
        for (const auto& CallbackIt : It->second)
        {
            CallbackIt(Event);
        }
    }
}

bool InputManager::IsKeyPressed(const SDL_Keycode key) const noexcept
{
    return m_PressedKeys.contains(key);
}
