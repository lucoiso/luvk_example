// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Core/MeshRegistry.hpp>
#include <luvk/Types/Mesh.hpp>
#include <luvk/Core/Pipeline.hpp>
#include <luvk/Core/Buffer.hpp>
#include <luvk/Core/Device.hpp>
#include <luvk/Core/SwapChain.hpp>

namespace luvk_example
{
    class Cube
    {
        std::shared_ptr<luvk::MeshRegistry> m_Registry{};
        std::shared_ptr<luvk::Pipeline> m_Pipeline{};
        std::shared_ptr<luvk::Buffer> m_UBO{};
        std::size_t m_Index{};
        luvk::Mesh m_Mesh{};

    public:
        Cube(std::shared_ptr<luvk::MeshRegistry> Registry,
             std::shared_ptr<luvk::Device> Device,
             std::shared_ptr<luvk::SwapChain> Swap,
             std::shared_ptr<luvk::Memory> Memory);
        void Update(float DeltaTime, glm::mat4 const& View, glm::mat4 const& Proj) const;
        [[nodiscard]] luvk::Mesh& GetMesh() noexcept;
    };
} // namespace luvk_example
