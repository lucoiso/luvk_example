// Author: Lucas Vilas-Boas
// Year: 2025
// Repo: https://github.com/lucoiso/luvk_example

#include "luvk_example/Cube.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

using namespace luvk_example;

luvk_example::Cube::Cube(std::shared_ptr<luvk::MeshRegistry> Registry, const std::size_t Index)
    : m_Registry(std::move(Registry)),
      m_Index(Index),
      m_Mesh(m_Registry, m_Index) {}

void luvk_example::Cube::Update(const float DeltaTime, glm::mat4 const& View, glm::mat4 const& Proj) const
{
    static float Elapsed = 0.F;
    Elapsed += DeltaTime;

    const glm::mat4 Model = glm::rotate(glm::mat4(1.F), Elapsed, glm::vec3(0.F, 1.F, 0.F));
    const glm::mat4 Mvp = Proj * View * Model;

    const glm::vec4 Color{0.5F + 0.5F * std::sin(Elapsed), 0.5F + 0.5F * std::sin(Elapsed + 2.094F), 0.5F + 0.5F * std::sin(Elapsed + 4.188F), 1.f};

    struct
    {
        glm::mat4 Mvp;
        glm::vec4 Color;
    } Pc{Mvp, Color};

    m_Registry->UpdateUniform(m_Index, std::as_bytes(std::span{&Pc, 1}));
}

luvk::Mesh& luvk_example::Cube::GetMesh() noexcept
{
    return m_Mesh;
}
