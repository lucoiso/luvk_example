// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Core/MeshRegistry.hpp>
#include <luvk/Types/Mesh.hpp>
#include <luvk/Core/Pipeline.hpp>
#include <luvk/Core/DescriptorSet.hpp>
#include <luvk/Core/DescriptorPool.hpp>
#include <luvk/Core/CommandPool.hpp>
#include <luvk/Core/Device.hpp>
#include <luvk/Core/SwapChain.hpp>

namespace luvk_example
{
    class Triangle
    {
        std::shared_ptr<luvk::MeshRegistry> m_Registry{};
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        std::shared_ptr<luvk::Pipeline> m_Compute{};
        std::shared_ptr<luvk::DescriptorSet> m_Descriptor{};
        std::shared_ptr<luvk::DescriptorPool> m_DescriptorPool{};
        VkCommandBuffer m_ComputeCmd{VK_NULL_HANDLE};
        VkQueue m_Queue{VK_NULL_HANDLE};
        std::size_t m_Index{};
        luvk::Mesh m_Mesh{};
        std::vector<luvk::MeshRegistry::InstanceInfo> m_Instances{};

    public:
        Triangle(std::shared_ptr<luvk::MeshRegistry> Registry,
                 const std::shared_ptr<luvk::Device>& Device,
                 const std::shared_ptr<luvk::SwapChain>& Swap,
                 const std::shared_ptr<luvk::CommandPool>& CmdPool);
        void Update(float dt);
        void AddInstance(glm::vec2 Position);
        [[nodiscard]] luvk::Mesh& GetMesh() noexcept;
    };
} // namespace luvk_example
