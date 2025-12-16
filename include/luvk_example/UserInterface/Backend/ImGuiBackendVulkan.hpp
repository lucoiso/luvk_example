// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <imgui.h>
#include <memory>
#include <volk.h>
#include <luvk/Types/Vector.hpp>

namespace luvk
{
    class DescriptorPool;
    class Device;
    class Memory;
    class SwapChain;
    class DescriptorSet;
    class Image;
    class Pipeline;
    class Sampler;
    class Buffer;
    class Mesh;
    class Texture;
}

namespace luvk_example
{
    class ImGuiBackendVulkan
    {
        std::shared_ptr<luvk::DescriptorPool> m_DescPool{};
        std::shared_ptr<luvk::DescriptorSet>  m_FontSet{};
        std::shared_ptr<luvk::Texture>        m_FontTexture{};
        std::shared_ptr<luvk::Pipeline>       m_Pipeline{};

        std::shared_ptr<luvk::Mesh> m_Mesh{};

        std::shared_ptr<luvk::Device>    m_Device{};
        std::shared_ptr<luvk::SwapChain> m_SwapChain{};
        std::shared_ptr<luvk::Memory>    m_Memory{};

        luvk::Vector<ImDrawVert> m_Vertices{};
        luvk::Vector<ImDrawIdx>  m_Indices{};

    public:
        ImGuiBackendVulkan() = delete;
        explicit ImGuiBackendVulkan(const std::shared_ptr<luvk::Device>&         Device,
                                    const std::shared_ptr<luvk::DescriptorPool>& Pool,
                                    const std::shared_ptr<luvk::SwapChain>&      Swap,
                                    const std::shared_ptr<luvk::Memory>&         Memory);

        void NewFrame() const;
        void Render(const VkCommandBuffer& Cmd, std::uint32_t CurrentFrame);
    };
}
