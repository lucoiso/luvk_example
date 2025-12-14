// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <glm/glm.hpp>

namespace luvk_example
{
    class InputManager;

    class Camera
    {
        glm::vec3 m_Position{0.F, 0.F, 2.F};
        float     m_Speed{2.F};

    public:
        constexpr Camera() = default;

        void                    Update(float DeltaTime, const InputManager& Input);
        [[nodiscard]] glm::mat4 GetViewMatrix() const;
    };
} // namespace luvk_example
