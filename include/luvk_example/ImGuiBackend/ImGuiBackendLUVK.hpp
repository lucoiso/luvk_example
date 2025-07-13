// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <luvk/Core/Renderer.hpp>
#include <luvk/Core/Memory.hpp>
#include <luvk/Core/Image.hpp>
#include <luvk/Core/Sampler.hpp>
#include <luvk/Core/DescriptorPool.hpp>
#include <luvk/Core/DescriptorSet.hpp>
#include <luvk/Core/Pipeline.hpp>
#include <luvk/Core/Buffer.hpp>

namespace luvk_example
{
    class ImGuiBackendLUVK
    {
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
        void Render(std::shared_ptr<luvk::Memory> const& Memory, const VkCommandBuffer& Cmd);
    };
} // namespace luvk_example
