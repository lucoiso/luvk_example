// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <glm/glm.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/MeshRegistry.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Types/Mesh.hpp>
#include <memory>

namespace luvk_example
{
    /** Pixel particle mesh */
    class Pixel
    {
        std::shared_ptr<luvk::MeshRegistry> m_Registry{};
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        std::size_t m_Index{};
        luvk::Mesh m_Mesh{};
        std::vector<luvk::MeshRegistry::InstanceInfo> m_Instances{};

    public:
        /** Constructor */
        Pixel(std::shared_ptr<luvk::MeshRegistry> Registry,
              const std::shared_ptr<luvk::Device>& Device,
              const std::shared_ptr<luvk::SwapChain>& Swap);

        /** Adds an instance */
        void AddInstance(glm::vec2 const& Position);

        /** Mesh handle for registry */
        [[nodiscard]] constexpr luvk::Mesh& GetMesh() noexcept;
    };
} // namespace luvk_example
