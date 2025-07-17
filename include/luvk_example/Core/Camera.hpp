// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include "luvk_example/Core/InputManager.hpp"
#include <glm/glm.hpp>

namespace luvk_example {
/** Camera basica para movimentacao na cena */
class Camera {
  glm::vec3 m_Position{0.F, 0.F, 2.F};
  float m_Speed{2.F};

public:
  /** Construtor padrao */
  constexpr Camera() = default;

  /** Atualiza a camera com o tempo decorrido */
  void Update(float DeltaTime, luvk_example::InputManager const &Input);

  /** Retorna a matriz de visualizacao */
  [[nodiscard]] glm::mat4 GetViewMatrix() const;
};
} // namespace luvk_example
