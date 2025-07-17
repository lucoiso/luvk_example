// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include "luvk_example/UserInterface/ImGuiBackendSDL2.hpp"
#include "luvk_example/UserInterface/ImGuiMesh.hpp"

#include <SDL2/SDL.h>
#include <memory>

namespace luvk_example {
/** Layer that handles the ImGui interface */
class ImGuiLayer {
  ImGuiBackendSDL2 m_SdlBackend{};
  std::unique_ptr<ImGuiMesh> m_Mesh{};
  bool m_Initialized{false};

public:
  /** Constructor */
  constexpr ImGuiLayer() = default;

  /** Initializes layer components */
  bool Initialize(SDL_Window *Window,
                  std::shared_ptr<luvk::Renderer> const &Renderer,
                  std::shared_ptr<luvk::MeshRegistry> const &Registry,
                  std::shared_ptr<luvk::Device> const &Device,
                  std::shared_ptr<luvk::SwapChain> const &Swap,
                  std::shared_ptr<luvk::Memory> const &Memory);

  /** Shuts down and releases resources */
  void Shutdown();

  /** Updates ImGui for a new frame */
  void NewFrame(float DeltaTime) const;

  /** Renders ImGui elements */
  void Render(const VkCommandBuffer &Cmd);

  /** Forwards an event to the backend */
  bool ProcessEvent(SDL_Event const &Event) const;

  /** Indicates whether the layer has been initialized */
  [[nodiscard]] constexpr bool IsInitialized() const noexcept {
    return m_Initialized;
  }
};
} // namespace luvk_example
