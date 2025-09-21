// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <glm/glm.hpp>
#include "luvk_example/Core/InputManager.hpp"

namespace luvk_example
{
    class Camera
    {
        glm::vec3 m_Position{0.F, 0.F, 2.F};
        float m_Speed{2.F};

    public:
        constexpr Camera() = default;

        void Update(float DeltaTime, InputManager const& Input);
        [[nodiscard]] glm::mat4 GetViewMatrix() const;
    };
} // namespace luvk_example
