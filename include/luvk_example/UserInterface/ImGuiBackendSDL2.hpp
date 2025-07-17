// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <SDL2/SDL.h>
#include <array>
#include <imgui.h>

namespace luvk_example {
/** Backend SDL2 para integracao com ImGui */
class ImGuiBackendSDL2 {
  SDL_Window *m_Window{};
  std::array<SDL_Cursor *, ImGuiMouseCursor_COUNT> m_MouseCursors{};

public:
  /** Construtor */
  constexpr ImGuiBackendSDL2() = default;

  /** Inicializa com a janela SDL */
  bool Init(SDL_Window *Window);

  /** Libera recursos */
  void Shutdown();

  /** Inicia um novo frame */
  void NewFrame() const;

  /** Processa um evento SDL */
  bool ProcessEvent(SDL_Event const &Event) const;
};
} // namespace luvk_example
