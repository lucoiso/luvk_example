// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace Core
{
    class InputManager;
}

namespace UserInterface
{
    class Camera
    {
        float                               m_Speed{2.F};
        glm::vec3                           m_Position{0.F, 0.F, 2.F};
        std::shared_ptr<Core::InputManager> m_Manager;

    public:
        Camera() = delete;
        explicit Camera(const std::shared_ptr<Core::InputManager>& Manager);

        void                    Update(float DeltaTime);
        [[nodiscard]] glm::mat4 GetViewMatrix() const;
    };
} // namespace UserInterface
