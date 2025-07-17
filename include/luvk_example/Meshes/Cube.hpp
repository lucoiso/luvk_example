// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <glm/glm.hpp>
#include <luvk/Resources/Buffer.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/MeshRegistry.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Types/Mesh.hpp>
#include <memory>

namespace luvk_example
{
    /** Simple cube mesh */
    class Cube
    {
        std::shared_ptr<luvk::MeshRegistry> m_Registry{};
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        std::shared_ptr<luvk::Buffer> m_UBO{};
        std::size_t m_Index{};
        luvk::Mesh m_Mesh{};

    public:
        /** Cube constructor */
        Cube(std::shared_ptr<luvk::MeshRegistry> Registry,
             const std::shared_ptr<luvk::Device>& Device,
             const std::shared_ptr<luvk::SwapChain>& Swap,
             const std::shared_ptr<luvk::Memory>& Memory);

        /** Updates uniforms */
        void Update(float DeltaTime, glm::mat4 const& View, glm::mat4 const& Proj) const;

        /** Mesh handle for registry */
        [[nodiscard]] constexpr luvk::Mesh& GetMesh() noexcept;
    };
} // namespace luvk_example
