// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <imgui.h>
#include <memory>
#include <luvk/Types/Vector.hpp>
#include <volk/volk.h>

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
} // namespace luvk

namespace luvk_example
{
    class ImGuiMesh
    {
        std::shared_ptr<luvk::DescriptorPool> m_DescPool{};
        std::shared_ptr<luvk::DescriptorSet>  m_FontSet{};
        std::shared_ptr<luvk::Image>          m_FontImage{};
        std::shared_ptr<luvk::Sampler>        m_FontSampler{};
        std::shared_ptr<luvk::Pipeline>       m_Pipeline{};

        std::shared_ptr<luvk::Mesh> m_Mesh{};

        std::shared_ptr<luvk::Device>    m_Device{};
        std::shared_ptr<luvk::SwapChain> m_SwapChain{};
        std::shared_ptr<luvk::Memory>    m_Memory{};

        luvk::Vector<std::shared_ptr<luvk::Buffer>> m_VtxBuffers{};
        luvk::Vector<std::shared_ptr<luvk::Buffer>> m_IdxBuffers{};
        luvk::Vector<std::size_t>                   m_VtxBufferSizes{};
        luvk::Vector<std::size_t>                   m_IdxBufferSizes{};
        luvk::Vector<ImDrawVert>                    m_Vertices{};
        luvk::Vector<ImDrawIdx>                     m_Indices{};

    public:
        ImGuiMesh() = delete;
        explicit ImGuiMesh(const std::shared_ptr<luvk::Device>&         Device,
                           const std::shared_ptr<luvk::DescriptorPool>& Pool,
                           const std::shared_ptr<luvk::SwapChain>&      Swap,
                           const std::shared_ptr<luvk::Memory>&         Memory);

        void NewFrame() const;
        void Render(const VkCommandBuffer& Cmd, std::uint32_t CurrentFrame);
    };
} // namespace luvk_example
