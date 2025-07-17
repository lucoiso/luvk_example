// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <imgui.h>
#include <luvk/Resources/DescriptorSet.hpp>
#include <luvk/Resources/Image.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Resources/Sampler.hpp>
#include <luvk/Modules/DescriptorPool.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/Memory.hpp>
#include <luvk/Modules/MeshRegistry.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Types/Mesh.hpp>
#include <memory>

namespace luvk_example
{
    /** Mesh used to render ImGui through luvk */
    class ImGuiMesh
    {
        std::shared_ptr<luvk::MeshRegistry> m_Registry{};
        std::shared_ptr<luvk::DescriptorPool> m_DescPool{};
        std::shared_ptr<luvk::DescriptorSet> m_FontSet{};
        std::shared_ptr<luvk::Image> m_FontImage{};
        std::shared_ptr<luvk::Sampler> m_FontSampler{};
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        std::shared_ptr<luvk::Buffer> m_VtxBuffer{};
        std::shared_ptr<luvk::Buffer> m_IdxBuffer{};
        std::shared_ptr<luvk::Device> m_Device{};
        std::shared_ptr<luvk::SwapChain> m_SwapChain{};
        std::shared_ptr<luvk::Memory> m_Memory{};
        std::size_t m_Index{};
        luvk::Mesh m_Mesh{};
        std::size_t m_VtxBufferSize{0};
        std::size_t m_IdxBufferSize{0};
        std::vector<ImDrawVert> m_Vertices{};
        std::vector<ImDrawIdx> m_Indices{};

    public:
        /** Constructor */
        ImGuiMesh(std::shared_ptr<luvk::MeshRegistry> Registry,
                  std::shared_ptr<luvk::Device> Device,
                  std::shared_ptr<luvk::SwapChain> Swap,
                  std::shared_ptr<luvk::Memory> Memory);

        /** Prepares a new ImGui frame */
        void NewFrame() const;

        /** Executes rendering of the draw data */
        void Render(const VkCommandBuffer& Cmd);

        /** Resulting mesh for the renderer */
        [[nodiscard]] constexpr luvk::Mesh& GetMesh() noexcept;
    };
} // namespace luvk_example
