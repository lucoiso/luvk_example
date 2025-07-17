// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include "luvk_example/Core/InputManager.hpp"
#include <glm/glm.hpp>

namespace luvk_example {
/** Basic camera for scene movement */
class Camera {
  glm::vec3 m_Position{0.F, 0.F, 2.F};
  float m_Speed{2.F};

public:
  /** Default constructor */
  constexpr Camera() = default;

  /** Updates the camera using the elapsed time */
  void Update(float DeltaTime, luvk_example::InputManager const &Input);

  /** Returns the view matrix */
  [[nodiscard]] glm::mat4 GetViewMatrix() const;
};
} // namespace luvk_example
