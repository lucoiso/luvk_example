// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Core/MeshRegistry.hpp>
#include <luvk/Core/Device.hpp>
#include <luvk/Core/SwapChain.hpp>
#include <luvk/Core/CommandPool.hpp>
#include <luvk/Core/Pipeline.hpp>
#include <luvk/Core/Descriptor.hpp>
#include <luvk/Types/Mesh.hpp>

namespace luvk_example
{
    class Triangle
    {
        std::shared_ptr<luvk::MeshRegistry> m_Registry{};
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        std::shared_ptr<luvk::Pipeline> m_ComputePipeline{};
        std::shared_ptr<luvk::DescriptorPool> m_CompPool{};
        std::shared_ptr<luvk::DescriptorSet> m_CompDesc{};
        std::shared_ptr<luvk::Device> m_Device{};
        std::shared_ptr<luvk::CommandPool> m_CommandPool{};
        std::size_t m_Index{};
        luvk::Mesh m_Mesh{};
        std::vector<luvk::MeshRegistry::InstanceInfo> m_Instances{};

    public:
        Triangle(std::shared_ptr<luvk::MeshRegistry> Registry,
                 std::shared_ptr<luvk::Device> Device,
                 std::shared_ptr<luvk::SwapChain> SwapChain,
                 std::shared_ptr<luvk::CommandPool> CmdPool);
        void Update(float DeltaTime);
        void AddInstance(glm::vec2 Position);
        [[nodiscard]] luvk::Mesh& GetMesh() noexcept;
    };
} // namespace luvk_example
