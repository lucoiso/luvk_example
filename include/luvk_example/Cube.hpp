// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#pragma once

#include <memory>
#include <glm/glm.hpp>
#include <luvk/Core/MeshRegistry.hpp>
#include <luvk/Types/Mesh.hpp>

namespace luvk_example
{
    class Cube
    {
        std::shared_ptr<luvk::MeshRegistry> m_Registry{};
        std::size_t m_Index{};
        luvk::Mesh m_Mesh{};

    public:
        Cube(std::shared_ptr<luvk::MeshRegistry> Registry, std::size_t Index);
        void Update(float DeltaTime, glm::mat4 const& View, glm::mat4 const& Proj) const;
        [[nodiscard]] luvk::Mesh& GetMesh() noexcept;
    };
} // namespace luvk_example
