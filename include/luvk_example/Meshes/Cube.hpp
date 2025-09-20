// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Modules/Device.hpp>
#include <luvk/Modules/MeshRegistry.hpp>
#include <luvk/Modules/SwapChain.hpp>
#include <luvk/Resources/Buffer.hpp>
#include <luvk/Resources/Pipeline.hpp>
#include <luvk/Types/Mesh.hpp>

namespace luvk_example
{
    class Cube
    {
        std::shared_ptr<luvk::MeshRegistry> m_Registry{};
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        std::shared_ptr<luvk::Buffer> m_UBO{};
        std::size_t m_Index{};
        std::shared_ptr<luvk::Mesh> m_Mesh{};

    public:
        Cube() = delete;
        explicit Cube(const std::shared_ptr<luvk::MeshRegistry>& Registry,
                      const std::shared_ptr<luvk::Device>& Device,
                      const std::shared_ptr<luvk::SwapChain>& Swap,
                      const std::shared_ptr<luvk::Memory>& Memory);

        void Update(float DeltaTime, glm::mat4 const& View, glm::mat4 const& Proj) const;

        [[nodiscard]] constexpr const std::shared_ptr<luvk::Mesh>& GetMesh() const noexcept
        {
            return m_Mesh;
        }
    };
} // namespace luvk_example
