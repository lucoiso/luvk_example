// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include "luvk_example/UserInterface/ImGuiBackendSDL2.hpp"
#include "luvk_example/UserInterface/ImGuiMesh.hpp"

#include <memory>
#include <SDL2/SDL.h>

namespace luvk_example
{
    class ImGuiLayer
    {
        ImGuiBackendSDL2 m_SdlBackend{};
        std::unique_ptr<ImGuiMesh> m_Mesh{};
        bool m_Initialized{false};

    public:
        constexpr ImGuiLayer() = default;

        bool Initialize(SDL_Window* Window,
                        std::shared_ptr<luvk::Renderer> const& Renderer,
                        std::shared_ptr<luvk::MeshRegistry> const& Registry,
                        std::shared_ptr<luvk::Device> const& Device,
                        std::shared_ptr<luvk::SwapChain> const& Swap,
                        std::shared_ptr<luvk::Memory> const& Memory);

        void Shutdown();
        void NewFrame(float DeltaTime) const;
        void Render(const VkCommandBuffer& Cmd);
        [[nodiscard]] bool ProcessEvent(const SDL_Event& Event) const;

        [[nodiscard]] constexpr bool IsInitialized() const noexcept
        {
            return m_Initialized;
        }
    };
} // namespace luvk_example
