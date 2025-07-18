// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/MeshRegistry.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Types/Mesh.hpp>

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
        Pixel() = delete;
        explicit Pixel(std::shared_ptr<luvk::MeshRegistry> Registry,
                       const std::shared_ptr<luvk::Device>& Device,
                       const std::shared_ptr<luvk::SwapChain>& Swap);

        void AddInstance(glm::vec2 const& Position);

        [[nodiscard]] constexpr luvk::Mesh& GetMesh() noexcept
        {
            return m_Mesh;
        }
    };
} // namespace luvk_example
