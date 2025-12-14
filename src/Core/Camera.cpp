// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Core/Camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "luvk_example/Core/InputManager.hpp"

using namespace luvk_example;

void Camera::Update(const float DeltaTime, const InputManager& Input)
{
    if (Input.IsKeyPressed(SDLK_LEFT))
    {
        m_Position.x -= m_Speed * DeltaTime;
    }

    if (Input.IsKeyPressed(SDLK_RIGHT))
    {
        m_Position.x += m_Speed * DeltaTime;
    }

    if (Input.IsKeyPressed(SDLK_UP))
    {
        m_Position.y += m_Speed * DeltaTime;
    }

    if (Input.IsKeyPressed(SDLK_DOWN))
    {
        m_Position.y -= m_Speed * DeltaTime;
    }
}

glm::mat4 Camera::GetViewMatrix() const
{
    const glm::vec3 Target{m_Position.x, m_Position.y, m_Position.z - 1.F};
    return glm::lookAt(m_Position, Target, glm::vec3(0.F, 1.F, 0.F));
}
