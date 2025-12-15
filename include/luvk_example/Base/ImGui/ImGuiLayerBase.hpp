// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <SDL3/SDL.h>
#include <volk/volk.h>
#include "luvk_example/Base/ImGui/ImGuiBackendSDL.hpp"
#include "luvk_example/Base/ImGui/ImGuiMesh.hpp"

namespace luvk
{
    class Device;
    class DescriptorPool;
    class SwapChain;
    class Memory;
    class CommandBuffer;
} // namespace luvk

namespace luvk_example
{
    class ImGuiLayerBase
    {
    protected:
        ImGuiBackendSDL            m_SdlBackend{};
        std::unique_ptr<ImGuiMesh> m_Mesh{};
        bool                       m_Initialized{false};

        std::shared_ptr<luvk::Device>         m_Device;
        std::shared_ptr<luvk::DescriptorPool> m_Pool;
        std::shared_ptr<luvk::Memory>         m_Memory;

    public:
        constexpr ImGuiLayerBase() = default;
        virtual   ~ImGuiLayerBase();

        bool Initialize(SDL_Window*                                  Window,
                        std::shared_ptr<luvk::Device> const&         Device,
                        std::shared_ptr<luvk::DescriptorPool> const& Pool,
                        std::shared_ptr<luvk::SwapChain> const&      Swap,
                        std::shared_ptr<luvk::Memory> const&         Memory);

        void               Shutdown();
        void               NewFrame(float DeltaTime) const;
        void               Render(const VkCommandBuffer& Cmd, std::uint32_t CurrentFrame) const;
        [[nodiscard]] bool ProcessEvent(const SDL_Event& Event) const;

        [[nodiscard]] constexpr bool IsInitialized() const noexcept
        {
            return m_Initialized;
        }

        virtual void Draw();
    };
} // namespace luvk_example
