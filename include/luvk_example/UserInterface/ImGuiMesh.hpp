// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <imgui.h>
#include <memory>
#include <luvk/Modules/DescriptorPool.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/Memory.hpp>
#include <luvk/Modules/MeshRegistry.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Image.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Resources/Sampler.hpp>
#include <luvk/Types/Mesh.hpp>

namespace luvk_example
{
    class ImGuiMesh
    {
        std::shared_ptr<luvk::DescriptorPool> m_DescPool{};
        std::shared_ptr<luvk::DescriptorSet> m_FontSet{};
        std::shared_ptr<luvk::Image> m_FontImage{};
        std::shared_ptr<luvk::Sampler> m_FontSampler{};
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        std::shared_ptr<luvk::Buffer> m_VtxBuffer{};
        std::shared_ptr<luvk::Buffer> m_IdxBuffer{};
        std::shared_ptr<luvk::MeshRegistry> m_Registry{};
        std::shared_ptr<luvk::Device> m_Device{};
        std::shared_ptr<luvk::SwapChain> m_SwapChain{};
        std::shared_ptr<luvk::Memory> m_Memory{};
        std::size_t m_Index{};
        std::shared_ptr<luvk::Mesh> m_Mesh{};
        std::size_t m_VtxBufferSize{0};
        std::size_t m_IdxBufferSize{0};
        std::vector<ImDrawVert> m_Vertices{};
        std::vector<ImDrawIdx> m_Indices{};

    public:
        ImGuiMesh() = delete;
        explicit ImGuiMesh(const std::shared_ptr<luvk::MeshRegistry>& Registry,
                           const std::shared_ptr<luvk::Device>& Device,
                           const std::shared_ptr<luvk::SwapChain>& Swap,
                           const std::shared_ptr<luvk::Memory>& Memory);

        void NewFrame() const;
        void Render(const VkCommandBuffer& Cmd);

        [[nodiscard]] constexpr const std::shared_ptr<luvk::Mesh>& GetMesh() const noexcept
        {
            return m_Mesh;
        }
    };
} // namespace luvk_example
