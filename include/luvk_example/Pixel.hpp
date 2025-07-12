// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Core/MeshRegistry.hpp>
#include <luvk/Types/Mesh.hpp>
#include <luvk/Core/Pipeline.hpp>
#include <luvk/Core/Device.hpp>
#include <luvk/Core/SwapChain.hpp>

namespace luvk_example
{
    class Pixel
    {
        std::shared_ptr<luvk::MeshRegistry> m_Registry{};
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        std::size_t m_Index{};
        luvk::Mesh m_Mesh{};
        std::vector<luvk::MeshRegistry::InstanceInfo> m_Instances{};

    public:
        Pixel(std::shared_ptr<luvk::MeshRegistry> Registry,
              const std::shared_ptr<luvk::Device>& Device,
              const std::shared_ptr<luvk::SwapChain>& Swap);
        void AddInstance(glm::vec2 Position);
        [[nodiscard]] luvk::Mesh& GetMesh() noexcept;
    };
} // namespace luvk_example
