// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace luvk_example {
/** Responsavel por processar eventos de entrada */
class InputManager {
  using EventCallbacks = std::vector<std::function<void(SDL_Event const &)>>;

  std::unordered_map<Uint32, EventCallbacks> m_Bindings{};
  std::unordered_set<SDL_Keycode> m_PressedKeys{};
  bool m_Running{true};
  bool m_LeftHeld{false};

public:
  /** Construtor */
  InputManager();
  ~InputManager() = default;

  /** Associa um callback a um evento SDL */
  void BindEvent(Uint32 Type,
                 std::function<void(SDL_Event const &)> const &Callback);

  /** Processa a fila de eventos */
  void ProcessEvents();

  /** Verifica se a aplicacao ainda esta rodando */
  [[nodiscard]] constexpr bool Running() const noexcept { return m_Running; }

  /** Checa se uma tecla esta pressionada */
  [[nodiscard]] bool IsKeyPressed(SDL_Keycode key) const noexcept;

  /** Botao esquerdo do mouse esta pressionado */
  [[nodiscard]] constexpr bool LeftHeld() const noexcept { return m_LeftHeld; }

private:
  /** Executa os callbacks registrados para o tipo */
  void InvokeCallbacks(Uint32 Type, SDL_Event const &Event);
};
} // namespace luvk_example
