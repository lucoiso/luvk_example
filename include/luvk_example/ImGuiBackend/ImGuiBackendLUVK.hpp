// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <luvk/Core/Renderer.hpp>
#include <luvk/Core/Device.hpp>
#include <luvk/Core/CommandPool.hpp>
#include <luvk/Core/Memory.hpp>
#include <luvk/Core/Image.hpp>
#include <luvk/Core/Sampler.hpp>
#include <luvk/Core/DescriptorPool.hpp>
#include <luvk/Core/DescriptorSet.hpp>
#include <luvk/Core/Pipeline.hpp>
#include <luvk/Core/Buffer.hpp>
#include <luvk/Core/SwapChain.hpp>


namespace luvk_example
{
    class ImGuiBackendLUVK
    {
        std::shared_ptr<luvk::Renderer> m_Renderer{};
        std::shared_ptr<luvk::Device> m_Device{};
        std::shared_ptr<luvk::CommandPool> m_CommandPool{};
        std::shared_ptr<luvk::Memory> m_Memory{};
        std::shared_ptr<luvk::DescriptorPool> m_DescPool{};
        std::shared_ptr<luvk::DescriptorSet> m_FontSet{};
        std::shared_ptr<luvk::Image> m_FontImage{};
        std::shared_ptr<luvk::Sampler> m_FontSampler{};
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        std::shared_ptr<luvk::Buffer> m_VtxBuffer{};
        std::shared_ptr<luvk::Buffer> m_IdxBuffer{};
        std::size_t m_VtxBufferSize{0};
        std::size_t m_IdxBufferSize{0};

    public:
        ImGuiBackendLUVK() = default;

        bool Init(std::shared_ptr<luvk::Renderer> const& Renderer);
        void Shutdown();
        void NewFrame() const;
        void Render(VkCommandBuffer Cmd) const;
    };
} // namespace luvk_example
