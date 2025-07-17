// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include "luvk_example/UserInterface/ImGuiBackendSDL2.hpp"
#include "luvk_example/UserInterface/ImGuiMesh.hpp"

#include <SDL2/SDL.h>
#include <memory>

namespace luvk_example {
/** Camada responsavel pela interface ImGui */
class ImGuiLayer {
  ImGuiBackendSDL2 m_SdlBackend{};
  std::unique_ptr<ImGuiMesh> m_Mesh{};
  bool m_Initialized{false};

public:
  /** Construtor */
  constexpr ImGuiLayer() = default;

  /** Inicializa os componentes da camada */
  bool Initialize(SDL_Window *Window,
                  std::shared_ptr<luvk::Renderer> const &Renderer,
                  std::shared_ptr<luvk::MeshRegistry> const &Registry,
                  std::shared_ptr<luvk::Device> const &Device,
                  std::shared_ptr<luvk::SwapChain> const &Swap,
                  std::shared_ptr<luvk::Memory> const &Memory);

  /** Finaliza e libera recursos */
  void Shutdown();

  /** Atualiza ImGui para um novo frame */
  void NewFrame(float DeltaTime) const;

  /** Renderiza os elementos de ImGui */
  void Render(const VkCommandBuffer &Cmd);

  /** Encaminha evento para o backend */
  bool ProcessEvent(SDL_Event const &Event) const;

  /** Indica se a camada foi inicializada */
  [[nodiscard]] constexpr bool IsInitialized() const noexcept {
    return m_Initialized;
  }
};
} // namespace luvk_example
